///////////////////////////////////////////////////////////////////////////////////
//
// PPD_2DXY.cpp
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
//      derived from PPD_2D. Adds components for 2DXY plots
//
//      derived from PPD_Base.  Adds components for all 2D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "PPD_2DXYPS.h"
#include "PPD_2DXY.h"

const char* PPD_2DXY::objLongDesc = "2D XY Plot";
const char* PPD_2DXY::allocID = "PPD_2DXY";

PPD_2DXY :: PPD_2DXY() : PPD_2D(this, objLongDesc)
{
    AutoObjectSetup(allocID);
}
PPD_2DXY :: PPD_2DXY(const PPD_2DXY& a) : PD_2DXYPS(a), PPD_2D(this, objLongDesc)
{
    AutoObjectSetup(allocID);
}

PPD_2DXY::~PPD_2DXY()
{

}

// ************************* menu ops


PPD_2DXYPS& PPD_2DXY::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPD_2DXYPS&>(*assocMenu);
}


void PPD_2DXY::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPD_2DXY::PropOpenMenu()
{
    PPD_Base::PropOpenMenu();
    assocMenu = new PPD_2DXYPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPD_2DXY::PropApply()
{
    MenuRef().GetMenuData(*this);
    PPD_Base::PropApply();
}


PPD_2DXY& PPD_2DXY::operator= (const PPD_2DXY& a)
{
    if (&a != this)
       *((PD_2DXYPS*) this) = a;
    return *this;
}


bool  PPD_2DXY::ReadFromFile()
{
    ReadDefaultObjectHeader();
    PPD_2D::ReadFromFile();
    CheckForInputFailure("Reading PPD_2DXY");
    return true;
}


void PPD_2DXY::WriteToFile()
{
    const int objMajVer = 1;    // added set axes
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPD_2D::WriteToFile();
}
