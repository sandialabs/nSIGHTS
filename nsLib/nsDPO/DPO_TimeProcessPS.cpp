///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TimeProcessPS.cpp
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

#include <genClassPS/PS_UIBase.h>

#include <genApp/ExposedReal.h>
#include "DPO_TimeProcess.h"
#include "DPO_TimeProcessPS.h"

DPO_TimeProcessPS::DPO_TimeProcessPS() :
    inputTPObjRefUI(typeid(DO_XYData)),
    inputTQObjRefUI(typeid(DO_XYData)),
    masterObjRefUI(typeid(DO_TimeProcess),false),
    constantTMSUI(typeid(DO_Real), true)
{
}

void DPO_TimeProcessPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TimeProcessPS*>(inObj)->SetAllSensitive();
}

void DPO_TimeProcessPS::SetAllSensitive()
{
    bool isMaster     = masterObjRefUI.isMaster.GetValue();
    int tChoice  = timeProcessTypeUI.GetChoice();
    if(tChoice == 2) tChoice = 0; //no more Horner Super, but difficult to remove without breaking old config files
    timeProcessTypeUI.SetChoice(tChoice);
    isConstantT  = isMaster && (tChoice < 2);
    isSuperT     = isMaster && (tChoice > 1);

    timeProcessTypeUI.SetSensitive(isMaster);
    constantTMSUI.SetSensitive(isConstantT);
    inputTQObjRefUI.SetSensitive(isSuperT);
    optionsFrame.SetSensitive(isSuperT);
    if (isSuperT)
    {
        if (tChoice == 3)
            addReplaceLastQUI.SetToggleText("add new Final Q ");
        else
            addReplaceLastQUI.SetToggleText("replace Final Q ");

        newLastQUI.SetSensitive(addReplaceLastQUI.GetValue());
    }
    if (isConstantT)
    {
        constantTMSUI.SetAllSensitive();
    }
    else
    {
        static_cast<DPO_TimeProcess*>(MenuObjC::currMenuObj)->exposedConstantTValue.CloseIfCreated();
        constantTMSUI.realValueMS.isExposed.SetValue(false);
    }
}

void DPO_TimeProcessPS::ToggleExposedConstantTValue()
{
    SetAllSensitive();
    static_cast<DPO_TimeProcess*>(MenuObjC::currMenuObj)
            ->exposedConstantTValue.ToggleExposed(constantTMSUI.TurnExposedOff());
}

void DPO_TimeProcessPS::ToggleExposedConstantTValueCB(void* inObj)
{
    static_cast<DPO_TimeProcessPS*>(inObj)->ToggleExposedConstantTValue();
}


void DPO_TimeProcessPS::InitMenu()
{
    MenuBaseC::InitMenu();
    ArgListC args;

    args.AllFormAttach();
    inputTPObjRefUI.InitFramedUI("Input P(t) Data", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    masterObjRefUI.horizontal = true;
    masterObjRefUI.InitFramedUI("Time Processing Specification", PPform.GetWidget(), args);
    masterObjRefUI.isMaster.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(masterObjRefUI);
    static const char* timeProcessLabels[]  = {" Horner  ", " Agarwal  ", " ", " Bourdet Super"};
    //static const char* timeProcessLabels[]  = {" Horner  ", " Agarwal  ", " Horner Super", " Bourdet Super"}; //"removed" 10/29/12 at the request of Randy
    timeProcessTypeUI.PreInitSetUp(timeProcessLabels, 4);
    timeProcessTypeUI.InitFramedUI("Time Processing", PPform.GetWidget(), args);
    timeProcessTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(timeProcessTypeUI);
    constantTMSUI.InitUI("Horner/Agarwal T", args, PPform.GetWidget(), true, false, ToggleExposedConstantTValueCB, this);

    args.Clear();
    args.StdTopObjectAttach(constantTMSUI);
    inputTQObjRefUI.InitFramedUI("Superposition Input Q(t) Data", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(inputTQObjRefUI);
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    addReplaceLastQUI.StdToggleInit("replace Final Q  ", optionsFrame.GetForm(), args);
    addReplaceLastQUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitSetup();
    args.StdSplitFinish(60);
    newLastQUI.SetFieldLen(10);
    newLastQUI.InitLabeledUI("Final Q value", optionsFrame.GetForm(), args);

}

void DPO_TimeProcessPS::MenuOpen(DPO_TimeProcess&   inData)
{
    MenuBaseC::MenuOpen(false, false);

    inputTPObjRefUI.StdMenuInit(inData.inputTPObjRef);
    masterObjRefUI.masterObjectList.StdMenuInit(inData.masterObjRef);
    constantTMSUI.MenuInit(inData.constantTMS);
    inputTQObjRefUI.StdMenuInit(inData.inputTQObjRef);
}

void DPO_TimeProcessPS::SetMenuData(DPO_TimeProcess&    inData)
{
    MenuBaseC::SetMenuData(inData);

    inputTPObjRefUI.SetFuncObjRef(inData.inputTPObjRef);
    masterObjRefUI.SetMasterSlaveSettings(inData.isMaster, inData.masterObjRef);
    int tptype = inData.timeProcessDC.timeProcessType;
    if(tptype == 2) tptype = 0; //Horner Super is no longer an option
    timeProcessTypeUI.SetChoice(tptype);
    inputTQObjRefUI.SetFuncObjRef(inData.inputTQObjRef);

    constantTMSUI.SetValue(inData.constantTMS, inData.exposedConstantTValue);

    addReplaceLastQUI.SetValue(inData.timeProcessDC.addReplaceLastQ);
    newLastQUI.SetFieldValue(inData.timeProcessDC.newLastQ);

    SetAllSensitive();
}

bool DPO_TimeProcessPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    bool isMaster     = masterObjRefUI.isMaster.GetValue();
    isConstantT  = isMaster && (timeProcessTypeUI.GetChoice() < 2);
    isSuperT     = isMaster && (timeProcessTypeUI.GetChoice() > 1);
    inputTPObjRefUI.FuncObjRefOK(allOK, "P(t) input");
    masterObjRefUI.MasterSlaveOK(allOK, "input spec");
    if (isConstantT)
        constantTMSUI.UIdataOK(allOK);
    if (isSuperT)
    {
        inputTQObjRefUI.FuncObjRefOK(allOK, "Q(t) input");
        if (addReplaceLastQUI.GetValue())
            newLastQUI.FieldValueOK(allOK, "Final Q");
    }
    return allOK;
}

void DPO_TimeProcessPS::GetMenuData(DPO_TimeProcess&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTPObjRef = inputTPObjRefUI.GetFuncObjRef();
    masterObjRefUI.GetMasterSlaveSettings(outData.isMaster, outData.masterObjRef);
    if (outData.isMaster)
    {
        int tptype = timeProcessTypeUI.GetChoice();
        if(tptype == 2) tptype = 0;
        timeProcessTypeUI.SetChoice(tptype);
        outData.timeProcessDC.timeProcessType =  TimeProcess::TimeProcessType(tptype);
        if (isConstantT)
            constantTMSUI.GetValue(outData.constantTMS);
        if (isSuperT)
        {
            outData.inputTQObjRef = inputTQObjRefUI.GetFuncObjRef();
            outData.timeProcessDC.addReplaceLastQ   = addReplaceLastQUI.GetValue();
            if (outData.timeProcessDC.addReplaceLastQ)
                outData.timeProcessDC.newLastQ = newLastQUI.GetFieldValue();
        }
    }
}

