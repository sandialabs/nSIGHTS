///////////////////////////////////////////////////////////////////////////////////
//
// PPD_Composite.cpp
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
//      adds components for composite plots.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "PPD_Composite.h"
#include "PPD_CompositePS.h"

const char* PPD_Composite::objLongDesc = "Composite Plot";
const char* PPD_Composite::allocID = "PPD_Composite";

PPD_Composite::PPD_Composite()
    :PPD_Base(this, objLongDesc)
{
    AutoObjectSetup(allocID);
}

PPD_Composite::PPD_Composite(const PPD_Composite& a)
    :PD_CompositePS(a),PPD_Base(this, objLongDesc)
{
    AutoObjectSetup(allocID);
}

PPD_Composite::~PPD_Composite()
{
}

// ************************* menu ops

PPD_CompositePS& PPD_Composite::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPD_CompositePS&>(*assocMenu);
}

void PPD_Composite::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void PPD_Composite::PropOpenMenu()
{
    PPD_Base::PropOpenMenu();
    assocMenu = new PPD_CompositePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPD_Composite::PropApply()
{
    MenuRef().GetMenuData(*this);
    PPD_Base::PropApply();
}


PPD_Composite& PPD_Composite::operator= (const PPD_Composite& a)
{
    if (&a != this)
       *((PD_CompositePS*) this) = a;
    return *this;
}


bool  PPD_Composite::ReadFromFile()
{
    ReadDefaultObjectHeader();

    PPD_Base::ReadFromFile();

    autoLayout  = ReadBool();
    showLayout  = ReadBool();
    layoutPen   = ReadInt();

    int nsub = ReadInt();
    subPlots.AllocAndSetSize(nsub);
    for (int i = 0; i < nsub; i++)
    {
        FuncObjRef temp;
        ReadFuncObjRef(temp);
        subPlots[i] = static_cast<PlotDefC*>(temp.objRef);
    }


    CheckForInputFailure("Reading PPD_Composite");
    return true;
}


void PPD_Composite::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    PPD_Base::WriteToFile();

    WriteBool(autoLayout);
    WriteBool(showLayout);
    WriteInt(layoutPen);
    WriteLine();

    WriteInt(subPlots.Size());
    for (int i = 0; i < subPlots.Size(); i++)
        WriteFuncObjRef(FuncObjRef(*(subPlots[i])));


}


