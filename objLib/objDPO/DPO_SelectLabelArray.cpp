///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectLabelArray.cpp
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

#ifndef ADCONSOLEAPP
#include "DPO_SelectLabelArrayPS.h"
#endif
#include "DPO_SelectLabelArray.h"

const char* DPO_SelectLabelArray::objLongDesc = "Select Label Array";

DPO_SelectLabelArray :: DPO_SelectLabelArray() :
    DPO_SelectBase(this, objLongDesc, "Select Label Array")
{
}

DPO_SelectLabelArray :: DPO_SelectLabelArray(const DPO_SelectLabelArray& a) :
    DFO_SelectLabelArray(a),
    DPO_SelectBase(this, objLongDesc, "Select Label Array")
{
}

DPO_SelectLabelArray& DPO_SelectLabelArray::operator= (const DPO_SelectLabelArray& a)
{
    if (&a != this)
       *((DFO_SelectLabelArray*) this) = a;
    return *this;
}


bool  DPO_SelectLabelArray::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(labDataArrayObjRef);

    sortSpec = DFO_SelectLabelArray::SortSpec(ReadInt());
    sortBasis = DFO_SelectLabelArray::SortBasis(ReadInt());

    return DPO_SelectBase::ReadFromFile();
}


void DPO_SelectLabelArray::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(labDataArrayObjRef);
    WriteInt(int(sortSpec));
    WriteInt(int(sortBasis));
    WriteLine();

    DPO_SelectBase::WriteToFile();
}

#ifndef ADCONSOLEAPP


DPO_SelectLabelArrayPS& DPO_SelectLabelArray::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_SelectLabelArrayPS&>(*assocMenu);
}


void DPO_SelectLabelArray::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_SelectLabelArray::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_SelectLabelArrayPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_SelectLabelArray::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_SelectLabelArray::CalcOutput(FOcalcType  calcType)
{
    DFO_SelectLabelArray::CalcOutput(calcType);

    indexStrings.DeAlloc();
    if (StatusOK())
        indexStrings = labDataDO->dataLabels;
    UpdateUI();
}

#endif //!ADCONSOLEAPP

