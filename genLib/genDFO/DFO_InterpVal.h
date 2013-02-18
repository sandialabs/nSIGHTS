///////////////////////////////////////////////////////////////////////////////////
//
// DFO_InterpVal.h
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
//              Interpolates a Y value from XY input given an X value.
//              Various options are available (linear, log, nearest, etc)
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_INTERPVAL_H
#define DFO_INTERPVAL_H

#include <genDataClass/DSC_RealMSSpecBase.h>
#include <genClass/DO_Real.h>
#include <genClass/SC_DoubleArray.h>


class DFO_InterpVal : public FuncObjC {
public:
    enum    InterpMethod    {imLinear, imPrevious, imNext, imClosest};

    InterpMethod            interpMethod;
    DSC_RealMSSpecBase      interpXValueMS;  // min value - input if autoLimits false; output always
    bool                    logX;
    bool                    logY;

    DO_Real                 outputRealDO;

private:
    double                  interpX;

public:
    DFO_InterpVal(const char* inID);
    DFO_InterpVal(const DFO_InterpVal& a);
    ~DFO_InterpVal();

    DFO_InterpVal&          operator= (const DFO_InterpVal& a);

    void                    DoStatusChk();
protected:
    void                    DoInterpVal(const SC_DoubleArray& xData,
                                        const SC_DoubleArray& yData);
private:
    void                    InitCommon();                        // common to normal and copy constructor
    void                    LocalCopy(const DFO_InterpVal& a);   // common to copy constructor and = operator
};

#endif // DFO_INTERPVAL_H

