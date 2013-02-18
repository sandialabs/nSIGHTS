///////////////////////////////////////////////////////////////////////////////////
//
// LPO_Covariance.cpp
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
//      File I/O and user interface for all osLFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "LPO_CovariancePS.h"
#include "LPO_Covariance.h"

const char* LPO_Covariance::objLongDesc = "Covariance";

LPO_Covariance :: LPO_Covariance(ListDefC* assLst) :
    LPO_Base(this, objLongDesc), LFO_Covariance(*assLst)
{
}

LPO_Covariance :: LPO_Covariance(ListDefC* assLst, const LPO_Covariance& a) :
    LFO_Covariance(a, *assLst),
    LPO_Base(this, objLongDesc)
{
}

LPO_Covariance& LPO_Covariance::operator= (const LPO_Covariance& a)
{
    if (&a != this)
       *((LFO_Covariance*) this) = a;

    return *this;
}

// ************************* menu ops

LPO_CovariancePS& LPO_Covariance::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<LPO_CovariancePS&>(*assocMenu);
}

void LPO_Covariance::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void LPO_Covariance::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new LPO_CovariancePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void LPO_Covariance::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  LPO_Covariance::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(covarObjRef);

    CheckForInputFailure("Reading LPO_Covariance");
    return true;
}

void LPO_Covariance::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(covarObjRef);
}
