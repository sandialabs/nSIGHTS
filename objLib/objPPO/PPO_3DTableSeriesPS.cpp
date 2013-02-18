///////////////////////////////////////////////////////////////////////////////////
//
// PPO_3DTableSeriesPS.cpp
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/DO_TableData.h>

#include "PPO_3DTableSeriesPS.h"

// ****************************************

PPO_3DTableSeriesPS::PPO_3DTableSeriesPS() :
    tableDataObjRefUI(typeid(DO_TableData)),
    tableDataOK(false)
{
}

void PPO_3DTableSeriesPS::SetAllSensitive()
{
    xDataIndxUI.SetSensitive(tableDataOK);
    yDataIndxUI.SetSensitive(tableDataOK);
    zDataIndxUI.SetSensitive(tableDataOK);
    seriesLabelUI.SetSensitive(true);
    xyzSeriesUI.SetSensitive(true);
    lastNRowsUI.SetSensitive(!plotAllRowsUI.GetValue());
}

void PPO_3DTableSeriesPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_3DTableSeriesPS*>(inObj)->SetAllSensitive();
}


void PPO_3DTableSeriesPS::SetTableColumnsCB(void* inObj)

{
    static_cast<PPO_3DTableSeriesPS*>(inObj)->SetTableColumns();
}



void PPO_3DTableSeriesPS::SetTableColumns()
{

    //tableDataOK = tableDataObjRefUI.DataObjAvail();   //if available data object not okay, will crash
    tableDataOK = tableDataObjRefUI.CheckDataObj();
    DO_TableData* dataDO;
    if (tableDataOK)
    {
        dataDO = static_cast<DO_TableData*>(tableDataObjRefUI.GetDataObj());
        tableDataOK = dataDO->tableData->columnDesc.Size() > 0;
    }

    if (tableDataOK)
    {
        const SC_StringArray& colDesc = dataDO->tableData->columnDesc;
        xDataIndxUI.SetListText(colDesc);
        yDataIndxUI.SetListText(colDesc);
        zDataIndxUI.SetListText(colDesc);
    }
    else
    {
        xDataIndxUI.SetListNA();
        yDataIndxUI.SetListNA();
        zDataIndxUI.SetListNA();
    }
    SetAllSensitive();
}


void PPO_3DTableSeriesPS::InitMenu(const PPO_3DTableSeries& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    tableDataObjRefUI.topOffset = 0;
    tableDataObjRefUI.InitFramedUI("Table Data", topRow.GetWidget(), args);
    tableDataObjRefUI.AddOtherObjectCallback(SetTableColumnsCB, this);

    args.StdSplitFinish();
    xDataIndxUI.topOffset = 0;
    xDataIndxUI.listWidth = 20;
    xDataIndxUI.InitFramedUI("X Data Column", topRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRow.GetWidget());
    nextRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    yDataIndxUI.topOffset = 0;
    yDataIndxUI.listWidth = 20;
    yDataIndxUI.InitFramedUI("Y Data Column", nextRow.GetWidget(), args);

    args.StdSplitFinish();
    zDataIndxUI.topOffset = 0;
    zDataIndxUI.listWidth = 20;
    zDataIndxUI.InitFramedUI("Z Data Column", nextRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(nextRow.GetWidget());
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach(10);
    plotAllRowsUI.StdToggleInit("Plot all rows", optionsFrame.GetForm(), args);
    plotAllRowsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitSetup();
    args.StdSplitFinish();
    lastNRowsUI.InitLabeledUI("# of rows to plot", optionsFrame.GetForm(), args);


    args.Clear();
    args.StdTopObjectAttach(optionsFrame);
    xyzSeriesUI.InitFramedUI("Plotting Spec.", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(xyzSeriesUI);
    seriesLabelUI.InitFramedUI("Series Label", PPform.GetWidget(), args);
    seriesLabelUI.overrideDefaultLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(seriesLabelUI);
    offsetCoordUI.StdInit("Offset", PPform.GetWidget(), args);
}


void PPO_3DTableSeriesPS::MenuOpen(PPO_3DTableSeries&   inData)
{
    MenuBaseC::MenuOpen(true, false);
    tableDataObjRefUI.StdMenuInit(inData.tableDataObjRef);
    xyzSeriesUI.ResetPens(inData.objPenSet);
}

void PPO_3DTableSeriesPS::SetMenuData(PPO_3DTableSeries&    inData)
{
    tableDataObjRefUI.SetFuncObjRef(inData.tableDataObjRef);
    xDataIndxUI.SetSelection(inData.xDataIndx);
    yDataIndxUI.SetSelection(inData.yDataIndx);
    zDataIndxUI.SetSelection(inData.zDataIndx);
    plotAllRowsUI.SetValue(inData.plotAllRows);
    lastNRowsUI.SetFieldValue(inData.lastNRows);


    xyzSeriesUI.Set3DSeriesData(inData.xyzSeries);
    seriesLabelUI.SetLegendOverride(inData.seriesLabel);

    SetTableColumns();
    PPO_BasePS::SetMenuData(inData);
}

bool PPO_3DTableSeriesPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    tableDataObjRefUI.FuncObjRefOK(allOK, "table input");
    seriesLabelUI.UIdataOK(allOK);
    if (!plotAllRowsUI.GetValue())
        lastNRowsUI.FieldValueOK(allOK, "# of rows", 1, 10000);
    return allOK;
}

void PPO_3DTableSeriesPS::GetMenuData(PPO_3DTableSeries&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.tableDataObjRef = tableDataObjRefUI.GetFuncObjRef();
    outData.xDataIndx = xDataIndxUI.GetSelection();
    outData.yDataIndx = yDataIndxUI.GetSelection();
    outData.zDataIndx = zDataIndxUI.GetSelection();
    outData.plotAllRows = plotAllRowsUI.GetValue();
    if (!outData.plotAllRows)
        outData.lastNRows = lastNRowsUI.GetFieldValue();

    xyzSeriesUI.Get3DSeriesData(outData.xyzSeries);
    seriesLabelUI.GetLegendOverride(outData.seriesLabel);
}

void PPO_3DTableSeriesPS::MenuDefault()
{
    PPO_BasePS::MenuDefault();
    xyzSeriesUI.Set3DSeriesData(PSC_3DSeriesSpec());
    seriesLabelUI.SetLegendOverride(PSC_LegendOverride());
    SetAllSensitive();
}

