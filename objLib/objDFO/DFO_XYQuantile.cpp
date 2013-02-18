///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYQuantile.cpp
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

#include <objDFO/DFO_XYQuantile.h>

DFO_XYQuantile :: DFO_XYQuantile() :
    DFO_Quantile("XY CDF")
{
    InitCommon();
}

DFO_XYQuantile::DFO_XYQuantile(const DFO_XYQuantile& a) :
    DFO_Quantile(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYQuantile :: ~DFO_XYQuantile ()
{
}

void DFO_XYQuantile::InitCommon()
{
    DSC_XYColSel::AddPort(*this);
    DoStatusChk();
}

DFO_XYQuantile& DFO_XYQuantile::operator= (const DFO_XYQuantile& a)
{
    if (&a != this)
    {
        DFO_Quantile::operator=(a);
        DSC_XYColSel::operator=(a);
    }
    return *this;
}


void  DFO_XYQuantile::LocalCopy(const DFO_XYQuantile& a)
{
}


void  DFO_XYQuantile:: DoStatusChk()
{
    DFO_Quantile::DoStatusChk();
    DSC_XYColSel::DoCheck(*this);
}

void DFO_XYQuantile:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (operateOnX)
    {
        DoQuantile(inputData->xData);
    }
    else
    {
        DoQuantile(inputData->yData);
    }
}

