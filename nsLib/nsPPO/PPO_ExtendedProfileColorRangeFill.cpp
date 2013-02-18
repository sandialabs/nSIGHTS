///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ExtendedProfileColorRangeFill.cpp
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

#include <genPPO/genPPOConfigFile.h>
#include <nsPPO/nsPPOConfigFile.h>

#include "PPO_ExtendedProfileColorRangeFillPS.h"
#include "PPO_ExtendedProfileColorRangeFill.h"

static const char* objLongDesc = "Extended Profile Color Fill";

PPO_ExtendedProfileColorRangeFill :: PPO_ExtendedProfileColorRangeFill(PlotDefC* assPlt) :
    PFO_ExtendedProfileColorRangeFillGL(*assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_ExtendedProfileColorRangeFill :: PPO_ExtendedProfileColorRangeFill(PlotDefC* assPlt, const PPO_ExtendedProfileColorRangeFill& a) :
    PFO_ExtendedProfileColorRangeFillGL(a, *assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_ExtendedProfileColorRangeFill& PPO_ExtendedProfileColorRangeFill::operator= (const PPO_ExtendedProfileColorRangeFill& a)
{
    if (&a != this)
       *((PPO_ExtendedProfileColorRangeFill*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_ExtendedProfileColorRangeFillPS& PPO_ExtendedProfileColorRangeFill::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_ExtendedProfileColorRangeFillPS&>(*assocMenu);
}


void PPO_ExtendedProfileColorRangeFill::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_ExtendedProfileColorRangeFill::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_ExtendedProfileColorRangeFillPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_ExtendedProfileColorRangeFill::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_ExtendedProfileColorRangeFill::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadColorBase(*this);
    Read3DExtendedProfileData(*this);
    ReadEdgeSpec(edgeSpec);
    ReadRealFormat(reportFormat);

    CheckForInputFailure("Reading PPO_ExtendedProfileColorRangeFill");
    return true;
}


void PPO_ExtendedProfileColorRangeFill::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();
    WriteColorBase(*this);
    Write3DExtendedProfileData(*this);
    WriteEdgeSpec(edgeSpec);

    WriteRealFormat(reportFormat);
    WriteLine();

}

