///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYRemoveDuplicatePS.cpp
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

#include "DPO_XYRemoveDuplicate.h"
#include "DPO_XYRemoveDuplicatePS.h"

DPO_XYRemoveDuplicatePS::DPO_XYRemoveDuplicatePS()
{
}

void DPO_XYRemoveDuplicatePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_XYRemoveDuplicatePS*>(inObj)->SetAllSensitive();
}

void DPO_XYRemoveDuplicatePS::SetAllSensitive()
{
    bool isRemoveX  = removeDuplicateXValuesUI.GetBoolChoice();
    bool isRemoveY  = removeDuplicateYValuesUI.GetBoolChoice();

    yValueToKeepOpUI.SetSensitive(isRemoveX);
    duplicateXLimitField.SetSensitive(isRemoveX);
    removeOutOfSeqUI.SetSensitive(isRemoveX);

    duplicateYLimitField.SetSensitive(isRemoveY);
}

void DPO_XYRemoveDuplicatePS::InitMenu()
{
    const char* operationStrings[DFO_XYRemoveDuplicate::maxReductionType] = {"Minimum", "Maximum", "Average", "First", "Last"};

    DPO_XYBasePS::InitMenu("XY Remove Duplicate");

    ArgListC args;

    // X frame
    args.Clear();
    args.StdFormAttach();
    removeDuplicateXFrame.InitFramedUI("X Values", PPform.GetWidget(), args);

    // remove duplicate X values (yes/no)
    args.Clear();
    args.StdFormAttach();
    removeDuplicateXValuesUI.PreInitSetUp("   No    ","   Yes   ");
    removeDuplicateXValuesUI.InitLabeledUI("Remove Duplicate X Values", removeDuplicateXFrame.GetForm(), args);
    removeDuplicateXValuesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    // Y values to keep
    args.Clear();
    args.StdTopObjectAttach(removeDuplicateXValuesUI);
    yValueToKeepOpUI.PreInitSetUp(operationStrings, DFO_XYRemoveDuplicate::maxReductionType);
    yValueToKeepOpUI.InitLabeledUI("Data Reduction Operation", removeDuplicateXFrame.GetForm(), args);

    // duplicate X limit
    args.Clear();
    args.StdTopObjectAttach(yValueToKeepOpUI);
    duplicateXLimitField.InitLabeledUI("Duplicate X Limit", removeDuplicateXFrame.GetForm(), args);

    // remove duplicate X values (yes/no)
    args.Clear();
    args.StdTopObjectAttach(duplicateXLimitField);
    removeOutOfSeqUI.PreInitSetUp("   No    ","   Yes   ");
    removeOutOfSeqUI.InitLabeledUI("Remove Out of Sequence X Points", removeDuplicateXFrame.GetForm(), args);

    // Y frame
    args.Clear();
    args.StdTopObjectAttach(removeDuplicateXFrame);
    removeDuplicateYFrame.InitFramedUI("Y Values", PPform.GetWidget(), args);

    // remove duplicate Y values (yes/no)
    args.Clear();
    args.StdFormAttach();
    removeDuplicateYValuesUI.PreInitSetUp("   No    ","   Yes   ");
    removeDuplicateYValuesUI.InitLabeledUI("Remove Duplicate Y Values", removeDuplicateYFrame.GetForm(), args);
    removeDuplicateYValuesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    // duplicate Y limit
    args.Clear();
    args.StdTopObjectAttach(removeDuplicateYValuesUI);
    duplicateYLimitField.InitLabeledUI("Duplicate Y Limit", removeDuplicateYFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(removeDuplicateYFrame);
    SetOutputDescUI(args);

    args.Clear();
    args.StdTopObjectAttach(outputDescUI);
    statusFrame.InitFramedUI("Removal Status", PPform.GetWidget(), args);

    args.StdSplitSetup();
    nIn.InitLabeledUI("# of input", statusFrame.GetForm(), args);

    args.StdSplitFinish();
    nOut.InitLabeledUI("# of output", statusFrame.GetForm(), args);
}

void DPO_XYRemoveDuplicatePS::MenuOpen(DPO_XYRemoveDuplicate& inData)
{
    DPO_XYBasePS::MenuOpen(inData);
}


void DPO_XYRemoveDuplicatePS::SetMenuData(DPO_XYRemoveDuplicate&  inData)
{
    DPO_XYBasePS::SetMenuData(inData, inData);

    // X values
    removeDuplicateXValuesUI.SetBoolChoice(inData.removeDuplicateXValues);
    yValueToKeepOpUI.SetChoice(inData.yValueToKeepOp);
    duplicateXLimitField.SetFieldValue(inData.duplicateXLimit);
    removeOutOfSeqUI.SetBoolChoice(inData.removeOutOfSeq);

    // Y values
    removeDuplicateYValuesUI.SetBoolChoice(inData.removeDuplicateYValues);
    duplicateYLimitField.SetFieldValue(inData.duplicateYLimit);

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

bool DPO_XYRemoveDuplicatePS::UIdataOK()
{
    DPO_XYBasePS::UIdataOK();
    if (removeDuplicateXValuesUI.GetBoolChoice())
        duplicateXLimitField.FieldValueOK(allOK, "Duplicate X Limit");
    if (removeDuplicateYValuesUI.GetBoolChoice())
        duplicateYLimitField.FieldValueOK(allOK, "Duplicate Y Limit");
    return allOK;
}

void DPO_XYRemoveDuplicatePS::GetMenuData(DPO_XYRemoveDuplicate&  outData)
{
    DPO_XYBasePS::GetMenuData(outData, outData);

    outData.removeDuplicateXValues = removeDuplicateXValuesUI.GetBoolChoice();
    if (outData.removeDuplicateXValues)
    {
        outData.yValueToKeepOp = DFO_XYRemoveDuplicate::RemoveDuplicateOp(yValueToKeepOpUI.GetChoice());
        outData.duplicateXLimit = duplicateXLimitField.GetFieldValue();
        outData.removeOutOfSeq = removeOutOfSeqUI.GetBoolChoice();
    }

    outData.removeDuplicateYValues = removeDuplicateYValuesUI.GetBoolChoice();
    if (outData.removeDuplicateYValues)
    {
        outData.duplicateYLimit = duplicateYLimitField.GetFieldValue();
    }
}

