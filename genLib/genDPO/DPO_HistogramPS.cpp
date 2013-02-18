///////////////////////////////////////////////////////////////////////////////////
//
// DPO_HistogramPS.cpp
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

#include "DPO_Histogram.h"
#include "DPO_HistogramPS.h"


DPO_HistogramPS::DPO_HistogramPS() :
    minValueMSUI(typeid(DO_Real), false),
    maxValueMSUI(typeid(DO_Real), false)
{
}

void DPO_HistogramPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_HistogramPS*>(inObj)->SetAllSensitive();
}

void DPO_HistogramPS::SetAllSensitive()
{
    minValueMSUI.SetSensitive(!autoLimitsUI.GetBoolChoice());
    maxValueMSUI.SetSensitive(!autoLimitsUI.GetBoolChoice());
    numberOfBinsUI.SetSensitive(!(autoBinWidthUI.GetValue() || intBinWidthUI.GetValue()));
}


void DPO_HistogramPS::InitMenu()
{
    MenuBaseC::InitMenu();
}

void DPO_HistogramPS::InitHistogramCommon(ArgListC& attachments)
{
    topRowForm.InitWidget(PPform.GetWidget(), attachments);

    ArgListC args;
    args.StdSplitSetup(33);
    autoLimitsUI.PreInitSetUp("Specified", "Automatic");
    autoLimitsUI.InitFramedUI("Histogram Limits", topRowForm.GetWidget(), args);
    autoLimitsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(33, 66);
    binFrame.InitFramedUI("# of bins", topRowForm.GetWidget(), args);

    args.StdSplitFinish(33);
    outputXisIndexUI.PreInitSetUp("Bin Value", "Bin Index");
    outputXisIndexUI.InitFramedUI("Output X Value", topRowForm.GetWidget(), args);

    args.StdToggleSetup(1);
    autoBinWidthUI.StdToggleInit("auto", binFrame.GetForm(), args);
    autoBinWidthUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(17);
    intBinWidthUI.StdToggleInit("ints", binFrame.GetForm(), args);
    intBinWidthUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(65);
    numberOfBinsUI.InitWidget(binFrame.GetForm(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    minValueMSUI.InitUI("Bin Minimum", args, PPform.GetWidget(), true, true);

    args.StdTopObjectAttach(minValueMSUI);
    maxValueMSUI.InitUI("Bin Maximum", args, PPform.GetWidget(), true, true);

    args.StdTopObjectAttach(maxValueMSUI);
    optionBaseFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    logHistogramUI.StdToggleInit("Log Histogram", optionBaseFrame.GetForm(), args);

    args.StdToggleSetup(40);
    cumulativeUI.StdToggleInit("Cumulative", optionBaseFrame.GetForm(), args);

    args.StdToggleSetup(70);
    normalizeYUI.StdToggleInit("Normalize", optionBaseFrame.GetForm(), args);

    args.StdTopObjectAttach(optionBaseFrame);
    binWidthStatus.InitFramedUI("Calculated Bin Width", PPform.GetWidget(), args);

}

void DPO_HistogramPS::SetMenuData(DPO_Histogram&  inData)
{
    MenuBaseC::SetMenuData(inData);

    autoLimitsUI.SetBoolChoice(inData.bpRef.autoLimits);

    autoBinWidthUI.SetValue(inData.bpRef.autoBinWidth);
    intBinWidthUI.SetValue(inData.bpRef.intBinWidth);

    numberOfBinsUI.SetFieldValue(inData.bpRef.numberOfBins);

    outputXisIndexUI.SetBoolChoice(inData.bpRef.outputXisIndex);

    minValueMSUI.SetValue(inData.bpRef.minValueMS);
    maxValueMSUI.SetValue(inData.bpRef.maxValueMS);

    logHistogramUI.SetValue(inData.bpRef.logHistogram);
    cumulativeUI.SetValue(inData.bpRef.cumulative);
    normalizeYUI.SetValue(inData.bpRef.normalizeY);

    if (inData.bpRef.StatusOK())
        binWidthStatus.SetFieldValue(inData.bpRef.binWidth);
    else
        binWidthStatus.SetStatusNA();

    SetAllSensitive();
}

void DPO_HistogramPS::MenuOpen(DPO_Histogram& inData)
{
    MenuBaseC::MenuOpen(false, false);
    minValueMSUI.MenuInit(inData.bpRef.minValueMS);
    maxValueMSUI.MenuInit(inData.bpRef.maxValueMS);
}


bool DPO_HistogramPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    if (!autoLimitsUI.GetBoolChoice())
        {
            minValueMSUI.UIdataOK(allOK);
            maxValueMSUI.UIdataOK(allOK);
        }

    if (!autoBinWidthUI.GetValue())
        numberOfBinsUI.FieldValueOK(allOK, "# of bins", 5, 1000);

    return allOK;
}

void DPO_HistogramPS::GetMenuData(DPO_Histogram&  outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.bpRef.autoLimits = autoLimitsUI.GetBoolChoice();

    outData.bpRef.autoBinWidth = autoBinWidthUI.GetValue();

    outData.bpRef.intBinWidth = intBinWidthUI.GetValue();

    if (!outData.bpRef.autoBinWidth)
        outData.bpRef.numberOfBins = numberOfBinsUI.GetFieldValue();

    outData.bpRef.outputXisIndex = outputXisIndexUI.GetBoolChoice();

    if (!autoLimitsUI.GetBoolChoice())
        {
            minValueMSUI.GetValue(outData.bpRef.minValueMS);
            maxValueMSUI.GetValue(outData.bpRef.maxValueMS);
        }
    outData.bpRef.logHistogram = logHistogramUI.GetValue();
    outData.bpRef.cumulative = cumulativeUI.GetValue();
    outData.bpRef.normalizeY = normalizeYUI.GetValue();
}

