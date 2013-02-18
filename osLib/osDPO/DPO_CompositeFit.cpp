///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CompositeFit.cpp
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
//      I/O and user interface for all osDFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_CompositeFitPS.h"
#endif
#include "DPO_CompositeFit.h"

static const char* objLongDesc = "Composite Fit";

DPO_CompositeFit :: DPO_CompositeFit() :
    MenuObjC(this, objLongDesc)
{
}

DPO_CompositeFit :: DPO_CompositeFit(const DPO_CompositeFit& a) :
    DFO_CompositeFit(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_CompositeFit& DPO_CompositeFit::operator= (const DPO_CompositeFit& a)
{
    if (&a != this)
       *((DFO_CompositeFit*) this) = a;
    return *this;
}


bool  DPO_CompositeFit::ReadFromFile()
{
    ReadObjectHeader();

    for (int i = 0; i < maxFitData; i++)
    {
        ReadFuncObjRef(fitData[i].inputFitObjRef);
        fitData[i].useFit = ReadBool();
        fitData[i].fitScaling = ReadDouble();
        fitData[i].logFit = ReadBool();
    }

    CheckForInputFailure("Reading DPO_CompositeFit");
    return true;
}


void DPO_CompositeFit::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    for (int i = 0; i < maxFitData; i++)
    {
        WriteFuncObjRef(fitData[i].inputFitObjRef);
        WriteBool(fitData[i].useFit);
        WriteDouble(fitData[i].fitScaling);
        WriteBool(fitData[i].logFit);
        WriteLine();
    }
}

#ifndef ADCONSOLEAPP

DPO_CompositeFitPS& DPO_CompositeFit::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CompositeFitPS&>(*assocMenu);
}


void DPO_CompositeFit::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_CompositeFit::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CompositeFitPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_CompositeFit::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC ::PropApply();
}

#endif //!ADCONSOLEAPP

