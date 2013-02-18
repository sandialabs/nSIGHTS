///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ChooseColorMap.cpp
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

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_ChooseColorMapPS.h"
#endif
#include "DPO_ChooseColorMap.h"

static const char* objLongDesc = "Choose Color Map";

DPO_ChooseColorMap :: DPO_ChooseColorMap() :
    MenuObjC(this, objLongDesc)
{
}

DPO_ChooseColorMap :: DPO_ChooseColorMap(const DPO_ChooseColorMap& a) :
    DFO_ChooseColorMap(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_ChooseColorMap& DPO_ChooseColorMap::operator= (const DPO_ChooseColorMap& a)
{
    if (&a != this)
       *((DFO_ChooseColorMap*) this) = a;
    return *this;
}


bool  DPO_ChooseColorMap::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(inputColorMapObjRef);
    CheckForInputFailure("Reading DPO_ChooseColorMap");
    return true;
}


void DPO_ChooseColorMap::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteFuncObjRef(inputColorMapObjRef);
}

#ifndef ADCONSOLEAPP

DPO_ChooseColorMapPS& DPO_ChooseColorMap::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ChooseColorMapPS&>(*assocMenu);
}


void DPO_ChooseColorMap::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ChooseColorMap::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ChooseColorMapPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_ChooseColorMap::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif

