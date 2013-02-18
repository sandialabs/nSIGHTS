///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYArrayScaleTransform.cpp
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

#include <objDPO/objDPOConfigFile.h>

#include "DPO_XYArrayScaleTransform.h"

#ifndef ADCONSOLEAPP
#include "DPO_XYArrayScaleTransformPS.h"
#endif

static const char* objLongDesc = "Scale/Transform of XY Array";

DPO_XYArrayScaleTransform :: DPO_XYArrayScaleTransform() :
    DPO_ScaleTransform(this, objLongDesc)
{
}

DPO_XYArrayScaleTransform :: DPO_XYArrayScaleTransform(const DPO_XYArrayScaleTransform& a) :
    DFO_XYArrayScaleTransform(a),
    DPO_ScaleTransform(this, objLongDesc)
{
}


DPO_XYArrayScaleTransform& DPO_XYArrayScaleTransform::operator= (const DPO_XYArrayScaleTransform& a)
{
    if (&a != this)
       *((DFO_XYArrayScaleTransform*) this) = a;
    return *this;
}


bool  DPO_XYArrayScaleTransform::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(xyArrayInputObjRef);

    DPO_ScaleTransform::ReadFromFile();

    operateOnX = ReadBool();

    ReadThreshold(thresholdOp);


    CheckForInputFailure("Reading DPO_XYArrayScaleTransform");
    return true;
}


void DPO_XYArrayScaleTransform::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(xyArrayInputObjRef);
    DPO_ScaleTransform::WriteToFile();

    WriteBool(operateOnX);

    WriteThreshold(thresholdOp);

}

#ifndef ADCONSOLEAPP

DPO_XYArrayScaleTransformPS& DPO_XYArrayScaleTransform::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_XYArrayScaleTransformPS&>(*assocMenu);
}

void DPO_XYArrayScaleTransform::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_XYArrayScaleTransform::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_XYArrayScaleTransformPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_XYArrayScaleTransform::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_XYArrayScaleTransform::CalcOutput(FOcalcType  calcType)
{
    DFO_XYArrayScaleTransform::CalcOutput(calcType);
    UpdateUI();
}

#endif //!ADCONSOLEAPP

