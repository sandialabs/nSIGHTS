///////////////////////////////////////////////////////////////////////////////////
//
// DFO_RealScaleTransform.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <objDFO/DFO_RealScaleTransform.h>

DFO_RealScaleTransform :: DFO_RealScaleTransform() : DFO_ScaleTransform("Sc/Tran Real")
{
    limitMin     = false;
    limitMax     = false;
    minLimit     = 0.0;
    maxLimit     = 1.0;

    InitCommon();
}

DFO_RealScaleTransform::DFO_RealScaleTransform(const DFO_RealScaleTransform& a) : DFO_ScaleTransform(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_RealScaleTransform :: ~DFO_RealScaleTransform ()
{
}

void DFO_RealScaleTransform::InitCommon()
{
    AddInPort(realInputObjRef, typeid(DO_Real));
    AddOutPort(outputRealDO);
    outputRealDO.Reset("Output value");
    DoStatusChk();
}

DFO_RealScaleTransform& DFO_RealScaleTransform::operator= (const DFO_RealScaleTransform& a)
{
    if (&a != this)
    {
        DFO_ScaleTransform::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_RealScaleTransform::LocalCopy(const DFO_RealScaleTransform& a)
{
    realInputObjRef = a.realInputObjRef;
    limitMin    = a.limitMin   ;
    limitMax    = a.limitMax   ;
    minLimit    = a.minLimit   ;
    maxLimit    = a.maxLimit   ;
}

void  DFO_RealScaleTransform:: DoStatusChk()
{
    DFO_ScaleTransform::DoStatusChk();
 
    if (CheckInputFO(realInputObjRef, "input real"))
        inputReal = (static_cast<DO_Real*> (GetInPortData(realInputObjRef)))->realVal;

    if (limitMin && limitMax && (minLimit >= maxLimit))
        SetObjErrMsg("limits incompatible");

}


void DFO_RealScaleTransform:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
    {
        outputRealDO.Reset("Output bad");
        return;
    }

    double outputReal = inputReal;

    DoScaleTransform(outputReal);

    if (!RealIsNull(outputReal))
    {
        if (limitMin && (outputReal < minLimit))
            outputReal = minLimit;
        if (limitMax && (outputReal > maxLimit))
            outputReal = maxLimit;
    }
    else
        if (processNulls)
            outputReal = setNullValue;

    outputRealDO.Reset("Output value");
    outputRealDO.realVal = outputReal;
    outputRealDO.SetValueLabel(outputReal);
}

