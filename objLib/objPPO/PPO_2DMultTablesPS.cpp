///////////////////////////////////////////////////////////////////////////////////
//
// PPO_2DMultTablesPS.cpp
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

#include "PPO_2DMultTables.h"
#include "PPO_2DMultTablesPS.h"


PPO_2DMultTablesPS::TableDataUIC::TableDataUIC() : tableDataObjRefUI(typeid(DO_TableData))
{
    listDataOK = false;
}

void PPO_2DMultTablesPS::TableDataUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<PPO_2DMultTablesPS::TableDataUIC*>(currObj)->SetAllSensitive();
}

void PPO_2DMultTablesPS::TableDataUIC::ResetColumnList()
{
    listDataOK = tableDataObjRefUI.CheckDataObj();
    if (listDataOK)
    {
        const SC_StringArray& colDesc = static_cast<DO_TableData*>(tableDataObjRefUI.GetDataObj())->tableData->columnDesc;;
        xColumnUI.SetListText(colDesc);
        yColumnUI.SetListText(colDesc);
    }
    else
    {
        xColumnUI.SetListNA();
        yColumnUI.SetListNA();
    }

    SetAllSensitive();
}

void PPO_2DMultTablesPS::TableDataUIC::ResetColumnListCB(void* inObj)
{
    static_cast<PPO_2DMultTablesPS::TableDataUIC*>(inObj)->ResetColumnList();
}


void PPO_2DMultTablesPS::TableDataUIC::InitSeriesData()
{
    ArgListC args;

    args.StdFormAttach();
    topRow.InitWidget(outsideForm.GetLeft(), args);

    args.StdToggleSetup(2);
    doSeriesUI.StdToggleInit(" ", topRow.GetWidget(), args);
    doSeriesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitSetup();
    args.StdSplitFinish(30);
    tableDataObjRefUI.InitFormUI(topRow.GetWidget(), args);
    tableDataObjRefUI.AddOtherObjectCallback(ResetColumnListCB, this);

    args.Clear();
    args.StdTopObjectAttach(topRow.GetWidget());
    xColumnUI.InitLabeledUI("X", outsideForm.GetLeft(), args);

    args.Clear();
    args.StdTopObjectAttach(xColumnUI);
    yColumnUI.InitLabeledUI("Y", outsideForm.GetLeft(), args);

    args.Clear();
    args.StdFormAttach();
    seriesSpecUI.InitFormUI(outsideForm.GetRight(), args);

    args.Clear();
    args.StdTopObjectAttach(seriesSpecUI);
    legendOverrideUI.InitFormUI(outsideForm.GetRight(), args);
    legendOverrideUI.overrideDefaultLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
}

void PPO_2DMultTablesPS::TableDataUIC::SetSeriesData(const PFO_2DMultTables::TableSeries& seriesData)
{
    seriesSpecUI.SetSeriesData(seriesData);
    legendOverrideUI.SetLegendOverride(seriesData);
    doSeriesUI.SetValue(seriesData.doSeries);
    tableDataObjRefUI.SetFuncObjRef(seriesData.tableDataObjRef);
    xColumnUI.SetSelection(seriesData.xColumn);
    yColumnUI.SetSelection(seriesData.yColumn);
    ResetColumnList();
}

void PPO_2DMultTablesPS::TableDataUIC::GetSeriesData(PFO_2DMultTables::TableSeries& seriesData)
{
    seriesSpecUI.GetSeriesData(seriesData);
    legendOverrideUI.GetLegendOverride(seriesData);

    seriesData.tableDataObjRef = tableDataObjRefUI.GetFuncObjRef();
    seriesData.doSeries = doSeriesUI.GetValue();
    seriesData.xColumn = xColumnUI.GetSelection();
    seriesData.yColumn = yColumnUI.GetSelection();
}

bool PPO_2DMultTablesPS::TableDataUIC::SeriesDataOK(bool& prevOK)
{
    if (doSeriesUI.GetValue())
    {
        legendOverrideUI.UIdataOK(prevOK);
        tableDataObjRefUI.FuncObjRefOK(prevOK, "table input");
    }
    return prevOK;
}


void PPO_2DMultTablesPS::TableDataUIC::SetAllSensitive()
{
    doSeriesUI.SetSensitive(true);
    bool doSeries = doSeriesUI.GetValue();

    xColumnUI.SetSensitive(doSeries && listDataOK);
    yColumnUI.SetSensitive(doSeries && listDataOK);
    seriesSpecUI.SetSensitive(doSeries);
    legendOverrideUI.SetSensitive(doSeries);
    tableDataObjRefUI.SetSensitive(doSeries);
}

CSize PPO_2DMultTablesPS::TableDataScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 750);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// PPO_2DMultTablesPS

PPO_2DMultTablesPS::PPO_2DMultTablesPS()
{
}


void PPO_2DMultTablesPS::SetAllSensitive()
{
    for (int i = 0; i < PFO_2DMultTables::maxTableSeries; i++)
        tableSeriesUI[i].SetAllSensitive();
}


void PPO_2DMultTablesPS::InitMenu(const PPO_2DMultTables&   inData)

{
    PPO_BasePS::InitMenu(inData);

//  scrolled window in middle
    ArgListC args;
    args.Clear();
    args.AllFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);

    //scrollArea.needHorizontal = false;
    scrollArea.InitWidget(PPform.GetWidget(), args);
    args.Clear();
    args.StdFormAttach();

    for (int i = 0; i < PFO_2DMultTables::maxTableSeries; i++)
    {
        tableSeriesUI[i].outsideForm.PreInitSetUp(0, 40);
        tableSeriesUI[i].outsideForm.InitFormUI(scrollArea.GetWidget(), args);
        tableSeriesUI[i].InitSeriesData();

        args.Clear();
        args.StdTopObjectAttach(tableSeriesUI[i].outsideForm);
        tableSeriesUI[i].hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(tableSeriesUI[i].hSep.GetWidget());
    }
}


void PPO_2DMultTablesPS::MenuOpen(PPO_2DMultTables& inData)
{
    MenuBaseC::MenuOpen(true, false);
    for (int i = 0; i < PFO_2DMultTables::maxTableSeries; i++)
    {
        tableSeriesUI[i].tableDataObjRefUI.StdMenuInit(inData.tableSeries[i].tableDataObjRef);
        tableSeriesUI[i].seriesSpecUI.seriesPen.ResetPens(inData.objPenSet);
    }
}


void PPO_2DMultTablesPS::SetMenuData(PPO_2DMultTables&  inData)
{
    for (int i = 0; i < PFO_2DMultTables::maxTableSeries; i++)
        tableSeriesUI[i].SetSeriesData(inData.tableSeries[i]);
    PPO_BasePS::SetMenuData(inData);
    SetAllSensitive();
}

bool PPO_2DMultTablesPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    for (int i = 0; i < PFO_2DMultTables::maxTableSeries; i++)
        if (!tableSeriesUI[i].SeriesDataOK(allOK))
            return false;

    return allOK;
}

void PPO_2DMultTablesPS::GetMenuData(PPO_2DMultTables&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    for (int i = 0; i < PFO_2DMultTables::maxTableSeries; i++)
        tableSeriesUI[i].GetSeriesData(outData.tableSeries[i]);
}


