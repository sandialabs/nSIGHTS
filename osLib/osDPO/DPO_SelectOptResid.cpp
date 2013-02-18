///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectOptResid.cpp
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
#include "DPO_SelectOptResidPS.h"
#endif
#include "DPO_SelectOptResid.h"

const char* DPO_SelectOptResid::objLongDesc = "Select Residuals";

DPO_SelectOptResid :: DPO_SelectOptResid() :
    DPO_SelectOpt(this, objLongDesc, "Resid Index")
{
}

DPO_SelectOptResid :: DPO_SelectOptResid(const DPO_SelectOptResid& a) :
    DFO_SelectOptResid(a),
    DPO_SelectOpt(this, objLongDesc, "Resid Index")
{
}

DPO_SelectOptResid& DPO_SelectOptResid::operator= (const DPO_SelectOptResid& a)
{
    if (&a != this)
       *((DFO_SelectOptResid*) this) = a;
    return *this;
}


bool  DPO_SelectOptResid::ReadFromFile()
{
    ReadObjectHeader();

    return DPO_SelectOpt::ReadFromFile(currentObjReadMajorVersion == 0);
}


void DPO_SelectOptResid::WriteToFile()
{
    const int objMajVer = 1; // new MS spec
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    DPO_SelectOpt::WriteToFile();
}

#ifndef ADCONSOLEAPP

// ************************* menu stuff


DPO_SelectOptResidPS& DPO_SelectOptResid::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_SelectOptResidPS&>(*assocMenu);
}


void DPO_SelectOptResid::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_SelectOptResid::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_SelectOptResidPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_SelectOptResid::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_SelectOptResid::CalcOutput(FOcalcType  calcType)
{
    DFO_SelectOptResid::CalcOutput(calcType);
    DPO_SelectOpt::UpdateUI();
}

#endif //!ADCONSOLEAPP

