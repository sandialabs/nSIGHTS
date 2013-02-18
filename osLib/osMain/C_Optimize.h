///////////////////////////////////////////////////////////////////////////////////
//
// C_Optimize.h
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


#ifndef C_OPTIMIZE_H
#define C_OPTIMIZE_H

#include <genClass/T_SC_Array.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_Random.h>

#include <osClass/C_Runnable.h>
#include <osClass/C_SimErr.h>
#include <osClass/C_OptVar.h>

#include <osClass/DC_FitResults.h>


typedef T_SC_Array<SC_DoubleArray> RunParametersArray;

class Optimizer : public OptimizationControlStatics {  // for easy access to control vars

    friend class OptimizerPackage;

    public:
        typedef void (*UpdateCB)(Optimizer& opt);

    public:
        int                 noptVars;
        OptVarArray         varsToOptimize;
        SC_DoubleArray      initNormParEst, startNormParEst;
        SC_DoubleArray      bestNormParEst, bestParEst;
        RunParametersArray  runPars;

        //  basic status
        int                 runCount, optIterCount;
        double              bestFitVal, worstFitVal, currFitVal;

        //  tolerances
        double              currFitTolerance, currParameterTolerance, currErrorTolerance;


        // covar results
        SC_DoubleMatrix     optActualCovar, optEstimatedCovar;

        // perturbation random # generator
        SC_Random           pertRand;


        const char*         opMessage;

        // status vars for deriv span calc
        double              optDerivSpan;
        int                 derivSpanIndex;
        double              derivSpanCriteria;
        double              derivSpanResult;

        FitResultsMatrix    multRunFitResults;   // multRunFitResults[i] is results for fit i
                                                 // multRunFitResults[i][j] is run j of fit i
        SC_DoubleArray      multRunFitChisq;
        SC_DoubleCube       multFitHessianMatrix;   // [i] is for fit i


    protected:
        SC_DoubleMatrix     optDerivAlpha;
        SC_DoubleArray      optDerivBeta;

        UpdateCB            updateCB;
        bool                okForOutput;  // for forward output on failed sims
        //  slight kluge to retain semi-fatal error mesage
        SimError            lastSemiFatal;

        double              optInitialDerivSpan;

    private:
        RunnableC*          currSimulator;
        double              toleranceMultiplier;
        //  derivative span calc control/status
        double              covarDaComp, covarDaCriteria, lastDaSpan;
        int                 covarDaIndex;

        SC_DoubleMatrix     perturbValues;

        bool                useMPI;
    public:
                            Optimizer();
        virtual             ~Optimizer();

        virtual bool        Initialize(UpdateCB       upCB,
                                       RunnableC&  simulator);

        void                SetMultirunDerivCalcParametersArray(const SC_DoubleArray& normParEstimates);
        bool                OptimizerRunMultiple(const RunParametersArray& runPars,
                                                const bool                  checkResAgainstFirst,
                                                const int                   runOffset,
                                                       bool                 finalDerivCalc,
                                                       bool                 checkRun0Fit,
                                                const  double&              lastRun0Fit,
                                                       bool&                run0FitDeteriorated,
                                                       bool                 ignoreLimits = false);


        void                PerturbationSetup();
        void                FirstStageSetStart(int fitIndex, int caseIndex, int fitNumber);
        void                SecondStageSetStart(const Optimizer& firstStage);

        virtual void        Optimize();
        bool                FinalCovarCalc();
        void                SetFinalParVals();
        void                SetMultiRunOptDoDaCalcRunParameter(const SC_DoubleArray& normParEstimates,
                                                          int             pIndex,
                                                          const double&   parStep,
                                                        int               fIndex);
        void                FinalCovarOnly(int fitIndex);
        void                OutputBestFit();

        void                OptFinalUpdate(); // called on opt error to update
                                              // registered output files

        //  for extracting optimizer tolerance data
        int                 GetnBaseOutput();
        void                GetBaseOutputID(SC_StringArray& baseIDs) const; // table column IDs
        void                GetBaseOutput(SC_DoubleArray& baseData) const; // table column data

        //  for extracting optimizer specific data
        virtual int         GetnExtraOutput() const;                          // number of extra data to put in output tables
        virtual void        GetExtraOutputID(SC_StringArray& extraIDs) const; // table column IDs
        virtual void        GetExtraOutput(SC_DoubleArray& extraData) const; // table column data

        //  opt array / matrix support -- somewhat more efficient that T_SC_Array copies etc
        //  public to allow access by output file data prep routines
        void                OptAllocMatrix(SC_DoubleMatrix&  mat) const;
        void                OptClearMatrix(SC_DoubleMatrix&  mat) const;
        void                OptCopyMatrix(      SC_DoubleMatrix&  dest,
                                         const SC_DoubleMatrix&  src) const;

        void                OptAllocArray(SC_DoubleArray&  arr) const;
        void                OptClearArray(SC_DoubleArray&  arr) const;
        void                OptCopyArray(       SC_DoubleArray&  dest,
                                         const SC_DoubleArray&  src) const;


#ifdef ADCONSOLEAPP
            //              all optizer MPI code
        void                MPIOptimizer(bool useMPIMultipleRuns);
        void                MPIRunSlave(int slaveRank, int fitIndex);
        bool                MPISlaveRunCalculations(int slaveRank, int fitIndex);
        void                GetResultsFromSlaves(int slaveRank);

#endif
    protected:


        bool                ToleranceCheck(const double& currFit,
                                           const double& prevFit,
                                           const SC_DoubleArray& currParEstimates,
                                           const SC_DoubleArray& prevParEstimates);


        bool                NormParOK(const SC_DoubleArray& normParEstimates);
                            // returns true if parameters are OK (0 - 1) and
                            // not equal to the current best fit parameters
                            // if not OK currFitVal = worstFit * multiplier
                            // if equal currFitVal = bestFitVal


        bool                CalcFitValue(const SC_DoubleArray& normParEstimates,
                                               double&         nextFit,
                                               bool            finalDerivCalc = false, // if true, ignores minor normalizationm limits errors
                                               bool            ignoreLimits = false);  // if true, ignores limits altogether - for MINPACK L-M



        //  L-M routines
        bool                OptDoDerivRun(const SC_DoubleArray& normParEstimates,
                                                int             iIndex,
                                                int             jIndex,
                                          const int             numResStart,
                                                bool            finalDerivCalc,
                                                bool            ignoreLimits = false);

        int                 GetNumResiduals(int runNumber);

        bool                OptHalfDerivRun(RunParametersArray&   runPars,
                                            const int             numRes,
                                                 bool             finalDerivCalc,
                                                 bool             ignoreLimits = false);

        void                OptCalcFullDeriv(SC_DoubleMatrix&   alpha,
                                            SC_DoubleArray&    beta);
        void                OptCalcHalfDeriv(SC_DoubleMatrix&   alpha,
                                            SC_DoubleArray&    beta);

        void                OptSetBottomTri(SC_DoubleMatrix&   alpha);

        double              OptDoDaCalcRun(const SC_DoubleArray& normParEstimates,
                                            int             pIndex,
                                            const double&   parStep);

        void                OptSetDerivSpan(const SC_DoubleArray& pars,
                                              int             parIndx);

        void                ParSensitivityCheck(const SC_DoubleMatrix& alpha);



        void                Invert(SC_DoubleMatrix&   a,
                                   int                n);


        //  misc support
        void                ResetToInitial();
        void                OptLimitError(int           badPar,
                                          const char*   charDesc,
                                          const double& parVal);

        void                GetOptNormParEst(SC_DoubleArray& normParEstimates);
        void                GetOptParEst(SC_DoubleArray& parEstimates);

        //  standard cleanup on exit
        void                OptErrorProcessing(SimError& optErr);


    private:
        void                OptStartCommon();
        void                ResetTolerances();
};


class OptimizerPackage : public OptimizationControlStatics  {
    public:
        SimError    lastErr;

        Optimizer*  firstStageOpt;
        Optimizer*  secondStageOpt;
        Optimizer*  finalStageOpt;  // either first or second
        Optimizer*  lastOptRun;     // either first or second

    public:
                    OptimizerPackage();
                    ~OptimizerPackage();

        void        SetOptimizers();
        void        ClearOptimizers();

        bool        Initialize(Optimizer::UpdateCB  upCB,
                                RunnableC&      simulator);

        void        MultiStageOptRun(int fitIndex, int caseIndex, int fitNumber);

#ifdef ADCONSOLEAPP
        void        MasterMultiStageOptRun(int fitIndex, int fitNumber, int nSlaves);

        void        SlaveMultiStageOptRun(int slaveRank);
        void        MPIOptStopSlaves(int nSlaves);

#endif

    private:
        Optimizer*  GetOptimizer(OptimizationType  optType);
};






#endif // !C_OPTIMIZE_H

