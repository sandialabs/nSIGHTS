///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CubeExtractGrid.cpp
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

#include "DPO_CubeExtractGrid.h"
#ifndef ADCONSOLEAPP
#include "DPO_CubeExtractGridPS.h"
#endif

static const char* objLongDesc = "Extract Grid from Cube";

DPO_CubeExtractGrid :: DPO_CubeExtractGrid() :
    MenuObjC(this, objLongDesc),
    exposedExtractVal(*this, "Extract Grid", extractValMS.enteredRealValue)
{

}

DPO_CubeExtractGrid :: DPO_CubeExtractGrid(const DPO_CubeExtractGrid& a) :
    MenuObjC(this, objLongDesc),
    DFO_CubeExtractGrid(a),
    exposedExtractVal(*this, "Extract XYZ", extractValMS.enteredRealValue)
{
}


DPO_CubeExtractGrid& DPO_CubeExtractGrid::operator= (const DPO_CubeExtractGrid& a)
{
    if (&a != this)
       *((DFO_CubeExtractGrid*) this) = a;
    return *this;
}


bool  DPO_CubeExtractGrid::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(cubeInputObjRef);

    gridX   = DC_CubeData::ExtractAxes(ReadInt());
    gridY   = DC_CubeData::ExtractAxes(ReadInt());
    logInterp = ReadBool();

    ReadRealMSSpec(extractValMS);
    exposedExtractVal.ReadExposed();

    CheckForInputFailure("Reading DPO_CubeExtractGrid");
    return true;
}


void DPO_CubeExtractGrid::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(cubeInputObjRef);

    WriteInt(int(gridX));
    WriteInt(int(gridY));
    WriteBool(logInterp);

    WriteRealMSSpec(extractValMS);
    exposedExtractVal.WriteExposed();

}

#ifndef ADCONSOLEAPP

bool DPO_CubeExtractGrid::ContainsExposedObject()
{
    return exposedExtractVal.IsExposed();
}


DPO_CubeExtractGridPS& DPO_CubeExtractGrid::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CubeExtractGridPS&>(*assocMenu);
}

void DPO_CubeExtractGrid::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_CubeExtractGrid::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CubeExtractGridPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_CubeExtractGrid::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_CubeExtractGrid::CalcOutput(FOcalcType  calcType)
{
    DFO_CubeExtractGrid::CalcOutput(calcType);
    exposedExtractVal.UpdateIfCreated();
}

#endif //!ADCONSOLEAPP

