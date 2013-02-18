///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYDualScaleTransform.cpp
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
//      Provides basic scale/transform operations for both X and Y components
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objDFO/DFO_XYDualScaleTransform.h>

DFO_XYDualScaleTransform :: DFO_XYDualScaleTransform() :
    FuncObjC("Sc/Tran X and Y"),
    xoffsetMS("X offset value"),
    xscaleMS("X scale value"),
    yoffsetMS("Y offset value"),
    yscaleMS("Y scale value")

{
    outputTypeDesc[0] = '\0';
    positiveXOnly = false;

    xscaleMS.enteredRealValue = 1.0;
    xoffsetMS.enteredRealValue = 0.0;
    yscaleMS.enteredRealValue = 1.0;
    yoffsetMS.enteredRealValue = 0.0;

    InitCommon();
}

DFO_XYDualScaleTransform::DFO_XYDualScaleTransform(const DFO_XYDualScaleTransform& a) :
    FuncObjC(a),
    xoffsetMS(a.xoffsetMS),
    xscaleMS(a.xscaleMS),
    yoffsetMS(a.yoffsetMS),
    yscaleMS(a.yscaleMS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYDualScaleTransform :: ~DFO_XYDualScaleTransform ()
{
}

void DFO_XYDualScaleTransform::InitCommon()
{
    xoffsetMS.AddPort(*this);
    xscaleMS.AddPort(*this);
    yoffsetMS.AddPort(*this);
    yscaleMS.AddPort(*this);

    AddInPort(xyInputObjRef, typeid(DO_XYData));
    AddOutPort(outputDataDO);
    outputDataDO.xyData = &outputData;
    DoStatusChk();
}

DFO_XYDualScaleTransform& DFO_XYDualScaleTransform::operator= (const DFO_XYDualScaleTransform& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        xscaleMS  = a.xscaleMS;
        xoffsetMS = a.xoffsetMS;
        yscaleMS  = a.yscaleMS;
        yoffsetMS = a.yoffsetMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYDualScaleTransform::LocalCopy(const DFO_XYDualScaleTransform& a)
{
    xST = a.xST;
    yST = a.yST;
    positiveXOnly = a.positiveXOnly;
    CopyString(outputTypeDesc,  a.outputTypeDesc, descLen);
}

void  DFO_XYDualScaleTransform::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    outputData.SetID("Dual S/T output");

    xoffsetMS.DoCheck("X Offset Value Source", *this);
    xscaleMS.DoCheck("X Scale Value Source", *this);
    yoffsetMS.DoCheck("Y Offset Value Source", *this);
    yscaleMS.DoCheck("Y Scale Value Source", *this);

    xST.offsetVal = xoffsetMS.GetCurrentValue();
    xST.scaleVal = xscaleMS.GetCurrentValue();
    yST.offsetVal = yoffsetMS.GetCurrentValue();
    yST.scaleVal = yscaleMS.GetCurrentValue();

    if ((fabs(xscaleMS.GetCurrentValue()) < stdEps) || (fabs(yscaleMS.GetCurrentValue()) < stdEps))
        SetObjErrMsg("ABS(scale) must > 1E-30");

    inputData = 0;
    if (CheckInputFO(xyInputObjRef, "input data source"))
        inputData = static_cast<DO_XYData*>(GetInPortData(xyInputObjRef))->xyData;
}


void DFO_XYDualScaleTransform:: CalcOutput(FOcalcType  calcType)
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

    int nOK = 0;
    for ( int i = 0; i < outputData.Size(); i++)
    {
        double xVal = outputData.xData[i];
        double yVal = outputData.yData[i];
        xST.DoScaleTransform(xVal);
        yST.DoScaleTransform(yVal);

        if (positiveXOnly && (xVal <= 0.0))
            continue;
        if (RealIsNull(xVal) || RealIsNull(yVal))
            continue;

        outputData.xData[nOK] = xVal;
        outputData.yData[nOK] = yVal;
        nOK++;
    }
    outputData.SetSize(nOK);


    // remove nulls
    if (outputData.IsEmpty())
    {
        SetObjErrMsg("Results are all null");
        outputData.DeAlloc();
        return;
    }

    //  update ID
    if (StringLength(outputTypeDesc) > 0)
        outputData.SetID(outputTypeDesc);
    else
        outputData.SetID(GetID());

}

