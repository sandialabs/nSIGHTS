///////////////////////////////////////////////////////////////////////////////////
//
// genPPOUtil.cpp
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/DO_ColorLimit.h>

#include "genPPOUtil.h"

namespace genPPOMenuUtilities {


XYLabelSpecUIC::XYLabelSpecUIC(const type_info& labType) :
    dataSource(labType)
{
}


void XYLabelSpecUIC::StdInit(Widget       parent,
                             ArgListC&    attachments)
{
    topRowForm.InitWidget(parent, attachments);

    ArgListC args;

    args.StdSplitSetup();
    dataSource.InitFramedUI("Label Data Source", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    labelPenUI.InitFramedUI("Label Pen", topRowForm.GetWidget(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    secondRowForm.InitWidget(parent, args);

    args.StdSplitSetup();
    horizontalPosUI.InitFramedUI("Horizontal Align", secondRowForm.GetWidget(), args);

    args.StdSplitFinish();
    verticalPosUI.InitFramedUI("Vertical Align", secondRowForm.GetWidget(), args);

    // add font
    args.StdTopObjectAttach(secondRowForm.GetWidget());
    labelFontUI.InitFramedUI("Label Font", parent, args);

    args.StdTopObjectAttach(labelFontUI);
    optionsFrame.InitFramedUI("Options", parent, args);

    args.StdToggleSetup(5);
    clipToAxesUI.StdToggleInit("Clip to axes", optionsFrame.GetForm(), args);

    args.StdSplitMiddle(30, 60);
    pixelOffsetUI.SetFieldLen(6);
    pixelOffsetUI.InitLabeledUI("Offset", optionsFrame.GetForm(), args);
}



void XYLabelSpecUIC::MenuOpen(PlotObjC&  inObj)
{
    labelPenUI.ResetPens(inObj.objPenSet);
}

bool XYLabelSpecUIC::UIdataOK(bool& allOK)
{
    dataSource.FuncObjRefOK(allOK, "Label source");
    pixelOffsetUI.FieldValueOK(allOK, "Offset", -100, 100);
    return allOK;
}


void XYLabelSpecUIC::SetXYLabelSpec(const PSC_XYLabelSpec& inData)
{
    labelPenUI.SetPenSelection(inData.labelPen);

    horizontalPosUI.SetHAlign(inData.horizontalPos);
    verticalPosUI.SetVAlign(inData.verticalPos);

    labelFontUI.SetFontSettings(inData.labelFont);
    labelFontUI.SetSensitive(true);

    clipToAxesUI.SetValue(inData.clipToAxes);
    pixelOffsetUI.SetFieldValue(inData.pixelOffset);
}

void XYLabelSpecUIC::GetXYLabelSpec(PSC_XYLabelSpec& outData)
{
    outData.labelPen = labelPenUI.GetPenSelection();

    outData.horizontalPos = horizontalPosUI.GetHAlign();
    outData.verticalPos = verticalPosUI.GetVAlign();

    labelFontUI.GetFontSettings(outData.labelFont);

    outData.clipToAxes = clipToAxesUI.GetValue();
    outData.pixelOffset = pixelOffsetUI.GetFieldValue();
}

XYZLabelSpecUIC::XYZLabelSpecUIC(const type_info& labType) :
    dataSource(labType)
{
}

void XYZLabelSpecUIC::SetSensitive()
{
    rotationAngleUI.SetSensitive(!autoLabelRotationUI.GetValue());
}


void XYZLabelSpecUIC::SetSensitiveCB(void* inObj)
{
    static_cast<XYZLabelSpecUIC*>(inObj)->SetSensitive();
}

void XYZLabelSpecUIC::StdInit(Widget       parent,
                             ArgListC&    attachments)
{
    topRowForm.InitWidget(parent, attachments);

    ArgListC args;

    args.StdSplitSetup();
    dataSource.InitFramedUI("Label Data Source", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    labelPenUI.InitFramedUI("Label Pen", topRowForm.GetWidget(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    secondRowForm.InitWidget(parent, args);

    args.StdSplitSetup();
    static const char* attLabels[]  = {"Auto", "Vertical", "Horizontal"};
    labelAttitudeUI.PreInitSetUp(attLabels, 3);
    labelAttitudeUI.InitFramedUI("Label Plane", secondRowForm.GetWidget(), args);

    args.StdSplitFinish();
    rotationFrame.InitFramedUI("Label XY Rotation", secondRowForm.GetWidget(), args);

    args.StdToggleSetup(5);
    autoLabelRotationUI.StdToggleInit("Auto", rotationFrame.GetForm(), args);
    autoLabelRotationUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdSplitFinish(60);
    rotationAngleUI.textFieldLen = 8;
    rotationAngleUI.InitWidget(rotationFrame.GetForm(), args);

    args.StdTopObjectAttach(secondRowForm.GetWidget());
    thirdRowForm.InitWidget(parent, args);

    args.StdSplitSetup();
    horizontalPosUI.InitFramedUI("Horizontal Align", thirdRowForm.GetWidget(), args);

    args.StdSplitFinish();
    verticalPosUI.InitFramedUI("Vertical Align", thirdRowForm.GetWidget(), args);

    // add font
    args.StdTopObjectAttach(thirdRowForm.GetWidget());
    labelFontUI.PreInitSetup(true);
    labelFontUI.InitFramedUI("Label Font", parent, args);

    args.StdTopObjectAttach(labelFontUI);
    optionsFrame.InitFramedUI("Options", parent, args);

    args.StdToggleSetup(5);
    clipToAxesUI.StdToggleInit("Clip to axes", optionsFrame.GetForm(), args);
}



void XYZLabelSpecUIC::MenuOpen(PlotObjC&  inObj)
{
    labelPenUI.ResetPens(inObj.objPenSet);
}

bool XYZLabelSpecUIC::UIdataOK(bool& allOK)
{
    dataSource.FuncObjRefOK(allOK, "Label source");
    if (!autoLabelRotationUI.GetValue())
        rotationAngleUI.FieldValueOK(allOK, "rot. ang" , -180.0, 180.0);

    return allOK;
}


void XYZLabelSpecUIC::SetXYZLabelSpec(const PSC_XYZLabelSpec& inData)
{
    labelPenUI.SetPenSelection(inData.labelPen);

    labelAttitudeUI.SetChoice(int(inData.labelAttitude));
    autoLabelRotationUI.SetValue(inData.autoLabelRotation);
    rotationAngleUI.SetFieldValue(inData.rotationAngle);

    horizontalPosUI.SetHAlign(inData.horizontalPos);
    verticalPosUI.SetVAlign(inData.verticalPos);

    labelFontUI.SetFontSettings(inData.labelFont);
    clipToAxesUI.SetValue(inData.clipToAxes);

    labelFontUI.SetSensitive(true);

    SetSensitive();
}


void XYZLabelSpecUIC::GetXYZLabelSpec(PSC_XYZLabelSpec& outData)
{
    outData.labelPen = labelPenUI.GetPenSelection();

    outData.labelAttitude = PSC_XYZLabelSpec::LabelAttitude(labelAttitudeUI.GetChoice());
    outData.autoLabelRotation = autoLabelRotationUI.GetValue();

    if (!outData.autoLabelRotation)
        outData.rotationAngle  = rotationAngleUI.GetFieldValue();

    outData.horizontalPos = horizontalPosUI.GetHAlign();
    outData.verticalPos = verticalPosUI.GetVAlign();

    labelFontUI.GetFontSettings(outData.labelFont);
    outData.clipToAxes = clipToAxesUI.GetValue();

}

ColorLegendSpecUIC::ColorLegendSpecUIC() :
    dataSource(typeid(DO_ColorLimit))
{
}

void ColorLegendSpecUIC::SetSensitive()
{
    if (orientationUI.GetChoice() == 0)
    {
        labelLeftToggle.SetToggleText("Above");
        incLeftToggle.SetToggleText("Above");
    }
    else
    {
        labelLeftToggle.SetToggleText("Left  ");
        incLeftToggle.SetToggleText("Left  ");
    }

    labelLeftToggle.SetSensitive(labelToggle.GetValue());
    defaultToggle.SetSensitive(labelToggle.GetValue());
    labField.SetSensitive(labelToggle.GetValue() && (!defaultToggle.GetValue()));

    frameThk.SetSensitive(frameToggle.GetValue());
}


void ColorLegendSpecUIC::SetSensitiveCB(void* inObj)
{
    static_cast<ColorLegendSpecUIC*>(inObj)->SetSensitive();
}

void ColorLegendSpecUIC::StdInit(Widget       parent,
                             ArgListC&    attachments)
{
    topRowForm.InitWidget(parent, attachments);

    ArgListC args;

    args.StdSplitSetup(50);
    dataSource.InitFramedUI("Color Legend Data Source", topRowForm.GetWidget(), args);

    args.StdSplitMiddle(50, 70);
    static const char*   orientationLabels[]  = {"Horiz", "Left", "Right"};
    orientationUI.PreInitSetUp(orientationLabels, 3);
    orientationUI.InitFramedUI("Orientation", topRowForm.GetWidget(), args);
    orientationUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdSplitFinish(30);
    legendPenUI.InitFramedUI("Legend Pen", topRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    locSizeFrame.InitFramedUI("Legend Box Location && Size", parent, args);

    args.Clear();
    args.AllFormAttach();
    locSizeSplit.SetOffsets(-1, -1, 10, 10);
    locSizeSplit.InitFormUI(locSizeFrame.GetForm(), args);

    args.Clear();
    args.StdFormAttach();
    xField.format = SC_RealFormat(1);
    xField.textFieldLen = 10;

    xField.InitLabeledUI("X position", locSizeSplit.GetLeft(), args);
    lengthField.format = SC_RealFormat(1);
    lengthField.textFieldLen = 10;
    lengthField.InitLabeledUI("Bar length", locSizeSplit.GetRight(), args);

    args.Clear();
    args.StdTopObjectAttach(xField);
    yField.format = SC_RealFormat(1);
    yField.textFieldLen = 10;
    yField.InitLabeledUI("Y position", locSizeSplit.GetLeft(), args);

    args.Clear();
    args.StdTopObjectAttach(lengthField);
    aspectField.format = SC_RealFormat(2);
    aspectField.textFieldLen = 10;
    aspectField.InitLabeledUI("Aspect ratio", locSizeSplit.GetRight(), args);

    args.Clear();
    args.StdTopObjectAttach(locSizeFrame);
    formatFrame.InitFramedUI("Legend Box Format", parent, args);

    args.StdToggleSetup(5);
    ticToggle.StdToggleInit("Tics", formatFrame.GetForm(), args);

    args.StdToggleSetup(25);
    opaqueToggle.StdToggleInit("Opaque", formatFrame.GetForm(), args);

    args.StdToggleSetup(50);
    frameToggle.StdToggleInit("Frame", formatFrame.GetForm(), args);
    frameToggle.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdSplitFinish(30);
    frameThk.AttachLabelToObject();
    frameThk.InitLabeledUI("FrameThk ", formatFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(formatFrame);
    legendFontUI.InitFramedUI("Legend Font", parent, args);

    args.Clear();
    args.StdTopObjectAttach(legendFontUI);
    labelIncForm.InitWidget(parent, args);

    args.StdSplitSetup(80);
    labelFrame.InitFramedUI("Main Label", labelIncForm.GetWidget(), args);

    args.StdSplitFinish(20);
    incFrame.InitFramedUI("Inc. Label", labelIncForm.GetWidget(), args);

    args.StdToggleSetup(5);
    labelToggle.StdToggleInit("Label", labelFrame.GetForm(), args);
    labelToggle.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdToggleSetup(20);
    labelLeftToggle.StdToggleInit("Above", labelFrame.GetForm(), args);

    args.StdToggleSetup(35);
    defaultToggle.StdToggleInit("Default", labelFrame.GetForm(), args);
    defaultToggle.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdSplitFinish();
    labField.SetFieldLen(0);
    labField.InitFormUI(labelFrame.GetForm(), args);

    args.StdToggleSetup(5);
    incLeftToggle.StdToggleInit("Above", incFrame.GetForm(), args);
}



void ColorLegendSpecUIC::MenuOpen(AnnoObjC&  inObj, PSC_ColorLegendSpec& inData)
{
    legendPenUI.ResetPens(inObj.objPenSet);
    dataSource.StdMenuInit(inData.colorLegendObjRef);
}


void ColorLegendSpecUIC::SetColorLegendSpec(const PSC_ColorLegendSpec& inData, const PSC_AnnoSpec& inAnno)
{
    dataSource.SetFuncObjRef(inData.colorLegendObjRef);
    orientationUI.SetChoice(int(inData.orientation));

    legendPenUI.SetPenSelection(inAnno.legendPen);

    xField.SetFieldValue(inAnno.ulLocation.pX);
    yField.SetFieldValue(inAnno.ulLocation.pY);
    lengthField.SetFieldValue(inData.barLength);
    aspectField.SetFieldValue(inData.barAspect);

    labelToggle.SetValue(inData.dataTypeLabel);
    labelLeftToggle.SetValue(inData.dataLabelAboveLeft);
    defaultToggle.SetValue(!inData.overrideLabel);
    labField.SetFieldString(inData.newLabel);
    incLeftToggle.SetValue(inData.incLabelAboveLeft);

    ticToggle.SetValue(inData.ticsAtIncs);
    frameToggle.SetValue(inAnno.frameLegend);
    frameThk.SetLineThick(inAnno.frameThick);
    opaqueToggle.SetValue(inAnno.opaqueBackground);

    legendFontUI.SetFontSettings(inAnno.legendFont);

    SetSensitive();
}

bool ColorLegendSpecUIC::UIdataOK(bool& allOK)
{
    dataSource.FuncObjRefOK(allOK, "Color map source");
    xField.FieldValueOK(allOK, "X position" , 0.0, 100.0);
    yField.FieldValueOK(allOK, "Y position" , 0.0, 100.0);
    lengthField.FieldValueOK(allOK, "Length" , 5.0, 75.0);
    aspectField.FieldValueOK(allOK, "Aspect" , 2.0, 50.0);

    return allOK;
}

void ColorLegendSpecUIC::GetColorLegendSpec(PSC_ColorLegendSpec& outData, PSC_AnnoSpec& outAnno)
{
    outData.colorLegendObjRef = dataSource.GetFuncObjRef();
    outData.orientation = PSC_Font::FontRotation(orientationUI.GetChoice());

    outAnno.legendPen = legendPenUI.GetPenSelection();

    outAnno.ulLocation.pX = xField.GetFieldValue();
    outAnno.ulLocation.pY = yField.GetFieldValue();
    outData.barLength = lengthField.GetFieldValue();
    outData.barAspect = aspectField.GetFieldValue();

    outData.dataTypeLabel      = labelToggle.GetValue();
    outData.dataLabelAboveLeft = labelLeftToggle.GetValue();
    outData.overrideLabel = !defaultToggle.GetValue();

    if (outData.overrideLabel)
        labField.GetFieldString(outData.newLabel, stdMsgStrLen);

    outData.incLabelAboveLeft = incLeftToggle.GetValue();

    outData.ticsAtIncs = ticToggle.GetValue();
    outAnno.frameLegend = frameToggle.GetValue();
    outAnno.frameThick = frameThk.GetLineThick();
    outAnno.opaqueBackground = opaqueToggle.GetValue();

    legendFontUI.GetFontSettings(outAnno.legendFont);
}



ArrowSpecUIC::ArrowSpecUIC()
{
}

void ArrowSpecUIC::SetSensitive(bool inSens)
{
    specFrame.SetSensitive(inSens);
    bool plotHeads = inSens && plotHeadsUI.GetValue();
    fillHeadsUI.SetSensitive(plotHeads);
    headSizeUI.SetSensitive(plotHeads);
}


void ArrowSpecUIC::SetSensitiveCB(void* inObj)
{
    static_cast<ArrowSpecUIC*>(inObj)->SetSensitive(true);
}



void ArrowSpecUIC::StdInit(Widget       parent,
                             ArgListC&    attachments)
{
    specFrame.InitFramedUI("Arrow Specification", parent, attachments);

    ArgListC args;

    args.StdToggleSetup(5);
    plotHeadsUI.StdToggleInit("Plot head", specFrame.GetForm(), args);
    plotHeadsUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdToggleSetup(25);
    fillHeadsUI.StdToggleInit("Fill head", specFrame.GetForm(), args);

    args.Clear();
    args.StdSplitSetup();
    args.StdSplitMiddle(40, 70);
    headSizeUI.AttachLabelToObject();
    headSizeUI.InitLabeledUI("Head size:", specFrame.GetForm(), args);

    //  arrow body format stuff
    args.StdSplitFinish(30);
    lineThkUI.AttachLabelToObject();
    lineThkUI.InitLabeledUI("Body thk ", specFrame.GetForm(), args);
}




bool ArrowSpecUIC::UIdataOK(bool& allOK)
{
    return allOK;
}


void ArrowSpecUIC::SetArrowSpec(const PSC_ArrowSpec& inData)
{
    plotHeadsUI.SetValue(inData.plotHeads);
    fillHeadsUI.SetValue(inData.fillHeads);
    headSizeUI.SetSymbolSize(inData.headSize);
    lineThkUI.SetLineThick(inData.lineThk);
    SetSensitive(true);
}

void ArrowSpecUIC::GetArrowSpec(PSC_ArrowSpec& outData)
{
    outData.plotHeads = plotHeadsUI.GetValue();
    outData.fillHeads = fillHeadsUI.GetValue();
    outData.headSize = headSizeUI.GetSymbolSize();
    outData.lineThk = lineThkUI.GetLineThick();
}

HistogramUIC::HistogramUIC()
{
}

void HistogramUIC::SetSensitive()
{
    baselineValueUI.SetSensitive(baselineIsValueUI.GetBoolChoice());
    indexOffsetUI.SetSensitive(!positionIsValueUI.GetBoolChoice());

    int wSpec = widthSpecUI.GetChoice();
    widthLineThkUI.SetSensitive(wSpec == 0);
    widthSpecValueUI.SetSensitive(wSpec == 1);
    widthSpecPixelUI.SetSensitive(wSpec == 2);
    widthSpecPctUI.SetSensitive(wSpec == 3);

    edgeSpecUI.SetSensitive(wSpec != 0);
    if (wSpec != 0)
        edgeSpecUI.SetInternalSensitive();
}

void HistogramUIC::SetSensitiveCB(void* inObj)
{
    static_cast<HistogramUIC*>(inObj)->SetSensitive();
}

void HistogramUIC::StdInit(Widget       parent,
                           ArgListC&    attachments)
{
    posRow.InitWidget(parent, attachments);

    ArgListC args;
    args.StdEvenSplitSetup(0, 3);
    orientationXUI.PreInitSetUp("Y Axis", "X Axis");
    orientationXUI.InitFramedUI("Histogram Axes", posRow.GetWidget(), args);

    args.StdEvenSplitSetup(1, 3);
    positionIsValueUI.PreInitSetUp("Index", "Value");
    positionIsValueUI.InitFramedUI("Axes Position", posRow.GetWidget(), args);
    positionIsValueUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdEvenSplitSetup(2, 3);
    indexOffsetUI.SetFieldFormat(SC_DecFormat(3));
    indexOffsetUI.InitFramedUI("Index Offset", posRow.GetWidget(), args);

    args.StdTopObjectAttach(posRow.GetWidget());
    baselineRow.InitWidget(parent, args);

    args.StdEvenSplitSetup(0, 3);
    baselineIsValueUI.PreInitSetUp("Axis", "Value");
    baselineIsValueUI.InitFramedUI("Baseline", baselineRow.GetWidget(), args);
    baselineIsValueUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdEvenSplitSetup(1, 3);
    baselineValueUI.InitFramedUI("Baseline Value", baselineRow.GetWidget(), args);

    args.StdEvenSplitSetup(2, 3);
    static const char* widthSpecLabels[] = {"Line", "Value", "Pixel", "% Avail"};
    widthSpecUI.PreInitSetUp(widthSpecLabels, 4);
    widthSpecUI.InitFramedUI("Histogram Width", baselineRow.GetWidget(), args);
    widthSpecUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdTopObjectAttach(baselineRow.GetWidget());
    widthRow.InitWidget(parent, args);

    args.StdEvenSplitSetup(0, 4);
    widthLineThkUI.InitFramedUI("Line Thick", widthRow.GetWidget(), args);

    args.StdEvenSplitSetup(1, 4);
    widthSpecValueUI.AttachObjectToFrame();
    widthSpecValueUI.InitFramedUI("Axes Thick", widthRow.GetWidget(), args);

    args.StdEvenSplitSetup(2, 4);
    widthSpecPixelUI.InitFramedUI("Pixels", widthRow.GetWidget(), args);

    args.StdEvenSplitSetup(3, 4);
    widthSpecPctUI.AttachObjectToFrame();
    widthSpecPctUI.InitFramedUI("% Available", widthRow.GetWidget(), args);

    args.StdTopObjectAttach(widthRow.GetWidget());
    edgeSpecUI.StdInit(parent, args, false);
    edgeSpecUI.plotEdgesUI.AddOtherObjectCallback(SetSensitiveCB, this);
}


bool HistogramUIC::UIdataOK(bool& allOK)
{
    if (!positionIsValueUI.GetBoolChoice())
        indexOffsetUI.FieldValueOK(allOK, "index offset", -50.0, 50.0);

    if (baselineIsValueUI.GetBoolChoice())
        baselineValueUI.FieldValueOK(allOK, "baseline value");

    if (widthSpecUI.GetChoice() == 1)
        widthSpecValueUI.FieldValueOK(allOK, "histogram width", 1.0E-5, 1.0E+06);

    if (widthSpecUI.GetChoice() == 3)
        widthSpecPctUI.FieldValueOK(allOK, "histogram % width", 1.0, 100.0);

    edgeSpecUI.UIdataOK(allOK);

    return allOK;
}

void HistogramUIC::MenuOpen(PlotObjC&  inObj,
                            PSC_Histogram& inData)
{
    edgeSpecUI.edgePenUI.ResetPens(inObj.objPenSet);
}


void HistogramUIC::SetHistogram(const PSC_Histogram& inData)
{
    orientationXUI.SetBoolChoice(inData.orientationX);
    positionIsValueUI.SetBoolChoice(inData.positionIsValue);
    indexOffsetUI.SetFieldValue(inData.indexOffset);

    baselineIsValueUI.SetBoolChoice(inData.baselineIsValue);
    baselineValueUI.SetFieldValue(inData.baselineValue);

    widthSpecUI.SetChoice(int(inData.widthSpec));
    widthLineThkUI.SetLineThick(inData.widthLineThk);
    widthSpecValueUI.SetFieldValue(inData.widthSpecValue);
    widthSpecPixelUI.SetSymbolSize(inData.widthSpecPixel);
    widthSpecPctUI.SetFieldValue(inData.widthSpecPct);

    edgeSpecUI.SetEdgeSpec(inData);

    SetSensitive();
}
void HistogramUIC::GetHistogram(PSC_Histogram& outData)
{
    outData.orientationX = orientationXUI.GetBoolChoice();
    outData.positionIsValue = positionIsValueUI.GetBoolChoice();
    if (!outData.positionIsValue)
        outData.indexOffset = indexOffsetUI.GetFieldValue();

    outData.baselineIsValue = baselineIsValueUI.GetBoolChoice();
    if (outData.baselineIsValue)
        outData.baselineValue = baselineValueUI.GetFieldValue();

    outData.widthSpec = PSC_Histogram::WidthSpec(widthSpecUI.GetChoice());

    switch (outData.widthSpec) {
        case PSC_Histogram::wsLine : {
            outData.widthLineThk = widthLineThkUI.GetLineThick();
            break;
         }
        case PSC_Histogram::wsSpecVal : {
            outData.widthSpecValue = widthSpecValueUI.GetFieldValue();
            break;
         }
        case PSC_Histogram::wsSpecPixel : {
            outData.widthSpecPixel = widthSpecPixelUI.GetSymbolSize();
            break;
         }
        case PSC_Histogram::wsPctAvail:{
            outData.widthSpecPct = widthSpecPctUI.GetFieldValue();
            break;
         }
    }

    edgeSpecUI.GetEdgeSpec(outData);
}


TubeExtrusionSpecUIC::TubeExtrusionSpecUIC()
{
}

void TubeExtrusionSpecUIC::SetSensitive(bool inSens)
{
    specFrame.SetSensitive(inSens);
}


void TubeExtrusionSpecUIC::StdInit(Widget       parent,
                             ArgListC&    attachments)
{
    specFrame.InitFramedUI("Tube Extrusion", parent, attachments);

    ArgListC args;
    args.StdToggleSetup(5);
    capEndsUI.StdToggleInit("Cap Ends", specFrame.GetForm(), args);

    args.StdSplitMiddle(35, 60);
    nextrusionPointsUI.PreInitSetUp();
    nextrusionPointsUI.InitWidget(specFrame.GetForm(), args);

    args.StdSplitFinish(30);
    extrusionPixelSizeUI.InitWidget(specFrame.GetForm(), args);
}


bool TubeExtrusionSpecUIC::UIdataOK(bool& allOK)
{
    return allOK;
}

void TubeExtrusionSpecUIC::SetTubeExtrusionSpec(const PSC_TubeExtrusionSpec& inData)
{
    capEndsUI.SetValue(inData.capEnds);
    nextrusionPointsUI.SetTubePoints(inData.nextrusionPoints);
    extrusionPixelSizeUI.SetSymbolSize(inData.extrusionPixelSize);
}

void TubeExtrusionSpecUIC::GetTubeExtrusionSpec(PSC_TubeExtrusionSpec& outData)
{
    outData.capEnds = capEndsUI.GetValue();
    outData.nextrusionPoints = nextrusionPointsUI.GetTubePoints();
    outData.extrusionPixelSize = extrusionPixelSizeUI.GetSymbolSize();
}



};                                      // end namespace

