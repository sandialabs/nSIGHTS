///////////////////////////////////////////////////////////////////////////////////
//
// DPO_HeadCalculationPS.cpp
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

#include "DPO_HeadCalculation.h"
#include "DPO_HeadCalculationPS.h"

DPO_HeadCalculationPS::DPO_HeadCalculationPS() :
    inputPressureObjRefUI(typeid(DO_XYData)),
    inputDensityObjRefUI(typeid(DO_XYData)),
    groundSurfaceElevationMSUI(typeid(DO_Real), false)
{
}


void DPO_HeadCalculationPS::SetAllSensitive()
{
    bool isNotFreshwater = headCalculationUI.GetChoice() != 0;
    inputDensityObjRefUI.SetSensitive(isNotFreshwater);
}

void DPO_HeadCalculationPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_HeadCalculationPS*>(inObj)->SetAllSensitive();
}


void DPO_HeadCalculationPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputPressureObjRefUI.InitFramedUI("Pressure v Depth (kPa) Data to Convert", URform.GetWidget(), args);

    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    const char* methodStrs[] = {"Freshwater", "Environmental", "Density Neutral"};
    headCalculationUI.PreInitSetUp(methodStrs, 3);
    headCalculationUI.InitFramedUI("Head Calculation", topRow.GetWidget(), args);
    headCalculationUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    outputIsElevationUI.PreInitSetUp("Depth [mBGS]","Elevation [mASL]");
    outputIsElevationUI.InitFramedUI("Output Depth", topRow.GetWidget(), args);

    args.StdTopObjectAttach(topRow.GetWidget());
    inputDensityObjRefUI.InitFramedUI("Density [kg/m^3] Profile Data", PPform.GetWidget(), args);

    args.StdTopObjectAttach(inputDensityObjRefUI);
    groundSurfaceElevationMSUI.InitUI("Ground Surface", args, PPform.GetWidget(), true, true);

}

void DPO_HeadCalculationPS::MenuOpen(DPO_HeadCalculation& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputPressureObjRefUI.StdMenuInit(inData.inputPressureObjRef);
    inputDensityObjRefUI.StdMenuInit(inData.inputDensityObjRef);
    groundSurfaceElevationMSUI.MenuInit(inData.groundSurfaceElevationMS);
}

void DPO_HeadCalculationPS::SetMenuData(DPO_HeadCalculation&  inData)
{
    MenuBaseC::SetMenuData(inData);

    inputPressureObjRefUI.SetFuncObjRef(inData.inputPressureObjRef);
    inputDensityObjRefUI.SetFuncObjRef(inData.inputDensityObjRef);
    groundSurfaceElevationMSUI.SetValue(inData.groundSurfaceElevationMS);

    headCalculationUI.SetChoice(int(inData.headCalculation));
    outputIsElevationUI.SetBoolChoice(inData.outputIsElevation);

    SetAllSensitive();
}

bool DPO_HeadCalculationPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputPressureObjRefUI.FuncObjRefOK(allOK, "Pressure input");
    if (headCalculationUI.GetChoice() != 0)
        inputDensityObjRefUI.FuncObjRefOK(allOK, "Barometric input");

    groundSurfaceElevationMSUI.UIdataOK(allOK);
    return allOK;
}

void DPO_HeadCalculationPS::GetMenuData(DPO_HeadCalculation&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputPressureObjRef = inputPressureObjRefUI.GetFuncObjRef();
    if (headCalculationUI.GetChoice() != 0)
        outData.inputDensityObjRef = inputDensityObjRefUI.GetFuncObjRef();
    outData.headCalculation = DFO_HeadCalculation::HeadCalculation(headCalculationUI.GetChoice());
    groundSurfaceElevationMSUI.GetValue(outData.groundSurfaceElevationMS);
    outData.outputIsElevation = outputIsElevationUI.GetBoolChoice();
}

