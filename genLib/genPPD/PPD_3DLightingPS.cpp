///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DLightingPS.cpp
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
//      lighting specification for 3D plots
//
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "PPD_3DLightingPS.h"


void PenAndSliderUIC::StdInit(     char       label[],
                                   Widget     parent,
                             const ArgListC&  inArgs)
{
    BaseUIC::InitFramedUI(label, parent, inArgs);

    ArgListC args;

    args.Clear();
    args.TopFormAttach();
    args.RightPosAttach(25);
    penUI.InitWidget(GetForm(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftWidgetAttach(penUI.GetWidget());
    args.LeftOffset(5);
    args.RightFormAttach();

    intensityUI.SetSliderVals(0.0, 1.0, 0, 1000, 10);
    intensityUI.InitLabeledUI("Intensity:", GetForm(), args);
}


void PenAndSliderUIC::SetPenAndSlider(int     penNum,
                                      double  intensityVal)
{
    penUI.SetPenSelection(penNum);
    intensityUI.SetValues(intensityVal);
}

void PenAndSliderUIC::GetPenAndSlider(int&     penNum,
                                      double&  intensityVal)
{
    penNum = penUI.GetPenSelection();
    intensityVal = intensityUI.valSlider.GetRealValue();
}

void PenAndSliderUIC::SetSensitive(bool isSensitive)
{
    penUI.SetSensitive(isSensitive);
    intensityUI.SetSensitive(isSensitive);
}


// **************************************** call backs

void PPD_3DLightingPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPD_3DLightingPS*>(inObj)->SetAllSensitive();
}

void PPD_3DLightingPS::SetAllSensitive()
{
    bool doLights = useLightingUI.GetValue();
    bool doDefLights = useDefaultLightUI.GetValue();
    bool isFixed = defaultLightIsFixedUI.GetBoolChoice();

    useDefaultLightUI.SetSensitive(doLights);
    showDefaultLightUI.SetSensitive(doLights);
    ambientLightUI.SetSensitive(doLights);
    defaultDiffuseLightUI.SetSensitive(doLights && doDefLights);
    defaultSpecularLightUI.SetSensitive(doLights && doDefLights);
    defaultLightPosForm.SetSensitive(doLights && doDefLights);

    fixedElevationUI.SetSensitive(doLights && doDefLights && isFixed);
    fixedAzimuthUI.SetSensitive(doLights && doDefLights && isFixed);

    relativeElevationUI.SetSensitive(doLights && doDefLights && (!isFixed));
    relativeAzimuthUI.SetSensitive(doLights && doDefLights && (!isFixed));

}

void PPD_3DLightingPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC   args;

    //  control
    args.AllFormAttach();
    topFrame.InitFramedUI("Control", URform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.SetPlaceHolder();
    args.LeftPosAttach(5);

    useLightingUI.StdToggleInit("Use lighting", topFrame.GetForm(), args);
    useLightingUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.ResetPlace();
    args.LeftPosAttach(35);
    useDefaultLightUI.StdToggleInit("Use default light", topFrame.GetForm(), args);
    useDefaultLightUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.ResetPlace();
    args.LeftPosAttach(65);
    showDefaultLightUI.StdToggleInit("Show light", topFrame.GetForm(), args);

    // pens
    args.Clear();
    args.StdFormAttach();
    ambientLightUI.StdInit("Ambient Light", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(ambientLightUI);
    defaultDiffuseLightUI.StdInit("Default Diffuse Light", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(defaultDiffuseLightUI);
    defaultSpecularLightUI.StdInit("Default Specular Light", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(defaultSpecularLightUI);
    defaultLightPosForm.InitWidget(PPform.GetWidget(), args);

    //  fix/rel control
    args.StdSplitSetup(20);
    defaultLightIsFixedUI.SetTopAttach();
    defaultLightIsFixedUI.PreInitSetUp("Relative", "Fixed", false);
    defaultLightIsFixedUI.InitFramedUI("Light Position", defaultLightPosForm.GetWidget(), args);
    defaultLightIsFixedUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(20);
    args.LeftUICAttach(defaultLightIsFixedUI);
    args.RightFormAttach();
    posSplit.InitFramedUI("Position Data", defaultLightPosForm.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    relativeElevationUI.SetFieldLen(6);
    relativeElevationUI.InitLabeledUI("Delta elevation", posSplit.GetLeft(), args);

    args.Clear();
    args.StdTopObjectAttach(relativeElevationUI);
    fixedElevationUI.SetFieldLen(6);
    fixedElevationUI.InitLabeledUI("Fixed elevation", posSplit.GetLeft(), args);

    args.Clear();
    args.StdTopObjectAttach(fixedElevationUI);
    defaultLightWUI.SetFieldLen(8);
    defaultLightWUI.InitLabeledUI("Inverse dist.", posSplit.GetLeft(), args);

    args.Clear();
    args.StdFormAttach();
    relativeAzimuthUI.SetFieldLen(6);
    relativeAzimuthUI.InitLabeledUI("Delta azimuth", posSplit.GetRight(), args);

    args.Clear();
    args.StdTopObjectAttach(relativeAzimuthUI);
    fixedAzimuthUI.SetFieldLen(6);
    fixedAzimuthUI.InitLabeledUI("Fixed azimuth", posSplit.GetRight(), args);

    SetMainformScrolled(true, 520, 310);
}

void PPD_3DLightingPS::MenuOpen(PlotDefC&  inPD)
{
    MenuBaseC::MenuOpen(true, false);
    ambientLightUI.penUI.ResetPens(inPD.defaultPenSet);
    defaultDiffuseLightUI.penUI.ResetPens(inPD.defaultPenSet);
    defaultSpecularLightUI.penUI.ResetPens(inPD.defaultPenSet);
}

void PPD_3DLightingPS::SetMenuData(const PC_Lighting&   inData)
{
    useLightingUI.SetValue(inData.useLighting);
    useDefaultLightUI.SetValue(inData.useDefaultLight);
    showDefaultLightUI.SetValue(inData.showDefaultLight);

    ambientLightUI.SetPenAndSlider(inData.ambientPen, inData.ambientIntensity);
    defaultDiffuseLightUI.SetPenAndSlider(inData.defaultDiffuseLightPen, inData.defaultDiffuseLightIntensity);
    defaultSpecularLightUI.SetPenAndSlider(inData.defaultSpecularLightPen, inData.defaultSpecularLightIntensity);

    defaultLightIsFixedUI.SetBoolChoice(inData.defaultLightIsFixed);

    fixedElevationUI.SetFieldValue(inData.fixedElevation);
    fixedAzimuthUI.SetFieldValue(inData.fixedAzimuth);
    relativeElevationUI.SetFieldValue(inData.relativeElevation);
    relativeAzimuthUI.SetFieldValue(inData.relativeAzimuth);
    defaultLightWUI.SetFieldValue(inData.defaultLightW);

    SetAllSensitive();
}
void PPD_3DLightingPS::SetMenuData(const PC_Lighting&   inData,
                                            MenuObjC& inObj)
{
    SetMenuData(inData);
    MenuBaseC::SetMenuData(inObj);
}

bool PPD_3DLightingPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    if (useLightingUI.GetValue() && useDefaultLightUI.GetValue())
    {
        if (defaultLightIsFixedUI.GetBoolChoice())
        {
            fixedElevationUI.FieldValueOK(allOK, "Fix El", -90.0, 90.0);
            fixedAzimuthUI.FieldValueOK(allOK, "Fix Az", -180.0, 180.0);
        }
        else
        {
            relativeElevationUI.FieldValueOK(allOK, "Rel El", -90.0, 90.0);
            relativeAzimuthUI.FieldValueOK(allOK, "Rel Az", -180.0, 180.0);
        }
        defaultLightWUI.FieldValueOK(allOK, "default W", 0.0, 10.0);
    }
    return allOK;
}

void PPD_3DLightingPS::GetMenuData(PC_Lighting& outData,
                                    MenuObjC&   outObj)
{
    MenuBaseC::GetMenuData(outObj);

    outData.useLighting = useLightingUI.GetValue();
    if (outData.useLighting)
    {
        outData.useDefaultLight = useDefaultLightUI.GetValue();
        outData.showDefaultLight = showDefaultLightUI.GetValue();
        ambientLightUI.GetPenAndSlider(outData.ambientPen, outData.ambientIntensity);
        if (outData.useDefaultLight)
        {
            defaultDiffuseLightUI.GetPenAndSlider(outData.defaultDiffuseLightPen, outData.defaultDiffuseLightIntensity);
            defaultSpecularLightUI.GetPenAndSlider(outData.defaultSpecularLightPen, outData.defaultSpecularLightIntensity);

            outData.defaultLightIsFixed = defaultLightIsFixedUI.GetBoolChoice();
            if (outData.defaultLightIsFixed)
            {
                outData.fixedElevation = fixedElevationUI.GetFieldValue();
                outData.fixedAzimuth   = fixedAzimuthUI.GetFieldValue();
            }
            else
            {
                outData.relativeElevation = relativeElevationUI.GetFieldValue();
                outData.relativeAzimuth = relativeAzimuthUI.GetFieldValue();
            }
             outData.defaultLightW = defaultLightWUI.GetFieldValue();
        }
    }
}
void PPD_3DLightingPS::MenuDefault()
{
    SetMenuData(PC_Lighting());
}

