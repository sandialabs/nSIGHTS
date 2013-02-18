///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TransformTable.cpp
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
#include "DPO_TransformTablePS.h"
#endif
#include "DPO_TransformTable.h"

const char* DPO_TransformTable::objLongDesc = "Transform Table";

DPO_TransformTable :: DPO_TransformTable() : MenuObjC(this, objLongDesc)
{
}

DPO_TransformTable :: DPO_TransformTable(const DPO_TransformTable& a) : DFO_TransformTable(a), MenuObjC(this, objLongDesc)
{
}

DPO_TransformTable::~DPO_TransformTable()
{
}

DPO_TransformTable& DPO_TransformTable::operator= (const DPO_TransformTable& a)
{
    if (&a != this)
       static_cast<DFO_TransformTable&>(*this) = a;
    return *this;
}


bool  DPO_TransformTable::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(inputTableObjRef);
    columnDescAreVarID = ReadBool();
    outputTableData = OutputTableData(ReadInt());
    for (int i = 0; i < maxTransform; i++)
    {
        transformDesc[i].doTransform = ReadBool();
        ReadText(transformDesc[i].transformID, transformIDLen);
        ReadText(transformDesc[i].transformDesc, transformDescLen);
    }

    CheckForInputFailure("Reading DPO_TransformTable");
    return true;
}


void DPO_TransformTable::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteFuncObjRef(inputTableObjRef);

    WriteBool(columnDescAreVarID);
    WriteInt(int(outputTableData));
    WriteLine();
    for (int i = 0; i < maxTransform; i++)
    {
        WriteBool(transformDesc[i].doTransform);
        WriteLine();
        WriteText(transformDesc[i].transformID);
        WriteText(transformDesc[i].transformDesc);
    }

    WriteLine();

}

#ifndef ADCONSOLEAPP

DPO_TransformTablePS& DPO_TransformTable::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_TransformTablePS&>(*assocMenu);
}


void DPO_TransformTable::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_TransformTable::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_TransformTablePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_TransformTable::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

