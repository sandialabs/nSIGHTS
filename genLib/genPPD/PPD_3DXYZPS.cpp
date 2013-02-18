///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DXYZPS.cpp
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
//      derived from PPD_3D. Adds components for 3DXYZ plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/C_Common.h>

#include <genApp/U_MenuPPO.h>

#include "PPD_3DXYZPS.h"

PPD_3DXYZPS::PPD_3DXYZPS()
    : xaxisIncFormatUI(true), yaxisIncFormatUI(true), zaxisIncFormatUI(true)
{
}



void PPD_3DXYZPS::InitMenu()
{
    PPD_3DPS::InitMenu();

    ArgListC args;
    args.StdTopObjectAttach(lastRowForm.GetWidget(), 10);
    xyzFrame.InitFramedUI("XYZ Properties", PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach(5);
    xyaxesLengthRatioUI.InitLabeledUI("X:Y axes length ratio", xyzFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(xyaxesLengthRatioUI, 10);
    xzaxesLengthRatioUI.InitLabeledUI("X:Z axes length ratio", xyzFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(xyzFrame);
    xForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    xIsLinearUI.PreInitSetUp("Logarithmic", "Linear", true);
    xIsLinearUI.InitFramedUI("X Axis Type", xForm.GetWidget(), args);

    args.StdSplitFinish();
    xaxisIncFormatUI.InitFramedUI("X Increment Format", xForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(xForm.GetWidget());
    yForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    yIsLinearUI.PreInitSetUp("Logarithmic", "Linear", true);
    yIsLinearUI.InitFramedUI("Y Axis Type", yForm.GetWidget(), args);

    args.StdSplitFinish();
    yaxisIncFormatUI.InitFramedUI("Y Increment Format", yForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(yForm.GetWidget());
    zForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    zIsLinearUI.PreInitSetUp("Logarithmic", "Linear", true);
    zIsLinearUI.InitFramedUI("Z Axis Type", zForm.GetWidget(), args);

    args.StdSplitFinish();
    zaxisIncFormatUI.InitFramedUI("Z Increment Format", zForm.GetWidget(), args);

}


void PPD_3DXYZPS::MenuOpen(PPD_3DXYZ&   inData)
{
    PPD_3DPS::MenuOpen(inData);
}

void PPD_3DXYZPS::SetMenuData(PPD_3DXYZ&    inData)
{
    xyaxesLengthRatioUI.SetFieldValue(inData.xyaxesLengthRatio);
    xzaxesLengthRatioUI.SetFieldValue(inData.xzaxesLengthRatio);

    xIsLinearUI.SetBoolChoice(inData.xAxis.axesAreLinear);
    xaxisIncFormatUI.SetRealFormatSettings(inData.xAxis.axisIncFormat);

    yIsLinearUI.SetBoolChoice(inData.yAxis.axesAreLinear);
    yaxisIncFormatUI.SetRealFormatSettings(inData.yAxis.axisIncFormat);

    zIsLinearUI.SetBoolChoice(inData.zAxis.axesAreLinear);
    zaxisIncFormatUI.SetRealFormatSettings(inData.zAxis.axisIncFormat);

    PPD_3DPS::SetMenuData(inData);
}

bool PPD_3DXYZPS::UIdataOK()
{
    PPD_3DPS::UIdataOK();
    xyaxesLengthRatioUI.FieldValueOK(allOK, "X:Y axes length ration", 0.05, 20.0);
    xzaxesLengthRatioUI.FieldValueOK(allOK, "X:Z axes length ration", 0.05, 20.0);
    return allOK;
}

void PPD_3DXYZPS::GetMenuData(PPD_3DXYZ&   outData)
{
   PPD_3DPS::GetMenuData(outData);
   outData.xyaxesLengthRatio = xyaxesLengthRatioUI.GetFieldValue();
   outData.xzaxesLengthRatio = xzaxesLengthRatioUI.GetFieldValue();

    outData.xAxis.axesAreLinear = xIsLinearUI.GetBoolChoice();
    xaxisIncFormatUI.GetRealFormatSettings(outData.xAxis.axisIncFormat);

    outData.yAxis.axesAreLinear = yIsLinearUI.GetBoolChoice();
    yaxisIncFormatUI.GetRealFormatSettings(outData.yAxis.axisIncFormat);

    outData.zAxis.axesAreLinear = zIsLinearUI.GetBoolChoice();
    zaxisIncFormatUI.GetRealFormatSettings(outData.zAxis.axisIncFormat);
}

void  PPD_3DXYZPS::MenuDefault()
{
    PPD_3DPS::MenuDefault();
    xyaxesLengthRatioUI.SetFieldValue(1.0);
    xzaxesLengthRatioUI.SetFieldValue(1.0);
}

