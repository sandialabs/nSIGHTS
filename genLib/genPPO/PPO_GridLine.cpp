///////////////////////////////////////////////////////////////////////////////////
//
// PPO_GridLine.cpp
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

#include "PPO_GridLinePS.h"
#include "PPO_GridLine.h"

const char* PPO_GridLine::objLongDesc = "2D Grid Line";

PPO_GridLine :: PPO_GridLine(PlotDefC* assPlt)
                   : PFO_GridLineGL(*assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_GridLine :: PPO_GridLine(PlotDefC* assPlt, const PPO_GridLine& a)
                            : PFO_GridLineGL (a, *assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_GridLine& PPO_GridLine::operator= (const PPO_GridLine& a)
{
    if (&a != this)
        PFO_GridLineGL::operator=(a);
    return *this;
}

// ************************* menu ops

PPO_GridLinePS& PPO_GridLine::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_GridLinePS&>(*assocMenu);
}


void PPO_GridLine::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_GridLine::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_GridLinePS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_GridLine::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


bool  PPO_GridLine::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();

    ReadFuncObjRef(penSetObjRef);

    ReadRealMSSpec(gridValue);

    gridIsX = ReadBool();
    gridLinePen = ReadInt();
    gridLineThickness = ReadInt();
    gridLineType = PlotLineType(ReadInt());

    ReadGridLineLabelFormat(labelFormat);

    ReadText(gridLabel, maxLabelLen);
    ReadFont(labelFont);

    if (currentObjReadMajorVersion > 0)
        ReadRealFormat(valueFormat);

    CheckForInputFailure("Reading PPO_GridLine");
    return true;
}

void PPO_GridLine::WriteToFile()
{
    const int objMajVer = 1;  // added value format
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(penSetObjRef);
    WriteRealMSSpec(gridValue);

    WriteBool(gridIsX);
    WriteInt(gridLinePen);
    WriteInt(gridLineThickness);
    WriteInt(int(gridLineType));
    WriteLine();

    WriteGridLineLabelFormat(labelFormat);

    WriteText(gridLabel);
    WriteFont(labelFont);

    WriteRealFormat(valueFormat);
    WriteLine();
}

