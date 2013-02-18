///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableRowStatistics.cpp
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

#include "DPO_TableRowStatistics.h"

#ifndef ADCONSOLEAPP
#include "DPO_TableRowStatisticsPS.h"
#endif

const char* DPO_TableRowStatistics::objLongDesc = "Univariate Statistics of Table Rows";

DPO_TableRowStatistics :: DPO_TableRowStatistics() : MenuObjC(this, objLongDesc)
{
}

DPO_TableRowStatistics :: DPO_TableRowStatistics(const DPO_TableRowStatistics& a) :
    DFO_TableRowStatistics(a),
    MenuObjC(this, objLongDesc)
{
}


DPO_TableRowStatistics& DPO_TableRowStatistics::operator= (const DPO_TableRowStatistics& a)
{
    if (&a != this)
       *((DFO_TableRowStatistics*) this) = a;
    return *this;
}


bool  DPO_TableRowStatistics::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(inputTableObjRef);


    skipXColumn     = ReadBool();
    xcolumnIndex    = ReadInt();

    outputAllStats          = ReadBool();
    outputConfidenceLimits  = ReadBool();
    outputMedian            = ReadBool();

    if (currentObjReadMajorVersion > 0)
    {
        outputCDF99     = ReadBool();
        outputCDF95     = ReadBool();
        outputCDF90     = ReadBool();

        if (currentObjReadMajorVersion > 1)
        {
            quantileType    = DC_XYCDF::QuantileType(ReadInt());
            genericQuantileValue = ReadDouble();
        }
    }

    CheckForInputFailure("Reading DPO_TableRowStatistics");
    return true;
}


void DPO_TableRowStatistics::WriteToFile()
{
    const int objMajVer = 2; // added quantile options
//    const int objMajVer = 1; // added CDF calcs
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(inputTableObjRef);

    WriteBool(skipXColumn);
    WriteInt(xcolumnIndex);

    WriteBool(outputAllStats);
    WriteBool(outputConfidenceLimits);
    WriteBool(outputMedian);

    WriteBool(outputCDF99); // added v1
    WriteBool(outputCDF95);
    WriteBool(outputCDF90);

    WriteInt(quantileType);
    WriteDouble(genericQuantileValue);

    WriteLine();
}

#ifndef ADCONSOLEAPP

DPO_TableRowStatisticsPS& DPO_TableRowStatistics::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_TableRowStatisticsPS&>(*assocMenu);
}


void DPO_TableRowStatistics::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_TableRowStatistics::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_TableRowStatisticsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_TableRowStatistics::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


#endif //!ADCONSOLEAPP

