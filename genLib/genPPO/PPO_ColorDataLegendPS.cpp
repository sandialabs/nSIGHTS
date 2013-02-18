///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ColorDataLegendPS.cpp
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

#include "PPO_ColorDataLegend.h"
#include "PPO_ColorDataLegendPS.h"

// ****************************************

void PPO_ColorDataLegendPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_ColorDataLegendPS*>(inObj)->SetAllSensitive();
}

void PPO_ColorDataLegendPS::SetAllSensitive()
{
    selectionObjRefUI.SetSensitive(frameSelectionUI.GetValue());
}

// ****************************************

PPO_ColorDataLegendPS::PPO_ColorDataLegendPS() :
    labelArrayObjRefUI(typeid(DO_LabelArray)),
    selectionObjRefUI(typeid(DO_Real))
{
}


void PPO_ColorDataLegendPS::InitMenu(const PPO_ColorDataLegend& inData)
{
    PPO_AnnoBasePS::InitMenu(inData);

    ArgListC    args;

    args.StdFormAttach();

    colorLegendUI.StdInit(PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(colorLegendUI.labelIncForm.GetWidget());
    incrementForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(70);
    labelArrayObjRefUI.InitFramedUI("Increment Label Source", incrementForm.GetWidget(), args);

    args.StdSplitFinish(30);
    reductionFactorUI.SetFieldFormat(SC_RealFormat(3));
    reductionFactorUI.SetFieldLen(6);
    reductionFactorUI.InitFramedUI("Reduction", incrementForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(incrementForm.GetWidget());
    selectionFrame.InitFramedUI("Selection ", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    frameSelectionUI.StdToggleInit("Frame", selectionFrame.GetForm(), args);
    frameSelectionUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitSetup(40);
    args.StdSplitFinish(60);
    selectionObjRefUI.InitLabeledUI("Location", selectionFrame.GetForm(), args);

}

void PPO_ColorDataLegendPS::MenuOpen(PPO_ColorDataLegend& inData)
{
    MenuBaseC::MenuOpen(true, false);
    colorLegendUI.MenuOpen(inData, inData);
    labelArrayObjRefUI.StdMenuInit(inData.labelArrayObjRef);
    selectionObjRefUI.StdMenuInit(inData.selectionObjRef);
}

void PPO_ColorDataLegendPS::SetMenuData(PPO_ColorDataLegend& inData)
{
    PPO_AnnoBasePS::SetMenuData(inData);
    colorLegendUI.SetColorLegendSpec(inData, inData);
    labelArrayObjRefUI.SetFuncObjRef(inData.labelArrayObjRef);
    reductionFactorUI.SetFieldValue(inData.reductionFactor);
    selectionObjRefUI.SetFuncObjRef(inData.selectionObjRef);
    frameSelectionUI.SetValue(inData.frameSelection);
    SetAllSensitive();
}

bool PPO_ColorDataLegendPS::UIdataOK()
{
    PPO_AnnoBasePS::UIdataOK();
    colorLegendUI.UIdataOK(allOK);
    labelArrayObjRefUI.FuncObjRefOK(allOK, "label map source");
    reductionFactorUI.FieldValueOK(allOK, "reduction" , 0.10, 1.00);
    if (frameSelectionUI.GetValue())
        selectionObjRefUI.FuncObjRefOK(allOK, "selection pos");

    return allOK;
}

void PPO_ColorDataLegendPS::GetMenuData(PPO_ColorDataLegend& outData)
{
    PPO_AnnoBasePS::GetMenuData(outData);
    colorLegendUI.GetColorLegendSpec(outData, outData);
    outData.labelArrayObjRef = labelArrayObjRefUI.GetFuncObjRef();
    outData.reductionFactor = reductionFactorUI.GetFieldValue();

    outData.frameSelection = frameSelectionUI.GetValue();
    if (outData.frameSelection)
        outData.selectionObjRef = selectionObjRefUI.GetFuncObjRef();

}

void  PPO_ColorDataLegendPS::UpdateULUI(PPO_ColorDataLegend&  inData)
{
    colorLegendUI.xField.SetFieldValue(inData.ulLocation.pX);
    colorLegendUI.yField.SetFieldValue(inData.ulLocation.pY);
}

