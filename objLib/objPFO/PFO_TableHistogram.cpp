///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableHistogram.cpp
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

#include <genClass/DO_TableData.h>

#include <objPFO/PFO_TableHistogram.h>


PFO_TableHistogram :: PFO_TableHistogram(PlotDefC& assPlt)
    : PlotObjC("Table Bars", assPlt)
{
    xDataIndx = 0;
    yDataIndx = 1;
    histogramPen = 5;
    SetDefault();
    InitCommon();
}

PFO_TableHistogram::PFO_TableHistogram(const PFO_TableHistogram& a, PlotDefC& assPlt) :
    PlotObjC(a, assPlt), PSC_Histogram(a)
{
    LocalCopy(a);
    InitCommon();
}

PFO_TableHistogram :: ~PFO_TableHistogram ()
{
}

void PFO_TableHistogram::InitCommon()
{
    reportOK = false;

    AddInPort(tableInputObjRef, typeid(DO_TableData));

    AddOutPort(seriesLegendData);
    seriesLegendData.Alloc(1);
    seriesLegendData.AddSeries("Histogram input", PSC_SeriesSpec());

    DoStatusChk();
}

PFO_TableHistogram& PFO_TableHistogram::operator= (const PFO_TableHistogram& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        PSC_Histogram::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_TableHistogram::LocalCopy(const PFO_TableHistogram& a)
{
    tableInputObjRef = a.tableInputObjRef;
    histogramPen  = a.histogramPen;
    legendData    = a.legendData;
    xDataIndx = a.xDataIndx;
    yDataIndx = a.yDataIndx;
}


void  PFO_TableHistogram:: DoStatusChk()
{
    PlotObjC::DoStatusChk();

    if (CheckInputFO(tableInputObjRef, "Table data "))
    {
        tableDataDC = static_cast<DO_TableData*>(GetInPortData(tableInputObjRef))->tableData;
        int maxCol = tableDataDC->GetNCols();
        if (xDataIndx >= maxCol)
            xDataIndx = 0;
        if (yDataIndx >= maxCol)
            yDataIndx = 0;
    }
}


void PFO_TableHistogram:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
    {
        PSC_SeriesSpec temp;
        SetLegendData(temp);
        temp.seriesPen = histogramPen;
        seriesLegendData.Clear();
        seriesLegendData.AddSeries(legendData.GetLabel(tableDataDC->GetID()), temp);
    }
}

bool PFO_TableHistogram::ContainsLinesOnly()
{
    return IsLine();
}


Limit3D PFO_TableHistogram::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    int tempX = yDataIndx;
    int tempY = xDataIndx;
    if (positionIsValue)
    {
        tempX = xDataIndx;
        tempY = yDataIndx;
    }

    SC_DoubleArray& xData = tableDataDC->dataTable[tempX];
    SC_DoubleArray& yData = tableDataDC->dataTable[tempY];
    for (int i = 0; i < yData.Size(); i++)
    {
        double  xVal = xData[i];
        double  yVal = yData[i];

        if (positionIsValue)
            if (orientationX)
                xVal = double(i + 1) + indexOffset;
            else
                yVal = double(i + 1) + indexOffset;

        if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
            currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
    }

    if (baselineIsValue)
        currLimits.AddToLimits(Coord3D(nullReal, baselineValue, nullReal));

    return currLimits;
}

