///////////////////////////////////////////////////////////////////////////////////
//
// DPO_GridStatistics.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "DPO_GridStatistics.h"

#ifndef ADCONSOLEAPP
#include "DPO_GridStatisticsPS.h"
#endif

static const char* objLongDesc = "Univariate Statistics of Grid Data";

DPO_GridStatistics :: DPO_GridStatistics() :
    DPO_Statistics(this, objLongDesc)
{
}

DPO_GridStatistics :: DPO_GridStatistics(const DPO_GridStatistics& a) :
    DFO_GridStatistics(a),
    DPO_Statistics(this, objLongDesc)
{
}

DPO_GridStatistics& DPO_GridStatistics::operator= (const DPO_GridStatistics& a)
{
    if (&a != this)
       *((DFO_GridStatistics*) this) = a;
    return *this;
}


bool  DPO_GridStatistics::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(gridInputObjRef);
    DPO_Statistics::ReadFromFile(gridInputObjRef);

    CheckForInputFailure("Reading DPO_GridStatistics");
    return true;
}

void DPO_GridStatistics::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(gridInputObjRef);

    DPO_Statistics::WriteToFile();
    WriteLine();
}


#ifndef ADCONSOLEAPP

void DPO_GridStatistics::CalcOutput(FOcalcType  calcType)
{
    DFO_GridStatistics::CalcOutput(calcType);
    if (MenuUpdateOK())
        PropUpdate();
}

DPO_GridStatisticsPS& DPO_GridStatistics::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_GridStatisticsPS&>(*assocMenu);
}

void DPO_GridStatistics::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_GridStatisticsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_GridStatistics::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_GridStatistics::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

