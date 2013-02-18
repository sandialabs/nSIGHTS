///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CDFExtractPS.cpp
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

#include "ObjLibAfx.h"

#include <genClass/DO_XYCDF.h>

#include "DPO_CDFExtractPS.h"
#include "DPO_CDFExtract.h"

DPO_CDFExtractPS::DPO_CDFExtractPS() :
    cdfInputObjRefUI(typeid(DO_XYCDF)),
    rvalMSUI(typeid(DO_Real), true)
{
}

void DPO_CDFExtractPS::SetAllSensitive()
{
    rvalMSUI.SetAllSensitive();
}

void DPO_CDFExtractPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_CDFExtractPS*>(inObj)->SetAllSensitive();
}


void DPO_CDFExtractPS::ToggleExposedRvalValue()
{
    SetAllSensitive();
    static_cast<DPO_CDFExtract*>(MenuObjC::currMenuObj)->exposedRvalValue.ToggleExposed(rvalMSUI.TurnExposedOff());
}

void DPO_CDFExtractPS::ToggleExposedRvalValueCB(void* inObj)
{
    static_cast<DPO_CDFExtractPS*>(inObj)->ToggleExposedRvalValue();
}

void DPO_CDFExtractPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.AllFormAttach();
    cdfInputObjRefUI.InitFramedUI("Input CDF", URform.GetWidget(), args);

    args.StdFormAttach();
    calcProbabilityUI.PreInitSetUp("  Value from Probability  ", "  Probability from Value  ");
    calcProbabilityUI.InitFramedUI("Interpolation Type", PPform.GetWidget(), args);


    args.StdTopObjectAttach(calcProbabilityUI);
    rvalMSUI.InitUI("Value to Interpolate At", args, PPform.GetWidget(), true, true, ToggleExposedRvalValueCB, this);

    args.StdTopObjectAttach(rvalMSUI);
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    useLimitsUI.StdToggleInit("return limits if out-of-range", optionsFrame.GetForm(), args);

    args.StdTopObjectAttach(optionsFrame);
    outputValStatus.SetFieldFormat(SC_SciFormat(6));
    outputValStatus.InitFramedUI("Calculated value", PPform.GetWidget(), args);
}

void DPO_CDFExtractPS::MenuOpen(DPO_CDFExtract& inData)
{
    MenuBaseC::MenuOpen(true, false);
    rvalMSUI.MenuInit(inData.rvalMS);
    cdfInputObjRefUI.StdMenuInit(inData.cdfInputObjRef);

}


void DPO_CDFExtractPS::SetMenuData(DPO_CDFExtract&  inData)
{
    MenuBaseC::SetMenuData(inData);

    cdfInputObjRefUI.SetFuncObjRef(inData.cdfInputObjRef);
    rvalMSUI.SetValue(inData.rvalMS, inData.exposedRvalValue);
    calcProbabilityUI.SetBoolChoice(inData.calcProbability);
    useLimitsUI.SetValue(inData.useLimits);
    if (inData.StatusOK())
        outputValStatus.SetFieldValue(inData.outputValDO.realVal);
    else
        outputValStatus.SetStatusNA();

    SetAllSensitive();
}

bool DPO_CDFExtractPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    rvalMSUI.UIdataOK(allOK);
    cdfInputObjRefUI.FuncObjRefOK(allOK, "Input CDF");
    return allOK;
}

void DPO_CDFExtractPS::GetMenuData(DPO_CDFExtract&  outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.cdfInputObjRef  = cdfInputObjRefUI.GetFuncObjRef();
    rvalMSUI.GetValue(outData.rvalMS);

    outData.calcProbability = calcProbabilityUI.GetBoolChoice();
    outData.useLimits = useLimitsUI.GetValue();
}


void DPO_CDFExtractPS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    rvalMSUI.realValueUI.SetFieldValue(0.5);
}

