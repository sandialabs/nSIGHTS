///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectXYfromXYSimResults.cpp
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
#include "DPO_SelectXYfromXYSimResultsPS.h"
#endif

#include "DPO_SelectXYfromXYSimResults.h"

const char* DPO_SelectXYfromXYSimResults::objLongDesc = "Select XY from Results";

DPO_SelectXYfromXYSimResults :: DPO_SelectXYfromXYSimResults() :
    DPO_SelectBase(this, objLongDesc, "XY Results Index")
{
}

DPO_SelectXYfromXYSimResults :: DPO_SelectXYfromXYSimResults(const DPO_SelectXYfromXYSimResults& a) :
    DFO_SelectXYfromXYSimResults(a),
    DPO_SelectBase(this, objLongDesc, "XY Results Index")
{
}

DPO_SelectXYfromXYSimResults& DPO_SelectXYfromXYSimResults::operator= (const DPO_SelectXYfromXYSimResults& a)
{
    if (&a != this)
       *((DFO_SelectXYfromXYSimResults*) this) = a;
    return *this;
}


bool  DPO_SelectXYfromXYSimResults::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(xySimResultsObjRef);

    if (currentObjReadMajorVersion > 1)
        xySelIndex = ReadInt();

    return DPO_SelectBase::ReadFromFile(currentObjReadMajorVersion == 0);
}


void DPO_SelectXYfromXYSimResults::WriteToFile()
{
    const int objMajVer = 2;        // changed to XY selection
//  const int objMajVer = 1;        // added mult sel
//  const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(xySimResultsObjRef);
    WriteInt(xySelIndex);

    DPO_SelectBase::WriteToFile();
}

#ifndef ADCONSOLEAPP

// ************************* menu stuff


DPO_SelectXYfromXYSimResultsPS& DPO_SelectXYfromXYSimResults::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_SelectXYfromXYSimResultsPS&>(*assocMenu);
}


void DPO_SelectXYfromXYSimResults::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_SelectXYfromXYSimResults::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_SelectXYfromXYSimResultsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_SelectXYfromXYSimResults::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_SelectXYfromXYSimResults::CalcOutput(FOcalcType  calcType)
{
    DFO_SelectXYfromXYSimResults::CalcOutput(calcType);

    indexStrings.DeAlloc();
    if (StatusOK())
        xySimResultsDC->GetCaseIDs(indexStrings);
    UpdateUI();
}



#endif //!ADCONSOLEAPP

