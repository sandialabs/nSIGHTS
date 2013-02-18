///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CreateSpecialCDF.cpp
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

#include <genClass/DO_XYData.h>

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objDFO/DFO_CreateSpecialCDF.h>


DFO_CreateSpecialCDF :: DFO_CreateSpecialCDF()
    : FuncObjC("Create Special CDFs")
{
    distributionType = dtDiscrete;
    normalizeInputProbabilities = false;
    InitCommon();
}

DFO_CreateSpecialCDF::DFO_CreateSpecialCDF(const DFO_CreateSpecialCDF& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_CreateSpecialCDF :: ~DFO_CreateSpecialCDF ()
{
}

void DFO_CreateSpecialCDF::InitCommon()
{
    AddInPort(xyInputObjRef, typeid(DO_XYData));

    outputCDFDC.SetID("CDF");
    outputCDFDO.SetCDF(outputCDFDC);
    AddOutPort(outputCDFDO);

    DoStatusChk();
}

DFO_CreateSpecialCDF& DFO_CreateSpecialCDF::operator= (const DFO_CreateSpecialCDF& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_CreateSpecialCDF::LocalCopy(const DFO_CreateSpecialCDF& a)
{
    xyInputObjRef = a.xyInputObjRef;
    distributionType = a.distributionType;
    normalizeInputProbabilities = a.normalizeInputProbabilities;
}



void  DFO_CreateSpecialCDF:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    if (CheckInputFO(xyInputObjRef, "input data source"))
    {
        inputXYDC = static_cast<DO_XYData*>(GetInPortData(xyInputObjRef))->xyData;

        int minSize = 2;
        if (IsDiscrete())
            minSize = 1;
        ninput = inputXYDC->Size();

        if (ninput < minSize)
            SetObjErrMsg("insufficient points in input XY");

        const SC_DoubleArray& xvals = inputXYDC->xData;
        // must be ascending X
        if (!xvals.IsAscending(true))
            SetObjErrMsg("x data must be ascending");

        if (distributionType == dtLogStepwiseUniform)
        {
            for (int i = 0; i < ninput; i++)
                if (!IsPositive(xvals[i]))
                    SetObjErrMsg("x data must be +ve for log distribution");
        }

        const SC_DoubleArray& yvals = inputXYDC->yData;
        int firstInc = 1;
        if (IsDiscrete())
            firstInc = 0;
        normFactor = 0.0;

        for (int i = firstInc; i < ninput; i++)
        {
            double testY = yvals[i];
            if (IsDiscrete())
            {
                if (!IsPositive(testY))
                    SetObjErrMsg("probability values must be +ve");
            }
            else
            {
                if (testY < 0.0)
                    SetObjErrMsg("probability values must be zero or +ve");
            }

            normFactor += testY;
        }

        if ((!normalizeInputProbabilities) && (!RealsAreSame(normFactor, 1.0, 1E-06))) 
            SetObjErrMsg("non-normalized probabilities must sum to 0.0");
    }
}


void DFO_CreateSpecialCDF:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;


    if (IsDiscrete())
    {
        outputCDFDC.Alloc(2 * ninput);
        const SC_DoubleArray& iny = inputXYDC->yData;
        const SC_DoubleArray& inx = inputXYDC->xData;
        SC_DoubleArray& outy = outputCDFDC.yData;
        SC_DoubleArray& outx = outputCDFDC.xData;

        double lastP = 0.0;
        for (int i = 0; i < ninput; i++)
        {
            outx += inx[i];
            outy += lastP;
            lastP += iny[i] / normFactor;
            outx += inx[i];
            outy += lastP;
        }
        outputCDFDC.SetLinear();
    }
    else
    {
        outputCDFDC.DC_XYData::operator =(*inputXYDC);
        if (distributionType == dtLogStepwiseUniform)
        {
            outputCDFDC.xData.Log();
            outputCDFDC.SetLog();
        }
        else
            outputCDFDC.SetLinear();

        SC_DoubleArray& outy = outputCDFDC.yData;
        outy[0] = 0.0;
        for (int i = 1; i < ninput; i++)
        {
            outy[i] /= normFactor;
            outy[i] += outy[i - 1];
        }
    }


    SC_SetupErr cdfErr;
    if (!outputCDFDC.SetupOK(cdfErr))
    {
        SetObjErrMsg(cdfErr.GetErrorText());
        outputCDFDC.DeAlloc();
    }
    else
    {
        char newID[80];
        MakeString(newID, "CDF(",xyInputObjRef.objRef->GetID(), ")", 80);
        SetID(newID);

        outputCDFDC.SetID(GetID());
    }
}


