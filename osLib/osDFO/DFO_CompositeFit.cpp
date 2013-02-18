///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CompositeFit.cpp
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
//      combines the results from multiple single fits (and/or other composite fits).
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <math.h>


#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osDFO/DFO_CompositeFit.h>

#include <iostream>

DFO_CompositeFit::FitData::FitData() :
    useFit(false),
    fitScaling(1.0),
    logFit(false)
{
}

DFO_CompositeFit::FitData::FitData(const FitData& a)
{
    LocalCopy(a);
}
DFO_CompositeFit::FitData& DFO_CompositeFit::FitData::operator= (const FitData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void DFO_CompositeFit::FitData::LocalCopy(const FitData& a)
{
    useFit          = a.useFit    ;
    inputFitObjRef  = a.inputFitObjRef;
    fitScaling      = a.fitScaling;
    logFit          = a.logFit    ;
}

DFO_CompositeFit :: DFO_CompositeFit() : DFO_FitBase("CompositeFit")
{
    InitCommon();
}

DFO_CompositeFit::DFO_CompositeFit(const DFO_CompositeFit& a) : DFO_FitBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_CompositeFit :: ~DFO_CompositeFit ()
{
}

void DFO_CompositeFit::InitCommon()
{
    recursiveAlloc = false;
    for (int i = 0; i < maxFitData; i++)
        AddUseFlagInPort(fitData[i].inputFitObjRef, typeid(DO_FitResults), fitData[i].useFit);
    DoStatusChk();
}

DFO_CompositeFit& DFO_CompositeFit::operator= (const DFO_CompositeFit& a)
{
    if (&a != this)
    {
        DFO_FitBase::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_CompositeFit::LocalCopy(const DFO_CompositeFit& a)
{
    for (int i = 0; i < maxFitData; i++)
        fitData[i] = a.fitData[i];
}

void DFO_CompositeFit::DoAllocCheck()
{
    if (recursiveAlloc)
    {
        SetObjErrMsg("allocation recursion ???");
        return;
    }

    recursiveAlloc = true;
    for (int i = 0; i < maxFitData; i++)
    {
        FitData& currFit = fitData[i];
        if (currFit.useFit)
        {
            if (CheckInputFO(currFit.inputFitObjRef, "Input fit"))
            {
                // propagate back up (possible) tree
                static_cast<DFO_FitBase*>(currFit.inputFitObjRef.objRef)->DoAllocCheck();
            }
            else
            {
                // try and get a better error message
                if (!currFit.inputFitObjRef.IsNull())
                {

                    char errMsg[255];
                    MakeString(errMsg, "CompFitErr:", currFit.inputFitObjRef.objRef->GetID(), ":", currFit.inputFitObjRef.objRef->GetStatusMsg(), 255);
                    SetObjErrMsg(errMsg);
                }
            }
        }
    }

    // assumes no throwing .....
    recursiveAlloc = false;
}

void DFO_CompositeFit::GetFitScaling(SC_DoubleArray& scaleData)
{
    scaleData.Alloc(maxFitData);
    for (int i = 0; i < maxFitData; i++)
        if (fitData[i].useFit)
            scaleData += fitData[i].fitScaling;
}


void  DFO_CompositeFit::DoStatusChk()
{
    FuncObjC::DoStatusChk();

    fitResultsDO.Reset("Composite fit");
    fitResultsDO.fitResults.SetEmpty();

    bool fitFound = false;
    for (int i = 0; i < maxFitData; i++)
        // if it isn't null its attached to a fit and all fits have OK fit results vectors
        if ((fitData[i].useFit) && (!fitData[i].inputFitObjRef.IsNull()))
        {
            fitFound = true;
            FitResultsPtrArray& inputFits = static_cast<DO_FitResults*>(GetInPortData(fitData[i].inputFitObjRef))->fitResults;
            //  array concat
            fitResultsDO.fitResults += inputFits;
        }

    if (StatusNotOK())
        return;

    if (!fitFound)
        SetObjErrMsg("no input fit specified");
    else if (fitResultsDO.fitResults.IsEmpty())
        SetObjErrMsg("input fits are all empty");
    else
        DoAllocCheck();
}


void DFO_CompositeFit:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    double fitVal = 0.0;
    for (int i = 0; i < maxFitData; i++)
        if (fitData[i].useFit)
        {
            double inputFitVal = static_cast<DO_FitResults*>(GetInPortData(fitData[i].inputFitObjRef))->realVal;
            if (!RequiresChisq())
            {
                if (fitData[i].logFit)
                    inputFitVal = log10(inputFitVal);
                inputFitVal *= fitData[i].fitScaling;
            }
            fitVal += inputFitVal;
        }

    fitResultsDO.InitLabelAndValue(fitVal);
}

