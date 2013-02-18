///////////////////////////////////////////////////////////////////////////////////
//
// C_Optimize.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sandia Corporation nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////
//
// DESCRIPTION:
//
//      class implementing the optimizer.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>

#ifdef LAPACK
#include <genClass/U_Lapack.h>
#else //LAPACK
#include <genClass/U_Matrix.h>
#endif // LAPACK

#include <genApp/DebugUtil.h>

#include <osClass/C_SimErr.h>

#include <osClass/DC_FitResults.h>
#include <osMain/G_Optimize.h>

#ifdef ADCONSOLEAPP
#include <genClass/C_MPISupport.h>
#include <genApp/MPIAppSupport.h>

//undefs required by mpich2 - kluge recommended by them!
#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include <mpi.h>
#endif


#include "OptSimplex.h"
#include "OptLevMar.h"
#include "OptSimAnneal.h"
#include "OptSCEM.h"

#include "C_Optimize.h"

#include "../nPre/CmdLine/RunControl.h"
#include "../nPre/Run/RunCommon.h"


Optimizer::Optimizer() :
    updateCB(0),
    currSimulator(0)
{
    useMPI = false;
    ResetTolerances();
}
Optimizer::~Optimizer()
{
}

void Optimizer::Optimize()
{
    GenAppInternalError("Optimizer::Optimize");
}


void  Optimizer::OptLimitError(      int    badPar,
                               const char*  charDesc,
                               const double& parVal)
{
    SC_SciFormat rForm(8);
    char rStr[40];
    rForm.RealToString(parVal, rStr, 40);

    char tempStr[80];
    MakeString(tempStr, varsToOptimize[badPar]->GetShortID(), ": ", charDesc, " out of limits ", rStr, 80);

    throw SimError(tempStr);
}


void  Optimizer::GetOptNormParEst(SC_DoubleArray& normParEstimates)
{
    for (int i = 0; i < noptVars; i++)
        normParEstimates[i] = varsToOptimize[i]->GetNormalizedEstimate();
}

void  Optimizer::GetOptParEst(SC_DoubleArray& parEstimates)
{
    for (int i = 0; i < noptVars; i++)
        parEstimates[i] = varsToOptimize[i]->GetParEstimate();
}


bool Optimizer::Initialize(UpdateCB       upCB,
                           RunnableC&  simulator)
{
    updateCB = upCB;
    currSimulator = &simulator;

    //  get current vars
    OptVar::GetActiveArray(varsToOptimize);
    noptVars = varsToOptimize.Size();

    try
    {
        //  runtime par estimates
        bestParEst.AllocAndSetSize(noptVars);
        bestParEst.SetNull();

        startNormParEst.AllocAndSetSize(noptVars);
        initNormParEst.AllocAndSetSize(noptVars);

        //  get start fit
        GetOptNormParEst(initNormParEst);

        // set best to start
        bestNormParEst = initNormParEst;

        runCount = 0;
        // set run parameters for multirun
        int maxRuns = IMax(noptVars + 1, 4);
        if (firstStageOptType == otSCEM)
            maxRuns = IMax(maxRuns, scem_m * scem_p);   //For opt SCEM only

#ifdef ADCONSOLEAPP
        useMPI = MPIAppSupport::MPIOptimizationRun();
        if (useMPI)
        {
            int maxnRanks = MPIAppSupport::MPIGetNmaxMPIRank();
            if (firstStageOptType == otSimplex)
                maxRuns = IMax(maxRuns, maxnRanks);   //For opt MPI Simplex case only
        }
#endif

        runPars.Alloc(maxRuns);
        runPars.SetSize(noptVars+1);

        for (int i = 0; i< maxRuns; i++)
            runPars[i].AllocAndSetSize(noptVars);

        int maxnFits = optimizeInput.GetMaxNFitComponents();  //
        multRunFitResults.AllocAndSetSize(maxnFits);

        for (int i = 0; i< maxnFits; i++)
            multRunFitResults[i].AllocAndSetSize(maxRuns);

        multFitHessianMatrix.CubeAllocAndSetSize(maxnFits, noptVars, noptVars);
        multRunFitChisq.AllocAndSetSize(maxRuns);

        OptAllocMatrix(optActualCovar);
        OptAllocMatrix(optEstimatedCovar);
        OptAllocMatrix(optDerivAlpha);
        OptAllocArray(optDerivBeta);

    }
    catch (SC_Array::AllocationError)
    {
        return false;
    }

    return true;
}


void Optimizer::ResetToInitial()
{
    for (int i = 0; i < noptVars; i++)
        varsToOptimize[i]->SetNormalizedEstimate(initNormParEst[i]);
}

void Optimizer::SetFinalParVals()
{
    if (!updateParameterEstimates)
        ResetToInitial();
}

void Optimizer::OutputBestFit()
{
    opMessage = "Running at best-fit ...";
    updateCB(*this);

    // set parameters
    for (int i = 0; i < noptVars; i++)
        varsToOptimize[i]->SetNormalizedEstimate(bestNormParEst[i]);

    // fatal setup error
    SC_SetupErr errData;
    if (!currSimulator->SetupForRun(errData))
        throw SimError(errData.GetErrorText());
    try
    {
        currSimulator->Execute();
        currSimulator->FinalUpdate();
    }
    catch (SimError& err)
    {
        if (err.errorLevel != SimError::seSemiFatal)
            // re throw
            throw err;
    }
    optCompleteOutput.CalcAndUpdate();  // best fit was last

#ifdef DEBUGACTIVE
    std::ostream&  dbout = GetDebugOutStream();
//  dbout << debugCaseIndex << ": " << processorNode << " **** opt results" << std::endl;
//  dbout << debugCaseIndex << ": " << runCount << "  " << std::scientific << std::setprecision(12) << bestFitVal << std::endl;
//  for (int i = 0; i < noptVars; i++)
//      dbout << debugCaseIndex << ": " << i << " " << bestNormParEst[i] << std::endl;
#endif
}

void Optimizer::OptStartCommon()
{
    ResetTolerances();
    //  initialize to avoid garbage in output
    okForOutput = false;
    optCalculationRun = false;
    optInitialDerivSpan = initialDerivSpan;  // overriden (possibly) in LM
}

void Optimizer::ResetTolerances()
{
    // reset tolerances
    currFitTolerance = 1000.0;
    currParameterTolerance = 1000.0;
    currErrorTolerance = 1000.0;
    toleranceMultiplier = 1.0;

}

void Optimizer::PerturbationSetup()
{
    int npertValues = nPerturb;
    if (!perturbMultiFitSameStart)
        npertValues *= optimizeInput.GetNFits();

    perturbValues.MatrixAllocAndSetSize(noptVars, npertValues);

    pertRand.SetupRandom(perturbRandomSeed);

    for (int i = 0; i < npertValues; i++)
        for (int j = 0; j < noptVars; j++)
        {
            perturbValues[j][i] = perturbSpan * (pertRand.Random() - 0.5);
        }

}


void Optimizer::FirstStageSetStart(int fitIndex, int caseIndex, int fitNumber)
{
   if (doOptPerturbations)
    {
        if (perturbFromStart)
            startNormParEst = initNormParEst;
        else
            startNormParEst = bestNormParEst;

        int pertVarIndex = caseIndex;
        if (!perturbMultiFitSameStart)
        {
            pertVarIndex *= optimizeInput.GetNFits();
            pertVarIndex += fitNumber;
        }

        for (int i = 0; i < noptVars; i++)
        {
            double delPar = perturbValues[i][pertVarIndex];
            startNormParEst[i] += delPar;
            if ((startNormParEst[i] > 1.0) ||(startNormParEst[i] < 0.0))
                startNormParEst[i] -= delPar + delPar;
        }

    }
   else {
        if (startMultipleFromOriginalEst)
            startNormParEst = initNormParEst;
        else
            startNormParEst = bestNormParEst;
   }

#ifdef DEBUGACTIVE
   debugCaseIndex = caseIndex;
#endif

    optimizeInput.currSelectedFit = fitIndex;
    runCount = 0;
    OptStartCommon();
    if (!singleStageOpt)
        toleranceMultiplier = transitionToleranceMultiplier;
    optProgressOutput.Setup();
}

void Optimizer::SecondStageSetStart(const Optimizer& firstStage)
{
    OptCopyArray(startNormParEst, firstStage.bestNormParEst);
    runCount = firstStage.runCount;
    OptStartCommon();
}


void Optimizer::OptErrorProcessing(SimError& err)
{
    if (okForOutput && (!err.IsUserAbort()))
    {
        // should be OK but catch/try anyway
        try
        {
            OutputBestFit();
        }
        catch (SimError&)
        {
            // ignore err
        }
    }

    // make sure original values get restored ...
    // removed at RR request - 04 Feb 02
//      ResetToInitial();

    // only reset if !update
    if (err.IsUserAbort() && updateParameterEstimates)
    {
        // only update if OK
        if (GenAppYesNoButtonDialog("Reset parameters to initial values?"))
            ResetToInitial();

    }
    else
        SetFinalParVals();

    // clear critical LM settings
    optCalculationRun = false;

    // and rethrow ...
    throw err;
}


bool Optimizer::ToleranceCheck(const double& currFit,
                               const double& prevFit,
                               const SC_DoubleArray& currParEstimates,
                               const SC_DoubleArray& prevParEstimates)
{
    currErrorTolerance = fabs(currFit - prevFit);

    double denom = fabs(currFit) + fabs(prevFit);
    if (denom < stdEps)
        currFitTolerance = stdEps;
    else
        currFitTolerance = 2.0 * currErrorTolerance / fabs(currFit + prevFit);

    currParameterTolerance = 0.0;
    for (int i = 0; i < noptVars; i++)
    {
        double deltaPar = fabs(currParEstimates[i] - prevParEstimates[i]);
        if (deltaPar > currParameterTolerance)
            currParameterTolerance = deltaPar;
    }
#ifdef DEBUGACTIVE
if (debugCaseIndex == 3)
{
    std::ostream&  dbout = GetDebugOutStream();
//  dbout  << " **** tolerance  " << std::endl;
//  dbout  << "opt " << optIterCount << " run " << runCount << std::endl;
//  dbout  << currErrorTolerance << " " << currFitTolerance << " " << currParameterTolerance << std::endl;
}
#endif

    bool fitTolOK = !combineToleranceWithOR;
    if (useFitTolerance)
        fitTolOK = currFitTolerance < (fitTolerance * toleranceMultiplier);

    bool parameterTolOK = !combineToleranceWithOR;
    if (useParameterTolerance)
        parameterTolOK = currParameterTolerance < (parameterTolerance * toleranceMultiplier);

/*  bool errorTolOK = !combineToleranceWithOR;
    if (useErrorTolerance)
        errorTolOK = currErrorTolerance < (errorTolerance * toleranceMultiplier);
removed Sep05 by KV request */

    if (combineToleranceWithOR)
        return fitTolOK || parameterTolOK;

    return fitTolOK && parameterTolOK;
}

bool Optimizer::NormParOK(const SC_DoubleArray& normParEstimates)
    // returns true if parameters are OK (0 - 1) and
    // not equal to the current best fit parameters
    // if not OK fitVal = worstFit * multiplier
    // if equal fitVal = bestFitVal
{
    double absDel = 0.0;
    bool isPrevBest = true;
    for (int i = 0; i < noptVars; i++)
    {
        double parEst = normParEstimates[i];
        if (parEst < 0.0)
            absDel += fabs(parEst);
        if (parEst > 1.0)
            absDel += parEst - 1.0;
        if (fabs(parEst - bestNormParEst[i]) > 1.0E-10)
            isPrevBest = false;
    }
    if ((isPrevBest) && (runCount > 0))
    {
        currFitVal = bestFitVal;
        return false;
    }

    // outside norm range ?
    if (absDel > 0.0)
    {
        currFitVal = worstFitVal * (1.0 +  10.0 * absDel);
        return false;
    }

    return true;
}

void  Optimizer::FinalCovarOnly(int fitIndex)
{
    startNormParEst = initNormParEst;
    bestNormParEst = initNormParEst;

    // need to set bestParEst
    for (int i = 0; i < noptVars; i++)
        varsToOptimize[i]->SetNormalizedEstimate(bestNormParEst[i]);
    GetOptParEst(bestParEst);


    optimizeInput.currSelectedFit = fitIndex;
    OptStartCommon();
    runCount = 0;

    try
    {
        optCalculationRun = true;

        opMessage = "Running at best-fit ...";
        updateCB(*this);

        if (!FinalCovarCalc())
            throw SimError("semi-fatal error during final partials calc");

        OutputBestFit();
    }
    catch (SimError& err)
    {
        // clear critical LM settings
        optCalculationRun = false;

        // make sure original values get restored ...
        ResetToInitial();

        // and rethrow ...
        throw err;
    }

    ResetToInitial();
}

bool Optimizer::CalcFitValue(const SC_DoubleArray& normParEstimates,
                                   double&         nextFit,
                                   bool            finalDerivCalc,
                                   bool            ignoreLimits)
{
    runCount++;
    if (runCount > maxOptRun)
    {
      throw SimError("maximum run count exceeded");
    }

    double minLim = 0.0;
    double maxLim = 1.0;

    if (finalDerivCalc)
    {
        minLim = -0.1;
        maxLim = 1.1;
    }
    // set parameters
    for (int i = 0; i < noptVars; i++)
    {
        if (!ignoreLimits)
            if ((normParEstimates[i] < minLim) || (normParEstimates[i] > maxLim))
                OptLimitError(i, "norm par", normParEstimates[i]);
        varsToOptimize[i]->SetNormalizedEstimate(normParEstimates[i]);
    }

    // fatal setup error
    SC_SetupErr errData;
    if (!currSimulator->SetupForRun(errData))
        throw SimError(errData.GetErrorText());

    optimizeInput.ClearCalcFlag();
    try
    {
        currSimulator->Execute();
        currSimulator->UpdateOutput();
    }
    catch (SimError& err)
    {
        if (err.errorLevel == SimError::seSemiFatal)
        {
            lastSemiFatal = err;
            return false;
        }

        // re throw
        throw err;
    }

    if (!optimizeInput.GetCalcFlag())
        throw SimError("fit calculation error. See List->Messages");

    nextFit = optimizeInput.GetFitValue();

#ifdef DEBUGACTIVE
if (debugCaseIndex == 3)
{
    std::ostream&  dbout = GetDebugOutStream();
//  dbout << "fit calc" << runCount << "  " << nextFit << " " << normParEstimates[0] << normParEstimates[1] << std::endl;
//  for (int i = 0; i < noptVars; i++)
//      dbout << debugCaseIndex << ": " << i << " " << bestNormParEst[i] << std::endl;
}
#endif



    return true;
}



void Optimizer::OptAllocArray(SC_DoubleArray&  arr) const
{
    arr.AllocAndSetSize(noptVars);
}
void  Optimizer::OptClearArray(SC_DoubleArray&  arr) const
{
    for (int i = 0; i < noptVars; i++)
        arr[i] = 0.0;
}
void  Optimizer::OptCopyArray(      SC_DoubleArray&  dest,
                             const SC_DoubleArray&  src) const
{
    for (int i = 0; i < noptVars; i++)
        dest[i] = src[i];
}


void Optimizer::OptAllocMatrix(SC_DoubleMatrix&  mat) const
{
    mat.MatrixAllocAndSetSize(noptVars, noptVars);
}

void Optimizer::OptClearMatrix(SC_DoubleMatrix&  mat) const
{
    for (int i = 0; i < noptVars; i++)
        OptClearArray(mat[i]);
}

void  Optimizer::OptCopyMatrix(      SC_DoubleMatrix&  dest,
                              const SC_DoubleMatrix&  src) const
{
    for (int i = 0; i < noptVars; i++)
        OptCopyArray(dest[i], src[i]);
}

int Optimizer::GetnBaseOutput()
{
    return 3;
}
void Optimizer::GetBaseOutputID(SC_StringArray& baseIDs) const
{
    baseIDs.Alloc(3);
    baseIDs += "FitToler";
    baseIDs += "ParToler";
    baseIDs += "ErrToler";
}


void Optimizer::GetBaseOutput(SC_DoubleArray& baseData) const
{
    baseData.Alloc(3);
    baseData += currFitTolerance;
    baseData += currParameterTolerance;
    baseData += currErrorTolerance;
}

int Optimizer::GetnExtraOutput() const
{
    return 0;
}
void Optimizer::GetExtraOutputID(SC_StringArray& extraIDs) const
{
    extraIDs.SetEmpty();
}
void Optimizer::GetExtraOutput(SC_DoubleArray& extraData) const
{
    extraData.SetEmpty();
}


bool  Optimizer::OptDoDerivRun(const SC_DoubleArray& normParEstimates,
                                    int             iIndex,
                                    int             jIndex,
                              const int             numResStart,
                                    bool            finalDerivCalc,
                                    bool            ignoreLimits)
{
    SC_DoubleArray currParEst(normParEstimates);

    if (iIndex < 0)
    {
        currParEst[jIndex] -= optDerivSpan;
        if (currParEst[jIndex] < 0.0)
            return false;
    }
    else if (iIndex == jIndex)
    {
        currParEst[jIndex] += optDerivSpan;
    }
    else
    {
        currParEst[iIndex] += optDerivSpan;
        currParEst[jIndex] += optDerivSpan;
    }

//MX    optHessianI = iIndex;
//MX    optHessianJ = jIndex;

    double dummy;                       // don't care about actual fit ....
    //do care that have same number of residuals for each fit before and after
    bool fitOK = CalcFitValue(currParEst, dummy, finalDerivCalc, ignoreLimits);
    int numResAfter = GetNumResiduals(0);
    if (numResAfter != numResStart)
        throw SimError("Difference in # of residuals - decrease span");

    updateCB(*this);

    return fitOK;
}

int Optimizer::GetNumResiduals(int runNumber)
{
    int nRes = 0;

    for (int i = 0; i < multRunFitResults.Size(); i++)
    {
        DC_FitResults&   currFit = multRunFitResults[i][runNumber];
        nRes += currFit.fitResiduals.Size();
    }

    return nRes;
}

bool  Optimizer::OptHalfDerivRun(RunParametersArray& runPars,
                                 const int             numRes,
                                       bool            finalDerivCalc,
                                       bool            ignoreLimits)

{
    bool checkRun0Fit = true;
    bool run0FitDeteriorated = true;
    double dummy = 0.0;

    return OptimizerRunMultiple(runPars, true, 1, finalDerivCalc, checkRun0Fit, dummy, run0FitDeteriorated, ignoreLimits);
}

void Optimizer::SetMultirunDerivCalcParametersArray(const SC_DoubleArray& normParEstimates)
{
     for (int i = 0; i < noptVars+1; i++)
     {
        SC_DoubleArray currParEst(normParEstimates);
        if (i==0)
            runPars[i] = currParEst;
        else
        {
            currParEst[i-1] += optDerivSpan;
            runPars[i] = currParEst;
        }
     }

}



void Optimizer::OptSetBottomTri(SC_DoubleMatrix&   alpha)
{
    // LMCalcFull & CalcHalf fill in top tri of array
    // this fills in bottom

    int nFit = optimizeInput.GetNCurrentFits();
    for (int j = 1; j < noptVars; j++)
        for (int k = 0; k < j; k++)
        {
            alpha[j][k] = alpha[k][j];
            for (int i = 0; i < nFit; i++ )
                multFitHessianMatrix[i][j][k] = multFitHessianMatrix[i][k][j];
        }
}

//ToDo: this function is not working yet.
void Optimizer::OptCalcFullDeriv(SC_DoubleMatrix&   alpha,
                                SC_DoubleArray&    beta)
{
    double da2  = optDerivSpan + optDerivSpan;
    double dasq = Sqr(optDerivSpan);

//  clear input ??
    OptClearMatrix(alpha);
    OptClearArray(beta);

    SC_DoubleArray dyda(noptVars);
    SC_DoubleArray dy2da(noptVars);

    for (int ii = 0; ii < optimizeInput.GetNCurrentFits(); ii++ )
    {
        DC_FitResults&   currFit = multRunFitResults[ii][0];
//MX        DC_FitResults& currFit = *(currFits[ii]);
//MX        SC_DoubleCube& fitDeriv = currFit.fitDerivatives;
        SC_DoubleArray& fitDeriv = currFit.fitDerivatives;

        //
        SC_DoubleMatrix& fitHessian = multFitHessianMatrix[ii];
        OptClearMatrix(fitHessian);

        double sig2i = 1.0 / currFit.sigma;

        for (int i = 0; i < currFit.fitResults.Size(); i++)
        {
            double y = currFit.fitResults[i];
            double dy = currFit.fitResiduals.yData[i];  // residual at point

            for (int j = 0; j < noptVars; j++)
            {
                double dpa = multRunFitResults[ii][j+1].fitDerivatives[i];   //fitDeriv[j][j][i];
//MX???                double dma = currFit.fitDerivDiagonal[j][i];
//MX???                dyda[j] = (dpa - dma) / da2;
//MX???                dy2da[j] = (dpa - 2.0 * y + dma) / dasq;
            }

            for (int k = 0; k < noptVars; k++)
            {
                double dydak = dyda[k];
                for (int l = k; l < noptVars; l++)
                {
                    double dydal = dyda[l];
                    double d2ydakl;
                    if (l == k)
                        d2ydakl = dy2da[l];
                    else l=k;
//MX???                        d2ydakl = (fitDeriv[k][l][i] + y - fitDeriv[k][k][i] - fitDeriv[l][l][i]) / dasq;

                    double term = dydak * dydal - dy * d2ydakl;
                    alpha[k][l] += sig2i * term;
                    fitHessian[k][l] += term;
                }
                beta[k] += dy * sig2i * dydak;
            }

        }
    }

//  routine fills in top half - set bottom
    OptSetBottomTri(alpha);

}


void Optimizer::OptCalcHalfDeriv(SC_DoubleMatrix&   alpha,
                                SC_DoubleArray&    beta)
{
        SC_DoubleArray dyda(noptVars);

    //  clear input
        OptClearMatrix(alpha);
        OptClearArray(beta);

        for (int ii = 0; ii < optimizeInput.GetNCurrentFits(); ii++ )
        {
            DC_FitResults&   currFit = multRunFitResults[ii][0];
            SC_DoubleArray&   fitDeriv = currFit.fitDerivatives;
            SC_DoubleMatrix& fitHessian = multFitHessianMatrix[ii];
            OptClearMatrix(fitHessian);

            double sig2i = 1.0 / currFit.sigma;

            for (int i = 0; i < currFit.fitResults.Size(); i++)
            {
                double y = currFit.fitResults[i];
                double dy = currFit.fitResiduals.yData[i];  // residual at point

                // modified from Pascal -- fills top half, requires extra loop but consistent with FullCalc
                int j;

                // set dyda
                for (j = 0; j < noptVars; j++)
                {
                    double dVal = multRunFitResults[ii][j+1].fitDerivatives[i];
                    dyda[j] = (dVal - y) / optDerivSpan;
//                  dyda[j] = dVal / optDerivSpan - y / optDerivSpan;
                }

                // fill matrices
                for (j = 0; j < noptVars; j++)
                {
                    double wt = dyda[j] * sig2i;

                    for (int k = j; k < noptVars; k++)
                    {
                        alpha[j][k] += wt * dyda[k];
                        fitHessian[j][k] += dyda[j] * dyda[k];

                    }
                    beta[j] += dy * wt;

                }
            }
        }

    //  routine fills in top half - set bottom
        OptSetBottomTri(alpha);

}


double Optimizer::OptDoDaCalcRun(const SC_DoubleArray& normParEstimates,
                                  int             pIndex,
                            const double&         parStep)
{

    SC_DoubleArray currParEst(normParEstimates);
    currParEst[pIndex] += parStep;

    double currChisq;
    if (!CalcFitValue(currParEst, currChisq, true))
        throw SimError("error during derivative span calc");

    SC_SciFormat daForm(4);
    char daStr[40];
    daForm.RealToString(parStep, daStr, 40);

    char errStr[80];
    MakeString(errStr, "Calculating derivative span. dA = ", daStr, 80);

    opMessage = errStr;
    updateCB(*this);

    return currChisq;
}

void Optimizer::SetMultiRunOptDoDaCalcRunParameter(const SC_DoubleArray& normParEstimates,
                                  int             pIndex,
                            const double&         parStep,
                                int               fIndex)
{

    SC_DoubleArray currParEst(normParEstimates);
    for (int i = 0; i < fIndex; i++){
        switch (i) {
            case 0: {
                currParEst[pIndex] -= parStep;
                runPars[i] = currParEst;
                currParEst[pIndex] += parStep;
                break;
            }
            case 1: {
                currParEst[pIndex] += parStep;
                runPars[i] = currParEst;
                currParEst[pIndex] -= parStep;
                break;
            }
            case 2: {
                currParEst[pIndex] -= parStep * 2.0;
                runPars[i] = currParEst;
                currParEst[pIndex] += parStep * 2.0;
                break;
            }
            case 3: {
                currParEst[pIndex] += parStep * 2.0;
                runPars[i] = currParEst;
                currParEst[pIndex] -= parStep * 2.0;
                break;
            }
        }
    }
}
// from somewhat obscure undocumented Janis White algorithm
// was bool -- however can never return false (errors are thrown)


void Optimizer::OptSetDerivSpan(const SC_DoubleArray& pars,
                                  int             parIndex)
{
    static const int maxIter = 25;

    optDerivSpan = fabs(pars[parIndex] * optInitialDerivSpan);

    double minSpan = sqrt(derivativeTolerance);
    if (optDerivSpan < minSpan)
        optDerivSpan = minSpan;

    int currentNPars = runPars.Size();

    runPars.SetSize(4);

    derivSpanIndex = 0;
    SetMultiRunOptDoDaCalcRunParameter(pars, parIndex, optDerivSpan, 4);
    bool finalDerivCalc = true;
    bool run0FitDeteriorated = false;
    double dummy = multRunFitChisq[parIndex];

    OptimizerRunMultiple(runPars, false, 0, finalDerivCalc, false, dummy, run0FitDeteriorated);

    double fOfX1 = multRunFitChisq[0];
    double fOfX2 = multRunFitChisq[1];
    double fOfXX1 = multRunFitChisq[2];
    double fOfXX2 = multRunFitChisq[3];

    double oldEstimate =  (fabs(fOfX2 - fOfX1) / 1.5 + fabs(fOfXX1 - fOfXX2) / 12.0) / optDerivSpan;
    derivSpanResult = oldEstimate;

    double minDa;

    for ( int i = 0; i < maxIter; i++)
    {
        fOfXX1 = fOfX1;
        fOfXX2 = fOfX2;

        optDerivSpan = optDerivSpan / 2.0;
        derivSpanIndex = i;

        SetMultiRunOptDoDaCalcRunParameter(pars, parIndex, optDerivSpan, 2);
        bool finalDerivCalc = true;
        if (i==0)
            runPars.SetSize(2);
        OptimizerRunMultiple(runPars, false, 0, finalDerivCalc, false, dummy, finalDerivCalc);

        fOfX1 = multRunFitChisq[0];
        fOfX2 = multRunFitChisq[1];

        double newEstimate =  (fabs(fOfX2 - fOfX1) / 1.5 + fabs(fOfXX1 - fOfXX2) / 12.0) / optDerivSpan;

        derivSpanResult = fabs(oldEstimate - newEstimate) / 15.0;

        if ((i == 0) || (derivSpanResult < minDa))
            minDa = derivSpanResult;

        derivSpanCriteria = derivativeTolerance;
        if (fabs(oldEstimate) > 1.0)
            derivSpanCriteria *= fabs(oldEstimate);

        if (derivSpanResult <= derivSpanCriteria)
            return;

        oldEstimate = newEstimate;

    }

    SC_SciFormat daForm(4);
    char daStr[40];
    daForm.RealToString(minDa, daStr, 40);

    char errStr[80];
    MakeString(errStr, "no convergence on derivative span calc. Min attained: ", daStr, 80);

    throw SimError(errStr);

    runPars.SetSize(currentNPars);
}


void Optimizer::ParSensitivityCheck(const SC_DoubleMatrix& alpha)
{
    // check for zero diagonal
    for (int i = 0; i < noptVars; i++)
        if (fabs(alpha[i][i]) < 1.0E-99)
        {
            char errStr[132];
            MakeString(errStr, "Not sensitive to ", varsToOptimize[i]->GetShortID(), 132);
            throw SimError(errStr);
        }
}

void Optimizer::Invert(SC_DoubleMatrix&   a,
                        int                n)
{
    ParSensitivityCheck(a);
    try
    {
#ifdef LAPACK
        LAPACKInvert(a, n);
#else // LAPACK
        matrixSolvers::Invert(a, n);
#endif // LAPACK
    }
    catch (MathError& err)
    {
        throw SimError(err.errStr);
    }
}


bool Optimizer::FinalCovarCalc()
{
    optCalculationRun = true;

    //  need to rerun to set DFO_SingleFit vars
    int numRuns = noptVars + 1;
    runPars[0] = bestNormParEst;
    runPars.SetSize(1);
    double dummy;
    bool run0FitDeteriorated = false;
    if (!OptimizerRunMultiple(runPars, true, 0, true, false, dummy, run0FitDeteriorated,  false))
        return false;
    runPars.SetSize(numRuns);

    int numRes = GetNumResiduals(0);  //needed to check if number of residuals changes btwn
                                    // runs - ensures correct derivative calc

    if (useFixedDerivSpan)
    {
        optDerivSpan = fixedDerivSpan;
    }
    else
    {
        optDerivSpan = optInitialDerivSpan;

        // get initial covar to determine most sensitive par -- may also be redundant for LM
        opMessage = "Generating initial 1/2 partials for derivative span ...";

        SetMultirunDerivCalcParametersArray(bestNormParEst);

        int runOffset = 1;
        if (!OptimizerRunMultiple(runPars, false, runOffset, false, false, dummy, run0FitDeteriorated,  false))
            return false;

        //  derivative calc for max check
        OptCalcHalfDeriv(optDerivAlpha, optDerivBeta);
        Invert(optDerivAlpha, noptVars);
        int maxIndex = 0;
        for (int j = 1; j < noptVars; j++)
            if (optDerivAlpha[j][j] > optDerivAlpha[maxIndex][maxIndex])
                maxIndex = j;

        opMessage = "Adjusting derivative span ...";
        OptSetDerivSpan(bestNormParEst, maxIndex);

        //  need to rerun to set DFO_SingleFit vars
        runPars[0] = bestNormParEst;
        runPars.SetSize(1);
        double dummy;
        bool run0FitDeteriorated = false;
        if (!OptimizerRunMultiple(runPars, true, 0, true, false, dummy, run0FitDeteriorated,  false))
            return false;
        runPars.SetSize(numRuns);

    }

    // do final derivative runs
    opMessage = "Final derivative runs ...";
    runPars.SetSize(noptVars+1);
    SetMultirunDerivCalcParametersArray(bestNormParEst);

    if (!OptimizerRunMultiple(runPars, false, 0, true, false, dummy, run0FitDeteriorated,  false))
        return false;

    if (use2ndOrderCovar)
    {
        for (int i = 0; i < noptVars; i++)
            if (!OptDoDerivRun(bestNormParEst, -1, i, numRes, true))
                return false;

        for (int i = 0; i < noptVars; i++)
            for (int j = i + 1; j < noptVars; j++)
                if (!OptDoDerivRun(bestNormParEst, i, j, numRes, true))
                    return false;

        OptCalcFullDeriv(optDerivAlpha, optDerivBeta);
    }
    else
        OptCalcHalfDeriv(optDerivAlpha, optDerivBeta);

    OptCopyMatrix(optActualCovar, optDerivAlpha);
    Invert(optActualCovar, noptVars);

    OptClearMatrix(optEstimatedCovar);
    int numFits = optimizeInput.GetNCurrentFits();  //.GetNFits();
    int nFull = 0;
    for (int i = 0; i < numFits; i++ )
    {
        DC_FitResults& currFit = multRunFitResults[i][0];
        int nCurr = currFit.fitResults.Size();
        currFit.nfitPoints = nCurr;

        //  calculate estimated variance
        currFit.estVar = currFit.sse / (nCurr - noptVars);
        nFull += nCurr;

        double optEstimatedCov;
        double estV;
        for (int j = 0; j < noptVars; j++)
            for (int k = 0; k < noptVars; k++){
                optEstimatedCovar[j][k] += multFitHessianMatrix[i][j][k] / currFit.estVar;
                optEstimatedCov = optEstimatedCovar[j][k];  //for debug only
                estV = currFit.estVar;
            }
    }
    Invert(optEstimatedCovar, noptVars);


    //  fit value should be full sse if compound fit, single sse if single fit
    //    lmFitEstimatedVar = optimizeInput.GetFitValue() / double (nFull - currFits.Size() * noptVars);
    //  was calculated - not used anywhere ???

    // invert each hessian for estimated fit covariance
    for (int i = 0; i < numFits; i++ )
    {
        DC_FitResults& currFit = multRunFitResults[i][0];
        SC_DoubleMatrix& fitHessian = multFitHessianMatrix[i];
        double sig = 1.0 / currFit.estVar;

        for (int j = 0; j < noptVars; j++)
            for (int k = 0; k < noptVars; k++)
                fitHessian[j][k] *= sig;

        Invert(fitHessian, noptVars);
    }

    return true;
}


void Optimizer::OptFinalUpdate()
{
    currSimulator->FinalUpdate();
}

// optimizer MPI stuff here ....

#ifdef ADCONSOLEAPP
extern const int mpiTag_FitIndex            = 101;
const int mpiTag_NPars              = 102;
const int mpiTag_ParVals            = 103;
const int mpiTag_FinalDerivCalc     = 104;
const int mpiTag_IgnoreLimits       = 105;

//Send back results
const int mpiTag_FitOK              = 106;
const int mpiTag_ChiSq              = 107;

const int mpiTag_Nfits              = 108;
const int mpiTag_nfitPoints         = 109;
const int mpiTag_fitVal             = 110;
const int mpiTag_sse                = 111;

const int mpiTag_sigma              = 112;
const int mpiTag_estVar             = 113;
const int mpiTag_fitWasL1           = 114;
const int mpiTag_fitWasNormed       = 115;

const int mpiTag_fitResults         = 116;
const int mpiTag_fitResidualsX      = 117;
const int mpiTag_fitResidualsY      = 118;
const int mpiTag_fitDerivatives     = 119;
const int mpiTag_fitResDerivatives  = 120;
const int mpiTag_NextRun            = 121;
const int mpiTag_RunComplete        = 122;



bool Optimizer::MPISlaveRunCalculations(int slaveRank, const int fitIndex)
{
    optimizeInput.currSelectedFit = fitIndex;

    FitResultsPtrArray&  currFits = optimizeInput.GetCurrentFitResults();
    int nFits = currFits.Size();

    int nPars;
    MPI_Status status;
    while (true)
    {
        int runIndx;
        MPIAppSupport::CheckMPI(MPI_Recv(&runIndx, 1, MPI_INT, 0, mpiTag_NextRun, MPI_COMM_WORLD, &status), "Send case index to slave");

        // all done indicator ?
        if (runIndx < 0)
            return false;

        // get number of parameters from master
        MPIAppSupport::CheckMPI(MPI_Recv(&nPars, 1, MPI_INT, 0, mpiTag_NPars, MPI_COMM_WORLD, &status), "Slave get nPars");

        // all done indicator ?
        if (nPars < 0)
            break;

        if (nPars != noptVars && (nPars != noptVars +1))
            GenAppInternalError("Optimizer::MPISlaveRunCalculations - opt mismatch");

        // get parameters from master
        MPIAppSupport::CheckMPI(MPI_Recv(runPars[runIndx].tListData, noptVars, MPI_DOUBLE, 0, mpiTag_ParVals, MPI_COMM_WORLD, &status), "Slave get par vals");

        int finalDerivCalc;
        MPIAppSupport::CheckMPI(MPI_Recv(&finalDerivCalc, 1, MPI_INT, 0, mpiTag_FinalDerivCalc, MPI_COMM_WORLD, &status), "Slave get final Deriv");

        int ignoreLimits;
        MPIAppSupport::CheckMPI(MPI_Recv(&ignoreLimits, 1, MPI_INT, 0, mpiTag_IgnoreLimits, MPI_COMM_WORLD, &status), "Slave get ignore limits");

        double chiSqVal;
        int fitOK = int(CalcFitValue(runPars[runIndx], chiSqVal, finalDerivCalc != 0, ignoreLimits != 0));
        if (!fitOK)
            continue;

        MPIAppSupport::CheckMPI(MPI_Send(&fitOK, 1, MPI_INT, 0, mpiTag_FitOK, MPI_COMM_WORLD), "Slave send fitOK");
        MPIAppSupport::CheckMPI(MPI_Send(&runIndx, 1, MPI_INT, 0, mpiTag_NextRun, MPI_COMM_WORLD), "Slave send runIndx");


        //Send results back

        MPIAppSupport::CheckMPI(MPI_Send(&chiSqVal, 1, MPI_DOUBLE, 0, mpiTag_ChiSq, MPI_COMM_WORLD), "Slave send chisq");

//      if (nFits != fitIndex)
//          GenAppInternalError("Optimizer::MPISlaveRunCalculations - opt mismatch");

        MPIAppSupport::CheckMPI(MPI_Send(&nFits, 1, MPI_INT, 0, mpiTag_Nfits, MPI_COMM_WORLD), "Slave send nFits");

        for (int i = 0; i < nFits; i++)
        {
            const DC_FitResults& currDC = *(currFits[i]);

            int nfitPoints = currDC.nfitPoints;
            MPIAppSupport::CheckMPI(MPI_Send(&nfitPoints, 1, MPI_INT, 0, mpiTag_nfitPoints, MPI_COMM_WORLD), "Slave send nfitPoints");

            double fitValue = currDC.fitValue;
            MPIAppSupport::CheckMPI(MPI_Send(&fitValue, 1, MPI_DOUBLE, 0, mpiTag_fitVal, MPI_COMM_WORLD), "Slave send fitValue");
            double sse = currDC.sse;
            MPIAppSupport::CheckMPI(MPI_Send(&sse, 1, MPI_DOUBLE, 0, mpiTag_sse, MPI_COMM_WORLD), "Slave send sse");
            double sigma = currDC.sigma;
            MPIAppSupport::CheckMPI(MPI_Send(&sigma, 1, MPI_DOUBLE, 0, mpiTag_sigma, MPI_COMM_WORLD), "Slave send sigma");
            double estVar = currDC.estVar;
            MPIAppSupport::CheckMPI(MPI_Send(&estVar, 1, MPI_DOUBLE, 0, mpiTag_estVar, MPI_COMM_WORLD), "Slave send estVar");
            int fitWasL = int(currDC.fitWasL1);
            MPIAppSupport::CheckMPI(MPI_Send(&fitWasL, 1, MPI_INT, 0, mpiTag_fitWasL1, MPI_COMM_WORLD), "Slave send fitWasL1");
            int fitWasNormed1 = int(currDC.fitWasNormed);
            MPIAppSupport::CheckMPI(MPI_Send(&fitWasNormed1, 1, MPI_INT, 0, mpiTag_fitWasNormed, MPI_COMM_WORLD), "Slave send fitWasNormed");

            MPIAppSupport::CheckMPI(MPI_Send(currDC.fitResults.tListData, nfitPoints, MPI_DOUBLE, 0, mpiTag_fitResults, MPI_COMM_WORLD), "Slave send fitResults");
            MPIAppSupport::CheckMPI(MPI_Send(currDC.fitResiduals.xData.tListData, nfitPoints, MPI_DOUBLE, 0, mpiTag_fitResidualsX, MPI_COMM_WORLD), "Slave send fitResiduals X");
            MPIAppSupport::CheckMPI(MPI_Send(currDC.fitResiduals.yData.tListData, nfitPoints, MPI_DOUBLE, 0, mpiTag_fitResidualsY, MPI_COMM_WORLD), "Slave send fitResiduals Y");
            MPIAppSupport::CheckMPI(MPI_Send(currDC.fitDerivatives.tListData, nfitPoints, MPI_DOUBLE, 0, mpiTag_fitDerivatives, MPI_COMM_WORLD), "Slave send fitDerivatives");
            MPIAppSupport::CheckMPI(MPI_Send(currDC.fitResidualDerivatives.tListData, nfitPoints, MPI_DOUBLE, 0, mpiTag_fitResDerivatives, MPI_COMM_WORLD), "Slave send fitResidualDerivatives");

        }
        return true;
    }
    return false;
}


void Optimizer::MPIRunSlave(int slaveRank, int fitIndex)
{
    optCalculationRun = true;

    while (true)
    {
        //Slave calculating
        if (!MPISlaveRunCalculations(slaveRank, fitIndex))
            return;

        //Slave send message back to the master that run is complete
        int complete = 1;
        MPIAppSupport::CheckMPI(MPI_Send(&complete, 1, MPI_INT, 0, mpiTag_RunComplete, MPI_COMM_WORLD), "Slave send case complete");
    }
    optCalculationRun = false;
}

void Optimizer::GetResultsFromSlaves(int nSlaves)
{
    int fitOK;
    int slaveRank;
    MPI_Status status;
    int nCount = nSlaves;
    if (nSlaves < 1)
        return;
    while (nCount >= 1){
        MPIAppSupport::CheckMPI(MPI_Recv(&fitOK, 1, MPI_INT, MPI_ANY_SOURCE, mpiTag_FitOK, MPI_COMM_WORLD, &status), "Receive fitOK from slave");

        if (!fitOK)
            return;

        runCount++;
        slaveRank = status.MPI_SOURCE;
        --nCount;

        //Receiving results from slave[slaveRank]
        int runIndx;
        MPIAppSupport::CheckMPI(MPI_Recv(&runIndx, 1, MPI_INT, slaveRank, mpiTag_NextRun, MPI_COMM_WORLD, &status), "Receive case index from slave");

        double chiSqVal;
        MPIAppSupport::CheckMPI(MPI_Recv(&chiSqVal, 1, MPI_DOUBLE, slaveRank, mpiTag_ChiSq, MPI_COMM_WORLD, &status), "Receive chisq");
        multRunFitChisq[runIndx] = chiSqVal;

        int nFits;
        MPIAppSupport::CheckMPI(MPI_Recv(&nFits, 1, MPI_INT, slaveRank, mpiTag_Nfits, MPI_COMM_WORLD, &status), "Receive nFits");

        for (int i = 0; i < nFits; i++)
        {
            int nfitPoints;
            DC_FitResults&   currFit = multRunFitResults[i][runIndx];

            MPIAppSupport::CheckMPI(MPI_Recv(&nfitPoints, 1, MPI_INT, slaveRank, mpiTag_nfitPoints, MPI_COMM_WORLD, &status), "Receive nfitPoints");
            currFit.nfitPoints = nfitPoints;

            double fitValue;
            MPIAppSupport::CheckMPI(MPI_Recv(&fitValue, 1, MPI_DOUBLE, slaveRank, mpiTag_fitVal, MPI_COMM_WORLD, &status), "Receive fitValue");
            currFit.fitValue = fitValue;

            double sse;
            MPIAppSupport::CheckMPI(MPI_Recv(&sse, 1, MPI_DOUBLE, slaveRank, mpiTag_sse, MPI_COMM_WORLD, &status), "Receive sse");
            currFit.sse = sse;

            double sigma;
            MPIAppSupport::CheckMPI(MPI_Recv(&sigma, 1, MPI_DOUBLE, slaveRank, mpiTag_sigma, MPI_COMM_WORLD, &status), "Receive sigma");
            currFit.sigma = sigma;

            double estVar;
            MPIAppSupport::CheckMPI(MPI_Recv(&estVar, 1, MPI_DOUBLE, slaveRank, mpiTag_estVar, MPI_COMM_WORLD, &status), "Receive estVar");
            currFit.estVar = estVar;

            int fitWasL;
            MPIAppSupport::CheckMPI(MPI_Recv(&fitWasL, 1, MPI_INT, slaveRank, mpiTag_fitWasL1, MPI_COMM_WORLD, &status), "Receive fitWasL1");
            currFit.fitWasL1 = (fitWasL != 0) ? 1 : 0;

            int fitWasNormed1;
            MPIAppSupport::CheckMPI(MPI_Recv(&fitWasNormed1, 1, MPI_INT, slaveRank, mpiTag_fitWasNormed, MPI_COMM_WORLD, &status), "Receive fitWasNormed");
            currFit.fitWasNormed = (fitWasNormed1 != 0) ? 1 : 0;

            currFit.fitResults.AllocAndSetSize(nfitPoints);
            MPIAppSupport::CheckMPI(MPI_Recv(currFit.fitResults.tListData, nfitPoints, MPI_DOUBLE, slaveRank, mpiTag_fitResults, MPI_COMM_WORLD, &status), "Receive fitResults");

            currFit.fitResiduals.xData.AllocAndSetSize(nfitPoints);
            MPIAppSupport::CheckMPI(MPI_Recv(currFit.fitResiduals.xData.tListData, nfitPoints, MPI_DOUBLE, slaveRank, mpiTag_fitResidualsX, MPI_COMM_WORLD, &status), "Receive fitResiduals X");

            currFit.fitResiduals.yData.AllocAndSetSize(nfitPoints);
            MPIAppSupport::CheckMPI(MPI_Recv(currFit.fitResiduals.yData.tListData, nfitPoints, MPI_DOUBLE, slaveRank, mpiTag_fitResidualsY, MPI_COMM_WORLD, &status), "Receive fitResiduals Y");

            currFit.fitDerivatives.AllocAndSetSize(nfitPoints);
            MPIAppSupport::CheckMPI(MPI_Recv(currFit.fitDerivatives.tListData, nfitPoints, MPI_DOUBLE, slaveRank, mpiTag_fitDerivatives, MPI_COMM_WORLD, &status), "Receive fitDerivatives");

            currFit.fitResidualDerivatives.AllocAndSetSize(nfitPoints);
            MPIAppSupport::CheckMPI(MPI_Recv(currFit.fitResidualDerivatives.tListData, nfitPoints, MPI_DOUBLE, slaveRank, mpiTag_fitResDerivatives, MPI_COMM_WORLD, &status), "Receive fitResidualDerivatives");
        }

    }
    return;
}
#endif //!ADCONSOLEAPP


bool  Optimizer::OptimizerRunMultiple(const RunParametersArray& runPars,
                                        const bool              checkResAgainstFirst,
                                        const int               runOffset,
                                        bool                    finalDerivCalc,
                                        bool                    checkRun0Fit,
                                      const  double&            lastRun0Fit,
                                           bool&                run0FitDeteriorated,
                                        bool                    ignoreLimits)
{
    FitResultsPtrArray&  currFits = optimizeInput.GetCurrentFitResults();
    int nFit = currFits.Size();
    run0FitDeteriorated = false;

    int nResFirst;
    if ((runOffset > 0) && checkResAgainstFirst)
        nResFirst = GetNumResiduals(0);


#ifdef ADCONSOLEAPP
    useMPI = MPIAppSupport::MPIOptimizationRun();
    if (useMPI)
    {
        if (runOffset == 0) {
            bool fitOK = CalcFitValue(runPars[0], multRunFitChisq[0], finalDerivCalc, ignoreLimits);
            if (!fitOK)
                return false;

                    //Record fit results here [k][i]
            for (int k = 0; k < nFit; k++){
                multRunFitResults[k][0] = *(currFits[k]);
            }

            if (runCount == 0)
                nResFirst = GetNumResiduals(0);
            if (checkRun0Fit)
            {
                if (multRunFitChisq[0] > lastRun0Fit)
                {
                    run0FitDeteriorated = true;
                    return true;
                }
            }


            //do care that have same number of residuals for each fit before and after
            if (checkResAgainstFirst)
            {
                int numResAfter = GetNumResiduals(0);
                nResFirst = numResAfter;
            }
            updateCB(*this);
        }



        int nRuns;
        if (runOffset == 0)
            nRuns = runPars.Size() - 1;
        else
            nRuns = runPars.Size() - runOffset;
        MPI_Status status;
        int nSlaves = MPIAppSupport::MPIGetNmaxMPIRank() - 1;

        int runIndx = 0;
        if (nRuns > nSlaves)
        {
            int nLoops = 0;

            while ((nRuns - nLoops * nSlaves)>nSlaves) {
                nLoops++;
            //initialize & start first set of processes
                for (int i = 0; i  < nSlaves; i++)
                {
                    runIndx += 1;
                    MPIAppSupport::CheckMPI(MPI_Send(&runIndx, 1, MPI_INT, i + 1, mpiTag_NextRun, MPI_COMM_WORLD), "Starting slave");
                    MPIAppSupport::CheckMPI(MPI_Send(&noptVars, 1, MPI_INT, i + 1, mpiTag_NPars, MPI_COMM_WORLD), "Sending nPars");
                    MPIAppSupport::CheckMPI(MPI_Send(runPars[runIndx].tListData, noptVars, MPI_DOUBLE, i + 1, mpiTag_ParVals, MPI_COMM_WORLD), "Send par vals to Slaves");

                    int finalDerivCalcInt = int(finalDerivCalc);
                    MPIAppSupport::CheckMPI(MPI_Send(&finalDerivCalcInt, 1, MPI_INT, i + 1, mpiTag_FinalDerivCalc, MPI_COMM_WORLD), "Send final Deriv to Slaves ");

                    int ignoreLimitsInt = int(ignoreLimits);
                    MPIAppSupport::CheckMPI(MPI_Send(&ignoreLimitsInt, 1, MPI_INT, i + 1, mpiTag_IgnoreLimits, MPI_COMM_WORLD), "Send ignore limits to slaves");


                }
                GetResultsFromSlaves(nSlaves);
            }

            int currProcComplete;
            int currRank;

            while (runIndx < nRuns )
            {
                // get next available slave rank
                MPIAppSupport::CheckMPI(MPI_Recv(&currProcComplete, 1, MPI_INT, MPI_ANY_SOURCE, mpiTag_RunComplete, MPI_COMM_WORLD, &status), "Get next available slave");

                currRank = status.MPI_SOURCE;

                // start next run on available slave
                runIndx++;
                MPIAppSupport::CheckMPI(MPI_Send(&runIndx, 1, MPI_INT, currRank, mpiTag_NextRun, MPI_COMM_WORLD), "Send case index to slave");
                MPIAppSupport::CheckMPI(MPI_Send(&noptVars, 1, MPI_INT, currRank, mpiTag_NPars, MPI_COMM_WORLD), "Sending nPars"); //MX
                MPIAppSupport::CheckMPI(MPI_Send(runPars[runIndx].tListData, noptVars, MPI_DOUBLE, currRank, mpiTag_ParVals, MPI_COMM_WORLD), "Send par vals to Slaves");

                int finalDerivCalcInt = int(finalDerivCalc);
                MPIAppSupport::CheckMPI(MPI_Send(&finalDerivCalcInt, 1, MPI_INT, currRank, mpiTag_FinalDerivCalc, MPI_COMM_WORLD), "Send final Deriv to Slaves ");

                int ignoreLimitsInt = int(ignoreLimits);
                MPIAppSupport::CheckMPI(MPI_Send(&ignoreLimitsInt, 1, MPI_INT, currRank, mpiTag_IgnoreLimits, MPI_COMM_WORLD), "Send ignore limits to slaves");

                GetResultsFromSlaves(1);
            }

        }
        else
        {
            for (int i = 0; i < nRuns; i++){
                // Send parameters to slaves
                runIndx = i+1;

                MPIAppSupport::CheckMPI(MPI_Send(&runIndx, 1, MPI_INT, i + 1, mpiTag_NextRun, MPI_COMM_WORLD), "Starting slave");
                MPIAppSupport::CheckMPI(MPI_Send(&noptVars, 1, MPI_INT, i + 1, mpiTag_NPars, MPI_COMM_WORLD), "Sending nPars"); //MX

                MPIAppSupport::CheckMPI(MPI_Send(runPars[runIndx].tListData, noptVars, MPI_DOUBLE, i + 1, mpiTag_ParVals, MPI_COMM_WORLD), "Send par vals to Slaves");

                int finalDerivCalcInt = int(finalDerivCalc);
                MPIAppSupport::CheckMPI(MPI_Send(&finalDerivCalcInt, 1, MPI_INT, i + 1, mpiTag_FinalDerivCalc, MPI_COMM_WORLD), "Send final Deriv to Slaves ");

                int ignoreLimitsInt = int(ignoreLimits);
                MPIAppSupport::CheckMPI(MPI_Send(&ignoreLimitsInt, 1, MPI_INT, i + 1, mpiTag_IgnoreLimits, MPI_COMM_WORLD), "Send ignore limits to slaves");

            }
            GetResultsFromSlaves(nRuns);
        }

        for (int i = runOffset+1; i < runPars.Size(); i++){

            if ((i == 0) && checkRun0Fit)
            {
                if (multRunFitChisq[i] > lastRun0Fit)
                {
                    run0FitDeteriorated = true;
                    return true;
                }
            }


            //do care that have same number of residuals for each fit before and after
            if (checkResAgainstFirst && (runOffset == 0))
            {
                int numResAfter = GetNumResiduals(i);
                if (i == 0)
                {
                    nResFirst = numResAfter;
                }
                else
                {
                    if (numResAfter != nResFirst)
                        throw SimError("Difference in # of residuals - decrease span");
                }
            }
            updateCB(*this);
        }
        return true;
    }
#endif //!ADCONSOLEAPP



    for (int i = runOffset; i < runPars.Size(); i++){

        if (!CalcFitValue(runPars[i], multRunFitChisq[i], finalDerivCalc, ignoreLimits))
            return false;

#ifdef DEBUGACTIVE
if (IsDebugCase())
{
    std::ostream&  dbout = GetDebugOutStream();
    dbout << std::scientific << std::setprecision(12) << i << " " << runPars[i][0] << " " << runPars[i][1]  << std::endl;
}
#endif


        for (int k = 0; k < nFit; k++)
        {
            multRunFitResults[k][i] = *(currFits[k]);
        }

        if ((i == 0) && checkRun0Fit)
        {
            if (multRunFitChisq[0] > lastRun0Fit)
            {
                run0FitDeteriorated = true;
                return true;
            }
        }

        if (checkResAgainstFirst && (runOffset == 0))
        {
            int numResAfter = GetNumResiduals(i);
            if (i == 0)
            {
                nResFirst = numResAfter;
            }
            else
            {
                if (numResAfter != nResFirst)
                    throw SimError("Difference in # of residuals - decrease span");
            }
        }
        updateCB(*this);
    }
    return true;
}



OptimizerPackage::OptimizerPackage()
{
    firstStageOpt   = 0;
    secondStageOpt  = 0;
    finalStageOpt   = 0;
    lastOptRun      = 0;
}

OptimizerPackage::~OptimizerPackage()
{
    ClearOptimizers();
}

void OptimizerPackage::SetOptimizers()
{
    ClearOptimizers();

    firstStageOpt = GetOptimizer(firstStageOptType);
    if (!singleStageOpt)
    {
        secondStageOpt = GetOptimizer(secondStageOptType);
        finalStageOpt = secondStageOpt;
    }
    else
    {
        finalStageOpt = firstStageOpt;
        secondStageOpt = 0;
    }

    OptimizeRuntimeOutput::currOptPkg = this;
}

void OptimizerPackage::ClearOptimizers()
{
    delete firstStageOpt;
    delete secondStageOpt;
    firstStageOpt = 0;
    secondStageOpt = 0;
    finalStageOpt   = 0;
    lastOptRun      = 0;

    OptimizeRuntimeOutput::currOptPkg = 0;
}

bool OptimizerPackage::Initialize(Optimizer::UpdateCB   upCB,
                                RunnableC&      simulator)
{
    if (!firstStageOpt->Initialize(upCB, simulator))
    {
        GenAppErrorMsg("OptInit", "first stage optimizer initialization failure");
        return false;
    }

    if ((!singleStageOpt) &&
        (!secondStageOpt->Initialize(upCB, simulator)))
    {
        GenAppErrorMsg("OptInit", "second stage optimizer initialization failure");
        return false;
    }

    return true;
}


void OptimizerPackage::MultiStageOptRun(int fitIndex, int caseIndex, int fitNumber)
{
    lastErr = SimError();

    lastOptRun = firstStageOpt;
    firstStageOpt->FirstStageSetStart(fitIndex, caseIndex, fitNumber);

    firstStageOpt->Optimize();
    if (!singleStageOpt)
    {
        lastOptRun = secondStageOpt;
        secondStageOpt->SecondStageSetStart(*firstStageOpt);
        secondStageOpt->Optimize();
        // for perturbation update
        firstStageOpt->OptCopyArray(firstStageOpt->bestNormParEst, secondStageOpt->bestNormParEst);
    }

    if (calcFinalCovariance)
    {
        try
        {
            finalStageOpt->FinalCovarCalc();
        }
        catch (SimError& err)
        {
            finalStageOpt->OptErrorProcessing(err);
        }
    }

    finalStageOpt->OutputBestFit();

    finalStageOpt->SetFinalParVals();
}

#ifdef ADCONSOLEAPP

void OptimizerPackage::SlaveMultiStageOptRun(int slaveRank)
{
    while (true){
        int fitIndex;
        MPI_Status status;
        // get fit index
        MPIAppSupport::CheckMPI(MPI_Recv(&fitIndex, 1, MPI_INT, 0, mpiTag_FitIndex, MPI_COMM_WORLD, &status), "Slave get fit index");

        // all done indicator ?
        if (fitIndex < 0){
            return;
        }

        firstStageOpt->MPIRunSlave(slaveRank, fitIndex);
    }
    return;
}


void OptimizerPackage::MasterMultiStageOptRun(int fitIndex, int fitNumber, int nSlaves)
{
    //set fit number at each slave
    for (int i = 0; i  < nSlaves; i++)
    {
        MPIAppSupport::CheckMPI(MPI_Send(&fitIndex, 1, MPI_INT, i + 1, mpiTag_FitIndex, MPI_COMM_WORLD), "Setting slave fit#");

    }

    MultiStageOptRun(fitIndex, 0, 0);

    // SEND RUN INDEX = -1
    int runIndex = -1;
    for (int i = 0; i  < nSlaves; i++)
    {
        MPIAppSupport::CheckMPI(MPI_Send(&runIndex, 1, MPI_INT, i + 1, mpiTag_NextRun, MPI_COMM_WORLD), "Setting slave run# =  -1");
    }

}

void OptimizerPackage::MPIOptStopSlaves(int nSlaves)
{
    int nCount = nSlaves;
    int fitIndex = -1;

    for (int i = 0; i < nSlaves; i++)
        MPIAppSupport::CheckMPI(MPI_Send(&fitIndex, 1, MPI_INT, i+1, mpiTag_FitIndex, MPI_COMM_WORLD), "Send stop command to slaves");
}
#endif //!ADCONSOLEAPP



Optimizer* OptimizerPackage::GetOptimizer(OptimizationType  optType)
{
    switch (optType) {
        case otSimplex       : return new SimplexOptimizer();
        case otLevMar        : return new LevMarOptimizer();
        case otSimAnneal     : return new SimulatedAnnealingOptimizer();
        case otSCEM          : return new SCEMOptimizer();
    }
    return 0;
}


