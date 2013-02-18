///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CreateRealPS.cpp
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

#include "DPO_CreateRealPS.h"
#include "DPO_CreateReal.h"

DPO_CreateRealPS::DPO_CreateRealPS() :
    rvalMSUI(typeid(DO_Real), true)
{
}

void DPO_CreateRealPS::SetAllSensitive()
{
    rvalMSUI.SetAllSensitive();
}

void DPO_CreateRealPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_CreateRealPS*>(inObj)->SetAllSensitive();
}


void DPO_CreateRealPS::ToggleExposedRvalValue()
{
    SetAllSensitive();
    static_cast<DPO_CreateReal*>(MenuObjC::currMenuObj)->exposedRvalValue.ToggleExposed(rvalMSUI.TurnExposedOff());
}

void DPO_CreateRealPS::ToggleExposedRvalValueCB(void* inObj)
{
    static_cast<DPO_CreateRealPS*>(inObj)->ToggleExposedRvalValue();
}

void DPO_CreateRealPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.Clear();
    args.StdFormAttach();
    rvalMSUI.InitUI("Real", args, PPform.GetWidget(), true, false, ToggleExposedRvalValueCB, this);

    args.Clear();
    args.StdTopObjectAttach(rvalMSUI);
    realDescUI.SetFieldLen(25);
    realDescUI.InitFramedUI("Output Description", PPform.GetWidget(), args);
}

void DPO_CreateRealPS::MenuOpen(DPO_CreateReal& inData)
{
    MenuBaseC::MenuOpen(true, false);
    rvalMSUI.MenuInit(inData.rvalMS);
}


void DPO_CreateRealPS::SetMenuData(DPO_CreateReal&  inData)
{
    rvalMSUI.SetValue(inData.rvalMS, inData.exposedRvalValue);
    realDescUI.SetFieldString(inData.realDesc);

    MenuBaseC::SetMenuData(inData);

    SetAllSensitive();
}

bool DPO_CreateRealPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    rvalMSUI.UIdataOK(allOK);
    return allOK;
}

void DPO_CreateRealPS::GetMenuData(DPO_CreateReal&  outData)
{
    MenuBaseC::GetMenuData(outData);

    rvalMSUI.GetValue(outData.rvalMS);
    realDescUI.GetFieldString(outData.realDesc, DFO_CreateReal::descMaxLen);
}


void DPO_CreateRealPS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    rvalMSUI.realValueUI.SetFieldValue(0.5);
}

