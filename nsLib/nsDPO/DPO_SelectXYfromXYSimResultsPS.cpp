///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectXYfromXYSimResultsPS.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "nsLibAfx.h"


#include "DPO_SelectXYfromXYSimResultsPS.h"

DPO_SelectXYfromXYSimResultsPS::DPO_SelectXYfromXYSimResultsPS() :
    xySimResultsObjRefUI(typeid(DO_XYSimResults))
{
}


void DPO_SelectXYfromXYSimResultsPS::SetAllSensitive()
{
    bool multSensitive = indexMSUI.isMultiple.GetValue();

    clearAllUI.SetSensitive(multSensitive);
    selectAllUI.SetSensitive(multSensitive);
    selectSameRunUI.SetSensitive(multSensitive);
    selectSameCaseUI.SetSensitive(multSensitive);

    xySelIndexUI.SetSensitive(listDataOK);
    DPO_SelectBasePS::SetAllSensitive();
}

void DPO_SelectXYfromXYSimResultsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_SelectXYfromXYSimResultsPS*>(inObj)->SetAllSensitive();
}


void DPO_SelectXYfromXYSimResultsPS::ClearAll()
{
    indexMSUI.indexListUI.ClearAll();
}

void DPO_SelectXYfromXYSimResultsPS::ClearAllCB(void* inObj)
{
    static_cast<DPO_SelectXYfromXYSimResultsPS*>(inObj)->ClearAll();
}

void DPO_SelectXYfromXYSimResultsPS::SelectAll()
{
    indexMSUI.indexListUI.SelectAll();
}

void DPO_SelectXYfromXYSimResultsPS::SelectAllCB(void* inObj)
{
    static_cast<DPO_SelectXYfromXYSimResultsPS*>(inObj)->SelectAll();
}


bool DPO_SelectXYfromXYSimResultsPS::SetCurrentIndexes()
{
    if (indexMSUI.indexListUI.currMultipleSelection.IsEmpty() || (!xySimResultsObjRefUI.CheckDataObj()))
        return false;

    dataDC = static_cast<DO_XYSimResults*>(xySimResultsObjRefUI.GetDataObj())->xyResults;

    int menuIndex = indexMSUI.indexListUI.currMultipleSelection[0];

    for (int i = 0; i < dataDC->Size(); i++)
    {
        currRunIndex = i;
        XYSimRunResults& currRun = (*dataDC)[i];

        int ncase = currRun.Size();
        if (menuIndex < ncase)
        {
            currCaseIndex = menuIndex;
            return true;
        }
        menuIndex -= ncase;
    }
    return false;
}

void DPO_SelectXYfromXYSimResultsPS::SetMatchIndexes(int runMatch, int caseMatch)
{
    SC_IntArray selections;
    // worst case
    selections.Alloc(dataDC->GetnCases());

    int selNum = 0;
    for (int i = 0; i < dataDC->Size(); i++)
    {
        XYSimRunResults& currRun = (*dataDC)[i];

        for (int j = 0; j < currRun.Size(); j++)
        {
            if ((runMatch == i) || (caseMatch == j))
                selections += selNum;
            selNum++;
        }
    }
    indexMSUI.indexListUI.SetMultipleSelection(selections);
}


void DPO_SelectXYfromXYSimResultsPS::SelectRun()
{
    if (SetCurrentIndexes())
        SetMatchIndexes(currRunIndex, -1);
}

void DPO_SelectXYfromXYSimResultsPS::SelectRunCB(void* inObj)
{
    static_cast<DPO_SelectXYfromXYSimResultsPS*>(inObj)->SelectRun();
}

void DPO_SelectXYfromXYSimResultsPS::SelectCase()
{
    if (SetCurrentIndexes())
        SetMatchIndexes(-1, currCaseIndex);
}

void DPO_SelectXYfromXYSimResultsPS::SelectCaseCB(void* inObj)
{
    static_cast<DPO_SelectXYfromXYSimResultsPS*>(inObj)->SelectCase();
}



void DPO_SelectXYfromXYSimResultsPS::SetIndexList()
{
    SC_StringArray dataIDs, xyIDs;
    if (xySimResultsObjRefUI.CheckDataObj())
    {
        dataDC = static_cast<DO_XYSimResults*>(xySimResultsObjRefUI.GetDataObj())->xyResults;
        dataDC->GetCaseIDs(dataIDs);
        dataDC->GetXYIDs(xyIDs);
    }

    listDataOK = !dataIDs.IsEmpty();
    if (listDataOK)
    {
        indexMSUI.indexListUI.SetListText(dataIDs);
        xySelIndexUI.SetListText(xyIDs);
    }
    else
    {
        indexMSUI.indexListUI.SetListNA();
        xySelIndexUI.SetListNA();
    }
    SetAllSensitive();

}

void DPO_SelectXYfromXYSimResultsPS::SetIndexListCB(void* inObj)
{
    static_cast<DPO_SelectXYfromXYSimResultsPS*>(inObj)->SetIndexList();
}


void DPO_SelectXYfromXYSimResultsPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    xySimResultsObjRefUI.InitFramedUI("nSIGHTS XY Results To Select From", URform.GetWidget(), args);
    xySimResultsObjRefUI.AddOtherObjectCallback(SetIndexListCB, this);

    args.Clear();
    args.StdFormAttach();
    xySelIndexUI.InitFramedUI("XY Data to Select", PPform.GetWidget(), args);


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

    args.Clear();
    args.StdTopObjectAttach(xySelIndexUI);
    args.BottomWidgetAttach(buttonForm.GetWidget());

    DPO_SelectBasePS::InitMenu(args, true, true);
    indexMSUI.isMultiple.AddOtherObjectCallback(SetAllSensitiveCB, this);
}


void DPO_SelectXYfromXYSimResultsPS::MenuOpen(DPO_SelectXYfromXYSimResults& inData)
{
    DPO_SelectBasePS::MenuOpen(inData);
    xySimResultsObjRefUI.StdMenuInit(inData.xySimResultsObjRef);
}


void DPO_SelectXYfromXYSimResultsPS::SetMenuData(DPO_SelectXYfromXYSimResults&  inData)
{
    xySimResultsObjRefUI.SetFuncObjRef(inData.xySimResultsObjRef);
    xySelIndexUI.SetSelection(inData.xySelIndex);

    DPO_SelectBasePS::SetMenuData(inData);

    SetIndexList();
    SetAllSensitive();

}

bool DPO_SelectXYfromXYSimResultsPS::UIdataOK()
{
    DPO_SelectBasePS::UIdataOK();
    xySimResultsObjRefUI.FuncObjRefOK(allOK, "Data source");

    if (allOK && (!indexMSUI.indexListUI.isSingleSel) && indexMSUI.indexListUI.currMultipleSelection.IsEmpty())
    {
        PropErrorMsg("no selections indicated");
        allOK = false;
    }

    return allOK;
}

void DPO_SelectXYfromXYSimResultsPS::GetMenuData(DPO_SelectXYfromXYSimResults&  outData)
{
    DPO_SelectBasePS::GetMenuData(outData);
    outData.xySimResultsObjRef = xySimResultsObjRefUI.GetFuncObjRef();
    outData.xySelIndex = xySelIndexUI.GetSelection();
}

