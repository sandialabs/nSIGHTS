///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectCurve.cpp
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

#include <objDFO/DFO_SelectCurve.h>

DFO_SelectCurve :: DFO_SelectCurve() : DFO_SelectBase("SelectCurve", baseMS)
{
    InitCommon();
}

DFO_SelectCurve::DFO_SelectCurve(const DFO_SelectCurve& a) : 
    DFO_SelectBase(a, baseMS)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectCurve::InitCommon()
{
    curveDO.curve = &curve;
    AddOutPort(curveDO);
    AddInPort(curveArrayObjRef, typeid(DO_CurveArray));

    DoStatusChk();
}

DFO_SelectCurve& DFO_SelectCurve::operator= (const DFO_SelectCurve& a)
{
    if (&a != this)
    {
        DFO_SelectBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectCurve::LocalCopy(const DFO_SelectCurve& a)
{
    curveArrayObjRef    = a.curveArrayObjRef;
}

void  DFO_SelectCurve:: DoStatusChk()
{
    DFO_SelectBase::DoStatusChk();
    indexMS.SetTypeLabel("Selected curve");

    //  check input objects
    if (!CheckInputFO(curveArrayObjRef, "Data source"))
        return;

    dataDO = static_cast<DO_CurveArray*>(GetInPortData(curveArrayObjRef));

    if (IndexRangeOK(dataDO->curveArray->Size()))
    {
        indexMS.indexDO.SetValueLabel((*dataDO->curveArray)[indexMS.GetCurrentIndex()].curveID);
    }

}

void DFO_SelectCurve:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int index = indexMS.GetCurrentIndex();

    curve  = (*dataDO->curveArray)[index];

    SetDefaultID((*dataDO->curveArray)[index].curveID);
}

