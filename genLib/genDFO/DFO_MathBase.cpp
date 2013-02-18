///////////////////////////////////////////////////////////////////////////////////
//
// DFO_MathBase.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genDFO/DFO_MathBase.h>

DFO_MathBase :: DFO_MathBase(const char* inID) : FuncObjC(inID)
{
    SetToNull(outputTypeDesc);
    mathOp = ot_Add;
}

DFO_MathBase::DFO_MathBase(const DFO_MathBase& a) : FuncObjC(a)
{
    LocalCopy(a);
}

DFO_MathBase :: ~DFO_MathBase ()
{
}


DFO_MathBase& DFO_MathBase::operator= (const DFO_MathBase& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_MathBase::LocalCopy(const DFO_MathBase& a)
{
    mathOp = a.mathOp;
    CopyString(outputTypeDesc,  a.outputTypeDesc, descLen);
}


double DFO_MathBase::MathResult(const double& aVal,
                                const double& bVal)
{
    if (RealIsNull(aVal) || RealIsNull(bVal))
    {
        if ((mathOp == ot_Max) || (mathOp == ot_Min))
        {
            if (!RealIsNull(aVal))
                return aVal;
            if (!RealIsNull(bVal))
                return bVal;
        }
        return nullReal;
    }

    switch (mathOp)
    {
        case ot_Add : {
            return  aVal + bVal;
        }
        case ot_Subtract : {
            return  aVal - bVal;
        }
        case ot_Multiply : {
            return  aVal * bVal;
        }
        case ot_Divide : {
            if (fabs(bVal) < 1.0E-100)
                return nullReal;
            return aVal / bVal;
        }
        case ot_Max : {
            if (aVal > bVal)
                return aVal;
            return  bVal;
        }
        case ot_Min : {
            if (aVal < bVal)
                return aVal;
            return  bVal;
        }
    }
    return 0.0;  // keep compiler happy
}


const char* DFO_MathBase::GetCurrID()
{
    if (StringLength(outputTypeDesc) > 0)
        return outputTypeDesc;

    return GetID();
}

