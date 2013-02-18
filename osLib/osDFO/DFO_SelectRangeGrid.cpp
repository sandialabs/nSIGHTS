///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectRangeGrid.cpp
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
//      extracts a single grid from two multiple parameter value results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/SC_DoubleArray.h>

#include <osDFO/DFO_SelectRangeGrid.h>

DFO_SelectRangeGrid :: DFO_SelectRangeGrid() : DFO_SelectRange("SelectRangeGrid")
{
    InitCommon();
}

DFO_SelectRangeGrid::DFO_SelectRangeGrid(const DFO_SelectRangeGrid& a) : DFO_SelectRange(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectRangeGrid::InitCommon()
{
    gridDataDO.gridData = &gridDataDC;
    AddOutPort(gridDataDO);

    DoStatusChk();
}

DFO_SelectRangeGrid& DFO_SelectRangeGrid::operator= (const DFO_SelectRangeGrid& a)
{
    if (&a != this)
    {
        DFO_SelectRange::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectRangeGrid::LocalCopy(const DFO_SelectRangeGrid& a)
{
}

void  DFO_SelectRangeGrid:: DoStatusChk()
{
    DFO_SelectRange::DoStatusChk();
    indexMS.indexDO.SetTypeLabel("Selected Output");

    // make sure that file contains covariance
    if (StatusOK() && (rangeSimResultsDC->IsCubes()))
        SetObjErrMsg("no grids in input");
}

void DFO_SelectRangeGrid:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (!(*rangeSimResultsDC)[runIndex].GetGridData(outputIndex, gridDataDC))
        SetObjErrMsg("grid transcription error ??");
}

