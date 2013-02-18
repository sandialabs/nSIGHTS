///////////////////////////////////////////////////////////////////////////////////
//
// G_OptRange.cpp
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
//      support for combined multiple value/optimization runs.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <iostream>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <osDFO/DFO_FitBase.h>
#include <osMain/C_Optimize.h>

#include <osMain/G_Optimize.h>
#include <osMain/G_Vary.h>

#include <osMain/G_OptRange.h>


namespace osRange  {


OptRangeOutputFO    optRangeOutputFO;


OptRangeOutputFO::OptRangeOutputFO() :
    RangeOutputFO("OptRangeOutput")
{
    currFits.Alloc(OptimizeInput::maxOptFitSel);
    currFitOutput.Alloc(OptimizeInput::maxOptFitSel);

    DoStatusChk();
}



bool OptRangeOutputFO::AllocOutputIDs(bool optRun)
{
    if (!AllocSetup())
        return false;

    int nsubFits = 0;
    currFits.SetEmpty();
    currFitOutput.SetEmpty();
    int i;
    for (i = 0; i < OptimizeInput::maxOptFitSel; i++)
        if (optimizeInput.inputFits[i].optimizeOnFit)
        {
            DFO_FitBase* currFit = static_cast<DFO_FitBase*>(optimizeInput.inputFits[i].fitObject.objRef);
            if (currFit)
            {
                currFits += currFit;
                currFitOutput += &(currFit->GetFitReal());
                int nComp = currFit->GetFitResults().Size();
                if (nComp > 1)
                    nsubFits += nComp;
            }
        }

    if (currFits.IsEmpty())
    {
        SetToInitial();
        return false;
    }

    int nFits = currFits.Size();


    int nAlloc = nFits;

    OptVarArray     optVars;
    int nOpt = 0;
    int nCovar = 0;
    if (optRun)
    {
        nOpt = optVars.Size();

    //MX    if (OptVar::RequiresChisq())
            nCovar = nOpt;  // main diagonal only

        nAlloc += nFits * (1 + nOpt + nCovar) + nsubFits * (nCovar + 1);
    }

    // reAlloc required ?
    if (nAlloc != gridOutputDC.Size())
    {
        DeAllocDC();
        gridOutputDC.AllocAndSetSize(nAlloc);
        cubeOutputDC.AllocAndSetSize(nAlloc);
    }

    // reset IDs
    int nextID = 0;
    char tempStr[80];
    for (i = 0; i < nFits; i++)
    {
        // fit value
        MakeString(tempStr, currFits[i]->GetID(), ":result", 80);
        SetDCID(tempStr, nextID);
        if (optRun)
        {
            // fit OK
            MakeString(tempStr, currFits[i]->GetID(), ":OK", 80);
            SetDCID(tempStr, nextID);

            int j;
            // opt value for fit
            for (j = 0; j < nOpt; j++)
            {
                MakeString(tempStr, currFits[i]->GetID(), ":", optVars[j]->GetShortID(), 80);
                SetDCID(tempStr, nextID);
            }

            // opt covar (main diagonal only)
            for (j = 0; j < nCovar; j++)
            {
                MakeString(tempStr, currFits[i]->GetID(), ":C[", optVars[j]->GetShortID(), "]", 80);
                SetDCID(tempStr, nextID);
            }

            // any fit components ?
            FitResultsPtrArray&  fitComp = currFits[i]->GetFitResults();
            int nComp = fitComp.Size();
            if (nComp < 2)
                continue;

            for (j = 0; j < nComp; j++)
            {
                MakeString(tempStr, currFits[i]->GetID(), ":", fitComp[j]->fitID,80);

                char covStr[80];
                MakeString(covStr, tempStr, ":result", 80);
                SetDCID(covStr, nextID);

                // opt covar (main diagonal only)
                for (int k = 0; k < nCovar; k++)
                {
                    MakeString(covStr, tempStr, ":C[", optVars[k]->GetShortID(), "]", 80);
                    SetDCID(covStr, nextID);
                }
            }
        }
    }

    DoStatusChk();
    return true;
}



bool OptRangeOutputFO::PreCalcSetup(bool optRun)
{
    GlobalFunc::PreCalcSetup();
    if (!vary.VaryOK())
        return false;

    if (!AllocOutputIDs(optRun))
        return false;

    if (!RangeOutputFO::PreCalcSetup())
        return false;

    DFO_FitBase::useFitsForOptimization = optRun;

    return true;
}


void OptRangeOutputFO::DoStatusChk()
{
    RangeOutputFO::DoStatusChk();
}

void OptRangeOutputFO::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}


void OptRangeOutputFO::AddForwardRun()
{
    DoStatusChk();
    if (!StatusOK())
        GenAppInternalError("OptRangeOutputFO::CalcOutput");

    SetIndexes();
    if (doingCubes)
    {
        for (int i = 0; i < currFits.Size(); i++)
            cubeOutputDC[i].cData[xIndex][yIndex][zIndex] = currFitOutput[i]->realVal;
    }
    else
    {
        for (int i = 0; i < currFits.Size(); i++)
            gridOutputDC[i].gData[xIndex][yIndex] = currFitOutput[i]->realVal;
    }
}


void OptRangeOutputFO::FirstOptRunSetup()
{
    DoStatusChk();
    if (!StatusOK())
        GenAppInternalError("OptRangeOutputFO::CalcOutput");

    rangeOutputIndex = 0;
    SetIndexes();
}


void OptRangeOutputFO::AddOptRun(const Optimizer& opt, bool runOK)
{
    // first the main fit
    SetNextRangeValue(optimizeInput.GetFitValue());

    if (runOK)
        SetNextRangeValue(1.0);
    else
        SetNextRangeValue(0.0);

    // and the results
    int i;
    for (i = 0; i < opt.noptVars; i++)
        SetNextRangeValue(opt.bestParEst[i] * opt.varsToOptimize[i]->GetFinalMultiplier());

    // and the covar
    if (OptVar::RequiresChisq())
    {
        for (i = 0; i < opt.noptVars; i++)
            SetNextRangeValue(opt.optActualCovar[i][i]);
    }

    // and components
    DFO_FitBase& currFit = *(static_cast<DFO_FitBase*>(optimizeInput.inputFits[optimizeInput.currSelectedFit].fitObject.objRef));
    FitResultsPtrArray& fitComp = currFit.GetFitResults();

    if (fitComp.Size() < 2)
        return;


    for (i = 0; i < fitComp.Size(); i++)
    {
        DC_FitResults& currCompDC = (*fitComp[i]);

        SetNextRangeValue(currCompDC.fitValue);

        if (OptVar::RequiresChisq())
        {
            for (int j = 0; j < opt.noptVars; j++)
                SetNextRangeValue(opt.multFitHessianMatrix[i][j][j]);  //MX

        }
    }
}





};

