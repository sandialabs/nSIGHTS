///////////////////////////////////////////////////////////////////////////////////
//
// DPO_MergeColorMap.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              File I/O for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#ifndef ADCONSOLEAPP
#include "DPO_MergeColorMapPS.h"
#endif

#include <genApp/AppConfigFile.h>

#include "DPO_MergeColorMap.h"

// **************************************** constructor/destructor

static const char* objLongDesc = "Merge Color Maps";

DPO_MergeColorMap :: DPO_MergeColorMap() :
    MenuObjC(this, objLongDesc)
{
}

DPO_MergeColorMap :: DPO_MergeColorMap(const DPO_MergeColorMap& a) :
    DFO_MergeColorMap(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_MergeColorMap& DPO_MergeColorMap::operator= (const DPO_MergeColorMap& a)
{
    if (&a != this)
        *((DFO_MergeColorMap*) this) = a;
    return *this;
}

bool  DPO_MergeColorMap::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(input1ObjRef);
    ReadFuncObjRef(input2ObjRef);

    CheckForInputFailure("Reading DPO_MergeColorMap");
    return true;
}

void DPO_MergeColorMap::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;
    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(input1ObjRef);
    WriteFuncObjRef(input2ObjRef);
}

#ifndef ADCONSOLEAPP

DPO_MergeColorMapPS& DPO_MergeColorMap::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_MergeColorMapPS&>(*assocMenu);
}

void DPO_MergeColorMap::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_MergeColorMap::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_MergeColorMapPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_MergeColorMap::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif

