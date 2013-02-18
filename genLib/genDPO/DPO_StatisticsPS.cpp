///////////////////////////////////////////////////////////////////////////////////
//
// DPO_StatisticsPS.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              User interface for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include "DPO_Statistics.h"
#include "DPO_StatisticsPS.h"

DPO_StatisticsPS::DPO_StatisticsPS()
{
}

void DPO_StatisticsPS::InitMenu()
{
    MenuBaseC::InitMenu();
}

void DPO_StatisticsPS::SetAllSensitive()
{
    useLog10UI.SetSensitive(calcLogDataUI.GetValue());
    presentTransformedUI.SetSensitive(calcLogDataUI.GetValue());
    if (resultsOK)
    {
        resultStatus[SC_Statistics::soMedian].SetSensitive(calcModeAndMedianUI.GetValue());
        resultStatus[SC_Statistics::soMode].SetSensitive(calcModeAndMedianUI.GetValue());
    }
}

void DPO_StatisticsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_StatisticsPS*>(inObj)->SetAllSensitive();
}


void DPO_StatisticsPS::InitStatsCommon(ArgListC& attachments)
{
    selectionFrame.InitFramedUI("Data To Output", PPform.GetWidget(), attachments);

    ArgListC args;


    // options
    args.StdTopObjectAttach(selectionFrame);
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    calcLogDataUI.StdToggleInit("log Data", optionFrame.GetForm(), args);
    calcLogDataUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(20, 40);
    args.TopOffset(10);
    useLog10UI.PreInitSetUp("natural log", "log base 10");
    useLog10UI.InitWidget(optionFrame.GetForm(), args);

    args.StdToggleSetup(45);
    presentTransformedUI.StdToggleInit("results are log", optionFrame.GetForm(), args);

    args.StdToggleSetup(70);
    calcModeAndMedianUI.StdToggleInit("calc median and mode", optionFrame.GetForm(), args);
    calcModeAndMedianUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    // results
    args.StdTopObjectAttach(optionFrame);
    statusFrame.InitFramedUI("Result Values", PPform.GetWidget(), args);

    // contents
    args.StdFormAttach();
    args.LeftOffset(5);
    args.RightOffset(15);

    value1OpUI.PreInitSetUp(SC_Statistics::opIDs, SC_Statistics::maxNormStatisticType);
    value1OpUI.InitLabeledUI("Output #1", selectionFrame.GetLeft(), args);

    value2OpUI.PreInitSetUp(SC_Statistics::opIDs, SC_Statistics::maxNormStatisticType);
    value2OpUI.InitLabeledUI("Output #2", selectionFrame.GetRight(), args);

    args.StdTopObjectAttach(value1OpUI);
    args.LeftOffset(5);
    args.RightOffset(15);
    value3OpUI.PreInitSetUp(SC_Statistics::opIDs, SC_Statistics::maxNormStatisticType);
    value3OpUI.InitLabeledUI("Output #3", selectionFrame.GetLeft(), args);

    args.StdTopObjectAttach(value2OpUI);
    args.LeftOffset(5);
    args.RightOffset(15);
    value4OpUI.PreInitSetUp(SC_Statistics::opIDs, SC_Statistics::maxNormStatisticType);
    value4OpUI.InitLabeledUI("Output #4", selectionFrame.GetRight(), args);

    args.StdFormAttach(5);
    args.LeftOffset(5);
    args.RightOffset(15);
    for (int i = 0; i < 6; i++)
    {
        if (SC_Statistics::IsInteger(i))
            resultStatus[i].SetFieldFormat(SC_DecFormat(0));

        resultStatus[i].InitLabeledUI(SC_Statistics::opIDs[i], statusFrame.GetLeft(), args);
        args.StdTopObjectAttach(resultStatus[i]);
        args.LeftOffset(5);
        args.RightOffset(15);
    }

    args.StdFormAttach(5);
    args.LeftOffset(5);
    args.RightOffset(15);

    for (int i = 6; i < SC_Statistics::maxNormStatisticType; i++)
    {
        if (SC_Statistics::IsInteger(i))
            resultStatus[i].SetFieldFormat(SC_DecFormat(0));

        resultStatus[i].InitLabeledUI(SC_Statistics::opIDs[i], statusFrame.GetRight(), args);
        args.StdTopObjectAttach(resultStatus[i]);
        args.LeftOffset(5);
        args.RightOffset(15);
    }

}

void DPO_StatisticsPS::SetMenuData(DPO_Statistics&  inData)
{
    MenuBaseC::SetMenuData(inData);

    useLog10UI.SetBoolChoice(inData.bpRef.useLog10);
    calcLogDataUI.SetValue(inData.bpRef.calcLogData);
    calcModeAndMedianUI.SetValue(inData.bpRef.calcModeAndMedian);
    presentTransformedUI.SetValue(inData.bpRef.presentTransformed);

    value1OpUI.SetChoice(int(inData.bpRef.value1Op));
    value2OpUI.SetChoice(int(inData.bpRef.value2Op));
    value3OpUI.SetChoice(int(inData.bpRef.value3Op));
    value4OpUI.SetChoice(int(inData.bpRef.value4Op));

    resultsOK = (inData.bpRef.StatusOK());
    statusFrame.SetSensitive(resultsOK);
    for (int i = 0; i < SC_Statistics::maxNormStatisticType; i++)
    {
        bool statOK = resultsOK;
        if ((i == SC_Statistics::soMedian) || (i == SC_Statistics::soMode))
            statOK = resultsOK && inData.bpRef.calcModeAndMedian;

        if (statOK)
            resultStatus[i].SetFieldValue(inData.bpRef.realResults[i]);
        else
            resultStatus[i].SetStatusNA();
    }

    SetAllSensitive();
}

void DPO_StatisticsPS::MenuOpen(DPO_Statistics& inData)
{
    MenuBaseC::MenuOpen(true, false);
}


bool DPO_StatisticsPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    return allOK;
}

void DPO_StatisticsPS::GetMenuData(DPO_Statistics&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.bpRef.value1Op  = SC_Statistics::StatisticOp(value1OpUI.GetChoice());
    outData.bpRef.value2Op  = SC_Statistics::StatisticOp(value2OpUI.GetChoice());
    outData.bpRef.value3Op  = SC_Statistics::StatisticOp(value3OpUI.GetChoice());
    outData.bpRef.value4Op  = SC_Statistics::StatisticOp(value4OpUI.GetChoice());

    outData.bpRef.useLog10 = useLog10UI.GetBoolChoice();
    outData.bpRef.presentTransformed = presentTransformedUI.GetValue();

    outData.bpRef.calcLogData = calcLogDataUI.GetValue();
    outData.bpRef.calcModeAndMedian = calcModeAndMedianUI.GetValue();
}

void DPO_StatisticsPS::MenuDefault()
{
    MenuBaseC::MenuDefault();
    value1OpUI.SetChoice(0);
    value2OpUI.SetChoice(1);
    value3OpUI.SetChoice(2);
    value4OpUI.SetChoice(3);
}

