///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectOpt.cpp
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

#include <osDFO/DFO_SelectOpt.h>


DFO_SelectOpt :: DFO_SelectOpt(const char* inID, bool snglOK, bool multOK) :
    DFO_SelectBase(inID, baseMS),  baseMS(snglOK, multOK)
{
    includeSubFits = false;
    selectOKonly = false;
    InitCommon();
}

DFO_SelectOpt::DFO_SelectOpt(const DFO_SelectOpt& a) :
    DFO_SelectBase(a, baseMS), baseMS(a.baseMS)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectOpt::InitCommon()
{
    InitMSPorts();
    AddInPort(optSimResultsObjRef, typeid(DO_OptSimResults));
}

DFO_SelectOpt& DFO_SelectOpt::operator= (const DFO_SelectOpt& a)
{
    if (&a != this)
    {
        DFO_SelectBase::operator=(a);
        baseMS = a.baseMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectOpt::LocalCopy(const DFO_SelectOpt& a)
{
    optSimResultsObjRef  = a.optSimResultsObjRef;
    includeSubFits       = a.includeSubFits;
    selectOKonly         = a.selectOKonly;
}

void  DFO_SelectOpt::DoStatusChk()
{
    DFO_SelectBase::DoStatusChk();

    //  clear current selections
    runIndex.SetEmpty();
    caseIndex.SetEmpty();
    fitIndex.SetEmpty();
    subfitIndex.SetEmpty();

    //  check input objects
    if (!CheckInputFO(optSimResultsObjRef, "Data source"))
        return;

    optSimResultsDC = static_cast<DO_OptSimResults*>(GetInPortData(optSimResultsObjRef))->optResults;


    int nSel = 1;
    if (!baseMS.CurrentIndexIsSingle())
        nSel = baseMS.GetCurrentIndexArray().Size();

    runIndex.Alloc(nSel);
    caseIndex.Alloc(nSel);
    fitIndex.Alloc(nSel);
    subfitIndex.Alloc(nSel);

    if (baseMS.CurrentIndexIsSingle())
    {
        AddIndex(indexMS.GetCurrentIndex());
    }
    else
    {
        const SC_IntArray& tmp = baseMS.GetCurrentIndexArray();
        for (int i = 0; i < nSel; i++)
            AddIndex(tmp[i]);
    }

    if (runIndex.IsEmpty())
        SetObjErrMsg("no selection(s) in range ???");

}

void DFO_SelectOpt::AddIndex(int menuIndex)
{
    SC_IntArray okMap;
    okMap.SetResizable(10);

    for (int i = 0; i < optSimResultsDC->Size(); i++)
    {
        OptCaseResultsArray& currCases = (*optSimResultsDC)[i].runResults;
        for (int j = 0; j < currCases.Size(); j++)
        {
            OptSingleFitArray&   currFits = currCases[j].caseResults;
            int nfit = currFits.Size();
            if (includeSubFits)
            {
                for (int k = 0; k < nfit; k++)
                {
                    if ((!selectOKonly) || (currFits[k].fitCompletedOK))
                    {
                        int nsubFit = currFits[k].fitComponents.Size();
                        if (menuIndex < nsubFit)
                        {
                            DoAdd(i, j, k, menuIndex);
                            return;
                        }
                        menuIndex -= nsubFit;
                    }
                }
            }
            else
            {
                if (selectOKonly)
                {
                    okMap.SetEmpty();
                    int nOK = 0;
                    for (int k = 0; k < nfit; k++)
                        if (currFits[k].fitCompletedOK)
                        {
                            okMap += k;
                            nOK++;
                        }
                    nfit = nOK;
                    if (menuIndex < nfit)
                    {
                        DoAdd(i, j, okMap[menuIndex], 0);
                        return;
                    }
                }
                else
                    if (menuIndex < nfit)
                    {
                        DoAdd(i, j, menuIndex, 0);
                        return;
                    }
                menuIndex -= nfit;
            }
        }
    }
}


void DFO_SelectOpt::DoAdd(int runI, int caseI, int fitI, int sfitI)
{
    runIndex    += runI;
    caseIndex   += caseI;
    fitIndex    += fitI;;
    subfitIndex += sfitI;
}


void DFO_SelectOpt:: CalcOutput(FOcalcType  calcType)
{
    if (StatusNotOK())
        return;

    // nothing happens here yet

}

