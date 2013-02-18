///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYInterpVal.cpp
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

#include <genClass/U_Str.h>

#include <objDFO/DFO_XYInterpVal.h>

DFO_XYInterpVal :: DFO_XYInterpVal() :
    DFO_InterpVal("Interpolate XY")
{
    InitCommon();
}

DFO_XYInterpVal::DFO_XYInterpVal(const DFO_XYInterpVal& a) :
    DFO_InterpVal(a),
    DSC_XYColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYInterpVal :: ~DFO_XYInterpVal ()
{
}

void DFO_XYInterpVal::InitCommon()
{
    DSC_XYColSel::AddPort(*this);
    DoStatusChk();
}

DFO_XYInterpVal& DFO_XYInterpVal::operator= (const DFO_XYInterpVal& a)
{
    if (&a != this)
    {
        DFO_InterpVal::operator=(a);
        DSC_XYColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYInterpVal::LocalCopy(const DFO_XYInterpVal& a)
{
}


void  DFO_XYInterpVal:: DoStatusChk()
{
    DFO_InterpVal::DoStatusChk();
    DSC_XYColSel::DoCheck(*this);
}

void DFO_XYInterpVal:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;
    if (operateOnX)
    {
        DoInterpVal(inputData->xData, inputData->yData);
    }
    else
    {
        DoInterpVal(inputData->yData, inputData->xData);
    }
}

