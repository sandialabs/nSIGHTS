///////////////////////////////////////////////////////////////////////////////////
//
// DC_FitResults.cpp
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
//      optimizer results (fit value, residuals) for a single fit.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osClass/DC_FitResults.h>

DC_FitResults::DC_FitResults()
{
    sigma = 1.0;  // for chisq fits
    fitWasL1 = false;
    fitWasNormed = false;
    okToAlloc = false;
    nfitAlloc = -1;
    nfitPoints = -1;
    CopyString(fitID, "Fit", fitIDLen);
    fitResiduals.SetID("Resid");
}

DC_FitResults::~DC_FitResults()
{
    // empty
}

bool  DC_FitResults::PreRunAlloc(bool allocDeriv)
{
    if (!okToAlloc)
        return false;

    fitDerivatives.DeAlloc();
    fitResidualDerivatives.DeAlloc();
    try
    {
        fitResults.Alloc(nfitAlloc);
        fitResiduals.Alloc(nfitAlloc);


//        if (allocDeriv && RequiresChisq())
//        {
            fitDerivatives.AllocAndSetSize(nfitAlloc);
            fitResidualDerivatives.AllocAndSetSize(nfitAlloc);
 //       }
        return true;
    }
    catch  (SC_Array::AllocationError )  {
        fitDerivatives.DeAlloc();
        fitResidualDerivatives.DeAlloc();
    }
    return false;
}


DC_FitResults&  DC_FitResults::operator= (const DC_FitResults& a)
{
    if ( this != &a)
        LocalCopy(a);
    return *this;
}

void DC_FitResults::LocalCopy(const DC_FitResults& a)
{
    CopyString(fitID, a.fitID, fitIDLen);
    fitResiduals = a.fitResiduals;
    fitDerivatives = a.fitDerivatives;
    currPerturbIndex = a.currPerturbIndex;
    derivativeTolerance = a.derivativeTolerance;
    doOptPerturbations = a.doOptPerturbations;
    estVar = a.estVar;
    firstStageOptType = a.firstStageOptType;
    fitResidualDerivatives = a.fitResidualDerivatives;
    fitResults = a.fitResults;
    fitTolerance = a.fitTolerance;
    fitValue = a.fitValue;
    fitWasL1 = a.fitWasL1;
    fitWasNormed = a.fitWasNormed;
    fixedDerivSpan = a.fixedDerivSpan;
    initialDerivSpan = a.initialDerivSpan;
    lmAdjustDerivSpan = a.lmAdjustDerivSpan;
    lmInitialDerivSpan = a.lmInitialDerivSpan;
    lmInitialLambda = a.lmInitialLambda;
    lmLambdaFactor = a.lmLambdaFactor;
    lmMinLambda = a.lmMinLambda;
    lmRelChangeTolerance = a.lmRelChangeTolerance;
    maxOptRun = a.maxOptRun;
    nfitAlloc = a.nfitAlloc;
    sse = a.sse;
    nfitPoints = a.nfitPoints;
    sigma = a.sigma;
}

