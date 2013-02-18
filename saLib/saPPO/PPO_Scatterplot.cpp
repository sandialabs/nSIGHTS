///////////////////////////////////////////////////////////////////////////////////
//
// PPO_Scatterplot.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "SaLibAfx.h"

#include <genPPO/genPPOConfigFile.h>

#include "PPO_ScatterplotPS.h"
#include "PPO_Scatterplot.h"

const char* PPO_Scatterplot::objLongDesc = "SA Scatterplot";

PPO_Scatterplot :: PPO_Scatterplot(PlotDefC* assPlt) :
    PFO_ScatterplotGL(*assPlt),
    PPO_Base(this, objLongDesc)
{ }

PPO_Scatterplot :: PPO_Scatterplot(PlotDefC* assPlt, const PPO_Scatterplot& a) :
    PFO_ScatterplotGL (a, *assPlt),
    PPO_Base(this, objLongDesc)
{ }

PPO_Scatterplot& PPO_Scatterplot::operator= (const PPO_Scatterplot& a)
{
    if (&a != this)
       *((PFO_Scatterplot*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_ScatterplotPS& PPO_Scatterplot::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_ScatterplotPS&>(*assocMenu);
}

void PPO_Scatterplot::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void PPO_Scatterplot::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_ScatterplotPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_Scatterplot::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_Scatterplot::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);
    ReadFuncObjRef(scatterDataObjRef);
    ReadFuncObjRef(xgridLineObjRef);
    ReadFuncObjRef(ygridLineObjRef);

    plotXGridLines = ReadBool();
    plotYGridLines = ReadBool();
    plotAxesLabels = ReadBool();

    plotDVIndx = ReadInt();
    plotIVIndx = ReadInt();

    ReadSymbolSpec(scatterSpec);

    CheckForInputFailure("Reading PPO_Scatterplot");
    return true;
}

void PPO_Scatterplot::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(penSetObjRef);
    WriteFuncObjRef(scatterDataObjRef);
    WriteFuncObjRef(xgridLineObjRef);
    WriteFuncObjRef(ygridLineObjRef);

    WriteBool(plotXGridLines);
    WriteBool(plotYGridLines);
    WriteBool(plotAxesLabels);

    WriteInt(plotDVIndx);
    WriteInt(plotIVIndx);
    WriteLine();

    WriteSymbolSpec(scatterSpec);
    WriteLine();
}


