///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectOptPS.cpp
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
//      I/O and user interface for all osDFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"


#include "DPO_SelectOptPS.h"

DPO_SelectOptPS::DPO_SelectOptPS() :
    optSimResultsObjRefUI(typeid(DO_OptSimResults))
{
    showOKonly = false;
    singleOK = true;
    multipleOK = true;
}

void DPO_SelectOptPS::SetAllSensitive()
{
    if (multipleOK)
    {
        bool multSensitive = true;
        if (singleOK)
            multSensitive = indexMSUI.isMultiple.GetValue();

        clearAllUI.SetSensitive(multSensitive);
        selectAllUI.SetSensitive(multSensitive);
        selectSameRunUI.SetSensitive(multSensitive);
        selectSameCaseUI.SetSensitive(multSensitive);
        selectSameFitUI.SetSensitive(multSensitive);
    }

    indexMSUI.indexListUI.SetSensitive(listDataOK);
}

void DPO_SelectOptPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_SelectOptPS*>(inObj)->SetAllSensitive();
}

void DPO_SelectOptPS::SetIndexList()
{
    SC_StringArray dataIDs;
    if (optSimResultsObjRefUI.CheckDataObj())
    {
        dataDC = static_cast<DO_OptSimResults*>(optSimResultsObjRefUI.GetDataObj())->optResults;
        dataDC->GetMenuStrings(dataIDs, showFitComponents, showOKonly);
    }

    listDataOK = !dataIDs.IsEmpty();
    if (listDataOK)
    {
        indexMSUI.indexListUI.SetListText(dataIDs);
    }
    else
    {
        indexMSUI.indexListUI.SetListNA();
    }
    SetAllSensitive();
}

void DPO_SelectOptPS::SetIndexListCB(void* inObj)
{
    static_cast<DPO_SelectOptPS*>(inObj)->SetIndexList();
}



void DPO_SelectOptPS::ClearAll()
{
    indexMSUI.indexListUI.ClearAll();
}

void DPO_SelectOptPS::ClearAllCB(void* inObj)
{
    static_cast<DPO_SelectOptPS*>(inObj)->ClearAll();
}

void DPO_SelectOptPS::SelectAll()
{
    indexMSUI.indexListUI.SelectAll();
}

void DPO_SelectOptPS::SelectAllCB(void* inObj)
{
    static_cast<DPO_SelectOptPS*>(inObj)->SelectAll();
}


bool DPO_SelectOptPS::SetCurrentIndexes()
{
    if (indexMSUI.indexListUI.currMultipleSelection.IsEmpty() || (!optSimResultsObjRefUI.CheckDataObj()))
        return false;

    dataDC = static_cast<DO_OptSimResults*>(optSimResultsObjRefUI.GetDataObj())->optResults;

    int menuIndex = indexMSUI.indexListUI.currMultipleSelection[0];

    for (int i = 0; i < dataDC->Size(); i++)
    {
        currRunIndex = i;
        OptCaseResultsArray& currCases = (*dataDC)[i].runResults;
        for (int j = 0; j < currCases.Size(); j++)
        {
            currCaseIndex = j;
            OptSingleFitArray&   currFits = currCases[j].caseResults;
            int nfit = currFits.Size();
            if (showFitComponents)
            {
                for (int k = 0; k < nfit; k++)
                {
                    if ((!showOKonly) || (currFits[k].fitCompletedOK))
                    {
                        currFitIndex = k;
                        int nsubFit = currFits[k].fitComponents.Size();
                        if (menuIndex < nsubFit)
                            return true;
                        menuIndex -= nsubFit;
                    }
                }
            }
            else
            {
                if (showOKonly)
                {
                    int nOK = 0;
                    for (int k = 0; k < nfit; k++)
                        if (currFits[k].fitCompletedOK)
                            nOK++;
                    nfit = nOK;
                }

                if (menuIndex < nfit)
                {
                    currFitIndex = menuIndex;
                    return true;
                }
                menuIndex -= nfit;
            }
        }
    }
    return false;
}

void DPO_SelectOptPS::SetMatchIndexes(int runMatch, int caseMatch, int fitMatch)
{
    SC_IntArray selections;
    // worst case
    selections.Alloc(dataDC->GetnFitComponents());

    int selNum = 0;
    for (int i = 0; i < dataDC->Size(); i++)
    {
        OptCaseResultsArray& currCases = (*dataDC)[i].runResults;
        for (int j = 0; j < currCases.Size(); j++)
        {
            OptSingleFitArray&   currFits = currCases[j].caseResults;
            int nfit = currFits.Size();
            if (showFitComponents)
            {
                OptSingleFitArray&   currFits = currCases[j].caseResults;
                for (int k = 0; k < nfit; k++)
                {
                    if ((!showOKonly) || (currFits[k].fitCompletedOK))
                    {
                        int nsubFit = currFits[k].fitComponents.Size();
                        for (int l = 0; l < nsubFit; l++)
                        {
                            if ((runMatch == i) || (caseMatch == j) || (fitMatch == k))
                                selections += selNum;
                            selNum++;
                        }
                    }
                }
            }
            else
            {
                for (int k = 0; k < nfit; k++)
                {
                    if ((!showOKonly) || (currFits[k].fitCompletedOK))
                    {
                        if ((runMatch == i) || (caseMatch == j) || (fitMatch == k))
                            selections += selNum;
                        selNum++;
                    }
                }
            }
        }
    }
    indexMSUI.indexListUI.SetMultipleSelection(selections);
}


void DPO_SelectOptPS::SelectRun()
{
    if (SetCurrentIndexes())
        SetMatchIndexes(currRunIndex, -1, -1);
}

void DPO_SelectOptPS::SelectRunCB(void* inObj)
{
    static_cast<DPO_SelectOptPS*>(inObj)->SelectRun();
}

void DPO_SelectOptPS::SelectCase()
{
    if (SetCurrentIndexes())
        SetMatchIndexes(-1, currCaseIndex, -1);
}

void DPO_SelectOptPS::SelectCaseCB(void* inObj)
{
    static_cast<DPO_SelectOptPS*>(inObj)->SelectCase();
}

void DPO_SelectOptPS::SelectFit()
{
    if (SetCurrentIndexes())
        SetMatchIndexes(-1, -1, currFitIndex);
}

void DPO_SelectOptPS::SelectFitCB(void* inObj)
{
    static_cast<DPO_SelectOptPS*>(inObj)->SelectFit();
}

void DPO_SelectOptPS::InitMenu(ArgListC& topAttach, bool snglOK, bool multOK)
{
    ArgListC args;
    args.AllFormAttach();
    optSimResultsObjRefUI.InitFramedUI("nSIGHTS Opt Results To Select From", URform.GetWidget(), args);
    optSimResultsObjRefUI.AddOtherObjectCallback(SetIndexListCB, this);

    topAttach.RightFormAttach();
    topAttach.LeftFormAttach();

    singleOK = snglOK;
    multipleOK = multOK;
    if (multipleOK)
    {
        args.Clear();
        args.LeftFormAttach();
        args.RightFormAttach();
        args.BottomFormAttach();
        buttonForm.InitWidget(PPform.GetWidget(), args);

        args.Clear();
        args.TopFormAttach();
        args.BottomFormAttach();
        args.AddArg(Arg::XmNwidth,  55);
        args.SetPlaceHolder();
        args.LeftPosAttach(5);
        clearAllUI.StdButtonInit("None", buttonForm.GetWidget(), args);
        clearAllUI.AddOtherObjectCallback(ClearAllCB, this);

        args.ResetPlace();
        args.LeftPosAttach(25);
        selectAllUI.StdButtonInit("All", buttonForm.GetWidget(), args);
        selectAllUI.AddOtherObjectCallback(SelectAllCB, this);

        args.ResetPlace();
        args.LeftPosAttach(45);
        selectSameRunUI.StdButtonInit("Run", buttonForm.GetWidget(), args);
        selectSameRunUI.AddOtherObjectCallback(SelectRunCB, this);

        args.ResetPlace();
        args.LeftPosAttach(65);
        selectSameCaseUI.StdButtonInit("Case", buttonForm.GetWidget(), args);
        selectSameCaseUI.AddOtherObjectCallback(SelectCaseCB, this);

        args.ResetPlace();
        args.LeftPosAttach(85);
        selectSameFitUI.StdButtonInit("Fit", buttonForm.GetWidget(), args);
        selectSameFitUI.AddOtherObjectCallback(SelectFitCB, this);

        topAttach.BottomWidgetAttach(buttonForm.GetWidget());
    }
    else
        topAttach.BottomFormAttach();

    DPO_SelectBasePS::InitMenu(topAttach, snglOK, multOK);

    if (snglOK && multOK)
        indexMSUI.isMultiple.AddOtherObjectCallback(SetAllSensitiveCB, this);

}

void DPO_SelectOptPS::MenuOpen(DPO_SelectOpt& inData)
{
    DPO_SelectBasePS::MenuOpen(inData);
    optSimResultsObjRefUI.StdMenuInit(inData.currOptFO.optSimResultsObjRef);
}


void DPO_SelectOptPS::SetMenuData(DPO_SelectOpt&  inData)
{
    optSimResultsObjRefUI.SetFuncObjRef(inData.currOptFO.optSimResultsObjRef);
    showFitComponents = inData.currOptFO.includeSubFits;

    DPO_SelectBasePS::SetMenuData(inData);

    SetIndexList();

    SetAllSensitive();
}

bool DPO_SelectOptPS::UIdataOK()
{
    DPO_SelectBasePS::UIdataOK();
    optSimResultsObjRefUI.FuncObjRefOK(allOK, "Data source");

    if (allOK && (!indexMSUI.indexListUI.isSingleSel) && indexMSUI.indexListUI.currMultipleSelection.IsEmpty())
    {
        PropErrorMsg("no selections indicated");
        allOK = false;
    }

    return allOK;
}

void DPO_SelectOptPS::GetMenuData(DPO_SelectOpt&  outData)
{
    DPO_SelectBasePS::GetMenuData(outData);
    outData.currOptFO.optSimResultsObjRef = optSimResultsObjRefUI.GetFuncObjRef();
}

