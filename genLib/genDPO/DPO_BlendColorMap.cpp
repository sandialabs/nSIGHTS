///////////////////////////////////////////////////////////////////////////////////
//
// DPO_BlendColorMap.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              File I/O for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#ifndef ADCONSOLEAPP
#include "DPO_BlendColorMapPS.h"
#endif

#include <genApp/AppConfigFile.h>
#include <genApp/ExposedReal.h>

#include "DPO_BlendColorMap.h"

// **************************************** constructor/destructor

static const char* objLongDesc = "Blend Color Maps";

DPO_BlendColorMap :: DPO_BlendColorMap() :
    MenuObjC(this, objLongDesc),
    exposedT(*this, "Blend T Value", tValMS.enteredRealValue)
{
}

DPO_BlendColorMap :: DPO_BlendColorMap(const DPO_BlendColorMap& a) :
    DFO_BlendColorMap(a),
    MenuObjC(this, objLongDesc),
    exposedT(*this, "Blend T Value", tValMS.enteredRealValue)

{
}

DPO_BlendColorMap& DPO_BlendColorMap::operator= (const DPO_BlendColorMap& a)
{
    if (&a != this)
       *((DFO_BlendColorMap*) this) = a;
    return *this;
}

bool  DPO_BlendColorMap::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(input1ObjRef);
    ReadFuncObjRef(input2ObjRef);

    ReadRealMSSpec(tValMS);

    exposedT.ReadExposed();

    CheckForInputFailure("Reading DPO_BlendColorMap");
    return true;
}

void DPO_BlendColorMap::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;
    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(input1ObjRef);
    WriteFuncObjRef(input2ObjRef);

    SetRealConvFormat(SC_RealFormat(ncf_Scientific, 8));
    WriteRealMSSpec(tValMS);

    exposedT.WriteExposed();

}

#ifndef ADCONSOLEAPP

DPO_BlendColorMapPS& DPO_BlendColorMap::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_BlendColorMapPS&>(*assocMenu);
}

void DPO_BlendColorMap::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_BlendColorMap::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_BlendColorMapPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_BlendColorMap::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

// ************************* expose support

void DPO_BlendColorMap::CalcOutput(FOcalcType  calcType)
{
    DFO_BlendColorMap::CalcOutput(calcType);
    if (MenuUpdateOK())
    {
        MenuRef().tValMSUI.realValueUI.SetFieldValue(tValMS.GetCurrentValue());
        MenuRef().colorMapDraw.DrawColorMap(blendColorMap);
    }
}

bool DPO_BlendColorMap::ContainsExposedObject()
{
    return exposedT.IsExposed();
}

#endif

