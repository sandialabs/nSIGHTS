///////////////////////////////////////////////////////////////////////////////////
//
// C_OptVar.cpp
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

#include "OsLibAfx.h"
#include <genClass/C_Common.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osClass/C_OptVar.h>

OptimizationControlStatics::OptIDArray::OptIDArray()
{
    Alloc(6);
    (*this) += "Simplex";
    (*this) += "LevMar";
    (*this) += "SimAnneal";
    (*this) += "SCEM";
}

OptimizationControlStatics::OptIDArray  OptimizationControlStatics::optTypeNames;


bool    OptimizationControlStatics::singleStageOpt        = true;
OptimizationControlStatics::OptimizationType  OptimizationControlStatics::firstStageOptType = otSimplex;
OptimizationControlStatics::OptimizationType  OptimizationControlStatics::secondStageOptType = otLevMar;

bool    OptimizationControlStatics::calcFinalCovariance     = true;
bool    OptimizationControlStatics::use2ndOrderCovar        = false;
bool    OptimizationControlStatics::useFixedDerivSpan       = false;

          //  general control
double  OptimizationControlStatics::derivativeTolerance  = 1.0E-04;
double  OptimizationControlStatics::fixedDerivSpan       = 1.0E-06;
double  OptimizationControlStatics::initialDerivSpan     = 1.0E-03;
double  OptimizationControlStatics::transitionToleranceMultiplier  = 1000.0;;

        // new tolerance section - Aug04
bool    OptimizationControlStatics::useFitTolerance         = true;
bool    OptimizationControlStatics::useParameterTolerance   = false;
bool    OptimizationControlStatics::combineToleranceWithOR  = true;
double  OptimizationControlStatics::parameterTolerance      = 1.0E-04;
double  OptimizationControlStatics::fitTolerance            = 1.0E-04;

//  removed Sep 05 at KV request
//bool      OptimizationControlStatics::useErrorTolerance       = false;
//double    OptimizationControlStatics::errorTolerance          = 1.0E-04;

          //  simplex control
double  OptimizationControlStatics::simplexSpan                     = 0.1;
double  OptimizationControlStatics::simplexReflectionCoeff          = 1.0;
double  OptimizationControlStatics::simplexContractionCoeff         = 0.5;
double  OptimizationControlStatics::simplexExpansionCoeff           = 3.5;  // KV spec 7 Jul 05
double  OptimizationControlStatics::simplexShrinkageCoeff           = 0.5;


         //  Lev-Mar control
double   OptimizationControlStatics::lmInitialDerivSpan   = 0.01;
double   OptimizationControlStatics::lmInitialLambda      = 1.0;
double   OptimizationControlStatics::lmLambdaFactor       = 2.0;
double   OptimizationControlStatics::lmMinLambda          = 0.001;
double   OptimizationControlStatics::lmRelChangeTolerance = 0.001;
bool     OptimizationControlStatics::useLastLMForDerivativeCalc = true;

        // Lev-Mar minpack control
double   OptimizationControlStatics::lmLambda             = 100.0;
bool     OptimizationControlStatics::normalizeParMINPACK  = true;
bool     OptimizationControlStatics::lmAdjustDerivSpan    = false;

         //  sim anneal control

OptimizationControlStatics::CoolingSchedule OptimizationControlStatics::coolingSchedule = csNR1;

double  OptimizationControlStatics::sa_T0 = 100.0;      // t0 for all Luke
int     OptimizationControlStatics::sa_N  = 100;                    // iteration budget
                                                                // K in NR, N in Luke
int     OptimizationControlStatics::saRandomSeed = 35471;

double  OptimizationControlStatics::saL_TN      = 0.0;          // tN for all Luke



            // SCEM
int     OptimizationControlStatics::scem_p = 2;             // # of complexes
int     OptimizationControlStatics::scem_m = 4;             // # of points in each complex
int     OptimizationControlStatics::scem_q = 3;             // # of points in each CCE subcomplex
int     OptimizationControlStatics::scem_alpha = 1;         // # of iterations in gen offspring - KV spec 7 Jul 05
int     OptimizationControlStatics::scem_beta  = 2;         // # of iterations in CCE
int     OptimizationControlStatics::scem_seed  =  2976221;  // random seed

        //perturbation

bool    OptimizationControlStatics::doOptPerturbations      = false;
bool    OptimizationControlStatics::perturbFromStart        = true;
int     OptimizationControlStatics::nPerturb                = 50;
int     OptimizationControlStatics::currPerturbIndex        = 0;
double  OptimizationControlStatics::perturbSpan             = 0.20;
int     OptimizationControlStatics::perturbRandomSeed       = 13597;
bool    OptimizationControlStatics::perturbMultiFitSameStart = false;

bool    OptimizationControlStatics::startMultipleFromOriginalEst = true;
bool    OptimizationControlStatics::updateParameterEstimates     = false;

int     OptimizationControlStatics::maxOptRun            = 10000;

bool    OptimizationControlStatics::optCalculationRun   = false;


bool OptimizationControlStatics::UseSimplex()
{
    return (firstStageOptType == otSimplex) ||
        ((!singleStageOpt) && (secondStageOptType == otSimplex));
}
bool OptimizationControlStatics::UseLevMar()
{
    return (firstStageOptType == otLevMar) ||
        ((!singleStageOpt) && (secondStageOptType == otLevMar));
}

bool OptimizationControlStatics::UseSimAnneal()
{
    return (firstStageOptType == otSimAnneal) ||
        ((!singleStageOpt) && (secondStageOptType == otSimAnneal));
}


bool OptimizationControlStatics::UseSCEM()
{
    return (firstStageOptType == otSCEM) ||
        ((!singleStageOpt) && (secondStageOptType == otSCEM));
}

bool OptimizationControlStatics::RequiresChisq()
{
    return UseLevMar() || (calcFinalCovariance);
}


VarBase* OptVar::listHead = 0;

OptVar::OptVar(bool& isActiveInit, const char* varIDInit, bool defIsLinear)
        : VarBase(isActiveInit, varIDInit, listHead), optIsLinear(defIsLinear)
{
    InitCommon();
}

OptVar::OptVar(bool& isActiveInit, const char* varShortIDInit, const char* varLongIDInit, bool defIsLinear)
        : VarBase(isActiveInit, varShortIDInit, varLongIDInit, listHead), optIsLinear(defIsLinear)
{
    InitCommon();
}

void OptVar::InitCommon()
{
    // properties
    optMinLim       = nullReal;
    optMaxLim       = nullReal;
    optParEstimate  = nullReal;
    optParSigma     = 1.0;
}

OptVar::~OptVar()
{
    RemoveVarBase(listHead);
}

OptVar& OptVar::operator= (const OptVar& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void OptVar::SetNormalizedEstimate(const double& normVal)
{
    optParEstimate = DeNormalize(normVal);
}

double OptVar::GetNormalizedEstimate() const
{
    return Normalize(optParEstimate);
}

double  OptVar::Normalize(const double& parEst) const
{
    if (optIsLinear)
        return (parEst - optMinLim) / (optMaxLim - optMinLim);

    double logMin = log10(optMinLim);

    return  (log10(parEst) - logMin) / (log10(optMaxLim) - logMin);
}


double  OptVar::DeNormalize(const double& normVal) const
{
    //not always within range for L-M MINPACK
    //if ((normVal < -0.1) || (normVal > 1.1))   // wider range to allow for final deriv calcs
    //  GenAppInternalError("OptVar::DeNormalize");

    if (optIsLinear)
        return optMinLim + normVal * (optMaxLim - optMinLim);

    double logMin = log10(optMinLim);
    return pow(10.0,  logMin + normVal * (log10(optMaxLim) - logMin));
}


bool OptVar::OptimizeSetupOK(SC_SetupErr& errData)       // returns true if basic setup is OK
{
    // don't check if not set for optimization
    if (!isActive)
        return true;

    SC_SetupErr optErr;

    if (RealIsNull(optParEstimate) || RealIsNull(optMaxLim) || RealIsNull(optMinLim))
        optErr.SetConstantError("opt data not set");
    else if (optMinLim >= optMaxLim)
        optErr.SetConstantError("opt min must be < max");
    else if ((optParEstimate >= optMaxLim) || (optParEstimate <= optMinLim))
        optErr.SetConstantError("opt best estimate must be between min and max");
    else if ((! optIsLinear) && (optMinLim < stdEps))
        optErr.SetConstantError("log stepping opt min must be greater than 0.0");

    return errData.SetupOK(varShortID, optErr);
}

void OptVar::GetMenuStr(char* menuStr, int maxLen)
{
    SC_SetupErr optErr;
    if (OptimizeSetupOK(optErr))
    {
        if (optIsLinear)
            CopyString(menuStr, "Lin", maxLen);
        else
            CopyString(menuStr, "Log", maxLen);

    }
    else
        CopyString(menuStr, "BAD", maxLen);
}



void  OptVar::LocalCopy(const OptVar& a)
{
    optMinLim           = a.optMinLim      ;
    optMaxLim           = a.optMaxLim      ;
    optParEstimate      = a.optParEstimate ;
    optParSigma         = a.optParSigma    ;
    optIsLinear         = a.optIsLinear    ;
}


OptVar* OptVar::GetNextActive() const
{
    return static_cast<OptVar*>(VarBase::GetNextActive());
}

OptVar* OptVar::GetFirstActive()
{
    return static_cast<OptVar*>(VarBase::GetFirstActive(listHead));
}

void OptVar::SetWasActive()
{
    VarBase::SetWasActive(listHead);
}

int OptVar::GetnActive()
{
    return VarBase::GetnActive(listHead);
}


double OptVar::GetVarValue() const
{
    return optParEstimate;
}
double OptVar::GetMinVarValue() const
{
    return optMinLim;
}
double OptVar::GetMaxVarValue() const
{
    return optMaxLim;
}


void OptVar::GetActiveArray(OptVarArray& activeArray)
{
    // klugy approach
    VarBaseArray temp;
//  temp.GetActiveArray(listHead);
    temp.GetSortedArray(listHead);
    activeArray.CreateFrom(temp);
}


OptVarArray::OptVarArray()
{
    OptVar::GetActiveArray(*this);
}


void OptVarArray::CreateFrom(const VarBaseArray& a)
{
    AllocAndSetSize(a.Size());
    for (int i = 0; i < Size(); i++)
        (*this)[i] = static_cast<OptVar*>(a[i]);
}

