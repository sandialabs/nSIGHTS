///////////////////////////////////////////////////////////////////////////////////
//
// DPO_HeadCalculation.cpp
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
#include "DPO_HeadCalculationPS.h"
#endif

#include "DPO_HeadCalculation.h"

const char* DPO_HeadCalculation::objLongDesc = "Head Calculation";

DPO_HeadCalculation :: DPO_HeadCalculation() :
    MenuObjC(this, objLongDesc)
{
}

DPO_HeadCalculation :: DPO_HeadCalculation(const DPO_HeadCalculation& a) :
    DFO_HeadCalculation(a),
    MenuObjC(this, objLongDesc)
{
}


DPO_HeadCalculation& DPO_HeadCalculation::operator= (const DPO_HeadCalculation& a)
{
    if (&a != this)
       *((DFO_HeadCalculation*) this) = a;
    return *this;
}


bool  DPO_HeadCalculation::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(inputPressureObjRef);
    ReadFuncObjRef(inputDensityObjRef);
    ReadRealMSSpec(groundSurfaceElevationMS);

    headCalculation = HeadCalculation(ReadInt());
    outputIsElevation = ReadBool();

    CheckForInputFailure("Reading DPO_HeadCalculation");
    return true;
}


void DPO_HeadCalculation::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(inputPressureObjRef);
    WriteFuncObjRef(inputDensityObjRef);
    WriteRealMSSpec(groundSurfaceElevationMS);
    WriteLine();

    WriteInt(int(headCalculation));
    WriteBool(outputIsElevation);
    WriteLine();
}

#ifndef ADCONSOLEAPP
// ************************* menu stuff

DPO_HeadCalculationPS& DPO_HeadCalculation::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_HeadCalculationPS&>(*assocMenu);
}


void DPO_HeadCalculation::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_HeadCalculation::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_HeadCalculationPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_HeadCalculation::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //ADCONSOLEAPP

