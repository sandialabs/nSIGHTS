///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableErrorBars.cpp
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

#include <objPFO/PFO_TableErrorBars.h>


PFO_TableErrorBars :: PFO_TableErrorBars(PlotDefC& assPlt)
    : PlotObjC("Table Error Bars", assPlt)
{
    xdataIndx   = 0;
    ydataIndx   = 1;
    dxIndx      = 2;
    dyIndx      = 3;
    xminIndx    = 2;
    xmaxIndx    = 3;
    yminIndx    = 4;
    ymaxIndx    = 5;

    errorBarData    = ebtMinMax;
    errorBarFormat  = ebfBox;
    errorBarType    = ebtXandY;

    lineHasCrossTick  = true;
    crossTickPixels   = 5;

    InitCommon();
}

PFO_TableErrorBars::PFO_TableErrorBars(const PFO_TableErrorBars& a, PlotDefC& assPlt) :
    PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_TableErrorBars :: ~PFO_TableErrorBars ()
{
}

void PFO_TableErrorBars::InitCommon()
{
    reportOK = false;

    AddInPort(tableInputObjRef, typeid(DO_TableData));

    AddOutPort(seriesLegendData);
    seriesLegendData.Alloc(1);
    seriesLegendData.AddSeries("Error bar input", PSC_SeriesSpec());

    DoStatusChk();
}

PFO_TableErrorBars& PFO_TableErrorBars::operator= (const PFO_TableErrorBars& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_TableErrorBars::LocalCopy(const PFO_TableErrorBars& a)
{
    tableInputObjRef = a.tableInputObjRef;

    xdataIndx   = a.xdataIndx;
    ydataIndx   = a.ydataIndx;
    dxIndx      = a.dxIndx;
    dyIndx      = a.dyIndx;
    xminIndx    = a.xminIndx;
    xmaxIndx    = a.xmaxIndx;
    yminIndx    = a.yminIndx;
    ymaxIndx    = a.ymaxIndx;

    errorBarData    = a.errorBarData;
    errorBarFormat  = a.errorBarFormat;
    errorBarType    = a.errorBarType;


    lineHasCrossTick  = a.lineHasCrossTick;
    crossTickPixels   = a.crossTickPixels;
}


void  PFO_TableErrorBars:: DoStatusChk()
{
    PlotObjC::DoStatusChk();

    if (!CheckInputFO(tableInputObjRef, "Table data "))
        return;

    tableDataDC = static_cast<DO_TableData*>(GetInPortData(tableInputObjRef))->tableData;
    int maxCol = tableDataDC->GetNCols();

    bool dataPtRequired = (errorBarFormat == ebfLine) || (errorBarData == ebdDelta);
    if (dataPtRequired && ((xdataIndx >= maxCol) || (ydataIndx >= maxCol)))
        SetObjErrMsg("X and Y data column out of range");

    if (errorBarData == ebdDelta)
    {
        if ((errorBarType != ebtYOnly) && (dxIndx >= maxCol))
            SetObjErrMsg("dX data column out of range");
        if ((errorBarType != ebtXOnly) && (dyIndx >= maxCol))
            SetObjErrMsg("dY data column out of range");
    }
    else
    {
        if ((errorBarType != ebtYOnly) && ((xminIndx >= maxCol) || (xmaxIndx >= maxCol) ||(xminIndx == xmaxIndx)))
            SetObjErrMsg("X min or X max data column out of range or the same");
        if ((errorBarType != ebtXOnly) && ((yminIndx >= maxCol) || (ymaxIndx >= maxCol) ||(yminIndx == ymaxIndx)))
            SetObjErrMsg("Y min or Y max data column out of range or the same");
    }
}


void PFO_TableErrorBars:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
    {
        PSC_SeriesSpec temp(barLineSpec);
        seriesLegendData.Clear();
        seriesLegendData.AddSeries(legendData.GetLabel(tableDataDC->GetID()), temp);
    }
}

bool PFO_TableErrorBars::ContainsLinesOnly()
{
    return true;
}


Limit3D PFO_TableErrorBars::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

//  PD_2DXYPS*  currXY = dynamic_cast<PD_2DXYPS*>(assocPlot);
//  if (currXY == 0)
//      GenAppInternalError("PFO_TimeAxesGL::DrawPlotObject");

    const SC_DoubleMatrix& errorData = tableDataDC->dataTable;

    double xVal, yVal;
    for (int i = 0; i < tableDataDC->GetNRows(); i++)
    {
        // special box case first
        if ((errorBarFormat == ebfBox) && (errorBarData == ebtMinMax))
        {
            xVal = errorData[xminIndx][i];
            yVal = errorData[yminIndx][i];
            if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
                currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));

            xVal = errorData[xmaxIndx][i];
            yVal = errorData[ymaxIndx][i];
            if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
                currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
        }
        else
        {
            if (((errorBarType != ebtXOnly) || (errorBarFormat == ebfBox)) && (xLin || (xVal > stdEps)))
            {
                xVal = errorData[xdataIndx][i];
                double ymin, ymax;
                if (errorBarData == ebtMinMax)
                {
                    ymin = errorData[yminIndx][i];
                    ymax = errorData[ymaxIndx][i];
                }
                else
                {
                    yVal = errorData[ydataIndx][i];
                    if (RealIsNull(yVal))
                        continue;

                    double dy = errorData[dyIndx ][i] / 2.0;
                    ymin = yVal - dy;
                    ymax = yVal + dy;
                }
                if (yLin || (ymin > stdEps))
                    currLimits.AddToLimits(Coord3D(xVal, ymin, nullReal));
                if (yLin || (ymax > stdEps))
                    currLimits.AddToLimits(Coord3D(xVal, ymax, nullReal));
            }
            if (((errorBarType != ebtYOnly) || (errorBarFormat == ebfBox)) && (yLin || (yVal > stdEps)))
            {
                yVal = errorData[ydataIndx][i];
                double xmin, xmax;
                if (errorBarData == ebtMinMax)
                {
                    xmin = errorData[xminIndx][i];
                    xmax = errorData[xmaxIndx][i];
                }
                else
                {
                    xVal = errorData[xdataIndx][i];
                    if (RealIsNull(xVal))
                        continue;
                    double dx = errorData[dxIndx ][i] / 2.0;
                    xmin = xVal - dx;
                    xmax = xVal + dx;
                }
                if (xLin || (xmin > stdEps))
                    currLimits.AddToLimits(Coord3D(xmin, yVal, nullReal));
                if (xLin || (xmax > stdEps))
                    currLimits.AddToLimits(Coord3D(xmax, yVal, nullReal));
            }
        }
    }

    return currLimits;
}

