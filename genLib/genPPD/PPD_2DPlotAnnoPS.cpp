///////////////////////////////////////////////////////////////////////////////////
//
// PPD_2DPlotAnnoPS.cpp
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
//      axes and increment annotation and formatting for a 2D plot
//
//      derived from PPD_Base.  Adds components for all 2D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "PPD_2DPlotAnnoPS.h"

PPD_2DPlotAnnoPS::PPD_2DPlotAnnoPS()
{
}

void PPD_2DPlotAnnoPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPD_2DPlotAnnoPS*>(inObj)->SetAllSensitive();
}

void PPD_2DPlotAnnoPS::SetAllSensitive()
{
    int annoChoice = plotAnnoUI.GetChoice();
    bool doX = (annoChoice == 0) || (annoChoice == 1);
    bool doY = (annoChoice == 0) || (annoChoice == 2);

    if  (annoChoice == 3)
    {
        formatFrame.SetSensitive(false);
        labelFrame.SetSensitive(false);
        axesPenOption.SetSensitive(false);
        gridPenOption.SetSensitive(false);
        return;
    }

    formatFrame.SetSensitive(true);
    axesPenOption.SetSensitive(true);
    gridPenOption.SetSensitive(true);

    majorGridType.SetLineColorAndThick(gridPenOption.GetSelectedColorValue(), majorGridThk.GetLineThick());
    minorGridType.SetLineColorAndThick(gridPenOption.GetSelectedColorValue(), minorGridThk.GetLineThick());

    bool axesLabel = axesLabelToggle.GetValue();
    labelFrame.SetSensitive(axesLabel);
    xlabelField.SetSensitive(axesLabel && doX);
    ylabelField.SetSensitive(axesLabel && doY);
}

void PPD_2DPlotAnnoPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC   args;

    args.StdSplitSetup(30);

    static const char* paStrs[] = {"Both X and Y", "X Axis only", "Y Axis only", "None"};
    plotAnnoUI.PreInitSetUp(paStrs, 4);
    plotAnnoUI.InitFramedUI("Control", URform.GetWidget(), args);
    plotAnnoUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(30, 65);
    axesPenOption.InitFramedUI("Axes/Tic Pen", URform.GetWidget(), args);

    args.StdSplitFinish(35);
    gridPenOption.InitFramedUI("Grid Pen", URform.GetWidget(), args);
    gridPenOption.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdFormAttach();
    formatFrame.InitFramedUI("Format", PPform.GetWidget(), args);

    args.StdToggleSetup(2);
    axesLabelToggle.StdToggleInit("Axis labels", formatFrame.GetForm(), args);
    axesLabelToggle.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(20);
    labelIncrementUI.StdToggleInit("Inc. labels", formatFrame.GetForm(), args);

    args.StdToggleSetup(40);
    frameToggle.StdToggleInit("Frame", formatFrame.GetForm(), args);

    args.StdToggleSetup(60);
    axesLineToggle.StdToggleInit("Axis lines", formatFrame.GetForm(), args);

    args.StdToggleSetup(80);
    gridOverToggle.StdToggleInit("Grid On Top", formatFrame.GetForm(), args);

    args.StdTopObjectAttach(formatFrame);
    labelFrame.InitFramedUI("Labels", PPform.GetWidget(), args);

    args.StdFormAttach();
    xlabelField.SetFieldLen(0);
    xlabelField.InitLabeledUI("X axis  ", labelFrame.GetForm(), args);

    args.StdTopObjectAttach(xlabelField);
    ylabelField.SetFieldLen(0);
    ylabelField.InitLabeledUI("Y axis  ", labelFrame.GetForm(), args);

    args.StdTopObjectAttach(labelFrame);
    labelFont.InitFramedUI("Label Font", PPform.GetWidget(), args);

    args.StdTopObjectAttach(labelFont);
    incrementFont.InitFramedUI("Increment Font", PPform.GetWidget(), args);

    //  form for misc stuff
    args.StdTopObjectAttach(incrementFont);
    miscForm.InitWidget(PPform.GetWidget(), args);

    //  do all frames first
    args.StdSplitSetup(25);
    axesFrame.InitFramedUI("Axes Line", miscForm.GetWidget(), args);

    args.StdSplitMiddle(25, 50);
    majorTicFrame.InitFramedUI("Major Tics", miscForm.GetWidget(), args);

    args.StdSplitMiddle(50, 75);
    minorTicFrame.InitFramedUI("Minor Tics", miscForm.GetWidget(), args);

    args.StdSplitFinish(25);
    gridFrame.InitFramedUI("Grid Lines", miscForm.GetWidget(), args);

    //  top row stuff
    args.StdFormAttach();

    axesOffset.SetFieldLen(3);
    axesOffset.InitLabeledUI("Offset", axesFrame.GetForm(), args);

    majorTicLen.SetFieldLen(3);
    majorTicLen.InitLabeledUI("Size", majorTicFrame.GetForm(), args);

    minorTicLen.SetFieldLen(3);
    minorTicLen.InitLabeledUI("Size", minorTicFrame.GetForm(), args);

    majorGridThk.InitLabeledUI("Major thk", gridFrame.GetForm(), args);
    majorGridThk.AddOtherObjectCallback(SetAllSensitiveCB, this);

    //  other thicknesses below
    args.StdTopObjectAttach(axesOffset);
    axesThk.InitLabeledUI("Thk", axesFrame.GetForm(), args);

    args.StdTopObjectAttach(majorTicLen);
    majorTicThk.InitLabeledUI("Thk", majorTicFrame.GetForm(), args);

    args.StdTopObjectAttach(minorTicLen);
    minorTicThk.InitLabeledUI("Thk", minorTicFrame.GetForm(), args);

    args.StdTopObjectAttach(majorGridThk);
    args.TopOffset(3);
    majorGridType.InitLabeledUI("Major type", gridFrame.GetForm(), args);

    args.StdTopObjectAttach(majorGridType);
    args.TopOffset(5);
    minorGridThk.InitLabeledUI("Minor thk", gridFrame.GetForm(), args);
    minorGridThk.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(minorGridThk);
    args.TopOffset(3);
    minorGridType.InitLabeledUI("Minor type", gridFrame.GetForm(), args);

    // and offsets
    args.StdTopObjectAttach(majorTicThk);
    majorTicOffsetUI.SetFieldLen(3);
    majorTicOffsetUI.InitLabeledUI("Offset", majorTicFrame.GetForm(), args);

    args.StdTopObjectAttach(minorTicThk);
    minorTicOffsetUI.SetFieldLen(3);
    minorTicOffsetUI.InitLabeledUI("Offset", minorTicFrame.GetForm(), args);

    SetMainformScrolled(true, 520, 330);
}

void PPD_2DPlotAnnoPS::MenuOpen(PlotDefC&  inPD)
{
    MenuBaseC::MenuOpen(true, false);
    axesPenOption.ResetPens(inPD.defaultPenSet);
    gridPenOption.ResetPens(inPD.defaultPenSet);
}


void PPD_2DPlotAnnoPS::SetMenuData(const PC_2DPlotAnno& inData)
{
    plotAnnoUI.SetChoice(inData.plotAnno   );
    axesPenOption.SetPenSelection(inData.axesDataPen    );
    gridPenOption.SetPenSelection(inData.gridLineDataPen);

    axesLabelToggle.SetValue(inData.labelAxes    );
    labelIncrementUI.SetValue(inData.labelIncrements);
    axesLineToggle.SetValue (inData.drawAxesLines);
    frameToggle.SetValue    (inData.framePlot    );
    gridOverToggle.SetValue (inData.gridOverData );

    xlabelField.SetFieldString(inData.xaxesLabel);
    ylabelField.SetFieldString(inData.yaxesLabel);

    labelFont.SetFontSettings(inData.labelFont);
    incrementFont.SetFontSettings(inData.incrementFont);

    axesThk.SetLineThick(inData.axesLineWidth );
    majorTicThk.SetLineThick(inData.majorTicWidth );
    minorTicThk.SetLineThick(inData.minorTicWidth );
    majorGridThk.SetLineThick(inData.majorGridWidth);
    minorGridThk.SetLineThick(inData.minorGridWidth);
    majorGridType.SetLineType(inData.majorGridLineType);
    minorGridType.SetLineType(inData.minorGridLineType);

    axesOffset.SetFieldValue (inData.axesOffset    );
    majorTicLen.SetFieldValue(inData.majorTicLength);
    minorTicLen.SetFieldValue(inData.minorTicLength);

    majorTicOffsetUI.SetFieldValue(inData.majorTicOffset);
    minorTicOffsetUI.SetFieldValue(inData.minorTicOffset);

    SetAllSensitive();
}

void PPD_2DPlotAnnoPS::SetMenuData(const PC_2DPlotAnno& inData,
                                        MenuObjC& inObj)
{
    SetMenuData(inData);
    MenuBaseC::SetMenuData(inObj);
}

bool PPD_2DPlotAnnoPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    axesOffset.FieldValueOK(allOK, "axes offset" , 0, 50);
    majorTicLen.FieldValueOK(allOK, "major tick length" , 0, 50);
    minorTicLen.FieldValueOK(allOK, "minor tick length" , 0, 50);
    majorTicOffsetUI.FieldValueOK(allOK, "major tick offset" , 0, 50);
    minorTicOffsetUI.FieldValueOK(allOK, "minor tick offset" , 0, 50);
    return allOK;
}

void PPD_2DPlotAnnoPS::GetMenuData(PC_2DPlotAnno& outData,
                                    MenuObjC&       outObj)
{
    MenuBaseC::GetMenuData(outObj);

    outData.plotAnno = PC_2DPlotAnno::PlotAnno(plotAnnoUI.GetChoice());

    if (outData.DoAnno())
    {
        outData.labelAxes          = axesLabelToggle.GetValue();
        outData.labelIncrements    = labelIncrementUI.GetValue();
        outData.drawAxesLines      = axesLineToggle.GetValue ();
        outData.framePlot          = frameToggle.GetValue();
        outData.gridOverData       = gridOverToggle.GetValue ();

        if (outData.labelAxes)
        {
            if (outData.DoXAxis())
                xlabelField.GetFieldString(outData.xaxesLabel, PC_2DPlotAnno::maxLabelLen);

            if (outData.DoYAxis())
                ylabelField.GetFieldString(outData.yaxesLabel, PC_2DPlotAnno::maxLabelLen);
        }
    }

    outData.axesDataPen        = axesPenOption.GetPenSelection();
    outData.gridLineDataPen    = gridPenOption.GetPenSelection();

    labelFont.GetFontSettings(outData.labelFont);
    incrementFont.GetFontSettings(outData.incrementFont);

    outData.axesLineWidth      =  axesThk.GetLineThick();

    outData.majorTicWidth      =  majorTicThk.GetLineThick();
    outData.minorTicWidth      =  minorTicThk.GetLineThick();
    outData.majorGridWidth     =  majorGridThk.GetLineThick();
    outData.minorGridWidth     =  minorGridThk.GetLineThick();

    outData.majorGridLineType = majorGridType.GetLineType();
    outData.minorGridLineType  = minorGridType.GetLineType();

    outData.axesOffset         =  axesOffset.GetFieldValue ();
    outData.majorTicLength     =  majorTicLen.GetFieldValue();
    outData.minorTicLength     =  minorTicLen.GetFieldValue();
    outData.majorTicOffset     =  majorTicOffsetUI.GetFieldValue();
    outData.minorTicOffset     =  minorTicOffsetUI.GetFieldValue();

}

void PPD_2DPlotAnnoPS::MenuDefault()
{
    SetMenuData(PC_2DPlotAnno());
}

