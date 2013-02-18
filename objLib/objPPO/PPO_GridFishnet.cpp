///////////////////////////////////////////////////////////////////////////////////
//
// PPO_GridFishnet.cpp
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
#include <objPPO/objPPOConfigFile.h>

#include "PPO_GridFishnetPS.h"
#include "PPO_GridFishnet.h"

const char* PPO_GridFishnet::objLongDesc = "Grid Fishnet";

PPO_GridFishnet :: PPO_GridFishnet(PlotDefC* assPlt)
                   : PFO_GridFishnetGL(*assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_GridFishnet :: PPO_GridFishnet(PlotDefC* assPlt, const PPO_GridFishnet& a)
                            : PFO_GridFishnetGL (a, *assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_GridFishnet& PPO_GridFishnet::operator= (const PPO_GridFishnet& a)
{
    if (&a != this)
       *((PFO_GridFishnet*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_GridFishnetPS& PPO_GridFishnet::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_GridFishnetPS&>(*assocMenu);
}


void PPO_GridFishnet::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_GridFishnet::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_GridFishnetPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_GridFishnet::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


bool  PPO_GridFishnet::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);
    ReadFuncObjRef(gridDataObjRef);

    if (currentObjReadMajorVersion > 0)
        plotAsTubes = ReadBool();

    gridPen        = ReadInt();
    gridLineThk    = ReadInt();
    gridLineType   = PlotLineType(ReadInt());


    xplotModulus = ReadInt();
    plotLastX    = ReadBool();

    yplotModulus = ReadInt();
    plotLastY    = ReadBool();

    ReadTubeExtrusionSpec(tubeData);

    ReadLegendOverride(legendOverride);

    Read3DCoordMap(coordMap);

    CheckForInputFailure("Reading PPO_GridFishnet");
    return true;
}

void PPO_GridFishnet::WriteToFile()
{
    // ver 0
    const int objMajVer = 1;  // added missing plotAsTubes
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(penSetObjRef);
    WriteFuncObjRef(gridDataObjRef);
    WriteLine();

    WriteBool(plotAsTubes);
    WriteInt(gridPen);
    WriteInt(gridLineThk);
    WriteInt(gridLineType);
    WriteLine();

    WriteInt(xplotModulus);
    WriteBool(plotLastX);
    WriteInt(yplotModulus);
    WriteBool(plotLastY);
    WriteLine();

    WriteTubeExtrusionSpec(tubeData);

    WriteLegendOverride(legendOverride);

    Write3DCoordMap(coordMap);
}

