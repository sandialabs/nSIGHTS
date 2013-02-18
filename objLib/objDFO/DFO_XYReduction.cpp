///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYReduction.cpp
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
//      Options to reduce the X density of an input data set using a variety of
//  methods
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genClass/SC_DoubleArray.h>
#include <objDFO/DFO_XYReduction.h>

DFO_XYReduction :: DFO_XYReduction() :
    DFO_XYBase("Reduction")
{
    dataReductionOperation = reSkip;
    pointSkipInterval = 0;
    maxXChange = 1.0;
    maxYChange = 1.0;
    logXChange = false;
    logYChange = false;
    InitCommon();
}

DFO_XYReduction::DFO_XYReduction(const DFO_XYReduction& a) :
    DFO_XYBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYReduction :: ~DFO_XYReduction ()
{
}

void DFO_XYReduction::InitCommon()
{
    DoStatusChk();
}

DFO_XYReduction& DFO_XYReduction::operator= (const DFO_XYReduction& a)
{
    if (&a != this)
    {
        DFO_XYBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYReduction::LocalCopy(const DFO_XYReduction& a)
{
    dataReductionOperation = a.dataReductionOperation;
    pointSkipInterval = a.pointSkipInterval;
    maxXChange = a.maxXChange;
    maxYChange = a.maxYChange;
    logXChange = a.logXChange;
    logYChange = a.logYChange;
}

void  DFO_XYReduction:: DoStatusChk()
{
    outputXY.SetID("reduction");

    DFO_XYBase::DoStatusChk();
    if (StatusOK() && (inputXY->Size() < 2))
    {
        SetObjErrMsg("insufficient data in input");
    }
}


bool DFO_XYReduction::CheckChange(      double  currVal, 
                                        double  lastVal,
                                  const double& maxChange,
                                        bool    logChange)
{
    if (logChange)
    {
        if ((!IsPositive(currVal)) || (!IsPositive(lastVal)))
            return false;
        currVal = log10(currVal);
        lastVal = log10(lastVal);
    }
    return fabs(currVal - lastVal) >= maxChange;
}


bool DFO_XYReduction::CheckXChange(const double& currX, 
                                   const double& lastX)
{
    return CheckChange(currX, lastX, maxXChange, logXChange);
}

bool DFO_XYReduction::CheckYChange(const double& currY, 
                                   const double& lastY)
{
    return CheckChange(currY, lastY, maxYChange, logYChange);
}


void DFO_XYReduction:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    outputXY.CreateFrom(inputXY->xData, inputXY->yData);
    // basic copy & clean
    if (outputXY.Size() < 2)
    {
        SetObjErrMsg("insufficient data after cleaning");
        return;
    }

    //  outputXY has been cleaned
    SC_DoubleArray& x = outputXY.xData;
    SC_DoubleArray& y = outputXY.yData;
    int nPts = outputXY.Size();

    x.SetEmpty();
    y.SetEmpty();

    int nskip = pointSkipInterval;
    double lastX, lastY;
    bool lastWasAdded = false;
    for (int j = 0; j < nPts; j++)
    {
        bool doAdd = (j == 0) || (j == (nPts - 1));
        if (dataReductionOperation != reMax)
        {
            nskip++;
            doAdd = (nskip > pointSkipInterval);
        }

        double currX = x[j];
        double currY = y[j];
        if ((!doAdd) && (dataReductionOperation != reSkip))
        {
            if (CheckYChange(currY, lastY))
            {
                if (!lastWasAdded)
                {
                    x += lastX;
                    y += lastY;
                    nskip = 0;
                }
                doAdd = true;
            }

            if (!doAdd)
            {
                doAdd = CheckYChange(currY, y.LastIndex()) || CheckXChange(currX, x.LastIndex());
            }

        }

        lastX = currX;
        lastY = currY;
        lastWasAdded = doAdd;
        if (doAdd)
        {
            x += lastX;
            y += lastY;
            nskip = 0;
        }
    }

    CalcFinish();
}



