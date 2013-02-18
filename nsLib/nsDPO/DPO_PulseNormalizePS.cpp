///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PulseNormalizePS.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "nsLibAfx.h"

#include <nsClass/DO_TimeProcess.h>

#include <genClassPS/PS_UIBase.h>
#include <genApp/ExposedReal.h>

#include "DPO_PulseNormalize.h"
#include "DPO_PulseNormalizePS.h"

DPO_PulseNormalizePS::DPO_PulseNormalizePS() :
    xyInputObjRefUI(typeid(DO_XYData)),
    p0_MSUI(typeid(DO_Real), true),
    pI_MSUI(typeid(DO_Real), true)
{
}

void DPO_PulseNormalizePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_PulseNormalizePS*>(inObj)->SetAllSensitive();
}

void DPO_PulseNormalizePS::SetAllSensitive()
{
    pI_MSUI.SetAllSensitive();
    p0_MSUI.SetAllSensitive();
}

void DPO_PulseNormalizePS::ToggleExposedP0Value()
{
    SetAllSensitive();
    static_cast<DPO_PulseNormalize*>(MenuObjC::currMenuObj)->exposedP0Value.ToggleExposed(p0_MSUI.TurnExposedOff());
}

void DPO_PulseNormalizePS::ToggleExposedP0ValueCB(void* inObj)
{
    static_cast<DPO_PulseNormalizePS*>(inObj)->ToggleExposedP0Value();
}

void DPO_PulseNormalizePS::ToggleExposedPIValue()
{
    SetAllSensitive();
    static_cast<DPO_PulseNormalize*>(MenuObjC::currMenuObj)->exposedPIValue.ToggleExposed(pI_MSUI.TurnExposedOff());
}

void DPO_PulseNormalizePS::ToggleExposedPIValueCB(void* inObj)
{
    static_cast<DPO_PulseNormalizePS*>(inObj)->ToggleExposedPIValue();
}

void DPO_PulseNormalizePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    xyInputObjRefUI.InitFramedUI("Input P(t) Data", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    static const char* pulseNormalizationTypeLabels[]  = {"  (Pi - P(t)) / (Pi - P0) ",
                                                    "   1 - (Pi - P(t)) / (Pi - P0)  "};
    pulseNormalizationTypeUI.PreInitSetUp(pulseNormalizationTypeLabels, 2);
    pulseNormalizationTypeUI.InitFramedUI("Pulse Normalization Type", PPform.GetWidget(), args);
    pulseNormalizationTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(pulseNormalizationTypeUI);
    p0_MSUI.InitUI("P0 (initial pulse pressure)", args, PPform.GetWidget(), true, false, ToggleExposedP0ValueCB, this);

    args.Clear();
    args.StdTopObjectAttach(p0_MSUI);
    pI_MSUI.InitUI("Pi (static pressure)", args, PPform.GetWidget(), true, false, ToggleExposedPIValueCB, this);


    args.Clear();
    args.StdTopObjectAttach(pI_MSUI);
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    multiplyYbyXUI.StdToggleInit("Multiply Y by X", optionsFrame.GetForm(), args);
}

void DPO_PulseNormalizePS::MenuOpen(DPO_PulseNormalize& inData)
{
    MenuBaseC::MenuOpen(false, false);
    xyInputObjRefUI.StdMenuInit(inData.xyInputObjRef);
    pI_MSUI.MenuInit(inData.pI_MS);
    p0_MSUI.MenuInit(inData.p0_MS);
}

void DPO_PulseNormalizePS::SetMenuData(DPO_PulseNormalize&  inData)
{
    xyInputObjRefUI.SetFuncObjRef(inData.xyInputObjRef);
    pulseNormalizationTypeUI.SetChoice(inData.pulseNormalizationType);
    p0_MSUI.SetValue(inData.p0_MS, inData.exposedP0Value);
    pI_MSUI.SetValue(inData.pI_MS, inData.exposedPIValue);
    multiplyYbyXUI.SetValue(inData.multiplyYbyX);

    MenuBaseC::SetMenuData(inData);

    SetAllSensitive();
}

bool DPO_PulseNormalizePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    xyInputObjRefUI.FuncObjRefOK(allOK, "P(t) input");
    p0_MSUI.UIdataOK(allOK);
    pI_MSUI.UIdataOK(allOK);
    return allOK;
}

void DPO_PulseNormalizePS::GetMenuData(DPO_PulseNormalize&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.xyInputObjRef = xyInputObjRefUI.GetFuncObjRef();
    outData.pulseNormalizationType = DFO_PulseNormalize::PulseNormalizationType(pulseNormalizationTypeUI.GetChoice());
    p0_MSUI.GetValue(outData.p0_MS);
    pI_MSUI.GetValue(outData.pI_MS);
    outData.multiplyYbyX = multiplyYbyXUI.GetValue();


}

