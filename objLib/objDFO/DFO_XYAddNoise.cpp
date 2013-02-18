///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYAddNoise.cpp
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
//      Adds normally or uniformly distributed noise to the Y values in an XY
//  data set
//
///////////////////////////////////////////////////////////////////////////////////


#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_Random.h>
#include <genClass/SC_SampVar.h>

#include <objDFO/DFO_XYAddNoise.h>

DFO_XYAddNoise::DFO_XYAddNoise() :
    DFO_XYBase("Add Noise to XY")
{
    noiseDistributionOp = anNormal;
    stdDeviation = 1.0E0;
    noiseRange = 1.0E0;
    randomSeed = 25837623;
}

DFO_XYAddNoise::DFO_XYAddNoise(const DFO_XYAddNoise& a) :
    DFO_XYBase(a)
{
    LocalCopy(a);
}

DFO_XYAddNoise :: ~DFO_XYAddNoise ()
{
}

void DFO_XYAddNoise::InitCommon()
{
    DoStatusChk();
}

DFO_XYAddNoise& DFO_XYAddNoise::operator= (const DFO_XYAddNoise& a)
{
    if (&a != this)
    {
        DFO_XYBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYAddNoise::LocalCopy(const DFO_XYAddNoise& a)
{
    noiseDistributionOp = a.noiseDistributionOp;
    stdDeviation = a.stdDeviation;
    noiseRange = a.noiseRange;
    randomSeed = a.randomSeed;
}

void  DFO_XYAddNoise:: DoStatusChk()
{
    outputXY.SetID("noise output");
    DFO_XYBase::DoStatusChk();
    if (StatusOK())
    {
        if (inputXY->Size() < 2)
        {
            SetObjErrMsg("insufficient data in input");
        }
    }
}


void DFO_XYAddNoise:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    // basic copy & clean
    outputXY.CreateFrom(inputXY->xData, inputXY->yData);

    int nXY = outputXY.Size();
    if (nXY < 2)
    {
        SetObjErrMsg("insufficient data after cleaning");
        return;
    }

    SC_SampVar sample;
    if (noiseDistributionOp == anUniform)
    {
        sample.svDistribution = SampleDistributionBase::svdUniform;
        sample.distMin = - noiseRange / 2.0;
        sample.distMax = noiseRange / 2.0;
    }
    else
    {
        sample.svDistribution = SampleDistributionBase::svdNormal;
        sample.distMean = 0.0;
        sample.distStdDev = stdDeviation;
    }

    SC_Random randGen;
    randGen.SetupRandom(randomSeed);

    SC_DoubleArray noise;
    sample.Sample(randGen, nXY, true, noise);

    SC_IntArray order;
    order.AllocAndIndex(nXY);
    randGen.RandomMix(order);

    SC_DoubleArray& y = outputXY.yData;
    for(int i = 0; i < nXY; i++)
        y[i] += noise[order[i]];

    CalcFinish();
}

