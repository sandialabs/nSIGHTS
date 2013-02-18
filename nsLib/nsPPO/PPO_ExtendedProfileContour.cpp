///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ExtendedProfileContour.cpp
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
//      File I/O and user interface for all nsPFOGL functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#include "NsLibAfx.h"

#include <nsPPO/nsPPOConfigFile.h>
#include <genPPO/genPPOConfigFile.h>

#include "PPO_ExtendedProfileContourPS.h"
#include "PPO_ExtendedProfileContour.h"

static const char* objLongDesc = "Extended Profile Contour Lines";

PPO_ExtendedProfileContour::PPO_ExtendedProfileContour(PlotDefC* assPlt) :
    PFO_ExtendedProfileContourGL(*assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_ExtendedProfileContour::PPO_ExtendedProfileContour(PlotDefC* assPlt, const PPO_ExtendedProfileContour& a) :
    PFO_ExtendedProfileContourGL(a, *assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_ExtendedProfileContour& PPO_ExtendedProfileContour::operator= (const PPO_ExtendedProfileContour& a)
{
    if (&a != this)
       *((PPO_ExtendedProfileContour*) this) = a;
    return *this;
}

// ************************* menu ops
PPO_ExtendedProfileContourPS& PPO_ExtendedProfileContour::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_ExtendedProfileContourPS&>(*assocMenu);
}

void PPO_ExtendedProfileContour::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_ExtendedProfileContour::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_ExtendedProfileContourPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_ExtendedProfileContour::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_ExtendedProfileContour::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();

    Read3DExtendedProfileData(*this);

    ReadContourSpec(*this);

    plotAsTubes = ReadBool();
    ReadTubeExtrusionSpec(tubeData);

    CheckForInputFailure("Reading PPO_ExtendedProfileContour");
    return true;
}

void PPO_ExtendedProfileContour::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    Write3DExtendedProfileData(*this);

    WriteContourSpec(*this);

    WriteBool(plotAsTubes); 
    WriteTubeExtrusionSpec(tubeData);

    WriteLine();
}

