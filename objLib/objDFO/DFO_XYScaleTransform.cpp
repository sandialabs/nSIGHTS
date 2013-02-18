///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYScaleTransform.cpp
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
//      provides extended scale/transform operations for either X or Y components.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <objDFO/DFO_XYScaleTransform.h>

DFO_XYScaleTransform :: DFO_XYScaleTransform() :
    DFO_ScaleTransform("Sc/Tran X or Y")
{
    outputTypeDesc[0] = '\0';
    InitCommon();
}

DFO_XYScaleTransform::DFO_XYScaleTransform(const DFO_XYScaleTransform& a) :
    DFO_ScaleTransform(a), DSC_XYColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYScaleTransform :: ~DFO_XYScaleTransform ()
{
}

void DFO_XYScaleTransform::InitCommon()
{
    DSC_XYColSel::AddPort(*this);
    outputDataDO.xyData = &outputData;
    AddOutPort(outputDataDO);
    DoStatusChk();
}

DFO_XYScaleTransform& DFO_XYScaleTransform::operator= (const DFO_XYScaleTransform& a)
{
    if (&a != this)
    {
        DFO_ScaleTransform::operator=(a);
        DSC_XYColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYScaleTransform::LocalCopy(const DFO_XYScaleTransform& a)
{
    thresholdOp     = a.thresholdOp;

    CopyString(outputTypeDesc,  a.outputTypeDesc, descLen);
}

void  DFO_XYScaleTransform::DoStatusChk()
{
    outputData.SetID("S/T output");
    DFO_ScaleTransform::DoStatusChk();
    DSC_XYColSel::DoCheck(*this);
}

void DFO_XYScaleTransform::DoScaleTransform(SC_DoubleArray& stData)
{
    for (int i = 0; i < stData.Size(); i++)
    {
        DSC_ScaleTransform::DoScaleTransform(stData[i]);
        stData[i] = thresholdOp.ApplyThresholds(stData[i]);
        if (processNulls && RealIsNull(stData[i]))
            stData[i] = setNullValue;
    }
}


void DFO_XYScaleTransform:: CalcOutput(FOcalcType  calcType)
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

    if (operateOnX)
        DoScaleTransform(outputData.xData);
    else
        DoScaleTransform(outputData.yData);

    // remove nulls
    outputData.CleanUp();
    if (outputData.IsEmpty())
    {
        SetObjErrMsg("Results are all null");
        outputData.DeAlloc();
        return;
    }

    //  update ID
    if (StringLength(outputTypeDesc) > 0)
    {
        outputData.SetID(outputTypeDesc);
//      SetDefaultID(outputData.dataID);
    }
    else
        outputData.SetID(GetID());

}

