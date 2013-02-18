///////////////////////////////////////////////////////////////////////////////////
//
// DPO_GridExtractXYPS.cpp
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

#include "DPO_GridExtractXY.h"
#include "DPO_GridExtractXYPS.h"

DPO_GridExtractXYPS::DPO_GridExtractXYPS()
    : extractValMSUI(typeid(DO_Real), true)
{
}


void DPO_GridExtractXYPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_GridExtractXYPS*>(inObj)->SetAllSensitive();
}

void DPO_GridExtractXYPS::SetAllSensitive()
{
    extractValMSUI.SetAllSensitive();
}


void DPO_GridExtractXYPS::ToggleExposedExtractValCB(void* inObj)
{
    static_cast<DPO_GridExtractXYPS*>(inObj)->ToggleExposedExtractVal();
}

void DPO_GridExtractXYPS::ToggleExposedExtractVal()
{
    SetAllSensitive();
    // recast current to local
    static_cast<DPO_GridExtractXY*>(MenuObjC::currMenuObj)->
        exposedExtractVal.ToggleExposed(extractValMSUI.TurnExposedOff());
}



void DPO_GridExtractXYPS::InitMenu()
{

    MenuBaseC::InitMenu();

    gridInputUI.StdInit(URform.GetWidget());

    ArgListC args;
    args.Clear();
    args.StdFormAttach();
    extractFrame.InitFramedUI("Extraction Options", PPform.GetWidget(), args);


    args.StdSplitSetup();
    extractXUI.PreInitSetUp("Y Data", "X Data");
    extractXUI.InitLabeledUI("Constant:", extractFrame.GetForm(), args);

    args.ResetPlace();
    args.LeftPosAttach(75);
    logInterpUI.StdToggleInit("Interp on log data", extractFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(extractFrame);
    extractValMSUI.InitUI("Extraction Constant", args, PPform.GetWidget(), true, true, ToggleExposedExtractValCB, this);

}

void DPO_GridExtractXYPS::SetMenuData(DPO_GridExtractXY&  inData)
{
    MenuBaseC::SetMenuData(inData);
    gridInputUI.SetGridInput(inData);
    extractXUI.SetBoolChoice(inData.extractX);
    logInterpUI.SetValue(inData.logInterp);
    extractValMSUI.SetValue(inData.extractValMS, inData.exposedExtractVal);
    SetAllSensitive();
}

void DPO_GridExtractXYPS::MenuOpen(DPO_GridExtractXY& inData)
{
    MenuBaseC::MenuOpen(false, false);
    gridInputUI.MenuOpen(inData);
    extractValMSUI.MenuInit(inData.extractValMS);
}


bool DPO_GridExtractXYPS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    gridInputUI.UIdataOK(allOK);
    extractValMSUI.UIdataOK(allOK);
    return allOK;
}

void DPO_GridExtractXYPS::GetMenuData(DPO_GridExtractXY&  outData)
{
    MenuBaseC::GetMenuData(outData);
    gridInputUI.GetGridInput(outData);
    extractValMSUI.GetValue(outData.extractValMS);
    outData.extractX = extractXUI.GetBoolChoice();
    outData.logInterp = logInterpUI.GetValue();
}

