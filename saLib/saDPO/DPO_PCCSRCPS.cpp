///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PCCSRCPS.cpp
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

#include "DPO_PCCSRCPS.h"


void DPO_PCCSRCPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_PCCSRCPS*>(inObj)->SetAllSensitive();
}

void DPO_PCCSRCPS::SetAllSensitive()
{
}

void DPO_PCCSRCPS::InitMenu()
{
    DPO_SAObjBasePS::InitMenu();

    ArgListC args;

    args.StdTopObjectAttach(varForm.GetWidget());

    topForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    boolFrame.InitFramedUI("Basic Control", topForm.GetWidget(), args);

    args.StdToggleSetup(2);
    doPCCUI.StdToggleInit("PCC", boolFrame.GetForm(), args);

    args.StdToggleSetup(25);
    doSRCUI.StdToggleInit("SRC", boolFrame.GetForm(), args);

    args.StdToggleSetup(50);
    doRANKUI.StdToggleInit("RANK", boolFrame.GetForm(), args);

    args.StdToggleSetup(75);
    verboseOutputUI.StdToggleInit("verbose", boolFrame.GetForm(), args);

    args.StdSplitFinish(35);
    coeffFormatUI.InitFramedUI("Coefficient Listing Format", topForm.GetWidget(), args);

    args.StdTopObjectAttach(topForm.GetWidget());
    optionForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    listCutoffUI.InitFramedUI("PRINT_CUTOFF", optionForm.GetWidget(), args);

    args.StdSplitMiddle(34, 66);
    plotCutoffUI.InitFramedUI("PLOT_CUTOFF", optionForm.GetWidget(), args);

    args.StdSplitFinish(33);
    topINDUI.InitFramedUI("TOP_IND", optionForm.GetWidget(), args);
}

void DPO_PCCSRCPS::MenuOpen(DPO_PCCSRC& inData)
{
    DPO_SAObjBasePS::MenuOpen(inData);
}

void DPO_PCCSRCPS::SetMenuData(DPO_PCCSRC&  inData)
{
    MenuBaseC::SetMenuData(inData);
    DPO_SAObjBasePS::SetMenuData(inData);

    doPCCUI.SetValue(inData.doPCC);
    doSRCUI.SetValue(inData.doSRC);
    doRANKUI.SetValue(inData.doRANK);
    verboseOutputUI.SetValue(inData.verboseOutput);

    coeffFormatUI.SetRealFormatSettings(inData.coeffFormat);

    listCutoffUI.SetFieldValue(inData.listCutoff);
    plotCutoffUI.SetFieldValue(inData.plotCutoff);
    topINDUI.SetFieldValue(inData.topIND);


    SetAllSensitive();
}

bool DPO_PCCSRCPS::UIdataOK()
{
    DPO_SAObjBasePS::UIdataOK();
    listCutoffUI.FieldValueOK(allOK, "PRINT_CUTOFF", 0.0, 1.0);
    plotCutoffUI.FieldValueOK(allOK, "PLOT_CUTOFF", 0.0, 1.0);
    topINDUI.FieldValueOK(allOK, "TOP_IND", -1, 100);

    return allOK;
}

void DPO_PCCSRCPS::GetMenuData(DPO_PCCSRC&  outData)
{
    MenuBaseC::GetMenuData(outData);
    DPO_SAObjBasePS::GetMenuData(outData);

    outData.doPCC = doPCCUI.GetValue();
    outData.doSRC = doSRCUI.GetValue();
    outData.doRANK = doRANKUI.GetValue();
    outData.verboseOutput = verboseOutputUI.GetValue();

    coeffFormatUI.GetRealFormatSettings(outData.coeffFormat);

    outData.listCutoff = listCutoffUI.GetFieldValue();
    outData.plotCutoff = plotCutoffUI.GetFieldValue();
    outData.topIND = topINDUI.GetFieldValue();
}

