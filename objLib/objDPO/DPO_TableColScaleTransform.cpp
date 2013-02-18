///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableColScaleTransform.cpp
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

#include "DPO_TableColScaleTransform.h"
#ifndef ADCONSOLEAPP
#include "DPO_TableColScaleTransformPS.h"
#endif

static const char* objLongDesc = "Scale/Transform Table Column";

DPO_TableColScaleTransform :: DPO_TableColScaleTransform() :
    DPO_ScaleTransform(this, objLongDesc)
{
}

DPO_TableColScaleTransform :: DPO_TableColScaleTransform(const DPO_TableColScaleTransform& a) :
    DFO_TableColScaleTransform(a),
    DPO_ScaleTransform(this, objLongDesc)
{
}


DPO_TableColScaleTransform& DPO_TableColScaleTransform::operator= (const DPO_TableColScaleTransform& a)
{
    if (&a != this)
       *((DFO_TableColScaleTransform*) this) = a;
    return *this;
}


bool  DPO_TableColScaleTransform::ReadFromFile()
{
    ReadObjectHeader();

    ReadTableColSel(*this);
    if (currentObjReadMajorVersion > 1)
        columnsSel = ColumnsSel(ReadInt());

    DPO_ScaleTransform::ReadFromFile();

    ReadThreshold(thresholdOp);

    if (currentObjReadMajorVersion > 0)
    {
        operateOnExisting = ReadBool(); // v1
        ReadText(outputColID, idLen);  // v1
    }

    CheckForInputFailure("Reading DPO_TableColScaleTransform");
    return true;
}


void DPO_TableColScaleTransform::WriteToFile()
{
    const int objMajVer = 2;  // added all columns, skip column options
//    const int objMajVer = 1;  // added operateOnExisting, outputColID
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteTableColSel(*this);
    WriteInt(int(columnsSel));

    DPO_ScaleTransform::WriteToFile();

    WriteThreshold(thresholdOp);

    WriteBool(operateOnExisting); // v1
    WriteLine();

    WriteText(outputColID);  // v1

}

#ifndef ADCONSOLEAPP

DPO_TableColScaleTransformPS& DPO_TableColScaleTransform::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_TableColScaleTransformPS&>(*assocMenu);
}

void DPO_TableColScaleTransform::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_TableColScaleTransform::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_TableColScaleTransformPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_TableColScaleTransform::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_TableColScaleTransform::CalcOutput(FOcalcType  calcType)
{
    DFO_TableColScaleTransform::CalcOutput(calcType);
    UpdateUI();
}


#endif //!ADCONSOLEAPP

