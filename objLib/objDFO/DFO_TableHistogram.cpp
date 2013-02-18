///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableHistogram.cpp
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
//      Creates a histogram based on a single input table column
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/DO_TableData.h>

#include <objDFO/DFO_TableHistogram.h>


DFO_TableHistogram :: DFO_TableHistogram() : DFO_Histogram("Table Histogram")
{
    InitCommon();
}

DFO_TableHistogram::DFO_TableHistogram(const DFO_TableHistogram& a) : 
    DFO_Histogram(a),
    DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableHistogram :: ~DFO_TableHistogram ()
{
}

void DFO_TableHistogram::InitCommon()
{
    DSC_TableColSel::AddPort(*this);
    DoStatusChk();
}

DFO_TableHistogram& DFO_TableHistogram::operator= (const DFO_TableHistogram& a)
{
    if (&a != this)
    {
        DFO_Histogram::operator=(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableHistogram::LocalCopy(const DFO_TableHistogram& a)
{
}


void  DFO_TableHistogram:: DoStatusChk()
{
    DFO_Histogram::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
}


void DFO_TableHistogram:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
        DoHistogram(*selectedColData);
}

