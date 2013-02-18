///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableColumnSort.cpp
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

#include "DPO_TableColumnSort.h"
#ifndef ADCONSOLEAPP
#include "DPO_TableColumnSortPS.h"
#endif

static const char* objLongDesc = "Sort All Table Rows by a Single Column Value";

DPO_TableColumnSort :: DPO_TableColumnSort() :
    MenuObjC(this, objLongDesc)
{
}

DPO_TableColumnSort :: DPO_TableColumnSort(const DPO_TableColumnSort& a) :
    DFO_TableColumnSort(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_TableColumnSort& DPO_TableColumnSort::operator= (const DPO_TableColumnSort& a)
{
    if (&a != this)
       *((DFO_TableColumnSort*) this) = a;
    return *this;
}


bool DPO_TableColumnSort::ReadFromFile()
{
    ReadObjectHeader();

    ReadTableColSel(*this);
    sortAscending = ReadBool();
    CheckForInputFailure("Reading DPO_TableColumnSort");
    return true;
}

void DPO_TableColumnSort::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteTableColSel(*this);
    WriteBool(sortAscending);
    WriteLine();
}

#ifndef ADCONSOLEAPP

DPO_TableColumnSortPS& DPO_TableColumnSort::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_TableColumnSortPS&>(*assocMenu);
}

void DPO_TableColumnSort::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_TableColumnSortPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_TableColumnSort::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_TableColumnSort::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

