///////////////////////////////////////////////////////////////////////////////////
//
// LPO_OptRun.cpp
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

#include "LPO_OptRunPS.h"
#include "LPO_OptRun.h"

const char* LPO_OptRun::objLongDesc = "List Optimization Run";

LPO_OptRun :: LPO_OptRun(ListDefC* assLst) :
    LPO_Base(this, objLongDesc), LFO_OptRun(*assLst)
{
}

LPO_OptRun :: LPO_OptRun(ListDefC* assLst, const LPO_OptRun& a) :
    LFO_OptRun(a, *assLst),
    LPO_Base(this, objLongDesc)
{
}

LPO_OptRun& LPO_OptRun::operator= (const LPO_OptRun& a)
{
    if (&a != this)
       *((LFO_OptRun*) this) = a;

    return *this;
}

// ************************* menu ops

LPO_OptRunPS& LPO_OptRun::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<LPO_OptRunPS&>(*assocMenu);
}

void LPO_OptRun::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void LPO_OptRun::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new LPO_OptRunPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void LPO_OptRun::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  LPO_OptRun::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(optSimResultsObjRef);

    if (currentObjReadMajorVersion > 1)
    {
        listRuns = ReadBool();
        ReadIntArray(casesToList);
    }
    ReadIntArray(runsToList);

    if (currentObjReadMajorVersion == 0)
    {
        bool useAllCovarBasis = ReadBool();
        int covarianceBasis  = CovarianceBasis(ReadInt());
    }
    else
    {
        csActual        = ReadBool();
        csEstimated     = ReadBool();
        csFitComponent  = ReadBool();
    }

    mosListAll          = ReadBool();
    mosListBest         = ReadBool();
    mosListStatistics   = ReadBool();

    listRunSummary                      = ReadBool();
    listFitValues                       = ReadBool();
    listParameterEstimates              = ReadBool();
    listEstimatedDataVariances          = ReadBool();
    listParameterCorrelations           = ReadBool();
    listNormalizedCovarianceMatrices    = ReadBool();
    listDenormalizedCovarianceMatrices  = ReadBool();
    list95ConfidenceLimits              = ReadBool();
    listJacobian                        = ReadBool();
    listModelStructure                  = ReadBool();
    listCovarianceMatrixCorrelations    = ReadBool();

    CheckForInputFailure("Reading LPO_OptRun");
    return true;
}

void LPO_OptRun::WriteToFile()
{

    const int objMajVer = 2;  // added single selection
//    const int objMajVer = 1;  // changed covar selection
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(optSimResultsObjRef);

    WriteBool(listRuns);
    WriteLine();
    WriteIntArray(casesToList);
    WriteIntArray(runsToList);

    WriteBool(csActual);
    WriteBool(csEstimated);
    WriteBool(csFitComponent);

    WriteBool(mosListAll);
    WriteBool(mosListBest);
    WriteBool(mosListStatistics);
    WriteLine();

    WriteBool(listRunSummary);
    WriteBool(listFitValues);
    WriteBool(listParameterEstimates);
    WriteBool(listEstimatedDataVariances);
    WriteBool(listParameterCorrelations);
    WriteBool(listNormalizedCovarianceMatrices);
    WriteBool(listDenormalizedCovarianceMatrices);
    WriteBool(list95ConfidenceLimits);
    WriteBool(listJacobian);
    WriteBool(listModelStructure);
    WriteBool(listCovarianceMatrixCorrelations);
    WriteLine();

}


