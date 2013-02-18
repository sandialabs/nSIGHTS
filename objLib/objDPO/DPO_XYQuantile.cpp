///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYQuantile.cpp
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

#include "DPO_XYQuantile.h"
#ifndef ADCONSOLEAPP
#include "DPO_XYQuantilePS.h"
#endif

static const char* objLongDesc = "XY CDF";

DPO_XYQuantile :: DPO_XYQuantile() :
    DPO_Quantile(this, objLongDesc)
{
}

DPO_XYQuantile :: DPO_XYQuantile(const DPO_XYQuantile& a) :
    DFO_XYQuantile(a),
    DPO_Quantile(this, objLongDesc)
{
}

DPO_XYQuantile& DPO_XYQuantile::operator= (const DPO_XYQuantile& a)
{
    if (&a != this)
       *((DFO_XYQuantile*) this) = a;
    return *this;
}


bool  DPO_XYQuantile::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(xyInputObjRef);
    operateOnX = ReadBool();

    DPO_Quantile::ReadFromFile();

    CheckForInputFailure("Reading DPO_XYQuantile");
    return true;
}

void DPO_XYQuantile::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(xyInputObjRef);
    WriteBool(operateOnX);
    WriteLine();

    DPO_Quantile::WriteToFile();
    WriteLine();
}

#ifndef ADCONSOLEAPP

void DPO_XYQuantile::CalcOutput(FOcalcType  calcType)
{
    DFO_XYQuantile::CalcOutput(calcType);
    if (MenuUpdateOK())
        PropUpdate();
}

DPO_XYQuantilePS& DPO_XYQuantile::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_XYQuantilePS&>(*assocMenu);
}

void DPO_XYQuantile::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_XYQuantilePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_XYQuantile::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_XYQuantile::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

