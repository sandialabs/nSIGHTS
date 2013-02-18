///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableStatistics.cpp
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
#include <objDPO/objDPOConfigFile.h>

#include "DPO_TableStatistics.h"
#ifndef ADCONSOLEAPP
#include "DPO_TableStatisticsPS.h"
#endif

static const char* objLongDesc = "Univariate Statistics of a Table Column";

DPO_TableStatistics :: DPO_TableStatistics() :
    DPO_Statistics(this, objLongDesc)
{
}

DPO_TableStatistics :: DPO_TableStatistics(const DPO_TableStatistics& a) :
    DFO_TableStatistics(a),
    DPO_Statistics(this, objLongDesc)
{
}

DPO_TableStatistics& DPO_TableStatistics::operator= (const DPO_TableStatistics& a)
{
    if (&a != this)
       *((DFO_TableStatistics*) this) = a;
    return *this;
}


bool  DPO_TableStatistics::ReadFromFile()
{
    ReadObjectHeader();
    ReadTableColSel(*this);
    DPO_Statistics::ReadFromFile(tableInputObjRef);

    CheckForInputFailure("Reading DPO_TableStatistics");
    return true;
}

void DPO_TableStatistics::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteTableColSel(*this);

    DPO_Statistics::WriteToFile();
}

#ifndef ADCONSOLEAPP

void DPO_TableStatistics::CalcOutput(FOcalcType  calcType)
{
    DFO_TableStatistics::CalcOutput(calcType);
    if (MenuUpdateOK())
        PropUpdate();
}

DPO_TableStatisticsPS& DPO_TableStatistics::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_TableStatisticsPS&>(*assocMenu);
}

void DPO_TableStatistics::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_TableStatisticsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_TableStatistics::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_TableStatistics::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

