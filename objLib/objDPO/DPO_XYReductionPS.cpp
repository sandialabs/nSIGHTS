///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYReductionPS.cpp
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

#include "DPO_XYReduction.h"
#include "DPO_XYReductionPS.h"

DPO_XYReductionPS::DPO_XYReductionPS()
{
}

void DPO_XYReductionPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_XYReductionPS*>(inObj)->SetAllSensitive();
}

void DPO_XYReductionPS::SetAllSensitive()
{
    bool skipOK = (dataReductionOperationUI.GetChoice() != DFO_XYReduction::reMax);
    bool changeOK = (dataReductionOperationUI.GetChoice() != DFO_XYReduction::reSkip);
    pointSkipIntervalField.SetSensitive(skipOK);
    xChangeForm.SetSensitive(changeOK);
    yChangeForm.SetSensitive(changeOK);
/*
    maxXChangeField.SetSensitive(changeOK);
    logXChangeUI.SetSensitive(changeOK);
    maxYChangeField.SetSensitive(changeOK);
    logYChangeUI.SetSensitive(changeOK);
*/
}

void DPO_XYReductionPS::InitMenu()
{
    DPO_XYBasePS::InitMenu("XY Reduction");

    ArgListC args;

    args.Clear();
    args.StdFormAttach();
    reductionFrame.InitFramedUI("Operation", PPform.GetWidget(), args);

    //  operation
    args.Clear();
    args.StdFormAttach();

    static const char* operationStrings[] = {"Skip", "Maximum Change", "Both"};
    dataReductionOperationUI.PreInitSetUp(operationStrings, 3);
    dataReductionOperationUI.InitLabeledUI("Data Reduction Operation", reductionFrame.GetForm(), args);
    dataReductionOperationUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    // point skip interval
    args.Clear();
    args.StdTopObjectAttach(dataReductionOperationUI);
    args.TopOffset(10);
    pointSkipIntervalField.InitLabeledUI("Point Skip Interval:", reductionFrame.GetForm(), args);

    // xchange
    args.Clear();
    args.StdTopObjectAttach(pointSkipIntervalField);
    xChangeForm.InitWidget(reductionFrame.GetForm(), args);

    args.StdToggleSetup(5);
    logXChangeUI.StdToggleInit("log X change", xChangeForm.GetWidget(), args);

    args.StdSplitSetup();
    args.StdSplitFinish(60);
    maxXChangeField.InitLabeledUI("Maximum X Change:", xChangeForm.GetWidget(), args);

    // ychange
    args.Clear();
    args.StdTopObjectAttach(xChangeForm.GetWidget());
    yChangeForm.InitWidget(reductionFrame.GetForm(), args);

    args.StdToggleSetup(5);
    logYChangeUI.StdToggleInit("log Y change", yChangeForm.GetWidget(), args);

    args.StdSplitSetup();
    args.StdSplitFinish(60);
    maxYChangeField.InitLabeledUI("Maximum Y Change:", yChangeForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(reductionFrame);
    SetOutputDescUI(args);

    args.Clear();
    args.StdTopObjectAttach(outputDescUI);
    statusFrame.InitFramedUI("Reduction Status", PPform.GetWidget(), args);

    args.StdSplitSetup();
    nIn.InitLabeledUI("# of input", statusFrame.GetForm(), args);

    args.StdSplitFinish();
    nOut.InitLabeledUI("# of output", statusFrame.GetForm(), args);

}

void DPO_XYReductionPS::MenuOpen(DPO_XYReduction& inData)
{
    DPO_XYBasePS::MenuOpen(inData);
}


void DPO_XYReductionPS::SetMenuData(DPO_XYReduction&  inData)
{
    DPO_XYBasePS::SetMenuData(inData, inData);

    dataReductionOperationUI.SetChoice(inData.dataReductionOperation);
    pointSkipIntervalField.SetFieldValue(inData.pointSkipInterval);
    maxXChangeField.SetFieldValue(inData.maxXChange);
    logXChangeUI.SetValue(inData.logXChange);
    maxYChangeField.SetFieldValue(inData.maxYChange);
    logYChangeUI.SetValue(inData.logYChange);

    if (inData.StatusOK())
    {
        nIn.SetFieldValue(inData.inputXY->Size());
        nOut.SetFieldValue(inData.outputXY.Size());
    }
    else
    {
        nIn.SetStatusNA();
        nOut.SetStatusNA();
    }

    SetAllSensitive();
}

bool DPO_XYReductionPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    bool skipOK = (dataReductionOperationUI.GetChoice() != DFO_XYReduction::reMax);
    bool changeOK = (dataReductionOperationUI.GetChoice() != DFO_XYReduction::reSkip);
    if (skipOK)
    {
        pointSkipIntervalField.FieldValueOK(allOK, "Point Skip Interval", 0, 50);
    }
    if (changeOK)
    {
        maxXChangeField.FieldValueOK(allOK, "Maximum X Change");
        maxYChangeField.FieldValueOK(allOK, "Maximum Y Change");
    }
    return allOK;
}

void DPO_XYReductionPS::GetMenuData(DPO_XYReduction&  outData)
{
    DPO_XYBasePS::GetMenuData(outData, outData);

    outData.dataReductionOperation = DFO_XYReduction::ReductionOp(dataReductionOperationUI.GetChoice());
    if (outData.dataReductionOperation != DFO_XYReduction::reMax)
        outData.pointSkipInterval = pointSkipIntervalField.GetFieldValue();
    if (outData.dataReductionOperation != DFO_XYReduction::reSkip)
    {
        outData.maxXChange = maxXChangeField.GetFieldValue();
        outData.logXChange = logXChangeUI.GetValue();

        outData.maxYChange = maxYChangeField.GetFieldValue();
        outData.logYChange = logYChangeUI.GetValue();
    }
}

