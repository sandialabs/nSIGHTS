///////////////////////////////////////////////////////////////////////////////////
//
// PFO_XYHistogram.cpp
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
//    Plotting functional objects for cube, grid, tables and XY data.
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genClass/DO_XYData.h>

#include <objPFO/PFO_XYHistogram.h>


PFO_XYHistogram :: PFO_XYHistogram(PlotDefC& assPlt)
    : PlotObjC("XY Bars", assPlt)
{
    histogramPen = 5;
    SetDefault();
    InitCommon();
}

PFO_XYHistogram::PFO_XYHistogram(const PFO_XYHistogram& a, PlotDefC& assPlt) :
    PlotObjC(a, assPlt), PSC_Histogram(a)
{
    LocalCopy(a);
    InitCommon();
}

PFO_XYHistogram :: ~PFO_XYHistogram ()
{
}

void PFO_XYHistogram::InitCommon()
{
    reportOK = false;

    AddInPort(xyInputObjRef, typeid(DO_XYData));

    AddOutPort(seriesLegendData);
    seriesLegendData.Alloc(1);
    seriesLegendData.AddSeries("Histogram input", PSC_SeriesSpec());

    DoStatusChk();
}

PFO_XYHistogram& PFO_XYHistogram::operator= (const PFO_XYHistogram& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        PSC_Histogram::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_XYHistogram::LocalCopy(const PFO_XYHistogram& a)
{
    xyInputObjRef = a.xyInputObjRef;
    histogramPen  = a.histogramPen;
    legendData    = a.legendData;
}


void  PFO_XYHistogram:: DoStatusChk()
{
    PlotObjC::DoStatusChk();

    if (CheckInputFO(xyInputObjRef, "XY data "))
        xyDataDC = static_cast<DO_XYData*>(GetInPortData(xyInputObjRef))->xyData;

}


void PFO_XYHistogram:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
    {
        PSC_SeriesSpec temp;
        SetLegendData(temp);
        temp.seriesPen = histogramPen;
        seriesLegendData.Clear();
        seriesLegendData.AddSeries(legendData.GetLabel(xyDataDC->GetID()), temp);
    }
}

bool PFO_XYHistogram::ContainsLinesOnly()
{
    return IsLine();
}


Limit3D PFO_XYHistogram::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    const SC_DoubleArray& xData = xyDataDC->xData;
    const SC_DoubleArray& yData = xyDataDC->yData;
    for (int i = 0; i < xData.Size(); i++)
    {
        double xVal = xData[i];
        double yVal = yData[i];
        if (orientationX)
        {
            if (!positionIsValue)
                xVal = double(i + 1) + indexOffset;
        }
        else
        {
            if (!positionIsValue)
                yVal = double(i + 1) + indexOffset;
        }
        if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
            currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
    }

    if (baselineIsValue)
        if (orientationX)
            currLimits.AddToLimits(Coord3D(nullReal, baselineValue, nullReal));
        else
            currLimits.AddToLimits(Coord3D(baselineValue, nullReal, nullReal));

    return currLimits;
}

