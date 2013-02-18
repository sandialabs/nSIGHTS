///////////////////////////////////////////////////////////////////////////////////
//
// PPO_DataLabels.cpp
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
#include "PPO_DataLabelsPS.h"
#include "PPO_DataLabels.h"

const char* PPO_DataLabels::objLongDesc = "Labels from Object Output";

PPO_DataLabels :: PPO_DataLabels(PlotDefC* assPlt) :
    PFO_DataLabelsGL(*assPlt),
    PPO_AnnoBase(this, objLongDesc)
{ }

PPO_DataLabels :: PPO_DataLabels(PlotDefC* assPlt, const PPO_DataLabels& a) :
    PFO_DataLabelsGL (a, *assPlt),
    PPO_AnnoBase(this, objLongDesc)
{ }

PPO_DataLabels& PPO_DataLabels::operator= (const PPO_DataLabels& a)
{
    if (&a != this)
       *((PFO_DataLabels*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_DataLabelsPS& PPO_DataLabels::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_DataLabelsPS&>(*assocMenu);
}

void PPO_DataLabels::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void PPO_DataLabels::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_DataLabelsPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void PPO_DataLabels::UpdateULUI()
{
    if (UpdateOK())
        MenuRef().UpdateULUI(*this);
}

void PPO_DataLabels::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_DataLabels::ReadFromFile()
{
    ReadObjectHeader();
    PPO_AnnoBase::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);

    int nread = PFO_DataLabels::maxDataLabels;
    if (currentObjReadMajorVersion < 1)
        nread = 6;

    for ( int i = 0; i < nread; i++)
    {
        ReadFuncObjRef(labelData[i].labelObjRef);
        labelData[i].doLabel   = ReadBool();
        labelData[i].labAlign  = HorizontalTextAlignment(ReadInt());

        labelData[i].overrideFormat                 = ReadBool();
        labelData[i].labelFormat.format             = NumConvFormat(ReadInt());
        labelData[i].labelFormat.digitsPrecision    = ReadInt();

        labelData[i].overrideMainLabel     = ReadBool();
        ReadText(labelData[i].mainLabel, DO_Label::maxLabelLen);
        ReadText(labelData[i].unitsLabel, DO_Label::maxLabelLen);
    }

    ulLocation.pX = ReadDouble();
    ulLocation.pY = ReadDouble();

    ReadText(legendTitle, PSC_AnnoSpec::legendTitleLen);

    frameLegend        = ReadBool();
    frameThick         = ReadInt();
    opaqueBackground   = ReadBool();
    legendPen          = ReadInt();
    minDataFieldWidth  = ReadInt();

    ReadFont(legendFont);

    CheckForInputFailure("Reading PPO_DataLabels");
    return true;
}



void PPO_DataLabels::WriteToFile()
{
    const int objMajVer = 1;   // increased max data labels to 16
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_AnnoBase::WriteToFile();

    WriteFuncObjRef(penSetObjRef);

    for ( int i = 0; i < PFO_DataLabels::maxDataLabels; i++)
    {
        WriteFuncObjRef(labelData[i].labelObjRef);
        WriteBool(labelData[i].doLabel);
        WriteInt(int(labelData[i].labAlign));

        WriteBool(labelData[i].overrideFormat);
        WriteInt(int(labelData[i].labelFormat.format));
        WriteInt(labelData[i].labelFormat.digitsPrecision);
        WriteBool(labelData[i].overrideMainLabel);
        WriteLine();

        WriteText(labelData[i].mainLabel);
        WriteText(labelData[i].unitsLabel);
    }

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
    WriteDouble(ulLocation.pX);
    WriteDouble(ulLocation.pY);
    WriteLine();

    WriteText(legendTitle);

    WriteBool(frameLegend);
    WriteInt (frameThick);
    WriteBool(opaqueBackground);
    WriteInt (legendPen);
    WriteInt (minDataFieldWidth);
    WriteLine();

    WriteFont(legendFont);
}

