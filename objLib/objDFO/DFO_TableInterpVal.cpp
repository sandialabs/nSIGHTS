///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableInterpVal.cpp
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
//      Interpolates Y value given X value and two selected table columns
//  representing X and Y vectors.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <objDFO/DFO_TableInterpVal.h>

DFO_TableInterpVal :: DFO_TableInterpVal() :
    DFO_InterpVal("Interpolate Table")
{
    InitCommon();
}

DFO_TableInterpVal::DFO_TableInterpVal(const DFO_TableInterpVal& a) :
    DFO_InterpVal(a),
    DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableInterpVal :: ~DFO_TableInterpVal ()
{
}

void DFO_TableInterpVal::InitCommon()
{
    DSC_TableColSel::AddPort(*this);
    DoStatusChk();
}

DFO_TableInterpVal& DFO_TableInterpVal::operator= (const DFO_TableInterpVal& a)
{
    if (&a != this)
    {
        DFO_InterpVal::operator=(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableInterpVal::LocalCopy(const DFO_TableInterpVal& a)
{
    selectedYCol   = a.selectedYCol;
}


void  DFO_TableInterpVal:: DoStatusChk()
{
    DFO_InterpVal::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
    if (StatusOK())
    {
        if (inputData->dataTable.Size() <= selectedYCol)
            SetObjErrMsg("Y column index out of range");
    }
}

void DFO_TableInterpVal:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
        DoInterpVal(*selectedColData, inputData->dataTable[selectedYCol]);
}

