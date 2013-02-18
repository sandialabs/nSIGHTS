///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableToSATable.cpp
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
#include "DPO_TableToSATablePS.h"
#endif
#include "DPO_TableToSATable.h"

const char* DPO_TableToSATable::objLongDesc = "Convert Table to SA Table";

DPO_TableToSATable :: DPO_TableToSATable() : MenuObjC(this, objLongDesc)
{
}

DPO_TableToSATable :: DPO_TableToSATable(const DPO_TableToSATable& a) : DFO_TableToSATable(a), MenuObjC(this, objLongDesc)
{
}

DPO_TableToSATable::~DPO_TableToSATable()
{
}

DPO_TableToSATable& DPO_TableToSATable::operator= (const DPO_TableToSATable& a)
{
    if (&a != this)
       static_cast<DFO_TableToSATable&>(*this) = a;
    return *this;
}


bool  DPO_TableToSATable::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(inputTableObjRef);

    rowData = DC_SATable::SATableComponent(ReadInt());
    colData = DC_SATable::SATableComponent(ReadInt());

    ReadText(variableName, DC_SATable::varNameLen);
    stepVal = ReadDouble();
    colRow0IsObsIndex = ReadBool();
    stepValueIsIDs = ReadBool();

    CheckForInputFailure("Reading DPO_TableToSATable");
    return true;
}


void DPO_TableToSATable::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteFuncObjRef(inputTableObjRef);

    WriteInt(int(rowData));
    WriteInt(int(colData));
    WriteLine();

    WriteText(variableName);
    WriteDouble(stepVal);
    WriteBool(colRow0IsObsIndex);
    WriteBool(stepValueIsIDs);
    WriteLine();

}

#ifndef ADCONSOLEAPP

DPO_TableToSATablePS& DPO_TableToSATable::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_TableToSATablePS&>(*assocMenu);
}


void DPO_TableToSATable::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_TableToSATable::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_TableToSATablePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_TableToSATable::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

