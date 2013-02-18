///////////////////////////////////////////////////////////////////////////////////
//
// DFO_Statistics.h
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

#ifndef DFO_STATISTICS_H
#define DFO_STATISTICS_H

#include <genClass/C_Common.h>
#include <genClass/C_FuncObj.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_Statistics.h>

#include <genClass/DO_Real.h>


class DFO_Statistics : public FuncObjC, public SC_Statistics {
public:
    StatisticOp         value1Op;
    StatisticOp         value2Op;
    StatisticOp         value3Op;
    StatisticOp         value4Op;

    bool                calcModeAndMedian;
    bool                calcLogData;
    bool                useLog10;
    bool                presentTransformed;             // to reconcile RR & OPG
    static  bool        defaultPresentTransformed;      // change default on app basis
private:
    //  output data
    DO_Real             value1OutputDO;
    DO_Real             value2OutputDO;
    DO_Real             value3OutputDO;
    DO_Real             value4OutputDO;

public:
    DFO_Statistics(const char* inID,
                   bool  medianModeOK = false);
    DFO_Statistics(const DFO_Statistics& a);
    ~DFO_Statistics();

    DFO_Statistics& operator= (const DFO_Statistics& a);

    void            DoStatusChk();
protected:
    void            DoStatistic(const SC_DoubleArray& data);

private:
    void            InitCommon();                        // common to normal and copy constructor
    void            LocalCopy(const DFO_Statistics& a);  // common to copy constructor and = operator
    void            SetOutput();

    void            UnLogIfNecessary(StatisticOp valueOp);

};

#endif // DFO_STATISTICS_H

