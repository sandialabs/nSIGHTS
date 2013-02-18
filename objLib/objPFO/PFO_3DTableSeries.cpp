///////////////////////////////////////////////////////////////////////////////////
//
// PFO_3DTableSeries.cpp
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
//    Plots three selected columns for a single table using symbols and/or lines
// on a 3D XYZ plot.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <genClass/DO_TableData.h>

#include <objPFO/PFO_3DTableSeries.h>


PFO_3DTableSeries :: PFO_3DTableSeries(PlotDefC& assPlt)
    : PlotObjC("Single Table Series", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_3DTableSeries::PFO_3DTableSeries(const PFO_3DTableSeries& a, PlotDefC& assPlt) : PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_3DTableSeries :: ~PFO_3DTableSeries ()
{
}

void PFO_3DTableSeries::InitCommon()
{
    reportOK        = false;                // no reporting
    usePolyOffset   = true;

    //  base class
    AddInPort(tableDataObjRef,  typeid(DO_TableData));
    AddOutPort(seriesLegendData);

    DoStatusChk();
}

PFO_3DTableSeries& PFO_3DTableSeries::operator= (const PFO_3DTableSeries& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void  PFO_3DTableSeries::FullCopy(const PFO_3DTableSeries& a)
{
    PlotObjC::FullCopy(a);
    LocalCopy(a);
}

void  PFO_3DTableSeries::LocalCopy(const PFO_3DTableSeries& a)
{
    tableDataObjRef         = a.tableDataObjRef;
    xyzSeries               = a.xyzSeries;
    xDataIndx               = a.xDataIndx;
    yDataIndx               = a.yDataIndx;
    zDataIndx               = a.zDataIndx;

    plotAllRows             = a.plotAllRows;
    lastNRows               = a.lastNRows;

    seriesLabel             = a.seriesLabel;
}


void PFO_3DTableSeries::SetDefault()
{
    //  properties
    xyzSeries    = PSC_3DSeriesSpec();
    seriesLabel  = PSC_LegendOverride();

    xDataIndx   = 0;
    yDataIndx   = 1;
    zDataIndx   = 2;
    plotAllRows = true;
    lastNRows   = 100;

}

void  PFO_3DTableSeries:: DoStatusChk()
{
    // base class check
    PlotObjC::DoStatusChk();

    if (!CheckInputFO(tableDataObjRef, "Table Data"))
        return;
    objTableData = static_cast<DO_TableData*>(GetInPortData(tableDataObjRef))->tableData;

    //  check parameters
    int maxCol = objTableData->GetNCols();
    if (xDataIndx >= maxCol)
        SetObjErrMsg("X index out of range for current table");
//        xDataIndx = 0;
    if (yDataIndx >= maxCol)
        SetObjErrMsg("Y index out of range for current table");
//        yDataIndx = 0;
    if (zDataIndx >= maxCol)
        SetObjErrMsg("Z index out of range for current table");
//        zDataIndx = 0;

    if (StatusNotOK())
        return;

    // set up output
    seriesLegendData.Alloc(1);
    seriesLegendData.AddSeries(seriesLabel.GetLabel(objTableData->columnDesc[zDataIndx]), xyzSeries);

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

bool PFO_3DTableSeries::ContainsLinesOnly()
{
    return !xyzSeries.IsSolid();
}


void PFO_3DTableSeries:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

Limit3D PFO_3DTableSeries::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (! doPlot))
        return currLimits;

    bool xLin, yLin, zLin;
    GetAxesTypes(xLin, yLin, zLin);

    SC_DoubleArray& xData = objTableData->dataTable[xDataIndx];
    SC_DoubleArray& yData = objTableData->dataTable[yDataIndx];
    SC_DoubleArray& zData = objTableData->dataTable[zDataIndx];

    for (int i = startRow; i < xData.Size(); i++)
    {
        double xVal = xData[i];
        double yVal = yData[i];
        double zVal = zData[i];
        if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)) && (zLin || (zVal > stdEps)))
            currLimits.AddToLimits(Coord3D(xVal, yVal, zVal));
    }
    return currLimits;
}

