///////////////////////////////////////////////////////////////////////////////////
//
// DPO_BarometricCompensation.cpp
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
#include "DPO_BarometricCompensationPS.h"
#endif

#include "DPO_BarometricCompensation.h"

const char* DPO_BarometricCompensation::objLongDesc = "Barometric Pressure Compensation";

DPO_BarometricCompensation :: DPO_BarometricCompensation() :
    MenuObjC(this, objLongDesc)
{
}

DPO_BarometricCompensation :: DPO_BarometricCompensation(const DPO_BarometricCompensation& a) :
    DFO_BarometricCompensation(a),
    MenuObjC(this, objLongDesc)
{
}


DPO_BarometricCompensation& DPO_BarometricCompensation::operator= (const DPO_BarometricCompensation& a)
{
    if (&a != this)
       *((DFO_BarometricCompensation*) this) = a;
    return *this;
}


bool  DPO_BarometricCompensation::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(pressureInputObjRef);
    ReadFuncObjRef(barometricInputObjRef);
    ReadRealMSSpec(barometricEfficiencyMS);

    if (currentObjReadMajorVersion > 0)
    {

        correctionMethod = CorrectionMethod(ReadInt());
        regressLag = ReadInt();
        regressM = ReadInt();

        if (currentObjReadMajorVersion > 1)
        {
            evenTimeSpacing = ReadBool();
            timeSpacing = ReadDouble();
        }
    }

    CheckForInputFailure("Reading DPO_BarometricCompensation");
    return true;
}


void DPO_BarometricCompensation::WriteToFile()
{
    const int objMajVer = 2; // added even space regression approach
//  const int objMajVer = 1; // added regression approach
//  const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(pressureInputObjRef);
    WriteFuncObjRef(barometricInputObjRef);
    WriteRealMSSpec(barometricEfficiencyMS);
    WriteLine();

    // all v1
    WriteInt(int(correctionMethod));
    WriteInt(regressLag);
    WriteInt(regressM);

    // v2
    WriteBool(evenTimeSpacing);
    WriteDouble(timeSpacing);

    WriteLine();
}

#ifndef ADCONSOLEAPP
// ************************* menu stuff

DPO_BarometricCompensationPS& DPO_BarometricCompensation::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_BarometricCompensationPS&>(*assocMenu);
}


void DPO_BarometricCompensation::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_BarometricCompensation::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_BarometricCompensationPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_BarometricCompensation::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //ADCONSOLEAPP

