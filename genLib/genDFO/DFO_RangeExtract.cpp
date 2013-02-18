///////////////////////////////////////////////////////////////////////////////////
//
// DFO_RangeExtract.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <genDFO/DFO_RangeExtract.h>

DFO_RangeExtract :: DFO_RangeExtract(const char* inID) :
    FuncObjC(inID),
    referenceValAMS("Reference A Value"),
    referenceValBMS("Reference B Value")
{
    dataUse         = dut_Aonly;
    dataOpA         = ot_GreaterEqual;
    dataOpB         = ot_LessEqual;
    referenceValAMS.enteredRealValue = 0.0;
    referenceValBMS.enteredRealValue = 1.0;
    InitCommon();
}

DFO_RangeExtract::DFO_RangeExtract(const DFO_RangeExtract& a) :
    FuncObjC(a),
    referenceValAMS(a.referenceValAMS),
    referenceValBMS(a.referenceValBMS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_RangeExtract :: ~DFO_RangeExtract ()
{
}

void DFO_RangeExtract::InitCommon()
{
    referenceValAMS.AddPort(*this);
    referenceValBMS.AddPort(*this);
    refBCheckRef = GetLastInPortCheckRef();
    naccepted = -1;
    nrejected = -1;

    DoStatusChk();
}

DFO_RangeExtract& DFO_RangeExtract::operator= (const DFO_RangeExtract& a)
{
    if (&a != this)
        {
            FuncObjC::operator=(a);
            referenceValAMS = a.referenceValAMS;
            referenceValBMS = a.referenceValBMS;
            LocalCopy(a);
        }
    return *this;
}


void  DFO_RangeExtract::LocalCopy(const DFO_RangeExtract& a)
{
    dataUse  = a.dataUse;
    dataOpA  = a.dataOpA;
    dataOpB  = a.dataOpB;
}

void  DFO_RangeExtract:: DoStatusChk()
{
    checkB = (dataUse != dut_Aonly);

    *refBCheckRef = checkB;

    FuncObjC::DoStatusChk();

    referenceValAMS.DoCheck("Value A  master", *this);

    if (checkB)
        referenceValBMS.DoCheck("Value B  master", *this);

    if (StatusOK())
        {
            aVal = referenceValAMS.GetCurrentValue();
            if (checkB)
                bVal = referenceValBMS.GetCurrentValue();
        }
    else
        {
            naccepted = -1;
            nrejected = -1;
        }
}

void DFO_RangeExtract::InitStatusVals()
{
    naccepted = 0;
    nrejected = 0;
}

bool DFO_RangeExtract::ValueInRange(const double&  inVal)
{
    if (RealIsNull(inVal))
        {
            nrejected++;
            return false;
        }

    bool outOK = PassesOp(dataOpA, aVal, inVal);
    if (checkB)
        {
            bool bOK = PassesOp(dataOpB, bVal, inVal);
            if (dataUse == dut_AandB)
                outOK = outOK && bOK;
            else
                outOK = outOK || bOK;
        }

    if (outOK)
        naccepted++;
    else
        nrejected++;

    return outOK;
}


bool DFO_RangeExtract::PassesOp(OpType  dataOp,
                                const double& referenceVal,
                                const double&  inVal)
{
    bool opOK;
    switch (dataOp)
        {
        case ot_GreaterThan : {
            opOK = (inVal > referenceVal);
            break;
        }
        case ot_GreaterEqual : {
            opOK = (inVal >= referenceVal);
            break;
        }
        case ot_LessThan : {
            opOK = (inVal < referenceVal);
            break;
        }
        case ot_LessEqual : {
            opOK = (inVal <= referenceVal);
            break;
        }
        }
    return opOK;
}

