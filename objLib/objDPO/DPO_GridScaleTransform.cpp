///////////////////////////////////////////////////////////////////////////////////
//
// DPO_GridScaleTransform.cpp
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

#include "DPO_GridScaleTransform.h"

#ifndef ADCONSOLEAPP
#include "DPO_GridScaleTransformPS.h"
#endif

static const char* objLongDesc = "Scale/Transform Grid Data";

DPO_GridScaleTransform :: DPO_GridScaleTransform() :
    DPO_ScaleTransform(this, objLongDesc)
{
}

DPO_GridScaleTransform :: DPO_GridScaleTransform(const DPO_GridScaleTransform& a) :
    DFO_GridScaleTransform(a),
    DPO_ScaleTransform(this, objLongDesc)
{
}


DPO_GridScaleTransform& DPO_GridScaleTransform::operator= (const DPO_GridScaleTransform& a)
{
    if (&a != this)
       *((DFO_GridScaleTransform*) this) = a;
    return *this;
}


bool  DPO_GridScaleTransform::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(gridInputObjRef);

    DPO_ScaleTransform::ReadFromFile(false);

    ReadThreshold(thresholdOp);

    if (currentObjReadMajorVersion > 0)
    {
        averageGrid = ReadBool();
        if (currentObjReadMajorVersion > 1)
            replaceNullsWithAverage = ReadBool(); // added v2
        averageKernelSize = ReadInt();
    }

    ReadText(outputTypeDesc, descLen);

    CheckForInputFailure("Reading DPO_GridScaleTransform");
    return true;
}


void DPO_GridScaleTransform::WriteToFile()
{
    const int objMajVer = 2;  // added nulle replacement averaging
//    const int objMajVer = 1;  // added averagin
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(gridInputObjRef);
    DPO_ScaleTransform::WriteToFile();

    WriteThreshold(thresholdOp);

    WriteBool(averageGrid);
    WriteBool(replaceNullsWithAverage); // added v2
    WriteInt(averageKernelSize);


    WriteLine();

    WriteText(outputTypeDesc);
}

#ifndef ADCONSOLEAPP

DPO_GridScaleTransformPS& DPO_GridScaleTransform::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_GridScaleTransformPS&>(*assocMenu);
}

void DPO_GridScaleTransform::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_GridScaleTransform::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_GridScaleTransformPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_GridScaleTransform::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_GridScaleTransform::CalcOutput(FOcalcType  calcType)
{
    DFO_GridScaleTransform::CalcOutput(calcType);
    UpdateUI();
}


#endif //!ADCONSOLEAPP

