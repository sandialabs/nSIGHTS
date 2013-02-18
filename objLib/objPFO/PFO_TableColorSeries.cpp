///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableColorSeries.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <genClass/DO_TableData.h>

#include <objPFO/PFO_TableColorSeries.h>


PFO_TableColorSeries :: PFO_TableColorSeries(PlotDefC& assPlt)
    : PFO_ColorBase("Table Color Series", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_TableColorSeries::PFO_TableColorSeries(const PFO_TableColorSeries& a, PlotDefC& assPlt)
    : PFO_ColorBase(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_TableColorSeries :: ~PFO_TableColorSeries ()
{
}

void PFO_TableColorSeries::InitCommon()
{
    //  base class
    AddInPort(inputDataObjRef,  typeid(DO_TableData));

    DoStatusChk();
}

PFO_TableColorSeries& PFO_TableColorSeries::operator= (const PFO_TableColorSeries& a)
{
    if (&a != this)
    {
        PFO_ColorBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void  PFO_TableColorSeries::FullCopy(const PFO_TableColorSeries& a)
{
    PFO_ColorBase::FullCopy(a);
    LocalCopy(a);
}


void  PFO_TableColorSeries::LocalCopy(const PFO_TableColorSeries& a)
{
    xDataIndx               = a.xDataIndx;
    yDataIndx               = a.yDataIndx;
    zDataIndx               = a.zDataIndx;
    colorDataIndx           = a.colorDataIndx;

    plotAllRows             = a.plotAllRows;
    lastNRows               = a.lastNRows;
    reversePlotOrder        = a.reversePlotOrder;

    pointSymbol             = a.pointSymbol;
    pointSymSize            = a.pointSymSize;
    edgeSpec                = a.edgeSpec;
}


void PFO_TableColorSeries::SetDefault()
{
    //  properties

    xDataIndx   = 0;
    yDataIndx   = 1;
    zDataIndx   = 2;
    if (!is3DObject)
        colorDataIndx   = 2;
    else
        colorDataIndx   = 3;
    plotAllRows = true;
    lastNRows   = 100;
    reversePlotOrder = false;

    //sym properties
    pointSymbol         = ps_FCircle;
    pointSymSize        = 8;
    edgeSpec.plotEdges  = false;

}

void  PFO_TableColorSeries:: DoStatusChk()
{
    autoLimitToData = (baseAutoDataSource == 1);
    // base class check
    PFO_ColorBase::DoStatusChk();
    if (StatusNotOK())
        return;

    if (!CheckInputFO(inputDataObjRef, "Table Data"))
        return;
    objTableData = static_cast<DO_TableData*>(GetInPortData(inputDataObjRef))->tableData;

    //  check parameters
    int maxCol = objTableData->GetNCols();
    if (xDataIndx >= maxCol)
        xDataIndx = 0;
    if (yDataIndx >= maxCol)
        yDataIndx = 0;
    if (zDataIndx >= maxCol)
        zDataIndx = 0;
    if (colorDataIndx >= maxCol)
        colorDataIndx = 0;

    startRow = 0;
    if (!plotAllRows)
    {
        if (lastNRows <1 )
            SetObjErrMsg("at least one row must be plotted");
        startRow = objTableData->dataTable[xDataIndx].Size() - lastNRows;
        if (startRow < 0)
            startRow = 0;
    }

    if (limitIsMaster && autoLimits)
    {
        if (autoLimitToData)
        {
            SetRangeInView();
        }
        else
        {
            const SC_DoubleArray& colorData = objTableData->dataTable[colorDataIndx];
            rangeLimit.InitLimit(colorData);
        }
    }

    CheckRangeLimits();

    recalcOnViewChange   = (limitIsMaster && autoLimits && autoLimitToData);

    colorLimitDO.SetValueLabel(objTableData->GetID());

}

void PFO_TableColorSeries:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
        CalcDataColors();
}

void PFO_TableColorSeries::CalcDataColors()
{
    SC_DoubleArray& colorData = objTableData->dataTable[colorDataIndx];
    rangeLimit.MapDataToRange(colorData, symbolColors, clipToRange, colorMap->ncolorMap);
}

void  PFO_TableColorSeries::ViewChangeRecalc()
{
    DoStatusChk();
    if (StatusOK())
        CalcDataColors();
}

void PFO_TableColorSeries::SetRangeInView()
{
    Limit3D currLimits = GetCurrentViewLimits();
    //  if no limits set ??
    if (RealIsNull(currLimits.minLim.cX))
    {
        SetObjErrMsg("no limits (???) - cannot set range");
        return;
    }

    // if 2D set Z limits to null for limit check
    if (!is3DObject)
    {
        currLimits.minLim.cZ = nullReal;
        currLimits.maxLim.cZ = nullReal;
    }

    bool first = true;
    Coord3D pointCoord;

    const SC_DoubleArray& xData = objTableData->dataTable[xDataIndx];
    const SC_DoubleArray& yData = objTableData->dataTable[yDataIndx];
    const SC_DoubleArray& zData = objTableData->dataTable[zDataIndx];
    const SC_DoubleArray& colorData = objTableData->dataTable[colorDataIndx];

    int nrows = xData.Size() - startRow;
    for (int i = 0; i < nrows; i++)
    {
        if (!is3DObject)
            MapCoord(pointCoord, xData[i], yData[i], nullReal);
        else
            MapCoord(pointCoord, xData[i], yData[i], zData[i]);

        if (currLimits.WithinLimits(pointCoord))
        {
            double color = colorData[i];
            if (first)
            {
                rangeLimit.InitLimit(color);
                first = false;
            }
            else
                rangeLimit.AddToLimit(color);
        }
    }

    if (first)
        SetObjErrMsg("no data in view - cannot set range");
}

void PFO_TableColorSeries::MapCoord(Coord3D &pointCoord,
                                    double  x,
                                    double  y,
                                    double  z)
{
    pointCoord = Coord3D(x, y, z);
    coordMap.MapCoords(pointCoord);
}

Limit3D PFO_TableColorSeries::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (! doPlot))
        return currLimits;

    bool xLin, yLin, zLin;
    if (!is3DObject)
        GetAxesTypes(xLin, yLin);
    else
        GetAxesTypes(xLin, yLin, zLin);

    SC_DoubleArray& xData = objTableData->dataTable[xDataIndx];
    SC_DoubleArray& yData = objTableData->dataTable[yDataIndx];
    SC_DoubleArray& zData = objTableData->dataTable[zDataIndx];

    for (int i = startRow; i < xData.Size(); i++)
    {
        double xVal = xData[i];
        double yVal = yData[i];
        if (!is3DObject)
        {
            if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
                currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
        }
        else
        {
            double zVal = zData[i];
            if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)) && (zLin || (zVal > stdEps)))
                currLimits.AddToLimits(Coord3D(xVal, yVal, zVal));
        }
    }
    return currLimits;

}

