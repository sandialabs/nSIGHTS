///////////////////////////////////////////////////////////////////////////////////
//
// DFO_Statistics.cpp
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
//              Performs univariate statistics on an input vector.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genDFO/DFO_Statistics.h>


bool DFO_Statistics::defaultPresentTransformed = false;

DFO_Statistics :: DFO_Statistics(const char* inID, bool  medianOK )
    : FuncObjC(inID)
{
    value1Op = soSum;
    value2Op = soMin;
    value3Op = soMax;
    value4Op = soMean;

    calcModeAndMedian = false;
    calcLogData       = false;
    useLog10          = false;

    presentTransformed = defaultPresentTransformed;

    InitCommon();
}

DFO_Statistics::DFO_Statistics(const DFO_Statistics& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_Statistics :: ~DFO_Statistics ()
{
}

void DFO_Statistics::InitCommon()
{
    AddOutPort(value1OutputDO);
    AddOutPort(value2OutputDO);
    AddOutPort(value3OutputDO);
    AddOutPort(value4OutputDO);
}



DFO_Statistics& DFO_Statistics::operator= (const DFO_Statistics& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_Statistics::LocalCopy(const DFO_Statistics& a)
{
    value1Op            = a.value1Op;
    value2Op            = a.value2Op;
    value3Op            = a.value3Op;
    value4Op            = a.value4Op;
    calcModeAndMedian   = a.calcModeAndMedian;
    calcLogData         = a.calcLogData;
    useLog10            = a.useLog10;
    presentTransformed  = a.presentTransformed;
}


void  DFO_Statistics:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    value1OutputDO.Reset(opIDs[value1Op]);
    value2OutputDO.Reset(opIDs[value2Op]);
    value3OutputDO.Reset(opIDs[value3Op]);
    value4OutputDO.Reset(opIDs[value4Op]);
}

void DFO_Statistics::DoStatistic(const SC_DoubleArray& inData)
{
    if (inData.IsEmpty())
        return;

    double medianVal = nullReal;
    double modeVal = nullReal;

    if (calcModeAndMedian)
    {
        // copy and take log if necessary
        SC_DoubleArray tmpData = inData;
        if (calcLogData)
            if (useLog10)
                tmpData.Log10();
            else
                tmpData.Log();

        // med & mode need stats on log
        CalcStatistics(tmpData);
        CalcMedian(tmpData);
        CalcMode(tmpData);

        medianVal = realResults[soMedian];
        modeVal = realResults[soMode];
    }

    // get actual stats using CalcStats internal log
    CalcStatistics(inData, calcLogData, useLog10);

    // reset med & mode
    realResults[soMedian] = medianVal;
    realResults[soMode] = modeVal;


    UnLogIfNecessary(soMin);
    UnLogIfNecessary(soMax);
    UnLogIfNecessary(soMean);
    if (calcModeAndMedian)
    {
        UnLogIfNecessary(soMedian);
        UnLogIfNecessary(soMode);
    }
    realResults[soRange] = realResults[soMax] - realResults[soMin];

    value1OutputDO.InitLabelAndValue(realResults[value1Op]);
    value2OutputDO.InitLabelAndValue(realResults[value2Op]);
    value3OutputDO.InitLabelAndValue(realResults[value3Op]);
    value4OutputDO.InitLabelAndValue(realResults[value4Op]);
}


void DFO_Statistics::UnLogIfNecessary(StatisticOp valueOp)
{
    if ((!calcLogData) || presentTransformed)
        return;

    if (useLog10)
    {
        realResults[valueOp] = InvLgt(realResults[valueOp]);
    }
    else
    {
        realResults[valueOp] = exp(realResults[valueOp]);
    }
}

