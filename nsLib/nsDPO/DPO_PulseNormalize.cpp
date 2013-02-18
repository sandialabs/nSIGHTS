///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PulseNormalize.cpp
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
#include "DPO_PulseNormalizePS.h"
#endif

#include "DPO_PulseNormalize.h"

const char* DPO_PulseNormalize::objLongDesc = "Normalize Pulse Data";

DPO_PulseNormalize :: DPO_PulseNormalize() :
    MenuObjC(this, objLongDesc),
    exposedP0Value(*this, "P0 (Static)", p0_MS.enteredRealValue),
    exposedPIValue(*this, "Pi (pulse start)", pI_MS.enteredRealValue)
{
}

DPO_PulseNormalize :: DPO_PulseNormalize(const DPO_PulseNormalize& a) :
    DFO_PulseNormalize(a),
    MenuObjC(this, objLongDesc),
    exposedP0Value(*this, "P0 (Static)", p0_MS.enteredRealValue),
    exposedPIValue(*this, "Pi (pulse start)", pI_MS.enteredRealValue)
{
}


DPO_PulseNormalize& DPO_PulseNormalize::operator= (const DPO_PulseNormalize& a)
{
    if (&a != this)
       *((DFO_PulseNormalize*) this) = a;
    return *this;
}


bool  DPO_PulseNormalize::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(xyInputObjRef);
    ReadRealMSSpec(pI_MS);
    ReadRealMSSpec(p0_MS);

    pulseNormalizationType     = PulseNormalizationType(ReadInt());

    if (currentObjReadMajorVersion > 0)
        multiplyYbyX = ReadBool();

    exposedP0Value.ReadExposed();
    exposedPIValue.ReadExposed();

    CheckForInputFailure("Reading DPO_PulseNormalize");
    return true;
}


void DPO_PulseNormalize::WriteToFile()
{
    const int objMajVer = 1;   // added multiply Y by X
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(xyInputObjRef);
    WriteRealMSSpec(pI_MS);
    WriteRealMSSpec(p0_MS);
    WriteLine();

    WriteInt(pulseNormalizationType);
    WriteBool(multiplyYbyX);
    WriteLine();

    exposedP0Value.WriteExposed();
    exposedPIValue.WriteExposed();

}

#ifndef ADCONSOLEAPP

// ************************* expose support

void DPO_PulseNormalize::CalcOutput(FOcalcType  calcType)
{
    DFO_PulseNormalize::CalcOutput(calcType);
    if (MenuUpdateOK())
    {
        MenuRef().pI_MSUI.realValueUI.SetFieldValue(pI_MS.GetCurrentValue());
        MenuRef().p0_MSUI.realValueUI.SetFieldValue(p0_MS.GetCurrentValue());
    }
}

bool DPO_PulseNormalize::ContainsExposedObject()
{
    return exposedP0Value.IsExposed() || exposedPIValue.IsExposed() ;
}


// ************************* menu stuff

DPO_PulseNormalizePS& DPO_PulseNormalize::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_PulseNormalizePS&>(*assocMenu);
}


void DPO_PulseNormalize::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_PulseNormalize::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_PulseNormalizePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_PulseNormalize::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}



#endif //ADCONSOLEAPP

