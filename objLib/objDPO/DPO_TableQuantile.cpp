///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableQuantile.cpp
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

#include <objDPO/objDPOConfigFile.h>

#include "DPO_TableQuantile.h"

#ifndef ADCONSOLEAPP
#include "DPO_TableQuantilePS.h"
#endif

static const char* objLongDesc = "Calculate Table Column CDF";

DPO_TableQuantile :: DPO_TableQuantile() :
    DPO_Quantile(this, objLongDesc)
{
}

DPO_TableQuantile :: DPO_TableQuantile(const DPO_TableQuantile& a) :
    DFO_TableQuantile(a),
    DPO_Quantile(this, objLongDesc)
{
}

DPO_TableQuantile& DPO_TableQuantile::operator= (const DPO_TableQuantile& a)
{
    if (&a != this)
       *((DFO_TableQuantile*) this) = a;
    return *this;
}


bool  DPO_TableQuantile::ReadFromFile()
{
    ReadObjectHeader();
    ReadTableColSel(*this);
    if (currentObjReadMajorVersion > 0)
    {
        areaWeighted = ReadBool();
        areaColumn = ReadInt();
    }

    DPO_Quantile::ReadFromFile();

    CheckForInputFailure("Reading DPO_TableQuantile");
    return true;
}

void DPO_TableQuantile::WriteToFile()
{
    const int objMajVer = 1;  // add area weighted
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteTableColSel(*this);

    WriteBool(areaWeighted);
    WriteInt(areaColumn);

    DPO_Quantile::WriteToFile();
}

#ifndef ADCONSOLEAPP

void DPO_TableQuantile::CalcOutput(FOcalcType  calcType)
{
    DFO_TableQuantile::CalcOutput(calcType);
    if (MenuUpdateOK())
        PropUpdate();
}

DPO_TableQuantilePS& DPO_TableQuantile::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_TableQuantilePS&>(*assocMenu);
}

void DPO_TableQuantile::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_TableQuantilePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_TableQuantile::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_TableQuantile::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

