///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ScaleTransform.cpp
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
//              Performs basic scaling/transform operations on an input vector.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genDFO/DFO_ScaleTransform.h>

DFO_ScaleTransform :: DFO_ScaleTransform(const char* objID) :
    FuncObjC(objID),
    offsetMS("Offset value"),
    scaleMS("Scale value")
{
    processNulls    = false;
    setNullValue    = 0.0;

    // initialize to valid
    scaleMS.enteredRealValue = 1.0;
    offsetMS.enteredRealValue = 0.0;

    InitCommon();
}

DFO_ScaleTransform::DFO_ScaleTransform(const DFO_ScaleTransform& a) :
    FuncObjC(a),
    offsetMS(a.offsetMS),
    scaleMS(a.scaleMS),
    DSC_ScaleTransform(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_ScaleTransform :: ~DFO_ScaleTransform ()
{
}

void DFO_ScaleTransform::InitCommon()
{
    offsetMS.AddPort(*this);
    scaleMS.AddPort(*this);
}

DFO_ScaleTransform& DFO_ScaleTransform::operator= (const DFO_ScaleTransform& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        DSC_ScaleTransform::operator=(a);
        scaleMS  = a.scaleMS;
        offsetMS = a.offsetMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ScaleTransform::LocalCopy(const DFO_ScaleTransform& a)
{
    processNulls    = a.processNulls;
    setNullValue    = a.setNullValue;
}

void  DFO_ScaleTransform:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    offsetMS.DoCheck("Offset Value Source", *this);
    scaleMS.DoCheck("Scale Value Source", *this);

   // it is better to do this here once than every time through the loop

    offsetVal = offsetMS.GetCurrentValue();
    scaleVal = scaleMS.GetCurrentValue();

    if (((scaleOp == st_DdSpO) || (scaleOp == st_DpOdS)) &&
        (fabs(scaleMS.GetCurrentValue()) < 1.0E-60))
        SetObjErrMsg("ABS(scale) must > 1E-60 for divide by scale");

}

