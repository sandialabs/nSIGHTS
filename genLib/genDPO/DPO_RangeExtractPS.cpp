///////////////////////////////////////////////////////////////////////////////////
//
// DPO_RangeExtractPS.cpp
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
#include <genClass/DO_Real.h>

#include "DPO_RangeExtractPS.h"
#include "DPO_RangeExtract.h"

DPO_RangeExtractPS::DPO_RangeExtractPS() :
    referenceValAMSUI(typeid(DO_Real)),
    referenceValBMSUI(typeid(DO_Real))
{
}

void DPO_RangeExtractPS::SetAllSensitive()
{
    useB = (dataUseUI.GetChoice() < 2);
    referenceValAMSUI.SetAllSensitive();
    referenceValBMSUI.SetSensitive(useB);
    dataOpBUI.SetSensitive(useB);
}

void DPO_RangeExtractPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_RangeExtractPS*>(inObj)->SetAllSensitive();
}


void DPO_RangeExtractPS::ToggleexposedRefA()
{
    SetAllSensitive();
    static_cast<DPO_RangeExtract*>(MenuObjC::currMenuObj)->exposedRefA.ToggleExposed(referenceValAMSUI.TurnExposedOff());
}

void DPO_RangeExtractPS::ToggleexposedRefACB(void* inObj)
{
    static_cast<DPO_RangeExtractPS*>(inObj)->ToggleexposedRefA();
}

void DPO_RangeExtractPS::ToggleexposedRefB()
{
    SetAllSensitive();
    static_cast<DPO_RangeExtract*>(MenuObjC::currMenuObj)->exposedRefB.ToggleExposed((!useB) || referenceValBMSUI.TurnExposedOff());
}

void DPO_RangeExtractPS::ToggleexposedRefBCB(void* inObj)
{
    static_cast<DPO_RangeExtractPS*>(inObj)->ToggleexposedRefB();
}


void DPO_RangeExtractPS::InitMenu()
{
    MenuBaseC::InitMenu();

}

void  DPO_RangeExtractPS::InitRECommon()
{
    ArgListC args;
    args.StdTopObjectAttach(dataUseUI);
    InitRECommon(args);
}

void DPO_RangeExtractPS::InitRECommon(ArgListC& attachments)
{
    referenceValAMSUI.InitUI("A", attachments, PPform.GetWidget(), true, true, ToggleexposedRefACB, this);

    ArgListC args;
    args.Clear();
    args.StdTopObjectAttach(referenceValAMSUI);
    static const char* typeLabels[]  = {" Greater Than   ", " Greater Equal   ", " Less Than   ", " Less Equal   "};
    dataOpAUI.PreInitSetUp(typeLabels, 4);
    dataOpAUI.InitFramedUI("A Operation", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(dataOpAUI);
    referenceValBMSUI.InitUI("B", args, PPform.GetWidget(), true, true, ToggleexposedRefBCB, this);

    args.Clear();
    args.StdTopObjectAttach(referenceValBMSUI);
    dataOpBUI.PreInitSetUp(typeLabels, 4);
    dataOpBUI.InitFramedUI("B Operation", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(dataOpBUI);
    statusFrame.InitFramedUI("Status", PPform.GetWidget(), args);

    args.StdSplitSetup(45);
    nacceptedStatus.SetFieldLen(12);
    nacceptedStatus.InitLabeledUI("Accepted", statusFrame.GetForm(), args);

    args.StdSplitFinish(55);
    nrejectedStatus.SetFieldLen(12);
    nrejectedStatus.InitLabeledUI("Rejected", statusFrame.GetForm(), args);

}


void DPO_RangeExtractPS::InitDataUseUI(ArgListC& attachments, Widget parent)
{
    static const char* opLabels[]  = {"A and B" , "A or B", " A only "};
    attachments.LeftOffset(10);
    dataUseUI.PreInitSetUp(opLabels, 3);
    dataUseUI.InitFramedUI("Operation", parent, attachments);
    dataUseUI.AddOtherObjectCallback(ToggleexposedRefBCB, this);
}

void DPO_RangeExtractPS::MenuOpen(DPO_RangeExtract& inData)
{
    MenuBaseC::MenuOpen(true, false);
    referenceValAMSUI.MenuInit(inData.bpRef.referenceValAMS);
    referenceValBMSUI.MenuInit(inData.bpRef.referenceValBMS);
}

void DPO_RangeExtractPS::SetMenuData(DPO_RangeExtract&  inData)
{
    MenuBaseC::SetMenuData(inData);

    dataUseUI.SetChoice(int(inData.bpRef.dataUse));
    referenceValAMSUI.SetValue(inData.bpRef.referenceValAMS, inData.exposedRefA);
    referenceValBMSUI.SetValue(inData.bpRef.referenceValBMS, inData.exposedRefB);

    dataOpAUI.SetChoice(int(inData.bpRef.dataOpA));
    dataOpBUI.SetChoice(int(inData.bpRef.dataOpB));

    if (inData.bpRef.StatusOK())
        {
            nacceptedStatus.SetFieldValue(inData.bpRef.naccepted);
            nrejectedStatus.SetFieldValue(inData.bpRef.nrejected);
        }
    else
        {
            nacceptedStatus.SetStatusNA();
            nrejectedStatus.SetStatusNA();
        }


    SetAllSensitive();
}

bool DPO_RangeExtractPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    referenceValAMSUI.UIdataOK(allOK);
    referenceValBMSUI.UIdataOK(allOK);
    return allOK;
}

void DPO_RangeExtractPS::GetMenuData(DPO_RangeExtract&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.bpRef.dataUse  = DFO_RangeExtract::DataUseType(dataUseUI.GetChoice());
    outData.bpRef.dataOpA  = DFO_RangeExtract::OpType(dataOpAUI.GetChoice());
    referenceValAMSUI.GetValue(outData.bpRef.referenceValAMS);
    if (outData.bpRef.dataUse != DFO_RangeExtract::dut_Aonly)
        {
            outData.bpRef.dataOpB       = DFO_RangeExtract::OpType(dataOpBUI.GetChoice());
            referenceValBMSUI.GetValue(outData.bpRef.referenceValBMS);
        }
}


void DPO_RangeExtractPS::MenuDefault()
{
    MenuBaseC::MenuDefault();
    dataUseUI.SetChoice(0);
    referenceValAMSUI.realValueUI.SetFieldValue(0.0);
    referenceValBMSUI.realValueUI.SetFieldValue(1.0);
}

