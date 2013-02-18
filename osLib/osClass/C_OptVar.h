///////////////////////////////////////////////////////////////////////////////////
//
// C_OptVar.h
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
//      variable to be optimized.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_OPTVAR_H
#define C_OPTVAR_H

#include <genClass/T_SC_Array.h>
#include <genClass/SC_SetupErr.h>
#include <genClass/SC_StringArray.h>
#include <osClass/C_VarBase.h>


//  class containing static control variables
//  set up as super class so implementation can use in derived class without instantiating a
//  OptimizeVar

class   OptimizationControlStatics  {
    public:
        enum    OptimizationType  {otSimplex, otLevMar, otSimAnneal, otSCEM};

        class OptIDArray : public SC_StringArray {
            public:
                OptIDArray();
                const char* GetID(OptimizationType optType) const {return (*this)[int(optType)];}
        };

        static  OptIDArray        optTypeNames;


        //  static control variables basic - approach modified 16 Aug 04
        static  bool                singleStageOpt;
        static OptimizationType     firstStageOptType;
        static OptimizationType     secondStageOptType;


        static bool                calcFinalCovariance;
        static bool                use2ndOrderCovar;   // if calcFinalCovariance
                                                       // 2nd order covariance matrix
        static bool                useFixedDerivSpan;  // if calcFinalCovariance, if true use
                                                      // fixed value for derivative span in covar calcs
                                                      // if false, use iterative approach for determining span

        //  general control
        static double              derivativeTolerance; // convergence on Janis White algorithm
        static double              initialDerivSpan;    // start span for calculated - may be overriden by LM option
        static double              fixedDerivSpan;      // for final covar calcs if useFixedDerivSpan

        static double              transitionToleranceMultiplier; // criteria for change from 1st stage to 2nd stage

        //  tolerance control added for Kay - Aug 04
        static  bool                useFitTolerance;
        static  bool                useParameterTolerance;
//          static  bool                useErrorTolerance; - removed Sep05 at KV request
        static  bool                combineToleranceWithOR;
        static double               parameterTolerance;  // was in prev version - misnamed
        static double               fitTolerance;        // delta fit
//          static double               errorTolerance;      // abs fit - removed Sep05 at KV request

        //  simplex control
        static double              simplexSpan;        // initial size of simplex

                                                            // false - Nelder/Mead
        static  double              simplexReflectionCoeff;     // for Nelder-Mead
        static  double              simplexContractionCoeff;    // for Nelder-Mead
        static  double              simplexExpansionCoeff;      // for Nelder-Mead
        static  double              simplexShrinkageCoeff;      // for Nelder-Mead revised

        //  Lev-Mar control
        static double               lmInitialDerivSpan;     // initial LM deriv span
        static double               lmInitialLambda;        // initial lambda
        static double               lmLambdaFactor;         // lambda mult/div factor
        static double               lmMinLambda;            // minimum lambda factor
        static double               lmRelChangeTolerance;   // tolerance for LM completion
        static bool                 useLastLMForDerivativeCalc; // use last LM deriv span as initial for calculated covar span

        static double               lmLambda;               // MINPACK lambda (factor)
        static bool                 normalizeParMINPACK;    // normalize parameters for MINPACK
        static bool                 lmAdjustDerivSpan;      // allow deriv span to be calculated

        // simulated annealing - uses simplex coefficients
        enum CoolingSchedule        {csNR1, csNR2,
                                    // csNR3, - removed Sep05 by KV request
                                     csLuke1, csLuke2,
                                     // csLuke6, - removed Sep05 by KV request
                                     csLuke7, csLuke8, csLuke9};

        static CoolingSchedule      coolingSchedule;
        static int                  saRandomSeed;
        static double               sa_T0;              // initial T for all
        static int                  sa_N;               // iteration budget
                                                        // K in NR, N in Luke


//  NR3 support removed Sep 05 by KV request
//          static double               saNR_beta;          // beta for NR3
//          static double               saNR_gamma;         // gamma for NR3

        static double               saL_TN;             // tN for all Luke


        // SCEM
        static int                  scem_p;             // # of complexes
        static int                  scem_m;             // # of points in each complex
        static int                  scem_q;             // # of points in each CCE subcomplex
        static int                  scem_alpha;         // # of iterations in gen offspring
        static int                  scem_beta;          // # of iterations in CCE
        static int                  scem_seed;          // # random # seed


        // perturbation control -- only valid for Optimize/Single
        static bool                 doOptPerturbations;     // true - do perturbations
        static bool                 perturbFromStart;       // true - start perturbations from original estimate
                                                            // false - start perturbation from result of last optimization
        static int                  nPerturb;               // number of perturbations to perform > 1
        static double               perturbSpan;            // percent of parameter range used in sample
        static int                  perturbRandomSeed;      // random number seed for perturb runs.
        static bool                 perturbMultiFitSameStart;  // multiple fits start at the same place
        static int                  currPerturbIndex;

        // multiple optimization control
        static bool                 startMultipleFromOriginalEst;   // true - start multiple optimizations from original estimate
                                                                    // false - start multiple from best estimate
        static bool                 updateParameterEstimates;       // true - updates start point to best estimate after optimization complete
                                                                    // false - leave start point at original value

        static int                  maxOptRun;              // minimum # of simulations before giving up



    protected:
        //  following vars are shared between optimizer and fit objects for Hessian calcs in LM
        //  this is a bit of a kluge
        static bool                 optCalculationRun;       // if set the fitResultsDC vars will be calc


    public:

                                    // for menu & listing
        static bool                 UseSimplex();
        static bool                 UseLevMar();
        static bool                 UseSimAnneal();
        static bool                 UseSCEM();
        inline static bool          HasCovariance()  {return calcFinalCovariance;}

        static bool                 RequiresChisq();

};


class OptVar;                       // forward ref for ptr

class OptVarArray : public T_SC_Array<OptVar*> {
    public:
                OptVarArray(); // ctor initializes with call to OptVar::GetActiveArray();
        void    CreateFrom(const VarBaseArray& a);
};


// base class for all Optimized variables
class   OptVar  :  public OptimizationControlStatics, public VarBase {

    friend class OptVarArray;

    public:
        double              optMinLim;
        double              optMaxLim;
        double              optParEstimate;
        double              optParSigma;
        bool                optIsLinear;

    private:
        static VarBase*     listHead;                //  head of list


    public:
                                OptVar(bool& isActiveInit, const char* varIDInit, bool defIsLinear = true);
                                OptVar(bool& isActiveInit, const char* varShortIDInit, const char* varLongIDInit, bool defIsLinear = true);
        virtual                 ~OptVar();
        OptVar&                 operator= (const OptVar& a);

    //  access to best estimate / curr optimizer val
        void                    SetNormalizedEstimate(const double& normVal);       // 0.0 <= norm val <= 1.0
        double                  GetNormalizedEstimate() const;                       // 0.0 <= norm val <= 1.0

        double                  GetParEstimate() const {return optParEstimate;}
        void                    SetParEstimate(const double& parVal) {optParEstimate = parVal;}

        virtual double          GetFinalParEstimate() const {return optParEstimate;}  // after any unit conversions
        virtual double          GetFinalMultiplier() const  {return 1.0;}             // unit conversion -- required for post-processing

        double                  Normalize(const double& parEst)         const;
        double                  DeNormalize(const double& normVal)      const;

        bool                    OptimizeSetupOK(SC_SetupErr& errData);      // returns true if basic setup is OK
        void                    GetMenuStr(char* menuStr, int maxLen);



    // VarBase virtuals
        virtual double          GetVarValue()    const;
        virtual double          GetMinVarValue() const;
        virtual double          GetMaxVarValue() const;


    //  remainder are all statics
        static int              GetnActive();     // returns number of active Optimized vars
        static void             GetActiveArray(OptVarArray& activeArray);
        static void             SetWasActive();

    protected:
        void                    LocalCopy(const OptVar& a);  // implementation of copy

    private:
        OptVar*                 GetNextActive() const; // returns next active from this
        static OptVar*          GetFirstActive(); // returns first active

                                // copy ctor not allowed
                                OptVar(const OptVar& a);
        void                    InitCommon();

};


#endif // !C_OPTVAR_H

