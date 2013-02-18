///////////////////////////////////////////////////////////////////////////////////
//
// PPO_GridContour.cpp
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

#include <objPPO/objPPOConfigFile.h>
#include <genPPO/genPPOConfigFile.h>

#include "PPO_GridContourPS.h"
#include "PPO_GridContour.h"

static const char* objLongDesc = "Grid Contour Lines";

PPO_GridContour::PPO_GridContour(PlotDefC* assPlt) :
    PFO_GridContourGL(*assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_GridContour::PPO_GridContour(PlotDefC* assPlt, const PPO_GridContour& a) :
    PFO_GridContourGL(a, *assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_GridContour& PPO_GridContour::operator= (const PPO_GridContour& a)
{
    if (&a != this)
       *((PPO_GridContour*) this) = a;
    return *this;
}

// ************************* menu ops
PPO_GridContourPS& PPO_GridContour::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_GridContourPS&>(*assocMenu);
}

void PPO_GridContour::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_GridContour::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_GridContourPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_GridContour::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_GridContour::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();

    if (currentObjReadMajorVersion > 0)
        Read3DGridData(*this);

    ReadContourSpec(*this);

    if (currentObjReadMajorVersion > 1)
    {
        plotAsTubes = ReadBool();
        ReadTubeExtrusionSpec(tubeData);
    }
    else
    {
        bool   outputCoords = ReadBool();
    }

    CheckForInputFailure("Reading PPO_GridContour");
    return true;
}

void PPO_GridContour::WriteToFile()
{
    const int objMajVer = 2;    // added tune extrusion for 3D
//    const int objMajVer = 1;    // added 3D
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    Write3DGridData(*this);

    WriteContourSpec(*this);

    WriteBool(plotAsTubes);  // added v2
    WriteTubeExtrusionSpec(tubeData);

    WriteLine();
}

