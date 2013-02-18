///////////////////////////////////////////////////////////////////////////////////
//
// LPO_TableStatisticsPS.cpp
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

#include "LPO_TableStatistics.h"
#include "LPO_TableStatisticsPS.h"

LPO_TableStatisticsPS::LPO_TableStatisticsPS() :
    tableObjRefUI(typeid(DO_TableData))
{
}

void LPO_TableStatisticsPS::SetAllSensitive()
{
    useLog10UI.SetSensitive(logDataUI.GetValue());
    presentTransformedUI.SetSensitive(logDataUI.GetValue());
}

void LPO_TableStatisticsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<LPO_TableStatisticsPS*>(inObj)->SetAllSensitive();
}

void LPO_TableStatisticsPS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    tableObjRefUI.InitFramedUI("Table List Statistics For", URform.GetWidget(), args);

    args.StdFormAttach();
    optionsFrame.InitFramedUI("Data To Write", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    listTableIDUI.StdToggleInit("table ID", optionsFrame.GetForm(), args);

    args.StdToggleSetup(20);
    listColumnHeadersUI.StdToggleInit("column headers", optionsFrame.GetForm(), args);

    args.StdToggleSetup(35);
    logDataUI.StdToggleInit("log Data", optionsFrame.GetForm(), args);
    logDataUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(50, 70);
    args.TopOffset(10);
    useLog10UI.PreInitSetUp("natural log", "log base 10");
    useLog10UI.InitWidget(optionsFrame.GetForm(), args);

    args.StdToggleSetup(80);
    presentTransformedUI.StdToggleInit("results are log", optionsFrame.GetForm(), args);

    args.StdTopObjectAttach(optionsFrame);
    tableDataFormatUI.InitFramedUI("Table Data Format", PPform.GetWidget(), args);

    args.StdTopObjectAttach(tableDataFormatUI);
    layoutFrame.InitFramedUI("Layout", PPform.GetWidget(), args);

    args.StdSplitSetup(45);
    tableColWidthUI.SetFieldLen(6);
    tableColWidthUI.InitLabeledUI("Column width", layoutFrame.GetForm(), args);

    args.StdSplitFinish(45);
    maxListWidthUI.SetFieldLen(6);
    maxListWidthUI.InitLabeledUI("Maximum listing width", layoutFrame.GetForm(), args);
}


void LPO_TableStatisticsPS::MenuOpen(LPO_TableStatistics&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    tableObjRefUI.StdMenuInit(inData.tableObjRef);
}


void LPO_TableStatisticsPS::SetMenuData(LPO_TableStatistics&    inData)
{
    MenuBaseC::SetMenuData(inData);
    tableObjRefUI.SetFuncObjRef(inData.tableObjRef);

    listTableIDUI.SetValue(inData.listTableID);
    listColumnHeadersUI.SetValue(inData.listColumnHeaders);
    logDataUI.SetValue(inData.logData);
    useLog10UI.SetBoolChoice(inData.useLog10);
    presentTransformedUI.SetValue(inData.presentTransformed);

    tableDataFormatUI.SetRealFormatSettings(inData.tableDataFormat);
    tableColWidthUI.SetFieldValue(inData.tableColWidth);
    maxListWidthUI.SetFieldValue(inData.maxListWidth);
}

bool LPO_TableStatisticsPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    tableObjRefUI.FuncObjRefOK(allOK, "Table data source");
    tableColWidthUI.FieldValueOK(allOK, "Col width", 5, 30);
    maxListWidthUI.FieldValueOK(allOK, "Col width", 40, ListObjC::listingLineLen - 1);

    return allOK;
}

void LPO_TableStatisticsPS::GetMenuData(LPO_TableStatistics&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.tableObjRef = tableObjRefUI.GetFuncObjRef();

    outData.listTableID = listTableIDUI.GetValue();
    outData.listColumnHeaders = listColumnHeadersUI.GetValue();
    outData.logData = logDataUI.GetValue();
    outData.useLog10 = useLog10UI.GetBoolChoice();
    outData.presentTransformed = presentTransformedUI.GetValue();

    tableDataFormatUI.GetRealFormatSettings(outData.tableDataFormat);
    outData.tableColWidth = tableColWidthUI.GetFieldValue();
    outData.maxListWidth = maxListWidthUI.GetFieldValue();

}

