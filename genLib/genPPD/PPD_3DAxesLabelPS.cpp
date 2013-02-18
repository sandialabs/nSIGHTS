///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DAxesLabelPS.cpp
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
//      axes and increment labeling for 3D plot
//
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "PPD_3DAxesLabelPS.h"

void PPD_3DAxesLabelPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPD_3DAxesLabelPS*>(inObj)->SetAllSensitive();
}

void PPD_3DAxesLabelPS::SetAllSensitive()
{
    bool doLabs = plotAxesLabelUI.GetValue();
    labelFrame.SetSensitive(doLabs);
    labelFont.SetSensitive(doLabs);

    bool doIncs = plotIncrementLabelUI.GetValue();
    incrementFont.SetSensitive(doIncs);

    bool doAuto = false;
    if (doLabs || doIncs)
        doAuto = autoPositionLabelsUI.GetValue();

    autoPositionLabelsUI.SetSensitive((doLabs || doIncs));

    miscForm.SetSensitive((!doAuto) && (doLabs || doIncs));
}

void PPD_3DAxesLabelPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC   args;
    args.AllFormAttach();
    topFrame.InitFramedUI("Control", URform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.SetPlaceHolder();

    args.LeftPosAttach(5);
    plotAxesLabelUI.StdToggleInit("Axis labels", topFrame.GetForm(), args);
    plotAxesLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.ResetPlace();
    args.LeftPosAttach(32);
    plotIncrementLabelUI.StdToggleInit("Increment labels", topFrame.GetForm(), args);
    plotIncrementLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.ResetPlace();
    args.LeftPosAttach(65);
    autoPositionLabelsUI.StdToggleInit("Auto Position labels", topFrame.GetForm(), args);
    autoPositionLabelsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdFormAttach();
    labelFrame.InitFramedUI("Labels", PPform.GetWidget(), args);

    xlabelField.SetFieldLen(0);
    xlabelField.InitLabeledUI("X axis  ", labelFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(xlabelField);
    ylabelField.SetFieldLen(0);
    ylabelField.InitLabeledUI("Y axis  ", labelFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(ylabelField);
    zlabelField.SetFieldLen(0);
    zlabelField.InitLabeledUI("Z axis  ", labelFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(labelFrame);
    labelFont.PreInitSetup(true);
    labelFont.InitFramedUI("Label Font", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(labelFont);
    incrementFont.PreInitSetup(true);
    incrementFont.InitFramedUI("Increment Font", PPform.GetWidget(), args);

    //  form for misc stuff
    args.Clear();
    args.StdTopObjectAttach(incrementFont);
    miscForm.SetOffsets(0, 0, 0, 0);
    miscForm.InitFramedUI("Label Orientation/Position", PPform.GetWidget(), args);

    // XY form
    args.Clear();
    args.StdFormAttach();
    xyForm.InitWidget(miscForm.GetForm(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();

    args.LeftFormAttach();
    xyTitleLabel.StdLabelInit("X&Y Axes", xyForm.GetWidget(), args);

    args.ResetPlace();
    args.RightPosAttach(40);
    xyLabIsVertUI.PreInitSetUp("Horiz", "Vert");
    xyLabIsVertUI.InitWidget(xyForm.GetWidget(), args);

    args.ResetPlace();
    args.RightPosAttach(70);
    xLabYPosIsMaxUI.PreInitSetUp("X@Y_Min", "X@Y_Max");
    xLabYPosIsMaxUI.InitWidget(xyForm.GetWidget(), args);

    args.ResetPlace();
    args.RightFormAttach();
    yLabXPosIsMaxUI.PreInitSetUp("Y@X_Min", "Y@X_Max");
    yLabXPosIsMaxUI.InitWidget(xyForm.GetWidget(), args);

    // Z form
    args.Clear();
    args.StdTopObjectAttach(xyForm.GetWidget());
    zForm.InitWidget(miscForm.GetForm(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();

    args.LeftFormAttach();
    zTitleLabel.StdLabelInit("Z Axes", zForm.GetWidget(), args);

    args.ResetPlace();
    args.RightPosAttach(40);
    static const char* zorLabels[]  = {"XZ norm", "XZ rev", "YZ norm", "YZ rev"};
    zLabOrientationUI.PreInitSetUp(zorLabels, 4);
    zLabOrientationUI.InitWidget(zForm.GetWidget(), args);

    args.ResetPlace();
    args.RightPosAttach(70);
    zLabYPosIsMaxUI.PreInitSetUp("Y Min", "Y Max");
    zLabYPosIsMaxUI.InitWidget(zForm.GetWidget(), args);

    args.ResetPlace();
    args.RightFormAttach();
    zLabXPosIsMaxUI.PreInitSetUp("X Min", "X Max");
    zLabXPosIsMaxUI.InitWidget(zForm.GetWidget(), args);

    SetMainformScrolled(true, 520, 320);
}


void PPD_3DAxesLabelPS::SetMenuData(const PC_3DAxesLabel&   inData)
{
    plotAxesLabelUI.SetValue(inData.plotAxesLabel);
    plotIncrementLabelUI.SetValue(inData.plotIncrementLabel);
    autoPositionLabelsUI.SetValue(inData.autoPositionLabels);

    xlabelField.SetFieldString(inData.xaxesLabel);
    ylabelField.SetFieldString(inData.yaxesLabel);
    zlabelField.SetFieldString(inData.zaxesLabel);

    labelFont.SetFontSettings(inData.labelFont);
    incrementFont.SetFontSettings(inData.incrementFont);

    xyLabIsVertUI.SetBoolChoice(inData.xyLabIsVert);
    xLabYPosIsMaxUI.SetBoolChoice(inData.xLabYPosIsMax);
    yLabXPosIsMaxUI.SetBoolChoice(inData.yLabXPosIsMax);
    zLabOrientationUI.SetChoice(inData.zLabOrientation);
    zLabYPosIsMaxUI.SetBoolChoice(inData.zLabYPosIsMax);
    zLabXPosIsMaxUI.SetBoolChoice(inData.zLabXPosIsMax);

    SetAllSensitive();
}

void PPD_3DAxesLabelPS::SetMenuData(const PC_3DAxesLabel&   inData,
                                            MenuObjC& inObj)
{
    SetMenuData(inData);
    MenuBaseC::SetMenuData(inObj);
}

bool PPD_3DAxesLabelPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    return allOK;
}

void PPD_3DAxesLabelPS::GetMenuData(PC_3DAxesLabel& outData,
                                    MenuObjC&       outObj)
{
    MenuBaseC::GetMenuData(outObj);

    outData.plotAxesLabel      = plotAxesLabelUI.GetValue();
    outData.plotIncrementLabel = plotIncrementLabelUI.GetValue();
    outData.autoPositionLabels = autoPositionLabelsUI.GetValue();

    if (outData.plotAxesLabel)
    {
        xlabelField.GetFieldString(outData.xaxesLabel, PC_3DAxesLabel::maxLabelLen);
        ylabelField.GetFieldString(outData.yaxesLabel, PC_3DAxesLabel::maxLabelLen);
        zlabelField.GetFieldString(outData.zaxesLabel, PC_3DAxesLabel::maxLabelLen);
        labelFont.GetFontSettings(outData.labelFont);
    }

    if (outData.plotIncrementLabel)
        incrementFont.GetFontSettings(outData.incrementFont);

    outData.xyLabIsVert    = xyLabIsVertUI.GetBoolChoice();
    outData.xLabYPosIsMax  = xLabYPosIsMaxUI.GetBoolChoice();
    outData.yLabXPosIsMax  = yLabXPosIsMaxUI.GetBoolChoice();
    outData.zLabOrientation= PC_3DAxesLabel::ZLabOrientation(zLabOrientationUI.GetChoice());
    outData.zLabYPosIsMax  = zLabYPosIsMaxUI.GetBoolChoice();
    outData.zLabXPosIsMax  = zLabXPosIsMaxUI.GetBoolChoice();
}
void PPD_3DAxesLabelPS::MenuDefault()
{
    SetMenuData(PC_3DAxesLabel());
}

