///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectRangeCube.cpp
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
//      extracts a single cube from three multiple parameter value results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/SC_DoubleArray.h>

#include <osDFO/DFO_SelectRangeCube.h>

DFO_SelectRangeCube :: DFO_SelectRangeCube() : DFO_SelectRange("SelectRangeCube")
{
    InitCommon();
}

DFO_SelectRangeCube::DFO_SelectRangeCube(const DFO_SelectRangeCube& a) : DFO_SelectRange(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectRangeCube::InitCommon()
{
    cubeDataDO.cubeData = &cubeDataDC;
    AddOutPort(cubeDataDO);

    DoStatusChk();
}

DFO_SelectRangeCube& DFO_SelectRangeCube::operator= (const DFO_SelectRangeCube& a)
{
    if (&a != this)
    {
        DFO_SelectRange::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectRangeCube::LocalCopy(const DFO_SelectRangeCube& a)
{
}

void  DFO_SelectRangeCube:: DoStatusChk()
{
    DFO_SelectRange::DoStatusChk();
    indexMS.indexDO.SetTypeLabel("Selected Output");

    // make sure that file contains covariance
    if (StatusOK() && (!rangeSimResultsDC->IsCubes()))
        SetObjErrMsg("no cubes in input");
}

void DFO_SelectRangeCube:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (!(*rangeSimResultsDC)[runIndex].GetCubeData(outputIndex, cubeDataDC))
        SetObjErrMsg("cubes transcription error ??");
}

