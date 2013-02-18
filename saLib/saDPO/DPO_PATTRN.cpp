///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PATTRN.cpp
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
#include "DPO_PATTRNPS.h"
#endif
#include "DPO_PATTRN.h"


const char* DPO_PATTRN::objLongDesc = "PATTRN";

DPO_PATTRN :: DPO_PATTRN() : MenuObjC(this, objLongDesc)
{
}

DPO_PATTRN :: DPO_PATTRN(const DPO_PATTRN& a) : DFO_PATTRN(a), MenuObjC(this, objLongDesc)
{
}

DPO_PATTRN::~DPO_PATTRN()
{
}

DPO_PATTRN& DPO_PATTRN::operator= (const DPO_PATTRN& a)
{
    if (&a != this)
       *((DFO_PATTRN*) this) = a;
    return *this;
}


bool  DPO_PATTRN::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(indepVarObjRef);
    ReadFuncObjRef(depVarObjRef);
    ReadText(title, titleLen);

    ReadText(obsStr, inpStrLen);
    ReadText(mcRepStr, inpStrLen);
    nreplicates = ReadInt();
    randSeed = ReadInt();
    nxintInp = ReadInt();
    nyintInp = ReadInt();
    nrandomPatterns = ReadInt();
    logarithms = ReadBool();
    alphaCutoff = ReadDouble();
    mtest1      = ReadInt();
    ReadRealFormat(coeffFormat);

    verbose = ReadBool();
    verboseMC = ReadBool();
    verboseCV = ReadBool();
    coefficients = ReadBool();
    rankCoefficients = ReadBool();

    for (int i = ptCC; i < ptLastTest; i++)
        tests[i] = ReadBool();


    CheckForInputFailure("Reading DPO_PATTRN");
    return true;
}


void DPO_PATTRN::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteFuncObjRef(indepVarObjRef);
    WriteFuncObjRef(depVarObjRef);
    WriteText(title);

    WriteText(obsStr);
    WriteText(mcRepStr);
    WriteInt(nreplicates);
    WriteInt(randSeed);
    WriteInt(nxintInp);
    WriteInt(nyintInp);
    WriteInt(nrandomPatterns);
    WriteBool(logarithms);
    WriteDouble(alphaCutoff);
    WriteInt(mtest1);
    WriteLine();
    WriteRealFormat(coeffFormat);
    WriteLine();

    WriteBool(verbose);
    WriteBool(verboseMC);
    WriteBool(verboseCV);
    WriteBool(coefficients);
    WriteBool(rankCoefficients);
    WriteLine();

    for (int i = ptCC; i < ptLastTest; i++)
        WriteBool(tests[i]);
    WriteLine();
}

#ifndef ADCONSOLEAPP

DPO_PATTRNPS& DPO_PATTRN::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_PATTRNPS&>(*assocMenu);
}


void DPO_PATTRN::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_PATTRN::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_PATTRNPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_PATTRN::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

