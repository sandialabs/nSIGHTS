///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectProfilefromProfileSimResultsPS.cpp
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

#include "DPO_SelectProfilefromProfileSimResultsPS.h"

DPO_SelectProfilefromProfileSimResultsPS::DPO_SelectProfilefromProfileSimResultsPS() :
    profileSimResultsObjRefUI(typeid(DO_ProfileSimResults))
{
}


void DPO_SelectProfilefromProfileSimResultsPS::SetIndexList()
{
    SC_StringArray dataIDs;
    if (profileSimResultsObjRefUI.CheckDataObj())
    {
        DC_ProfileSimResults* dataDC = static_cast<DO_ProfileSimResults*>(profileSimResultsObjRefUI.GetDataObj())->profileResults;
        dataDC->GetCaseIDs(dataIDs);
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

void DPO_SelectProfilefromProfileSimResultsPS::SetIndexListCB(void* inObj)
{
    static_cast<DPO_SelectProfilefromProfileSimResultsPS*>(inObj)->SetIndexList();
}


void DPO_SelectProfilefromProfileSimResultsPS::InitMenu()
{
    DPO_SelectBasePS::InitMenu(true, false);

    ArgListC args;
    args.AllFormAttach();
    profileSimResultsObjRefUI.InitFramedUI("nSIGHTS Profile Results To Select From", URform.GetWidget(), args);
    profileSimResultsObjRefUI.AddOtherObjectCallback(SetIndexListCB, this);

}

void DPO_SelectProfilefromProfileSimResultsPS::MenuOpen(DPO_SelectProfilefromProfileSimResults& inData)
{
    DPO_SelectBasePS::MenuOpen(inData);
    profileSimResultsObjRefUI.StdMenuInit(inData.profileSimResultsObjRef);
}


void DPO_SelectProfilefromProfileSimResultsPS::SetMenuData(DPO_SelectProfilefromProfileSimResults&  inData)
{
    profileSimResultsObjRefUI.SetFuncObjRef(inData.profileSimResultsObjRef);

    DPO_SelectBasePS::SetMenuData(inData);

    SetIndexList();
}

bool DPO_SelectProfilefromProfileSimResultsPS::UIdataOK()
{
    DPO_SelectBasePS::UIdataOK();
    profileSimResultsObjRefUI.FuncObjRefOK(allOK, "Data source");
    return allOK;
}

void DPO_SelectProfilefromProfileSimResultsPS::GetMenuData(DPO_SelectProfilefromProfileSimResults&  outData)
{
    DPO_SelectBasePS::GetMenuData(outData);
    outData.profileSimResultsObjRef = profileSimResultsObjRefUI.GetFuncObjRef();
}

