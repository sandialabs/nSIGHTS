///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExecuteObjectsPS.cpp
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

#include <string.h>

#include <genClass/U_Str.h>

#include "DPO_ExecuteObjects.h"
#include "DPO_ExecuteObjectsPS.h"


DPO_ExecuteObjectsPS::ObjectDataUIC::ObjectDataUIC()
    : execObjectRefUI(typeid(FuncObjC))
{
}


void DPO_ExecuteObjectsPS::ObjectDataUIC::SetAllSensitive()
{
    executeObjectUI.SetSensitive(true);
    bool executeObject = executeObjectUI.GetValue();
    execObjectRefUI.SetSensitive(executeObject);
    execDownstreamUI.SetSensitive(executeObject);
    stopOnErrorUI.SetSensitive(executeObject);
    statusMustBeOKUI.SetSensitive(executeObject);
}


void DPO_ExecuteObjectsPS::ObjectDataUIC::SetAllSensitiveCB(void* inObj)
{
    static_cast<ObjectDataUIC*>(inObj)->SetAllSensitive();
}


enum {specFormwidth = 800};

void DPO_ExecuteObjectsPS::ObjectDataUIC::InitObjectData()
{

    ArgListC args;

    args.StdToggleSetup(2);
    executeObjectUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    executeObjectUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopFormAttach();
    args.SpecifiedWidthAttach(5, 55, specFormwidth);
    execObjectRefUI.InitWidget(outsideForm.GetWidget(), args);
    execObjectRefUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.SpecifiedWidthAttach(58, 72, specFormwidth);
    const char* detStrs[] = {"None", "Immediate", "At End"};
    execDownstreamUI.PreInitSetUp(detStrs, 3);
    execDownstreamUI.InitWidget(outsideForm.GetWidget(), args);

    args.StdToggleSetup(80);
    stopOnErrorUI.StdToggleInit(" ", outsideForm.GetWidget(), args);

    args.StdToggleSetup(95);
    statusMustBeOKUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
}

void DPO_ExecuteObjectsPS::ObjectDataUIC::SetObjectData(const DFO_ExecuteObjects::ObjectData& objData)
{
    executeObjectUI.SetValue(objData.executeObject);
    execObjectRefUI.SetFuncObjRef(objData.execObjectRef);
    execDownstreamUI.SetChoice(int(objData.execDownstream));
    stopOnErrorUI.SetValue(objData.stopOnError);
    statusMustBeOKUI.SetValue(objData.statusMustBeOK);
    SetAllSensitive();
}

void DPO_ExecuteObjectsPS::ObjectDataUIC::GetObjectData(DFO_ExecuteObjects::ObjectData& objData)
{
    objData.executeObject = executeObjectUI.GetValue();
    objData.execObjectRef = execObjectRefUI.GetFuncObjRef();
    objData.execDownstream = DFO_ExecuteObjects::ObjectData::DownstreamExecType(execDownstreamUI.GetChoice());
    objData.stopOnError = stopOnErrorUI.GetValue();
    objData.statusMustBeOK = statusMustBeOKUI.GetValue();
}

bool DPO_ExecuteObjectsPS::ObjectDataUIC::ObjectDataOK(bool& prevOK)
{
    if (executeObjectUI.GetValue())
    {
        execObjectRefUI.FuncObjRefOK(prevOK, "Object to exec");
    }
    return prevOK;
}


CSize DPO_ExecuteObjectsPS::RealScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 800);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// DPO_ExecuteObjectsPS

DPO_ExecuteObjectsPS::DPO_ExecuteObjectsPS()
: upstreamTriggerObjRefUI(typeid(FuncObjC))
{
}


void DPO_ExecuteObjectsPS::SetAllSensitive()
{
    upstreamTriggerObjRefUI.SetSensitive(useUpstreamTriggerUI.GetValue());
    for (int i = 0; i < DFO_ExecuteObjects::maxExecObject; i++)
        objectDataUI[i].SetAllSensitive();
}

void DPO_ExecuteObjectsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ExecuteObjectsPS*>(inObj)->SetAllSensitive();
}

void DPO_ExecuteObjectsPS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.AllFormAttach();
    triggerFrame.InitFramedUI("Upstream Trigger ?", URform.GetWidget(), args);

    args.StdToggleSetup(2);
    useUpstreamTriggerUI.StdToggleInit(" ", triggerFrame.GetForm(), args);
    useUpstreamTriggerUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(85);
    upstreamTriggerObjRefUI.InitWidget(triggerFrame.GetForm(), args);

    args.AllFormAttach();
    objectFrame.InitFramedUI("Objects to Execute", PPform.GetWidget(), args);

    args.LeftOffset(5);
    args.RightOffset(5);

    //  Radim: if this is not put in the space where the horizontal scroll bar would
    //  go wipes out the top of the attached statusFrame
    args.BottomOffset(25);

    scrollArea.InitWidget(objectFrame.GetForm(), args);

    args.StdFormAttach();
    labelRow.InitWidget(scrollArea, args);

    args.SpecifiedWidthAttach(5, 55, specFormwidth);
    objLabel.StdLabelInit("Object to Execute", labelRow.GetWidget(), args);

    args.SpecifiedWidthAttach(58, 72, specFormwidth);
    downstreamLabel.StdLabelInit("Downstream Exec", labelRow.GetWidget(), args);

    args.StdSplitMiddle(75, 89);
    stopLabel.StdLabelInit("Stop on Error?", labelRow.GetWidget(), args);

    args.StdSplitFinish(10);
    statusOKLabel.StdLabelInit("Must be OK?", labelRow.GetWidget(), args);

    args.StdTopObjectAttach(labelRow.GetWidget());
    args.TopOffset(5);

    for (int i = 0; i < DFO_ExecuteObjects::maxExecObject; i++)
    {
        objectDataUI[i].outsideForm.InitWidget(scrollArea, args);
        objectDataUI[i].InitObjectData();

        args.StdTopObjectAttach(objectDataUI[i].outsideForm.GetWidget());
        objectDataUI[i].hSep.InitWidget(scrollArea, args);

        args.StdTopObjectAttach(objectDataUI[i].hSep.GetWidget());
    }
}


void DPO_ExecuteObjectsPS::MenuOpen(DPO_ExecuteObjects&   inData)
{
    MenuBaseC::MenuOpen(false, true);
    upstreamTriggerObjRefUI.StdMenuInit(inData.upstreamTriggerObjRef);
    for (int i = 0; i < DFO_ExecuteObjects::maxExecObject; i++)
    {
        objectDataUI[i].execObjectRefUI.StdMenuInit(inData.objectData[i].execObjectRef);
    }
}


void DPO_ExecuteObjectsPS::SetMenuData(DPO_ExecuteObjects&    inData)
{
    useUpstreamTriggerUI.SetValue(inData.useUpstreamTrigger);
    upstreamTriggerObjRefUI.SetFuncObjRef(inData.upstreamTriggerObjRef);

    for (int i = 0; i < DFO_ExecuteObjects::maxExecObject; i++)
        objectDataUI[i].SetObjectData(inData.objectData[i]);
    MenuBaseC::SetMenuData(inData);

    SetAllSensitive();
}

bool DPO_ExecuteObjectsPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    if (useUpstreamTriggerUI.GetValue())
        upstreamTriggerObjRefUI.FuncObjRefOK(allOK, "Upstream trigger");

    for (int i = 0; i < DFO_ExecuteObjects::maxExecObject; i++)
        if (!objectDataUI[i].ObjectDataOK(allOK))
            return false;
    return allOK;
}

void DPO_ExecuteObjectsPS::GetMenuData(DPO_ExecuteObjects&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.useUpstreamTrigger = useUpstreamTriggerUI.GetValue();
    if (outData.useUpstreamTrigger)
        outData.upstreamTriggerObjRef = upstreamTriggerObjRefUI.GetFuncObjRef();

    for (int i = 0; i < DFO_ExecuteObjects::maxExecObject; i++)
        objectDataUI[i].GetObjectData(outData.objectData[i]);
}

void DPO_ExecuteObjectsPS::MenuClear()
{
    useUpstreamTriggerUI.SetValue(false);
    for (int i = 0; i < DFO_ExecuteObjects::maxExecObject; i++)
        objectDataUI[i].executeObjectUI.SetValue(false);
    SetAllSensitive();
}

