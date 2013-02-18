///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SATableToTablePS.cpp
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

#include "DPO_SATableToTablePS.h"


DPO_SATableToTablePS::DPO_SATableToTablePS() :
    inputSATableObjRefUI(typeid(DO_SATable))
{
    listDataOK = false;
}


void DPO_SATableToTablePS::SetListsCB(void* inObj)
{
    static_cast<DPO_SATableToTablePS*>(inObj)->SetLists();
}

void DPO_SATableToTablePS::SetLists()
{
   listDataOK = inputSATableObjRefUI.CheckDataObj();

    SC_StringArray varList;
    SC_StringArray obsList;
    SC_StringArray stepList;
    if (listDataOK)
    {
        const DC_SATable&   inputTableDC =
            static_cast<DO_SATable*>(inputSATableObjRefUI.GetDataObj())->GetSATableRef();

        inputTableDC.GetVariableIDs(varList);
        inputTableDC.GetStepIDs(stepList);
        obsList.Alloc(inputTableDC.GetNVector());
        obsList.SetIndexed("Obs# ", 1);
    }
    else
    {
        varList.Alloc(1);
        varList += "no variables";
        obsList.Alloc(1);
        obsList += "no obs";
        stepList.Alloc(1);
        stepList += "no steps";
    }

    vectorIndexUI.SetListText(obsList);
    variableIndexUI.SetListText(varList);
    stepIndexUI.SetListText(stepList);
    SetAllSensitive();
}

void DPO_SATableToTablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_SATableToTablePS*>(inObj)->SetAllSensitive();
}

void DPO_SATableToTablePS::SetAllSensitive()
{
    int rowType = rowDataUI.GetChoice();
    int colType = colDataUI.GetChoice();
    if ((!listDataOK) || (rowType == colType))
    {
        variableFrame.SetSensitive(false);
        vectorFrame.SetSensitive(false);
        stepFrame.SetSensitive(false);
    }
    else
    {
        int fixType = 3 - (rowType + colType);
        variableFrame.SetSensitive(fixType == 0);
        vectorFrame.SetSensitive(fixType == 1);
        stepFrame.SetSensitive(fixType == 2);
    }
}

void DPO_SATableToTablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputSATableObjRefUI.InitFramedUI("Input SA Table Table", URform.GetWidget(), args);
    inputSATableObjRefUI.AddOtherObjectCallback(SetListsCB, this);

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
    args.BottomFormAttach();
    selectForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(35);
    variableFrame.InitFramedUI("Variable", selectForm.GetWidget(), args);

    args.StdSplitMiddle(35, 70);
    vectorFrame.InitFramedUI("Observation", selectForm.GetWidget(), args);

    args.StdSplitFinish(30);
    stepFrame.InitFramedUI("Step", selectForm.GetWidget(), args);

    args.AllFormAttach();

    variableIndexUI.InitWidget(variableFrame.GetForm(), args);
    vectorIndexUI.InitWidget(vectorFrame.GetForm(), args);
    stepIndexUI.InitWidget(stepFrame.GetForm(), args);
}

void  DPO_SATableToTablePS::MenuClose()
{
    MenuBaseC::MenuClose();
}

void DPO_SATableToTablePS::MenuOpen(DPO_SATableToTable& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputSATableObjRefUI.StdMenuInit(inData.inputSATableObjRef);
}

void DPO_SATableToTablePS::SetMenuData(DPO_SATableToTable&  inData)
{
    MenuBaseC::SetMenuData(inData);
    inputSATableObjRefUI.SetFuncObjRef(inData.inputSATableObjRef);

    rowDataUI.SetChoice(int(inData.rowData));
    colDataUI.SetChoice(int(inData.colData));

    variableIndexUI.SetSingleSelection(inData.variableIndex);
    vectorIndexUI.SetSingleSelection(inData.vectorIndex);
    stepIndexUI.SetSingleSelection(inData.stepIndex);

    SetLists();
}

bool DPO_SATableToTablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputSATableObjRefUI.FuncObjRefOK(allOK, "Input table");

    return allOK;
}

void DPO_SATableToTablePS::GetMenuData(DPO_SATableToTable&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputSATableObjRef = inputSATableObjRefUI.GetFuncObjRef();

    outData.rowData = DC_SATable::SATableComponent(rowDataUI.GetChoice());
    outData.colData = DC_SATable::SATableComponent(colDataUI.GetChoice());

    if ((outData.rowData != DC_SATable::saVariable) &&
        (outData.colData != DC_SATable::saVariable))
        outData.variableIndex = variableIndexUI.GetSingleSelection();

    if ((outData.rowData != DC_SATable::saVector) &&
        (outData.colData != DC_SATable::saVector))
        outData.vectorIndex = vectorIndexUI.GetSingleSelection();

    if ((outData.rowData != DC_SATable::saSteps) &&
        (outData.colData != DC_SATable::saSteps))
        outData.stepIndex = stepIndexUI.GetSingleSelection();

}

