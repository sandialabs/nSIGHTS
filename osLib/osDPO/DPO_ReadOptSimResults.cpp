///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadOptSimResults.cpp
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
#include "DPO_ReadOptSimResultsPS.h"
#endif
#include "DPO_ReadOptSimResults.h"


const char* DPO_ReadOptSimResults::objLongDesc = "Read Optimizer Results File";

DPO_ReadOptSimResults :: DPO_ReadOptSimResults() :
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(optSimResultsIO.fileName);
}

DPO_ReadOptSimResults :: DPO_ReadOptSimResults(const DPO_ReadOptSimResults& a) :
    DFO_ReadOptSimResults(a),
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(optSimResultsIO.fileName);
}

DPO_ReadOptSimResults::~DPO_ReadOptSimResults()
{
}

DPO_ReadOptSimResults& DPO_ReadOptSimResults::operator= (const DPO_ReadOptSimResults& a)
{
    if (&a != this)
       *((DFO_ReadOptSimResults*) this) = a;
    return *this;
}


bool  DPO_ReadOptSimResults::ReadFromFile()
{
    ReadObjectHeader();

    ReadFileName(optSimResultsIO.fileName);

    CheckForInputFailure("Reading DPO_ReadOptSimResults");
    return true;
}


void DPO_ReadOptSimResults::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteText(optSimResultsIO.fileName);
    WriteLine();
}


#ifndef ADCONSOLEAPP

// ************************* menu stuff


DPO_ReadOptSimResultsPS& DPO_ReadOptSimResults::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ReadOptSimResultsPS&>(*assocMenu);
}


void DPO_ReadOptSimResults::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ReadOptSimResults::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ReadOptSimResultsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(false, true);

    PropUpdate();
}


void DPO_ReadOptSimResults::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


#endif //!ADCONSOLEAPP
