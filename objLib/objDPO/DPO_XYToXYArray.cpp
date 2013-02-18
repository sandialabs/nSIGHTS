///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYToXYArray.cpp
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
#include "DPO_XYToXYArrayPS.h"
#endif
#include "DPO_XYToXYArray.h"

const char* DPO_XYToXYArray::objLongDesc = "Add XY Data to XY Array";

DPO_XYToXYArray :: DPO_XYToXYArray() :
    MenuObjC(this, objLongDesc)
{
}

DPO_XYToXYArray :: DPO_XYToXYArray(const DPO_XYToXYArray& a) :
    DFO_XYToXYArray(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_XYToXYArray& DPO_XYToXYArray::operator= (const DPO_XYToXYArray& a)
{
    if (&a != this)
    {
       *((DFO_XYToXYArray*) this) = a;
    }
    return *this;
}

#ifndef ADCONSOLEAPP

DPO_XYToXYArrayPS& DPO_XYToXYArray::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_XYToXYArrayPS&>(*assocMenu);
}

void DPO_XYToXYArray::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_XYToXYArray::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_XYToXYArrayPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_XYToXYArray::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}
#endif //!ADCONSOLEAPP


bool  DPO_XYToXYArray::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(xyInputObjRef);

    CheckForInputFailure("Reading DPO_XYToXYArray");
    return true;
}

void DPO_XYToXYArray::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteFuncObjRef(xyInputObjRef);
}

