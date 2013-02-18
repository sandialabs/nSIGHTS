///////////////////////////////////////////////////////////////////////////////////
//
// PPO_TableErrorBarsPS.cpp
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

#include "PPO_TableErrorBars.h"
#include "PPO_TableErrorBarsPS.h"


PPO_TableErrorBarsPS::PPO_TableErrorBarsPS() :
    tableInputObjRefUI(typeid(DO_TableData))
{
}

void PPO_TableErrorBarsPS::SetAllSensitive()
{
    legendDataUI.SetSensitive(true);

    bool isLine = errorBarFormatUI.GetChoice() == 0;
    errorBarTypeUI.SetSensitive(isLine);
    crossTickFrame.SetSensitive(isLine);
    if (isLine)
        crossTickPixelsUI.SetSensitive(lineHasCrossTickUI.GetValue());

    if (!tableDataOK)
    {
        xdataIndxUI.SetSensitive(false);
        ydataIndxUI.SetSensitive(false);
        dxIndxUI.SetSensitive(false);
        dyIndxUI.SetSensitive(false);

        xminIndxUI.SetSensitive(false);
        xmaxIndxUI.SetSensitive(false);
        yminIndxUI.SetSensitive(false);
        ymaxIndxUI.SetSensitive(false);
        return;
    }

    bool isDelta = errorBarDataUI.GetChoice() == 0;
    bool isMinMax = !isDelta;

    if ((!isLine) || (errorBarTypeUI.GetChoice() == 0))
    {
        xdataIndxUI.SetSensitive(isDelta || isLine);
        ydataIndxUI.SetSensitive(isDelta || isLine);
        dxIndxUI.SetSensitive(isDelta);
        dyIndxUI.SetSensitive(isDelta);

        xminIndxUI.SetSensitive(isMinMax);
        xmaxIndxUI.SetSensitive(isMinMax);
        yminIndxUI.SetSensitive(isMinMax);
        ymaxIndxUI.SetSensitive(isMinMax);
    }
    else
    {
        if (errorBarTypeUI.GetChoice() == 1)
        {
            xdataIndxUI.SetSensitive(isDelta);
            ydataIndxUI.SetSensitive(true);
            dxIndxUI.SetSensitive(isDelta);
            dyIndxUI.SetSensitive(false);
            xminIndxUI.SetSensitive(isMinMax);
            xmaxIndxUI.SetSensitive(isMinMax);
            yminIndxUI.SetSensitive(false);
            ymaxIndxUI.SetSensitive(false);
        }
        else
        {
            ydataIndxUI.SetSensitive(isDelta);
            xdataIndxUI.SetSensitive(true);
            dyIndxUI.SetSensitive(isDelta);
            dxIndxUI.SetSensitive(false);
            yminIndxUI.SetSensitive(isMinMax);
            ymaxIndxUI.SetSensitive(isMinMax);
            xminIndxUI.SetSensitive(false);
            xmaxIndxUI.SetSensitive(false);
        }
    }
}

void PPO_TableErrorBarsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_TableErrorBarsPS*>(inObj)->SetAllSensitive();
}

void PPO_TableErrorBarsPS::SetTableColumnsCB(void* inObj)

{
    static_cast<PPO_TableErrorBarsPS*>(inObj)->SetTableColumns();
}

void PPO_TableErrorBarsPS::SetTableColumns()
{
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
        xdataIndxUI.SetListText(colDesc);
        ydataIndxUI.SetListText(colDesc);
        dxIndxUI.SetListText(colDesc);
        dyIndxUI.SetListText(colDesc);
        xminIndxUI.SetListText(colDesc);
        yminIndxUI.SetListText(colDesc);
        xmaxIndxUI.SetListText(colDesc);
        ymaxIndxUI.SetListText(colDesc);
    }
    else
    {
        xdataIndxUI.SetListNA();
        ydataIndxUI.SetListNA();
        dxIndxUI.SetListNA();
        dyIndxUI.SetListNA();
        xminIndxUI.SetListNA();
        yminIndxUI.SetListNA();
        xmaxIndxUI.SetListNA();
        ymaxIndxUI.SetListNA();
    }
    SetAllSensitive();
}


void PPO_TableErrorBarsPS::InitMenu(const PPO_TableErrorBars& inData)
{
    PPO_BasePS::InitMenu(inData);


    ArgListC    args;

    args.StdFormAttach();
    tableInputObjRefUI.InitFramedUI("Table Data to Plot", PPform.GetWidget(), args);
    tableInputObjRefUI.AddOtherObjectCallback(SetTableColumnsCB, this);

    args.StdTopObjectAttach(tableInputObjRefUI);
    firstRow.InitWidget(PPform.GetWidget(), args);

    args.StdEvenSplitSetup(0, 3);
    const char* ebfStrs[] = {" Line ", " Box "};
    errorBarFormatUI.PreInitSetUp(ebfStrs, 2);
    errorBarFormatUI.InitFramedUI("Format", firstRow.GetWidget(), args);
    errorBarFormatUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdEvenSplitSetup(1, 3);
    const char* ebdStrs[] = {" Delta X/Y ", " X/Y Limits "};
    errorBarDataUI.PreInitSetUp(ebdStrs, 2);
    errorBarDataUI.InitFramedUI("Data", firstRow.GetWidget(), args);
    errorBarDataUI.AddOtherObjectCallback(SetAllSensitiveCB, this);


    args.StdEvenSplitSetup(2, 3);
    const char* ebtStrs[] = {" X and Y ", " X only ", " Y only "};
    errorBarTypeUI.PreInitSetUp(ebtStrs, 3);
    errorBarTypeUI.InitFramedUI("Type", firstRow.GetWidget(), args);
    errorBarTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(firstRow.GetWidget());
    secondRow.InitWidget(PPform.GetWidget(), args);
    args.StdEvenSplitSetup(0, 4);
    xdataIndxUI.InitFramedUI("X Value", secondRow.GetWidget(), args);
    args.StdEvenSplitSetup(1, 4);
    ydataIndxUI.InitFramedUI("Y Value", secondRow.GetWidget(), args);
    args.StdEvenSplitSetup(2, 4);
    dxIndxUI.InitFramedUI("Delta X", secondRow.GetWidget(), args);
    args.StdEvenSplitSetup(3, 4);
    dyIndxUI.InitFramedUI("Delta Y", secondRow.GetWidget(), args);

    args.StdTopObjectAttach(secondRow.GetWidget());
    thirdRow.InitWidget(PPform.GetWidget(), args);
    args.StdEvenSplitSetup(0, 4);
    xminIndxUI.InitFramedUI("X Minimum", thirdRow.GetWidget(), args);
    args.StdEvenSplitSetup(1, 4);
    xmaxIndxUI.InitFramedUI("X Maximum", thirdRow.GetWidget(), args);
    args.StdEvenSplitSetup(2, 4);
    yminIndxUI.InitFramedUI("Y Minimum", thirdRow.GetWidget(), args);
    args.StdEvenSplitSetup(3, 4);
    ymaxIndxUI.InitFramedUI("Y Maximum", thirdRow.GetWidget(), args);

    args.StdTopObjectAttach(thirdRow.GetWidget());
    fourthRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    barLineSpecUI.InitFramedUI("Bar/Box Line", fourthRow.GetWidget(), args);

    args.StdSplitFinish(35);
    crossTickFrame.InitFramedUI("Line Cross Tick", fourthRow.GetWidget(), args);

    args.StdToggleSetup(5);
    lineHasCrossTickUI.StdToggleInit("cross-tick", crossTickFrame.GetForm(), args);
    lineHasCrossTickUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(65);
    crossTickPixelsUI.PreInitSetUp();
    crossTickPixelsUI.InitWidget(crossTickFrame.GetForm(), args);

    args.StdTopObjectAttach(fourthRow.GetWidget());
    legendDataUI.InitFramedUI("Legend", PPform.GetWidget(), args);
    legendDataUI.overrideDefaultLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
}

void PPO_TableErrorBarsPS::MenuOpen(PPO_TableErrorBars&   inData)
{
    MenuBaseC::MenuOpen(true, false);
    tableInputObjRefUI.StdMenuInit(inData.tableInputObjRef);
    barLineSpecUI.ResetPens(inData.objPenSet);
}

void PPO_TableErrorBarsPS::SetMenuData(PPO_TableErrorBars&    inData)
{
    PPO_BasePS::SetMenuData(inData);
    tableInputObjRefUI.SetFuncObjRef(inData.tableInputObjRef);

    errorBarFormatUI.SetChoice(int(inData.errorBarFormat));
    errorBarDataUI.SetChoice(int(inData.errorBarData));
    errorBarTypeUI.SetChoice(int(inData.errorBarType));

    xdataIndxUI.SetSelection(inData.xdataIndx);
    ydataIndxUI.SetSelection(inData.ydataIndx);
    dxIndxUI.SetSelection(inData.dxIndx);
    dyIndxUI.SetSelection(inData.dyIndx);

    xminIndxUI.SetSelection(inData.xminIndx);
    xmaxIndxUI.SetSelection(inData.xmaxIndx);
    yminIndxUI.SetSelection(inData.yminIndx);
    ymaxIndxUI.SetSelection(inData.ymaxIndx);

    barLineSpecUI.SetLineSpec(inData.barLineSpec);
    lineHasCrossTickUI.SetValue(inData.lineHasCrossTick);
    crossTickPixelsUI.SetSymbolSize(inData.crossTickPixels);

    legendDataUI.SetLegendOverride(inData.legendData);

    SetTableColumns();
}

bool PPO_TableErrorBarsPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    tableInputObjRefUI.FuncObjRefOK(allOK, "table Data");

    legendDataUI.UIdataOK(allOK);

    return allOK;
}

void PPO_TableErrorBarsPS::GetMenuData(PPO_TableErrorBars&    outData)
{
    PPO_BasePS::GetMenuData(outData);
    outData.tableInputObjRef = tableInputObjRefUI.GetFuncObjRef();

    outData.errorBarFormat = PFO_TableErrorBars::ErrorBarFormat(errorBarFormatUI.GetChoice());
    outData.errorBarData = PFO_TableErrorBars::ErrorBarData(errorBarDataUI.GetChoice());
    if (outData.errorBarFormat != PFO_TableErrorBars::ebfBox)
        outData.errorBarType = PFO_TableErrorBars::ErrorBarType(errorBarTypeUI.GetChoice());

    if (tableDataOK)
    {
        outData.xdataIndx = xdataIndxUI.GetSelection();
        outData.ydataIndx = ydataIndxUI.GetSelection();
        outData.dxIndx = dxIndxUI.GetSelection();
        outData.dyIndx = dyIndxUI.GetSelection();

        outData.xminIndx = xminIndxUI.GetSelection();
        outData.xmaxIndx = xmaxIndxUI.GetSelection();
        outData.yminIndx = yminIndxUI.GetSelection();
        outData.ymaxIndx = ymaxIndxUI.GetSelection();
    }

    barLineSpecUI.GetLineSpec(outData.barLineSpec);
    outData.lineHasCrossTick = lineHasCrossTickUI.GetValue();
    outData.crossTickPixels = crossTickPixelsUI.GetSymbolSize();

    legendDataUI.GetLegendOverride(outData.legendData);
}


