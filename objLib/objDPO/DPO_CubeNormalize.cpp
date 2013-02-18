///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CubeNormalize.cpp
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

#include "DPO_CubeNormalize.h"

#ifndef ADCONSOLEAPP
#include "DPO_CubeNormalizePS.h"
#endif

static const char* objLongDesc = "Normalize Cube Data";

DPO_CubeNormalize :: DPO_CubeNormalize() :
    DPO_Normalize(this, objLongDesc)
{
}

DPO_CubeNormalize :: DPO_CubeNormalize(const DPO_CubeNormalize& a) :
    DFO_CubeNormalize(a),
    DPO_Normalize(this, objLongDesc)
{
}

DPO_CubeNormalize& DPO_CubeNormalize::operator= (const DPO_CubeNormalize& a)
{
    if (&a != this)
       *((DFO_CubeNormalize*) this) = a;
    return *this;
}


bool  DPO_CubeNormalize::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(cubeInputObjRef);

    DPO_Normalize::ReadFromFile();

    CheckForInputFailure("Reading DPO_CubeNormalize");
    return true;
}

void DPO_CubeNormalize::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(cubeInputObjRef);

    DPO_Normalize::WriteToFile();
    WriteLine();
}

#ifndef ADCONSOLEAPP

void DPO_CubeNormalize::CalcOutput(FOcalcType  calcType)
{
    DFO_CubeNormalize::CalcOutput(calcType);
    if (MenuUpdateOK())
        UpdateUI();
}

DPO_CubeNormalizePS& DPO_CubeNormalize::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CubeNormalizePS&>(*assocMenu);
}

void DPO_CubeNormalize::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CubeNormalizePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_CubeNormalize::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_CubeNormalize::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

