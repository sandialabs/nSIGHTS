///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CubeToIndex.cpp
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

#include "DPO_CubeToIndex.h"
#ifndef ADCONSOLEAPP
#include "DPO_CubeToIndexPS.h"
#endif

const char* DPO_CubeToIndex::objLongDesc = "Extract Indexes of Cube Data";

DPO_CubeToIndex :: DPO_CubeToIndex() :
    DPO_RangeExtract(this, objLongDesc)
{
}

DPO_CubeToIndex :: DPO_CubeToIndex(const DPO_CubeToIndex& a) :
    DFO_CubeToIndex(a),
    DPO_RangeExtract(this, objLongDesc)
{
}


DPO_CubeToIndex& DPO_CubeToIndex::operator= (const DPO_CubeToIndex& a)
{
    if (&a != this)
       *((DFO_CubeToIndex*) this) = a;
    return *this;
}


bool  DPO_CubeToIndex::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(cubeObjRef);

    DPO_RangeExtract::ReadFromFile(false);

    CheckForInputFailure("Reading DPO_CubeToIndex");
    return true;
}


void DPO_CubeToIndex::WriteToFile()
{
    const int objMajVer = 0;            // to range extract base
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteFuncObjRef(cubeObjRef);

    DPO_RangeExtract::WriteToFile();

}

#ifndef ADCONSOLEAPP

DPO_CubeToIndexPS& DPO_CubeToIndex::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CubeToIndexPS&>(*assocMenu);
}


void DPO_CubeToIndex::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_CubeToIndex::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CubeToIndexPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_CubeToIndex::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_CubeToIndex::CalcOutput(FOcalcType  calcType)
{
    DFO_CubeToIndex::CalcOutput(calcType);
    if (MenuUpdateOK())
        PropUpdate();
}

#endif //!ADCONSOLEAPP

