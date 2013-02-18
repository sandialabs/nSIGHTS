///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableToGrid.cpp
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

#include "DPO_TableToGrid.h"

#ifndef ADCONSOLEAPP
#include "DPO_TableToGridPS.h"
#endif

static const char* objLongDesc = "Convert Table to Grid";

DPO_TableToGrid :: DPO_TableToGrid() :
    MenuObjC(this, objLongDesc)
{
}

DPO_TableToGrid :: DPO_TableToGrid(const DPO_TableToGrid& a) :
    DFO_TableToGrid(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_TableToGrid& DPO_TableToGrid::operator= (const DPO_TableToGrid& a)
{
    if (&a != this)
       *((DFO_TableToGrid*) this) = a;
    return *this;
}


bool DPO_TableToGrid::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(tableInputObjRef);
    if (currentObjReadMajorVersion > 1)
    {
        convertFormat = ConvertFormat(ReadInt());   // v2
        selectedCol = ReadInt();    // v2
        ycolumnIndex = ReadInt();   // v2
        dataColumnIndex = ReadInt();    // v2
        useColumnIndex = ReadBool(); // v2
    }

    columnValueIsHeader = ReadBool();
    gridXIsColumns = ReadBool();
    if (currentObjReadMajorVersion > 0)
    {
        tableRowModulus = ReadInt();  // added v1
        tableColModulus = ReadInt();    // added v1
        includeLastRow = ReadBool();    // added v1
        includeLastCol = ReadBool();    // added v1
    }

    ReadText(xdataID, DC_DataCoord::dataIDLen);
    ReadText(ydataID, DC_DataCoord::dataIDLen);

    CheckForInputFailure("Reading DPO_TableToGrid");
    return true;
}

void DPO_TableToGrid::WriteToFile()
{
    const int objMajVer = 2; // added format & XYZ list
//    const int objMajVer = 1; // added modulus & keep last
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(tableInputObjRef);

    WriteInt(int(convertFormat));   // v2
    WriteInt(selectedCol);  // v2
    WriteInt(ycolumnIndex); // v2
    WriteInt(dataColumnIndex);  // v2
    WriteBool(useColumnIndex); // v2
    WriteLine();

    WriteBool(columnValueIsHeader);
    WriteBool(gridXIsColumns);

    WriteInt(tableRowModulus);  // added v1
    WriteInt(tableColModulus);  // added v1
    WriteBool(includeLastRow);  // added v1
    WriteBool(includeLastCol);  // added v1

    WriteLine();
    WriteText(xdataID);
    WriteText(ydataID);
}

#ifndef ADCONSOLEAPP

DPO_TableToGridPS& DPO_TableToGrid::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_TableToGridPS&>(*assocMenu);
}

void DPO_TableToGrid::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_TableToGridPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_TableToGrid::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_TableToGrid::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

