///////////////////////////////////////////////////////////////////////////////////
//
// DFO_Histogram.h
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
//              Calculates a histogram for an input vector.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_HISTOGRAM_H
#define DFO_HISTOGRAM_H

#include <genDataClass/DSC_RealMSSpecBase.h>
#include <genClass/DO_XYData.h>
#include <genClass/SC_DoubleArray.h>


class DFO_Histogram : public FuncObjC {
public:
    bool                    autoBinWidth;  // if true based on Scott, 79
    bool                    intBinWidth;   //makes max-min+1 bins
    bool                    autoLimits;    // if true min/max based on input
    DSC_RealMSSpecBase      minValueMS;    // min value - input if autoLimits false; output always
    DSC_RealMSSpecBase      maxValueMS;    // max value - input if autoLimits false; output always

    bool                    logHistogram;   // bins calculated on log min/max
    int                     numberOfBins;
    bool                    cumulative;     // results accumulate
    bool                    outputXisIndex; // if true X output is bin index, if false mid value of bin
    bool                    normalizeY;     // if true Y output is normalized to # of input

    double                  binWidth;

protected:
    //  output data
    DC_XYData               outputXY;

private:
    //  output object
    DO_XYData               outputXYDO;
    bool*                   minValCheckRef;
    bool*                   maxValCheckRef;
    double                  minValue;
    double                  maxValue;

public:
    DFO_Histogram(const char* inID);
    DFO_Histogram(const DFO_Histogram& a);
    ~DFO_Histogram();

    DFO_Histogram&          operator= (const DFO_Histogram& a);

    void                    DoStatusChk();
protected:
    void                    DoHistogram(const SC_DoubleArray& data);
    // puts calculated bin# in bin index
    // -1 if outside range
    // -2 if input is null
    // binIndex only done if binIndex.Size() == data.Size()
    // i.e. must be pre alloced
    void                    DoHistogram(const SC_DoubleArray& data,
                                        SC_DoubleArray& binIndex);

private:
    void                    InitCommon();                        // common to normal and copy constructor
    void                    LocalCopy(const DFO_Histogram& a);   // common to copy constructor and = operator
    void                    CheckMinMax();
};

#endif // DFO_HISTOGRAM_H

