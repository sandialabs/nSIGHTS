///////////////////////////////////////////////////////////////////////////////////
//
// DPO_EnterColorMap.cpp
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
#include "DPO_EnterColorMapPS.h"
#endif

#include <genApp/AppConfigFile.h>

#include "DPO_EnterColorMap.h"

// **************************************** constructor/destructor

const char* DPO_EnterColorMap::objLongDesc = "Enter Color Map";

DPO_EnterColorMap :: DPO_EnterColorMap() :
    DFO_EnterColorMap(),
    MenuObjC(this, objLongDesc)
{
}


DPO_EnterColorMap :: DPO_EnterColorMap(const DPO_EnterColorMap& a) :
    DFO_EnterColorMap(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_EnterColorMap& DPO_EnterColorMap::operator= (const DPO_EnterColorMap& a)
{
    if (&a != this)
        *(static_cast<DFO_EnterColorMap*>(this)) = a;
    return *this;
}

bool  DPO_EnterColorMap::ReadFromFile()
{
    ReadObjectHeader();

    useHSVspec  = ReadBool();

    enterColorMap.ncolorMap = ReadInt();
    for (int i = 0; i < enterColorMap.ncolorMap; i++)
        {
            enterColorMap.colorMapColors[i].RH = ReadDouble();
            enterColorMap.colorMapColors[i].GS = ReadDouble();
            enterColorMap.colorMapColors[i].BV = ReadDouble();
        }

    CheckForInputFailure("Reading DPO_EnterColorMap");
    return true;
}

void DPO_EnterColorMap::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;
    WriteObjectHeader(objMajVer, objMinVer);

    WriteBool(useHSVspec);
    WriteInt(enterColorMap.ncolorMap);
    WriteLine();

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
    for (int i = 0; i < enterColorMap.ncolorMap; i++)
        {
            WriteDouble(enterColorMap.colorMapColors[i].RH);
            WriteDouble(enterColorMap.colorMapColors[i].GS);
            WriteDouble(enterColorMap.colorMapColors[i].BV);
            WriteLine();
        }

}

#ifndef ADCONSOLEAPP

// ************************* menu stuff

DPO_EnterColorMapPS& DPO_EnterColorMap::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_EnterColorMapPS&>(*assocMenu);
}


void DPO_EnterColorMap::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_EnterColorMap::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_EnterColorMapPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_EnterColorMap::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif

