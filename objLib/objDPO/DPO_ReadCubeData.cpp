///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadCubeData.cpp
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
#include "DPO_ReadCubeDataPS.h"
#endif
#include "DPO_ReadCubeData.h"

const char* DPO_ReadCubeData::objLongDesc = "Read Cube Data";

DPO_ReadCubeData :: DPO_ReadCubeData() :
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(cubeDataFname);
}

DPO_ReadCubeData :: DPO_ReadCubeData(const DPO_ReadCubeData& a) :
    DFO_ReadCubeData(a),
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(cubeDataFname);
}

DPO_ReadCubeData::~DPO_ReadCubeData()
{
}

DPO_ReadCubeData& DPO_ReadCubeData::operator= (const DPO_ReadCubeData& a)
{
    if (&a != this)
       *((DFO_ReadCubeData*) this) = a;
    return *this;
}


bool  DPO_ReadCubeData::ReadFromFile()
{
    ReadObjectHeader();

    ReadFileName(cubeDataFname);

    ReadText(setXID, DC_CubeData::cubeIDLen);
    ReadText(setYID, DC_CubeData::cubeIDLen);
    ReadText(setZID, DC_CubeData::cubeIDLen);
    setXLog = ReadBool();
    setYLog = ReadBool();
    setZLog = ReadBool();

    cubeFileFormat = IO_CubeData::CubeFileFormat(ReadInt());

    CheckForInputFailure("Reading DPO_ReadCubeData");
    return true;
}


void DPO_ReadCubeData::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteText(cubeDataFname);

    WriteText(setXID);
    WriteText(setYID);
    WriteText(setZID);

    WriteBool(setXLog);
    WriteBool(setYLog);
    WriteBool(setZLog);

    WriteInt(int(cubeFileFormat));

    WriteLine();
}

#ifndef ADCONSOLEAPP


DPO_ReadCubeDataPS& DPO_ReadCubeData::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ReadCubeDataPS&>(*assocMenu);
}


void DPO_ReadCubeData::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ReadCubeData::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ReadCubeDataPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(true, false);

    PropUpdate();
}


void DPO_ReadCubeData::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP
