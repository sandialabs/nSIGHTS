///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CreateCurve.cpp
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
#include <objClass/DO_Curve.h>

#include <objDFO/DFO_CreateCurve.h>


DFO_CreateCurve :: DFO_CreateCurve() :
    FuncObjC("Create Curve")
{
    InitCommon();
}

DFO_CreateCurve::DFO_CreateCurve(const DFO_CreateCurve& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_CreateCurve :: ~DFO_CreateCurve()
{
}

void DFO_CreateCurve::InitCommon()
{
    AddInPort(xyDataObjRef, typeid(DO_XYData));
    AddOutPort(curveDO);
    curveDO.curve = &curveDC;

    DoStatusChk();
}

DFO_CreateCurve& DFO_CreateCurve::operator= (const DFO_CreateCurve& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}

void  DFO_CreateCurve::LocalCopy(const DFO_CreateCurve& a)
{
    xyDataObjRef = a.xyDataObjRef;
    curveDC      = a.curveDC;
}


void DFO_CreateCurve::SetDefault()
{
}

void  DFO_CreateCurve:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    //  get data
    if (CheckInputFO(xyDataObjRef, "Input XY"))
    {
        xyDataDC = static_cast<DO_XYData*>(GetInPortData(xyDataObjRef))->xyData;
        SC_SetupErr errData;
        if (!curveDC.CurveSetupOK(xyDataDC->Size(), errData))
            SetObjErrMsg(errData.GetErrorText());
    }

}

void DFO_CreateCurve:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
    {
        SC_SetupErr errData;
        if (!curveDC.CreateCurve(*xyDataDC, errData))
            SetObjErrMsg(errData.GetErrorText());
    }
}

