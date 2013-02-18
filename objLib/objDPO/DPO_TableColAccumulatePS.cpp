///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableColAccumulatePS.cpp
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

#include "DPO_TableColAccumulatePS.h"
#include "DPO_TableColAccumulate.h"

DPO_TableColAccumulatePS::DPO_TableColAccumulatePS() :
    inputTableObjRefUI(typeid(DO_TableData))
{
}


void DPO_TableColAccumulatePS::SetAllSensitive()
{
    xcolumnIndexUI.SetSensitive(listDataOK && skipXColumnUI.GetValue());
}


void DPO_TableColAccumulatePS::ResetColumnList()
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

void DPO_TableColAccumulatePS::ResetColumnListCB(void* inObj)
{
    static_cast<DPO_TableColAccumulatePS*>(inObj)->ResetColumnList();
}


void DPO_TableColAccumulatePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableColAccumulatePS*>(inObj)->SetAllSensitive();
}



void DPO_TableColAccumulatePS::InitMenu()
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
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);
    args.StdToggleSetup(5);
    nullZeroAccValuesUI.StdToggleInit("set zero accumulation values to null (for stack plots)", optionsFrame.GetForm(), args);

}

void DPO_TableColAccumulatePS::MenuOpen(DPO_TableColAccumulate& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
}


void DPO_TableColAccumulatePS::SetMenuData(DPO_TableColAccumulate&  inData)
{
    MenuBaseC::SetMenuData(inData);

    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);
    skipXColumnUI.SetValue(inData.skipXColumn);
    xcolumnIndexUI.SetSelection(inData.xcolumnIndex);
    nullZeroAccValuesUI.SetValue(inData.nullZeroAccValues);
    ResetColumnList();
}

bool DPO_TableColAccumulatePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableObjRefUI.FuncObjRefOK(allOK, "Table source");
    return allOK;
}

void DPO_TableColAccumulatePS::GetMenuData(DPO_TableColAccumulate&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();
    outData.skipXColumn = skipXColumnUI.GetValue();
    if (outData.skipXColumn)
        outData.xcolumnIndex = xcolumnIndexUI.GetSelection();
    outData.nullZeroAccValues = nullZeroAccValuesUI.GetValue();
}

