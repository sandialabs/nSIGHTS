///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ColorLegend.cpp
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
#include "PPO_ColorLegendPS.h"
#include "PPO_ColorLegend.h"

static const char* objLongDesc = "Color Bar and Numeric Limits";

PPO_ColorLegend :: PPO_ColorLegend(PlotDefC* assPlt) :
    PFO_ColorLegendGL(*assPlt),
    PPO_AnnoBase(this, objLongDesc)
{
}

PPO_ColorLegend :: PPO_ColorLegend(PlotDefC* assPlt, const PPO_ColorLegend& a) :
    PFO_ColorLegendGL (a, *assPlt),
    PPO_AnnoBase(this, objLongDesc)
{
}

PPO_ColorLegend& PPO_ColorLegend::operator= (const PPO_ColorLegend& a)
{
    if (&a != this)
       *((PFO_ColorLegend*) this) = a;
    return *this;
}

// ************************* menu ops
PPO_ColorLegendPS& PPO_ColorLegend::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_ColorLegendPS&>(*assocMenu);
}

void PPO_ColorLegend::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_ColorLegend::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_ColorLegendPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_ColorLegend::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

void PPO_ColorLegend::UpdateULUI()
{
    if (UpdateOK())
        MenuRef().UpdateULUI(*this);
}


bool  PPO_ColorLegend::ReadFromFile()
{
    ReadObjectHeader();
    PPO_AnnoBase::ReadFromFile();

    if (currentObjReadMajorVersion > 2)
    {
        ReadFuncObjRef(penSetObjRef);
        ReadAnnoSpec(*this);
        ReadColorLegendSpec(*this);

        if (currentObjReadMajorVersion > 3)
        {
            specIncs      = ReadBool();
            if (currentObjReadMajorVersion > 4)
                useLabelX   = ReadBool();
            ReadFuncObjRef(labelArrayObjRef);
        }

        autoIncs      = ReadBool();
        if (currentObjReadMajorVersion > 4)
            includeSE   = ReadBool();

        nmanualIncs   = ReadInt();
        legendFormat.format             = NumConvFormat(ReadInt());
        legendFormat.digitsPrecision    = ReadInt();

    }
    else
    {
        // too intermixed to port cleanly ...

        ReadFuncObjRef(colorLegendObjRef);
        ReadFuncObjRef(penSetObjRef);

        ulLocation.pX = ReadDouble();
        ulLocation.pY = ReadDouble();
        barLength     = ReadDouble();
        barAspect     = ReadDouble();

        autoIncs      = ReadBool();
        nmanualIncs   = ReadInt();
        frameLegend   = ReadBool();
        ticsAtIncs    = ReadBool();
        dataTypeLabel = ReadBool();
        legendPen     = ReadInt();

        //  add ver1 stuff
        if (currentObjReadMajorVersion == 0)
        {
            frameThick = 2;
        }
        else
        {
            frameThick       = ReadInt();
            if (currentObjReadMajorVersion == 1)
            {
                bool horizontal       = ReadBool();
                if (horizontal)
                    orientation = PSC_Font::fr_Horiz;
                else
                    orientation = PSC_Font::fr_VertRight;
            }
            else
            {
                orientation = PSC_Font::FontRotation(ReadInt());
                dataLabelAboveLeft = ReadBool();
                incLabelAboveLeft = ReadBool();
                overrideLabel = ReadBool();
                ReadText(newLabel, stdMsgStrLen);
            }
            opaqueBackground = ReadBool();

            ReadFont(legendFont);

            legendFormat.format             = NumConvFormat(ReadInt());
            legendFormat.digitsPrecision    = ReadInt();
        }
    }

    CheckForInputFailure("Reading PPO_ColorLegend");
    return true;
}



void PPO_ColorLegend::WriteToFile()
{
    const int objMajVer = 5; //  added useLabelX && includeSE
//  const int objMajVer = 4; //  added specified labels

//  const int objMajVer = 3; //  went to new spec
//  const int objMajVer = 2; //  added orientation options
//  const int objMajVer = 1; //  added font and format in v1
    const int objMinVer = 0;


    WriteObjectHeader(objMajVer, objMinVer);
    PPO_AnnoBase::WriteToFile();
    WriteFuncObjRef(penSetObjRef);

    WriteAnnoSpec(*this);
    WriteColorLegendSpec(*this);

    WriteBool(specIncs);
    WriteBool(useLabelX);
    WriteFuncObjRef(labelArrayObjRef);
    WriteLine();

    WriteBool(autoIncs);
    WriteBool(includeSE);
    WriteInt(nmanualIncs);
    WriteInt(int(legendFormat.format));
    WriteInt(legendFormat.digitsPrecision);
    WriteLine();
}

