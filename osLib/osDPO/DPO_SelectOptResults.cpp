///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectOptResults.cpp
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
#include "DPO_SelectOptResultsPS.h"
#endif
#include "DPO_SelectOptResults.h"

const char* DPO_SelectOptResults::objLongDesc = "Select Results";

DPO_SelectOptResults :: DPO_SelectOptResults() :
    DPO_SelectOpt(this, objLongDesc, "ResultIndex")
{
}

DPO_SelectOptResults :: DPO_SelectOptResults(const DPO_SelectOptResults& a) :
    DFO_SelectOptResults(a),
    DPO_SelectOpt(this, objLongDesc, "ResultIndex")
{
}

DPO_SelectOptResults& DPO_SelectOptResults::operator= (const DPO_SelectOptResults& a)
{
    if (&a != this)
       *((DFO_SelectOptResults*) this) = a;
    return *this;
}


bool  DPO_SelectOptResults::ReadFromFile()
{
    ReadObjectHeader();

    DPO_SelectOpt::ReadFromFile(currentObjReadMajorVersion == 0);

    includeOptResults       = ReadBool();
    includeCaseParameters   = ReadBool();
    includeOptStatus        = ReadBool();

    if (currentObjReadMajorVersion > 1)
        startEndResults = StartEndResults(ReadInt());

    return true;
}


void DPO_SelectOptResults::WriteToFile()
{
    const int objMajVer = 2; // start end table
//    const int objMajVer = 1; // new MS spec
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    DPO_SelectOpt::WriteToFile();

    WriteBool(includeOptResults);
    WriteBool(includeCaseParameters);
    WriteBool(includeOptStatus);
    WriteInt(int(startEndResults));
    WriteLine();
}

#ifndef ADCONSOLEAPP

// ************************* menu stuff


DPO_SelectOptResultsPS& DPO_SelectOptResults::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_SelectOptResultsPS&>(*assocMenu);
}


void DPO_SelectOptResults::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_SelectOptResults::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_SelectOptResultsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_SelectOptResults::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_SelectOptResults::CalcOutput(FOcalcType  calcType)
{
    DFO_SelectOptResults::CalcOutput(calcType);
    DPO_SelectOpt::UpdateUI();
}

#endif //!ADCONSOLEAPP

