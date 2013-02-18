///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYConcat.cpp
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

#include <objDFO/DFO_XYConcat.h>

DFO_XYConcat::XYInputData::XYInputData() :
    doXY(false)
{
}
DFO_XYConcat::XYInputData::XYInputData(const XYInputData& a)
{
    LocalCopy(a);
}
DFO_XYConcat::XYInputData& DFO_XYConcat::XYInputData::operator= (const XYInputData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void DFO_XYConcat::XYInputData::LocalCopy(const XYInputData& a)
{
    doXY     = a.doXY;
    xyObjRef = a.xyObjRef;
}

DFO_XYConcat :: DFO_XYConcat()
    : FuncObjC("Concatenate XY"), inputXYDC(maxInputXY)
{
    deleteOverlaps = true;
    InitCommon();
}

DFO_XYConcat::DFO_XYConcat(const DFO_XYConcat& a)
    : FuncObjC(a), inputXYDC(maxInputXY)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYConcat :: ~DFO_XYConcat ()
{
}

void DFO_XYConcat::InitCommon()
{
    AddOutPort(outputXYDO);

    for (int i = 0; i < maxInputXY; i++)
        AddUseFlagInPort(xyToConcat[i].xyObjRef, typeid(DO_XYData), xyToConcat[i].doXY);

    xyToConcat[0].doXY = true;

    outputXYDC.SetID("Combined XY");
    outputXYDO.xyData = &outputXYDC;

    DoStatusChk();
}

DFO_XYConcat& DFO_XYConcat::operator= (const DFO_XYConcat& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_XYConcat::LocalCopy(const DFO_XYConcat& a)
{
    deleteOverlaps = a.deleteOverlaps;
    for (int i = 0; i < maxInputXY; i++)
        xyToConcat[i] = a.xyToConcat[i];
}


void  DFO_XYConcat:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    // check input objects
    inputXYDC.SetEmpty();

    nXY = 0;
    for (int i = 0; i < maxInputXY; i++)
        if (xyToConcat[i].doXY)
        {
            if (!CheckInputFO(xyToConcat[i].xyObjRef, "input XY"))
                return;

            const DC_XYData* nextXY = static_cast<DO_XYData*>(GetInPortData(xyToConcat[i].xyObjRef))->xyData;

            if (nextXY->IsEmpty())
            {
                SetObjErrMsg("input XY has no data ??");
                outputXYDC.SetEmpty();
                return;
            }

            nXY += nextXY->Size();
            inputXYDC += nextXY;
        }

    if (inputXYDC.IsEmpty())
        SetObjErrMsg("no XY to concatenate ??");

    if (StatusNotOK())
        outputXYDC.SetEmpty();
}


void DFO_XYConcat::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    const DC_XYData& inTable = inputXYDC.GetRef(0);
    if (!outputXYDC.Alloc(nXY))
    {
        SetObjErrMsg("could not allocate output XY");
        outputXYDC.SetEmpty();
        return;
    }

    SC_DoubleArray& outX = outputXYDC.xData;
    SC_DoubleArray& outY = outputXYDC.yData;

    for (int i = 0; i < inputXYDC.Size(); i++)
    {
        const DC_XYData& inputXY = inputXYDC.GetRef(i);
        const SC_DoubleArray& inX = inputXY.xData;
        const SC_DoubleArray& inY = inputXY.yData;
        int startX = 0;
        if (deleteOverlaps && outX.IsNotEmpty())
        {
            if (!inX.IsAscending())
            {
                SetObjErrMsg("input X not ascending - cannot check overlaps");
                outputXYDC.SetEmpty();
                return;
            }

            const double& lastX = outX.LastIndex();
            startX = -1;
            for (int j = 0; j < inX.Size(); j++)
                if (inX[j] > lastX)
                {
                    startX = j;
                    break;
                }

            if (startX < 0)
            {
                SetObjErrMsg("entire XY is overlapped");
                outputXYDC.SetEmpty();
                return;
            }
        }

        for (int j = startX; j < inX.Size(); j++)
        {
            outX += inX[j];
            outY += inY[j];
        }
    }
}


