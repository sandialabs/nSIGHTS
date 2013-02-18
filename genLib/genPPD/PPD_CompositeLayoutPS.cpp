///////////////////////////////////////////////////////////////////////////////////
//
// PPD_CompositeLayoutPS.cpp
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
//      describes layout of sub-plots on composite plot.
//
//      adds components for composite plots.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass\U_Real.h>
#include <genClass\U_Str.h>

#include "PPD_CompositeLayoutPS.h"


PPD_CompositeLayoutPS::PlotLayoutWidget::PlotLayoutWidget()
{
}

void PPD_CompositeLayoutPS::PlotLayoutWidget::SetSensitive()
{
    bool useLayout = usePlotLayoutUI.GetValue();
    xOffsetUI.SetSensitive(useLayout);
    yOffsetUI.SetSensitive(useLayout);
    xWidthUI.SetSensitive(useLayout);
    yWidthUI.SetSensitive(useLayout);
}

void PPD_CompositeLayoutPS::PlotLayoutWidget::SetSensitiveCB(void* inObj)
{
    static_cast<PPD_CompositeLayoutPS::PlotLayoutWidget*>(inObj)->SetSensitive();
}

void PPD_CompositeLayoutPS::PlotLayoutWidget::InitPlotLayout()
{
    ArgListC    args;
    args.StdToggleSetup(5);
    usePlotLayoutUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    usePlotLayoutUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdSplitSetup();
    args.StdSplitMiddle(10, 30);
    xOffsetUI.SetFieldLen(6);
    xOffsetUI.SetFieldFormat(SC_DecFormat(1));
    xOffsetUI.AttachObjectToLabel();
    xOffsetUI.InitLabeledUI("XOff:", outsideForm.GetWidget(), args);

    args.StdSplitMiddle(33, 53);
    yOffsetUI.SetFieldLen(6);
    yOffsetUI.SetFieldFormat(SC_DecFormat(1));
    yOffsetUI.AttachObjectToLabel();
    yOffsetUI.InitLabeledUI("YOff:", outsideForm.GetWidget(), args);

    args.StdSplitMiddle(55, 75);
    xWidthUI.SetFieldLen(6);
    xWidthUI.SetFieldFormat(SC_DecFormat(1));
    xWidthUI.AttachObjectToLabel();
    xWidthUI.InitLabeledUI("XWid:", outsideForm.GetWidget(), args);

    args.StdSplitMiddle(78, 100);
    yWidthUI.SetFieldLen(6);
    yWidthUI.SetFieldFormat(SC_DecFormat(1));
    yWidthUI.AttachObjectToLabel();
    yWidthUI.InitLabeledUI("YWid:", outsideForm.GetWidget(), args);
}


void PPD_CompositeLayoutPS::PlotLayoutWidget::SetPlotLayout(       bool            useLayout,
                                                             const PSC_PlotLayout& layoutData)
{
    usePlotLayoutUI.SetValue(useLayout);

    xOffsetUI.SetFieldValue(layoutData.xOffset);
    yOffsetUI.SetFieldValue(layoutData.yOffset);
    xWidthUI.SetFieldValue(layoutData.xWidth);
    yWidthUI.SetFieldValue(layoutData.yWidth);
}

bool PPD_CompositeLayoutPS::PlotLayoutWidget::UIdataOK()
{
    if (!usePlotLayoutUI.GetValue())
        return true;

    bool allOK = true;

    xOffsetUI.FieldValueOK(allOK, "X Offset", 0.0, 95.0);
    yOffsetUI.FieldValueOK(allOK, "Y Offset", 0.0, 95.0);
    xWidthUI.FieldValueOK(allOK, "X Width", 5.0, 100.0);
    yWidthUI.FieldValueOK(allOK, "Y Width", 5.0, 100.0);

    if (allOK && ((xOffsetUI.GetFieldValue() + xWidthUI.GetFieldValue()) > 100.0))
    {
        PropErrorMsg("X offset + X width must be <= 100.0");
        allOK = false;
    }

    if (allOK && ((yOffsetUI.GetFieldValue() + yWidthUI.GetFieldValue()) > 100.0))
    {
        PropErrorMsg("Y offset + Y width must be <= 100.0");
        allOK = false;
    }

    return allOK;
}

void PPD_CompositeLayoutPS::PlotLayoutWidget::GetPlotLayout(bool&           useLayout,
                                                            PSC_PlotLayout& layoutData)
{
    useLayout = usePlotLayoutUI.GetValue();
    if (useLayout)
    {
        layoutData.xOffset = xOffsetUI.GetFieldValue();
        layoutData.yOffset = yOffsetUI.GetFieldValue();
        layoutData.xWidth = xWidthUI.GetFieldValue();
        layoutData.yWidth = yWidthUI.GetFieldValue();
    }
}


CSize PPD_CompositeLayoutPS::PlotLayoutScrolledForm::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = LayoutManager::SetWindowPosX(0, 600);
    szForm.cy = LayoutManager::SetWindowPosY(0, 1000);
    return szForm;
}

PPD_CompositeLayoutPS::PPD_CompositeLayoutPS()
{
    rowSpacingUI.AllocAndCreate(maxRowCol);
    colSpacingUI.AllocAndCreate(maxRowCol);
}

PPD_CompositeLayoutPS::~PPD_CompositeLayoutPS()
{
}

void PPD_CompositeLayoutPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPD_CompositeLayoutPS*>(inObj)->SetAllSensitive();
}

void PPD_CompositeLayoutPS::GetNRowsCols(int& nrows, int& ncols)
{
    nrows = 1;
    bool wasOK = true;
    if (nlayoutRowsUI.FieldValueOK(wasOK, "# rows", 1, maxRowCol))
        nrows = nlayoutRowsUI.GetFieldValue();

    ncols = 1;
    wasOK = true;
    if (nlayoutColsUI.FieldValueOK(wasOK, "# cols", 1, maxRowCol))
        ncols = nlayoutColsUI.GetFieldValue();
}


void PPD_CompositeLayoutPS::SetAllSensitive()
{
    if (doingFreeForm)
    {
        for (int i = 0; i < PD_Composite::maxCustomLayout; i++)
            layoutData[i].SetSensitive();
    }
    else
    {
        int nrows, ncols;
        GetNRowsCols(nrows, ncols);

        for (int i = 0; i < maxRowCol; i++)
        {
            rowSpacingUI.GetRef(i).SetSensitive(i < nrows);
            colSpacingUI.GetRef(i).SetSensitive(i < ncols);
        }
    }
}

void PPD_CompositeLayoutPS::InitMenu(const PD_Composite&    inData)
{
    MenuBaseC::InitMenu();

    doingFreeForm = !inData.autoLayout;

    if (doingFreeForm)
        InitFreeFormMenu();
    else
        InitRowColumnMenu();
}


void PPD_CompositeLayoutPS::InitFreeFormMenu()
{
    ArgListC args;

    args.StdFormAttach();
    scrollArea.InitWidget(PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    for (int i = 0; i < PD_Composite::maxCustomLayout; i++)
    {
        layoutData[i].outsideForm.InitWidget(scrollArea, args);
        layoutData[i].InitPlotLayout();

        args.Clear();
        args.StdTopObjectAttach(layoutData[i].outsideForm.GetWidget());
    }
}


void PPD_CompositeLayoutPS::InitRowColumnMenu()
{
    ArgListC args;

    args.StdSplitSetup();
    nlayoutRowsUI.InitFramedUI("# of Rows", URform.GetWidget(), args);
    nlayoutRowsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    nlayoutColsUI.InitFramedUI("# of Columns", URform.GetWidget(), args);
    nlayoutColsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdFormAttach();
    rowLayoutFrame.InitFramedUI("Row spacing", PPform.GetWidget(), args);

    rowForms[0].InitWidget(rowLayoutFrame.GetForm(), args);
    int nextRow = 0;
    int nextCol = 0;
    int nextIndex = 0;
    for (int i = 0; i < maxRowCol; i++)
    {
        if (nextCol == 0)
            args.StdSplitSetup(25);
        else if (nextCol == 3)
            args.StdSplitFinish(25);
        else
            args.StdSplitMiddle(nextCol * 25, (nextCol + 1) * 25);

        RealEntryUIC& currUI =  rowSpacingUI.GetRef(i);
        currUI.SetFieldFormat(SC_DecFormat(1));
        currUI.SetFieldLen(5);
        currUI.AttachObjectToLabel();

        CopyString(rowIDs[i], "R#", rowColLabelLen);
        ConcatInt(i + 1, rowIDs[i], rowColLabelLen);
        currUI.InitLabeledUI(rowIDs[i], rowForms[nextRow].GetWidget(), args);

        nextCol++;
        if (nextCol > 3)
        {
            args.Clear();
            args.StdTopObjectAttach(rowForms[nextRow].GetWidget());
            nextRow++;
            rowForms[nextRow].InitWidget(rowLayoutFrame.GetForm(), args);
            nextCol = 0;
        }
    }


    args.Clear();
    args.StdTopObjectAttach(rowLayoutFrame);
    colLayoutFrame.InitFramedUI("Column spacing", PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    colForms[0].InitWidget(colLayoutFrame.GetForm(), args);
    nextRow = 0;
    nextCol = 0;
    nextIndex = 0;
    for (int i = 0; i < maxRowCol; i++)
    {
        if (nextCol == 0)
            args.StdSplitSetup(25);
        else if (nextCol == 3)
            args.StdSplitFinish(25);
        else
            args.StdSplitMiddle(nextCol * 25, (nextCol + 1) * 25);

        RealEntryUIC& currUI =  colSpacingUI.GetRef(i);
        currUI.SetFieldFormat(SC_DecFormat(1));
        currUI.SetFieldLen(5);
        currUI.AttachObjectToLabel();
        CopyString(colIDs[i], "C#", rowColLabelLen);
        ConcatInt(i + 1, colIDs[i], rowColLabelLen);
        currUI.InitLabeledUI(colIDs[i], colForms[nextRow].GetWidget(), args);

        nextCol++;
        if (nextCol > 3)
        {
            args.Clear();
            args.StdTopObjectAttach(colForms[nextRow].GetWidget());
            nextRow++;
            colForms[nextRow].InitWidget(colLayoutFrame.GetForm(), args);
            nextCol = 0;
        }
    }


    SetMainformScrolled(true, 520, 330);
}

void PPD_CompositeLayoutPS::MenuOpen(PD_Composite&  inPD)
{
    MenuBaseC::MenuOpen(true, true);
}


void PPD_CompositeLayoutPS::SetMenuData(const PD_Composite&  inData,
                                            MenuObjC& inObj)
{
    MenuBaseC::SetMenuData(inObj);

    if (doingFreeForm)
    {
        for (int i = 0; i < PD_Composite::maxCustomLayout; i++)
            layoutData[i].SetPlotLayout(inData.useCustomLayout[i], inData.customLayout[i]);

    }
    else
    {
        nlayoutRowsUI.SetFieldValue(inData.nlayoutRows);
        nlayoutColsUI.SetFieldValue(inData.nlayoutCols);

        for (int i = 0; i < maxRowCol; i++)
        {
            rowSpacingUI.GetRef(i).SetFieldValue(inData.rowSpacing[i]);
            colSpacingUI.GetRef(i).SetFieldValue(inData.colSpacing[i]);
        }
    }
    SetAllSensitive();
}

bool PPD_CompositeLayoutPS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    if (doingFreeForm)
    {
        for (int i = 0; i < PD_Composite::maxCustomLayout; i++)
            allOK = allOK && layoutData[i].UIdataOK();
    }
    else
    {
        int nrows, ncols;
        GetNRowsCols(nrows, ncols);
        for (int i = 0; i < maxRowCol; i++)
        {
            if (i < nrows)
                rowSpacingUI.GetRef(i).FieldValueOK(allOK, "row data", 5.0, 100.0);
            if (i < ncols)
                colSpacingUI.GetRef(i).FieldValueOK(allOK, "col data", 5.0, 100.0);
        }
    }
    return allOK;
}

void PPD_CompositeLayoutPS::GetMenuData(PD_Composite& outData,
                                        MenuObjC&       outObj)
{
    MenuBaseC::GetMenuData(outObj);
    if (doingFreeForm)
    {
        for (int i = 0; i < PD_Composite::maxCustomLayout; i++)
            layoutData[i].GetPlotLayout(outData.useCustomLayout[i], outData.customLayout[i]);
    }
    else
    {
        outData.nlayoutRows = nlayoutRowsUI.GetFieldValue();
        outData.nlayoutCols = nlayoutColsUI.GetFieldValue();

        for (int i = 0; i < maxRowCol; i++)
        {
            if (i < outData.nlayoutRows)
                outData.rowSpacing[i] = rowSpacingUI.GetRef(i).GetFieldValue();
            if (i < outData.nlayoutCols)
                outData.colSpacing[i] = colSpacingUI.GetRef(i).GetFieldValue();
        }
    }
}

void PPD_CompositeLayoutPS::MenuDefault()
{
    if (doingFreeForm)
    {
        layoutData[0].SetPlotLayout(true, PSC_PlotLayout());
        for (int i = 1; i < PD_Composite::maxCustomLayout; i++)
            layoutData[i].SetPlotLayout(false, PSC_PlotLayout());

    }
    else
    {
        int nrows, ncols;
        GetNRowsCols(nrows, ncols);
        for (int i = 0; i < maxRowCol; i++)
        {
            rowSpacingUI.GetRef(i).SetFieldValue(floor(100.0 / double(nrows)));
            colSpacingUI.GetRef(i).SetFieldValue(floor(100.0 / double(ncols)));
        }
    }
    SetAllSensitive();
}

void PPD_CompositeLayoutPS::MenuClear()
{
}

