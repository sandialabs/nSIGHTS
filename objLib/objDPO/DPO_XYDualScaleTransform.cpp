///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYDualScaleTransform.cpp
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

#include "DPO_XYDualScaleTransform.h"

#ifndef ADCONSOLEAPP
#include "DPO_XYDualScaleTransformPS.h"
#endif

static const char* objLongDesc = "Scale/Transform of X and Y Data";

DPO_XYDualScaleTransform :: DPO_XYDualScaleTransform() :
    MenuObjC(this, objLongDesc)
{
}

DPO_XYDualScaleTransform :: DPO_XYDualScaleTransform(const DPO_XYDualScaleTransform& a) :
    DFO_XYDualScaleTransform(a),
    MenuObjC(this, objLongDesc)
{
}


DPO_XYDualScaleTransform& DPO_XYDualScaleTransform::operator= (const DPO_XYDualScaleTransform& a)
{
    if (&a != this)
       *((DFO_XYDualScaleTransform*) this) = a;
    return *this;
}


bool  DPO_XYDualScaleTransform::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(xyInputObjRef);

    xST.scaleOp     = DSC_ScaleTransform::ScaleType(ReadInt());
    xST.tranOp      = DSC_ScaleTransform::TransformType(ReadInt());
    xST.scaleFirst  = ReadBool();

    yST.scaleOp     = DSC_ScaleTransform::ScaleType(ReadInt());
    yST.tranOp      = DSC_ScaleTransform::TransformType(ReadInt());
    yST.scaleFirst  = ReadBool();

    positiveXOnly   = ReadBool();

    ReadText(outputTypeDesc, descLen);

    ReadRealMSSpec(xscaleMS);
    ReadRealMSSpec(xoffsetMS);
    ReadRealMSSpec(yscaleMS);
    ReadRealMSSpec(yoffsetMS);

    CheckForInputFailure("Reading DPO_XYDualScaleTransform");
    return true;
}


void DPO_XYDualScaleTransform::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(xyInputObjRef);

    SetRealConvFormat(SC_RealFormat(ncf_Scientific, 8));

    WriteInt(int(xST.scaleOp));
    WriteInt(int(xST.tranOp));
    WriteBool(xST.scaleFirst);
    WriteLine();
    WriteInt(int(yST.scaleOp));
    WriteInt(int(yST.tranOp));
    WriteBool(yST.scaleFirst);
    WriteLine();

    WriteBool(positiveXOnly);
    WriteLine();

    WriteText(outputTypeDesc);

    WriteRealMSSpec(xscaleMS);
    WriteRealMSSpec(xoffsetMS);
    WriteRealMSSpec(yscaleMS);
    WriteRealMSSpec(yoffsetMS);
}

#ifndef ADCONSOLEAPP

DPO_XYDualScaleTransformPS& DPO_XYDualScaleTransform::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_XYDualScaleTransformPS&>(*assocMenu);
}

void DPO_XYDualScaleTransform::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_XYDualScaleTransform::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_XYDualScaleTransformPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_XYDualScaleTransform::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

