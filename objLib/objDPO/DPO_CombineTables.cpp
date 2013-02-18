///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CombineTables.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_CombineTablesPS.h"
#endif
#include "DPO_CombineTables.h"

const char* DPO_CombineTables::objLongDesc = "Combine Table Columns";

DPO_CombineTables :: DPO_CombineTables() :
    MenuObjC(this, objLongDesc)
{
}

DPO_CombineTables :: DPO_CombineTables(const DPO_CombineTables& a) :
    DFO_CombineTables(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_CombineTables& DPO_CombineTables::operator= (const DPO_CombineTables& a)
{
    if (&a != this)
    {
       *((DFO_CombineTables*) this) = a;
    }
    return *this;
}


bool  DPO_CombineTables::ReadFromFile()
{
    ReadObjectHeader();

    int nCol  = ReadInt();
    int i;
    for (i = 0; i < nCol; i++)
    {
        columnData[i].doTableCol   = true;
        ReadFuncObjRef(columnData[i].tableObjRef);
        columnData[i].tableColumnIndex = ReadInt();
        ReadText(columnData[i].columnID, maxColumnID);
    }

    for (i = nCol; i < maxTableCol; i++)
        columnData[i] = TableData();

    CheckForInputFailure("Reading DPO_CombineTables");
    return true;
}

void DPO_CombineTables::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    int nSel = 0;
    int i;
    for (i = 0; i < maxTableCol; i++)
        if (columnData[i].doTableCol)
            nSel++;

    WriteInt(nSel);
    WriteLine();

    for (i = 0; i < maxTableCol; i++)
        if (columnData[i].doTableCol)
        {
            WriteFuncObjRef(columnData[i].tableObjRef);
            WriteInt(columnData[i].tableColumnIndex);
            WriteLine();
            WriteText(columnData[i].columnID);
        }
}


#ifndef ADCONSOLEAPP

DPO_CombineTablesPS& DPO_CombineTables::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CombineTablesPS&>(*assocMenu);
}

void DPO_CombineTables::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_CombineTables::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CombineTablesPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_CombineTables::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

