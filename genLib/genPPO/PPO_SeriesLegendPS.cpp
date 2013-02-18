///////////////////////////////////////////////////////////////////////////////////
//
// PPO_SeriesLegendPS.cpp
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

#include "PPO_SeriesLegend.h"
#include "PPO_SeriesLegendPS.h"


PPO_SeriesLegendPS::SeriesLegendWidget::SeriesLegendWidget() :
    seriesObjRefUI(typeid(DO_SeriesLegendData))
{
}

void PPO_SeriesLegendPS::SeriesLegendWidget::SetSensitiveCB(void* inObj)
{
    static_cast<PPO_SeriesLegendPS::SeriesLegendWidget*>(inObj)->SetSensitive();
}

void PPO_SeriesLegendPS::SeriesLegendWidget::InitSeriesLegend()
{
    ArgListC    args;
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftPosAttach(5);
    doSeriesToggle.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doSeriesToggle.AddOtherObjectCallback(SetSensitiveCB, this);

    args.ResetPlace();
    args.LeftPosAttach(30);
    args.RightFormAttach();
    seriesObjRefUI.InitWidget(outsideForm.GetWidget(), args);
}

void PPO_SeriesLegendPS::SeriesLegendWidget::SetSeriesLegend(const PFO_SeriesLegend::SeriesLegend& legendData)
{
    seriesObjRefUI.SetFuncObjRef(legendData.seriesObjRef);
    doSeriesToggle.SetValue(legendData.doSeries);
}
void PPO_SeriesLegendPS::SeriesLegendWidget::GetSeriesLegend(PFO_SeriesLegend::SeriesLegend& legendData)
{
    legendData.seriesObjRef = seriesObjRefUI.GetFuncObjRef();
    legendData.doSeries    = doSeriesToggle.GetValue();
}

void PPO_SeriesLegendPS::SeriesLegendWidget::SetSensitive()
{
    seriesObjRefUI.SetSensitive(doSeriesToggle.GetValue());
}

/////////////////////////////////////////////////////////////////
// PPO_SeriesLegendPS

PPO_SeriesLegendPS::PPO_SeriesLegendPS()
{
}

void PPO_SeriesLegendPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_SeriesLegendPS*>(inObj)->SetAllSensitive();
}

void PPO_SeriesLegendPS::SetAllSensitive()
{
    for (int i = 0; i < PFO_SeriesLegend::maxSeriesLegend; i++)
        legendData[i].SetSensitive();

    frameThk.SetSensitive(frameToggle.GetValue());
}


CSize PPO_SeriesLegendPS::SeriesLegendScrolledForm::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = LayoutManager::SetWindowPosX(0, 440);
    szForm.cy = LayoutManager::SetWindowPosY(0, 1000);
    return szForm;
}

void PPO_SeriesLegendPS::InitMenu(PPO_SeriesLegend& inData)
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

    locSizeFrame.InitFramedUI("Legend Location & Size", topRowForm.GetWidget(), args);

    args.ResetPlace();
    args.LeftUICAttach(locSizeFrame);
    args.RightFormAttach();
    formatFrame.InitFramedUI("Format", topRowForm.GetWidget(), args);

    // X & Y loc in coordFrame
    args.Clear();
    args.AllFormAttach();
    locSizeSplit.SetOffsets(-1, -1, 5, 5);
    locSizeSplit.InitFormUI(locSizeFrame.GetForm(), args);

    args.Clear();
    args.StdFormAttach();
    xField.format = SC_RealFormat(1);
    xField.textFieldLen = 6;
    xField.InitLabeledUI("X pos", locSizeSplit.GetLeft(), args);

    sizeField.format = SC_RealFormat(1);
    sizeField.textFieldLen = 6;
    sizeField.InitLabeledUI("Data size", locSizeSplit.GetRight(), args);

    args.Clear();
    args.StdTopObjectAttach(xField);
    yField.format = SC_RealFormat(1);
    yField.textFieldLen = 6;
    yField.InitLabeledUI("Y pos", locSizeSplit.GetLeft(), args);

    args.Clear();
    args.StdTopObjectAttach(sizeField);
    nField.textFieldLen = 6;
    nField.InitLabeledUI("# points", locSizeSplit.GetRight(), args);

    //  toggles and pen in format

    args.Clear();
    args.TopLeftAttach(5, 10);
    opaqueToggle.StdToggleInit("Opaque", formatFrame.GetForm(), args);

    args.Clear();
    args.LeftFormAttach(10);
    //args.BottomFormAttach(0);
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

    // and label
    args.Clear();
    args.StdTopObjectAttach(legendFontUI);
    labField.SetFieldLen(40);
    labField.InitFramedUI("Legend Label", PPform.GetWidget(), args);

//  scrolled window in middle
    args.Clear();
    args.StdTopObjectAttach(labField);
    args.BottomFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);

    //scrollArea.needHorizontal = false;
    scrollArea.InitWidget(PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    for (int i = 0; i < PFO_SeriesLegend::maxSeriesLegend; i++)
    {
        legendData[i].outsideForm.InitWidget(scrollArea, args);
        legendData[i].InitSeriesLegend();

        args.Clear();
        args.StdTopObjectAttach(legendData[i].outsideForm.GetWidget());
    }
    SetMainformScrolled(true, 490, 250);
}

void PPO_SeriesLegendPS::MenuOpen(PPO_SeriesLegend& inData)
{
    MenuBaseC::MenuOpen(true, true);
    legendPenOption.ResetPens(inData.objPenSet);
    for (int i = 0; i < PFO_SeriesLegend::maxSeriesLegend; i++)
        legendData[i].seriesObjRefUI.StdMenuInit(inData.legendData[i].seriesObjRef);
}

void PPO_SeriesLegendPS::SetMenuData(PPO_SeriesLegend&  inData)
{
    legendPenOption.SetPenSelection(inData.legendPen);
    xField.SetFieldValue(inData.ulLocation.pX);
    yField.SetFieldValue(inData.ulLocation.pY);
    labField.SetFieldString(inData.legendTitle);
    sizeField.SetFieldValue(inData.seriesSize );
    nField.SetFieldValue(inData.seriesNpts);

    frameToggle.SetValue(inData.frameLegend);
    frameThk.SetLineThick(inData.frameThick);
    opaqueToggle.SetValue(inData.opaqueBackground);

    legendFontUI.SetFontSettings(inData.legendFont);

    for (int i = 0; i < PFO_SeriesLegend::maxSeriesLegend; i++)
    {
        legendData[i].SetSeriesLegend(inData.legendData[i]);
    }
    SetAllSensitive();

    PPO_AnnoBasePS::SetMenuData(inData);
}

bool PPO_SeriesLegendPS::UIdataOK()
{
    PPO_AnnoBasePS::UIdataOK();
    xField.FieldValueOK(allOK, "X position" , 0.0, 100.0);
    yField.FieldValueOK(allOK, "Y position" , 0.0, 100.0);
    sizeField.FieldValueOK(allOK, "Size" , 1.0, 50.0);
    nField.FieldValueOK(allOK, "# points" , 1, 10);
    for (int i = 0; i < PFO_SeriesLegend::maxSeriesLegend; i++)
        if (legendData[i].doSeriesToggle.GetValue())
            legendData[i].seriesObjRefUI.FuncObjRefOK(allOK, "Data source");
    return allOK;
}

void PPO_SeriesLegendPS::GetMenuData(PPO_SeriesLegend&  outData)
{
    PPO_AnnoBasePS::GetMenuData(outData);

    for (int i = 0; i < PFO_SeriesLegend::maxSeriesLegend; i++)
        legendData[i].GetSeriesLegend(outData.legendData[i]);

    outData.legendPen = legendPenOption.GetPenSelection();
    outData.ulLocation.pX = xField.GetFieldValue();
    outData.ulLocation.pY = yField.GetFieldValue();
    outData.seriesSize = sizeField.GetFieldValue();
    outData.seriesNpts = nField.GetFieldValue();
    labField.GetFieldString(outData.legendTitle, outData.legendTitleLen);

    outData.frameLegend = frameToggle.GetValue();
    outData.frameThick  = frameThk.GetLineThick();
    outData.opaqueBackground = opaqueToggle.GetValue();

    legendFontUI.GetFontSettings(outData.legendFont);
}


void  PPO_SeriesLegendPS::MenuClear()
{
    for (int i = 0; i < PFO_SeriesLegend::maxSeriesLegend; i++)
    {
        legendData[i].SetSeriesLegend(PFO_SeriesLegend::SeriesLegend());
    }
    SetAllSensitive();
}

void  PPO_SeriesLegendPS::UpdateULUI(PPO_SeriesLegend&  inData)
{
    xField.SetFieldValue(inData.ulLocation.pX);
    yField.SetFieldValue(inData.ulLocation.pY);
}

