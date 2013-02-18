///////////////////////////////////////////////////////////////////////////////////
//
// PPO_2DXYData.cpp
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

#include "PPO_2DXYDataPS.h"
#include "PPO_2DXYData.h"

const char* PPO_2DXYData::objLongDesc = "Series from Multiple XY Data";

PPO_2DXYData :: PPO_2DXYData(PlotDefC* assPlt) :
    PFO_2DXYDataGL(*assPlt),
    PPO_Base(this, objLongDesc)
{ }

PPO_2DXYData :: PPO_2DXYData(PlotDefC* assPlt, const PPO_2DXYData& a) :
    PFO_2DXYDataGL (a, *assPlt),
    PPO_Base(this, objLongDesc)
{ }

PPO_2DXYData& PPO_2DXYData::operator= (const PPO_2DXYData& a)
{
    if (&a != this)
       *((PFO_2DXYData*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_2DXYDataPS& PPO_2DXYData::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_2DXYDataPS&>(*assocMenu);
}

void PPO_2DXYData::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void PPO_2DXYData::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_2DXYDataPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_2DXYData::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_2DXYData::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();

    ReadFuncObjRef(penSetObjRef);
    for (int i = 0; i < PFO_2DXYData::maxXYSeries; i++)
    {
        xySeries[i].doSeries         = ReadBool();
        ReadFuncObjRef(xySeries[i].xyDataObjRef);
        ReadSeriesSpec(xySeries[i]);
        ReadLegendOverride(xySeries[i]);
    }

    CheckForInputFailure("Reading PPO_2DXYData");
    return true;
}

void PPO_2DXYData::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(penSetObjRef);

    for ( int i = 0; i < PFO_2DXYData::maxXYSeries; i++)
    {
        WriteBool(xySeries[i].doSeries     );
        WriteFuncObjRef(xySeries[i].xyDataObjRef);
        WriteSeriesSpec(xySeries[i]);
        WriteLegendOverride(xySeries[i]);
    }
}


