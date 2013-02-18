///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableQuantile.cpp
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

#include <genClass/DO_TableData.h>

#include <genClass/U_Real.h>

#include <objDFO/DFO_TableQuantile.h>


DFO_TableQuantile :: DFO_TableQuantile() : DFO_Quantile("Table CDF")
{
    areaWeighted = false;
    areaColumn = 1;
    InitCommon();
}

DFO_TableQuantile::DFO_TableQuantile(const DFO_TableQuantile& a) :
    DFO_Quantile(a),
    DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableQuantile :: ~DFO_TableQuantile ()
{
}

void DFO_TableQuantile::InitCommon()
{
    DSC_TableColSel::AddPort(*this);
    DoStatusChk();
}

DFO_TableQuantile& DFO_TableQuantile::operator= (const DFO_TableQuantile& a)
{
    if (&a != this)
    {
        DFO_Quantile::operator=(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableQuantile::LocalCopy(const DFO_TableQuantile& a)
{
    areaWeighted = a.areaWeighted;
    areaColumn = a.areaColumn;
}


void  DFO_TableQuantile:: DoStatusChk()
{
    DFO_Quantile::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
    if (StatusOK() && areaWeighted)
    {
        if (areaColumn == selectedCol)
            SetObjErrMsg("area column must be different than data column");

        if (inputData->dataTable.Size() <= areaColumn)
            SetObjErrMsg("area column index out of range");
        else
            areaData = &inputData->dataTable[areaColumn];
    }
}


void DFO_TableQuantile:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    SetDefaultID(inputData->columnDesc[selectedCol]);   

    if (areaWeighted)
    {
        SC_DoubleArray tempD(*selectedColData);
        SC_DoubleArray tempA(*areaData);
        int ndata = tempD.Size();
        int nOK = 0;
        double normVal = 0.0;
        for (int i = 0; i < ndata; i++)
        {
            double nextD = tempD[i];
            double nextA = tempA[i];
            if (RealIsNull(nextD) || RealIsNull(nextA))
                continue;
            if (logData)
            {
                if (!IsPositive(nextD))
                {
                    SetObjErrMsg("non-positive data value -- cannot log");
                    return;
                }
                nextD = log(nextD);
            }
            if (!IsPositive(nextA))
            {
                SetObjErrMsg("non-positive area value -- cannot weight");
                return;
            }
            normVal += nextA;
            tempD[nOK] = nextD;
            tempA[nOK++] = nextA;
        }

        tempD.SetSize(nOK);
        tempA.SetSize(nOK);
        SC_IntArray indexes;
        indexes.AllocAndIndex(nOK);

        tempD.Sort(true, indexes);
        tempA.Reorder(indexes);


        // alloc output
        if (!outputCDFDC.Alloc(nOK))
        {
            SetObjErrMsg("output allocation ??");
            return;
        }

        outputCDFDC.xData = tempD;
        outputCDFDC.SetSize(nOK);
        SC_DoubleArray& normA = outputCDFDC.yData;
        double lastVal = 0.0;
        for (int i = 0; i < nOK; i++)
        {
            double nextA = tempA[i] / normVal;
            lastVal += nextA;
            normA[i] = lastVal;
        }

        // avoid round-off problems
        normA.LastIndex() = 1.0;

        if (logData)
            outputCDFDC.SetLog();
        else
            outputCDFDC.SetLinear();

        SC_SetupErr cdfErr;
        if (!outputCDFDC.SetupOK(cdfErr))
        {
            SetObjErrMsg(cdfErr.GetErrorText());
            outputCDFDC.DeAlloc();
        }
        else
            outputCDFDC.SetID(GetID());

    }
    else
    {
        DoQuantile(*selectedColData);
    }
}

