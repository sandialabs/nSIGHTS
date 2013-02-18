///////////////////////////////////////////////////////////////////////////////////
//
// DPO_GridExtractXY.cpp
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

#include <objDPO/objDPOConfigFile.h>

#include "DPO_GridExtractXY.h"

#ifndef ADCONSOLEAPP
#include "DPO_GridExtractXYPS.h"
#endif

static const char* objLongDesc = "Extract XY from Grid Data";

DPO_GridExtractXY :: DPO_GridExtractXY() :
    MenuObjC(this, objLongDesc),
    exposedExtractVal(*this, "Extract XY", extractValMS.enteredRealValue)
{

}

DPO_GridExtractXY :: DPO_GridExtractXY(const DPO_GridExtractXY& a) :
    MenuObjC(this, objLongDesc),
    DFO_GridExtractXY(a),
    exposedExtractVal(*this, "Extract XY", extractValMS.enteredRealValue)
{
}


DPO_GridExtractXY& DPO_GridExtractXY::operator= (const DPO_GridExtractXY& a)
{
    if (&a != this)
       *((DFO_GridExtractXY*) this) = a;
    return *this;
}


bool  DPO_GridExtractXY::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(gridInputObjRef);

    extractX  = ReadBool();
    logInterp = ReadBool();

    ReadRealMSSpec(extractValMS);
    exposedExtractVal.ReadExposed();

    CheckForInputFailure("Reading DPO_GridExtractXY");
    return true;
}


void DPO_GridExtractXY::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(gridInputObjRef);

    WriteBool(extractX);
    WriteBool(logInterp);

    WriteRealMSSpec(extractValMS);
    exposedExtractVal.WriteExposed();

}

#ifndef ADCONSOLEAPP

bool DPO_GridExtractXY::ContainsExposedObject()
{
    return exposedExtractVal.IsExposed();
}


DPO_GridExtractXYPS& DPO_GridExtractXY::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_GridExtractXYPS&>(*assocMenu);
}

void DPO_GridExtractXY::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_GridExtractXY::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_GridExtractXYPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_GridExtractXY::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_GridExtractXY::CalcOutput(FOcalcType  calcType)
{
    DFO_GridExtractXY::CalcOutput(calcType);
    exposedExtractVal.UpdateIfCreated();
}


#endif //!ADCONSOLEAPP

