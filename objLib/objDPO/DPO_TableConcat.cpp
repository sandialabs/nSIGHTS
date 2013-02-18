///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableConcat.cpp
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
#include "DPO_TableConcatPS.h"
#endif
#include "DPO_TableConcat.h"

static const char* objLongDesc = "Concatenate Tables";

DPO_TableConcat :: DPO_TableConcat() :
    MenuObjC(this, objLongDesc)
{
}

DPO_TableConcat :: DPO_TableConcat(const DPO_TableConcat& a) :
    DFO_TableConcat(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_TableConcat& DPO_TableConcat::operator= (const DPO_TableConcat& a)
{
    if (&a != this)
       *((DFO_TableConcat*) this) = a;
    return *this;
}


bool  DPO_TableConcat::ReadFromFile()
{
    ReadObjectHeader();

    if (currentObjReadMajorVersion > 0)
    {
        concatRows = ReadBool();
        if (currentObjReadMajorVersion > 1)
            skipFirstColumn = ReadBool();
    }
    else
        concatRows = true;

    int ntable  = ReadInt();
    for (int i = 0; i < ntable; i++)
    {
        tablesToConcat[i].doTable   = true;
        ReadFuncObjRef(tablesToConcat[i].tableObjRef);
    }

    for (int i = ntable; i < maxInputTable; i++)
        tablesToConcat[i] = DFO_TableConcat::TableInputData();

    CheckForInputFailure("Reading DPO_TableConcat");
    return true;
}

void DPO_TableConcat::WriteToFile()
{
    // ver 0
    const int objMajVer = 2; // added skipFirstColumn
//    const int objMajVer = 1; // added concatRows
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteBool(concatRows);       // v1
    WriteBool(skipFirstColumn);  // v2
    WriteLine();

    int nSel = 0;
    for (int i = 0; i < maxInputTable; i++)
        if (tablesToConcat[i].doTable)
            nSel++;

    WriteInt(nSel);
    WriteLine();
    for (int i = 0; i < maxInputTable; i++)
        if (tablesToConcat[i].doTable)
        {
            WriteFuncObjRef(tablesToConcat[i].tableObjRef);
        }

    WriteLine();

}


#ifndef ADCONSOLEAPP

DPO_TableConcatPS& DPO_TableConcat::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_TableConcatPS&>(*assocMenu);
}

void DPO_TableConcat::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_TableConcatPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_TableConcat::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_TableConcat::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


#endif //!ADCONSOLEAPP

