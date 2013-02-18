///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadSATable.cpp
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

#include "saLibAfx.h"

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_ReadSATablePS.h"
#endif
#include "DPO_ReadSATable.h"


const char* DPO_ReadSATable::objLongDesc = "Read SA Table File";

DPO_ReadSATable :: DPO_ReadSATable() : MenuObjC(this, objLongDesc)
{
    RegisterFileName(saFname);
    RegisterFileName(varNameFname);
}

DPO_ReadSATable :: DPO_ReadSATable(const DPO_ReadSATable& a) : DFO_ReadSATable(a), MenuObjC(this, objLongDesc)
{
    RegisterFileName(saFname);
    RegisterFileName(varNameFname);
}

DPO_ReadSATable::~DPO_ReadSATable()
{
}

DPO_ReadSATable& DPO_ReadSATable::operator= (const DPO_ReadSATable& a)
{
    if (&a != this)
       static_cast<DFO_ReadSATable&>(*this) = a;
    return *this;
}


bool  DPO_ReadSATable::ReadFromFile()
{
    ReadObjectHeader();

    ReadFileName(saFname);
    ReadFileName(varNameFname);
    ReadText(goldsimDepVar, DC_SATable::varNameLen);
    saFileFormat = IO_SATable::SAFileFormat(ReadInt());
    translateIndepVarNames = ReadBool();

    if (currentObjReadMajorVersion > 0)
    {
        selectOnlyNewNames = ReadBool();
        sortByNames = ReadBool();
    }

    CheckForInputFailure("Reading DPO_ReadSATable");
    return true;
}


void DPO_ReadSATable::WriteToFile()
{
    const int objMajVer = 1;  // added select and sort
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteText(saFname);
    WriteText(varNameFname);
    WriteText(goldsimDepVar);
    WriteInt(int(saFileFormat));
    WriteBool(translateIndepVarNames);
    WriteBool(selectOnlyNewNames); // added v1
    WriteBool(sortByNames);         // added v1
    WriteLine();

}

#ifndef ADCONSOLEAPP

DPO_ReadSATablePS& DPO_ReadSATable::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ReadSATablePS&>(*assocMenu);
}


void DPO_ReadSATable::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ReadSATable::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ReadSATablePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(true, true);

    PropUpdate();
}


void DPO_ReadSATable::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

