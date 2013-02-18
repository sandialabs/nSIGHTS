///////////////////////////////////////////////////////////////////////////////////
//
// DPO_RealToTable.cpp
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
#include "DPO_RealToTablePS.h"
#endif
#include "DPO_RealToTable.h"

const char* DPO_RealToTable::objLongDesc = "Add Real Values To Table";

DPO_RealToTable :: DPO_RealToTable() :
    MenuObjC(this, objLongDesc)
{
    saveTableData = false;
}

DPO_RealToTable :: DPO_RealToTable(const DPO_RealToTable& a) :
    DFO_RealToTable(a),
    MenuObjC(this, objLongDesc)
{
    saveTableData = a.saveTableData;
}

DPO_RealToTable& DPO_RealToTable::operator= (const DPO_RealToTable& a)
{
    if (&a != this)
    {
       *((DFO_RealToTable*) this) = a;
        saveTableData = a.saveTableData;
    }
    return *this;
}


bool  DPO_RealToTable::ReadFromFile()
{
    ReadObjectHeader();

    int nCol  = ReadInt();
    int i;
    for (i = 0; i < nCol; i++)
    {
        columnData[i].doReal   = true;
        ReadFuncObjRef(columnData[i].realObjRef);
        ReadText(columnData[i].columnID, maxColumnID);
    }

    for (i = nCol; i < maxTableCol; i++)
        columnData[i] = RealData();

    saveTableData = ReadBool();
    if (saveTableData)
    {
        int nRow = ReadInt();
        if (nRow > 0)
        {
            realTable.ResizableAlloc(nCol, nRow, maxColumnID);
            for (i = 0; i < nCol; i++)
                realTable.SetTitle(i, columnData[i].columnID);
            for (int j = 0; j < nRow; j++)
                for (i = 0; i < nCol; i++)
                    realTable.dataTable[i] += ReadNullDouble();
        }
    }

    CheckForInputFailure("Reading DPO_RealToTable");
    return true;
}

void DPO_RealToTable::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    int nSel = 0;

    int i;
    for (i = 0; i < maxTableCol; i++)
        if (columnData[i].doReal)
            nSel++;

    WriteInt(nSel);
    for (i = 0; i < maxTableCol; i++)
        if (columnData[i].doReal)
        {
            WriteFuncObjRef(columnData[i].realObjRef);
            WriteText(columnData[i].columnID);
        }


    WriteBool(saveTableData);
    WriteLine();
    if (saveTableData)
    {
        if (StatusNotOK() || realTable.dataTable.IsEmpty())
        {
            WriteInt(0);
            WriteLine();
        }
        else
        {
            WriteInt(realTable.GetNRows());
            WriteLine();
            for (int j = 0; j < realTable.GetNRows(); j++)
            {
                for (i = 0; i < realTable.GetNCols(); i++)
                    WriteNullDouble(realTable.dataTable[i][j]);
                WriteLine();
            }
        }
    }
}


#ifndef ADCONSOLEAPP

DPO_RealToTablePS& DPO_RealToTable::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_RealToTablePS&>(*assocMenu);
}

void DPO_RealToTable::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_RealToTable::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_RealToTablePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_RealToTable::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

