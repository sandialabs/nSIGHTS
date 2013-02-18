///////////////////////////////////////////////////////////////////////////////////
//
// PPO_2DTableHistogramPS.cpp
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

#include "PPO_2DTableHistogram.h"
#include "PPO_2DTableHistogramPS.h"

void PPO_2DTableHistogramPS::TableHistogramUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<TableHistogramUIC*>(currObj)->SetAllSensitive(true);
}

void PPO_2DTableHistogramPS::TableHistogramUIC::InitSeriesData()
{
    ArgListC args;

    args.StdFormAttach();
    topRow.InitWidget(outsideForm.GetWidget(), args);

    args.Clear();
    args.BottomFormAttach();
    args.TopFormAttach();
    args.LeftPosAttach(25);
    args.RightFormAttach();
    seriesSpec.InitFormUI(topRow.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach();
    doHistogram.StdToggleInit(" ", topRow.GetWidget(), args);
    doHistogram.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopPosAttach(50);
    args.BottomFormAttach();
    args.RightUICAttach(seriesSpec);

    //  kluge -- controls entire geometry
    seriesDataColumn.listWidth = 15;
    seriesDataColumn.InitWidget(topRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRow.GetWidget());
    nextRow.InitWidget(outsideForm.GetWidget(), args);

    args.Clear();
    args.BottomFormAttach();
    args.TopFormAttach();
    args.LeftPosAttach(10);
    args.RightFormAttach();
    legendOverrideUI.InitFormUI(nextRow.GetWidget(), args);
    legendOverrideUI.overrideDefaultLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
}

void PPO_2DTableHistogramPS::TableHistogramUIC::SetSeriesData(const PFO_2DTableHistogram::TableHistogram& seriesData)
{
    seriesSpec.SetSeriesData(seriesData);
    doHistogram.SetValue(seriesData.doHistogram);
    legendOverrideUI.SetLegendOverride(seriesData);
    seriesDataColumn.SetSelection(seriesData.tableColIndx);
}

void PPO_2DTableHistogramPS::TableHistogramUIC::GetSeriesData(PFO_2DTableHistogram::TableHistogram& seriesData)
{
    seriesData.doHistogram         = doHistogram.GetValue();
    seriesData.tableColIndx     = seriesDataColumn.GetSelection();
    seriesSpec.GetSeriesData(seriesData);
    legendOverrideUI.GetLegendOverride(seriesData);
}

void PPO_2DTableHistogramPS::TableHistogramUIC::SetAllSensitive(bool inSens)
{
    doHistogram.SetSensitive(inSens);
    bool otherSens = doHistogram.GetValue() && inSens;
    seriesDataColumn.SetSensitive(otherSens);
    seriesSpec.SetSensitive(otherSens);
    legendOverrideUI.SetSensitive(otherSens);
}

CSize PPO_2DTableHistogramPS::TableScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 600);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}


/////////////////////////////////////////////////////////////////
// PPO_2DTableHistogramPS

PPO_2DTableHistogramPS::PPO_2DTableHistogramPS() :
    dataSource(typeid(DO_TableData)),
    tableDataOK(false)
{
}

void PPO_2DTableHistogramPS::SetAllSensitive()
{
    xColumn.SetSensitive(tableDataOK);
    lastNRowsUI.SetSensitive(!plotAllRowsUI.GetValue());

    for ( int i = 0; i < PFO_2DTableHistogram::maxTableHistogram; i++)
        seriesDataUI[i].SetAllSensitive(tableDataOK);
}

void PPO_2DTableHistogramPS::SetAllSensitiveCB(void* currObj)
{
    static_cast<PPO_2DTableHistogramPS*>(currObj)->SetAllSensitive();
}

void PPO_2DTableHistogramPS::SetTableColumnsCB(void* inObj)
{
    static_cast<PPO_2DTableHistogramPS*>(inObj)->SetTableColumns();
}

void PPO_2DTableHistogramPS::SetTableColumns()
{
    tableDataOK = dataSource.CheckDataObj();
    if (tableDataOK)
    {
        DO_TableData* dataDO = (DO_TableData*) dataSource.GetDataObj();
        SC_StringArray& colDesc = dataDO->tableData->columnDesc;

        xColumn.SetListText(colDesc);
        for (int i = 0; i < PFO_2DTableHistogram::maxTableHistogram; i++)
            seriesDataUI[i].seriesDataColumn.SetListText(colDesc);
    }
    else
    {
        xColumn.SetListNA();
        for ( int i = 0; i < PFO_2DTableHistogram::maxTableHistogram; i++)
            seriesDataUI[i].seriesDataColumn.SetListNA();
    }
    SetAllSensitive();
}

void PPO_2DTableHistogramPS::InitMenu(const PPO_2DTableHistogram&   inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(55);
    dataSource.topOffset = 0;
    dataSource.InitFramedUI("Table Data", topRowForm.GetWidget(), args);
    dataSource.AddOtherObjectCallback(SetTableColumnsCB, this);

    args.StdSplitFinish(45);
    xColumn.topOffset = 0;
    xColumn.listWidth = 20;
    xColumn.InitFramedUI("X Data Column", topRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach(10);
    plotAllRowsUI.StdToggleInit("Plot all rows", optionsFrame.GetForm(), args);
    plotAllRowsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitSetup();
    args.StdSplitFinish();
    lastNRowsUI.InitLabeledUI("# of rows to plot", optionsFrame.GetForm(), args);

//  scrolled window in middle
    args.Clear();
    args.StdTopObjectAttach(optionsFrame);
    args.BottomFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);

    //scrollArea.needHorizontal = false;
    scrollArea.InitWidget(PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();

    for (int i = 0; i < PFO_2DTableHistogram::maxTableHistogram; i++)
    {
        seriesDataUI[i].outsideForm.InitWidget(scrollArea, args);
        seriesDataUI[i].InitSeriesData();

        args.Clear();
        args.StdTopObjectAttach(seriesDataUI[i].outsideForm.GetWidget());
        seriesDataUI[i].hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(seriesDataUI[i].hSep.GetWidget());
    }
}

void PPO_2DTableHistogramPS::MenuOpen(PPO_2DTableHistogram& inData)
{
    MenuBaseC::MenuOpen(true, false);
    dataSource.StdMenuInit(inData.tableDataObjRef);
    for (int i = 0; i < PFO_2DTableHistogram::maxTableHistogram; i++)
    {
        seriesDataUI[i].seriesSpec.seriesPen.ResetPens(inData.objPenSet);
    }
}

void PPO_2DTableHistogramPS::SetMenuData(PPO_2DTableHistogram&  inData)
{
    dataSource.SetFuncObjRef(inData.tableDataObjRef);
    xColumn.SetSelection(inData.xDataIndx);
    plotAllRowsUI.SetValue(inData.plotAllRows);
    lastNRowsUI.SetFieldValue(inData.lastNRows);

    for (int i = 0; i < PFO_2DTableHistogram::maxTableHistogram; i++)
        seriesDataUI[i].SetSeriesData(inData.seriesData[i]);

    SetTableColumns();
    PPO_BasePS::SetMenuData(inData);
}

bool PPO_2DTableHistogramPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    dataSource.FuncObjRefOK(allOK, "Data source");
    if (!plotAllRowsUI.GetValue())
        lastNRowsUI.FieldValueOK(allOK, "# of rows", 1, 10000);
    return allOK;
}

void PPO_2DTableHistogramPS::GetMenuData(PPO_2DTableHistogram&    outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.tableDataObjRef = dataSource.GetFuncObjRef();
    outData.xDataIndx = xColumn.GetSelection();
    outData.plotAllRows = plotAllRowsUI.GetValue();
    if (!outData.plotAllRows)
        outData.lastNRows = lastNRowsUI.GetFieldValue();

    for (int i = 0; i < PFO_2DTableHistogram::maxTableHistogram; i++)
        seriesDataUI[i].GetSeriesData(outData.seriesData[i]);
}

