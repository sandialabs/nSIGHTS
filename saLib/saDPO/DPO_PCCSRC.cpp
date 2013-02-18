///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PCCSRC.cpp
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
#include "DPO_PCCSRCPS.h"
#endif
#include "DPO_PCCSRC.h"


const char* DPO_PCCSRC::objLongDesc = "PCC/SRC";

DPO_PCCSRC :: DPO_PCCSRC() : MenuObjC(this, objLongDesc)
{
}

DPO_PCCSRC :: DPO_PCCSRC(const DPO_PCCSRC& a) : DFO_PCCSRC(a), MenuObjC(this, objLongDesc)
{
}

DPO_PCCSRC::~DPO_PCCSRC()
{
}

DPO_PCCSRC& DPO_PCCSRC::operator= (const DPO_PCCSRC& a)
{
    if (&a != this)
       *((DFO_PCCSRC*) this) = a;
    return *this;
}


bool  DPO_PCCSRC::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(indepVarObjRef);
    ReadFuncObjRef(depVarObjRef);
    ReadText(title, titleLen);

    doPCC = ReadBool();
    doRANK = ReadBool();
    doSRC = ReadBool();
    if (currentObjReadMajorVersion > 1)
        verboseOutput = ReadBool();

    listCutoff = ReadDouble();
    plotCutoff = ReadDouble();
    topIND = ReadInt();

    if (currentObjReadMajorVersion > 0)
        ReadRealFormat(coeffFormat);

    CheckForInputFailure("Reading DPO_PCCSRC");
    return true;
}


void DPO_PCCSRC::WriteToFile()
{
    const int objMajVer = 2; // added verbose
//    const int objMajVer = 1; // added coeff format
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteFuncObjRef(indepVarObjRef);
    WriteFuncObjRef(depVarObjRef);
    WriteText(title);

    WriteBool(doPCC);
    WriteBool(doRANK);
    WriteBool(doSRC);
    WriteBool(verboseOutput); // added v2

    WriteDouble(listCutoff);
    WriteDouble(plotCutoff);
    WriteInt(topIND);

    WriteRealFormat(coeffFormat); // added v1
    WriteLine();
}

#ifndef ADCONSOLEAPP

DPO_PCCSRCPS& DPO_PCCSRC::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_PCCSRCPS&>(*assocMenu);
}


void DPO_PCCSRC::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_PCCSRC::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_PCCSRCPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_PCCSRC::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

