///////////////////////////////////////////////////////////////////////////////////
//
// PPO_TableColorSeriesPS.cpp
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

#include "PPO_TableColorSeriesPS.h"

// ****************************************

PPO_TableColorSeriesPS::PPO_TableColorSeriesPS() :
    PPO_ColorBasePS(typeid(DO_TableData)),
    tableDataOK(false)
{
}

void PPO_TableColorSeriesPS::SetAllSensitive()
{
    xDataIndxUI.SetSensitive(tableDataOK);
    yDataIndxUI.SetSensitive(tableDataOK);

    if (is3D)
        zDataIndxUI.SetSensitive(tableDataOK);
    cDataIndxUI.SetSensitive(tableDataOK);
    lastNRowsUI.SetSensitive(!plotAllRowsUI.GetValue());
    pointSymbolUI.SetSymbolSize(pointSymSizeUI.GetSymbolSize());

    edgeSpecUI.SetInternalSensitive();
}

void PPO_TableColorSeriesPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_TableColorSeriesPS*>(inObj)->SetAllSensitive();
}


void PPO_TableColorSeriesPS::SetTableColumnsCB(void* inObj)

{
    static_cast<PPO_TableColorSeriesPS*>(inObj)->SetTableColumns();
}



void PPO_TableColorSeriesPS::SetTableColumns()
{

    tableDataOK = inputDataObjRefUI.CheckDataObj();
    DO_TableData* dataDO;
    if (tableDataOK)
    {
        dataDO = static_cast<DO_TableData*>(inputDataObjRefUI.GetDataObj());
        tableDataOK = dataDO->tableData->columnDesc.Size() > 0;
    }

    if (tableDataOK)
    {
        const SC_StringArray& colDesc = dataDO->GetTable().columnDesc;
        xDataIndxUI.SetListText(colDesc);
        yDataIndxUI.SetListText(colDesc);
        zDataIndxUI.SetListText(colDesc);
        cDataIndxUI.SetListText(colDesc);
    }
    else
    {
        xDataIndxUI.SetListNA();
        yDataIndxUI.SetListNA();
        zDataIndxUI.SetListNA();
        cDataIndxUI.SetListNA();
    }
    SetAllSensitive();
}


void PPO_TableColorSeriesPS::InitMenu(const PPO_TableColorSeries&   inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    inputDataObjRefUI.topOffset = 0;
    inputDataObjRefUI.InitFramedUI("Table Data", topRow.GetWidget(), args);
    inputDataObjRefUI.AddOtherObjectCallback(SetTableColumnsCB, this);

    args.StdSplitFinish();
    xDataIndxUI.topOffset = 0;
    xDataIndxUI.listWidth = 20;
    xDataIndxUI.InitFramedUI("X Data Column", topRow.GetWidget(), args);

    args.StdTopObjectAttach(topRow.GetWidget());
    nextRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    yDataIndxUI.topOffset = 0;
    yDataIndxUI.listWidth = 20;
    yDataIndxUI.InitFramedUI("Y Data Column", nextRow.GetWidget(), args);

    if (inData.is3DObject)
    {
        args.StdSplitFinish();
        zDataIndxUI.topOffset = 0;
        zDataIndxUI.listWidth = 20;
        zDataIndxUI.InitFramedUI("Z Data Column", nextRow.GetWidget(), args);

        args.StdTopObjectAttach(nextRow.GetWidget());
        colorRow.InitWidget(PPform.GetWidget(), args);

        args.TopFormAttach();
        args.LeftFormAttach(0);
        cDataIndxUI.listWidth = 20;
        cDataIndxUI.InitFramedUI("Color Data Column", colorRow.GetWidget(), args);

        args.StdTopObjectAttach(colorRow.GetWidget());

    }
    else
    {
        args.StdSplitFinish();
        cDataIndxUI.topOffset = 0;
        cDataIndxUI.listWidth = 20;
        cDataIndxUI.InitFramedUI("Color Data Column", nextRow.GetWidget(), args);

        args.StdTopObjectAttach(nextRow.GetWidget());
    }

    //args.StdTopObjectAttach(nextRow.GetWidget());
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    reversePlotOrderUI.StdToggleInit("Reverse plot order", optionsFrame.GetForm(), args);

    args.StdToggleSetup(25);
    plotAllRowsUI.StdToggleInit("Plot all rows", optionsFrame.GetForm(), args);
    plotAllRowsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    lastNRowsUI.InitLabeledUI("# of rows to plot", optionsFrame.GetForm(), args);

    //color limits
    args.StdTopObjectAttach(optionsFrame.GetForm());
    const char* viewStrings[] = {"All data", "Symbols in View"};
    PPO_ColorBasePS::InitMenu(inData, args, viewStrings, 2);

    //symbols
    args.StdTopObjectAttach(outsideForm.GetWidget());
    symFrame.InitFramedUI("Symbol", PPform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftPosAttach(20);
    pointSymbolUI.InitWidget(symFrame.GetForm(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftPosAttach(40);
    pointSymSizeUI.InitWidget(symFrame.GetForm(), args);
    pointSymSizeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    //  edges
    args.StdTopObjectAttach(symFrame);
    edgeSpecUI.StdInit(PPform.GetWidget(), args, inData.is3DObject);
    edgeSpecUI.plotEdgesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    if (inData.is3DObject)
    {
        args.StdTopObjectAttach(edgeSpecUI);
        offsetCoordUI.StdInit("Offset", PPform.GetWidget(), args);
    }

}


void PPO_TableColorSeriesPS::MenuOpen(PPO_TableColorSeries& inData)
{
    PPO_ColorBasePS::MenuOpen(inData);
    edgeSpecUI.edgePenUI.ResetPens(inData.objPenSet);
}

void PPO_TableColorSeriesPS::SetMenuData(PPO_TableColorSeries&  inData)
{
    PPO_ColorBasePS::SetMenuData(inData, inData);

    is3D = inData.is3DObject;

    xDataIndxUI.SetSelection(inData.xDataIndx);
    yDataIndxUI.SetSelection(inData.yDataIndx);
    cDataIndxUI.SetSelection(inData.colorDataIndx);

    if (is3D)
        zDataIndxUI.SetSelection(inData.zDataIndx);

    plotAllRowsUI.SetValue(inData.plotAllRows);
    lastNRowsUI.SetFieldValue(inData.lastNRows);
    reversePlotOrderUI.SetValue(inData.reversePlotOrder);

    pointSymbolUI.SetSymbolType(inData.pointSymbol);
    pointSymSizeUI.SetSymbolSize(inData.pointSymSize);

    edgeSpecUI.SetEdgeSpec(inData.edgeSpec);

    SetTableColumns();
}

bool PPO_TableColorSeriesPS::UIdataOK()
{
    PPO_ColorBasePS::UIdataOK();
    if (!plotAllRowsUI.GetValue())
        lastNRowsUI.FieldValueOK(allOK, "# of rows", 1, 10000);
    edgeSpecUI.UIdataOK(allOK);
    return allOK;
}

void PPO_TableColorSeriesPS::GetMenuData(PPO_TableColorSeries&  outData)
{
    PPO_ColorBasePS::GetMenuData(outData, outData);

    outData.xDataIndx = xDataIndxUI.GetSelection();
    outData.yDataIndx = yDataIndxUI.GetSelection();
    outData.colorDataIndx = cDataIndxUI.GetSelection();
    if (outData.is3DObject)
        outData.zDataIndx = zDataIndxUI.GetSelection();

    outData.reversePlotOrder = reversePlotOrderUI.GetValue();
    outData.plotAllRows = plotAllRowsUI.GetValue();
    if (!outData.plotAllRows)
        outData.lastNRows = lastNRowsUI.GetFieldValue();

    outData.pointSymbol = pointSymbolUI.GetSymbolType();
    outData.pointSymSize = pointSymSizeUI.GetSymbolSize();

    edgeSpecUI.GetEdgeSpec(outData.edgeSpec);
}

void PPO_TableColorSeriesPS::MenuDefault()
{
    PPO_BasePS::MenuDefault();
    SetAllSensitive();
}

