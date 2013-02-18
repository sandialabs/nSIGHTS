///////////////////////////////////////////////////////////////////////////////////
//
// DFO_PulseNormalize.cpp
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

#include <iostream>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <nsDFO/DFO_PulseNormalize.h>

DFO_PulseNormalize :: DFO_PulseNormalize() :
    FuncObjC("PulseNormalize"),
    p0_MS("p0 value"),
    pI_MS("pi value")
{
    pulseNormalizationType = pntNormal;
    multiplyYbyX = false;
    InitCommon();
}

DFO_PulseNormalize::DFO_PulseNormalize(const DFO_PulseNormalize& a) :
    FuncObjC(a),
    p0_MS(a.p0_MS),
    pI_MS(a.pI_MS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_PulseNormalize :: ~DFO_PulseNormalize ()
{
}

void DFO_PulseNormalize::InitCommon()
{
    AddInPort(xyInputObjRef, typeid(DO_XYData));
    AddOutPort(outputDataDO);
    outputDataDO.xyData = &outputData;

    pI_MS.AddPort(*this);
    p0_MS.AddPort(*this);

    DoStatusChk();
}

DFO_PulseNormalize& DFO_PulseNormalize::operator= (const DFO_PulseNormalize& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_PulseNormalize::LocalCopy(const DFO_PulseNormalize& a)
{
    pulseNormalizationType = a.pulseNormalizationType;
    pI_MS = a.pI_MS;
    p0_MS = a.p0_MS;
    multiplyYbyX = a.multiplyYbyX;
}

void  DFO_PulseNormalize:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    inputData = 0;
    if (CheckInputFO(xyInputObjRef, "pulse input source"))
        inputData = static_cast<DO_XYData*>(GetInPortData(xyInputObjRef))->xyData;

    p0_MS.DoCheck("Initial pressure value source", *this);
    pI_MS.DoCheck("Static pressure value source", *this);
    double pIVal = pI_MS.GetCurrentValue();
    double pOVal = p0_MS.GetCurrentValue();
    if (RealsAreSame(pIVal, pOVal, stdEps))
        SetObjErrMsg("ABS(pI - PO) must > 1E-30");

}


void DFO_PulseNormalize:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (objStatus != os_OK)
        return;

    outputData.DeAlloc();
    if (!outputData.CreateFrom(inputData->xData, inputData->yData))
    {
        SetObjErrMsg("memory allocation ??");
        return;
    }

    double pIVal = pI_MS.GetCurrentValue();
    double dP = pIVal - p0_MS.GetCurrentValue();

    for (int i = 0; i < outputData.Size(); i++)
    {
        double pNorm = (pIVal - outputData.yData[i]) / dP;
        if (pulseNormalizationType == pntOneMinusNormal)
            pNorm = 1.0 - pNorm;

        if (multiplyYbyX)
            pNorm *= outputData.xData[i];

        outputData.yData[i] = pNorm;
    }

    outputData.SetID(GetID());
}

