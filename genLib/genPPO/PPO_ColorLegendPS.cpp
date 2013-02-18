///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ColorLegendPS.cpp
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

#include <genClass/DO_ColorLimit.h>
#include <genClass/U_Str.h>

#include "PPO_ColorLegend.h"
#include "PPO_ColorLegendPS.h"

// ****************************************

void PPO_ColorLegendPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_ColorLegendPS*>(inObj)->SetAllSensitive();
}

void PPO_ColorLegendPS::SetAllSensitive()
{
    bool useLabels = specLabelsUI.GetValue();
    labelArrayObjRefUI.SetSensitive(useLabels);
    useLabelXUI.SetSensitive(useLabels);
    incrementForm.SetSensitive(!useLabels);
    if (!useLabels)
    {
        legendFormatUI.SetSensitive(true);
        includeSEUI.SetSensitive(autoIncsToggle.GetValue());
        nmanualIncField.SetSensitive(!autoIncsToggle.GetValue());
    }
}

// ****************************************

PPO_ColorLegendPS::PPO_ColorLegendPS() :
    labelArrayObjRefUI(typeid(DO_LabelArray)),
    legendFormatUI(true)
{
}

void PPO_ColorLegendPS::InitMenu(const PPO_ColorLegend& inData)
{
    PPO_AnnoBasePS::InitMenu(inData);

    ArgListC    args;

    args.StdFormAttach();

    colorLegendUI.StdInit(PPform.GetWidget(), args);

    //form for specified increments
    args.StdTopObjectAttach(colorLegendUI.labelIncForm.GetWidget());
    specFrame.InitFramedUI("Specified Labels", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    specLabelsUI.StdToggleInit("Specify Labels?", specFrame.GetForm(), args);
    specLabelsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(35);
    useLabelXUI.StdToggleInit("inc at label X", specFrame.GetForm(), args);

    args.StdSplitFinish(60);
    labelArrayObjRefUI.InitFormUI(specFrame.GetForm(), args);

    // form for format and incs
    args.StdTopObjectAttach(specFrame);
    incrementForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(50);
    legendFormatUI.InitFramedUI("Increment Label Format", incrementForm.GetWidget(), args);

    args.StdSplitFinish(50);
    incFrame.InitFramedUI("Increments", incrementForm.GetWidget(), args);

    args.StdToggleSetup(5);
    autoIncsToggle.StdToggleInit("Auto", incFrame.GetForm(), args);
    autoIncsToggle.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(25);
    includeSEUI.StdToggleInit("add St/End", incFrame.GetForm(), args);

    args.StdSplitFinish(70);
    nmanualIncField.textFieldLen = 5;
    nmanualIncField.InitFormUI(incFrame.GetForm(), args);
}

void PPO_ColorLegendPS::MenuOpen(PPO_ColorLegend& inData)
{
    MenuBaseC::MenuOpen(true, false);
    colorLegendUI.MenuOpen(inData, inData);
    labelArrayObjRefUI.StdMenuInit(inData.labelArrayObjRef);
}

void PPO_ColorLegendPS::SetMenuData(PPO_ColorLegend& inData)
{
    PPO_AnnoBasePS::SetMenuData(inData);
    colorLegendUI.SetColorLegendSpec(inData, inData);

    specLabelsUI.SetValue(inData.specIncs);
    useLabelXUI.SetValue(inData.useLabelX);
    labelArrayObjRefUI.SetFuncObjRef(inData.labelArrayObjRef);

    legendFormatUI.SetRealFormatSettings(inData.legendFormat);
    autoIncsToggle.SetValue(inData.autoIncs);
    includeSEUI.SetValue(inData.includeSE);
    nmanualIncField.SetFieldValue(inData.nmanualIncs);

    SetAllSensitive();
}

bool PPO_ColorLegendPS::UIdataOK()
{
    PPO_AnnoBasePS::UIdataOK();
    colorLegendUI.UIdataOK(allOK);
    if (specLabelsUI.GetValue())
        labelArrayObjRefUI.FuncObjRefOK(allOK, "label map source");
    if (!autoIncsToggle.GetValue())
        nmanualIncField.FieldValueOK(allOK, "# incs" , 1, 10);
    return allOK;
}

void PPO_ColorLegendPS::GetMenuData(PPO_ColorLegend& outData)
{
    PPO_AnnoBasePS::GetMenuData(outData);
    colorLegendUI.GetColorLegendSpec(outData, outData);

    outData.specIncs = specLabelsUI.GetValue();
    if (outData.specIncs)
    {
        outData.useLabelX = useLabelXUI.GetValue();
        outData.labelArrayObjRef = labelArrayObjRefUI.GetFuncObjRef();
    }
    else
    {
        legendFormatUI.GetRealFormatSettings(outData.legendFormat);
        outData.autoIncs = autoIncsToggle.GetValue();
        if (outData.autoIncs)
            outData.includeSE = includeSEUI.GetValue();
        else
            outData.nmanualIncs = nmanualIncField.GetFieldValue();
    }
}

void  PPO_ColorLegendPS::UpdateULUI(PPO_ColorLegend&  inData)
{
    colorLegendUI.xField.SetFieldValue(inData.ulLocation.pX);
    colorLegendUI.yField.SetFieldValue(inData.ulLocation.pY);
}

