///////////////////////////////////////////////////////////////////////////////////
//
// DPO_InterpValPS.cpp
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

#include "DPO_InterpVal.h"
#include "DPO_InterpValPS.h"


DPO_InterpValPS::DPO_InterpValPS() :
    interpXValueMSUI(typeid(DO_Real), false)
{
}

void DPO_InterpValPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_InterpValPS*>(inObj)->SetAllSensitive();
}

void DPO_InterpValPS::SetAllSensitive()
{
    logYUI.SetSensitive(interpMethodUI.GetChoice() == 0);
}


void DPO_InterpValPS::InitMenu()
{
    MenuBaseC::InitMenu();
}

void DPO_InterpValPS::InitInterpValCommon(ArgListC& attachments)
{
    interpXValueMSUI.InitUI("Interpolant Value", attachments, PPform.GetWidget(), true, true);

    ArgListC args;

    args.StdTopObjectAttach(interpXValueMSUI);
    const char* interpLabels[] = {"Linear", "Previous", "Next", "Closest"};
    interpMethodUI.PreInitSetUp(interpLabels, 4);
    interpMethodUI.InitFramedUI("Interpolation Method", PPform.GetWidget(), args);
    interpMethodUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(interpMethodUI);
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    logXUI.StdToggleInit("Log X", optionFrame.GetForm(), args);

    args.StdToggleSetup(40);
    logYUI.StdToggleInit("Log Y", optionFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(optionFrame);
    interpValStatus.InitFramedUI("Interpolation Results", PPform.GetWidget(), args);
}

void DPO_InterpValPS::SetMenuData(DPO_InterpVal&  inData)
{
    MenuBaseC::SetMenuData(inData);

    interpXValueMSUI.SetValue(inData.bpRef.interpXValueMS);
    interpMethodUI.SetChoice(inData.bpRef.interpMethod);
    logXUI.SetValue(inData.bpRef.logX);
    logYUI.SetValue(inData.bpRef.logY);

    if (inData.bpRef.StatusOK())
    {
        interpValStatus.SetFieldValue(inData.bpRef.outputRealDO.realVal);
    }
    else
    {
        interpValStatus.SetStatusNA();
    }


    SetAllSensitive();
}

void DPO_InterpValPS::MenuOpen(DPO_InterpVal& inData)
{
    MenuBaseC::MenuOpen(false, false);
    interpXValueMSUI.MenuInit(inData.bpRef.interpXValueMS);
}


bool DPO_InterpValPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    interpXValueMSUI.UIdataOK(allOK);
    return allOK;
}

void DPO_InterpValPS::GetMenuData(DPO_InterpVal&  outData)
{
    MenuBaseC::GetMenuData(outData);

    interpXValueMSUI.GetValue(outData.bpRef.interpXValueMS);
    outData.bpRef.interpMethod = DFO_InterpVal::InterpMethod(interpMethodUI.GetChoice());
    outData.bpRef.logX = logXUI.GetValue();
    outData.bpRef.logY = logYUI.GetValue();
}
