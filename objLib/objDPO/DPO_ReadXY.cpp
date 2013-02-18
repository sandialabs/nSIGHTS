///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadXY.cpp
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

#ifndef ADCONSOLEAPP
#include "DPO_ReadXYPS.h"
#endif
#include "DPO_ReadXY.h"


const char* DPO_ReadXY::objLongDesc = "Read XY Data";

DPO_ReadXY :: DPO_ReadXY() : MenuObjC(this, objLongDesc)
{
    RegisterFileName(xyFname);
}

DPO_ReadXY :: DPO_ReadXY(const DPO_ReadXY& a) : DFO_ReadXY(a), MenuObjC(this, objLongDesc)
{
    RegisterFileName(xyFname);
}

DPO_ReadXY::~DPO_ReadXY()
{
}

DPO_ReadXY& DPO_ReadXY::operator= (const DPO_ReadXY& a)
{
    if (&a != this)
       *((DFO_ReadXY*) this) = a;
    return *this;
}


bool  DPO_ReadXY::ReadFromFile()
{
    ReadObjectHeader();

    ReadFileName(xyFname);
    xyFileFormat = DSC_XYInputSpec::XYFileFormat(ReadInt());
    readColumnID = ReadBool();

    tableXcol = ReadInt();
    tableYcol = ReadInt();

    if (currentObjReadMajorVersion > 1)
    {
        dateSpec.timeSpecUnits = DC_DateTime::TimeUnits(ReadInt());
        dateSpec.timeSpecType = DC_DateTimeSpec::TimeSpecType(ReadInt());
        dateSpec.calendarYearStart = ReadInt();
    }

    ReadText(dataID, DC_XYData::dataIDLen);

    if (currentObjReadMajorVersion > 0)
        ReadNodeFileArray(mpiFiles);

    CheckForInputFailure("Reading DPO_ReadXY");
    return true;
}


void DPO_ReadXY::WriteToFile()
{
    const int objMajVer = 2;  // added date based reading
//    const int objMajVer = 1;  // added MPI node support
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteText(xyFname);
    WriteInt(int(xyFileFormat));
    WriteBool(readColumnID);
    WriteInt(tableXcol);
    WriteInt(tableYcol);
    WriteLine();

    WriteInt(int(dateSpec.timeSpecUnits));
    WriteInt(int(dateSpec.timeSpecType));
    WriteInt(dateSpec.calendarYearStart);
    WriteLine();

    WriteText(dataID);
    WriteNodeFileArray(mpiFiles);
}

#ifndef ADCONSOLEAPP

DPO_ReadXYPS& DPO_ReadXY::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ReadXYPS&>(*assocMenu);
}


void DPO_ReadXY::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ReadXY::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ReadXYPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(true, true);

    PropUpdate();
}


void DPO_ReadXY::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

