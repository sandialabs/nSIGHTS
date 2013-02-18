///////////////////////////////////////////////////////////////////////////////////
//
// PPO_SeriesLegend.cpp
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
#include "PPO_SeriesLegend.h"
#include "PPO_SeriesLegendPS.h"

static const char* objLongDesc = "Line and Symbol Legend";

PPO_SeriesLegend :: PPO_SeriesLegend(PlotDefC* assPlt) :
                    PPO_AnnoBase(this, objLongDesc), PFO_SeriesLegendGL(*assPlt)
{ }

PPO_SeriesLegend :: PPO_SeriesLegend(PlotDefC* assPlt, const PPO_SeriesLegend& a):
                            PPO_AnnoBase(this, objLongDesc), PFO_SeriesLegendGL (a, *assPlt)
{ }

PPO_SeriesLegend& PPO_SeriesLegend::operator= (const PPO_SeriesLegend& a)
{
    if (&a != this)
       *((PFO_SeriesLegend*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_SeriesLegendPS& PPO_SeriesLegend::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_SeriesLegendPS&>(*assocMenu);
}

void PPO_SeriesLegend::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void  PPO_SeriesLegend::UpdateULUI()
{
    if (UpdateOK())
        MenuRef().UpdateULUI(*this);
}

void PPO_SeriesLegend::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

void PPO_SeriesLegend::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_SeriesLegendPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


bool  PPO_SeriesLegend::ReadFromFile()
{
    ReadObjectHeader();
    PPO_AnnoBase::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);

    int nRead = 6; // old max
    if (currentObjReadMajorVersion > 0)
        nRead = ReadInt();

    for ( int i = 0; i < nRead; i++)
    {
        ReadFuncObjRef(legendData[i].seriesObjRef);
        legendData[i].doSeries   = ReadBool();
    }

    ulLocation.pX = ReadDouble();
    ulLocation.pY = ReadDouble();

    seriesSize = ReadDouble();
    seriesNpts = ReadInt();

    ReadText(legendTitle, PSC_AnnoSpec::legendTitleLen);

    frameLegend       = ReadBool();
    frameThick        = ReadInt();
    opaqueBackground  = ReadBool();
    legendPen         = ReadInt();

    ReadFont(legendFont);

    CheckForInputFailure("Reading PPO_SeriesLegend");
    return true;
}


void PPO_SeriesLegend::WriteToFile()
{
    const int objMajVer = 1; // upped max to 12 from 6
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_AnnoBase::WriteToFile();

    WriteFuncObjRef(penSetObjRef);

    WriteInt(PFO_SeriesLegend::maxSeriesLegend);
    WriteLine();
    for ( int i = 0; i < PFO_SeriesLegend::maxSeriesLegend; i++)
    {
        WriteFuncObjRef(legendData[i].seriesObjRef);
        WriteBool(legendData[i].doSeries);
        WriteLine();
    }

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
    WriteDouble(ulLocation.pX);
    WriteDouble(ulLocation.pY);
    WriteLine();

    WriteDouble(seriesSize);
    WriteInt(seriesNpts);
    WriteLine();

    WriteText(legendTitle);

    WriteBool(frameLegend);
    WriteInt (frameThick);
    WriteBool(opaqueBackground);
    WriteInt (legendPen);
    WriteLine();

    WriteFont(legendFont);
}

