///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableToSATablePS.cpp
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

#include <genClassPS/PS_UIBase.h>

#include "DPO_TableToSATablePS.h"


DPO_TableToSATablePS::DPO_TableToSATablePS() :
    inputTableObjRefUI(typeid(DO_TableData))
{
}



void DPO_TableToSATablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableToSATablePS*>(inObj)->SetAllSensitive();
}

void DPO_TableToSATablePS::SetAllSensitive()
{
    int rowType = rowDataUI.GetChoice();
    int colType = colDataUI.GetChoice();

    bool varNameOK = (rowType != colType) && (rowType != 0) && (colType != 0);
    bool stepValOK = (rowType != colType) && (rowType != 2) && (colType != 2);
    bool stepIDOK = (rowType != colType) && ((rowType == 2) || (colType == 2));

    stepValUI.SetSensitive(stepValOK);
    variableNameUI.SetSensitive(varNameOK);
    stepValueIsIDsUI.SetSensitive(stepIDOK);
}

void DPO_TableToSATablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Input Table", URform.GetWidget(), args);

    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    static const char* rcDataStrs[] = {"Variable", "Observation", "Step"};

    args.StdSplitSetup();
    rowDataUI.PreInitSetUp(rcDataStrs, 3);
    rowDataUI.InitFramedUI("Table Row Data", topRowForm.GetWidget(), args);
    rowDataUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    colDataUI.PreInitSetUp(rcDataStrs, 3);
    colDataUI.InitFramedUI("Table Column Data", topRowForm.GetWidget(), args);
    colDataUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    dataForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    variableNameUI.AttachObjectToFrame();
    variableNameUI.InitFramedUI("Variable Name", dataForm.GetWidget(), args);

    args.StdSplitFinish();
    stepValUI.AttachObjectToFrame();
    stepValUI.InitFramedUI("Step Value", dataForm.GetWidget(), args);

    args.StdTopObjectAttach(dataForm.GetWidget());
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    colRow0IsObsIndexUI.StdToggleInit("1st row/col is obs index", optionFrame.GetForm(), args);

    args.StdToggleSetup(50);
    stepValueIsIDsUI.StdToggleInit("step value is row/col ID", optionFrame.GetForm(), args);
}

void  DPO_TableToSATablePS::MenuClose()
{
    MenuBaseC::MenuClose();
}

void DPO_TableToSATablePS::MenuOpen(DPO_TableToSATable& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
}

void DPO_TableToSATablePS::SetMenuData(DPO_TableToSATable&  inData)
{
    MenuBaseC::SetMenuData(inData);
    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);

    rowDataUI.SetChoice(int(inData.rowData));
    colDataUI.SetChoice(int(inData.colData));

    variableNameUI.SetFieldString(inData.variableName);
    stepValUI.SetFieldValue(inData.stepVal);
    colRow0IsObsIndexUI.SetValue(inData.colRow0IsObsIndex);
    stepValueIsIDsUI.SetValue(inData.stepValueIsIDs);

    SetAllSensitive();
}

bool DPO_TableToSATablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableObjRefUI.FuncObjRefOK(allOK, "Input table");

    if ((rowDataUI.GetChoice() != 2) && (colDataUI.GetChoice() != 2))
        stepValUI.FieldValueOK(allOK, "Step value");

    return allOK;
}

void DPO_TableToSATablePS::GetMenuData(DPO_TableToSATable&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();

    outData.rowData = DC_SATable::SATableComponent(rowDataUI.GetChoice());
    outData.colData = DC_SATable::SATableComponent(colDataUI.GetChoice());

    if ((outData.rowData != DC_SATable::saVariable) &&
        (outData.colData != DC_SATable::saVariable))
        variableNameUI.GetFieldString(outData.variableName, DC_SATable::varNameLen);

    if ((outData.rowData != DC_SATable::saSteps) &&
        (outData.colData != DC_SATable::saSteps))
        outData.stepVal = stepValUI.GetFieldValue();

    outData.colRow0IsObsIndex = colRow0IsObsIndexUI.GetValue();
    outData.stepValueIsIDs = stepValueIsIDsUI.GetValue();

}

