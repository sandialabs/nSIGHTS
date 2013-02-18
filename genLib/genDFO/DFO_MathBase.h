///////////////////////////////////////////////////////////////////////////////////
//
// DFO_MathBase.h
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
//              For common mathematical operations (+-/*) on two input vectors
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_MATHBASE_H
#define DFO_MATHBASE_H

#include <genClass/C_FuncObj.h>

class DFO_MathBase : public FuncObjC {
public:
    enum OpType                 {ot_Add, ot_Subtract, ot_Multiply, ot_Divide, ot_Max, ot_Min};
    enum                        {descLen = 40};

    OpType                      mathOp;
    char                        outputTypeDesc[descLen];


public:
    DFO_MathBase(const char* inID);
    DFO_MathBase(const DFO_MathBase& a);
    ~DFO_MathBase();

    DFO_MathBase&               operator= (const DFO_MathBase& a);


protected:

    double          MathResult(const double& inA,
                               const double& inB);

    const char*     GetCurrID();

private:
    void            LocalCopy(const DFO_MathBase& a);     // common to copy constructor and = operator


};

#endif // DFO_MATHBASE_H

