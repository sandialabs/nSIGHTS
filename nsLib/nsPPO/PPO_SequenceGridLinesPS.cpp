///////////////////////////////////////////////////////////////////////////////////
//
// PPO_SequenceGridLinesPS.cpp
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
//      File I/O and user interface for all nsPFOGL functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#include "NsLibAfx.h"

#include <nsClass/DO_SequenceTimes.h>

#include "PPO_SequenceGridLinesPS.h"

// ****************************************

PPO_SequenceGridLinesPS::PPO_SequenceGridLinesPS() :
    sequenceTimesObjRefUI(typeid(DO_SequenceTimes)),
    labelFontUI(true)
{
}

void PPO_SequenceGridLinesPS::SetAllSensitive()
{
    labelFormatUI.SetSensitive(true);
    int labType = labelFormatUI.labelTypeUI.GetChoice();
    labelFontUI.SetSensitive(labType  > 0);
    optionsFrame.SetSensitive(labType  > 0);
    timeFormatUI.SetSensitive(labType == 2);
}

void PPO_SequenceGridLinesPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_SequenceGridLinesPS*>(inObj)->SetAllSensitive();
}


void PPO_SequenceGridLinesPS::InitMenu(const PPO_SequenceGridLines& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    sequenceTimesObjRefUI.InitFramedUI("Sequence Times Input", PPform.GetWidget(), args);


    args.StdTopObjectAttach(sequenceTimesObjRefUI);
    gridLineSpecUI.InitFramedUI("Grid Line Format", PPform.GetWidget(), args);

    args.StdTopObjectAttach(gridLineSpecUI);
    labelFormatUI.InitFramedUI("Label Format", PPform.GetWidget(), args);
    labelFormatUI.labelTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(labelFormatUI);
    labelFontUI.InitFramedUI("Label Font", PPform.GetWidget(), args);

    args.StdTopObjectAttach(labelFontUI);
    bottomRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    optionsFrame.InitFramedUI("Options",  bottomRow.GetWidget(), args);

    args.StdToggleSetup(5);
    showEndLabelUI.StdToggleInit("Show END label", optionsFrame.GetForm(), args);

    args.StdSplitFinish();
    timeFormatUI.InitFramedUI("Time Format",  bottomRow.GetWidget(), args);
}


void PPO_SequenceGridLinesPS::MenuOpen(PPO_SequenceGridLines&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    sequenceTimesObjRefUI.StdMenuInit(inData.sequenceTimesObjRef);
    gridLineSpecUI.ResetPens(inData.objPenSet);
}

void PPO_SequenceGridLinesPS::SetMenuData(PPO_SequenceGridLines&    inData)
{
    PPO_BasePS::SetMenuData(inData);

    sequenceTimesObjRefUI.SetFuncObjRef(inData.sequenceTimesObjRef);
    gridLineSpecUI.SetLineSpec(inData.gridLineSpec);

    labelFormatUI.SetGridLineLabelFormat(inData.labelFormat);

    labelFontUI.SetFontSettings(inData.labelFont);

    showEndLabelUI.SetValue(inData.showEndLabel);
    timeFormatUI.SetRealFormatSettings(inData.timeFormat);

    SetAllSensitive();
}

bool PPO_SequenceGridLinesPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    sequenceTimesObjRefUI.FuncObjRefOK(allOK, "Sequence times source");
    labelFormatUI.UIdataOK(allOK);
    return allOK;
}

void PPO_SequenceGridLinesPS::GetMenuData(PPO_SequenceGridLines&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.sequenceTimesObjRef = sequenceTimesObjRefUI.GetFuncObjRef();
    gridLineSpecUI.GetLineSpec(outData.gridLineSpec);
    labelFormatUI.GetGridLineLabelFormat(outData.labelFormat);

    if (outData.labelFormat.labelType != PSC_GridLineLabelFormat::ltNone)
    {
        labelFontUI.GetFontSettings(outData.labelFont);
        outData.showEndLabel = showEndLabelUI.GetValue();
    }

    if (outData.labelFormat.labelType == PSC_GridLineLabelFormat::ltValue)
        timeFormatUI.GetRealFormatSettings(outData.timeFormat);

}


