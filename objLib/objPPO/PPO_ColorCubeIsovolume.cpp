///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ColorCubeIsovolume.cpp
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

#include "PPO_ColorCubeIsovolumePS.h"
#include "PPO_ColorCubeIsovolume.h"

static const char* objLongDesc = "Single Color Isovolume of Cube Data";

PPO_ColorCubeIsovolume :: PPO_ColorCubeIsovolume(PlotDefC* assPlt) :
    PFO_ColorCubeIsovolumeGL(*assPlt),
    PPO_Base(this, objLongDesc),
    exposedisoValue(*this, "Isovalue", isoValueMS.enteredRealValue)
{
}

PPO_ColorCubeIsovolume :: PPO_ColorCubeIsovolume(PlotDefC* assPlt, const PPO_ColorCubeIsovolume& a) :
    PFO_ColorCubeIsovolumeGL(a, *assPlt),
    PPO_Base(this, objLongDesc),
    exposedisoValue(*this, "Isovalue", isoValueMS.enteredRealValue)
{
}

PPO_ColorCubeIsovolume& PPO_ColorCubeIsovolume::operator= (const PPO_ColorCubeIsovolume& a)
{
    if (&a != this)
       *((PPO_ColorCubeIsovolume*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_ColorCubeIsovolumePS& PPO_ColorCubeIsovolume::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_ColorCubeIsovolumePS&>(*assocMenu);
}


void PPO_ColorCubeIsovolume::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_ColorCubeIsovolume::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_ColorCubeIsovolumePS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_ColorCubeIsovolume::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_ColorCubeIsovolume::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadColorBase(*this);

    ReadRealMSSpec(isoValueMS);
    usePen = ReadBool();
    isothermPen = ReadInt();

    exposedisoValue.ReadExposed();

    CheckForInputFailure("Reading PPO_ColorCubeIsovolume");
    return true;
}


void PPO_ColorCubeIsovolume::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();
    WriteColorBase(*this);

    WriteRealMSSpec(isoValueMS);
    WriteBool(usePen);
    WriteInt(isothermPen);
    WriteLine();

    exposedisoValue.WriteExposed();
}

// ************************* expose support

void PPO_ColorCubeIsovolume::CalcOutput(FOcalcType  calcType)
{
    PFO_ColorCubeIsovolume::CalcOutput(calcType);
    if (MenuUpdateOK())
        MenuRef().isoValueMSUI.realValueUI.SetFieldValue(isoValueMS.GetCurrentValue());
}

bool PPO_ColorCubeIsovolume::ContainsExposedObject()
{
    return exposedisoValue.IsExposed();
}

