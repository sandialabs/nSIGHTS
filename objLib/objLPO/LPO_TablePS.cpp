///////////////////////////////////////////////////////////////////////////////////
//
// LPO_TablePS.cpp
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

#include "ObjLibAfx.h"

#include <genClass/U_Str.h>

#include <genClass/DO_TableData.h>

#include "LPO_Table.h"
#include "LPO_TablePS.h"

LPO_TablePS::LPO_TablePS() :
    tableObjRefUI(typeid(DO_TableData))
{
}


void LPO_TablePS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    tableObjRefUI.InitFramedUI("Table Data To List", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    optionsFrame.InitFramedUI("Data To Write", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    listTableIDUI.StdToggleInit("table ID", optionsFrame.GetForm(), args);

    args.StdToggleSetup(30);
    listColumnHeadersUI.StdToggleInit("column headers", optionsFrame.GetForm(), args);

    args.StdToggleSetup(55);
    listRowDescriptionsUI.StdToggleInit("row descriptions", optionsFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(optionsFrame);
    tableDataFormatUI.InitFramedUI("Table Data Format", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(tableDataFormatUI);
    layoutFrame.InitFramedUI("Layout", PPform.GetWidget(), args);

    args.StdSplitSetup(45);
    tableColWidthUI.SetFieldLen(6);
    tableColWidthUI.InitLabeledUI("Column width", layoutFrame.GetForm(), args);

    args.StdSplitFinish(45);
    maxListWidthUI.SetFieldLen(6);
    maxListWidthUI.InitLabeledUI("Maximum listing width", layoutFrame.GetForm(), args);
}


void LPO_TablePS::MenuOpen(LPO_Table&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    tableObjRefUI.StdMenuInit(inData.tableObjRef);
}


void LPO_TablePS::SetMenuData(LPO_Table&    inData)
{
    MenuBaseC::SetMenuData(inData);
    tableObjRefUI.SetFuncObjRef(inData.tableObjRef);

    listTableIDUI.SetValue(inData.listTableID);
    listColumnHeadersUI.SetValue(inData.listColumnHeaders);
    listRowDescriptionsUI.SetValue(inData.listRowDescriptions);

    tableDataFormatUI.SetRealFormatSettings(inData.tableDataFormat);
    tableColWidthUI.SetFieldValue(inData.tableColWidth);
    maxListWidthUI.SetFieldValue(inData.maxListWidth);
}

bool LPO_TablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    tableObjRefUI.FuncObjRefOK(allOK, "Table data source");
    tableColWidthUI.FieldValueOK(allOK, "Col width", 5, 30);
    maxListWidthUI.FieldValueOK(allOK, "Col width", 40, ListObjC::listingLineLen - 1);

    return allOK;
}

void LPO_TablePS::GetMenuData(LPO_Table&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.tableObjRef = tableObjRefUI.GetFuncObjRef();

    outData.listTableID = listTableIDUI.GetValue();
    outData.listColumnHeaders = listColumnHeadersUI.GetValue();
    outData.listRowDescriptions = listRowDescriptionsUI.GetValue();

    tableDataFormatUI.GetRealFormatSettings(outData.tableDataFormat);
    outData.tableColWidth = tableColWidthUI.GetFieldValue();
    outData.maxListWidth = maxListWidthUI.GetFieldValue();

}

