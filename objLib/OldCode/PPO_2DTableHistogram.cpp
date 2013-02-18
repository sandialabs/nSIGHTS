///////////////////////////////////////////////////////////////////////////////////
//
// PPO_2DTableHistogram.cpp
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genPPO/genPPOConfigFile.h>

#include "PPO_2DTableHistogramPS.h"
#include "PPO_2DTableHistogram.h"

const char* PPO_2DTableHistogram::objLongDesc = "Table Histogram";

PPO_2DTableHistogram :: PPO_2DTableHistogram(PlotDefC* assPlt) :
    PFO_2DTableHistogramGL(*assPlt),
    PPO_Base(this, objLongDesc)
{ }

PPO_2DTableHistogram :: PPO_2DTableHistogram(PlotDefC* assPlt, const PPO_2DTableHistogram& a) :
    PFO_2DTableHistogramGL (a, *assPlt),
    PPO_Base(this, objLongDesc)
{ }

PPO_2DTableHistogram& PPO_2DTableHistogram::operator= (const PPO_2DTableHistogram& a)
{
    if (&a != this)
       *((PFO_2DTableHistogram*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_2DTableHistogramPS& PPO_2DTableHistogram::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_2DTableHistogramPS&>(*assocMenu);
}

void PPO_2DTableHistogram::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void PPO_2DTableHistogram::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_2DTableHistogramPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_2DTableHistogram::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_2DTableHistogram::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);
    ReadFuncObjRef(tableDataObjRef);

    for (int i = 0; i < PFO_2DTableHistogram::maxTableHistogram; i++)
    {
        seriesData[i].doHistogram         = ReadBool();
        seriesData[i].tableColIndx     = ReadInt();

        ReadSeriesSpec(seriesData[i]);
        ReadLegendOverride(seriesData[i]);
    }

    xDataIndx     = ReadInt();

    if (currentObjReadMajorVersion > 0)
    {
        plotAllRows = ReadBool();
        lastNRows = ReadInt();
    }

    CheckForInputFailure("Reading PPO_2DTableHistogram");
    return true;
}

void PPO_2DTableHistogram::WriteToFile()
{
    const int objMajVer = 1;    // added plotAllRows
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(penSetObjRef);
    WriteFuncObjRef(tableDataObjRef);

    for ( int i = 0; i < PFO_2DTableHistogram::maxTableHistogram; i++)
    {
        WriteBool(seriesData[i].doHistogram     );
        WriteInt(seriesData[i].tableColIndx  );

        WriteSeriesSpec(seriesData[i]);
        WriteLegendOverride(seriesData[i]);
    }

    WriteInt(xDataIndx);
    WriteBool(plotAllRows);
    WriteInt(lastNRows);

    WriteLine();
}

