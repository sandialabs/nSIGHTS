///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ResidualHistogram.cpp
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
//      For calculating fits, and for reading, selecting, and operating on optimizer results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osClass/U_Resid.h>

#include <osDFO/DFO_ResidualHistogram.h>

DFO_ResidualHistogram :: DFO_ResidualHistogram() : FuncObjC("ResidualHistogram")
{
    cumulativeDistribution = false;
    standardizeResiduals = true;
    normalizeResiduals = true;
    normalizationLimits = 1.0;
    nhistoBins = 20;
    InitCommon();
}

DFO_ResidualHistogram::DFO_ResidualHistogram(const DFO_ResidualHistogram& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_ResidualHistogram :: ~DFO_ResidualHistogram ()
{
}

void DFO_ResidualHistogram::InitCommon()
{
    AddInPort(residInputObjRef, typeid(DO_XYData));

    outputXYDO.xyData = &outputXY;
    AddOutPort(outputXYDO);

    outputDistributionDO.xyData = &outputDistributionDC;
    AddOutPort(outputDistributionDO);

    DoStatusChk();
}

DFO_ResidualHistogram& DFO_ResidualHistogram::operator= (const DFO_ResidualHistogram& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ResidualHistogram::LocalCopy(const DFO_ResidualHistogram& a)
{
    residInputObjRef        = a.residInputObjRef;
    nhistoBins              = a.nhistoBins;
    cumulativeDistribution  = a.cumulativeDistribution;
    standardizeResiduals    = a.standardizeResiduals;
    normalizationLimits     = a.normalizationLimits;
    normalizeResiduals      = a.normalizeResiduals;
}

void  DFO_ResidualHistogram:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (CheckInputFO(residInputObjRef, "input data source"))
    {
        DO_XYData* tmpDO = static_cast<DO_XYData*>(GetInPortData(residInputObjRef));
        inputXY = tmpDO->xyData;
    }
}


void DFO_ResidualHistogram:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    // copy input
    SC_DoubleArray residData(inputXY->yData);
    int nResid = residData.Size();

    double delH, minR, maxR;
    if (standardizeResiduals)
    {
        StandardizeResiduals(residData);
        residData.CalcMinMax(minR, maxR);
        delH = 1.0 / double(residData.Size());
        outputXY.SetID("StdHistogram");
    }
    else
    {
        delH = 1.0;
        residData.CalcMinMax(minR, maxR);
        outputXY.SetID("Histogram");
        if (normalizeResiduals)
        {
            double scalingFactor = normalizationLimits * 2.0 / (maxR - minR);
            for (int i = 0; i < nResid; i++)
                residData[i] = -normalizationLimits + (residData[i] - minR) * scalingFactor;
            minR = -normalizationLimits;
            maxR = normalizationLimits;
            outputXY.SetID("NormHistogram");
        }
    }

    outputXY.Alloc(nhistoBins + 1);

    SC_DoubleArray& xDat = outputXY.xData;
    SC_DoubleArray& hDat = outputXY.yData;
    hDat.FillToAlloc(0.0);

    double delX = (maxR - minR) / double(nhistoBins);
    xDat[0] = minR;
    for (int i = 1; i <= nhistoBins; i++)
        xDat[i] = xDat[i - 1] + delX;

    if (standardizeResiduals && (!cumulativeDistribution))
        delH /= delX;

    for (int i = 0; i < nResid; i++)
        for (int j = 0; j < nhistoBins; j++)
            if (residData[i] < xDat[j + 1])
            {
                hDat[j] += delH;
                if (!cumulativeDistribution)
                    break;
            }

    // reset bins to centre
    xDat[0] = (xDat[0] + xDat[1]) / 2.0;
    for (int i = 1; i < nhistoBins; i++)
        xDat[i] = xDat[i - 1] + delX;

    xDat.SetSize(nhistoBins);
    hDat.SetSize(nhistoBins);

    if (standardizeResiduals)
    {
        StandardNormalDistribution(outputDistributionDC, 10 * nhistoBins, cumulativeDistribution, -5.0, 5.0);
        outputDistributionDC.SetID("Standard Normal");
    }
    else
    {
        outputDistributionDC.SetEmpty();
        outputDistributionDC.SetID("No distribution");
    }
}

