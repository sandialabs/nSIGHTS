///////////////////////////////////////////////////////////////////////////////////
//
// PPD_CompositeLayout.cpp
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
//      describes layout of sub-plots on composite plot.
//
//      adds components for composite plots.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "PPD_CompositeLayoutPS.h"
#include "PPD_CompositeLayout.h"

const char* PPD_CompositeLayout::objType        = "CompositeLayout";
const char* PPD_CompositeLayout::objLongDesc    = "Composite Plot Layout";
const char* PPD_CompositeLayout::allocID        = "PPD_CompositeLayout";

PPD_CompositeLayout :: PPD_CompositeLayout(PD_Composite& inPD)
    : MenuObjC(objID, objType, objLongDesc, objType), compPD(inPD)
{
    AutoObjectSetup(allocID);
}


PPD_CompositeLayoutPS& PPD_CompositeLayout::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPD_CompositeLayoutPS&>(*assocMenu);
}


void PPD_CompositeLayout::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(compPD, *this);
}


void PPD_CompositeLayout::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();

    assocMenu = new PPD_CompositeLayoutPS();
    MenuRef().InitMenu(compPD);
    MenuRef().MenuOpen(compPD);

    PropUpdate();
}


void PPD_CompositeLayout::PropApply()
{
    MenuRef().GetMenuData(compPD, *this);
    MenuObjC::PropApply();
    PropApplyAssoc(&compPD);
}

void PPD_CompositeLayout::PropClear()
{
    PropUpdate();
}



bool  PPD_CompositeLayout::ReadFromFile()
{
    ReadDefaultObjectHeader();

    compPD.nlayoutRows = ReadInt();
    compPD.nlayoutCols = ReadInt();

    int nread  = ReadInt();
    for (int i = 0; i < nread; i++)
    {
        compPD.rowSpacing[i] = ReadDouble();
        compPD.colSpacing[i] = ReadDouble();
    }

    if (currentObjReadMajorVersion > 0)
    {
        nread  = ReadInt();
        for (int i = 0; i < nread; i++)
        {
            compPD.useCustomLayout[i] = ReadBool();
            compPD.customLayout[i].xOffset = ReadDouble();
            compPD.customLayout[i].yOffset = ReadDouble();
            compPD.customLayout[i].xWidth = ReadDouble();
            compPD.customLayout[i].yWidth = ReadDouble();
        }
    }
    CheckForInputFailure("Reading PPD_CompositeLayout");
    return true;
}


void PPD_CompositeLayout::WriteToFile()
{
    const int objMajVer = 1;  // added free form layout
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteInt(compPD.nlayoutRows);
    WriteInt(compPD.nlayoutCols);
    WriteLine();

    WriteInt(compPD.maxLayoutRowCol);
    WriteLine();
    for (int i = 0; i < compPD.maxLayoutRowCol; i++)
    {
        WriteDouble(compPD.rowSpacing[i]);
        WriteDouble(compPD.colSpacing[i]);
    }
    WriteLine();

    WriteInt(PD_Composite::maxCustomLayout);
    for (int i = 0; i < PD_Composite::maxCustomLayout; i++)
    {
        WriteBool(compPD.useCustomLayout[i]);
        WriteDouble(compPD.customLayout[i].xOffset);
        WriteDouble(compPD.customLayout[i].yOffset);
        WriteDouble(compPD.customLayout[i].xWidth);
        WriteDouble(compPD.customLayout[i].yWidth);
        WriteLine();
    }


}

