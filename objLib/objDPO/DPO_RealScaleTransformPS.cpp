///////////////////////////////////////////////////////////////////////////////////
//
// DPO_RealScaleTransformPS.cpp
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

#include "DPO_RealScaleTransform.h"
#include "DPO_RealScaleTransformPS.h"

void DPO_RealScaleTransformPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_RealScaleTransformPS*>(inObj)->SetAllSensitive();
}

void DPO_RealScaleTransformPS::SetAllSensitive()
{
    minLimitUI.SetSensitive(limitMinUI.GetValue());
    maxLimitUI.SetSensitive(limitMaxUI.GetValue());
    setNullValueUI.SetSensitive(processNullsUI.GetValue());
}


DPO_RealScaleTransformPS::DPO_RealScaleTransformPS() :
    realInputObjRefUI(typeid(DO_Real))
{
}

void DPO_RealScaleTransformPS::InitMenu()
{
    DPO_ScaleTransformPS::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    realInputObjRefUI.InitFramedUI("Real To Scale/Transform", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    InitSTCommon(args);

    args.Clear();
    args.StdTopObjectAttach(nullFrame);
    limitRowForm.InitFormUI(PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    minFrame.InitFramedUI("Minimum Value", limitRowForm.GetLeft(), args);
    maxFrame.InitFramedUI("Maximum Value", limitRowForm.GetRight(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftFormAttach();
    args.LeftOffset(5);
    limitMinUI.StdToggleInit("limit", minFrame.GetForm(), args);
    limitMinUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    limitMaxUI.StdToggleInit("limit", maxFrame.GetForm(), args);
    limitMaxUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.ResetPlace();
    args.LeftPosAttach(50);
    args.RightFormAttach();
    minLimitUI.InitWidget(minFrame.GetForm(), args);
    maxLimitUI.InitWidget(maxFrame.GetForm(), args);


    args.Clear();
    args.StdTopObjectAttach(limitRowForm);
    statusFrame.InitFramedUI("Status", PPform.GetWidget(), args);

    args.StdSplitSetup(45);
    inputVal.InitLabeledUI("Input", statusFrame.GetForm(), args);
    args.StdSplitFinish(45);
    outputVal.InitLabeledUI("Output", statusFrame.GetForm(), args);
}

void DPO_RealScaleTransformPS::SetMenuData(DPO_RealScaleTransform&  inData)
{
    DPO_ScaleTransformPS::SetMenuData(inData);

    realInputObjRefUI.SetFuncObjRef(inData.realInputObjRef);
    limitMinUI.SetValue(inData.limitMin);
    minLimitUI.SetFieldValue(inData.minLimit);

    limitMaxUI.SetValue(inData.limitMax);
    maxLimitUI.SetFieldValue(inData.maxLimit);

    bool isOK = (inData.StatusOK());
    statusFrame.SetSensitive(isOK);
    if (isOK)
    {
        inputVal.SetFieldValue(inData.inputReal);
        outputVal.SetFieldValue(inData.outputRealDO.realVal);
    }
    else
    {
        inputVal.SetStatusNA();
        outputVal.SetStatusNA();
    }

    SetAllSensitive();
}

void DPO_RealScaleTransformPS::MenuOpen(DPO_RealScaleTransform& inData)
{
    DPO_ScaleTransformPS::MenuOpen(inData);
    realInputObjRefUI.StdMenuInit(inData.realInputObjRef);
}


bool DPO_RealScaleTransformPS::UIdataOK()
{
    DPO_ScaleTransformPS::UIdataOK();
    realInputObjRefUI.FuncObjRefOK(allOK, "Input real");
    if (limitMinUI.GetValue())
        minLimitUI.FieldValueOK(allOK, "minimum limit value");
    if (limitMaxUI.GetValue())
        maxLimitUI.FieldValueOK(allOK, "maximum limit value");
    return allOK;
}

void DPO_RealScaleTransformPS::GetMenuData(DPO_RealScaleTransform&  outData)
{
    DPO_ScaleTransformPS::GetMenuData(outData);
    outData.realInputObjRef = realInputObjRefUI.GetFuncObjRef();
    outData.limitMin = limitMinUI.GetValue();
    if (outData.limitMin)
        outData.minLimit = minLimitUI.GetFieldValue();
    outData.limitMax = limitMaxUI.GetValue();
    if (outData.limitMax)
        outData.maxLimit = maxLimitUI.GetFieldValue();
}
void DPO_RealScaleTransformPS::MenuDefault()
{
    DPO_ScaleTransformPS::MenuDefault();
    limitMinUI.SetValue(false);
    minLimitUI.SetFieldValue(0.0);

    limitMaxUI.SetValue(false);
    maxLimitUI.SetFieldValue(1.0);

    processNullsUI.SetValue(false);
    setNullValueUI.SetFieldValue(0.0);
}

