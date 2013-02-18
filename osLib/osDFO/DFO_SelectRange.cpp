///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectRange.cpp
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
//      For calculating fits, and for reading, selecting, and operating on optimizer results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>

#include <osDFO/DFO_SelectRange.h>


DFO_SelectRange :: DFO_SelectRange(const char* inID) :
    DFO_SelectBase(inID, baseMS)
{
    InitCommon();
}

DFO_SelectRange::DFO_SelectRange(const DFO_SelectRange& a) :
    DFO_SelectBase(a, baseMS), baseMS(a.baseMS)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectRange::InitCommon()
{
    InitMSPorts();
    AddInPort(rangeSimResultsObjRef, typeid(DO_RangeSimResults));
}

DFO_SelectRange& DFO_SelectRange::operator= (const DFO_SelectRange& a)
{
    if (&a != this)
    {
        DFO_SelectBase::operator=(a);
        baseMS = a.baseMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectRange::LocalCopy(const DFO_SelectRange& a)
{
    rangeSimResultsObjRef  = a.rangeSimResultsObjRef;
}

void  DFO_SelectRange::DoStatusChk()
{
    DFO_SelectBase::DoStatusChk();

    //  clear current selections
    runIndex = -1;
    outputIndex = -1;

    //  check input objects
    if (!CheckInputFO(rangeSimResultsObjRef, "Data source"))
        return;

    rangeSimResultsDC = static_cast<DO_RangeSimResults*>(GetInPortData(rangeSimResultsObjRef))->rangeResults;

    AddIndex(indexMS.GetCurrentIndex());

    if (runIndex < 0)
        SetObjErrMsg("no selection(s) in range ???");

}

void DFO_SelectRange::AddIndex(int menuIndex)
{
    for (int i = 0; i < rangeSimResultsDC->Size(); i++)
    {
        int nOutput = (*rangeSimResultsDC)[i].Size();
        if (menuIndex < nOutput)
        {
            runIndex = i;
            outputIndex = menuIndex;
            return;
        }
        menuIndex -= nOutput;
    }
}



void DFO_SelectRange:: CalcOutput(FOcalcType  calcType)
{
    if (StatusNotOK())
        return;

    // nothing happens here yet

}

