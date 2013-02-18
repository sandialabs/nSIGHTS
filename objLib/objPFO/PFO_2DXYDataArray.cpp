///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DXYDataArray.cpp
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

#include <objPFO/PFO_2DXYDataArray.h>

PFO_2DXYDataArray::XYSeries::XYSeries()
{
    doSeries    = false;
    xyArrayIndx = 0;
}
PFO_2DXYDataArray::XYSeries::XYSeries(const XYSeries& a) :
    PSC_SeriesSpec(a),
    PSC_LegendOverride(a)
{
    LocalCopy(a);
}
PFO_2DXYDataArray::XYSeries& PFO_2DXYDataArray::XYSeries::operator= (const XYSeries& a)
{
    if (&a != this)
    {
        PSC_SeriesSpec::operator=(a);
        PSC_LegendOverride::operator=(a);
        LocalCopy(a);
    }
    return *this;
}
void PFO_2DXYDataArray::XYSeries::LocalCopy(const XYSeries& a)
{
    doSeries    = a.doSeries;
    xyArrayIndx = a.xyArrayIndx;
}


PFO_2DXYDataArray :: PFO_2DXYDataArray(PlotDefC& assPlt)
    : PlotObjC("XY Array Series", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_2DXYDataArray::PFO_2DXYDataArray(const PFO_2DXYDataArray& a, PlotDefC& assPlt) : PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_2DXYDataArray :: ~PFO_2DXYDataArray ()
{
}

void PFO_2DXYDataArray::InitCommon()
{
    AddInPort(xyDataArrayObjRef, typeid(DO_XYDataArray));
    AddOutPort(seriesLegendData);

    DoStatusChk();
}

PFO_2DXYDataArray& PFO_2DXYDataArray::operator= (const PFO_2DXYDataArray& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void  PFO_2DXYDataArray::FullCopy(const PFO_2DXYDataArray& a)
{
    PlotObjC::FullCopy(a);
    LocalCopy(a);
}

void  PFO_2DXYDataArray::LocalCopy(const PFO_2DXYDataArray& a)
{
    xyDataArrayObjRef = a.xyDataArrayObjRef;
    for (int i = 0; i < maxXYSeries; i++)
        seriesData[i] = a.seriesData[i];
}


void PFO_2DXYDataArray::SetDefault()
{
    for ( int i = 0; i < maxXYSeries; i++)
    {
        seriesData[i].doSeries = false;
        seriesData[i].seriesPen = (i + 1) % DC_PenSet::maxPenSet;
        seriesData[i].symbolType = PlotSymbol(i);
        seriesData[i].xyArrayIndx = i;
    }
}

void  PFO_2DXYDataArray:: DoStatusChk()
{
    xyDataArrayDC    = 0;

    // base class check
    PlotObjC::DoStatusChk();

    // check input objects
    if (!CheckInputFO(xyDataArrayObjRef, "XY data array data"))
        return;
    xyDataArrayDC = static_cast<DO_XYDataArray*>(GetInPortData(xyDataArrayObjRef))->xyDataArray;

    //  check parameters
    int maxArray = xyDataArrayDC->Size();
    int nSeries = 0;
    for (int i = 0; i < maxXYSeries; i++)
        if (seriesData[i].PlotSeries(maxArray))
            nSeries++;

    seriesLegendData.Clear();
    if (nSeries == 0)
        SetObjErrMsg("no series set to plot");

    if (StatusNotOK())
        return;

    seriesLegendData.Alloc(nSeries);
    for (int i = 0; i < maxXYSeries; i++)
        if (seriesData[i].PlotSeries(maxArray))
            seriesLegendData.AddSeries(seriesData[i].GetLabel(
                    (*xyDataArrayDC)[seriesData[i].xyArrayIndx].GetID()),
                    seriesData[i]);

    if (cursorReport != 0)
        recalcOnViewChange  = cursorReport->showReport;

}

// objects that are lines only are drawn last
bool PFO_2DXYDataArray::ContainsLinesOnly()
{
    if (StatusNotOK())
        return false;

    int maxArray = xyDataArrayDC->Size();
    for ( int i = 0; i < maxXYSeries; i++)
        if (seriesData[i].PlotSeries(maxArray) && seriesData[i].IsSolid())
            return false;

    return true;
}


void PFO_2DXYDataArray:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
        cursorReport->ReportTitleSetup(GetID());
}

Limit3D PFO_2DXYDataArray::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    int maxArray = xyDataArrayDC->Size();

    for ( int i = 0; i < maxXYSeries; i++)
        if (seriesData[i].PlotSeries(maxArray))
        {
            int serIndx = seriesData[i].xyArrayIndx;
            const SC_DoubleArray& xData = (*xyDataArrayDC)[serIndx].xData;
            const SC_DoubleArray& yData = (*xyDataArrayDC)[serIndx].yData;

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

