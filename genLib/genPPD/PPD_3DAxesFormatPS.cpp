///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DAxesFormatPS.cpp
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
//      increment/grid line formatting for 3D plots
//
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "PPD_3DAxesFormatPS.h"

void AxesLinePosWidget::PreInitSetUp()
{
    static const char* lineLabels[]  = {"N", "-", "+", "B"};
    ChooserWidget::PreInitSetUp(lineLabels, 4);
}


void AxesTicPosWidget::PreInitSetUp()
{
    static const char* ticLabels[]  = {"-", "+", "B"};
    ChooserWidget::PreInitSetUp(ticLabels, 3);
}


void AxesFormatUIC::SetAxesSensitiveCB(void* inObj)
{
 static_cast<AxesFormatUIC*>(inObj)->SetSensitive(true);
}



void  AxesFormatUIC::InitAxesFormatWidgets()
{
    SetOffsets(1, 1, -1, -1);

    ArgListC args;

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.TopOffset(1);
    args.BottomOffset(1);
    // args.AddZeroArg(XmNborderWidth);
    // args.AddZeroArg(XmNmarginHeight);
    args.SetPlaceHolder();

    args.RightPosAttach(22);
    uMajor.ShortTextPreInitSetUp();
    uMajor.InitWidget(GetForm(), args);
    uMajor.AddOtherObjectCallback(SetAxesSensitiveCB, this);

    args.ResetPlace();
    args.RightPosAttach(32);
    uMinor.ShortTextPreInitSetUp();
    uMinor.InitWidget(GetForm(), args);

    args.ResetPlace();
    args.RightPosAttach(41);
    uLines.PreInitSetUp();
    uLines.InitWidget(GetForm(), args);

    args.ResetPlace();
    args.RightPosAttach(50);
    uTics.PreInitSetUp();
    uTics.InitWidget(GetForm(), args);

    args.RightPosAttach(72);
    vMajor.ShortTextPreInitSetUp();
    vMajor.InitWidget(GetForm(), args);
    vMajor.AddOtherObjectCallback(SetAxesSensitiveCB, this);

    args.ResetPlace();
    args.RightPosAttach(82);
    vMinor.ShortTextPreInitSetUp();
    vMinor.InitWidget(GetForm(), args);

    args.ResetPlace();
    args.RightPosAttach(91);
    vLines.PreInitSetUp();
    vLines.InitWidget(GetForm(), args);

    args.ResetPlace();
    args.RightFormAttach();
    vTics.PreInitSetUp();
    vTics.InitWidget(GetForm(), args);

}


void  AxesFormatUIC::
InitLabeledUI(      char      labText[],
                                       Widget     parent,
                                 const ArgListC&  inArgs)
{
    BaseUIC::InitLabeledUI(labText, parent, inArgs);
    InitAxesFormatWidgets();
}

void AxesFormatUIC::SetAxesFormat(const PC_AxesFormat& uFormat,
                                  const PC_AxesFormat& vFormat)
{
    uMajor.SetIncType(uFormat.axesMajorInc);
    uMinor.SetIncType(uFormat.axesMinorInc);
    uLines.SetAxesLinePos(uFormat.axesLinePos);
    uTics.SetAxesTicPos(uFormat.axesTicPos);

    vMajor.SetIncType(vFormat.axesMajorInc);
    vMinor.SetIncType(vFormat.axesMinorInc);
    vLines.SetAxesLinePos(vFormat.axesLinePos);
    vTics.SetAxesTicPos(vFormat.axesTicPos);
}


void AxesFormatUIC::GetAxesFormat(PC_AxesFormat& uFormat,
                                  PC_AxesFormat& vFormat)
{
    uFormat.axesMajorInc    =   uMajor.GetIncType();
    uFormat.axesMinorInc    =   uMinor.GetIncType();
    uFormat.axesLinePos     =   uLines.GetAxesLinePos();
    uFormat.axesTicPos      =   uTics.GetAxesTicPos();
    vFormat.axesMajorInc    =   vMajor.GetIncType();
    vFormat.axesMinorInc    =   vMinor.GetIncType();
    vFormat.axesLinePos     =   vLines.GetAxesLinePos();
    vFormat.axesTicPos      =   vTics.GetAxesTicPos();
}


void AxesFormatUIC::SetSensitive(bool inSens)
{
    uMajor.SetSensitive(inSens);
    vMajor.SetSensitive(inSens);
    uLines.SetSensitive(inSens);
    vLines.SetSensitive(inSens);
    if (inSens)
    {
        bool uMinOK = (uMajor.GetIncType() != PC_Axes::aitNone);
        uMinor.SetSensitive(uMinOK);
        uTics.SetSensitive((uMajor.GetIncType() == PC_Axes::aitTic) ||
                           (uMinOK && (uMinor.GetIncType() == PC_Axes::aitTic)));

        bool vMinOK = (vMajor.GetIncType() != PC_Axes::aitNone);
        vMinor.SetSensitive(vMinOK);
        vTics.SetSensitive((vMajor.GetIncType() == PC_Axes::aitTic) ||
                           (vMinOK && (vMinor.GetIncType() == PC_Axes::aitTic)));
    }
    else
    {
        uMinor.SetSensitive(false);
        uTics.SetSensitive(false);
        vMinor.SetSensitive(false);
        vTics.SetSensitive(false);
    }
}


void PPD_3DAxesFormatPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPD_3DAxesFormatPS*>(inObj)->SetAllSensitive();
}

void PPD_3DAxesFormatPS::SetAllSensitive()
{
    bool manual = !autoFormatUI.GetValue();
    axesFormatData[PC_3DAxesFormat::a3pXYp].SetSensitive(manual);
    axesFormatData[PC_3DAxesFormat::a3pXZp].SetSensitive(manual);
    axesFormatData[PC_3DAxesFormat::a3pYZp].SetSensitive(manual);
    axesFormatData[PC_3DAxesFormat::a3pXZn].SetSensitive(manual);
    axesFormatData[PC_3DAxesFormat::a3pYZn].SetSensitive(manual);

    majorGridType.SetLineColorAndThick(majorPenUI.GetSelectedColorValue(), majorGridThk.GetLineThick());
    minorGridType.SetLineColorAndThick(minorPenUI.GetSelectedColorValue(), minorGridThk.GetLineThick());
}

void PPD_3DAxesFormatPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();

    args.LeftFormAttach();
    args.RightPosAttach(33);
    axesLinePenUI.InitFramedUI("Axes Line Pen", URform.GetWidget(), args);

    args.ResetPlace();
    args.LeftUICAttach(axesLinePenUI);
    args.RightPosAttach(66);
    majorPenUI.InitFramedUI("Major Tic/Grid Pen", URform.GetWidget(), args);
    majorPenUI.AddOtherObjectCallback(SetAllSensitiveCB, this);


    args.ResetPlace();
    args.LeftUICAttach(majorPenUI);
    args.RightFormAttach();
    minorPenUI.InitFramedUI("Minor Tic/Grid Pen", URform.GetWidget(), args);
    majorPenUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdFormAttach();
    formatFrame.InitFramedUI("Layout", PPform.GetWidget(), args);

    args.StdFormAttach();
    topRowForm.InitWidget(formatFrame.GetForm(), args);


    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();

    args.LeftFormAttach();
    args.LeftOffset(5);
    autoFormatUI.StdToggleInit("Auto", topRowForm.GetWidget(), args);
    autoFormatUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.ResetPlace();
    args.RightPosAttach(22);
    uMajTitleLabel.StdLabelInit("UMaj", topRowForm.GetWidget(), args);

    args.ResetPlace();
    args.RightPosAttach(32);
    uMinTitleLabel.StdLabelInit("UMin", topRowForm.GetWidget(), args);

    args.ResetPlace();
    args.RightPosAttach(41);
    uLineTitleLabel.StdLabelInit("ULine", topRowForm.GetWidget(), args);

    args.ResetPlace();
    args.RightPosAttach(50);
    uTicTitleLabel.StdLabelInit("UTic", topRowForm.GetWidget(), args);

    args.ResetPlace();
    args.RightPosAttach(72);
    vMajTitleLabel.StdLabelInit("VMaj", topRowForm.GetWidget(), args);

    args.ResetPlace();
    args.RightPosAttach(82);
    vMinTitleLabel.StdLabelInit("VMin", topRowForm.GetWidget(), args);

    args.ResetPlace();
    args.RightPosAttach(91);
    vLineTitleLabel.StdLabelInit("VLine", topRowForm.GetWidget(), args);

    args.ResetPlace();
    args.RightFormAttach();
    vTicTitleLabel.StdLabelInit("VTic", topRowForm.GetWidget(), args);

    args.Clear();
    args.RightFormAttach();
    args.LeftFormAttach();
    args.SetPlaceHolder();
    args.TopWidgetAttach(topRowForm.GetWidget());

    static char* lineLabels[]  = {"XY+", "XY-", "XZ+", "XZ-", "YZ+", "YZ-"};
    for (int i = 0; i < PC_3DAxesFormat::naxes3DPlanes; i++)
    {
        axesFormatData[i].InitLabeledUI(lineLabels[i], formatFrame.GetForm(), args);
        args.ResetPlace();
        args.TopUICAttach(axesFormatData[i]);
    }

    //  form for misc stuff
    args.Clear();
    args.StdTopObjectAttach(formatFrame);
    miscForm.SetOffsets(0, 0, 0, 0);
    miscForm.InitFormUI(PPform.GetWidget(), args);

    //  do all frames first
    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();

    args.LeftFormAttach();
    args.RightPosAttach(25);
    axesFrame.InitFramedUI("Axes Line", miscForm.GetForm(), args);

    args.ResetPlace();
    args.LeftUICAttach(axesFrame);
    args.RightPosAttach(50);
    majorTicFrame.InitFramedUI("Major Tics", miscForm.GetForm(), args);

    args.ResetPlace();
    args.LeftUICAttach(majorTicFrame);
    args.RightPosAttach(75);
    minorTicFrame.InitFramedUI("Minor Tics", miscForm.GetForm(), args);

    args.ResetPlace();
    args.LeftUICAttach(minorTicFrame);

    args.RightFormAttach();
    gridFrame.InitFramedUI("Grid Lines", miscForm.GetForm(), args);

    //  top row stuff
    args.Clear();
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

    SetMainformScrolled(true, 520, 330);
}

void PPD_3DAxesFormatPS::MenuOpen(PlotDefC&  inPD)
{
    MenuBaseC::MenuOpen(true, true);

    axesLinePenUI.ResetPens(inPD.defaultPenSet);
    majorPenUI.ResetPens(inPD.defaultPenSet);
    minorPenUI.ResetPens(inPD.defaultPenSet);

}

void PPD_3DAxesFormatPS::SetMenuData(const PC_3DAxesFormat& inData)
{
    autoFormatUI.SetValue(inData.autoFormat);
    axesLinePenUI.SetPenSelection(inData.axesLinePen    );
    majorPenUI.SetPenSelection(inData.majorPen    );
    minorPenUI.SetPenSelection(inData.minorPen    );

    for (int i = 0; i < PC_3DAxesFormat::naxes3DPlanes; i++)
    {
        axesFormatData[i].SetAxesFormat(inData.axesUFormat[i], inData.axesVFormat[i]);
        axesFormatData[i].SetSensitive(true);
    }

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

    SetAllSensitive();
}

void PPD_3DAxesFormatPS::SetMenuData(const PC_3DAxesFormat& inData,
                                            MenuObjC& inObj)
{
    SetMenuData(inData);
    MenuBaseC::SetMenuData(inObj);
}

bool PPD_3DAxesFormatPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    axesOffset.FieldValueOK(allOK, "axes offset" , 0, 100);
    majorTicLen.FieldValueOK(allOK, "major tick length" , 0, 100);
    minorTicLen.FieldValueOK(allOK, "minor tick length" , 0, 100);
    return allOK;
}

void PPD_3DAxesFormatPS::GetMenuData(PC_3DAxesFormat& outData,
                                    MenuObjC&       outObj)
{
    MenuBaseC::GetMenuData(outObj);

    outData.axesLinePen        = axesLinePenUI.GetPenSelection();
    outData.majorPen           = majorPenUI.GetPenSelection();
    outData.minorPen           = minorPenUI.GetPenSelection();

    outData.autoFormat = autoFormatUI.GetValue();

    for (int i = 0; i < PC_3DAxesFormat::naxes3DPlanes; i++)
        axesFormatData[i].GetAxesFormat(outData.axesUFormat[i], outData.axesVFormat[i]);

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
}
void PPD_3DAxesFormatPS::MenuDefault()
{
    SetMenuData(PC_3DAxesFormat());
}

void PPD_3DAxesFormatPS::MenuClear()
{
    PC_3DAxesFormat temp;
    temp.Clear();
    temp.autoFormat = false;
    SetMenuData(temp);
}

