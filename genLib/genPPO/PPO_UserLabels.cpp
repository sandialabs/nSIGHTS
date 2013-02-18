///////////////////////////////////////////////////////////////////////////////////
//
// PPO_UserLabels.cpp
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
#include "PPO_UserLabels.h"
#include "PPO_UserLabelsPS.h"


const char* PPO_UserLabels::objLongDesc = "User Entered Labels";

PPO_UserLabels :: PPO_UserLabels(PlotDefC* assPlt) :
    PFO_UserLabelsGL(*assPlt),
    PPO_AnnoBase(this, objLongDesc)
{}

PPO_UserLabels :: PPO_UserLabels(PlotDefC* assPlt, const PPO_UserLabels& a) :
    PFO_UserLabelsGL (a, *assPlt),
    PPO_AnnoBase(this, objLongDesc)
{}

PPO_UserLabels& PPO_UserLabels::operator= (const PPO_UserLabels& a)
{
    if (&a != this)
       *((PFO_UserLabels*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_UserLabelsPS& PPO_UserLabels::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_UserLabelsPS&>(*assocMenu);
}

void PPO_UserLabels::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void  PPO_UserLabels::UpdateULUI()
{
    if (UpdateOK())
        MenuRef().UpdateULUI(*this);
}

void PPO_UserLabels::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

void PPO_UserLabels::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_UserLabelsPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

bool  PPO_UserLabels::ReadFromFile()
{
    ReadObjectHeader();
    PPO_AnnoBase::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);

    for ( int i = 0; i < PFO_UserLabels::maxUserLabels; i++)
    {
        labelData[i].doLabel = ReadBool();
        labelData[i].labAlign  = HorizontalTextAlignment(ReadInt());
        ReadTextWithTrailingBlanks(labelData[i].userLabel, PFO_UserLabels::UserLabel::userLabelLen);
    }

    ulLocation.pX = ReadDouble();
    ulLocation.pY = ReadDouble();

    frameLegend       = ReadBool();
    frameThick        = ReadInt();
    opaqueBackground  = ReadBool();
    legendPen         = ReadInt();

    ReadFont(legendFont);

    CheckForInputFailure("Reading PPO_UserLabels");
    return true;
}



void PPO_UserLabels::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_AnnoBase::WriteToFile();

    WriteFuncObjRef(penSetObjRef);

    for ( int i = 0; i < PFO_UserLabels::maxUserLabels; i++)
    {
        WriteBool(labelData[i].doLabel);
        WriteInt(int(labelData[i].labAlign));
        WriteLine();
        WriteTextWithTrailingBlanks(labelData[i].userLabel);
    }

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
    WriteDouble(ulLocation.pX);
    WriteDouble(ulLocation.pY);
    WriteLine();

    WriteBool(frameLegend);
    WriteInt (frameThick);
    WriteBool(opaqueBackground);
    WriteInt (legendPen);
    WriteLine();

    WriteFont(legendFont);

}

