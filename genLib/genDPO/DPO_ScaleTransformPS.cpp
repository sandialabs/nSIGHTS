///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ScaleTransformPS.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              User interface for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include "DPO_ScaleTransform.h"
#include "DPO_ScaleTransformPS.h"


DPO_ScaleTransformPS::DPO_ScaleTransformPS() :
    scaleValueMSUI(typeid(DO_Real), true),
    offsetValueMSUI(typeid(DO_Real), true)
{
}


void DPO_ScaleTransformPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ScaleTransformPS*>(inObj)->SetAllSensitive();
}

void DPO_ScaleTransformPS::SetAllSensitive()
{
    scaleValueMSUI.SetAllSensitive();
    offsetValueMSUI.SetAllSensitive();
    setNullValueUI.SetSensitive(processNullsUI.GetValue());
}


void DPO_ScaleTransformPS::ToggleExposedScaleValueCB(void* inObj)
{
    static_cast<DPO_ScaleTransformPS*>(inObj)->ToggleExposedScaleValue();
}

void DPO_ScaleTransformPS::ToggleExposedScaleValue()
{
    SetAllSensitive();
    // recast current to local
    static_cast<DPO_ScaleTransform*>(MenuObjC::currMenuObj)->
        exposedScaleValue.ToggleExposed(scaleValueMSUI.TurnExposedOff());
}

void DPO_ScaleTransformPS::ToggleExposedOffsetValueCB(void* inObj)
{
    static_cast<DPO_ScaleTransformPS*>(inObj)->ToggleExposedOffsetValue();
}

void DPO_ScaleTransformPS::ToggleExposedOffsetValue()
{
    SetAllSensitive();
    static_cast<DPO_ScaleTransform*>(MenuObjC::currMenuObj)->
        exposedOffsetValue.ToggleExposed(offsetValueMSUI.TurnExposedOff());
}

void DPO_ScaleTransformPS::InitMenu()
{
    MenuBaseC::InitMenu();

}

void DPO_ScaleTransformPS::InitSTCommon(ArgListC& attachments)
{
    topRowForm.InitWidget(PPform.GetWidget(), attachments);

    ArgListC args;
    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();

    args.LeftFormAttach();
    args.RightPosAttach(35);
    scaleFirstUI.PreInitSetUp("Transform -> Scale", "Scale -> Transform");
    scaleFirstUI.InitFramedUI("Operation Order", topRowForm.GetWidget(), args);


    static const char* scalLabels[]  = {"D * Sc + Off", "(D + Off) * Sc",
                                        "D / Sc + Off", "(D + Off) / Sc"};
    args.ResetPlace();
    args.LeftUICAttach(scaleFirstUI);
    args.RightPosAttach(70);
    scaleOpUI.PreInitSetUp(scalLabels, 4);
    scaleOpUI.InitFramedUI("Scale Operation", topRowForm.GetWidget(), args);

    args.ResetPlace();
    args.LeftUICAttach(scaleOpUI);
    args.RightFormAttach();

    // transform option
    static const char* tranLabels[]  = {"None", "ln(Data)", "log10(Data)", "Sqrt(Data)", "1 / Data",
                                        "exp(Data)", "10**(Data)", "Data * Data", "Abs(Data)"};

    tranOpUI.PreInitSetUp(tranLabels, 9);
    tranOpUI.InitFramedUI("Transform", topRowForm.GetWidget(), args);


    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    scaleValueMSUI.InitUI("Scale", args, PPform.GetWidget(), true, true, ToggleExposedScaleValueCB, this);

    args.Clear();
    args.StdTopObjectAttach(scaleValueMSUI);
    offsetValueMSUI.InitUI("Offset", args, PPform.GetWidget(), true, true, ToggleExposedOffsetValueCB, this);

    args.Clear();
    args.StdTopObjectAttach(offsetValueMSUI);
    nullFrame.InitFramedUI("Null Processing", PPform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftOffset(5);
    args.LeftFormAttach();
    processNullsUI.StdToggleInit("set to constant", nullFrame.GetForm(), args);
    processNullsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.ResetPlace();
    args.LeftPosAttach(50);
    args.RightFormAttach();
    setNullValueUI.InitLabeledUI("Constant:", nullFrame.GetForm(), args);


}

void DPO_ScaleTransformPS::SetMenuData(DPO_ScaleTransform&  inData)
{
    MenuBaseC::SetMenuData(inData);

    scaleFirstUI.SetBoolChoice(inData.bpRef.scaleFirst);
    scaleOpUI.SetChoice(int(inData.bpRef.scaleOp));
    tranOpUI.SetChoice(int(inData.bpRef.tranOp));

    scaleValueMSUI.SetValue(inData.bpRef.scaleMS, inData.exposedScaleValue);
    offsetValueMSUI.SetValue(inData.bpRef.offsetMS, inData.exposedOffsetValue);

    setNullValueUI.SetFieldValue(inData.bpRef.setNullValue);
    processNullsUI.SetValue(inData.bpRef.processNulls);

    SetAllSensitive();
}

void DPO_ScaleTransformPS::MenuOpen(DPO_ScaleTransform& inData)
{
    MenuBaseC::MenuOpen(true, false);
    scaleValueMSUI.MenuInit(inData.bpRef.scaleMS);
    offsetValueMSUI.MenuInit(inData.bpRef.offsetMS);
}


bool DPO_ScaleTransformPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    scaleValueMSUI.UIdataOK(allOK);
    offsetValueMSUI.UIdataOK(allOK);
    if (processNullsUI.GetValue())
        setNullValueUI.FieldValueOK(allOK, "null value");
    return allOK;
}

void DPO_ScaleTransformPS::GetMenuData(DPO_ScaleTransform&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.bpRef.scaleFirst = scaleFirstUI.GetBoolChoice();
    outData.bpRef.scaleOp    = DSC_ScaleTransform::ScaleType(scaleOpUI.GetChoice());
    outData.bpRef.tranOp     = DSC_ScaleTransform::TransformType(tranOpUI.GetChoice());

    scaleValueMSUI.GetValue(outData.bpRef.scaleMS);
    offsetValueMSUI.GetValue(outData.bpRef.offsetMS);

    outData.bpRef.processNulls = processNullsUI.GetValue();
    if (outData.bpRef.processNulls)
        outData.bpRef.setNullValue  = setNullValueUI.GetFieldValue();
}

void DPO_ScaleTransformPS::MenuDefault()
{
    MenuBaseC::MenuDefault();
    scaleValueMSUI.realValueUI.SetFieldValue(1.0);
    offsetValueMSUI.realValueUI.SetFieldValue(0.0);
    tranOpUI.SetChoice(int(DFO_ScaleTransform::tt_None));
}

