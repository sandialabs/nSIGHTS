///////////////////////////////////////////////////////////////////////////////////
//
// UPO_WriteLabelArray.cpp
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
//      File I/O and user-interface for objUFO.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "UPO_WriteLabelArrayPS.h"
#endif
#include "UPO_WriteLabelArray.h"


const char* UPO_WriteLabelArray::objLongDesc = "Write Label Array";

UPO_WriteLabelArray :: UPO_WriteLabelArray() : MenuObjC(this, objLongDesc)
{
    RegisterFileName(outFname);
}

UPO_WriteLabelArray :: UPO_WriteLabelArray(const UPO_WriteLabelArray& a) : UFO_WriteLabelArray(a), MenuObjC(this, objLongDesc)
{
    RegisterFileName(outFname);
}

UPO_WriteLabelArray& UPO_WriteLabelArray::operator= (const UPO_WriteLabelArray& a)
{
    if (&a != this)
       *((UFO_WriteLabelArray*) this) = a;
    return *this;
}

bool  UPO_WriteLabelArray::ReadFromFile()
{
    ReadObjectHeader();
    ReadFileName(outFname);
    ReadFuncObjRef(labelObjRef);
    CheckForInputFailure("Reading UPO_WriteLabelArray");
    return true;
}


void UPO_WriteLabelArray::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteText(outFname);
    WriteFuncObjRef(labelObjRef);
    WriteLine();
}


#ifndef ADCONSOLEAPP

UPO_WriteLabelArrayPS& UPO_WriteLabelArray::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<UPO_WriteLabelArrayPS&>(*assocMenu);
}


void UPO_WriteLabelArray::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void UPO_WriteLabelArray::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new UPO_WriteLabelArrayPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void UPO_WriteLabelArray::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

