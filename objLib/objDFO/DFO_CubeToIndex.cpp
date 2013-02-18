///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CubeToIndex.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <objClass/DO_CubeData.h>

#include <objDFO/DFO_CubeToIndex.h>

DFO_CubeToIndex :: DFO_CubeToIndex() :
    DFO_RangeExtract("Extract Cube Indexes")
{
    InitCommon();
}

DFO_CubeToIndex::DFO_CubeToIndex(const DFO_CubeToIndex& a) :
    DFO_RangeExtract(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_CubeToIndex :: ~DFO_CubeToIndex ()
{
}

void DFO_CubeToIndex::InitCommon()
{
    AddInPort(cubeObjRef, typeid(DO_CubeData));
    AddOutPort(outputIndexDO);
    DoStatusChk();
}

DFO_CubeToIndex& DFO_CubeToIndex::operator= (const DFO_CubeToIndex& a)
{
    if (&a != this)
    {
        DFO_RangeExtract::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_CubeToIndex::LocalCopy(const DFO_CubeToIndex& a)
{
    cubeObjRef    = a.cubeObjRef;
}

void  DFO_CubeToIndex:: DoStatusChk()
{
    DFO_RangeExtract::DoStatusChk();

    cubeDataDC = 0;
    outputIndexDO.Reset("Cube data");

    CheckInputFO(cubeObjRef, "input data source");
    if (StatusNotOK())
        return;

    cubeDataDC = static_cast<DO_CubeData*>(GetInPortData(cubeObjRef))->cubeData;
    outputIndexDO.baseCube      = cubeDataDC;
}

void DFO_CubeToIndex:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    SC_IntArray& outputData = outputIndexDO.dataIndexes;
    int ndata = cubeDataDC->Size();
    if (!outputData.ChkAlloc(ndata))
    {
        SetObjErrMsg("insufficient memory");
        return;
    }

    InitStatusVals();
    for (int i = 0; i < ndata; i++)
        if (ValueInRange((*cubeDataDC)[i]))
            outputData += i;

    if (outputData.IsEmpty())
    {
        ApplyWarningMsg(calcType, "No data passed test");
    }

}

