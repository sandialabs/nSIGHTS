///////////////////////////////////////////////////////////////////////////////////
//
// DPO_LinColorMap.cpp
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

#ifndef ADCONSOLEAPP
#include "GenLibRes.h"
#include "DPO_LinColorMapPS.h"
#endif

#include <genApp/AppConfigFile.h>

#include "DPO_LinColorMap.h"

// **************************************** constructor/destructor

const char* DPO_LinColorMap::objLongDesc = "Create Linear Color Map";

DPO_LinColorMap :: DPO_LinColorMap() :
    DFO_LinColorMap(),
    MenuObjC(this, objLongDesc)
{
}


DPO_LinColorMap :: DPO_LinColorMap(const DPO_LinColorMap& a) :
    DFO_LinColorMap(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_LinColorMap& DPO_LinColorMap::operator= (const DPO_LinColorMap& a)
{
    if (&a != this)
        *(static_cast<DFO_LinColorMap*>(this)) = a;
    return *this;
}

bool  DPO_LinColorMap::ReadFromFile()
{
    ReadObjectHeader();

    useHSVspec  = ReadBool();
    nmapShades  = ReadInt();

    mapStart.RH = ReadDouble();
    mapStart.GS = ReadDouble();
    mapStart.BV = ReadDouble();

    mapEnd.RH   = ReadDouble();
    mapEnd.GS   = ReadDouble();
    mapEnd.BV   = ReadDouble();

    CheckForInputFailure("Reading DPO_LinColorMap");
    return true;
}

void DPO_LinColorMap::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;
    WriteObjectHeader(objMajVer, objMinVer);

    WriteBool(useHSVspec);
    WriteInt(nmapShades);
    WriteLine();

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));

    WriteDouble(mapStart.RH);
    WriteDouble(mapStart.GS);
    WriteDouble(mapStart.BV);

    WriteDouble(mapEnd.RH);
    WriteDouble(mapEnd.GS);
    WriteDouble(mapEnd.BV);
    WriteLine();
}

#ifndef ADCONSOLEAPP

// ************************* menu stuff

DPO_LinColorMapPS& DPO_LinColorMap::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_LinColorMapPS&>(*assocMenu);
}


void DPO_LinColorMap::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_LinColorMap::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_LinColorMapPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(true, true);

    PropUpdate();
}


void DPO_LinColorMap::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif

