///////////////////////////////////////////////////////////////////////////////////
//
// DPO_GridTile.cpp
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

#ifndef ADCONSOLEAPP
#include "DPO_GridTilePS.h"
#endif
#include "DPO_GridTile.h"

static const char* objLongDesc = "Tile Multiple Grids";


DPO_GridTile::DPO_GridTile() :
    MenuObjC(this, objLongDesc)
{

}

DPO_GridTile :: DPO_GridTile(const DPO_GridTile& a) :
    MenuObjC(this, objLongDesc),
    DFO_GridTile(a)
{
}


DPO_GridTile& DPO_GridTile::operator= (const DPO_GridTile& a)
{
    if (&a != this)
       *((DFO_GridTile*) this) = a;
    return *this;
}


bool  DPO_GridTile::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(baseGridObjRef);

    logInterpolate  = ReadBool();
    dropoutFixCount = ReadInt();

    int ngrid  = ReadInt();
    for (int i = 0; i < ngrid; i++)
    {
        gridsToAdd[i].doGrid   = true;
        ReadFuncObjRef(gridsToAdd[i].gridObjRef);
    }

    for (int i = ngrid; i < DFO_GridTile::maxInputGrid; i++)
        gridsToAdd[i] = DFO_GridTile::GridInputData();

    CheckForInputFailure("Reading DPO_GridTile");
    return true;
}

void DPO_GridTile::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(baseGridObjRef);

    WriteBool(logInterpolate);
    WriteInt(dropoutFixCount);
    WriteLine();

    int nSel = 0;
    for (int i = 0; i < DFO_GridTile::maxInputGrid; i++)
        if (gridsToAdd[i].doGrid)
            nSel++;

    WriteInt(nSel);
    WriteLine();

    for (int i = 0; i < DFO_GridTile::maxInputGrid; i++)
        if (gridsToAdd[i].doGrid)
        {
            WriteFuncObjRef(gridsToAdd[i].gridObjRef);
        }

}

#ifndef ADCONSOLEAPP

DPO_GridTilePS& DPO_GridTile::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_GridTilePS&>(*assocMenu);
}

void DPO_GridTile::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_GridTile::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_GridTilePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_GridTile::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


#endif //!ADCONSOLEAPP

