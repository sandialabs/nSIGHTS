///////////////////////////////////////////////////////////////////////////////////
//
// LPO_Table.cpp
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

#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "LPO_TablePS.h"
#include "LPO_Table.h"

const char* LPO_Table::objLongDesc = "Listing of Table Data";

LPO_Table :: LPO_Table(ListDefC* assLst) :
    LPO_Base(this, objLongDesc), LFO_Table(*assLst)
{
}

LPO_Table :: LPO_Table(ListDefC* assLst, const LPO_Table& a) :
    LFO_Table(a, *assLst),
    LPO_Base(this, objLongDesc)
{
}

LPO_Table& LPO_Table::operator= (const LPO_Table& a)
{
    if (&a != this)
       *((LFO_Table*) this) = a;

    return *this;
}

// ************************* menu ops

LPO_TablePS& LPO_Table::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<LPO_TablePS&>(*assocMenu);
}

void LPO_Table::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void LPO_Table::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new LPO_TablePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void LPO_Table::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  LPO_Table::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(tableObjRef);

    listTableID = ReadBool();
    listColumnHeaders = ReadBool();
    listRowDescriptions = ReadBool();
    ReadRealFormat(tableDataFormat);
    tableColWidth = ReadInt();
    maxListWidth = ReadInt();

    CheckForInputFailure("Reading LPO_Table");
    return true;
}

void LPO_Table::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(tableObjRef);

    WriteBool(listTableID);
    WriteBool(listColumnHeaders);
    WriteBool(listRowDescriptions);
    WriteRealFormat(tableDataFormat);
    WriteInt(tableColWidth);
    WriteInt(maxListWidth);
    WriteLine();

}

