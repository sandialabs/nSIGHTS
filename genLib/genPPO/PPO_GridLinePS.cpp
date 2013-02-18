///////////////////////////////////////////////////////////////////////////////////
//
// PPO_GridLinePS.cpp
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

#include "PPO_GridLinePS.h"

// ****************************************

PPO_GridLinePS::PPO_GridLinePS() :
    gridValueUI(typeid(DO_Real), false),
    labelFontUI(true)
{
}

void PPO_GridLinePS::SetAllSensitive()
{
    labelFormatUI.SetSensitive(true);
    int labType = labelFormatUI.labelTypeUI.GetChoice();
    labelFontUI.SetSensitive(labType  > 0);
    gridLabelUI.SetSensitive(labType == 1);
    valueFormatUI.SetSensitive(labType == 2);

    gridLineTypeUI.SetLineColorAndThick(gridLinePenUI.GetSelectedColorValue(), gridLineThicknessUI.GetLineThick());
}

void PPO_GridLinePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_GridLinePS*>(inObj)->SetAllSensitive();
}


void PPO_GridLinePS::InitMenu(const PPO_GridLine& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    gridValueUI.InitUI("Grid X/Y Value", args, PPform.GetWidget(), true, true, SetAllSensitiveCB, this);

    args.StdTopObjectAttach(gridValueUI);
    formatFrame.InitFramedUI("Grid Line Format", PPform.GetWidget(), args);

    args.StdSplitSetup(25);
    args.LeftOffset(10);
    gridIsXUI.PreInitSetUp("X", "Y");
    gridIsXUI.InitWidget(formatFrame.GetForm(), args);

    args.StdSplitMiddle(25, 50);
    gridLinePenUI.InitWidget(formatFrame.GetForm(), args);
    gridLinePenUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(50, 75);
    gridLineThicknessUI.InitWidget(formatFrame.GetForm(), args);
    gridLineThicknessUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(25);
    gridLineTypeUI.InitWidget(formatFrame.GetForm(), args);

    args.StdTopObjectAttach(formatFrame);
    labelFormatUI.InitFramedUI("Label Format", PPform.GetWidget(), args);
    labelFormatUI.labelTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(labelFormatUI);
    labelFontUI.InitFramedUI("Label Font", PPform.GetWidget(), args);

    args.StdTopObjectAttach(labelFontUI);
    bottomRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    gridLabelUI.AttachObjectToFrame();
    gridLabelUI.InitFramedUI("Label",  bottomRow.GetWidget(), args);

    args.StdSplitFinish();
    valueFormatUI.InitFramedUI("Value Format",  bottomRow.GetWidget(), args);

}


void PPO_GridLinePS::MenuOpen(PPO_GridLine&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    gridValueUI.MenuInit(inData.gridValue);
    gridLinePenUI.ResetPens(inData.objPenSet);
}

void PPO_GridLinePS::SetMenuData(PPO_GridLine&    inData)
{
    PPO_BasePS::SetMenuData(inData);

    gridValueUI.SetValue(inData.gridValue);

    gridIsXUI.SetBoolChoice(!inData.gridIsX);
    gridLinePenUI.SetPenSelection(inData.gridLinePen);
    gridLineThicknessUI.SetLineThick(inData.gridLineThickness);
    gridLineTypeUI.SetLineType(inData.gridLineType);

    labelFormatUI.SetGridLineLabelFormat(inData.labelFormat);

    labelFontUI.SetFontSettings(inData.labelFont);
    gridLabelUI.SetFieldString(inData.gridLabel);
    valueFormatUI.SetRealFormatSettings(inData.valueFormat);
    SetAllSensitive();
}

bool PPO_GridLinePS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    gridValueUI.UIdataOK(allOK);
    labelFormatUI.UIdataOK(allOK);
    return allOK;
}

void PPO_GridLinePS::GetMenuData(PPO_GridLine&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    gridValueUI.GetValue(outData.gridValue);

    outData.gridIsX = ! gridIsXUI.GetBoolChoice();
    outData.gridLinePen = gridLinePenUI.GetPenSelection();
    outData.gridLineThickness = gridLineThicknessUI.GetLineThick();
    outData.gridLineType = gridLineTypeUI.GetLineType();

    labelFormatUI.GetGridLineLabelFormat(outData.labelFormat);

    if (outData.labelFormat.labelType != PSC_GridLineLabelFormat::ltNone)
        labelFontUI.GetFontSettings(outData.labelFont);

    if (outData.labelFormat.labelType == PSC_GridLineLabelFormat::ltText)
        gridLabelUI.GetFieldString(outData.gridLabel, PFO_GridLine::maxLabelLen);

    if (outData.labelFormat.labelType == PSC_GridLineLabelFormat::ltValue)
        valueFormatUI.GetRealFormatSettings(outData.valueFormat);
}


