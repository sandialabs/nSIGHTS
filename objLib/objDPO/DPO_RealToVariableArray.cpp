///////////////////////////////////////////////////////////////////////////////////
//
// DPO_RealToVariableArray.cpp
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
#include "DPO_RealToVariableArrayPS.h"
#endif
#include "DPO_RealToVariableArray.h"

const char* DPO_RealToVariableArray::objLongDesc = "Combine Reals in Variable Array";

DPO_RealToVariableArray :: DPO_RealToVariableArray() :
    MenuObjC(this, objLongDesc)
{
}

DPO_RealToVariableArray :: DPO_RealToVariableArray(const DPO_RealToVariableArray& a) :
    DFO_RealToVariableArray(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_RealToVariableArray& DPO_RealToVariableArray::operator= (const DPO_RealToVariableArray& a)
{
    if (&a != this)
    {
       *((DFO_RealToVariableArray*) this) = a;
    }
    return *this;
}

bool  DPO_RealToVariableArray::ReadFromFile()
{
    ReadObjectHeader();

    int nSel  = ReadInt();
    for (int i = 0; i < nSel; i++)
    {
        realSelData[i].doReal   = true;
        ReadFuncObjRef(realSelData[i].realObjRef);
        ReadText(realSelData[i].realID, DC_Variable::varIDLen);
    }

    for (int i = nSel; i < maxRealSel; i++)
        realSelData[i] = RealSel();

    CheckForInputFailure("Reading DPO_RealToVariableArray");
    return true;
}

void DPO_RealToVariableArray::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    int nSel = 0;
    for (int i = 0; i < maxRealSel; i++)
        if (realSelData[i].doReal)
            nSel++;

    WriteInt(nSel);
    WriteLine();
    for (int i = 0; i < maxRealSel; i++)
        if (realSelData[i].doReal)
        {
            WriteFuncObjRef(realSelData[i].realObjRef);
            WriteText(realSelData[i].realID);
        }

}

#ifndef ADCONSOLEAPP

DPO_RealToVariableArrayPS& DPO_RealToVariableArray::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_RealToVariableArrayPS&>(*assocMenu);
}

void DPO_RealToVariableArray::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_RealToVariableArray::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_RealToVariableArrayPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_RealToVariableArray::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

