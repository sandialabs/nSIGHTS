///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExtractSequenceIntervalPS.cpp
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
#include <genClass/DO_XYData.h>
#include <nsClass/DO_SequenceTimes.h>
#include <nsClass/DO_SeqIndx.h>

#include "DPO_ExtractSequenceIntervalPS.h"

DPO_ExtractSequenceIntervalPS::DPO_ExtractSequenceIntervalPS() :
    xyInputDataObjRefUI(typeid(DO_XYData)),
    seqTimesObjRefUI(typeid(DO_SequenceTimes)),
    startSeqMSUI(typeid(DO_SeqIndx), true),
    endSeqMSUI(typeid(DO_SeqIndx), true)
{
}

void DPO_ExtractSequenceIntervalPS::SetAllSensitive()
{
    startSeqMSUI.SetSensitive(listDataOK);

    bool endOK = setStartAndEndUI.GetBoolChoice();
    endSeqMSUI.SetSensitive(listDataOK && endOK);
    if (!endOK)
        endSeqMSUI.indexListUI.SetCurSel(-1);

    startTimeOffsetValueUI.SetSensitive(adjustTimeToStartUI.GetValue());

    fixedPressureOffsetUI.SetSensitive(adjustPressureToStartUI.GetValue());
    startPressureOffsetValueUI.SetSensitive(adjustPressureToStartUI.GetValue());
    finalFlowRateUI.SetSensitive(setFinalFlowRateUI.GetValue());

}
void DPO_ExtractSequenceIntervalPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ExtractSequenceIntervalPS*>(inObj)->SetAllSensitive();
}

void DPO_ExtractSequenceIntervalPS::ToggleExposedStartIndex()
{
    SetAllSensitive();

    // recast current to local
    static_cast<DPO_ExtractSequenceInterval*>(MenuObjC::currMenuObj)->exposedStartSeqIndex.ToggleExposed(startSeqMSUI.TurnExposedOff());
}

void DPO_ExtractSequenceIntervalPS::ToggleExposedStartIndexCB(void* inObj)
{
    static_cast<DPO_ExtractSequenceIntervalPS*>(inObj)->ToggleExposedStartIndex();
}


void DPO_ExtractSequenceIntervalPS::ToggleExposedEndIndex()
{
    if (!setStartAndEndUI.GetBoolChoice())
        endSeqMSUI.indexValueMS.isExposed.SetValue(false);

    SetAllSensitive();

    // recast current to local
    static_cast<DPO_ExtractSequenceInterval*>(MenuObjC::currMenuObj)->exposedEndSeqIndex.ToggleExposed(endSeqMSUI.TurnExposedOff());
}

void DPO_ExtractSequenceIntervalPS::ToggleExposedEndIndexCB(void* inObj)
{
    static_cast<DPO_ExtractSequenceIntervalPS*>(inObj)->ToggleExposedEndIndex();
}


void DPO_ExtractSequenceIntervalPS::SetIndexList()
{
    SC_StringArray dataIDs;
    if (seqTimesObjRefUI.CheckDataObj())
    {
        DO_SequenceTimes* dataDO = static_cast<DO_SequenceTimes*>(seqTimesObjRefUI.GetDataObj());
        dataIDs = dataDO->seqTimes->sequenceIDs;
    }

    listDataOK = !dataIDs.IsEmpty();
    if (listDataOK)
    {
        startSeqMSUI.indexListUI.SetListText(dataIDs);
        endSeqMSUI.indexListUI.SetListText(dataIDs);
    }
    else
    {
        startSeqMSUI.indexListUI.SetListNA();
        endSeqMSUI.indexListUI.SetListNA();
    }
    SetAllSensitive();

}

void DPO_ExtractSequenceIntervalPS::SetIndexListCB(void* inObj)
{
    static_cast<DPO_ExtractSequenceIntervalPS*>(inObj)->SetIndexList();
}

void DPO_ExtractSequenceIntervalPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    xyInputDataObjRefUI.InitFramedUI("XY Data To Extract From", URform.GetWidget(), args);

    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    seqTimesObjRefUI.InitFramedUI("Sequence Time Data", topRowForm.GetWidget(), args);
    seqTimesObjRefUI.AddOtherObjectCallback(SetIndexListCB, this);

    args.StdSplitFinish();
    setStartAndEndUI.PreInitSetUp("Single", "Multiple");
    setStartAndEndUI.InitFramedUI("# of Sequences", topRowForm.GetWidget(), args);
    setStartAndEndUI.AddOtherObjectCallback(ToggleExposedEndIndexCB, this);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    timeAdjustFrame.InitFramedUI("Time Adjustment", PPform.GetWidget(), args);

    // time setup
    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftPosAttach(1);
    adjustTimeToStartUI.StdToggleInit("Set start", timeAdjustFrame.GetForm(), args);
    adjustTimeToStartUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.ResetPlace();
    args.RightPosAttach(48);
    startTimeOffsetValueUI.SetFieldLen(10);
    startTimeOffsetValueUI.InitWidget(timeAdjustFrame.GetForm(), args);

    args.ResetPlace();
    args.LeftPosAttach(75);
    removeDuplicatesUI.StdToggleInit("Remove dups", timeAdjustFrame.GetForm(), args);

    // pressure frame
    args.StdTopObjectAttach(timeAdjustFrame);
    pressureAdjustFrame.InitFramedUI("Pressure/Flow Rate Adjustment", PPform.GetWidget(), args);

    // pressure setup
    args.StdFormAttach();
    pressureTopRowForm.InitWidget(pressureAdjustFrame.GetForm(), args);

    args.Clear();
    args.TopLeftAttach(5, 10);
    adjustPressureToStartUI.StdToggleInit("Adjust start", pressureTopRowForm.GetWidget(), args);
    adjustPressureToStartUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopFormAttach(5);
    args.BottomFormAttach();
    args.LeftPosAttach(30);
    args.RightPosAttach(60);
    fixedPressureOffsetUI.PreInitSetUp("Start", "Offset");
    fixedPressureOffsetUI.InitWidget(pressureTopRowForm.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.RightFormAttach();
    startPressureOffsetValueUI.SetFieldLen(10);
    startPressureOffsetValueUI.InitWidget(pressureTopRowForm.GetWidget(), args);

    args.StdTopObjectAttach(pressureTopRowForm.GetWidget());
    pressureSecondRowForm.InitWidget(pressureAdjustFrame.GetForm(), args);

    args.StdToggleSetup(-10);
    absValuePressureUI.StdToggleInit("ABS(Y)", pressureSecondRowForm.GetWidget(), args);

    args.StdToggleSetup(20);
    log10PressureUI.StdToggleInit("log10(Y)", pressureSecondRowForm.GetWidget(), args);

    args.StdToggleSetup(40);
    setFinalFlowRateUI.StdToggleInit("Set final", pressureSecondRowForm.GetWidget(), args);
    setFinalFlowRateUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.RightFormAttach();
    finalFlowRateUI.SetFieldLen(10);
    finalFlowRateUI.InitWidget(pressureSecondRowForm.GetWidget(), args);

    //  master slave ...
    args.Clear();
    args.StdTopObjectAttach(pressureAdjustFrame);
    args.BottomFormAttach();
    indexMSForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    startSeqMSUI.indexValueMS.horizontal = false;
    startSeqMSUI.InitUI("Start Sequence", args, indexMSForm.GetWidget(), ToggleExposedStartIndexCB, this, true, false);

    args.StdSplitFinish();
    endSeqMSUI.indexValueMS.horizontal = false;
    endSeqMSUI.InitUI("End Sequence", args, indexMSForm.GetWidget(), ToggleExposedEndIndexCB, this, true, false);
}

void DPO_ExtractSequenceIntervalPS::MenuOpen(DPO_ExtractSequenceInterval&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    xyInputDataObjRefUI.StdMenuInit(inData.xyInputDataObjRef);
    seqTimesObjRefUI.StdMenuInit(inData.seqTimesObjRef);
    startSeqMSUI.MenuInit(inData.startSeqMS);
    endSeqMSUI.MenuInit(inData.startSeqMS);
}

void DPO_ExtractSequenceIntervalPS::SetMenuData(DPO_ExtractSequenceInterval&    inData)
{
    xyInputDataObjRefUI.SetFuncObjRef(inData.xyInputDataObjRef);
    seqTimesObjRefUI.SetFuncObjRef(inData.seqTimesObjRef);
    startSeqMSUI.SetValue(inData.startSeqMS, inData.exposedStartSeqIndex);
    endSeqMSUI.SetValue(inData.endSeqMS, inData.exposedEndSeqIndex);
    setStartAndEndUI.SetBoolChoice(inData.setStartAndEnd);

    adjustTimeToStartUI.SetValue(inData.adjustTimeToStart);
    startTimeOffsetValueUI.SetFieldValue(inData.startTimeOffsetValue);

    removeDuplicatesUI.SetValue(inData.removeDuplicates);

    adjustPressureToStartUI.SetValue(inData.adjustPressureToStart);
    fixedPressureOffsetUI.SetBoolChoice(inData.fixedPressureOffset);
    startPressureOffsetValueUI.SetFieldValue(inData.startPressureOffsetValue);
    absValuePressureUI.SetValue(inData.absValuePressure);
    log10PressureUI.SetValue(inData.log10Pressure);

    setFinalFlowRateUI.SetValue(inData.setFinalFlowRate);
    finalFlowRateUI.SetFieldValue(inData.finalFlowRate);

    MenuBaseC::SetMenuData(inData);
    SetIndexList();
}

bool DPO_ExtractSequenceIntervalPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    startSeqMSUI.UIdataOK(allOK);

    if (!setStartAndEndUI.GetBoolChoice())
        endSeqMSUI.UIdataOK(allOK);

    xyInputDataObjRefUI.FuncObjRefOK(allOK, "XY data source");
    seqTimesObjRefUI.FuncObjRefOK(allOK, "Sequence times");

    if (adjustTimeToStartUI.GetValue())
        startTimeOffsetValueUI.FieldValueOK(allOK, "time adjust value");

    if (adjustPressureToStartUI.GetValue())
        startPressureOffsetValueUI.FieldValueOK(allOK, "pressure adjust value");

    if (setFinalFlowRateUI.GetValue())
        finalFlowRateUI.FieldValueOK(allOK, "last value");

    return allOK;
}

void DPO_ExtractSequenceIntervalPS::GetMenuData(DPO_ExtractSequenceInterval&    outData)
{
    MenuBaseC::GetMenuData(outData);
    startSeqMSUI.GetValue(outData.startSeqMS);

    if (setStartAndEndUI.GetBoolChoice())
        endSeqMSUI.GetValue(outData.endSeqMS);

    outData.setStartAndEnd = setStartAndEndUI.GetBoolChoice();

    outData.xyInputDataObjRef = xyInputDataObjRefUI.GetFuncObjRef();
    outData.seqTimesObjRef = seqTimesObjRefUI.GetFuncObjRef();

    outData.adjustTimeToStart = adjustTimeToStartUI.GetValue();
    if (outData.adjustTimeToStart)
        outData.startTimeOffsetValue = startTimeOffsetValueUI.GetFieldValue();

    outData.removeDuplicates = removeDuplicatesUI.GetValue();

    outData.adjustPressureToStart = adjustPressureToStartUI.GetValue();
    if (outData.adjustPressureToStart)
    {
        outData.fixedPressureOffset = fixedPressureOffsetUI.GetBoolChoice();
        outData.startPressureOffsetValue = startPressureOffsetValueUI.GetFieldValue();
    }
    outData.absValuePressure = absValuePressureUI.GetValue();
    outData.log10Pressure = log10PressureUI.GetValue();

    outData.setFinalFlowRate = setFinalFlowRateUI.GetValue();
    if (setFinalFlowRateUI.GetValue())
        outData.finalFlowRate = finalFlowRateUI.GetFieldValue();

}

