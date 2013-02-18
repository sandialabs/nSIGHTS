///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadGridData.cpp
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
#include "DPO_ReadGridDataPS.h"
#endif
#include "DPO_ReadGridData.h"

const char* DPO_ReadGridData::objLongDesc = "Read Grid Data";

DPO_ReadGridData :: DPO_ReadGridData() :
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(gridDataFname);
}

DPO_ReadGridData :: DPO_ReadGridData(const DPO_ReadGridData& a) :
    DFO_ReadGridData(a),
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(gridDataFname);
}

DPO_ReadGridData::~DPO_ReadGridData()
{
}

DPO_ReadGridData& DPO_ReadGridData::operator= (const DPO_ReadGridData& a)
{
    if (&a != this)
       *((DFO_ReadGridData*) this) = a;
    return *this;
}


bool  DPO_ReadGridData::ReadFromFile()
{
    ReadObjectHeader();

    ReadFileName(gridDataFname);

    if (currentObjReadMajorVersion > 0)
    {
        ReadText(setXID, DC_GridData::gridIDLen);
        ReadText(setYID, DC_GridData::gridIDLen);
        setXLog = ReadBool();
        setYLog = ReadBool();

        if (currentObjReadMajorVersion > 1)
            gridFileFormat = IO_GridData::GridFileFormat(ReadInt());

        if (currentObjReadMajorVersion > 2)
            decimationModulus = ReadInt();
    }

    CheckForInputFailure("Reading DPO_ReadGridData");
    return true;
}


void DPO_ReadGridData::WriteToFile()
{
    const int objMajVer = 3;        // added decimate
//    const int objMajVer = 2;      // added surfer
//    const int objMajVer = 1;      // added options
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteText(gridDataFname);

    WriteText(setXID);
    WriteText(setYID);

    WriteBool(setXLog);
    WriteBool(setYLog);

    WriteInt(int(gridFileFormat));

    WriteInt(decimationModulus);

    WriteLine();
}

#ifndef ADCONSOLEAPP

DPO_ReadGridDataPS& DPO_ReadGridData::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ReadGridDataPS&>(*assocMenu);
}


void DPO_ReadGridData::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ReadGridData::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ReadGridDataPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(true, false);

    PropUpdate();
}


void DPO_ReadGridData::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

