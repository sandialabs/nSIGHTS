///////////////////////////////////////////////////////////////////////////////////
//
// OptLevMar.cpp
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
//      Main optimizer/sampler code and global variables and other support classes used by optimizer and application.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/C_TxtFile.h>  // for debug - must be above mpi.h

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>

#include <osClass/C_SimErr.h>
#include <osMain/G_Optimize.h>

#ifdef LAPACK
#include <genClass/U_Lapack.h>
#endif // LAPACK

#include "OptLevMar.h"
#include "iostream"

//TxtFileC debugFF("C:/SWDev/Verification/nSIGHTS/FBCase161/MPI/optTwoStageFits/optSimplex2StageFit/debugOutputLM_old.out", false);
//TxtFileC debugFF("C:/SWDev/Verification/nSIGHTS/FBCase161/MPI/optTwoStageFits/optSimplex2StageFit/debugOutputLM_MPI.out", false);



LevMarOptimizer::LevMarOptimizer()
{
    lmRelChange = 1000.0;
}
LevMarOptimizer::~LevMarOptimizer()
{
}

bool LevMarOptimizer::Initialize(UpdateCB       upCB,
                           RunnableC&  simulator)
{
    if (!Optimizer::Initialize(upCB, simulator))
        return false;
    try
    {
        OptAllocArray(lmNormParEst);
        OptAllocArray(lmLastParEst);
        declineData.Alloc(maxDecline);

        OptAllocArray(da);
        OptAllocArray(atry);
        OptAllocMatrix(covar);

    }
    catch (SC_Array::AllocationError)
    {
        return false;
    }

    return true;
}


void  LevMarOptimizer::Optimize()
{
    lmRelChange = 1000.0;
    optIterCount = 0;
    try
    {
        optCalculationRun = true;
        opMessage = "LM - Initializing optimizer ...";

        LMStartOpt();

        LMDoOpt();

        if (useLastLMForDerivativeCalc)
            optInitialDerivSpan = optDerivSpan;

    }
    catch (SimError& err)
    {
        OptErrorProcessing(err);
    }
}

int LevMarOptimizer::GetnExtraOutput() const
{
    return 1;
}
void LevMarOptimizer::GetExtraOutputID(SC_StringArray& extraIDs) const
{
    extraIDs.Alloc(1);
    extraIDs += "LMRelChng";
}
void LevMarOptimizer::GetExtraOutput(SC_DoubleArray& extraData) const
{
    extraData.Alloc(1);
    extraData += lmRelChange;
}

bool  LevMarOptimizer::EvalFunction(const SC_DoubleArray& normParEstimates)
{
//   Chisq at point
    bool    run0FitDeteriorated = true;
    SetMultirunDerivCalcParametersArray(normParEstimates);

    opMessage = "LM - main run & partials  ...";
    if (!OptimizerRunMultiple(runPars, true, 0, false, !lmInitializing, lmChisq, run0FitDeteriorated,  false))
        return false;

    double currChisq = multRunFitChisq[0];

    if (run0FitDeteriorated)  // if fit is not better, no need to calculate further
    {
        if (lmLastChisq > currChisq)
            lmLastChisq = currChisq;

        opMessage = "LM - fit deteriorated ...";
        optProgressOutput.CalcAndUpdate();
        updateCB(*this);

        // no so return
        return false;
    }

    // for update routines only
    currFitVal = currChisq;

    if (lmInitializing)
    {
        bestFitVal = currChisq;
        GetOptParEst(bestParEst);

        lmChisq = currChisq;
        lmLastChisq = currChisq + currChisq;
        lmLastParEst = normParEstimates;
    }
    else
    {
        // for update routines only
        bestFitVal = currChisq;

        lmLastChisq = lmChisq;
        lmChisq = currChisq;
        GetOptParEst(bestParEst);

        opMessage = "LM - fit improved ...";
    }

    optProgressOutput.CalcAndUpdate();
    updateCB(*this);

    opMessage = "LM -partials for next iteration ...";

    return true;
}


bool LevMarOptimizer::Mrqcof(const SC_DoubleArray&    a,
                             SC_DoubleMatrix&   alpha,
                             SC_DoubleArray&    beta)
{
    if (EvalFunction(a))
    {
        OptCalcHalfDeriv(alpha, beta);

        return true;
    }
    return false;
}

void  LevMarOptimizer::LMStartOpt()
{
    lmInitializing = true;
    lmNormParEst = startNormParEst;
    nlambdaDecline  = 0;
    nlambdaIncrease = 0;
    optDerivSpan = lmInitialDerivSpan;

    if (!Mrqcof(lmNormParEst, optDerivAlpha, optDerivBeta))
    {
        char tempStr[SimError::maxLen];
        MakeString(tempStr, "semi-fatal in initial calc: ", lastSemiFatal.errStr, SimError::maxLen);
        throw SimError(tempStr);
    }

    lmLambda  = lmInitialLambda;
    lmInitializing = false;
}

void LevMarOptimizer::MrqMin(bool& fitImproved)
{

    bool reviseDa = true;
    double daTot;
    fitImproved = false;

#ifdef LAPACK
    //declare variables outside the loop to limit the amount of dynamic memory allocation
    //which needs done.  TODO: create a work class variable to do this just once.

    //it appears that the inverse is calculated by Gauss Jordan but never used after solving linear system!
    //This LM algorithm could see some major improvements but we ignore at this time
    //ksb 3/7/12

    //For now, use LAPACK to calculate upgrade vector, da
    //LAPACK solves Ax=b though by the variable definitions it looks
    //like we are solving covar x=da
    char trans = 'T';                   //Not working with the transpose
    int M = noptVars;                   //number of rows
    int N = noptVars;                   //number of columns
    int nrhs = 1;                       //number of right hand sides
    int lda = M;                        //leading dimension of A
    int ldb = N;                        //leading dimension of b
    int lwork = 100*M;                  //size of the work area
    double *work = new double[lwork];   //work area
    int info;                           //success or failure

    SC_DoubleMatrix A;
    A.FixedMatrixAllocAndSetSize(M,N);
#endif // LAPACK

    while (reviseDa)
    {
        OptCopyMatrix(covar, optDerivAlpha);
        double lamMult = 1.0 + lmLambda;
        for (int j = 0; j < noptVars; j++)
            covar[j][j] *= lamMult;

        OptCopyArray(da, optDerivBeta);
#ifdef LAPACK

        //copy covariance over
        for (int i = 0; i < noptVars; i++)
        {
            for (int j = 0; j < noptVars; j++)
            {
                A.buffer[i*noptVars+j]=covar[i][j];
            }
        }

        dgels_(&trans, &M, &N, &nrhs, (double *) A.buffer, &lda, (double *) da.tListData, &ldb, work, &lwork, &info);

        if(info != 0)
        {
            throw SimError("LevMarOptimizer::MrqMin - matrix solver failed");
        }

#else // LAPACK
        GaussJ(covar, noptVars, da);
#endif // LAPACK

        daTot = 0.0;
        reviseDa = false;
        int badDaIndex;
        for (int j = 0; j < noptVars; j++)
        {
            atry[j] = lmNormParEst[j] + da[j];
            if ((atry[j] >= 1.0) || (atry[j] <=0.0))
            {
                reviseDa = true;
                badDaIndex = j;
            }
            daTot += Sqr(da[j]);
        }

        if (reviseDa)
        {
            lmLambda += lmLambda;
            if (lmLambda > 1.0E+08)
                OptLimitError(badDaIndex, "lambda", lmLambda);
        }
    }

    optDerivSpan = sqrt(daTot) / 2.0;
    if (optDerivSpan > lmInitialDerivSpan)
        optDerivSpan = lmInitialDerivSpan;

    if (Mrqcof(atry, covar, da))
    {
        nlambdaIncrease = 0;
        nlambdaDecline++;
        double lamFactor = lmLambdaFactor * double(nlambdaDecline + 1);
        if (lmLambda > 100.0)
            lamFactor = Sqr(lamFactor);

        lmLambda /= lamFactor;
        if (lmLambda < lmMinLambda)
            lmLambda = lmMinLambda;

        OptCopyMatrix(optDerivAlpha, covar);
        OptCopyArray(optDerivBeta, da);
        OptCopyArray(lmNormParEst, atry);

        // we have a best fit by this point
        bestNormParEst = lmNormParEst;
        okForOutput = true;

        fitImproved = true;
    }
    else
    {
        nlambdaIncrease++;
        nlambdaDecline = 0;

        double lamFactor = lmLambdaFactor * double(nlambdaIncrease + 1);
        if (lmLambda > 100.0)
            lamFactor = Sqr(lamFactor);

        if (lamFactor > 500.0)
            lamFactor = 500.0;

        lmLambda *= lamFactor;
    }
#ifdef LAPACK
    //cleanup workspace
    delete [] work;
#endif // LAPACK

}


void LevMarOptimizer::LMDoOpt()
{
    optIterCount  = 0;
    declineData[0] = lmChisq;
    int nDecline = 0;
    lmRelChange = lmRelChangeTolerance + 1.0;
    bool lastWasBest = false;
    bool tolOK = false;
    do
    {
        optIterCount ++;
        lmLastParEst = lmNormParEst;

        MrqMin(lastWasBest);

        if (lmChisq < declineData[nDecline])
        {
            if (nDecline < (maxDecline - 1))
            {
                nDecline++;
                declineData[nDecline] = lmChisq;
            }
            else
            {
                for (int i = 0; i < (maxDecline - 1); i++)
                    declineData[i] = declineData[i + 1];
                declineData[maxDecline - 1] = lmChisq;
                lmRelChange = 1.0 - declineData[maxDecline - 1] / declineData[0];
            }
        }

        tolOK = ToleranceCheck(lmLastChisq, lmChisq, lmLastParEst, lmNormParEst);

    } while (!(tolOK || (lmRelChange <= lmRelChangeTolerance)));

}


