///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectVarStepPS.cpp
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

#include "SaLibAfx.h"

#include <genClass/U_Str.h>
#include <saClass/DO_SATable.h>

#include "DPO_SelectVarStepPS.h"
#include "DPO_SelectVarStep.h"

DPO_SelectVarStepPS::DPO_SelectVarStepPS() :
    variableIndexMSUI(typeid(DO_Indx), true),
    stepIndexMSUI(typeid(DO_Indx), true),
    inputTableObjRefUI(typeid(DO_SATable))
{
}

void DPO_SelectVarStepPS::SetAllSensitive()
{
    UpdateStatus();
    variableIndexMSUI.SetSensitive(listDataOK);
    stepIndexMSUI.SetSensitive(listDataOK);
    allVarButton.SetSensitive(variableIndexMSUI.isMultiple.GetValue());
    noVarButton.SetSensitive(variableIndexMSUI.isMultiple.GetValue());
    allStepButton.SetSensitive(stepIndexMSUI.isMultiple.GetValue());
    noStepButton.SetSensitive(stepIndexMSUI.isMultiple.GetValue());
}

void DPO_SelectVarStepPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_SelectVarStepPS*>(inObj)->SetAllSensitive();
}


void DPO_SelectVarStepPS::UpdateStatus()
{
    listDataOK = inputTableObjRefUI.CheckDataObj();

    if (!listDataOK)
        return;

    const DC_SATable&   inputTableDC =
        static_cast<DO_SATable*>(inputTableObjRefUI.GetDataObj())->GetSATableRef();

    SC_StringArray varIDs, stepIDs;
    inputTableDC.GetVariableIDs(varIDs);
    inputTableDC.GetStepIDs(stepIDs);

    variableIndexMSUI.indexListUI.SetListText(varIDs);
    stepIndexMSUI.indexListUI.SetListText(stepIDs);
}


void DPO_SelectVarStepPS::UpdateStatusCB(void* inObj)
{
    static_cast<DPO_SelectVarStepPS*>(inObj)->UpdateStatus();
}



void DPO_SelectVarStepPS::ToggleExposedVarIndex()
{
    SetAllSensitive();

    // recast current to local
    static_cast<DPO_SelectVarStep*>(MenuObjC::currMenuObj)->exposedVarIndex.ToggleExposed(variableIndexMSUI.TurnExposedOff());
}

void DPO_SelectVarStepPS::ToggleExposedVarIndexCB(void* inObj)
{
    static_cast<DPO_SelectVarStepPS*>(inObj)->ToggleExposedVarIndex();
}

void DPO_SelectVarStepPS::ToggleExposedStepIndex()
{
    SetAllSensitive();

    // recast current to local
    static_cast<DPO_SelectVarStep*>(MenuObjC::currMenuObj)->exposedStepIndex.ToggleExposed(stepIndexMSUI.TurnExposedOff());
}

void DPO_SelectVarStepPS::ToggleExposedStepIndexCB(void* inObj)
{
    static_cast<DPO_SelectVarStepPS*>(inObj)->ToggleExposedStepIndex();
}


void DPO_SelectVarStepPS::AllVarButton()
{
    variableIndexMSUI.indexListUI.SelectAll();
}

void DPO_SelectVarStepPS::AllVarButtonCB(void* inObj)
{
    static_cast<DPO_SelectVarStepPS*>(inObj)->AllVarButton();
}

void DPO_SelectVarStepPS::NoVarButton()
{
    variableIndexMSUI.indexListUI.ClearAll();
}

void DPO_SelectVarStepPS::NoVarButtonCB(void* inObj)
{
    static_cast<DPO_SelectVarStepPS*>(inObj)->NoVarButton();
}

void DPO_SelectVarStepPS::AllStepButton()
{
    stepIndexMSUI.indexListUI.SelectAll();
}

void DPO_SelectVarStepPS::AllStepButtonCB(void* inObj)
{
    static_cast<DPO_SelectVarStepPS*>(inObj)->AllStepButton();
}

void DPO_SelectVarStepPS::NoStepButton()
{
    stepIndexMSUI.indexListUI.ClearAll();
}

void DPO_SelectVarStepPS::NoStepButtonCB(void* inObj)
{
    static_cast<DPO_SelectVarStepPS*>(inObj)->NoStepButton();
}

void DPO_SelectVarStepPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Input SA Table Table", URform.GetWidget(), args);
    inputTableObjRefUI.AddOtherObjectCallback(UpdateStatusCB, this);

    //  form for buttons
    args.Clear();
    args.BottomFormAttach();
    args.RightFormAttach();
    args.LeftFormAttach();
    buttonForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitMiddle(5, 20);
    allVarButton.StdButtonInit("All Var", buttonForm.GetWidget(), args);
    allVarButton.AddOtherObjectCallback(AllVarButtonCB, this);

    args.StdSplitMiddle(25, 40);
    noVarButton.StdButtonInit("No Var", buttonForm.GetWidget(), args);
    noVarButton.AddOtherObjectCallback(NoVarButtonCB, this);

    args.StdSplitMiddle(55, 70);
    allStepButton.StdButtonInit("All Step", buttonForm.GetWidget(), args);
    allStepButton.AddOtherObjectCallback(AllStepButtonCB, this);

    args.StdSplitMiddle(75, 90);
    noStepButton.StdButtonInit("No Step", buttonForm.GetWidget(), args);
    noStepButton.AddOtherObjectCallback(NoStepButtonCB, this);

    args.StdFormAttach();
    args.BottomWidgetAttach(buttonForm.GetWidget());
    listForm.InitFormUI(PPform.GetWidget(), args);

    args.Clear();
    args.AllFormAttach();
    args.RightOffset(5);

    variableIndexMSUI.indexValueMS.horizontal = false;
    variableIndexMSUI.InitUI("Variable Selection", args, listForm.GetLeft(), ToggleExposedVarIndexCB, this, true, true);
    variableIndexMSUI.indexListUI.AddOtherObjectCallback(UpdateStatusCB, this);
    variableIndexMSUI.isMultiple.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.AllFormAttach();
    args.LeftOffset(5);
    stepIndexMSUI.indexValueMS.horizontal = false;
    stepIndexMSUI.InitUI("Step Selection", args, listForm.GetRight(), ToggleExposedStepIndexCB, this, true, true);
    stepIndexMSUI.indexListUI.AddOtherObjectCallback(UpdateStatusCB, this);
    stepIndexMSUI.isMultiple.AddOtherObjectCallback(SetAllSensitiveCB, this);
}


void DPO_SelectVarStepPS::MenuOpen(DPO_SelectVarStep& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
    variableIndexMSUI.MenuInit(inData.variableIndexMS);
    stepIndexMSUI.MenuInit(inData.stepIndexMS);
}


void DPO_SelectVarStepPS::SetMenuData(DPO_SelectVarStep&  inData)
{
    MenuBaseC::SetMenuData(inData);
    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);
    variableIndexMSUI.SetValue(inData.variableIndexMS, inData.exposedVarIndex);
    stepIndexMSUI.SetValue(inData.stepIndexMS, inData.exposedStepIndex);
    SetAllSensitive();
}

bool DPO_SelectVarStepPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableObjRefUI.FuncObjRefOK(allOK, "Input table");
    stepIndexMSUI.UIdataOK(allOK);
    variableIndexMSUI.UIdataOK(allOK);
    return allOK;
}

void DPO_SelectVarStepPS::GetMenuData(DPO_SelectVarStep&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();
    variableIndexMSUI.GetValue(outData.variableIndexMS);
    stepIndexMSUI.GetValue(outData.stepIndexMS);
}

