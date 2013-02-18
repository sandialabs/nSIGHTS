///////////////////////////////////////////////////////////////////////////////////
//
// PFO_EnterXYOnPlot.cpp
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
//    Allows entry/editing of XY data with a mouse.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objPFO/PFO_EnterXYOnPlot.h>


PFO_EnterXYOnPlot :: PFO_EnterXYOnPlot(PlotDefC& assPlt)
    : ActiveObjC("Enter/Edit XY", assPlt, "Enter/Edit XY")
{
    currentPtSymbol = PSC_SymbolSpec(ps_FCircle, 8, 1);
    enteredPtSymbol = PSC_SymbolSpec(ps_FSquare, 6, 3);
    deletedPtSymbol = PSC_SymbolSpec(ps_Square, 6, 6);
    currLinePen     = 2;

    InitCommon();
}

PFO_EnterXYOnPlot::PFO_EnterXYOnPlot(const PFO_EnterXYOnPlot& a, PlotDefC& assPlt) : ActiveObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_EnterXYOnPlot :: ~PFO_EnterXYOnPlot ()
{
}

void PFO_EnterXYOnPlot::InitCommon()
{
    pointStatus.SetResizable();
    pointStatus.Alloc(10);

    useInputXY = false;
    AddUseFlagInPort(inputXYObjRef, typeid(DO_XYData), useInputXY);
    xyDataDO.xyData = &xyDataDC;
    AddOutPort(xyDataDO);

    AddOutPort(lastSelectedXDO);
    AddOutPort(lastSelectedYDO);

    lastSelectedXDO.Reset("MouseX");
    lastSelectedYDO.Reset("MouseY");

    DoStatusChk();
}

PFO_EnterXYOnPlot& PFO_EnterXYOnPlot::operator= (const PFO_EnterXYOnPlot& a)
{
    if (&a != this)
    {
        ActiveObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_EnterXYOnPlot::LocalCopy(const PFO_EnterXYOnPlot& a)
{
    inputXYObjRef      = a.inputXYObjRef   ;
    currentPtSymbol    = a.currentPtSymbol ;
    enteredPtSymbol    = a.enteredPtSymbol ;
    deletedPtSymbol    = a.deletedPtSymbol ;
    currLinePen        = a.currLinePen     ;
}

void  PFO_EnterXYOnPlot::DoStatusChk()
{
    ActiveObjC::DoStatusChk();
}


void PFO_EnterXYOnPlot:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK() && (calcType == foc_Apply))
    {
        DeleteType(pstDeleted);
    }
}


void PFO_EnterXYOnPlot::DeleteType(PointStatusType typeToDelete)
{
    int nOK = 0;
    for (int i = 0; i < xyDataDC.Size(); i++)
        if (pointStatus[i] != typeToDelete)
        {
            xyDataDC.xData[nOK] = xyDataDC.xData[i];
            xyDataDC.yData[nOK] = xyDataDC.yData[i];
            pointStatus[nOK++] = pstCurrent;
        }
    pointStatus.SetSize(nOK);
    xyDataDC.SetSize(nOK);
}

Limit3D PFO_EnterXYOnPlot::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (! doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    for (int j = 0; j < xyDataDC.Size(); j++)
    {
        double xVal = xyDataDC.xData[j];
        double yVal = xyDataDC.yData[j];
        if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
            currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
    }


    return currLimits;
}


void PFO_EnterXYOnPlot::UpdateFrom(const DC_XYData& newXY)
{
    int nNew = newXY.Size();
    xyDataDC.Alloc(nNew);
    pointStatus.Alloc(nNew);
    for (int i = 0; i < nNew; i++)
    {
        xyDataDC.xData[i] = newXY.xData[i];
        xyDataDC.yData[i] = newXY.yData[i];
        pointStatus[i] = pstCurrent;
    }

    xyDataDC.SetSize(nNew);
    pointStatus.SetSize(nNew);
    PlotDefC::ExecuteAndReplot(*this);
}

