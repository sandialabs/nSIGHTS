///////////////////////////////////////////////////////////////////////////////////
//
// DFO_GridScaleTransform.cpp
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
//      Scale/transform all data in a grid.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <objDFO/DFO_GridScaleTransform.h>

DFO_GridScaleTransform :: DFO_GridScaleTransform() :
    DFO_ScaleTransform("Sc/Tran Grid")
{
    SetToNull(outputTypeDesc);
    averageGrid = false;
    replaceNullsWithAverage = false;
    averageKernelSize = 1;
    InitCommon();
}

DFO_GridScaleTransform::DFO_GridScaleTransform(const DFO_GridScaleTransform& a) :
    DFO_ScaleTransform(a), DSC_GridInput(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_GridScaleTransform :: ~DFO_GridScaleTransform ()
{
}

void DFO_GridScaleTransform::InitCommon()
{
    DSC_GridInput::AddPort(*this);
    outputDataDO.gridData = &outputData;
    AddOutPort(outputDataDO);
    DoStatusChk();
}

DFO_GridScaleTransform& DFO_GridScaleTransform::operator= (const DFO_GridScaleTransform& a)
{
    if (&a != this)
    {
        DFO_ScaleTransform::operator=(a);
        DSC_GridInput::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_GridScaleTransform::LocalCopy(const DFO_GridScaleTransform& a)
{
    thresholdOp     = a.thresholdOp;
    averageGrid     = a.averageGrid;
    averageKernelSize = a.averageKernelSize;
    replaceNullsWithAverage = a.replaceNullsWithAverage;

    CopyString(outputTypeDesc,  a.outputTypeDesc, descLen);
}

void  DFO_GridScaleTransform::DoStatusChk()
{
    DFO_ScaleTransform::DoStatusChk();
    DSC_GridInput::DoCheck(*this);
}


void DFO_GridScaleTransform:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (!outputData.CreateFrom(*gridInputDC))
    {
        SetObjErrMsg("memory allocation ??");
        return;
    }

    for (int i = 0; i < outputData.Size(); i++)
    {
        DSC_ScaleTransform::DoScaleTransform(outputData[i]);
        outputData[i] = thresholdOp.ApplyThresholds(outputData[i]);
    }

    if (averageGrid)
    {
        DC_GridData  tempOut;
        if (!tempOut.CreateFrom(outputData))
        {
            SetObjErrMsg("memory allocation ??");
            return;
        }


        for (int i = 0; i < outputData.xData.Size(); i++)
        {
            int istart = i - averageKernelSize;
            if (istart < 0)
                istart = 0;
            int iend = i + averageKernelSize;
            if (iend > outputData.xData.UpperBound())
                iend = outputData.xData.UpperBound();

            for (int j = 0; j < outputData.yData.Size(); j++)
            {
                if (replaceNullsWithAverage && (!RealIsNull(outputData.gData[i][j])))
                    continue;

                int jstart = j - averageKernelSize;
                if (jstart < 0)
                    jstart = 0;
                int jend = j + averageKernelSize;
                if (jend > outputData.yData.UpperBound())
                    jend = outputData.yData.UpperBound();

                double avgVal = 0.0;
                int nAvg = 0;
                for (int is = istart; is <= iend; is++)
                    for (int js = jstart; js <= jend; js++)
                    {
                        double temp = tempOut.gData[is][js];
                        if (!RealIsNull(temp))
                        {
                            avgVal += temp;
                            nAvg++;
                        }
                    }

                if (nAvg > 0)
                    avgVal /= double(nAvg);
                else
                    avgVal = nullReal;

                outputData.gData[i][j] = avgVal;
            }
        }
    }

    if (processNulls)
    {
        for (int i = 0; i < outputData.Size(); i++)
            if (RealIsNull(outputData[i]))
                outputData[i] = setNullValue;
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

