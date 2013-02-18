///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableRowAccumulatePS.cpp
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

#include <genClass/DO_TableData.h>

#include "DPO_TableRowAccumulatePS.h"
#include "DPO_TableRowAccumulate.h"

DPO_TableRowAccumulatePS::DPO_TableRowAccumulatePS() :
    inputTableObjRefUI(typeid(DO_TableData))
{
}


void DPO_TableRowAccumulatePS::SetAllSensitive()
{
    xcolumnIndexUI.SetSensitive(listDataOK && skipXColumnUI.GetValue());
}


void DPO_TableRowAccumulatePS::ResetColumnList()
{
    listDataOK = inputTableObjRefUI.CheckDataObj();

    if (listDataOK)
    {
        const SC_StringArray& colDesc = static_cast<DO_TableData*>(inputTableObjRefUI.GetDataObj())->tableData->columnDesc;;
        xcolumnIndexUI.SetListText(colDesc);
    }
    else
    {
        xcolumnIndexUI.SetListNA();
    }

    SetAllSensitive();
}

void DPO_TableRowAccumulatePS::ResetColumnListCB(void* inObj)
{
    static_cast<DPO_TableRowAccumulatePS*>(inObj)->ResetColumnList();
}


void DPO_TableRowAccumulatePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableRowAccumulatePS*>(inObj)->SetAllSensitive();
}



void DPO_TableRowAccumulatePS::InitMenu()
{
    MenuBaseC::InitMenu();
    ArgListC args;

    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Table to Summarize", URform.GetWidget(), args);
    inputTableObjRefUI.AddOtherObjectCallback(ResetColumnListCB, this);

    //  form for row with row/col selections
    args.StdFormAttach();
    skipXFrame.InitFramedUI("X Column Processing", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    skipXColumnUI.StdToggleInit("skip X column", skipXFrame.GetForm(), args);
    skipXColumnUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(60);
    xcolumnIndexUI.InitWidget(skipXFrame.GetForm(), args);

    args.StdTopObjectAttach(skipXFrame);
    const char* troStrs[] = {" Accumulate All ", " Accumulate +ve ", "Delta"};
    tableRowOperationUI.PreInitSetUp(troStrs, 3);
    tableRowOperationUI.InitFramedUI("Operation", PPform.GetWidget(), args);
}

void DPO_TableRowAccumulatePS::MenuOpen(DPO_TableRowAccumulate& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
}


void DPO_TableRowAccumulatePS::SetMenuData(DPO_TableRowAccumulate&  inData)
{
    MenuBaseC::SetMenuData(inData);

    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);
    skipXColumnUI.SetValue(inData.skipXColumn);
    xcolumnIndexUI.SetSelection(inData.xcolumnIndex);
    tableRowOperationUI.SetChoice(int(inData.tableRowOperation));
    ResetColumnList();
}

bool DPO_TableRowAccumulatePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableObjRefUI.FuncObjRefOK(allOK, "Table source");
    return allOK;
}

void DPO_TableRowAccumulatePS::GetMenuData(DPO_TableRowAccumulate&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();
    outData.skipXColumn = skipXColumnUI.GetValue();
    if (outData.skipXColumn)
        outData.xcolumnIndex = xcolumnIndexUI.GetSelection();
    outData.tableRowOperation = DFO_TableRowAccumulate::TableRowOperation(tableRowOperationUI.GetChoice());
}

