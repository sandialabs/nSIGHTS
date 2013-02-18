///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectExtendedProfilePS.cpp
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

#include "DPO_SelectExtendedProfilePS.h"

DPO_SelectExtendedProfilePS::DPO_SelectExtendedProfilePS() :
    extendedProfileCaseObjRefUI(typeid(DO_ExtendedProfileCase))
{
}


void DPO_SelectExtendedProfilePS::SetIndexList()
{
    SC_StringArray dataIDs;
    if (extendedProfileCaseObjRefUI.CheckDataObj())
    {
        const ExtendedProfile*  dataDC = static_cast<DO_ExtendedProfileCase*>(extendedProfileCaseObjRefUI.GetDataObj())->profileCaseResults;
        dataDC->GetTimeSteps(dataIDs);
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

void DPO_SelectExtendedProfilePS::SetIndexListCB(void* inObj)
{
    static_cast<DPO_SelectExtendedProfilePS*>(inObj)->SetIndexList();
}


void DPO_SelectExtendedProfilePS::InitMenu()
{
    DPO_SelectBasePS::InitMenu(true, false);

    ArgListC args;
    args.AllFormAttach();
    extendedProfileCaseObjRefUI.InitFramedUI("Profile Case To Select From", URform.GetWidget(), args);
    extendedProfileCaseObjRefUI.AddOtherObjectCallback(SetIndexListCB, this);

}

void DPO_SelectExtendedProfilePS::MenuOpen(DPO_SelectExtendedProfile& inData)
{
    DPO_SelectBasePS::MenuOpen(inData);
    extendedProfileCaseObjRefUI.StdMenuInit(inData.extendedProfileCaseObjRef);
}


void DPO_SelectExtendedProfilePS::SetMenuData(DPO_SelectExtendedProfile&  inData)
{
    extendedProfileCaseObjRefUI.SetFuncObjRef(inData.extendedProfileCaseObjRef);

    DPO_SelectBasePS::SetMenuData(inData);

    SetIndexList();
}

bool DPO_SelectExtendedProfilePS::UIdataOK()
{
    DPO_SelectBasePS::UIdataOK();
    extendedProfileCaseObjRefUI.FuncObjRefOK(allOK, "Data source");
    return allOK;
}

void DPO_SelectExtendedProfilePS::GetMenuData(DPO_SelectExtendedProfile&  outData)
{
    DPO_SelectBasePS::GetMenuData(outData);
    outData.extendedProfileCaseObjRef = extendedProfileCaseObjRefUI.GetFuncObjRef();
}

