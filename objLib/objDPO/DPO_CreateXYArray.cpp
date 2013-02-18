///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CreateXYArray.cpp
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
#include "DPO_CreateXYArrayPS.h"
#endif
#include "DPO_CreateXYArray.h"

const char* DPO_CreateXYArray::objLongDesc = "Create XY Array";

DPO_CreateXYArray :: DPO_CreateXYArray() :
    MenuObjC(this, objLongDesc)
{ }

DPO_CreateXYArray :: DPO_CreateXYArray(const DPO_CreateXYArray& a) :
    DFO_CreateXYArray(a),
    MenuObjC(this, objLongDesc)
{ }

DPO_CreateXYArray& DPO_CreateXYArray::operator= (const DPO_CreateXYArray& a)
{
    if (&a != this)
       *((DFO_CreateXYArray*) this) = a;
    return *this;
}

bool  DPO_CreateXYArray::ReadFromFile()
{
    ReadObjectHeader();

    int nSel = ReadInt();
    int i;
    for (i = 0; i < nSel; i++)
    {
        xyData[i].doXY   = true;
        ReadFuncObjRef(xyData[i].xyObjRef);
        ReadText(xyData[i].newID, DC_XYData::dataIDLen);
    }

    for (i = nSel; i < DFO_CreateXYArray::maxXYDataDesc; i++)
        xyData[i] = XYDataDesc();


    CheckForInputFailure("Reading DPO_CreateXYArray");
    return true;
}

void DPO_CreateXYArray::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    int nSel = 0;
    int i;
    for (i = 0; i < DFO_CreateXYArray::maxXYDataDesc; i++)
        if (xyData[i].doXY)
            nSel++;

    WriteInt(nSel);
    for (i = 0; i < DFO_CreateXYArray::maxXYDataDesc; i++)
        if (xyData[i].doXY)
        {
            WriteFuncObjRef(xyData[i].xyObjRef);
            WriteText(xyData[i].newID);
            WriteLine();
        }
    WriteLine();
}


#ifndef ADCONSOLEAPP

DPO_CreateXYArrayPS& DPO_CreateXYArray::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CreateXYArrayPS&>(*assocMenu);
}

void DPO_CreateXYArray::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_CreateXYArray::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CreateXYArrayPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_CreateXYArray::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}
#endif //!ADCONSOLEAPP

