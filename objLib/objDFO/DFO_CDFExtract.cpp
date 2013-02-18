///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CDFExtract.cpp
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

#include <genClass/DO_XYCDF.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/SC_RealFormat.h>

#include <objDFO/DFO_CDFExtract.h>

DFO_CDFExtract :: DFO_CDFExtract() :
    FuncObjC("Interpolate CDF"),
    rvalMS("Interpolate at value")
{
    calcProbability = true;
    useLimits = true;
    InitCommon();
}

DFO_CDFExtract::DFO_CDFExtract(const DFO_CDFExtract& a) :
    FuncObjC(a),
    rvalMS(a.rvalMS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_CDFExtract :: ~DFO_CDFExtract ()
{
}

void DFO_CDFExtract::InitCommon()
{
    AddInPort(cdfInputObjRef, typeid(DO_XYData));
    AddOutPort(outputValDO);
    outputValDO.Reset("Extracted");
    rvalMS.AddPort(*this);
}

DFO_CDFExtract& DFO_CDFExtract::operator= (const DFO_CDFExtract& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        rvalMS  = a.rvalMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_CDFExtract::LocalCopy(const DFO_CDFExtract& a)
{
    cdfInputObjRef = a.cdfInputObjRef;
    calcProbability = a.calcProbability;
    useLimits = a.useLimits;
}

void  DFO_CDFExtract:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    rvalMS.DoCheck("Extract at value", *this);
    if (CheckInputFO(cdfInputObjRef, "Input CDF"))
    {
        inputCDFDC = static_cast<DO_XYCDF*>(GetInPortData(cdfInputObjRef))->GetCDF();
    }

    if (StatusOK())
    {
        currentRVal = rvalMS.GetCurrentValue();
        if ((!calcProbability) && ((currentRVal < 0.0) || (currentRVal > 1.0)))
            SetObjErrMsg("probability value out of range");
        if (calcProbability && inputCDFDC->IsLog() && (currentRVal <= 0.0))
            SetObjErrMsg("log CDF requires +ve data value");
    }

}


void DFO_CDFExtract::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    outputValDO.Reset("Extracted");
    if (StatusNotOK())
        return;

    if (calcProbability) 
    {
        outputValDO.Reset("Calculated P");
        outputValDO.InitLabelAndValue(inputCDFDC->GetProbability(currentRVal,!useLimits),
                                      SC_DecFormat(6));
    }
    else
    {
        outputValDO.Reset("Calculated value");
        outputValDO.InitLabelAndValue(inputCDFDC->GetDataVal(currentRVal, !useLimits),
                                      SC_SciFormat(6));
    }
}

