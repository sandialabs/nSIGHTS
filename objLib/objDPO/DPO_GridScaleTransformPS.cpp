///////////////////////////////////////////////////////////////////////////////////
//
// DPO_GridScaleTransformPS.cpp
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


#include "DPO_GridScaleTransform.h"
#include "DPO_GridScaleTransformPS.h"

void DPO_GridScaleTransformPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_GridScaleTransformPS*>(inObj)->SetAllSensitive();
}

void DPO_GridScaleTransformPS::SetAllSensitive()
{
    thresholdUI.SetInternalSensitive();
    averageKernelSizeUI.SetSensitive(averageGridUI.GetValue());
    replaceNullsWithAverageUI.SetSensitive(averageGridUI.GetValue());
}


DPO_GridScaleTransformPS::DPO_GridScaleTransformPS()
{
}

void DPO_GridScaleTransformPS::InitMenu()
{
    DPO_ScaleTransformPS::InitMenu();

    gridInputUI.StdInit(URform.GetWidget());

    ArgListC args;
    args.Clear();
    args.StdFormAttach();

    InitSTCommon(args);

    args.StdTopObjectAttach(nullFrame);
    thresholdUI.StdInit(PPform.GetWidget(), args, true);
    thresholdUI.minThresholdUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
    thresholdUI.maxThresholdUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(thresholdUI);
    averageFrame.InitFramedUI("Average/Smoothing", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    averageGridUI.StdToggleInit("Average", averageFrame.GetForm(), args);
    averageGridUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(30);
    replaceNullsWithAverageUI.StdToggleInit("null replacement only", averageFrame.GetForm(), args);

    args.StdSplitFinish(40);
    averageKernelSizeUI.SetFieldLen(8);
    averageKernelSizeUI.AttachLabelToObject();
    averageKernelSizeUI.InitLabeledUI("Kernel Size", averageFrame.GetForm(), args);

    args.StdTopObjectAttach(averageFrame);
    outputTypeDescUI.SetFieldLen(25);
    outputTypeDescUI.InitFramedUI("Output Description", PPform.GetWidget(), args);
}

void DPO_GridScaleTransformPS::SetMenuData(DPO_GridScaleTransform&  inData)
{
    DPO_ScaleTransformPS::SetMenuData(inData);

    gridInputUI.SetGridInput(inData);

    averageGridUI.SetValue(inData.averageGrid);
    replaceNullsWithAverageUI.SetValue(inData.replaceNullsWithAverage);
    averageKernelSizeUI.SetFieldValue(inData.averageKernelSize);

    outputTypeDescUI.SetFieldString(inData.outputTypeDesc);
    thresholdUI.SetThreshold(inData.thresholdOp);

    SetAllSensitive();
}

void DPO_GridScaleTransformPS::MenuOpen(DPO_GridScaleTransform& inData)
{
    DPO_ScaleTransformPS::MenuOpen(inData);
    gridInputUI.MenuOpen(inData);
}


bool DPO_GridScaleTransformPS::UIdataOK()
{
    DPO_ScaleTransformPS::UIdataOK();
    gridInputUI.UIdataOK(allOK);
    thresholdUI.UIdataOK(allOK);
    if (averageGridUI.GetValue())
        averageKernelSizeUI.FieldValueOK(allOK, "kernel size", 1, 10);

    return allOK;
}

void DPO_GridScaleTransformPS::GetMenuData(DPO_GridScaleTransform&  outData)
{
    DPO_ScaleTransformPS::GetMenuData(outData);
    gridInputUI.GetGridInput(outData);
    outputTypeDescUI.GetFieldString(outData.outputTypeDesc, stdMsgStrLen);
    thresholdUI.GetThreshold(outData.thresholdOp);
    outData.averageGrid = averageGridUI.GetValue();
    if (outData.averageGrid)
    {
        outData.replaceNullsWithAverage = replaceNullsWithAverageUI.GetValue();
        outData.averageKernelSize = averageKernelSizeUI.GetFieldValue();
    }
}


