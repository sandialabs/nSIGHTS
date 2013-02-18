///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DTableSeries.cpp
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
//    Plots one or more selected columns againsta  single X column from a single
// table using symbols and/or lines on a 2D XY plot
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <objPFO/PFO_2DTableSeries.h>


PFO_2DTableSeries::TableSeries::TableSeries()
{
    doSeries              = false;
    tableColIndx          = 0;
}
PFO_2DTableSeries::TableSeries::TableSeries(const TableSeries& a) :
    PSC_SeriesSpec(a),
    PSC_LegendOverride(a)
{
    LocalCopy(a);
}
PFO_2DTableSeries::TableSeries& PFO_2DTableSeries::TableSeries::operator= (const TableSeries& a)
{
    if (&a != this)
    {
        PSC_SeriesSpec::operator=(a);
        PSC_LegendOverride::operator=(a);
        LocalCopy(a);
    }
    return *this;
}
void PFO_2DTableSeries::TableSeries::LocalCopy(const TableSeries& a)
{
    doSeries              = a.doSeries             ;
    tableColIndx          = a.tableColIndx         ;
}


PFO_2DTableSeries :: PFO_2DTableSeries(PlotDefC& assPlt)
    : PlotObjC("Single Table Series", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_2DTableSeries::PFO_2DTableSeries(const PFO_2DTableSeries& a, PlotDefC& assPlt) : PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_2DTableSeries :: ~PFO_2DTableSeries ()
{
}

void PFO_2DTableSeries::InitCommon()
{
    AddInPort(tableDataObjRef,        typeid(DO_TableData));
    AddOutPort(seriesLegendData);

    DoStatusChk();
}

PFO_2DTableSeries& PFO_2DTableSeries::operator= (const PFO_2DTableSeries& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void  PFO_2DTableSeries::FullCopy(const PFO_2DTableSeries& a)
{
    PlotObjC::FullCopy(a);
    LocalCopy(a);
}

void  PFO_2DTableSeries::LocalCopy(const PFO_2DTableSeries& a)
{
    tableDataObjRef = a.tableDataObjRef;
    xDataIndx       = a.xDataIndx;
    plotAllRows     = a.plotAllRows;
    lastNRows       = a.lastNRows;
    shadeBetween    = a.shadeBetween;

    for (int i = 0; i < maxTableSeries; i++)
        seriesData[i] = a.seriesData[i];
}


void PFO_2DTableSeries::SetDefault()
{
    for ( int i = 0; i < maxTableSeries; i++)
    {
        seriesData[i].doSeries = false;
        seriesData[i].seriesPen = (i + 1) % DC_PenSet::maxPenSet;
        seriesData[i].symbolType = PlotSymbol(i);
        seriesData[i].tableColIndx = i + 1;
    }
    xDataIndx   = 0;
    plotAllRows = true;
    lastNRows   = 100;
    shadeBetween = false;
}

void  PFO_2DTableSeries:: DoStatusChk()
{
    objTableData    = 0;

    // base class check
    PlotObjC::DoStatusChk();

    // check input objects
    if (!CheckInputFO(tableDataObjRef, "Table Data"))
        return;
    objTableData = static_cast<DO_TableData*>(GetInPortData(tableDataObjRef))->tableData;

    //  check parameters
    int maxCol = objTableData->GetNCols();
    if (xDataIndx >= maxCol)
        SetObjErrMsg("X index out of range for current table");

    int nSeries = 0;
    for ( int i = 0; i < maxTableSeries; i++)
        if (seriesData[i].doSeries && (seriesData[i].tableColIndx < maxCol))
            nSeries++;

    seriesLegendData.Clear();
    if (nSeries == 0)
        SetObjErrMsg("no series set to plot");

    if (StatusNotOK())
        return;

    seriesLegendData.Alloc(nSeries);
    for (int i = 0; i < maxTableSeries; i++)
        if (seriesData[i].doSeries && (seriesData[i].tableColIndx < maxCol))
            seriesLegendData.AddSeries(seriesData[i].GetLabel(
                            objTableData->columnDesc[seriesData[i].tableColIndx]),
                            seriesData[i]);

    if (cursorReport != 0)
        recalcOnViewChange  = cursorReport->showReport;

    startRow = 0;
    if (!plotAllRows)
    {
        if (lastNRows <1 )
            SetObjErrMsg("at least one row must be plotted");
        startRow = objTableData->dataTable[xDataIndx].Size() - lastNRows;
        if (startRow < 0)
            startRow = 0;
    }

}

// objects that are lines only are drawn last
bool PFO_2DTableSeries::ContainsLinesOnly()
{
    if (StatusNotOK())
        return false;

    if (shadeBetween)
        return false;

    int maxCol = objTableData->GetNCols();
    for ( int i = 0; i < maxTableSeries; i++)
        if (seriesData[i].doSeries && (seriesData[i].tableColIndx < maxCol) && seriesData[i].IsSolid())
            return false;

    return true;
}


void PFO_2DTableSeries:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
        cursorReport->ReportTitleSetup(GetID());
}

Limit3D PFO_2DTableSeries::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    int maxCol = objTableData->GetNCols();
    SC_DoubleArray& xData = objTableData->dataTable[xDataIndx];

    for ( int i = 0; i < maxTableSeries; i++)
        if (seriesData[i].doSeries && (seriesData[i].tableColIndx < maxCol))
        {
            SC_DoubleArray& yData = objTableData->dataTable[seriesData[i].tableColIndx];

            for (int j = startRow; j < xData.Size(); j++)
            {
                double xVal = xData[j];
                double yVal = yData[j];
                if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
                    currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
            }
        }

    return currLimits;
}
