///////////////////////////////////////////////////////////////////////////////////
//
// PPO_UserLabelsPS.cpp
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

#include "PPO_UserLabels.h"
#include "PPO_UserLabelsPS.h"

void PPO_UserLabelsPS::UserLabelWidget::SetSensitiveCB(void* inObj)
{
    static_cast<PPO_UserLabelsPS::UserLabelWidget*>(inObj)->SetSensitive();
}

void PPO_UserLabelsPS::UserLabelWidget::InitUserLabel()
{
    static const int specFormwidth = 585;

    ArgListC args;

    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftPosAttach(5);
    doLabelToggle.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doLabelToggle.AddOtherObjectCallback(SetSensitiveCB, this);

    args.ResetPlace();
    args.SpecifiedWidthAttach(15, 80, specFormwidth);
    userLabel.SetFieldLen(PFO_UserLabels::UserLabel::userLabelLen);
    userLabel.InitWidget(outsideForm.GetWidget(), args);

    args.ResetPlace();
    args.RightFormAttach();
    labAlign.InitWidget(outsideForm.GetWidget(), args);
}

void PPO_UserLabelsPS::UserLabelWidget::SetUserLabel(const PFO_UserLabels::UserLabel& labelData)
{
    userLabel.SetFieldString(labelData.userLabel);
    doLabelToggle.SetValue(labelData.doLabel);
    labAlign.SetHAlign(labelData.labAlign);
}
void PPO_UserLabelsPS::UserLabelWidget::GetUserLabel(PFO_UserLabels::UserLabel& labelData)
{
    userLabel.GetFieldString(labelData.userLabel, PFO_UserLabels::UserLabel::userLabelLen);
    labelData.doLabel     = doLabelToggle.GetValue();
    labelData.labAlign    = labAlign.GetHAlign();
}

void PPO_UserLabelsPS::UserLabelWidget::SetSensitive()
{
    bool selected = doLabelToggle.GetValue();
    userLabel.SetSensitive(selected);
    labAlign.SetSensitive(selected);
}

/////////////////////////////////////////////////////////////////
// PPO_UserLabelsPS

PPO_UserLabelsPS::PPO_UserLabelsPS() :
    legendFontUI(true)
{
}

void PPO_UserLabelsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_UserLabelsPS*>(inObj)->SetAllSensitive();
}

void PPO_UserLabelsPS::SetAllSensitive()
{
    for (int i = 0; i < PFO_UserLabels::maxUserLabels; i++)
        labData[i].SetSensitive();

    frameThk.SetSensitive(frameToggle.GetValue());
}


CSize PPO_UserLabelsPS::UserLabelScrolledForm::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = LayoutManager::SetWindowPosX(0, 440);
    szForm.cy = LayoutManager::SetWindowPosY(0, 1000);
    return szForm;
}

void PPO_UserLabelsPS::InitMenu(PPO_UserLabels& inData)
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

    // add font
    args.Clear();
    args.StdTopObjectAttach(formatFrame);
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
    for (int i = 0; i < PFO_UserLabels::maxUserLabels; i++)
    {
        labData[i].outsideForm.InitWidget(scrollArea, args);

        labData[i].InitUserLabel();

        args.Clear();
        args.StdTopObjectAttach(labData[i].outsideForm.GetWidget());
    }
    SetMainformScrolled(true, 490, 250);
}

void PPO_UserLabelsPS::MenuOpen(PPO_UserLabels& inData)
{
    MenuBaseC::MenuOpen(true, true);
    legendPenOption.ResetPens(inData.objPenSet);
}

void PPO_UserLabelsPS::SetMenuData(PPO_UserLabels&  inData)
{
    legendPenOption.SetPenSelection(inData.legendPen);
    xField.SetFieldValue(inData.ulLocation.pX);
    yField.SetFieldValue(inData.ulLocation.pY);

    frameToggle.SetValue(inData.frameLegend);
    frameThk.SetLineThick(inData.frameThick);
    opaqueToggle.SetValue(inData.opaqueBackground);

    legendFontUI.SetFontSettings(inData.legendFont);

    for (int i = 0; i < PFO_UserLabels::maxUserLabels; i++)
    {
        labData[i].SetUserLabel(inData.labelData[i]);
    }
    SetAllSensitive();

    PPO_AnnoBasePS::SetMenuData(inData);
}

bool PPO_UserLabelsPS::UIdataOK()
{
    PPO_AnnoBasePS::UIdataOK();
    xField.FieldValueOK(allOK, "X position" , 0.0, 100.0);
    yField.FieldValueOK(allOK, "Y position" , 0.0, 100.0);
    return allOK;
}

void PPO_UserLabelsPS::GetMenuData(PPO_UserLabels&  outData)
{
    PPO_AnnoBasePS::GetMenuData(outData);

    for (int i = 0; i < PFO_UserLabels::maxUserLabels; i++)
        labData[i].GetUserLabel(outData.labelData[i]);

    outData.legendPen = legendPenOption.GetPenSelection();
    outData.ulLocation.pX = xField.GetFieldValue();
    outData.ulLocation.pY = yField.GetFieldValue();

    outData.frameLegend = frameToggle.GetValue();
    outData.frameThick  = frameThk.GetLineThick();
    outData.opaqueBackground = opaqueToggle.GetValue();

    legendFontUI.GetFontSettings(outData.legendFont);
}

void PPO_UserLabelsPS::MenuClear()
{
    for (int i = 0; i < PFO_UserLabels::maxUserLabels; i++)
    {
        labData[i].SetUserLabel(PFO_UserLabels::UserLabel());
    }
    SetAllSensitive();
}

void  PPO_UserLabelsPS::UpdateULUI(PPO_UserLabels&  inData)
{
    xField.SetFieldValue(inData.ulLocation.pX);
    yField.SetFieldValue(inData.ulLocation.pY);
}

