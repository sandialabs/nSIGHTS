///////////////////////////////////////////////////////////////////////////////////
//
// PPO_GridColorPoint.cpp
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

#include "PPO_GridColorPointPS.h"
#include "PPO_GridColorPoint.h"

static const char* objLongDesc = "Grid Color Symbols";

PPO_GridColorPoint :: PPO_GridColorPoint(PlotDefC* assPlt) :
    PFO_GridColorPointGL(*assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_GridColorPoint :: PPO_GridColorPoint(PlotDefC* assPlt, const PPO_GridColorPoint& a) :
    PFO_GridColorPointGL(a, *assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_GridColorPoint& PPO_GridColorPoint::operator= (const PPO_GridColorPoint& a)
{
    if (&a != this)
       *((PPO_GridColorPoint*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_GridColorPointPS& PPO_GridColorPoint::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_GridColorPointPS&>(*assocMenu);
}


void PPO_GridColorPoint::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_GridColorPoint::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_GridColorPointPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_GridColorPoint::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_GridColorPoint::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadColorBase(*this);

    Read3DGridData(*this);

    pointSymbol  = PlotSymbol(ReadInt());
    pointSymSize = ReadInt();

    ReadEdgeSpec(edgeSpec);

    CheckForInputFailure("Reading PPO_GridColorPoint");
    return true;
}


void PPO_GridColorPoint::WriteToFile()
{
    const int objMajVer = 1;    // added 3D
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();
    WriteColorBase(*this);

    Write3DGridData(*this);

    WriteInt(int(pointSymbol));
    WriteInt(pointSymSize);
    WriteLine();

    WriteEdgeSpec(edgeSpec);

}

