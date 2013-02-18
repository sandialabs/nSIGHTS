///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYArrayScaleTransformPS.cpp
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

#include <objClass/DO_XYDataArray.h>

#include "DPO_XYArrayScaleTransform.h"
#include "DPO_XYArrayScaleTransformPS.h"

void DPO_XYArrayScaleTransformPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_XYArrayScaleTransformPS*>(inObj)->SetAllSensitive();
}

void DPO_XYArrayScaleTransformPS::SetAllSensitive()
{
    thresholdUI.SetInternalSensitive();
}


DPO_XYArrayScaleTransformPS::DPO_XYArrayScaleTransformPS() :
    xyArrayInputObjRefUI(typeid(DO_XYDataArray))
{
}

void DPO_XYArrayScaleTransformPS::InitMenu()
{
    DPO_ScaleTransformPS::InitMenu();

    ArgListC args;

    args.AllFormAttach();
    xyArrayInputObjRefUI.InitFramedUI("XY Array Input Data", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    operateOnXUI.rightOffset = 50;
    operateOnXUI.PreInitSetUp("   X Data   ", "   Y Data   ");
    operateOnXUI.InitFramedUI("Data Component to Operate On", PPform.GetWidget(), args);

    args.StdTopObjectAttach(operateOnXUI);

    InitSTCommon(args);

    args.Clear();
    args.StdTopObjectAttach(nullFrame);
    thresholdUI.StdInit(PPform.GetWidget(), args, true);
    thresholdUI.minThresholdUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
    thresholdUI.maxThresholdUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
}

void DPO_XYArrayScaleTransformPS::SetMenuData(DPO_XYArrayScaleTransform&  inData)
{
    DPO_ScaleTransformPS::SetMenuData(inData);

    xyArrayInputObjRefUI.SetFuncObjRef(inData.xyArrayInputObjRef);
    operateOnXUI.SetBoolChoice(!inData.operateOnX);

    thresholdUI.SetThreshold(inData.thresholdOp);

    SetAllSensitive();
}

void DPO_XYArrayScaleTransformPS::MenuOpen(DPO_XYArrayScaleTransform& inData)
{
    DPO_ScaleTransformPS::MenuOpen(inData);
    xyArrayInputObjRefUI.StdMenuInit(inData.xyArrayInputObjRef);
}


bool DPO_XYArrayScaleTransformPS::UIdataOK()
{
    DPO_ScaleTransformPS::UIdataOK();
    xyArrayInputObjRefUI.FuncObjRefOK(allOK, "XY array data source");
    thresholdUI.UIdataOK(allOK);
    return allOK;
}

void DPO_XYArrayScaleTransformPS::GetMenuData(DPO_XYArrayScaleTransform&  outData)
{
    DPO_ScaleTransformPS::GetMenuData(outData);
    outData.xyArrayInputObjRef = xyArrayInputObjRefUI.GetFuncObjRef();
    outData.operateOnX   = ! operateOnXUI.GetBoolChoice();

    thresholdUI.GetThreshold(outData.thresholdOp);
}


