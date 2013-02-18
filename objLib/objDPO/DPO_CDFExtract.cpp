///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CDFExtract.cpp
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
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "DPO_CDFExtract.h"
#ifndef ADCONSOLEAPP
#include "DPO_CDFExtractPS.h"
#endif

const char* DPO_CDFExtract::objLongDesc = "Interpolate Value or Probability from CDF";

DPO_CDFExtract :: DPO_CDFExtract() : MenuObjC(this, objLongDesc),
    exposedRvalValue(*this, "Extract at Value", rvalMS.enteredRealValue)
{
}

DPO_CDFExtract :: DPO_CDFExtract(const DPO_CDFExtract& a) :
    DFO_CDFExtract(a),
    MenuObjC(this, objLongDesc),
    exposedRvalValue(*this, "Extract at Value", rvalMS.enteredRealValue)
{
}


DPO_CDFExtract& DPO_CDFExtract::operator= (const DPO_CDFExtract& a)
{
    if (&a != this)
       *((DFO_CDFExtract*) this) = a;
    return *this;
}


bool  DPO_CDFExtract::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(cdfInputObjRef);
    ReadRealMSSpec(rvalMS);

    calcProbability = ReadBool();
    useLimits = ReadBool();

    exposedRvalValue.ReadExposed();

    CheckForInputFailure("Reading DPO_CDFExtract");
    return true;
}


void DPO_CDFExtract::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(cdfInputObjRef);

    SetRealConvFormat(SC_RealFormat(ncf_Scientific, 8));
    WriteRealMSSpec(rvalMS);

    WriteBool(calcProbability);
    WriteBool(useLimits);
    WriteLine();

    exposedRvalValue.WriteExposed();

}

#ifndef ADCONSOLEAPP

// ************************* menu support

DPO_CDFExtractPS& DPO_CDFExtract::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CDFExtractPS&>(*assocMenu);
}


void DPO_CDFExtract::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_CDFExtract::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CDFExtractPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_CDFExtract::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_CDFExtract::CalcOutput(FOcalcType  calcType)
{
    DFO_CDFExtract::CalcOutput(calcType);
    if (MenuUpdateOK())
    {
        MenuRef().rvalMSUI.realValueUI.SetFieldValue(rvalMS.GetCurrentValue());
        MenuRef().outputValStatus.SetFieldValue(outputValDO.realVal);
    }
}

bool DPO_CDFExtract::ContainsExposedObject()
{
    return exposedRvalValue.IsExposed();
}

#endif // ADCONSOLEAPP

