///////////////////////////////////////////////////////////////////////////////////
//
// PPO_SingleConfLimits.cpp
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
//      File I/O and user interface for all osPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genPPO/genPPOConfigFile.h>

#include "PPO_SingleConfLimitsPS.h"
#include "PPO_SingleConfLimits.h"

const char* PPO_SingleConfLimits::objLongDesc = "Covariance Matrix Confidence Limits - Multiple Single Parameter";

PPO_SingleConfLimits :: PPO_SingleConfLimits(PlotDefC* assPlt)
                   : PFO_SingleConfLimitsGL(*assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_SingleConfLimits :: PPO_SingleConfLimits(PlotDefC* assPlt, const PPO_SingleConfLimits& a)
                            : PFO_SingleConfLimitsGL (a, *assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_SingleConfLimits& PPO_SingleConfLimits::operator= (const PPO_SingleConfLimits& a)
{
    if (&a != this)
       *((PFO_SingleConfLimits*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_SingleConfLimitsPS& PPO_SingleConfLimits::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_SingleConfLimitsPS&>(*assocMenu);
}


void PPO_SingleConfLimits::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_SingleConfLimits::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_SingleConfLimitsPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_SingleConfLimits::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


bool  PPO_SingleConfLimits::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    if (currentObjReadMajorVersion > 0)
        ReadColorBase(*this);

    ReadFuncObjRef(penSetObjRef);

    if (currentObjReadMajorVersion == 0)
    {
        ReadFuncObjRef(inputDataObjRef);
    }
    covarPen = ReadInt();

    if (currentObjReadMajorVersion == 0)
    {
        ReadFuncObjRef(colorMapObjRef);
    }
    plotWithColorMap = ReadBool();

    allParameters = ReadBool();
    xDataIndx = ReadInt();

    cLimits = DC_Covar::ConfidenceLimits(ReadInt());
    plotConfLimits = ReadBool();
    covarLineThk = ReadInt();

    plotBestEstimate = ReadBool();
    ReadSymbolSpec(bestEstSymbol);

    widthSpec = PFO_SingleConfLimits::WidthSpec(ReadInt());
    widthSpecValue = ReadDouble();
    widthSpecPixel = ReadInt();
    widthSpecPct = ReadDouble();

    ReadLegendOverride(legendOverride);

    CheckForInputFailure("Reading PPO_SingleConfLimits");
    return true;
}

void PPO_SingleConfLimits::WriteToFile()
{
    // ver 0
    const int objMajVer = 1;  //added color base
    //const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();
    WriteColorBase(*this);

    WriteFuncObjRef(penSetObjRef);
    //WriteFuncObjRef(covarArrayObjRef);
    WriteInt(covarPen);
    WriteLine();

    WriteBool(plotWithColorMap);
    WriteLine();

    WriteBool(allParameters);
    WriteInt(xDataIndx);
    WriteLine();

    WriteInt(int(cLimits));
    WriteBool(plotConfLimits);
    WriteInt(covarLineThk);
    WriteLine();

    WriteBool(plotBestEstimate);
    WriteSymbolSpec(bestEstSymbol);
    WriteLine();

    WriteInt(int(widthSpec));
    WriteDouble(widthSpecValue);
    WriteInt(widthSpecPixel);
    WriteDouble(widthSpecPct);
    WriteLine();

    WriteLegendOverride(legendOverride);

}

