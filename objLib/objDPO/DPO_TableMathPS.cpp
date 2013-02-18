///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableMathPS.cpp
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

#include "DPO_TableMathPS.h"
#include "DPO_TableMath.h"

DPO_TableMathPS::DPO_TableMathPS() :
    inputTableAObjRefUI(typeid(DO_TableData)),
    inputTableBObjRefUI(typeid(DO_TableData))
{
}


void DPO_TableMathPS::SetAllSensitive()
{
    xcolumnIndexUI.SetSensitive(listDataOK && skipXColumnUI.GetValue());
}


void DPO_TableMathPS::ResetColumnList()
{
    listDataOK = inputTableAObjRefUI.CheckDataObj();
    if (listDataOK)
    {
        const SC_StringArray& colDesc = static_cast<DO_TableData*>(inputTableAObjRefUI.GetDataObj())->tableData->columnDesc;;
        xcolumnIndexUI.SetListText(colDesc);
    }
    else
    {
        xcolumnIndexUI.SetListNA();
    }

    SetAllSensitive();
}

void DPO_TableMathPS::ResetColumnListCB(void* inObj)
{
    static_cast<DPO_TableMathPS*>(inObj)->ResetColumnList();
}


void DPO_TableMathPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableMathPS*>(inObj)->SetAllSensitive();
}



void DPO_TableMathPS::InitMenu()
{
    MenuBaseC::InitMenu();
    ArgListC args;

    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    inputTableAObjRefUI.InitFramedUI("Table A", topRow.GetWidget(), args);
    inputTableAObjRefUI.AddOtherObjectCallback(ResetColumnListCB, this);

    args.StdSplitFinish();
    inputTableBObjRefUI.InitFramedUI("Table B", topRow.GetWidget(), args);

    //  form for row with row/col selections
    args.StdTopObjectAttach(topRow.GetWidget());
    skipXFrame.InitFramedUI("X Column Processing", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    skipXColumnUI.StdToggleInit("skip X column", skipXFrame.GetForm(), args);
    skipXColumnUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(60);
    xcolumnIndexUI.InitWidget(skipXFrame.GetForm(), args);

    args.StdTopObjectAttach(skipXFrame);
    const char* troStrs[] = {" A + B ", " A - B ", " A * B", " A / B "};
    tableMathOperationUI.PreInitSetUp(troStrs, 4);
    tableMathOperationUI.InitFramedUI("Operation", PPform.GetWidget(), args);
}

void DPO_TableMathPS::MenuOpen(DPO_TableMath& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableAObjRefUI.StdMenuInit(inData.inputTableAObjRef);
    inputTableBObjRefUI.StdMenuInit(inData.inputTableBObjRef);
}


void DPO_TableMathPS::SetMenuData(DPO_TableMath&  inData)
{
    MenuBaseC::SetMenuData(inData);

    inputTableAObjRefUI.SetFuncObjRef(inData.inputTableAObjRef);
    inputTableBObjRefUI.SetFuncObjRef(inData.inputTableBObjRef);
    skipXColumnUI.SetValue(inData.skipXColumn);
    xcolumnIndexUI.SetSelection(inData.xcolumnIndex);
    tableMathOperationUI.SetChoice(int(inData.tableMathOperation));
    ResetColumnList();
}

bool DPO_TableMathPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableAObjRefUI.FuncObjRefOK(allOK, "Table A source");
    inputTableBObjRefUI.FuncObjRefOK(allOK, "Table B source");
    return allOK;
}

void DPO_TableMathPS::GetMenuData(DPO_TableMath&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTableAObjRef = inputTableAObjRefUI.GetFuncObjRef();
    outData.inputTableBObjRef = inputTableBObjRefUI.GetFuncObjRef();
    outData.skipXColumn = skipXColumnUI.GetValue();
    if (outData.skipXColumn)
        outData.xcolumnIndex = xcolumnIndexUI.GetSelection();
    outData.tableMathOperation = DFO_TableMath::TableMathOperation(tableMathOperationUI.GetChoice());
}

