///////////////////////////////////////////////////////////////////////////////////
//
// DPO_GridExtractIsoPointPS.cpp
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

#include "DPO_GridExtractIsoPoint.h"
#include "DPO_GridExtractIsoPointPS.h"

DPO_GridExtractIsoPointPS::DPO_GridExtractIsoPointPS()
    : isoValMSUI(typeid(DO_Real), true)
{
}


void DPO_GridExtractIsoPointPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_GridExtractIsoPointPS*>(inObj)->SetAllSensitive();
}

void DPO_GridExtractIsoPointPS::SetAllSensitive()
{
    isoValMSUI.SetAllSensitive();
}


void DPO_GridExtractIsoPointPS::ToggleexposedIsoValCB(void* inObj)
{
    static_cast<DPO_GridExtractIsoPointPS*>(inObj)->ToggleexposedIsoVal();
}

void DPO_GridExtractIsoPointPS::ToggleexposedIsoVal()
{
    SetAllSensitive();
    // recast current to local
    static_cast<DPO_GridExtractIsoPoint*>(MenuObjC::currMenuObj)->
        exposedIsoVal.ToggleExposed(isoValMSUI.TurnExposedOff());
}



void DPO_GridExtractIsoPointPS::InitMenu()
{
    MenuBaseC::InitMenu();

    gridInputUI.StdInit(URform.GetWidget());

    ArgListC args;
    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(35);
    isoOnXLinesUI.PreInitSetUp("Along Y", "Along X");
    isoOnXLinesUI.InitFramedUI("Direction", topRow.GetWidget(), args);

    args.StdSplitMiddle(35, 70);
    findIsoMaxUI.PreInitSetUp("Minimum", "Maximum");
    findIsoMaxUI.InitFramedUI("Operation", topRow.GetWidget(), args);

    args.StdSplitFinish(30);
    optionFrame.InitFramedUI("Options", topRow.GetWidget(), args);

    args.StdToggleSetup(5);
    logInterpUI.StdToggleInit("Interp on log data", optionFrame.GetForm(), args);

    args.StdTopObjectAttach(topRow.GetWidget());
    isoValMSUI.InitUI("Iso Line Constant", args, PPform.GetWidget(), true, true, ToggleexposedIsoValCB, this);

    args.StdTopObjectAttach(isoValMSUI);
    statusFrame.InitFramedUI("Status", PPform.GetWidget(), args);

    args.StdSplitSetup(45);
    outputXVal.InitLabeledUI("Output X", statusFrame.GetForm(), args);
    args.StdSplitFinish(45);
    outputYVal.InitLabeledUI("Output Y", statusFrame.GetForm(), args);
}

void DPO_GridExtractIsoPointPS::SetMenuData(DPO_GridExtractIsoPoint&  inData)
{
    MenuBaseC::SetMenuData(inData);
    gridInputUI.SetGridInput(inData);
    isoOnXLinesUI.SetBoolChoice(inData.isoOnXLines);
    findIsoMaxUI.SetBoolChoice(inData.findIsoMax);
    logInterpUI.SetValue(inData.logInterp);
    isoValMSUI.SetValue(inData.isoValMS, inData.exposedIsoVal);

    bool isOK = (inData.StatusOK());
    statusFrame.SetSensitive(isOK);
    if (isOK)
    {
        outputXVal.SetFieldValue(inData.outputXDO.realVal);
        outputYVal.SetFieldValue(inData.outputYDO.realVal);
    }
    else
    {
        outputXVal.SetStatusNA();
        outputYVal.SetStatusNA();
    }

    SetAllSensitive();
}
void DPO_GridExtractIsoPointPS::MenuOpen(DPO_GridExtractIsoPoint& inData)
{
    MenuBaseC::MenuOpen(false, false);
    gridInputUI.MenuOpen(inData);
    isoValMSUI.MenuInit(inData.isoValMS);
}


bool DPO_GridExtractIsoPointPS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    gridInputUI.UIdataOK(allOK);
    isoValMSUI.UIdataOK(allOK);
    return allOK;
}

void DPO_GridExtractIsoPointPS::GetMenuData(DPO_GridExtractIsoPoint&  outData)
{
    MenuBaseC::GetMenuData(outData);
    gridInputUI.GetGridInput(outData);
    isoValMSUI.GetValue(outData.isoValMS);
    outData.isoOnXLines = isoOnXLinesUI.GetBoolChoice();
    outData.findIsoMax = findIsoMaxUI.GetBoolChoice();
    outData.logInterp = logInterpUI.GetValue();
}

