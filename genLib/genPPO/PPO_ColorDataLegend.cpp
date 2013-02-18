///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ColorDataLegend.cpp
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "genPPOConfigFile.h"
#include "PPO_ColorDataLegendPS.h"
#include "PPO_ColorDataLegend.h"

static const char* objLongDesc = "Discrete Colors and Labels";

PPO_ColorDataLegend :: PPO_ColorDataLegend(PlotDefC* assPlt) :
    PFO_ColorDataLegendGL(*assPlt),
    PPO_AnnoBase(this, objLongDesc)
{
}

PPO_ColorDataLegend :: PPO_ColorDataLegend(PlotDefC* assPlt, const PPO_ColorDataLegend& a) :
    PFO_ColorDataLegendGL (a, *assPlt),
    PPO_AnnoBase(this, objLongDesc)
{
}

PPO_ColorDataLegend& PPO_ColorDataLegend::operator= (const PPO_ColorDataLegend& a)
{
    if (&a != this)
       *((PFO_ColorDataLegend*) this) = a;
    return *this;
}

// ************************* menu ops
PPO_ColorDataLegendPS& PPO_ColorDataLegend::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_ColorDataLegendPS&>(*assocMenu);
}

void PPO_ColorDataLegend::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_ColorDataLegend::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_ColorDataLegendPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_ColorDataLegend::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

void PPO_ColorDataLegend::UpdateULUI()
{
    if (UpdateOK())
        MenuRef().UpdateULUI(*this);
}


bool  PPO_ColorDataLegend::ReadFromFile()
{
    ReadObjectHeader();
    PPO_AnnoBase::ReadFromFile();

    ReadFuncObjRef(penSetObjRef);
    ReadAnnoSpec(*this);
    ReadColorLegendSpec(*this);

    ReadFuncObjRef(labelArrayObjRef);
    reductionFactor = ReadDouble();

    if (currentObjReadMajorVersion > 0)
    {
        ReadFuncObjRef(selectionObjRef);
        frameSelection = ReadBool();
    }

    CheckForInputFailure("Reading PPO_ColorDataLegend");
    return true;
}



void PPO_ColorDataLegend::WriteToFile()
{
    const int objMajVer = 1; // added frame selection
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_AnnoBase::WriteToFile();
    WriteFuncObjRef(penSetObjRef);

    WriteAnnoSpec(*this);
    WriteColorLegendSpec(*this);

    WriteFuncObjRef(labelArrayObjRef);
    WriteDouble(reductionFactor);
    WriteLine();

    WriteFuncObjRef(selectionObjRef);
    WriteBool(frameSelection);
    WriteLine();
}

