///////////////////////////////////////////////////////////////////////////////////
//
// DPO_RealScaleTransform.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////


#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>
#include <genApp/ExposedReal.h>

#include "DPO_RealScaleTransform.h"
#ifndef ADCONSOLEAPP
#include "DPO_RealScaleTransformPS.h"
#endif

static const char* objLongDesc = "Scale/Transform Real Data";

DPO_RealScaleTransform :: DPO_RealScaleTransform() :
    DPO_ScaleTransform(this, objLongDesc)
{
}

DPO_RealScaleTransform :: DPO_RealScaleTransform(const DPO_RealScaleTransform& a) :
    DFO_RealScaleTransform(a),
    DPO_ScaleTransform(this, objLongDesc)
{
}


DPO_RealScaleTransform& DPO_RealScaleTransform::operator= (const DPO_RealScaleTransform& a)
{
    if (&a != this)
       *((DFO_RealScaleTransform*) this) = a;
    return *this;
}

bool  DPO_RealScaleTransform::ReadFromFile()
{
    ReadObjectHeader();

    if (currentObjReadMajorVersion < 1)
        ReadInt();                      // oldVer#

    ReadFuncObjRef(realInputObjRef);

    DPO_ScaleTransform::ReadFromFile(currentObjReadMajorVersion == 0);

    limitMin = ReadBool();
    limitMax = ReadBool();
    minLimit = ReadDouble();
    maxLimit = ReadDouble();


    CheckForInputFailure("Reading DPO_RealScaleTransform");
    return true;
}

void DPO_RealScaleTransform::WriteToFile()
{
    const int objMajVer = 1;            // moved realInputObjRef from base class
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(realInputObjRef);

    DPO_ScaleTransform::WriteToFile();

    WriteBool(limitMin);
    WriteBool(limitMax);
    WriteDouble(minLimit, SC_RealFormat(ncf_Scientific, 7));
    WriteDouble(maxLimit, SC_RealFormat(ncf_Scientific, 7));
    WriteLine();

}

#ifndef ADCONSOLEAPP

DPO_RealScaleTransformPS& DPO_RealScaleTransform::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_RealScaleTransformPS&>(*assocMenu);
}

void DPO_RealScaleTransform::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_RealScaleTransform::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_RealScaleTransformPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_RealScaleTransform::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

// ************************* expose support


void DPO_RealScaleTransform::CalcOutput(FOcalcType  calcType)
{
    DFO_RealScaleTransform::CalcOutput(calcType);
    UpdateUI();
}

#endif //!ADCONSOLEAPP

