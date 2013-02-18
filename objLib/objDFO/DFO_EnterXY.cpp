///////////////////////////////////////////////////////////////////////////////////
//
// DFO_EnterXY.cpp
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
//      User entry of XY data in spreadsheet format.  Also allows editing/viewing
//  of existing data
//
///////////////////////////////////////////////////////////////////////////////////

#include <objDFO/DFO_EnterXY.h>

DFO_EnterXY :: DFO_EnterXY() :
    FuncObjC("Enter XY"),
    useInputXY(false)
{
    xyData.Alloc(100);
    InitCommon();
}

DFO_EnterXY::DFO_EnterXY(const DFO_EnterXY& a) :
    FuncObjC(a),
    useInputXY(false)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_EnterXY::InitCommon()
{
    AddUseFlagInPort(inputXYObjRef, typeid(DO_XYData), useInputXY);
    xyDataDO.xyData = &xyData;
    AddOutPort(xyDataDO);
    DoStatusChk();
}

DFO_EnterXY& DFO_EnterXY::operator= (const DFO_EnterXY& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_EnterXY::LocalCopy(const DFO_EnterXY& a)
{
    inputXYObjRef = a.inputXYObjRef;
    xFormat       = a.xFormat;
    yFormat       = a.yFormat;
    xyData        = a.xyData;
}

void  DFO_EnterXY:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (!xyDataDO.DataOK())
        SetObjErrMsg("No data input");
}

void DFO_EnterXY:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
        xyData.SetID("Entered data");
    else
        xyData.SetID("no data");

}

