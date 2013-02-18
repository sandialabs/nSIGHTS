///////////////////////////////////////////////////////////////////////////////////
//
// PPO_CubeColorPoint.cpp
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

#include "PPO_CubeColorPointPS.h"
#include "PPO_CubeColorPoint.h"

static const char* objLongDesc = "Cube Color Symbols";

PPO_CubeColorPoint :: PPO_CubeColorPoint(PlotDefC* assPlt) :
    PFO_CubeColorPointGL(*assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_CubeColorPoint :: PPO_CubeColorPoint(PlotDefC* assPlt, const PPO_CubeColorPoint& a) :
    PFO_CubeColorPointGL(a, *assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_CubeColorPoint& PPO_CubeColorPoint::operator= (const PPO_CubeColorPoint& a)
{
    if (&a != this)
       *((PPO_CubeColorPoint*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_CubeColorPointPS& PPO_CubeColorPoint::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_CubeColorPointPS&>(*assocMenu);
}


void PPO_CubeColorPoint::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_CubeColorPoint::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_CubeColorPointPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_CubeColorPoint::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_CubeColorPoint::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadColorBase(*this);

    Read3DCubeData(*this);

    pointSymbol  = PlotSymbol(ReadInt());
    pointSymSize = ReadInt();

    ReadEdgeSpec(edgeSpec);

    CheckForInputFailure("Reading PPO_CubeColorPoint");
    return true;
}


void PPO_CubeColorPoint::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();
    WriteColorBase(*this);

    Write3DCubeData(*this);

    WriteInt(int(pointSymbol));
    WriteInt(pointSymSize);
    WriteLine();

    WriteEdgeSpec(edgeSpec);

}

