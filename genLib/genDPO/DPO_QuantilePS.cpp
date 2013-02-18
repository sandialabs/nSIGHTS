///////////////////////////////////////////////////////////////////////////////////
//
// DPO_QuantilePS.cpp
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

#include "DPO_Quantile.h"
#include "DPO_QuantilePS.h"


DPO_QuantilePS::DPO_QuantilePS() :
    quantileMSUI(typeid(DO_Real), false)
{
}

void DPO_QuantilePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_QuantilePS*>(inObj)->SetAllSensitive();
}

void DPO_QuantilePS::SetAllSensitive()
{
    quantileMSUI.SetSensitive(quantileTypeUI.GetChoice() == 3);
}


void DPO_QuantilePS::InitMenu()
{
    MenuBaseC::InitMenu();
}

void DPO_QuantilePS::InitQuantileCommon(ArgListC& attachments)
{
    topRowForm.InitWidget(PPform.GetWidget(), attachments);

    ArgListC args;
    args.StdSplitSetup(65);

    quantileTypeUI.PreInitSetUp(DC_XYCDF::quantileStrs, DC_XYCDF::nquantileType);
    quantileTypeUI.InitFramedUI("Quantile Type", topRowForm.GetWidget(), args);
    quantileTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(35);
    optionFrame.InitFramedUI("Options", topRowForm.GetWidget(), args);

    args.StdToggleSetup(5);
    logDataUI.StdToggleInit("Take log of data", optionFrame.GetForm(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    quantileMSUI.InitUI("Quantile", args, PPform.GetWidget(), true, true);
}

void DPO_QuantilePS::SetMenuData(DPO_Quantile&  inData)
{
    MenuBaseC::SetMenuData(inData);

    quantileTypeUI.SetChoice(inData.bpRef.quantileType);
    logDataUI.SetValue(inData.bpRef.logData);
    quantileMSUI.SetValue(inData.bpRef.quantileMS);
    SetAllSensitive();
}

void DPO_QuantilePS::MenuOpen(DPO_Quantile& inData)
{
    MenuBaseC::MenuOpen(false, false);
    quantileMSUI.MenuInit(inData.bpRef.quantileMS);
}


bool DPO_QuantilePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    if (quantileTypeUI.GetChoice() == 3)
        quantileMSUI.UIdataOK(allOK);

    return allOK;
}

void DPO_QuantilePS::GetMenuData(DPO_Quantile&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.bpRef.quantileType = DC_XYCDF::QuantileType(quantileTypeUI.GetChoice());
    outData.bpRef.logData = logDataUI.GetValue();
    if (outData.bpRef.quantileType == DC_XYCDF::qtGeneric)
        quantileMSUI.GetValue(outData.bpRef.quantileMS);
}

