///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectVariableArrayPS.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClassPS/PS_UIBase.h>
#include <objClass/DO_VariableArray.h>

#include "DPO_SelectVariableArrayPS.h"

DPO_SelectVariableArrayPS::DPO_SelectVariableArrayPS() :
    varDataArrayObjRefUI(typeid(DO_VariableArray))
{
}


void DPO_SelectVariableArrayPS::SetIndexList()
{
    SC_StringArray dataIDs;
    if (varDataArrayObjRefUI.CheckDataObj())
    {
        static_cast<DO_VariableArray*>(varDataArrayObjRefUI.GetDataObj())->GetIDs(dataIDs);
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

void DPO_SelectVariableArrayPS::SetIndexListCB(void* inObj)
{
    static_cast<DPO_SelectVariableArrayPS*>(inObj)->SetIndexList();
}


void DPO_SelectVariableArrayPS::InitMenu()
{
    DPO_SelectBasePS::InitMenu(false, true);

    indexMSUI.indexListUI.SetSelectionPolicy(false);

    ArgListC args;
    args.AllFormAttach();
    varDataArrayObjRefUI.InitFramedUI("Variable Array To Select From", URform.GetWidget(), args);
    varDataArrayObjRefUI.AddOtherObjectCallback(SetIndexListCB, this);

}

void DPO_SelectVariableArrayPS::MenuOpen(DPO_SelectVariableArray&   inData)
{
    MenuBaseC::MenuOpen(true, true);
    indexMSUI.MenuInit(inData.indexMS);
//  DPO_SelectBasePS::MenuOpen(inData);
    varDataArrayObjRefUI.StdMenuInit(inData.varDataArrayObjRef);
}


void DPO_SelectVariableArrayPS::SetMenuData(DPO_SelectVariableArray&    inData)
{
    varDataArrayObjRefUI.SetFuncObjRef(inData.varDataArrayObjRef);

    DPO_SelectBasePS::SetMenuData(inData);

    SetIndexList();
}

bool DPO_SelectVariableArrayPS::UIdataOK()
{
    DPO_SelectBasePS::UIdataOK();
    varDataArrayObjRefUI.FuncObjRefOK(allOK, "Data source");
    return allOK;
}

void DPO_SelectVariableArrayPS::GetMenuData(DPO_SelectVariableArray&  outData)
{
    DPO_SelectBasePS::GetMenuData(outData);
    outData.varDataArrayObjRef = varDataArrayObjRefUI.GetFuncObjRef();
}

void DPO_SelectVariableArrayPS::MenuClear()
{
    indexMSUI.indexListUI.ClearAll();
}

void DPO_SelectVariableArrayPS::MenuDefault()
{
    indexMSUI.indexListUI.SelectAll();
}

