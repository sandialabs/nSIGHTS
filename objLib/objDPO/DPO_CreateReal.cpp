///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CreateReal.cpp
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

#include <genApp/AppConfigFile.h>

#include "DPO_CreateReal.h"

#ifndef ADCONSOLEAPP
#include "DPO_CreateRealPS.h"
#endif

const char* DPO_CreateReal::objLongDesc = "Create Real Value";

DPO_CreateReal :: DPO_CreateReal() : MenuObjC(this, objLongDesc),
    exposedRvalValue(*this, "Real Output", rvalMS.enteredRealValue)
{
}

DPO_CreateReal :: DPO_CreateReal(const DPO_CreateReal& a) :
    DFO_CreateReal(a),
    MenuObjC(this, objLongDesc),
    exposedRvalValue(*this, "Real Output", rvalMS.enteredRealValue)
{
}


DPO_CreateReal& DPO_CreateReal::operator= (const DPO_CreateReal& a)
{
    if (&a != this)
       *((DFO_CreateReal*) this) = a;
    return *this;
}


bool  DPO_CreateReal::ReadFromFile()
{
    ReadObjectHeader();

    ReadRealMSSpec(rvalMS);

    ReadText(realDesc, descMaxLen);

    exposedRvalValue.ReadExposed();

    CheckForInputFailure("Reading DPO_CreateReal");
    return true;
}


void DPO_CreateReal::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    SetRealConvFormat(SC_RealFormat(ncf_Scientific, 8));
    WriteRealMSSpec(rvalMS);
    WriteText(realDesc);

    exposedRvalValue.WriteExposed();

}

#ifndef ADCONSOLEAPP

DPO_CreateRealPS& DPO_CreateReal::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CreateRealPS&>(*assocMenu);
}


void DPO_CreateReal::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_CreateReal::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CreateRealPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_CreateReal::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_CreateReal::CalcOutput(FOcalcType  calcType)
{
    DFO_CreateReal::CalcOutput(calcType);
    if (MenuUpdateOK())
        MenuRef().rvalMSUI.realValueUI.SetFieldValue(rvalMS.GetCurrentValue());
}

bool DPO_CreateReal::ContainsExposedObject()
{
    return exposedRvalValue.IsExposed();
}

#endif //!ADCONSOLEAPP

