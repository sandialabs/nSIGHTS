///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadXYSimResults.cpp
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

#include "nsLibAfx.h"

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_ReadXYSimResultsPS.h"
#endif //!ADCONSOLEAPP

#include "DPO_ReadXYSimResults.h"


const char* DPO_ReadXYSimResults::objLongDesc = "Read XY Sim ResultsFile";

DPO_ReadXYSimResults :: DPO_ReadXYSimResults() :
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(xySimResultsIO.fileName);
}

DPO_ReadXYSimResults :: DPO_ReadXYSimResults(const DPO_ReadXYSimResults& a) :
    DFO_ReadXYSimResults(a),
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(xySimResultsIO.fileName);
}

DPO_ReadXYSimResults& DPO_ReadXYSimResults::operator= (const DPO_ReadXYSimResults& a)
{
    if (&a != this)
       *((DFO_ReadXYSimResults*) this) = a;
    return *this;
}


bool  DPO_ReadXYSimResults::ReadFromFile()
{
    ReadObjectHeader();

    ReadFileName(xySimResultsIO.fileName);

    CheckForInputFailure("Reading DPO_ReadXYSimResults");
    return true;
}


void DPO_ReadXYSimResults::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteText(xySimResultsIO.fileName);
    WriteLine();
}


#ifndef ADCONSOLEAPP

// ************************* menu stuff


DPO_ReadXYSimResultsPS& DPO_ReadXYSimResults::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ReadXYSimResultsPS&>(*assocMenu);
}


void DPO_ReadXYSimResults::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ReadXYSimResults::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ReadXYSimResultsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(false, true);

    PropUpdate();
}


void DPO_ReadXYSimResults::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}



#endif //!ADCONSOLEAPP

