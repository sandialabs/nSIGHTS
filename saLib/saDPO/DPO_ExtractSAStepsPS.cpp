///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExtractSAStepsPS.cpp
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

#include "SaLibAfx.h"

#include <genClassPS/PS_UIBase.h>

#include "DPO_ExtractSAStepsPS.h"


DPO_ExtractSAStepsPS::DPO_ExtractSAStepsPS() :
    inputTableObjRefUI(typeid(DO_SATable))
{
}

void DPO_ExtractSAStepsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ExtractSAStepsPS*>(inObj)->SetAllSensitive();
}

void DPO_ExtractSAStepsPS::SetAllSensitive()
{
    bool isIndex = extractByIndexesUI.GetBoolChoice();
    indexFrame.SetSensitive(isIndex);
    valueFrame.SetSensitive(!isIndex);
}

void DPO_ExtractSAStepsPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Input SA Table Table", URform.GetWidget(), args);
    inputTableObjRefUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdFormAttach();
    extractByIndexesUI.PreInitSetUp("Step Values", "Step Indexes");
    extractByIndexesUI.InitFramedUI("Step Selection Method", PPform.GetWidget(), args);
    extractByIndexesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(extractByIndexesUI);
    indexFrame.InitFramedUI("Index Extraction", PPform.GetWidget(), args);

    args.StdSplitSetup(30);
    minIndexUI.AttachObjectToLabel();
    minIndexUI.InitLabeledUI("Start index", indexFrame.GetForm(), args);

    args.StdSplitMiddle(35, 65);
    maxIndexUI.AttachObjectToLabel();
    maxIndexUI.InitLabeledUI("End index", indexFrame.GetForm(), args);

    args.StdSplitFinish(30);
    indexIncrementUI.AttachObjectToLabel();
    indexIncrementUI.InitLabeledUI("Increment", indexFrame.GetForm(), args);

    args.StdTopObjectAttach(indexFrame);
    valueFrame.InitFramedUI("Value Extraction", PPform.GetWidget(), args);

    args.StdSplitSetup(45);
    minValueUI.AttachObjectToLabel();
    minValueUI.InitLabeledUI("Minimum value", valueFrame.GetForm(), args);

    args.StdSplitFinish(45);
    maxValueUI.AttachObjectToLabel();
    maxValueUI.InitLabeledUI("Maximum value", valueFrame.GetForm(), args);

    args.StdTopObjectAttach(valueFrame);
    statusFrame.InitFramedUI("Extraction Results", PPform.GetWidget(), args);

    args.StdSplitSetup(45);
    ninSteps.InitLabeledUI("# of input steps", statusFrame.GetForm(), args);

    args.StdSplitFinish(45);
    noutSteps.InitLabeledUI("# of output steps", statusFrame.GetForm(), args);
}

void  DPO_ExtractSAStepsPS::MenuClose()
{
    MenuBaseC::MenuClose();
}

void DPO_ExtractSAStepsPS::MenuOpen(DPO_ExtractSASteps& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
}

void DPO_ExtractSAStepsPS::SetMenuData(DPO_ExtractSASteps&  inData)
{
    MenuBaseC::SetMenuData(inData);
    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);
    extractByIndexesUI.SetBoolChoice(inData.extractByIndexes);

    minIndexUI.SetFieldValue(inData.minIndex);
    maxIndexUI.SetFieldValue(inData.maxIndex);
    indexIncrementUI.SetFieldValue(inData.indexIncrement);

    minValueUI.SetFieldValue(inData.minValue);
    maxValueUI.SetFieldValue(inData.maxValue);

    if (inData.StatusOK())
    {
        ninSteps.SetFieldValue(inData.inputTableDC->GetNStep());
        noutSteps.SetFieldValue(inData.outputTableDC.GetNStep());
    }
    else
    {
        ninSteps.SetStatusNA();
        noutSteps.SetStatusNA();
    }
    statusFrame.SetSensitive(inData.StatusOK());

    SetAllSensitive();
}

bool DPO_ExtractSAStepsPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableObjRefUI.FuncObjRefOK(allOK, "Input table");
    if (extractByIndexesUI.GetBoolChoice())
    {
        minIndexUI.FieldValueOK(allOK, "Start index", 1, 9999);
        maxIndexUI.FieldValueOK(allOK, "End index", 1, 9999);
        indexIncrementUI.FieldValueOK(allOK, "Increment", 1, 9999);
    }
    else
    {
        minValueUI.FieldValueOK(allOK, "Minimum value");
        maxValueUI.FieldValueOK(allOK, "Maximum value");
    }

    return allOK;
}

void DPO_ExtractSAStepsPS::GetMenuData(DPO_ExtractSASteps&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();
    outData.extractByIndexes = extractByIndexesUI.GetBoolChoice();
    if (outData.extractByIndexes)
    {
        outData.minIndex = minIndexUI.GetFieldValue();
        outData.maxIndex = maxIndexUI.GetFieldValue();
        outData.indexIncrement = indexIncrementUI.GetFieldValue();
    }
    else
    {
        outData.minValue = minValueUI.GetFieldValue();
        outData.maxValue = maxValueUI.GetFieldValue();
    }

}

