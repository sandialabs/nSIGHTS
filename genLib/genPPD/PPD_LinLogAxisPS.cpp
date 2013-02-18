///////////////////////////////////////////////////////////////////////////////////
//
// PPD_LinLogAxisPS.cpp
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
//      a single axis for an 2DXY or 3DXYZ plot
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Real.h>

#include "PPD_LinLogAxisPS.h"

PPD_LinLogAxisPS::PPD_LinLogAxisPS()
    : axisIncFormatUI(true), cursorReportFormatUI(false)
{
};


void PPD_LinLogAxisPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPD_LinLogAxisPS*>(inObj)->SetAllSensitive();
}

void PPD_LinLogAxisPS::SetAllSensitive()
{
    bool rangeManual = (!autoRangeToggle.GetValue());
    bool isLinear = linLogSelector.GetBoolChoice();

    linMinField.SetSensitive(rangeManual && isLinear);
    linMaxField.SetSensitive(rangeManual && isLinear);
    logMinField.SetSensitive(rangeManual && (!isLinear));
    logMaxField.SetSensitive(rangeManual && (!isLinear));

    bool majIncsOK = (majorIncs.GetIncType() != PC_Axes::aitNone);
    majorIncFrame.SetSensitive(majIncsOK && isLinear);
    axisIncFormatUI.SetSensitive(majIncsOK);

    bool majIncsManual = ((!majIncAutoToggle.GetValue())&& majIncsOK);
    startAtFirstUI.SetSensitive(majIncsManual && isLinear);
    majIncSizeField.SetSensitive(majIncsManual && isLinear);

    bool minIncsOK = majIncsOK && (minorIncs.GetIncType() != PC_Axes::aitNone);
    minorIncFrame.SetSensitive(minIncsOK && isLinear);

    bool minIncsManual = ((!minIncAutoToggle.GetValue()) && minIncsOK);
    nminIncField.SetSensitive(minIncsManual && isLinear);
}

void PPD_LinLogAxisPS::InitMenu(bool is3DIn)
{
    MenuBaseC::InitMenu();

    ArgListC   args;

    is3D = is3DIn;
    if (!is3D)
    {
        args.AllFormAttach();
        incFrame.SetOffsets(0, 0, 10, 10);
        incFrame.InitFramedUI("Increment Style", URform.GetWidget(), args);

        args.Clear();
        args.BottomFormAttach();
        args.SetPlaceHolder();

        args.LeftFormAttach();
        args.RightPosAttach(50);
        majorIncs.SetOffsets(0, 0, 15, 10);
        majorIncs.AttachLabelToObject();
        majorIncs.PreInitSetUp();
        majorIncs.InitLabeledUI("Major", incFrame.GetForm(), args);
        majorIncs.AddOtherObjectCallback(SetAllSensitiveCB, this);

        args.ResetPlace();
        args.LeftBaseAttach(majorIncs);
        args.RightFormAttach();

        minorIncs.PreInitSetUp();
        minorIncs.SetOffsets(0, 0, 15, 10);
        minorIncs.AttachLabelToObject();
        minorIncs.InitLabeledUI("Minor", incFrame.GetForm(), args);
        minorIncs.AddOtherObjectCallback(SetAllSensitiveCB, this);
    }


    //  lin log selector
    args.StdFormAttach();
    firstRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    args.RightOffset(100);
    linLogSelector.PreInitSetUp("    Logarithmic      ", "    Linear   ");
    linLogSelector.InitFramedUI("Axis Type", firstRow.GetWidget(), args);
    linLogSelector.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    static const char*   rotationLabels[]  = {"Horiz", "Left", "Right"};
    fontRotationUI.PreInitSetUp(rotationLabels, 3);
    //fontRotationUI.AttachLabelToObject();
    //fontRotationUI.SetOffsets(0, 0, 0, 0);
    fontRotationUI.InitFramedUI("Increment Label Rotation", firstRow.GetWidget(), args);


    //  set frames
    //  enclosing frame
    args.Clear();
    args.StdTopObjectAttach(linLogSelector);
    limitsFrame.InitFramedUI("Axis Limits", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(limitsFrame);
    majorIncFrame.InitFramedUI("Major Increment Setup", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(majorIncFrame);
    minorIncFrame.InitFramedUI("Minor Increment Setup", PPform.GetWidget(), args);

    // set up split
    args.Clear();
    args.RightFormAttach();
    args.LeftPosAttach(12);
    args.TopFormAttach();
    args.SetPlaceHolder();
    limitsSplit.SetOffsets(0, 5, 0, 0);
    limitsSplit.InitFormUI(limitsFrame.GetForm(), args);

    //  toggles
    args.StdToggleSetup(2);
    autoRangeToggle.StdToggleInit("Auto", limitsFrame.GetForm(), args);
    autoRangeToggle.AddOtherObjectCallback(SetAllSensitiveCB, this);

    // stuff in limits frame
    args.Clear();
    args.StdFormAttach();
    linMinField.InitLabeledUI("Linear min", limitsSplit.GetLeft(), args);
    logMinField.format = SC_RealFormat(ncf_Scientific);
    logMinField.InitLabeledUI("Log min", limitsSplit.GetRight(), args);

    args.Clear();
    args.StdTopObjectAttach(linMinField);
    linMaxField.InitLabeledUI("Linear max", limitsSplit.GetLeft(), args);

    args.Clear();
    args.StdTopObjectAttach(logMinField);
    logMaxField.format = SC_RealFormat(ncf_Scientific);
    logMaxField.InitLabeledUI("Log max", limitsSplit.GetRight(), args);

    // major inc
    args.StdToggleSetup(2);
    majIncAutoToggle.StdToggleInit("Auto", majorIncFrame.GetForm(), args);
    majIncAutoToggle.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(25);
    startAtFirstUI.StdToggleInit("Inc at minimum", majorIncFrame.GetForm(), args);

    args.StdSplitSetup();
    args.StdSplitFinish();
    majIncSizeField.InitLabeledUI("Major increment", majorIncFrame.GetForm(), args);


    // minor inc
    args.StdToggleSetup(2);
    minIncAutoToggle.StdToggleInit("Auto", minorIncFrame.GetForm(), args);
    minIncAutoToggle.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitSetup();
    args.StdSplitFinish();
    nminIncField.SetFieldLen(5);
    nminIncField.InitLabeledUI("Minor per major", minorIncFrame.GetForm(), args);

    args.Clear();
    args.TopUICAttach(minorIncFrame);
    args.LeftFormAttach();
    args.RightPosAttach(50);
    axisIncFormatUI.InitFramedUI("Increment Label Format", PPform.GetWidget(), args);

    if (!is3D)
    {
        args.Clear();
        args.TopUICAttach(minorIncFrame);
        args.LeftPosAttach(50);
        args.RightFormAttach(50);
        cursorReportFormatUI.InitFramedUI("Cursor Reporting Format", PPform.GetWidget(), args);
    }

    SetMainformScrolled(true, 520, 330);
}


void PPD_LinLogAxisPS::SetMenuData(const LinLogAxis1D&  inData)

{
    majorIncs.SetIncType(inData.axisMajorIncs);
    minorIncs.SetIncType(inData.axisMinorIncs);

    autoRangeToggle.SetValue(inData.autoScaleAxes);
    linLogSelector.SetBoolChoice(inData.axesAreLinear);
    fontRotationUI.SetChoice(int(inData.incrFont.fontRotation));

    majIncAutoToggle.SetValue(inData.autoMajorIncs);
    startAtFirstUI.SetValue(inData.startAtFirst);
    minIncAutoToggle.SetValue(inData.autoMinorIncs);

    linMinField.SetFieldValue(inData.axisLinMin);
    linMaxField.SetFieldValue(inData.axisLinMax);
    logMinField.SetFieldValue(inData.axisLogMin);
    logMaxField.SetFieldValue(inData.axisLogMax);

    majIncSizeField.SetFieldValue(inData.majorIncVal);
    nminIncField.SetFieldValue(inData.nminorPerMajor);

    axisIncFormatUI.SetRealFormatSettings(inData.axisIncFormat);

    if (!is3D)
        cursorReportFormatUI.SetRealFormatSettings(inData.cursorReportFormat);

    SetAllSensitive();

}

void PPD_LinLogAxisPS::SetMenuData(const LinLogAxis1D&  inData,
                                        MenuObjC& inObj)

{
    SetMenuData(inData);
    MenuBaseC::SetMenuData(inObj);
}

bool PPD_LinLogAxisPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    static const double axesLinMin = -1.0E+30;
    static const double axesLinMax = 1.0E+30;

    static const double axesLogMin = 1.0E-30;
    static const double axesLogMax = 1.0E+30;

    if (!autoRangeToggle.GetValue())
    {
        if (linLogSelector.GetBoolChoice())
        {
            linMinField.FieldValueOK(allOK, "Linear axis minimum" , axesLinMin, axesLinMax);
            linMaxField.FieldValueOK(allOK, "Linear axis maximum" , axesLinMin, axesLinMax);
        }
        else
        {
            logMinField.FieldValueOK(allOK, "Log axis minimum" , axesLogMin, axesLogMax);
            logMaxField.FieldValueOK(allOK, "Log axis maximum" , axesLogMin, axesLogMax);
        }
    }

    if ((majorIncs.GetIncType() != PC_Axes::aitNone) && linLogSelector.GetBoolChoice())
    {
        if  (!majIncAutoToggle.GetValue())
            majIncSizeField.FieldValueOK(allOK, "Major inc size" , axesLinMin, axesLinMax);

        if ((minorIncs.GetIncType() != PC_Axes::aitNone) && (!minIncAutoToggle.GetValue()))
            nminIncField.FieldValueOK(allOK, "Minor inc spec" , 1, 20);
    }
    return allOK;
}

void PPD_LinLogAxisPS::GetMenuData(LinLogAxis1D& outData,
                                    MenuObjC&       outObj)
{
    MenuBaseC::GetMenuData(outObj);

    outData.axisMajorIncs = majorIncs.GetIncType();
    outData.axisMinorIncs = minorIncs.GetIncType();
    outData.autoScaleAxes = autoRangeToggle.GetValue();
    outData.axesAreLinear = linLogSelector.GetBoolChoice();
    outData.incrFont.fontRotation = PSC_Font::FontRotation(fontRotationUI.GetChoice());

    if (!outData.autoScaleAxes)
        if (outData.axesAreLinear)
        {
            outData.axisLinMin = linMinField.GetFieldValue();
            outData.axisLinMax = linMaxField.GetFieldValue();
        }
        else
        {
            outData.axisLogMin = logMinField.GetFieldValue();
            outData.axisLogMax = logMaxField.GetFieldValue();
        }

    if (outData.axesAreLinear)
    {
        outData.autoMajorIncs = majIncAutoToggle.GetValue();
        if (!outData.autoMajorIncs)
        {
            outData.startAtFirst = startAtFirstUI.GetValue();
            outData.majorIncVal = majIncSizeField.GetFieldValue();
            if (fabs(outData.majorIncVal) < 1.0E-30)
                outData.majorIncVal = 1.0;
        }

        outData.autoMinorIncs = minIncAutoToggle.GetValue();
        if (!outData.autoMinorIncs)
            outData.nminorPerMajor = nminIncField.GetFieldValue();
    }

    axisIncFormatUI.GetRealFormatSettings(outData.axisIncFormat);
    if (!is3D)
        cursorReportFormatUI.GetRealFormatSettings(outData.cursorReportFormat);

}

void  PPD_LinLogAxisPS::MenuDefault()
{
    SetMenuData(LinLogAxis1D());
}

