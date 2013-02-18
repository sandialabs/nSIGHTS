///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CreateBEETResponseFunctionPS.cpp
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

#include "DPO_CreateBEETResponseFunction.h"
#include "DPO_CreateBEETResponseFunctionPS.h"

DPO_CreateBEETResponseFunctionPS::DPO_CreateBEETResponseFunctionPS() :
    pressureInputObjRefUI(typeid(DO_XYData)),
    barometricInputObjRefUI(typeid(DO_XYData)),
    earthTideInputObjRefUI(typeid(DO_XYData)),
    maxTimeDelayMSUI(typeid(DO_Real), true),
    specTimeSpacingMSUI(typeid(DO_Real), true)
{
}


void DPO_CreateBEETResponseFunctionPS::SetAllSensitive()
{
    barometricInputObjRefUI.SetSensitive(calcBEUI.GetValue());
    earthTideInputObjRefUI.SetSensitive(calcETUI.GetValue());
    specTimeSpacingMSUI.SetSensitive(timeBaseUI.GetChoice() == 0);
    maxTimeDelayMSUI.SetAllSensitive();
}

void DPO_CreateBEETResponseFunctionPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_CreateBEETResponseFunctionPS*>(inObj)->SetAllSensitive();
}

void DPO_CreateBEETResponseFunctionPS::ToggleExposedMaxTimeDelayValue()
{
    SetAllSensitive();

    // recast current to local
    static_cast<DPO_CreateBEETResponseFunction*>(MenuObjC::currMenuObj)->exposedMaxTimeDelayValue.ToggleExposed(maxTimeDelayMSUI.TurnExposedOff());
}

void DPO_CreateBEETResponseFunctionPS::ToggleExposedMaxTimeDelayValueCB(void* inObj)
{
    static_cast<DPO_CreateBEETResponseFunctionPS*>(inObj)->ToggleExposedMaxTimeDelayValue();
}


void DPO_CreateBEETResponseFunctionPS::ToggleExposedTimeSpacingValue()
{
    if (DFO_CreateBEETResponseFunction::TimeBase(timeBaseUI.GetChoice())!= DFO_CreateBEETResponseFunction::tbSpecific)
        specTimeSpacingMSUI.realValueMS.isExposed.SetValue(false);

    SetAllSensitive();

    // recast current to local
    static_cast<DPO_CreateBEETResponseFunction*>(MenuObjC::currMenuObj)->exposedTimeSpacingValue.ToggleExposed(specTimeSpacingMSUI.TurnExposedOff());
}

void DPO_CreateBEETResponseFunctionPS::ToggleExposedTimeSpacingValueCB(void* inObj)
{
    static_cast<DPO_CreateBEETResponseFunctionPS*>(inObj)->ToggleExposedTimeSpacingValue();
}

void DPO_CreateBEETResponseFunctionPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    pressureInputObjRefUI.InitFramedUI("P(t)Data to Compensate", URform.GetWidget(), args);

    args.StdFormAttach();
    baroFrame.InitFramedUI("Barometric Pressure", PPform.GetWidget(), args);

    args.StdSplitSetup(25);
    args.StdToggleSetup(5);
    calcBEUI.StdToggleInit("Calculate?", baroFrame.GetForm(), args);
    calcBEUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(75);
    barometricInputObjRefUI.SetListWidth(55);
    barometricInputObjRefUI.InitLabeledUI("Barometric P(t) Data:", baroFrame.GetForm(), args);

    args.StdTopObjectAttach(baroFrame.GetForm());
    etFrame.InitFramedUI("Earth Tide Effects", PPform.GetWidget(), args);

    args.StdSplitSetup(25);
    args.StdToggleSetup(5);
    calcETUI.StdToggleInit("Calculate?", etFrame.GetForm(), args);
    calcETUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(75);
    earthTideInputObjRefUI.SetListWidth(55);
    earthTideInputObjRefUI.InitLabeledUI("Earth Tide Data:", etFrame.GetForm(), args);

    args.StdTopObjectAttach(etFrame.GetForm());
    maxTimeDelayMSUI.InitUI("Maximum Time Delay", args, PPform.GetWidget(), true, true, ToggleExposedMaxTimeDelayValueCB, this);

    args.StdTopObjectAttach(maxTimeDelayMSUI);


    /////////////////////////////////////////
    timeBaseRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(28);
    const char* methodStrs[] = {"Specified", "Average Pressure Data", "Average Barometric Data", "Average Earth Tide Data"};
    timeBaseUI.PreInitSetUp(methodStrs, 4);
    timeBaseUI.InitFramedUI("Time Spacing", timeBaseRow.GetWidget(), args);
    timeBaseUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(72);
    specTimeSpacingMSUI.InitUI("Specific Time Spacing", args, timeBaseRow.GetWidget(), true, true, ToggleExposedTimeSpacingValueCB, this);
    args.StdTopObjectAttach(timeBaseRow.GetWidget());

    /////////////////////////////////////////
    options.InitFramedUI("Options", PPform.GetWidget(), args);
    args.StdSplitSetup(25);
    static const char * optLabels[] = {"No conversion", "Convert PSIA to PSIG", "Convert PSIG to PSIA"};
    convertpressurePUI.PreInitSetUp(optLabels, 3);
    convertpressurePUI.InitFramedUI("Pressure Type Conversion", options.GetForm(), args);
    convertpressurePUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
    args.StdSplitSetup(75);
    args.StdTopObjectAttach(options.GetForm());
    /////////////////////////////////////////

    //status
    statusFrame.InitFramedUI("Status", PPform.GetWidget(), args);
    args.StdFormAttach(5);
    args.LeftOffset(10);
    args.RightOffset(10);
    timeSpacingStatus.InitLabeledUI("Time Spacing", statusFrame.GetForm(), args);

    args.StdTopObjectAttach(timeSpacingStatus);
    args.LeftOffset(10);
    args.RightOffset(10);
    memoryStatus.InitLabeledUI("Memory (# Time Delay Increments)", statusFrame.GetForm(), args);

}

void DPO_CreateBEETResponseFunctionPS::MenuOpen(DPO_CreateBEETResponseFunction& inData)
{
    MenuBaseC::MenuOpen(false, false);
    pressureInputObjRefUI.StdMenuInit(inData.pressureInputObjRef);
    barometricInputObjRefUI.StdMenuInit(inData.barometricInputObjRef);
    earthTideInputObjRefUI.StdMenuInit(inData.earthTideInputObjRef);
    maxTimeDelayMSUI.MenuInit(inData.maxTimeDelayMS);
    specTimeSpacingMSUI.MenuInit(inData.specTimeSpacingMS);
}

void DPO_CreateBEETResponseFunctionPS::SetMenuData(DPO_CreateBEETResponseFunction&  inData)
{
    MenuBaseC::SetMenuData(inData);

    pressureInputObjRefUI.SetFuncObjRef(inData.pressureInputObjRef);
    barometricInputObjRefUI.SetFuncObjRef(inData.barometricInputObjRef);
    earthTideInputObjRefUI.SetFuncObjRef(inData.earthTideInputObjRef);

    calcBEUI.SetValue(inData.calcBE);
    calcETUI.SetValue(inData.calcET);
    convertpressurePUI.SetChoice(inData.convertType);

    maxTimeDelayMSUI.SetValue(inData.maxTimeDelayMS, inData.exposedMaxTimeDelayValue);
    timeBaseUI.SetChoice(inData.timeBase);
    specTimeSpacingMSUI.SetValue(inData.specTimeSpacingMS, inData.exposedTimeSpacingValue);

    if (inData.StatusOK())
    {
        timeSpacingStatus.SetFieldValue(inData.timeSpacing);
        memoryStatus.SetFieldValue(inData.timeIntervalLag);
    }
    else
    {
        timeSpacingStatus.SetStatusNA();
        memoryStatus.SetStatusNA();
    }

    statusFrame.SetSensitive(inData.StatusOK());

    SetAllSensitive();
}

bool DPO_CreateBEETResponseFunctionPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    pressureInputObjRefUI.FuncObjRefOK(allOK, "P(t) input");
    if (calcBEUI.GetValue())
        barometricInputObjRefUI.FuncObjRefOK(allOK, "Barometric input");
    if (calcETUI.GetValue())
        earthTideInputObjRefUI.FuncObjRefOK(allOK, "Earth tide input");

    maxTimeDelayMSUI.UIdataOK(allOK, 1.0E-30, 100);
    if (timeBaseUI.GetChoice() == 0)
        specTimeSpacingMSUI.UIdataOK(allOK, 0.001, 1.0E6);

    return allOK;
}

void DPO_CreateBEETResponseFunctionPS::GetMenuData(DPO_CreateBEETResponseFunction&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.pressureInputObjRef = pressureInputObjRefUI.GetFuncObjRef();

    outData.calcBE = calcBEUI.GetValue();
    if (outData.calcBE)
        outData.barometricInputObjRef = barometricInputObjRefUI.GetFuncObjRef();

    outData.calcET = calcETUI.GetValue();
    if (outData.calcET)
        outData.earthTideInputObjRef = earthTideInputObjRefUI.GetFuncObjRef();

    maxTimeDelayMSUI.GetValue(outData.maxTimeDelayMS);

    outData.timeBase = DFO_CreateBEETResponseFunction::TimeBase(timeBaseUI.GetChoice());
    if (outData.timeBase == DFO_CreateBEETResponseFunction::tbSpecific)
    {
        specTimeSpacingMSUI.GetValue(outData.specTimeSpacingMS);
    }

    outData.convertType = convertpressurePUI.GetChoice();
}

