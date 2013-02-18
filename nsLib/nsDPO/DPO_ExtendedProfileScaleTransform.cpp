///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExtendedProfileScaleTransform.cpp
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

#include "NsLibAfx.h"

#include <objDPO/objDPOConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_ExtendedProfileScaleTransformPS.h"
#endif

#include "DPO_ExtendedProfileScaleTransform.h"

static const char* objLongDesc = "Scale/Transform Profile";

DPO_ExtendedProfileScaleTransform :: DPO_ExtendedProfileScaleTransform() :
    DPO_ScaleTransform(this, objLongDesc)
{
}

DPO_ExtendedProfileScaleTransform :: DPO_ExtendedProfileScaleTransform(const DPO_ExtendedProfileScaleTransform& a) :
    DFO_ExtendedProfileScaleTransform(a),
    DPO_ScaleTransform(this, objLongDesc)
{
}


DPO_ExtendedProfileScaleTransform& DPO_ExtendedProfileScaleTransform::operator= (const DPO_ExtendedProfileScaleTransform& a)
{
    if (&a != this)
       *((DFO_ExtendedProfileScaleTransform*) this) = a;
    return *this;
}


bool  DPO_ExtendedProfileScaleTransform::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(profileInputObjRef);

    DPO_ScaleTransform::ReadFromFile();

    ReadThreshold(thresholdOp);

    CheckForInputFailure("Reading DPO_ExtendedProfileScaleTransform");
    return true;
}


void DPO_ExtendedProfileScaleTransform::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(profileInputObjRef);

    DPO_ScaleTransform::WriteToFile();

    WriteThreshold(thresholdOp);
    WriteLine();
}

#ifndef ADCONSOLEAPP

DPO_ExtendedProfileScaleTransformPS& DPO_ExtendedProfileScaleTransform::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ExtendedProfileScaleTransformPS&>(*assocMenu);
}

void DPO_ExtendedProfileScaleTransform::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ExtendedProfileScaleTransform::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ExtendedProfileScaleTransformPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_ExtendedProfileScaleTransform::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_ExtendedProfileScaleTransform::CalcOutput(FOcalcType  calcType)
{
    DFO_ExtendedProfileScaleTransform::CalcOutput(calcType);
    UpdateUI();
}

#endif //ADCONSOLEAPP

