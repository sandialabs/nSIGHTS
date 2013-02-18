///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CurveInterp.cpp
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

#include <genClass/U_Str.h>
#include <objClass/DO_Curve.h>

#include <objDFO/DFO_CurveInterp.h>

DFO_CurveInterp :: DFO_CurveInterp() :
    FuncObjC("Curve Interpolate")
{
    SetDefault();
    InitCommon();
}

DFO_CurveInterp::DFO_CurveInterp(const DFO_CurveInterp& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_CurveInterp :: ~DFO_CurveInterp()
{
}

void DFO_CurveInterp::InitCommon()
{
    AddInPort(curveObjRef,typeid(DO_Curve));
    AddInPort(xyInputObjRef, typeid(DO_XYData));
    xyInputCheckRef = GetLastInPortCheckRef();

    xyDataDO.xyData = &interpOutput;
    AddOutPort(xyDataDO);
    interpOutput.SetID("InterpOutput");

    DoStatusChk();
}

DFO_CurveInterp& DFO_CurveInterp::operator= (const DFO_CurveInterp& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}

void  DFO_CurveInterp::LocalCopy(const DFO_CurveInterp& a)
{
    curveObjRef         = a.curveObjRef;
    xyInputObjRef       = a.xyInputObjRef;
    interpData          = a.interpData;
}


void DFO_CurveInterp::SetDefault()
{
    interpData = DSC_InterpPoints();
}

void  DFO_CurveInterp:: DoStatusChk()
{
    *xyInputCheckRef = interpData.AtInputPoints();

    FuncObjC::DoStatusChk();

    if (!CheckInputFO(curveObjRef, "CurveInterp"))
        return;

    curveData = static_cast<DO_Curve*>(GetInPortData(curveObjRef))->curve;

    if (interpData.AtInputPoints())
    {
        if (!CheckInputFO(xyInputObjRef, "XY input"))
            return;
        interpInput = static_cast<DO_XYData*>(GetInPortData(xyInputObjRef))->xyData;
    }
    else
    {
        if (interpData.useDataLimits)
            interpData.SetMinMax(curveData->inputXmin, curveData->inputXmax);

        SC_SetupErr interpErr;
        if (!interpData.InterpSetupOK(interpErr))
            SetObjErrMsg(interpErr.GetErrorText());
    }
}

void DFO_CurveInterp:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (interpData.AtInputPoints())
    {
        // just to fill it in
        if (!interpOutput.CreateFrom(interpInput->xData, interpInput->xData))
            SetObjErrMsg("memory ??");
    }
    else
    {
        interpData.GetInterpPoints(interpOutput);
    }
    curveData->GetCurveY(interpOutput);
}

