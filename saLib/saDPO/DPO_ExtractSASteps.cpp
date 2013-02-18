///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExtractSASteps.cpp
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

#include "saLibAfx.h"

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_ExtractSAStepsPS.h"
#endif
#include "DPO_ExtractSASteps.h"

const char* DPO_ExtractSASteps::objLongDesc = "Extract SA Table Steps";

DPO_ExtractSASteps :: DPO_ExtractSASteps() : MenuObjC(this, objLongDesc)
{
}

DPO_ExtractSASteps :: DPO_ExtractSASteps(const DPO_ExtractSASteps& a) : DFO_ExtractSASteps(a), MenuObjC(this, objLongDesc)
{
}

DPO_ExtractSASteps::~DPO_ExtractSASteps()
{
}

DPO_ExtractSASteps& DPO_ExtractSASteps::operator= (const DPO_ExtractSASteps& a)
{
    if (&a != this)
       static_cast<DFO_ExtractSASteps&>(*this) = a;
    return *this;
}


bool  DPO_ExtractSASteps::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(inputTableObjRef);

    extractByIndexes = ReadBool();
    minIndex = ReadInt();
    maxIndex = ReadInt();
    indexIncrement = ReadInt();
    minValue = ReadDouble();
    maxValue = ReadDouble();

    CheckForInputFailure("Reading DPO_ExtractSASteps");
    return true;
}


void DPO_ExtractSASteps::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteFuncObjRef(inputTableObjRef);

    WriteBool(extractByIndexes);
    WriteInt(minIndex);
    WriteInt(maxIndex);
    WriteInt(indexIncrement);
    WriteDouble(minValue);
    WriteDouble(maxValue);
    WriteLine();

}

#ifndef ADCONSOLEAPP

DPO_ExtractSAStepsPS& DPO_ExtractSASteps::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ExtractSAStepsPS&>(*assocMenu);
}


void DPO_ExtractSASteps::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ExtractSASteps::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ExtractSAStepsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_ExtractSASteps::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

