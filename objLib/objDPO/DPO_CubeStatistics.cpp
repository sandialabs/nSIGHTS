///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CubeStatistics.cpp
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

#include "DPO_CubeStatistics.h"
#ifndef ADCONSOLEAPP
#include "DPO_CubeStatisticsPS.h"
#endif

static const char* objLongDesc = "Univariate Statistics of Cube Data";

DPO_CubeStatistics :: DPO_CubeStatistics() :
    DPO_Statistics(this, objLongDesc)
{
}

DPO_CubeStatistics :: DPO_CubeStatistics(const DPO_CubeStatistics& a) :
    DFO_CubeStatistics(a),
    DPO_Statistics(this, objLongDesc)
{
}

DPO_CubeStatistics& DPO_CubeStatistics::operator= (const DPO_CubeStatistics& a)
{
    if (&a != this)
       *((DFO_CubeStatistics*) this) = a;
    return *this;
}


bool  DPO_CubeStatistics::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(cubeInputObjRef);
    DPO_Statistics::ReadFromFile(cubeInputObjRef);

    CheckForInputFailure("Reading DPO_CubeStatistics");
    return true;
}

void DPO_CubeStatistics::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(cubeInputObjRef);

    DPO_Statistics::WriteToFile();
    WriteLine();
}

#ifndef ADCONSOLEAPP

void DPO_CubeStatistics::CalcOutput(FOcalcType  calcType)
{
    DFO_CubeStatistics::CalcOutput(calcType);
    if (MenuUpdateOK())
        PropUpdate();
}

DPO_CubeStatisticsPS& DPO_CubeStatistics::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CubeStatisticsPS&>(*assocMenu);
}

void DPO_CubeStatistics::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CubeStatisticsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_CubeStatistics::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_CubeStatistics::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

