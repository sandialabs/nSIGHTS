///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DXYZ.cpp
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
//      derived from PPD_3D. Adds components for 3DXYZ plots
//
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "PPD_3DXYZPS.h"
#include "PPD_3DXYZ.h"

const char* PPD_3DXYZ::objLongDesc = "3D XYZ Plot";
const char* PPD_3DXYZ::allocID = "PPD_3DXYZ";

PPD_3DXYZ::PPD_3DXYZ() : PPD_3D(this, objLongDesc)
{
    AutoObjectSetup(allocID);
}
PPD_3DXYZ::PPD_3DXYZ(const PPD_3DXYZ& a) : PD_3DXYZPS(a), PPD_3D(this, objLongDesc)
{
    AutoObjectSetup(allocID);
}

PPD_3DXYZ::~PPD_3DXYZ() {}

PPD_3DXYZ& PPD_3DXYZ::operator= (const PPD_3DXYZ& a)
{
    if (&a != this)
       *((PD_3DXYZPS*) this) = a;
    return *this;
}


PPD_3DXYZPS& PPD_3DXYZ::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPD_3DXYZPS&>(*assocMenu);
}


void PPD_3DXYZ::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPD_3DXYZ::PropOpenMenu()
{
    PPD_Base::PropOpenMenu();
    assocMenu = new PPD_3DXYZPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPD_3DXYZ::PropApply()
{
    MenuRef().GetMenuData(*this);
    PPD_Base::PropApply();
}

bool  PPD_3DXYZ::ReadFromFile()
{
    ReadDefaultObjectHeader();
    PPD_3D::ReadFromFile();

    xyaxesLengthRatio = ReadDouble();
    xzaxesLengthRatio = ReadDouble();

    CheckForInputFailure("Reading PPD_3DXYZ");
    return true;
}

void PPD_3DXYZ::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPD_3D::WriteToFile();

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
    WriteDouble(xyaxesLengthRatio);
    WriteDouble(xzaxesLengthRatio);
    WriteLine();

}

