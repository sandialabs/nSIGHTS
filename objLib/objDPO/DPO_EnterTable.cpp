///////////////////////////////////////////////////////////////////////////////////
//
// DPO_EnterTable.cpp
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
#include "DPO_EnterTablePS.h"
#endif
#include "DPO_EnterTable.h"


const char* DPO_EnterTable::objLongDesc = "Enter Table Data";

DPO_EnterTable :: DPO_EnterTable() : MenuObjC(this, objLongDesc)
{
}

DPO_EnterTable :: DPO_EnterTable(const DPO_EnterTable& a) : DFO_EnterTable(a), MenuObjC(this, objLongDesc)
{
}

DPO_EnterTable& DPO_EnterTable::operator= (const DPO_EnterTable& a)
{
    if (&a != this)
       *((DFO_EnterTable*) this) = a;
    return *this;
}


bool  DPO_EnterTable::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(inputTableObjRef);
    ReadRealFormat(tableFormat);

    int nColRead = ReadInt();
    int nRowRead = ReadInt();
    tableDataDC.Alloc(nColRead, nRowRead, 40);

    char temp[80];
    for (int i = 0; i < nColRead; i++)
    {
        ReadText(temp, 80);
        tableDataDC.SetTitle(i, temp);
    }

    for (int i = 0; i < nRowRead; i++)
        for (int j = 0; j < nColRead; j++)
            tableDataDC.dataTable[j] += ReadNullDouble();

    tableDataDC.dataTable.SetSize(nColRead);

    if ((currentObjReadMajorVersion > 0) && ReadBool())
    {
       for (int i = 0; i < nRowRead; i++)
       {
            ReadText(temp, 80);
            tableDataDC.rowDesc += temp;
       }
    }

    CheckForInputFailure("Reading DPO_EnterTable");
    return true;
}


void DPO_EnterTable::WriteToFile()
{
    const int objMajVer = 1;  // added table row descs
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(inputTableObjRef);
    WriteRealFormat(tableFormat);
    WriteLine();

    int nCols = tableDataDC.GetNCols();
    int nRows = tableDataDC.GetNRows();

    WriteInt(nCols);
    WriteInt(nRows);
    WriteLine();

    for (int i = 0; i < nCols; i++)
        WriteText(tableDataDC.columnDesc[i]);

    SetRealConvFormat(SC_SciFormat(12));
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
            WriteDouble(tableDataDC.dataTable[j][i]);
        WriteLine();
    }

    // added v1
    bool writeRowDesc = (tableDataDC.rowDesc.Size() == nRows);
    WriteBool(writeRowDesc);
    WriteLine();
    if (writeRowDesc)
    {
       for (int i = 0; i < nRows; i++)
           WriteText(tableDataDC.rowDesc[i]);
    }

    WriteLine();

}

#ifndef ADCONSOLEAPP

DPO_EnterTablePS& DPO_EnterTable::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_EnterTablePS&>(*assocMenu);
}


void DPO_EnterTable::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_EnterTable::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_EnterTablePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_EnterTable::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

