///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadRangeSimResults.cpp
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
#include "DPO_ReadRangeSimResultsPS.h"
#endif
#include "DPO_ReadRangeSimResults.h"

const char* DPO_ReadRangeSimResults::objLongDesc = "Read Range Results File";

DPO_ReadRangeSimResults :: DPO_ReadRangeSimResults() :
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(rangeSimResultsIO.fileName);
}

DPO_ReadRangeSimResults :: DPO_ReadRangeSimResults(const DPO_ReadRangeSimResults& a) :
    DFO_ReadRangeSimResults(a),
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(rangeSimResultsIO.fileName);
}

DPO_ReadRangeSimResults::~DPO_ReadRangeSimResults()
{
}

DPO_ReadRangeSimResults& DPO_ReadRangeSimResults::operator= (const DPO_ReadRangeSimResults& a)
{
    if (&a != this)
       *((DFO_ReadRangeSimResults*) this) = a;
    return *this;
}


bool  DPO_ReadRangeSimResults::ReadFromFile()
{
    ReadObjectHeader();

    ReadFileName(rangeSimResultsIO.fileName);

    CheckForInputFailure("Reading DPO_ReadRangeSimResults");
    return true;
}


void DPO_ReadRangeSimResults::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteText(rangeSimResultsIO.fileName);
    WriteLine();
}


#ifndef ADCONSOLEAPP

// ************************* menu stuff


DPO_ReadRangeSimResultsPS& DPO_ReadRangeSimResults::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ReadRangeSimResultsPS&>(*assocMenu);
}


void DPO_ReadRangeSimResults::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ReadRangeSimResults::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ReadRangeSimResultsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(false, true);

    PropUpdate();
}


void DPO_ReadRangeSimResults::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP


