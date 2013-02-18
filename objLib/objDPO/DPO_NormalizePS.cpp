///////////////////////////////////////////////////////////////////////////////////
//
// DPO_NormalizePS.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClassPS/PS_UIBase.h>
#include <genApp/ExposedReal.h>

#include "DPO_Normalize.h"
#include "DPO_NormalizePS.h"

DPO_NormalizePS::DPO_NormalizePS() :
    masterObjRefUI(typeid(DO_Normalize),false),
    normPowerMSUI(typeid(DO_Real), true)
{
}


void DPO_NormalizePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_NormalizePS*>(inObj)->SetAllSensitive();
}

void DPO_NormalizePS::SetAllSensitive()
{
    bool isMaster = masterObjRefUI.isMaster.GetValue();
    bool limitsOK = isMaster && (normalizeOpUI.GetChoice() != 1);
    bool powerOK = isMaster && (normalizeOpUI.GetChoice() != 0);
    bool inputOK = limitsOK && (!autoLimitUI.GetValue());

    normalizeOpUI.SetSensitive(isMaster);

    limitsFrame.SetSensitive(limitsOK);
    inputMinimumUI.SetSensitive(inputOK);
    inputMaximumUI.SetSensitive(inputOK);

    normPowerMSUI.SetSensitive(powerOK);
    if (!powerOK)
    {
        DPO_Normalize* currObj = static_cast<DPO_Normalize*>(MenuObjC::currMenuObj);
        currObj->exposedNormPower.CloseIfCreated();
        normPowerMSUI.realValueMS.isExposed.SetValue(false);
    }
}

void DPO_NormalizePS::ToggleExposedNormPowerValue()
{
    SetAllSensitive();
    static_cast<DPO_Normalize*>(MenuObjC::currMenuObj)->exposedNormPower.ToggleExposed(normPowerMSUI.TurnExposedOff());
}

void DPO_NormalizePS::ToggleExposedNormPowerValueCB(void* inObj)
{
    static_cast<DPO_NormalizePS*>(inObj)->ToggleExposedNormPowerValue();
}

void DPO_NormalizePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.StdFormAttach();
    masterObjRefUI.horizontal = true;
    masterObjRefUI.InitFramedUI("Normalize Specification", PPform.GetWidget(), args);
    masterObjRefUI.isMaster.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(masterObjRefUI);
    static const char* normalizeOpLabels[]  = {" Limits   ", " Power   ", " Both   "};
    normalizeOpUI.PreInitSetUp(normalizeOpLabels, 3);
    normalizeOpUI.InitFramedUI("Normalize Operation", PPform.GetWidget(), args);
    normalizeOpUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(normalizeOpUI);
    limitsFrame.InitFramedUI("Limit Specification", PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    inputLimitForm.InitWidget(limitsFrame.GetForm(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.LeftPosAttach(5);
    autoLimitUI.StdToggleInit("Auto input", inputLimitForm.GetWidget(), args);
    autoLimitUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitSetup();
    args.StdSplitMiddle(25, 62);
    inputMinimumUI.InitLabeledUI("Input min", inputLimitForm.GetWidget(), args);

    args.StdSplitFinish(38);
    inputMaximumUI.InitLabeledUI("Input max", inputLimitForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(inputLimitForm.GetWidget());
    outputLimitForm.InitWidget(limitsFrame.GetForm(), args);

    args.StdSplitSetup();
    args.StdSplitMiddle(25, 62);
    outputMinimumUI.InitLabeledUI("Output min", outputLimitForm.GetWidget(), args);
    args.StdSplitFinish(38);
    outputMaximumUI.InitLabeledUI("Output max", outputLimitForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(limitsFrame);
    normPowerMSUI.InitUI("Power", args, PPform.GetWidget(), true, false, ToggleExposedNormPowerValueCB, this);
}

void DPO_NormalizePS::MenuOpen(DPO_Normalize& inData)
{
    MenuBaseC::MenuOpen(false, false);
    masterObjRefUI.masterObjectList.StdMenuInit(inData.bpRef.masterObjRef);
    normPowerMSUI.MenuInit(inData.bpRef.normPowerMS);
}

void DPO_NormalizePS::SetMenuData(DPO_Normalize&  inData)
{
    masterObjRefUI.SetMasterSlaveSettings(inData.bpRef.isMaster, inData.bpRef.masterObjRef);

    DC_Normalize& inNorm = inData.bpRef.normalizeDC;

    normalizeOpUI.SetChoice(inNorm.normalizeOp);

    autoLimitUI.SetValue(inNorm.autoLimit);
    inputMinimumUI.SetFieldValue(inNorm.inputMinimum);
    inputMaximumUI.SetFieldValue(inNorm.inputMaximum);
    outputMinimumUI.SetFieldValue(inNorm.outputMinimum);
    outputMaximumUI.SetFieldValue(inNorm.outputMaximum);

    normPowerMSUI.SetValue(inData.bpRef.normPowerMS, inData.exposedNormPower);

    MenuBaseC::SetMenuData(inData);

    SetAllSensitive();
}

bool DPO_NormalizePS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    bool isMaster = masterObjRefUI.isMaster.GetValue();
    masterObjRefUI.MasterSlaveOK(allOK, "input spec");
    if (isMaster)
    {
        int nopChoice = normalizeOpUI.GetChoice();

        if (nopChoice != 1)
        {
            if (!autoLimitUI.GetValue())
            {
                inputMinimumUI.FieldValueOK(allOK, "input min");
                inputMaximumUI.FieldValueOK(allOK, "input max");
            }
            outputMinimumUI.FieldValueOK(allOK, "output min");
            outputMaximumUI.FieldValueOK(allOK, "output max");
        }
        if (nopChoice != 0)
            normPowerMSUI.UIdataOK(allOK);
    }
    return allOK;
}

void DPO_NormalizePS::GetMenuData(DPO_Normalize&  outData)
{
    MenuBaseC::GetMenuData(outData);

    masterObjRefUI.GetMasterSlaveSettings(outData.bpRef.isMaster, outData.bpRef.masterObjRef);
    if (outData.bpRef.isMaster)
    {
        DC_Normalize& outNorm = outData.bpRef.normalizeDC;

        int nopChoice = normalizeOpUI.GetChoice();

        outNorm.normalizeOp = DC_Normalize::NormalizeOp(nopChoice);

        if (nopChoice != 1)
        {
            outNorm.autoLimit = autoLimitUI.GetValue();
            if (!outNorm.autoLimit)
            {
                outNorm.inputMinimum = inputMinimumUI.GetFieldValue();
                outNorm.inputMaximum = inputMaximumUI.GetFieldValue();
            }
            outNorm.outputMinimum = outputMinimumUI.GetFieldValue();
            outNorm.outputMaximum = outputMaximumUI.GetFieldValue();
        }
        if (nopChoice != 0)
            normPowerMSUI.GetValue(outData.bpRef.normPowerMS);
    }

}

