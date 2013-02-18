///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadRangeSimResults.cpp
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
//      reads multiple parameter value results from file.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>

#include <osDFO/DFO_ReadRangeSimResults.h>

DFO_ReadRangeSimResults :: DFO_ReadRangeSimResults() :
    FuncObjC("ReadRangeSimResults")
{
    InitCommon();
}

DFO_ReadRangeSimResults::DFO_ReadRangeSimResults(const DFO_ReadRangeSimResults& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadRangeSimResults::InitCommon()
{
    rangeSimResultsDO.rangeResults = &rangeSimResults;
    fileNameDO.SetTypeLabel("Range results");
    AddOutPort(rangeSimResultsDO);
    AddOutPort(fileNameDO);
    DoStatusChk();
}

DFO_ReadRangeSimResults& DFO_ReadRangeSimResults::operator= (const DFO_ReadRangeSimResults& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadRangeSimResults::LocalCopy(const DFO_ReadRangeSimResults& a)
{
    CopyString(rangeSimResultsIO.fileName, a.rangeSimResultsIO.fileName, stdFileStrLen);
}

void  DFO_ReadRangeSimResults:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (!rangeSimResultsDO.DataOK())
        SetObjErrMsg("No data read");
}

void DFO_ReadRangeSimResults:: CalcOutput(FOcalcType  calcType)
{
    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        rangeSimResults.DeAlloc();
        objStatus = os_OK;

        if (!CheckFileExists(rangeSimResultsIO.fileName))
        {
            SetObjErrMsg("File does not exist ??");
            return;
        }

        if (!rangeSimResultsIO.ReadSimResults(rangeSimResults))
        {
            SetObjErrMsg("Error opening file (is it read-only) ??");
            return;
        }

        SetDefaultFileID(rangeSimResultsIO.fileName);
        fileNameDO.SetFileValueLabel(rangeSimResultsIO.fileName);
    }

    DoStatusChk();
    if (StatusNotOK())
    {
        rangeSimResults.DeAlloc();
        return;
    }
}

