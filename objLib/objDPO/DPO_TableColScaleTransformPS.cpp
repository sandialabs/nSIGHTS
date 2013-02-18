///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableColScaleTransformPS.cpp
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

#include <genClass/DO_XYData.h>

#include "DPO_TableColScaleTransform.h"
#include "DPO_TableColScaleTransformPS.h"

void DPO_TableColScaleTransformPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableColScaleTransformPS*>(inObj)->SetAllSensitive();
}

void DPO_TableColScaleTransformPS::SetAllSensitive()
{
    thresholdUI.SetInternalSensitive();
    tableColSelUI.selectedColUI.SetSensitive(colSelUI.GetChoice() != DFO_TableColScaleTransform::col_all);
    lastRow.SetSensitive(colSelUI.GetChoice() == DFO_TableColScaleTransform::col_single);
}


DPO_TableColScaleTransformPS::DPO_TableColScaleTransformPS()
{
}

void DPO_TableColScaleTransformPS::InitMenu()
{
    DPO_ScaleTransformPS::InitMenu();

    ArgListC args;

    args.AllFormAttach();
    tableColSelUI.ObjRefInit(URform.GetWidget(), args);

    args.StdFormAttach();
    firstRow.InitFormUI(PPform.GetWidget(), args);

    args.StdSplitSetup(60);
    static const char* colSelLabels[] = {"Single Column", "All Columns", "Skip Single Column"};
    colSelUI.PreInitSetUp(colSelLabels, 3);
    colSelUI.InitFramedUI("Column Selection", firstRow.GetForm(), args);
    colSelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(40);
    tableColSelUI.SelColInit(firstRow.GetForm(), args);

    args.StdTopObjectAttach(firstRow);

    InitSTCommon(args);

    args.StdTopObjectAttach(nullFrame);
    thresholdUI.StdInit(PPform.GetWidget(), args, true);
    thresholdUI.minThresholdUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
    thresholdUI.maxThresholdUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(thresholdUI);
    lastRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    operateOnExistingUI.PreInitSetUp("New Column", "Selected Column");
    operateOnExistingUI.InitFramedUI("Output Column", lastRow.GetWidget(), args);

    args.StdSplitFinish();
    outputColIDUI.InitFramedUI("S/T Column ID", lastRow.GetWidget(), args);

}

void DPO_TableColScaleTransformPS::SetMenuData(DPO_TableColScaleTransform&  inData)
{
    DPO_ScaleTransformPS::SetMenuData(inData);

    colSelUI.SetChoice(int(inData.columnsSel));
    tableColSelUI.SetTableColSel(inData);
    thresholdUI.SetThreshold(inData.thresholdOp);
    operateOnExistingUI.SetBoolChoice(inData.operateOnExisting);
    outputColIDUI.SetFieldString(inData.outputColID);

    SetAllSensitive();
}

void DPO_TableColScaleTransformPS::MenuOpen(DPO_TableColScaleTransform& inData)
{
    DPO_ScaleTransformPS::MenuOpen(inData);
    tableColSelUI.MenuOpen(inData);
}


bool DPO_TableColScaleTransformPS::UIdataOK()
{
    DPO_ScaleTransformPS::UIdataOK();
    tableColSelUI.UIdataOK(allOK);
    thresholdUI.UIdataOK(allOK);
    return allOK;
}

void DPO_TableColScaleTransformPS::GetMenuData(DPO_TableColScaleTransform&  outData)
{
    DPO_ScaleTransformPS::GetMenuData(outData);
    tableColSelUI.GetTableColSel(outData);
    outData.columnsSel = DFO_TableColScaleTransform::ColumnsSel(colSelUI.GetChoice());
    thresholdUI.GetThreshold(outData.thresholdOp);
    outData.operateOnExisting = operateOnExistingUI.GetBoolChoice();
    outputColIDUI.GetFieldString(outData.outputColID, DPO_TableColScaleTransform::idLen);
}


