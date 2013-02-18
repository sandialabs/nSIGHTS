///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadLabelArray.cpp
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

#include "DPO_ReadLabelArray.h"
#ifndef ADCONSOLEAPP
#include "DPO_ReadLabelArrayPS.h"
#endif

const char* DPO_ReadLabelArray::objLongDesc = "Read Label Array";

DPO_ReadLabelArray :: DPO_ReadLabelArray() : MenuObjC(this, objLongDesc)
{
    RegisterFileName(labelsFname);
}

DPO_ReadLabelArray :: DPO_ReadLabelArray(const DPO_ReadLabelArray& a) :
    DFO_ReadLabelArray(a),
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(labelsFname);
}

DPO_ReadLabelArray::~DPO_ReadLabelArray()
{
}

DPO_ReadLabelArray& DPO_ReadLabelArray::operator= (const DPO_ReadLabelArray& a)
{
    if (&a != this)
       *((DFO_ReadLabelArray*) this) = a;
    return *this;
}


bool  DPO_ReadLabelArray::ReadFromFile()
{
    ReadObjectHeader();
    ReadFileName(labelsFname);
    labelFileFormat = IO_LabelArray::LabelFileFormat(ReadInt());

    CheckForInputFailure("Reading DPO_ReadLabelArray");
    return true;
}


void DPO_ReadLabelArray::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteText(labelsFname);
    WriteInt(int(labelFileFormat));
    WriteLine();

    WriteLine();

}

#ifndef ADCONSOLEAPP

DPO_ReadLabelArrayPS& DPO_ReadLabelArray::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ReadLabelArrayPS&>(*assocMenu);
}


void DPO_ReadLabelArray::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ReadLabelArray::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ReadLabelArrayPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_ReadLabelArray::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


#endif //!ADCONSOLEAPP

