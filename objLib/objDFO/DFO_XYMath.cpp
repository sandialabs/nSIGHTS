///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYMath.cpp
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
//      Basic math (+-/*) on X or Y components of two input XY
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genClass/SC_DoubleArray.h>
#include <objDFO/DFO_XYMath.h>

DFO_XYMath :: DFO_XYMath() : DFO_MathBase("XY Math")
{
    operateOnX     = false;
    InitCommon();
}

DFO_XYMath::DFO_XYMath(const DFO_XYMath& a) : DFO_MathBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYMath :: ~DFO_XYMath ()
{
}

void DFO_XYMath::InitCommon()
{
    AddInPort(dataInputAObjRef, typeid(DO_XYData));
    AddInPort(dataInputBObjRef, typeid(DO_XYData));
    outputDataDO.xyData = &outputData;
    AddOutPort(outputDataDO);
    DoStatusChk();
}

DFO_XYMath& DFO_XYMath::operator= (const DFO_XYMath& a)
{
    if (&a != this)
    {
        DFO_MathBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYMath::LocalCopy(const DFO_XYMath& a)
{
    dataInputAObjRef = a.dataInputAObjRef;
    dataInputBObjRef = a.dataInputBObjRef;
    operateOnX      = a.operateOnX;
}

void  DFO_XYMath:: DoStatusChk()
{
    outputData.SetID("math results");

    FuncObjC::DoStatusChk();
    inputDataA = 0;
    inputDataB = 0;

    if (CheckInputFO(dataInputAObjRef, "input data A source"))
        inputDataA = static_cast<DO_XYData*>(GetInPortData(dataInputAObjRef))->xyData;

    if (CheckInputFO(dataInputBObjRef, "input data B source"))
        inputDataB = static_cast<DO_XYData*>(GetInPortData(dataInputBObjRef))->xyData;

    if (StatusNotOK())
        return;

    if (inputDataA->Size() != inputDataB->Size())
        SetObjErrMsg("A and B must be the same size");
}


void DFO_XYMath:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    // basic copy
    try
    {
        outputData.xData = inputDataA->xData;
        outputData.yData = inputDataA->yData;
    }
    catch (SC_Array::AllocationError& )
    {
        SetObjErrMsg("allocation error ??");
        outputData.DeAlloc();
        return;
    }


    //  egregious error check -- should probably be internal errror
    if (outputData.yData.AllocSize() < outputData.xData.Size())
    {
        SetObjErrMsg("size mismatch ??");
        outputData.DeAlloc();
        return;
    }


    const SC_DoubleArray* dataA;
    const SC_DoubleArray* dataB;
    SC_DoubleArray* result;

    if (operateOnX) 
    {
        dataA = &inputDataA->xData;
        dataB = &inputDataB->xData;
        result = &outputData.xData;
    } 
    else 
    {
        dataA = &inputDataA->yData;
        dataB = &inputDataB->yData;
        result = &outputData.yData;
    }

    for ( int i = 0; i < outputData.Size(); i++)
    {
        (*result)[i] = MathResult((*dataA)[i], (*dataB)[i]);
    }

    // remove nulls
    outputData.CleanUp();
    if (outputData.IsEmpty())
    {
        SetObjErrMsg("Results are all null");
        outputData.DeAlloc();
        return;
    }

    outputData.SetID(GetCurrID());

}

