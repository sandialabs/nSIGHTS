///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DMultTables.cpp
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
//    Plots selected column pairs from one or more tables using symbols and/or
// lines on a 2D XY plot.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <genClass/DO_XYData.h>

#include <objPFO/PFO_2DMultTables.h>

PFO_2DMultTables::TableSeries::TableSeries() :
    doSeries(false), xColumn(0), yColumn(1)
{
}
PFO_2DMultTables::TableSeries::TableSeries(const TableSeries& a) :
    PSC_SeriesSpec(a),
    PSC_LegendOverride(a)
{
    LocalCopy(a);
}

bool PFO_2DMultTables::TableSeries::OKtoPlot()
{
    return doSeries && (!tableDataObjRef.IsNull()) && (tableDataDC != 0);
}
PFO_2DMultTables::TableSeries& PFO_2DMultTables::TableSeries::operator= (const TableSeries& a)
{
    if (&a != this)
    {
        PSC_SeriesSpec::operator=(a);
        PSC_LegendOverride::operator=(a);
        LocalCopy(a);
    }
    return *this;
}
void PFO_2DMultTables::TableSeries::LocalCopy(const TableSeries& a)
{
    doSeries        = a.doSeries;
    tableDataObjRef = a.tableDataObjRef;
    xColumn         = a.xColumn;
    yColumn         = a.yColumn;
}

PFO_2DMultTables :: PFO_2DMultTables(PlotDefC& assPlt)
    : PlotObjC("Multiple Table Series", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_2DMultTables::PFO_2DMultTables(const PFO_2DMultTables& a, PlotDefC& assPlt) : PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_2DMultTables :: ~PFO_2DMultTables ()
{
}

void PFO_2DMultTables::InitCommon()
{
    for (int i = 0; i < maxTableSeries; i++)
        AddUseFlagInPort(tableSeries[i].tableDataObjRef, typeid(DO_TableData), tableSeries[i].doSeries);

    seriesLegendData.Alloc(1);
    AddOutPort(seriesLegendData);

    DoStatusChk();
}

PFO_2DMultTables& PFO_2DMultTables::operator= (const PFO_2DMultTables& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void  PFO_2DMultTables::FullCopy(const PFO_2DMultTables& a)
{
    PlotObjC::FullCopy(a);
    LocalCopy(a);
}

void  PFO_2DMultTables::LocalCopy(const PFO_2DMultTables& a)
{
    for (int i = 0; i < maxTableSeries; i++)
        tableSeries[i] = a.tableSeries[i];
}


void PFO_2DMultTables::SetDefault()
{
    for ( int i = 0; i < maxTableSeries; i++)
    {
        tableSeries[i].doSeries = false;
        tableSeries[i].seriesPen = (i + 1) % DC_PenSet::maxPenSet;
        tableSeries[i].symbolType = PlotSymbol(i);
    }
}

void  PFO_2DMultTables:: DoStatusChk()
{

    int i;
    for (i = 0; i < maxTableSeries; i++)
        tableSeries[i].tableDataDC = 0;

    // base class check
    PlotObjC::DoStatusChk();

    if (StatusNotOK())
        return;

    // check input objects
    int nSelected = 0;
    int nOK = 0;
    for (i = 0; i < maxTableSeries; i++)
        if (tableSeries[i].doSeries)
        {
            // we want to allow bad XY if at least one XY is good
            if (CheckInputFO(tableSeries[i].tableDataObjRef, "Series"))
            {
                const DC_TableData* tmpDC = static_cast<DO_TableData*>(GetInPortData(tableSeries[i].tableDataObjRef))->tableData;
                int nCols = tmpDC->GetNCols();
                if ((tableSeries[i].xColumn < nCols) && (tableSeries[i].yColumn < nCols))
                {
                    tableSeries[i].tableDataDC = tmpDC;
                    nOK++;
                }
                else
                    SetObjErrMsg("X or Y column out of range");

            }
            nSelected++;
        }

    if (nSelected == 0)
    {
        SetObjErrMsg("no data selected to plot");
        return;
    }

    if (nOK == 0)
    {
        SetObjErrMsg("all input tables bad/not set");
        return;
    }

    //  reset error status
    objStatus = os_OK;

    // set up output
    seriesLegendData.Alloc(nSelected);
    for (i = 0; i < maxTableSeries; i++)
        if (tableSeries[i].OKtoPlot())
            seriesLegendData.AddSeries(tableSeries[i].GetLabel(tableSeries[i].tableDataDC->GetID()), tableSeries[i]);

    if (cursorReport != 0)
        recalcOnViewChange  = cursorReport->showReport;
}


void PFO_2DMultTables:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
        cursorReport->ReportTitleSetup(GetID());
}

bool PFO_2DMultTables::ContainsLinesOnly()
{
    for ( int i = 0; i < maxTableSeries; i++)
        if (tableSeries[i].OKtoPlot() && tableSeries[i].IsSolid())
            return false;

    return true;
}



Limit3D PFO_2DMultTables::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (! doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    for ( int i = 0; i < maxTableSeries; i++)
        if (tableSeries[i].OKtoPlot())
        {
            const SC_DoubleMatrix& currData = tableSeries[i].tableDataDC->dataTable;
            const SC_DoubleArray& xData = currData[tableSeries[i].xColumn];
            const SC_DoubleArray& yData = currData[tableSeries[i].yColumn];

            for (int j = 0; j < xData.Size(); j++)
            {
                double xVal = xData[j];
                double yVal = yData[j];
                if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
                    currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
            }
        }

    return currLimits;
}

