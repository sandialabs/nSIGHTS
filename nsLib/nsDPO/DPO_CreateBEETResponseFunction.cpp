///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CreateBEETResponseFunction.cpp
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

#include "nsLibAfx.h"

#include <genApp/AppConfigFile.h>
#include <genApp/ExposedReal.h>

#ifndef ADCONSOLEAPP
#include "DPO_CreateBEETResponseFunctionPS.h"
#endif

#include "DPO_CreateBEETResponseFunction.h"

const char* DPO_CreateBEETResponseFunction::objLongDesc = "Create BP/ET Response Function";

DPO_CreateBEETResponseFunction :: DPO_CreateBEETResponseFunction() :
    MenuObjC(this, objLongDesc),
    exposedMaxTimeDelayValue(*this, "MaxTimeDelay", maxTimeDelayMS.enteredRealValue),
    exposedTimeSpacingValue(*this, "TimeSpacing", specTimeSpacingMS.enteredRealValue)
{
}

DPO_CreateBEETResponseFunction :: DPO_CreateBEETResponseFunction(const DPO_CreateBEETResponseFunction& a) :
    DFO_CreateBEETResponseFunction(a),
    MenuObjC(this, objLongDesc),
    exposedMaxTimeDelayValue(*this, "MaxTimeDelay", maxTimeDelayMS.enteredRealValue),
    exposedTimeSpacingValue(*this, "TimeSpacing", specTimeSpacingMS.enteredRealValue)
{
}


DPO_CreateBEETResponseFunction& DPO_CreateBEETResponseFunction::operator= (const DPO_CreateBEETResponseFunction& a)
{
    if (&a != this)
       *((DFO_CreateBEETResponseFunction*) this) = a;
    return *this;
}


bool  DPO_CreateBEETResponseFunction::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(pressureInputObjRef);
    ReadFuncObjRef(barometricInputObjRef);
    ReadFuncObjRef(earthTideInputObjRef);

    calcBE = ReadBool();
    calcET = ReadBool();

    ReadRealMSSpec(maxTimeDelayMS);
    timeBase = TimeBase(ReadInt());
    ReadRealMSSpec(specTimeSpacingMS);

    convertType = ReadInt();

    CheckForInputFailure("Reading DPO_CreateBEETResponseFunction");
    return true;
}


void DPO_CreateBEETResponseFunction::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(pressureInputObjRef);
    WriteFuncObjRef(barometricInputObjRef);
    WriteFuncObjRef(earthTideInputObjRef);

    WriteBool(calcBE);
    WriteBool(calcET);
    WriteLine();

    WriteRealMSSpec(maxTimeDelayMS);
    WriteInt(int(timeBase));
    WriteRealMSSpec(specTimeSpacingMS);

    //options
    WriteInt(convertType);

    WriteLine();
}

#ifndef ADCONSOLEAPP
// ************************* menu stuff

DPO_CreateBEETResponseFunctionPS& DPO_CreateBEETResponseFunction::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CreateBEETResponseFunctionPS&>(*assocMenu);
}


void DPO_CreateBEETResponseFunction::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_CreateBEETResponseFunction::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CreateBEETResponseFunctionPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_CreateBEETResponseFunction::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

// ************************* expose support

bool DPO_CreateBEETResponseFunction::ContainsExposedObject()
{
    return exposedMaxTimeDelayValue.IsExposed() || exposedTimeSpacingValue.IsExposed();
}

void DPO_CreateBEETResponseFunction::UpdateUI()
{
    exposedMaxTimeDelayValue.UpdateIfCreated();
    exposedTimeSpacingValue.UpdateIfCreated();

    if (MenuUpdateOK())
    {
        MenuRef().maxTimeDelayMSUI.realValueUI.SetFieldValue(maxTimeDelayMS.GetCurrentValue());
        MenuRef().specTimeSpacingMSUI.realValueUI.SetFieldValue(specTimeSpacingMS.GetCurrentValue());
    }
}
#endif //ADCONSOLEAPP

