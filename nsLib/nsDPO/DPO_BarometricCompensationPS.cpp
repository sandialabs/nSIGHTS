///////////////////////////////////////////////////////////////////////////////////
//
// DPO_BarometricCompensationPS.cpp
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

#include "DPO_BarometricCompensation.h"
#include "DPO_BarometricCompensationPS.h"

DPO_BarometricCompensationPS::DPO_BarometricCompensationPS() :
    pressureInputObjRefUI(typeid(DO_XYData)),
    barometricInputObjRefUI(typeid(DO_XYData)),
    barometricEfficiencyMSUI(typeid(DO_Real), false)
{
}


void DPO_BarometricCompensationPS::SetAllSensitive()
{
    bool isEfficiency = 1;
    //bool isEfficiency = correctionMethodUI.GetChoice() == 0;

    barometricEfficiencyMSUI.SetSensitive(isEfficiency);

    regressLagUI.SetSensitive(!isEfficiency);
    regressMUI.SetSensitive(!isEfficiency);

    evenTimeSpacingUI.SetSensitive(!isEfficiency);
    timeSpacingUI.SetSensitive((!isEfficiency) && evenTimeSpacingUI.GetValue());

}

void DPO_BarometricCompensationPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_BarometricCompensationPS*>(inObj)->SetAllSensitive();
}


void DPO_BarometricCompensationPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    pressureInputObjRefUI.InitFramedUI("P(t)Data to Compensate", URform.GetWidget(), args);

    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    barometricInputObjRefUI.InitFramedUI("Barometric P(t) Data", topRow.GetWidget(), args);

    //args.StdSplitFinish();
    //const char* methodStrs[] = {"Efficiency", "Rasmussen Deconvolution"};
    //correctionMethodUI.PreInitSetUp(methodStrs, 2);
    //correctionMethodUI.InitFramedUI("Correction Method", topRow.GetWidget(), args);
    //correctionMethodUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(topRow.GetWidget());
    barometricEfficiencyMSUI.InitUI("Barometric Efficiency", args, PPform.GetWidget(), true, true);

    args.StdTopObjectAttach(barometricEfficiencyMSUI);
    regressRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    evenSpaceFrame.InitFramedUI("Spacing", regressRow.GetWidget(), args);

    args.StdToggleSetup(2);
    evenTimeSpacingUI.StdToggleInit("force even ", evenSpaceFrame.GetForm(), args);
    evenTimeSpacingUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(60);
    timeSpacingUI.AttachObjectToLabel();
    timeSpacingUI.InitLabeledUI("even increment", evenSpaceFrame.GetForm(), args);

    args.StdSplitMiddle(50, 75);
    regressLagUI.AttachObjectToFrame();
    regressLagUI.InitFramedUI("Regression Lag ", regressRow.GetWidget(), args);

    args.StdSplitFinish(25);
    regressMUI.AttachObjectToFrame();
    regressMUI.InitFramedUI("Regression Memory", regressRow.GetWidget(), args);
}

void DPO_BarometricCompensationPS::MenuOpen(DPO_BarometricCompensation& inData)
{
    MenuBaseC::MenuOpen(false, false);
    pressureInputObjRefUI.StdMenuInit(inData.pressureInputObjRef);
    barometricInputObjRefUI.StdMenuInit(inData.barometricInputObjRef);
    barometricEfficiencyMSUI.MenuInit(inData.barometricEfficiencyMS);
}

void DPO_BarometricCompensationPS::SetMenuData(DPO_BarometricCompensation&  inData)
{
    MenuBaseC::SetMenuData(inData);

    pressureInputObjRefUI.SetFuncObjRef(inData.pressureInputObjRef);
    barometricInputObjRefUI.SetFuncObjRef(inData.barometricInputObjRef);
    barometricEfficiencyMSUI.SetValue(inData.barometricEfficiencyMS);

    //correctionMethodUI.SetChoice(int(inData.correctionMethod));
    regressLagUI.SetFieldValue(inData.regressLag);
    regressMUI.SetFieldValue(inData.regressM);

    evenTimeSpacingUI.SetValue(inData.evenTimeSpacing);
    timeSpacingUI.SetFieldValue(inData.timeSpacing);

    SetAllSensitive();
}

bool DPO_BarometricCompensationPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    pressureInputObjRefUI.FuncObjRefOK(allOK, "P(t) input");
    barometricInputObjRefUI.FuncObjRefOK(allOK, "Barometric input");
    //if (correctionMethodUI.GetChoice() == 0)
    //{
        barometricEfficiencyMSUI.UIdataOK(allOK, 0.01, 1.0);
    //}
    //else
    //{
    //    regressLagUI.FieldValueOK(allOK, "regression lag", 1, 100);
    //    regressMUI.FieldValueOK(allOK, "regression memory", 1, 100);
    //    if (evenTimeSpacingUI.GetValue())
    //        timeSpacingUI.FieldValueOK(allOK, "time spacing", 0.001, 1.0E6);
    //}
    return allOK;
}

void DPO_BarometricCompensationPS::GetMenuData(DPO_BarometricCompensation&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.pressureInputObjRef = pressureInputObjRefUI.GetFuncObjRef();
    outData.barometricInputObjRef = barometricInputObjRefUI.GetFuncObjRef();

    //outData.correctionMethod = DFO_BarometricCompensation::CorrectionMethod(correctionMethodUI.GetChoice());
    //if (correctionMethodUI.GetChoice() == 0)
    //{
        barometricEfficiencyMSUI.GetValue(outData.barometricEfficiencyMS);
    //}
    //else
    //{
    //    outData.regressLag = regressLagUI.GetFieldValue();
    //    outData.regressM = regressMUI.GetFieldValue();
    //    outData.evenTimeSpacing = evenTimeSpacingUI.GetValue();
    //    if (outData.evenTimeSpacing)
    //        outData.timeSpacing = timeSpacingUI.GetFieldValue();
    //}
}

