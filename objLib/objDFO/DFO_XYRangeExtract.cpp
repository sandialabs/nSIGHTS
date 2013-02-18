///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYRangeExtract.cpp
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
//      Reduces XY data based on range of values of either X or Y components
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <objDFO/DFO_XYRangeExtract.h>

DFO_XYRangeExtract :: DFO_XYRangeExtract() :
    DFO_RangeExtract("Extract XY")
{
    outputTypeDesc[0] = '\0';
    operateOnX = true;

    InitCommon();
}

DFO_XYRangeExtract::DFO_XYRangeExtract(const DFO_XYRangeExtract& a) :
    DFO_RangeExtract(a), DSC_XYColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYRangeExtract :: ~DFO_XYRangeExtract ()
{
}

void DFO_XYRangeExtract::InitCommon()
{
    DSC_XYColSel::AddPort(*this);
    outputDataDO.xyData = &outputData;
    AddOutPort(outputDataDO);
    DoStatusChk();
}

DFO_XYRangeExtract& DFO_XYRangeExtract::operator= (const DFO_XYRangeExtract& a)
{
    if (&a != this)
    {
        DFO_RangeExtract::operator=(a);
        DSC_XYColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYRangeExtract::LocalCopy(const DFO_XYRangeExtract& a)
{
    CopyString(outputTypeDesc,  a.outputTypeDesc, descLen);
}

void  DFO_XYRangeExtract::DoStatusChk()
{
    outputData.SetID("extraction");
    DFO_RangeExtract::DoStatusChk();
    DSC_XYColSel::DoCheck(*this);
}


void DFO_XYRangeExtract:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    outputData.DeAlloc();
    if (!outputData.CreateFrom(inputData->xData, inputData->yData))
    {
        SetObjErrMsg("memory allocation ??");
        return;
    }

    SC_DoubleArray* chkData;
    if (operateOnX)
        chkData = &outputData.xData;
    else
        chkData = &outputData.yData;

    int nOK = 0;
    InitStatusVals();
    for (int i = 0; i < outputData.Size(); i++)
    {
        if (ValueInRange((*chkData)[i]))
        {
            outputData.xData[nOK] = outputData.xData[i];
            outputData.yData[nOK++] = outputData.yData[i];
        }
    }
    outputData.SetSize(nOK);

    //  update ID
    if (StringLength(outputTypeDesc) > 0)
    {
        outputData.SetID(outputTypeDesc);
//      SetDefaultID(outputData.dataID);
    }
    else
        outputData.SetID(GetID());

}

