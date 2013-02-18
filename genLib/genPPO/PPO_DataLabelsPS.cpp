///////////////////////////////////////////////////////////////////////////////////
//
// PPO_DataLabelsPS.cpp
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

#include "PPO_DataLabels.h"
#include "PPO_DataLabelsPS.h"

// ****************************************

void PPO_DataLabelsPS::LegendDataWidget::SetSensitiveCB(void* inObj)
{
    static_cast<PPO_DataLabelsPS::LegendDataWidget*>(inObj)->SetSensitive();
}

void PPO_DataLabelsPS::LegendDataWidget::InitLegendData()
{
    ArgListC args;

    args.StdFormAttach();
    topRow.InitWidget(outsideForm.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftPosAttach(1);
    doLabelToggle.StdToggleInit(" ", topRow.GetWidget(), args);
    doLabelToggle.AddOtherObjectCallback(SetSensitiveCB, this);

    args.ResetPlace();
    args.SpecifiedWidthAttach(10, 65, specFormwidth);
    labelFO.InitWidget(topRow.GetWidget(), args);

    args.ResetPlace();
    args.RightFormAttach();
    labAlign.InitWidget(topRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRow.GetWidget());
    nextRow.InitWidget(outsideForm.GetWidget(), args);

    args.Clear();
    args.TopLeftAttach(5, 20);
    overrideFormatToggle.StdToggleInit("New Format", nextRow.GetWidget(), args);
    overrideFormatToggle.AddOtherObjectCallback(SetSensitiveCB, this);

    args.Clear();
    args.BottomFormAttach();
    args.LeftPosAttach(25);
    args.RightFormAttach();
    labelFormatUI.InitFormUI(nextRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(nextRow.GetWidget());
    bottomRow.InitWidget(outsideForm.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.TopLeftAttach(5, 20);
    overrideMainLabelUI.StdToggleInit("New Label", bottomRow.GetWidget(), args);
    overrideMainLabelUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.ResetPlace();
    args.SpecifiedWidthAttach(25, 70, specFormwidth);
    mainLabelUI.SetFieldLen(DO_Label::maxLabelLen);
    mainLabelUI.InitWidget(bottomRow.GetWidget(), args);

    args.ResetPlace();
    args.SpecifiedWidthAttach(74, 100, specFormwidth);
    unitsLabelUI.SetFieldLen(DO_Label::maxLabelLen);
    unitsLabelUI.InitWidget(bottomRow.GetWidget(), args);
}

void PPO_DataLabelsPS::LegendDataWidget::SetLegendData(const PFO_DataLabels::DataLabel& labelData)
{
    labelFO.SetFuncObjRef(labelData.labelObjRef);
    doLabelToggle.SetValue(labelData.doLabel);
    labAlign.SetHAlign(labelData.labAlign);
    overrideFormatToggle.SetValue(labelData.overrideFormat);
    labelFormatUI.SetRealFormatSettings(labelData.labelFormat);

    overrideMainLabelUI.SetValue(labelData.overrideMainLabel);
    mainLabelUI.SetFieldString(labelData.mainLabel);
    unitsLabelUI.SetFieldString(labelData.unitsLabel);
}
void PPO_DataLabelsPS::LegendDataWidget::GetLegendData(PFO_DataLabels::DataLabel& labelData)
{
    labelData.labelObjRef = labelFO.GetFuncObjRef();
    labelData.doLabel     = doLabelToggle.GetValue();
    labelData.labAlign    = labAlign.GetHAlign();
    labelData.overrideFormat = overrideFormatToggle.GetValue();
    labelFormatUI.GetRealFormatSettings(labelData.labelFormat);
    labelData.overrideMainLabel = overrideMainLabelUI.GetValue();
    mainLabelUI.GetFieldString(labelData.mainLabel, DO_Label::maxLabelLen);
    unitsLabelUI.GetFieldString(labelData.unitsLabel, DO_Label::maxLabelLen);
}

void PPO_DataLabelsPS::LegendDataWidget::SetSensitive()
{
    bool selected = doLabelToggle.GetValue();
    labelFO.SetSensitive(selected);
    labAlign.SetSensitive(selected);
    overrideFormatToggle.SetSensitive(selected);
    overrideMainLabelUI.SetSensitive(selected);
    unitsLabelUI.SetSensitive(selected);
    if (selected)
    {
        labelFormatUI.SetSensitive(overrideFormatToggle.GetValue());
        mainLabelUI.SetSensitive(overrideMainLabelUI.GetValue());
    }
    else
    {
        labelFormatUI.SetSensitive(false);
        mainLabelUI.SetSensitive(false);
    }
}

CSize PPO_DataLabelsPS::DataLabelsScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 600);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// PPO_DataLabelsPS

PPO_DataLabelsPS::PPO_DataLabelsPS()
: legendFontUI(true)
{
}

void PPO_DataLabelsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_DataLabelsPS*>(inObj)->SetAllSensitive();
}

void PPO_DataLabelsPS::SetAllSensitive()
{
    for ( int i = 0; i < PFO_DataLabels::maxDataLabels; i++) {
        labData[i].SetSensitive();
    }
    frameThk.SetSensitive(frameToggle.GetValue());
}

// ****************************************

void PPO_DataLabelsPS::InitMenu(const PPO_DataLabels&   inData)
{
    PPO_AnnoBasePS::InitMenu(inData);

    ArgListC    args;

    // for for coord and format
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    // coord form
    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftFormAttach();
    args.RightPosAttach(50);

    coordFrame.InitFramedUI("Legend Box Location", topRowForm.GetWidget(), args);

    args.ResetPlace();
    args.LeftUICAttach(coordFrame);
    args.RightFormAttach();
    formatFrame.InitFramedUI("Format", topRowForm.GetWidget(), args);

    // X & Y loc in coordFrame
    args.Clear();
    args.StdFormAttach();
    xField.format = SC_RealFormat(1);
    xField.textFieldLen = 10;
    xField.InitLabeledUI("X position", coordFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(xField);
    yField.format = SC_RealFormat(1);
    yField.textFieldLen = 10;
    yField.InitLabeledUI("Y position", coordFrame.GetForm(), args);

    //  toggles and pen in format

    args.Clear();
    args.TopLeftAttach(5, 10);
    opaqueToggle.StdToggleInit("Opaque", formatFrame.GetForm(), args);

    args.Clear();
    args.LeftFormAttach(10);
    args.TopWidgetAttach(&opaqueToggle);
    args.TopOffset(2);
    frameToggle.StdToggleInit("Frame", formatFrame.GetForm(), args);
    frameToggle.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.LeftPosAttach(40);
    args.RightFormAttach();
    args.SetPlaceHolder();
    args.TopFormAttach();
    legendPenOption.AttachLabelToObject();
    legendPenOption.InitLabeledUI("Pen:", formatFrame.GetForm(), args);

    args.ResetPlace();
    args.BottomFormAttach();
    args.TopUICAttach(legendPenOption);
    args.TopOffset(2);
    frameThk.AttachLabelToObject();
    frameThk.InitLabeledUI("Thk ", formatFrame.GetForm(), args);

    // form row for legend and min width
    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    labelWidthForm.InitWidget(PPform.GetWidget(), args);

    // add label
    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach();
    args.RightPosAttach(70);
    labField.SetFieldLen(0);
    labField.InitFramedUI("Legend Label", labelWidthForm.GetWidget(), args);

    // add width field
    args.Clear();
    args.TopFormAttach();
    args.LeftUICAttach(labField);
    args.RightFormAttach();
    minDataFieldWidthUI.InitFramedUI("Minimum Data Field Width", labelWidthForm.GetWidget(), args);

    // add font
    args.Clear();
    args.StdTopObjectAttach(labelWidthForm.GetWidget());
    legendFontUI.InitFramedUI("Legend Font", PPform.GetWidget(), args);

    //  scrolled window in middle
    args.Clear();
    args.StdTopObjectAttach(legendFontUI);
    args.BottomFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);

    //scrollArea.needHorizontal = false;
    scrollArea.InitWidget(PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();

    for (int i = 0; i < PFO_DataLabels::maxDataLabels; i++)
    {
        labData[i].outsideForm.InitWidget(scrollArea, args);
        labData[i].InitLegendData();

        args.Clear();
        args.StdTopObjectAttach(labData[i].outsideForm.GetWidget());
        labData[i].hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(labData[i].hSep.GetWidget());
    }
}

void PPO_DataLabelsPS::MenuOpen(PPO_DataLabels& inData)
{
    MenuBaseC::MenuOpen(true, true);
    legendPenOption.ResetPens(inData.objPenSet);
    for (int i = 0; i < PFO_DataLabels::maxDataLabels; i++)
    {
        labData[i].labelFO.StdMenuInit(inData.labelData[i].labelObjRef);
    }
}

void PPO_DataLabelsPS::SetMenuData(PPO_DataLabels&  inData)
{
    legendPenOption.SetPenSelection(inData.legendPen);
    xField.SetFieldValue(inData.ulLocation.pX);
    yField.SetFieldValue(inData.ulLocation.pY);
    labField.SetFieldString(inData.legendTitle);
    minDataFieldWidthUI.SetFieldValue(inData.minDataFieldWidth);

    frameToggle.SetValue(inData.frameLegend);
    frameThk.SetLineThick(inData.frameThick);
    opaqueToggle.SetValue(inData.opaqueBackground);

    legendFontUI.SetFontSettings(inData.legendFont);

    for (int i = 0; i < PFO_DataLabels::maxDataLabels; i++)
    {
        labData[i].SetLegendData(inData.labelData[i]);
    }
    SetAllSensitive();
    PPO_AnnoBasePS::SetMenuData(inData);
}

void PPO_DataLabelsPS::UpdateULUI(PPO_DataLabels&  inData)
{
    xField.SetFieldValue(inData.ulLocation.pX);
    yField.SetFieldValue(inData.ulLocation.pY);
}

bool PPO_DataLabelsPS::UIdataOK()
{
    PPO_AnnoBasePS::UIdataOK();
    xField.FieldValueOK(allOK, "X position" , 0.0, 100.0);
    yField.FieldValueOK(allOK, "Y position" , 0.0, 100.0);
    minDataFieldWidthUI.FieldValueOK(allOK, "field width" , 0, 40);
    for (int i = 0; i < PFO_DataLabels::maxDataLabels; i++)
        if (labData[i].doLabelToggle.GetValue())
            labData[i].labelFO.FuncObjRefOK(allOK, "Label source");
    return allOK;
}

void PPO_DataLabelsPS::GetMenuData(PPO_DataLabels&  outData)
{
    PPO_AnnoBasePS::GetMenuData(outData);

    for (int i = 0; i < PFO_DataLabels::maxDataLabels; i++) {
        labData[i].GetLegendData(outData.labelData[i]);
    }
    outData.legendPen = legendPenOption.GetPenSelection();
    outData.ulLocation.pX = xField.GetFieldValue();
    outData.ulLocation.pY = yField.GetFieldValue();
    labField.GetFieldString(outData.legendTitle, outData.legendTitleLen);
    outData.minDataFieldWidth = minDataFieldWidthUI.GetFieldValue();

    outData.frameLegend = frameToggle.GetValue();
    outData.frameThick  = frameThk.GetLineThick();
    outData.opaqueBackground = opaqueToggle.GetValue();

    legendFontUI.GetFontSettings(outData.legendFont);
}

void PPO_DataLabelsPS::MenuClear()
{
    for (int i = 0; i < PFO_DataLabels::maxDataLabels; i++)
    {
        labData[i].SetLegendData(PFO_DataLabels::DataLabel());
    }
    SetAllSensitive();
}

