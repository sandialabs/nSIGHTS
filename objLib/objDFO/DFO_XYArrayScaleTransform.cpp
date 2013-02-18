///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYArrayScaleTransform.cpp
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
//      Provides basic scale/transform operations for all data in an XY array.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <objDFO/DFO_XYArrayScaleTransform.h>

DFO_XYArrayScaleTransform :: DFO_XYArrayScaleTransform() :
    DFO_ScaleTransform("Sc/Tran XY Array")
{
    operateOnX = false;
    InitCommon();
}

DFO_XYArrayScaleTransform::DFO_XYArrayScaleTransform(const DFO_XYArrayScaleTransform& a) :
    DFO_ScaleTransform(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYArrayScaleTransform :: ~DFO_XYArrayScaleTransform ()
{
}

void DFO_XYArrayScaleTransform::InitCommon()
{
    AddInPort(xyArrayInputObjRef, typeid(DO_XYDataArray));
    outputDataDO.xyDataArray = &outputDataDC;
    AddOutPort(outputDataDO);
    DoStatusChk();
}

DFO_XYArrayScaleTransform& DFO_XYArrayScaleTransform::operator= (const DFO_XYArrayScaleTransform& a)
{
    if (&a != this)
    {
        DFO_ScaleTransform::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYArrayScaleTransform::LocalCopy(const DFO_XYArrayScaleTransform& a)
{
    thresholdOp         = a.thresholdOp;
    operateOnX          = a.operateOnX;
    xyArrayInputObjRef  = a.xyArrayInputObjRef;
}

void  DFO_XYArrayScaleTransform::DoStatusChk()
{
    DFO_ScaleTransform::DoStatusChk();

    if (CheckInputFO(xyArrayInputObjRef, "XY array input data source"))
    {
        inputDataDC = static_cast<DO_XYDataArray*>(GetInPortData(xyArrayInputObjRef))->xyDataArray;
        if (inputDataDC->IsEmpty())
            SetObjErrMsg("no data in input array");
    }

    if (StatusNotOK())
        outputDataDC.DeAlloc();

}

void DFO_XYArrayScaleTransform::DoScaleTransform(SC_DoubleArray& stData)
{
    for (int i = 0; i < stData.Size(); i++)
    {
        DSC_ScaleTransform::DoScaleTransform(stData[i]);
        stData[i] = thresholdOp.ApplyThresholds(stData[i]);
        if (processNulls && RealIsNull(stData[i]))
            stData[i] = setNullValue;
    }
}


void DFO_XYArrayScaleTransform:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int nArray = inputDataDC->Size();
    outputDataDC.AllocAndSetSize(nArray);

    int i;
    for (i = 0; i < nArray; i++)
        if (!outputDataDC[i].CreateFrom((*inputDataDC)[i].xData, (*inputDataDC)[i].yData))
        {
            SetObjErrMsg("memory allocation ??");
            return;
        }

    bool allNulls = true;
    for (i = 0; i < nArray; i++)
    {
        if (operateOnX)
            DoScaleTransform(outputDataDC[i].xData);
        else
            DoScaleTransform(outputDataDC[i].yData);

        // remove nulls
        outputDataDC[i].CleanUp();
        allNulls = allNulls && outputDataDC[i].IsEmpty();
    }

    if (allNulls)
    {
        SetObjErrMsg("Results are all null");
        outputDataDC.DeAlloc();
    }

}

