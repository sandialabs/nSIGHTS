///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableTransposePS.cpp
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

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/DO_TableData.h>

#include "DPO_TableTranspose.h"
#include "DPO_TableTransposePS.h"


DPO_TableTransposePS::DPO_TableTransposePS()
    : inputTableObjRefUI(typeid(DO_TableData))
{
}

void DPO_TableTransposePS::SetAllSensitive()
{
    xcolumnIndexUI.SetSensitive(listDataOK && skipXColumnUI.GetValue());
}


void DPO_TableTransposePS::ResetColumnList()
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

void DPO_TableTransposePS::ResetColumnListCB(void* inObj)
{
    static_cast<DPO_TableTransposePS*>(inObj)->ResetColumnList();
}


void DPO_TableTransposePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableTransposePS*>(inObj)->SetAllSensitive();
}


void DPO_TableTransposePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Input Table Data", URform.GetWidget(), args);
    inputTableObjRefUI.AddOtherObjectCallback(ResetColumnListCB, this);

    //  form for row with row/col selections
    args.StdFormAttach();
    skipXFrame.InitFramedUI("X Column Processing", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    skipXColumnUI.StdToggleInit("skip X column", skipXFrame.GetForm(), args);
    skipXColumnUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(60);
    xcolumnIndexUI.InitWidget(skipXFrame.GetForm(), args);
}

void DPO_TableTransposePS::SetMenuData(DPO_TableTranspose&  inData)
{
    MenuBaseC::SetMenuData(inData);
    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);
    skipXColumnUI.SetValue(inData.skipXColumn);
    xcolumnIndexUI.SetSelection(inData.xcolumnIndex);

    ResetColumnList();
}


void DPO_TableTransposePS::MenuOpen(DPO_TableTranspose& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
}


bool DPO_TableTransposePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableObjRefUI.FuncObjRefOK(allOK, "Input table");
    return allOK;
}

void DPO_TableTransposePS::GetMenuData(DPO_TableTranspose&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();

    outData.skipXColumn = skipXColumnUI.GetValue();
    outData.xcolumnIndex = xcolumnIndexUI.GetSelection();
}

