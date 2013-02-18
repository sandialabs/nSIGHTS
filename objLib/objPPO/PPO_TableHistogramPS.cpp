///////////////////////////////////////////////////////////////////////////////////
//
// PPO_TableHistogramPS.cpp
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

#include "PPO_TableHistogram.h"
#include "PPO_TableHistogramPS.h"


PPO_TableHistogramPS::PPO_TableHistogramPS() :
    tableInputObjRefUI(typeid(DO_TableData))
{
}

void PPO_TableHistogramPS::SetAllSensitive()
{
    xDataIndxUI.SetSensitive(tableDataOK && histogramUI.positionIsValueUI.GetBoolChoice());
    yDataIndxUI.SetSensitive(tableDataOK);
    legendDataUI.SetSensitive(true);
}

void PPO_TableHistogramPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_TableHistogramPS*>(inObj)->SetAllSensitive();
}

void PPO_TableHistogramPS::SetTableColumnsCB(void* inObj)

{
    static_cast<PPO_TableHistogramPS*>(inObj)->SetTableColumns();
}

void PPO_TableHistogramPS::SetTableColumns()
{

    //tableDataOK = tableInputObjRefUI.DataObjAvail();  //if available data object not okay, will crash
    tableDataOK = tableInputObjRefUI.CheckDataObj();
    DO_TableData* dataDO;
    if (tableDataOK)
    {
        dataDO = static_cast<DO_TableData*>(tableInputObjRefUI.GetDataObj());
        tableDataOK = dataDO->tableData->columnDesc.Size() > 0;
    }

    if (tableDataOK)
    {
        const SC_StringArray& colDesc = dataDO->tableData->columnDesc;
        xDataIndxUI.SetListText(colDesc);
        yDataIndxUI.SetListText(colDesc);
    }
    else
    {
        xDataIndxUI.SetListNA();
        yDataIndxUI.SetListNA();
    }
    SetAllSensitive();
}


void PPO_TableHistogramPS::InitMenu(const PPO_TableHistogram& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    firstRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    tableInputObjRefUI.InitFramedUI("Table Data to Plot", firstRow.GetWidget(), args);
    tableInputObjRefUI.AddOtherObjectCallback(SetTableColumnsCB, this);

    args.StdSplitFinish(35);
    histogramPenUI.InitFramedUI("Histogram Pen", firstRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(firstRow.GetWidget());
    secondRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    xDataIndxUI.topOffset = 0;
    xDataIndxUI.listWidth = 20;
    xDataIndxUI.InitFramedUI("X Data Column", secondRow.GetWidget(), args);

    args.StdSplitFinish();
    yDataIndxUI.topOffset = 0;
    yDataIndxUI.listWidth = 20;
    yDataIndxUI.InitFramedUI("Y Data Column", secondRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(secondRow.GetWidget());
    histogramUI.StdInit(PPform.GetWidget(), args);
    histogramUI.positionIsValueUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(histogramUI.edgeSpecUI);
    legendDataUI.InitFramedUI("Legend", PPform.GetWidget(), args);
    legendDataUI.overrideDefaultLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
}

void PPO_TableHistogramPS::MenuOpen(PPO_TableHistogram&   inData)
{
    MenuBaseC::MenuOpen(true, false);
    tableInputObjRefUI.StdMenuInit(inData.tableInputObjRef);
    histogramPenUI.ResetPens(inData.objPenSet);
    histogramUI.MenuOpen(inData, inData);
}

void PPO_TableHistogramPS::SetMenuData(PPO_TableHistogram&    inData)
{
    PPO_BasePS::SetMenuData(inData);
    tableInputObjRefUI.SetFuncObjRef(inData.tableInputObjRef);
    xDataIndxUI.SetSelection(inData.xDataIndx);
    yDataIndxUI.SetSelection(inData.yDataIndx);
    histogramPenUI.SetPenSelection(inData.histogramPen);
    histogramUI.SetHistogram(inData);
    legendDataUI.SetLegendOverride(inData.legendData);

    SetTableColumns();
}

bool PPO_TableHistogramPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    tableInputObjRefUI.FuncObjRefOK(allOK, "table Data");

    histogramUI.UIdataOK(allOK);

    legendDataUI.UIdataOK(allOK);

    return allOK;
}

void PPO_TableHistogramPS::GetMenuData(PPO_TableHistogram&    outData)
{
    PPO_BasePS::GetMenuData(outData);
    outData.tableInputObjRef = tableInputObjRefUI.GetFuncObjRef();
    outData.xDataIndx = xDataIndxUI.GetSelection();
    outData.yDataIndx = yDataIndxUI.GetSelection();

    outData.histogramPen = histogramPenUI.GetPenSelection();

    histogramUI.GetHistogram(outData);

    legendDataUI.GetLegendOverride(outData.legendData);
}


