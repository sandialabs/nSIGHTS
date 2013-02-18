///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ViewXY.cpp
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

#include <objDFO/DFO_ViewXY.h>

DFO_ViewXY :: DFO_ViewXY() :
    FuncObjC("View XY")
{
    InitCommon();
}

DFO_ViewXY::DFO_ViewXY(const DFO_ViewXY& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ViewXY::InitCommon()
{
    AddInPort(inputXYObjRef, typeid(DO_XYData));
    AddOutPort(xyDataDO);
    DoStatusChk();
}

DFO_ViewXY& DFO_ViewXY::operator= (const DFO_ViewXY& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ViewXY::LocalCopy(const DFO_ViewXY& a)
{
    inputXYObjRef = a.inputXYObjRef;
    xFormat       = a.xFormat;
    yFormat       = a.yFormat;
}

void  DFO_ViewXY:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (CheckInputFO(inputXYObjRef, "XY input data source"))
        xyDataDO = *(static_cast<DO_XYData*>(GetInPortData(inputXYObjRef)));
}

void DFO_ViewXY:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

