///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ShadeRegion.cpp
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

#include <genPPO/genPPOConfigFile.h>

#include "PPO_ShadeRegionPS.h"
#include "PPO_ShadeRegion.h"

const char* PPO_ShadeRegion::objLongDesc = "Shade 2D Region";

PPO_ShadeRegion :: PPO_ShadeRegion(PlotDefC* assPlt)
                   : PFO_ShadeRegionGL(*assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_ShadeRegion :: PPO_ShadeRegion(PlotDefC* assPlt, const PPO_ShadeRegion& a)
                            : PFO_ShadeRegionGL (a, *assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_ShadeRegion& PPO_ShadeRegion::operator= (const PPO_ShadeRegion& a)
{
    if (&a != this)
        PFO_ShadeRegionGL::operator=(a);
    return *this;
}

// ************************* menu ops

PPO_ShadeRegionPS& PPO_ShadeRegion::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_ShadeRegionPS&>(*assocMenu);
}


void PPO_ShadeRegion::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_ShadeRegion::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_ShadeRegionPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_ShadeRegion::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


bool  PPO_ShadeRegion::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();

    ReadFuncObjRef(penSetObjRef);
    ReadFuncObjRef(colorMapObjRef);

    ReadRealMSSpec(regionXMin);
    ReadRealMSSpec(regionXMax);
    ReadRealMSSpec(regionYMin);
    ReadRealMSSpec(regionYMax);

    xRegionSpec = PFO_ShadeRegion::RegionSpecType(ReadInt());
    yRegionSpec = PFO_ShadeRegion::RegionSpecType(ReadInt());

    if (currentObjReadMajorVersion > 0)
        shadeRegion = ReadBool();
    shadeIsColorMap = ReadBool();
    shadeCMIndex = ReadInt();
    shadePen = ReadInt();

    labelRegion = ReadBool();
    labelPen = ReadInt();
    labelXPosition = ReadDouble();  // 0 to 100 along X limits
    labelYPosition = ReadDouble();  // 0 to 100 along X limits
    labelOffset = ReadInt();
    halign = HorizontalTextAlignment(ReadInt());
    valign = VerticalTextAlignment(ReadInt());

    ReadFont(labelFont);
    ReadText(regionLabel, maxLabelLen);

    if (currentObjReadMajorVersion > 0)
    {
        stippleRegion = ReadBool();
        ReadStippleSpec(regionStipple);
    }
    if (currentObjReadMajorVersion > 1)
    {
        frameRegion = ReadBool();
        ReadLineSpec(frameLine);
    }

    CheckForInputFailure("Reading PPO_ShadeRegion");
    return true;
}

void PPO_ShadeRegion::WriteToFile()
{
    const int objMajVer = 2;  // added frame
//    const int objMajVer = 1;  // added stipple
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();
    WriteFuncObjRef(penSetObjRef);
    WriteFuncObjRef(colorMapObjRef);

    WriteRealMSSpec(regionXMin);
    WriteRealMSSpec(regionXMax);
    WriteRealMSSpec(regionYMin);
    WriteRealMSSpec(regionYMax);

    WriteInt(int(xRegionSpec));
    WriteInt(int(yRegionSpec));

    WriteBool(shadeRegion);     // V1
    WriteBool(shadeIsColorMap);
    WriteInt(shadeCMIndex);
    WriteInt(shadePen);
    WriteLine();

    WriteBool(labelRegion);
    WriteInt(labelPen);
    WriteDouble(labelXPosition);  // 0 to 100 along X limits
    WriteDouble(labelYPosition);  // 0 to 100 along X limits
    WriteInt(labelOffset);
    WriteInt(int(halign));
    WriteInt(int(valign));
    WriteLine();

    WriteFont(labelFont);
    WriteText(regionLabel);
    WriteLine();

    // added v1
    WriteBool(stippleRegion);
    WriteStippleSpec(regionStipple);
    WriteLine();

    // added v2
    WriteBool(frameRegion);
    WriteLineSpec(frameLine);
    WriteLine();
}

