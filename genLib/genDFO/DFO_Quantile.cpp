///////////////////////////////////////////////////////////////////////////////////
//
// DFO_Quantile.cpp
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
// DESCRIPTION: Basic functional objects (all derived from FuncObjC)
//              used by all applications, primarily concerned with
//              defining/operating on pen sets and colour maps: Also
//              includes base classes for common operations with
//              different application specific implementations:
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>
#include <genClass/SC_Statistics.h>
#include <genClass/SC_Random.h>

#include <genDFO/DFO_Quantile.h>


DFO_Quantile :: DFO_Quantile(const char* inID) :
    FuncObjC(inID),
    quantileMS("Quantile value")
{
    quantileType = DC_XYCDF::qtWeibull;
    logData = false;
    InitCommon();

}

DFO_Quantile::DFO_Quantile(const DFO_Quantile& a) :
    FuncObjC(a),
    quantileMS(a.quantileMS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_Quantile :: ~DFO_Quantile ()
{
}

void DFO_Quantile::InitCommon()
{
    quantileMS.AddPort(*this);
    quantileCheckRef = GetLastInPortCheckRef();

    AddOutPort(outputCDFDO);
    outputCDFDO.SetCDF(outputCDFDC);
}

DFO_Quantile& DFO_Quantile::operator= (const DFO_Quantile& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        quantileMS = a.quantileMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_Quantile::LocalCopy(const DFO_Quantile& a)
{
    quantileType    =  a.quantileType;
    logData         = a.logData;
}

void  DFO_Quantile:: DoStatusChk()
{
    // only check MS ref if not auto
    *quantileCheckRef  = (quantileType == DC_XYCDF::qtGeneric);

    FuncObjC::DoStatusChk();

    outputCDFDC.SetID("CDF/Quantile");

    genericQuantileValue = 0.0;
    if (quantileType == DC_XYCDF::qtGeneric)
    {
        quantileMS.DoCheck("quantile valuemaster", *this);
        genericQuantileValue = quantileMS.GetCurrentValue();
    }

    if (!DC_XYCDF::QuantileOK(quantileType, genericQuantileValue))
    {
        SetObjErrMsg("generic quantile must be between 0 and 1");
    }
}

void DFO_Quantile:: DoQuantile(const SC_DoubleArray& data)
{
    quantileMS.InitLabelAndValue(genericQuantileValue);

    SC_DoubleArray temp(data);
    temp.Cleanup();

    int N = temp.Size();
    if (logData)
    {
        int nOK = 0;
        for (int i = 0; i < N; i++)
        {
            double dataVal = temp[i];
            if (dataVal > 0.0)
                temp[nOK++] = log(dataVal);
        }
        if (nOK < 2)
        {
            SetObjErrMsg("less than 2 points left after log");
            return;
        }
        N = nOK;
        temp.SetSize(N);
    }

    // mix before sort to help sort routine
    SC_Random mix;
    mix.RandomMix(temp);
    if (!temp.Sort(true))
    {
        SetObjErrMsg("stack overflow on sort - data bad ??");
        return;
    }

    // alloc output
    if (!outputCDFDC.Alloc(N))
    {
        SetObjErrMsg("output allocation ??");
        return;
    }

    outputCDFDC.xData = temp;
    outputCDFDC.SetSize(N);
    outputCDFDC.SetCDFQuantile(quantileType, genericQuantileValue);

    if (logData)
        outputCDFDC.SetLog();
    else
        outputCDFDC.SetLinear();

    outputCDFDC.SetSize(N);

    SC_SetupErr cdfErr;
    if (!outputCDFDC.SetupOK(cdfErr))
    {
        SetObjErrMsg(cdfErr.GetErrorText());
        outputCDFDC.DeAlloc();
    }
    else
        outputCDFDC.SetID(GetID());
}

