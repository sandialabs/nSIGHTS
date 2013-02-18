///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DXYData.cpp
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
//    Plots data from one or more XY data sets using symbols and/or lines on a
// 2D XY plot.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <genClass/DO_XYData.h>

#include <objPFO/PFO_2DXYData.h>

PFO_2DXYData::XYSeries::XYSeries() :
    doSeries(false)
{
}
PFO_2DXYData::XYSeries::XYSeries(const XYSeries& a) :
    PSC_SeriesSpec(a),
    PSC_LegendOverride(a)
{
    LocalCopy(a);
}

bool PFO_2DXYData::XYSeries::OKtoPlot()
{
    return doSeries && (!xyDataObjRef.IsNull()) && (xyDataDC != 0);
}
PFO_2DXYData::XYSeries& PFO_2DXYData::XYSeries::operator= (const XYSeries& a)
{
    if (&a != this)
    {
        PSC_SeriesSpec::operator=(a);
        PSC_LegendOverride::operator=(a);
        LocalCopy(a);
    }
    return *this;
}
void PFO_2DXYData::XYSeries::LocalCopy(const XYSeries& a)
{
    doSeries        = a.doSeries;
    xyDataObjRef    = a.xyDataObjRef;;
}

PFO_2DXYData :: PFO_2DXYData(PlotDefC& assPlt)
    : PlotObjC("XY Data Series", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_2DXYData::PFO_2DXYData(const PFO_2DXYData& a, PlotDefC& assPlt) : PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_2DXYData :: ~PFO_2DXYData ()
{
}

void PFO_2DXYData::InitCommon()
{
    for (int i = 0; i < maxXYSeries; i++)
        AddUseFlagInPort(xySeries[i].xyDataObjRef, typeid(DO_XYData), xySeries[i].doSeries);

    seriesLegendData.Alloc(1);
    AddOutPort(seriesLegendData);

    DoStatusChk();
}

PFO_2DXYData& PFO_2DXYData::operator= (const PFO_2DXYData& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void  PFO_2DXYData::FullCopy(const PFO_2DXYData& a)
{
    PlotObjC::FullCopy(a);
    LocalCopy(a);
}

void  PFO_2DXYData::LocalCopy(const PFO_2DXYData& a)
{
    for (int i = 0; i < maxXYSeries; i++)
        xySeries[i] = a.xySeries[i];
}


void PFO_2DXYData::SetDefault()
{
    for ( int i = 0; i < maxXYSeries; i++)
    {
        xySeries[i].doSeries = false;
        xySeries[i].seriesPen = (i + 1) % DC_PenSet::maxPenSet;
        xySeries[i].symbolType = PlotSymbol(i);
    }
}

void  PFO_2DXYData:: DoStatusChk()
{

    int i;
    for (i = 0; i < maxXYSeries; i++)
        xySeries[i].xyDataDC = 0;

    // base class check
    PlotObjC::DoStatusChk();

    if (StatusNotOK())
        return;

    // check input objects
    int nSelected = 0;
    int nOK = 0;
    for (i = 0; i < maxXYSeries; i++)
        if (xySeries[i].doSeries)
        {
            // we want to allow bad XY if at least one XY is good
            if (CheckInputFO(xySeries[i].xyDataObjRef, "Series"))
            {
                xySeries[i].xyDataDC = static_cast<DO_XYData*>(GetInPortData(xySeries[i].xyDataObjRef))->xyData;
                nOK++;
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
        SetObjErrMsg("all input XY bad/not set");
        return;
    }

    //  reset error status
    objStatus = os_OK;

    // set up output
    seriesLegendData.Alloc(nSelected);
    for (i = 0; i < maxXYSeries; i++)
        if (xySeries[i].OKtoPlot())
            seriesLegendData.AddSeries(xySeries[i].GetLabel(xySeries[i].xyDataDC->dataID), xySeries[i]);

    if (cursorReport != 0)
        recalcOnViewChange  = cursorReport->showReport;
}


void PFO_2DXYData:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
        SetReportTitles();
}

bool PFO_2DXYData::ContainsLinesOnly()
{
    for ( int i = 0; i < maxXYSeries; i++)
        if (xySeries[i].OKtoPlot() && xySeries[i].IsSolid())
            return false;

    return true;
}



Limit3D PFO_2DXYData::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (! doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    for ( int i = 0; i < maxXYSeries; i++)
        if (xySeries[i].OKtoPlot())
        {
            const DC_XYData& xyData = *xySeries[i].xyDataDC;
            for (int j = 0; j < xyData.Size(); j++)
            {
                double xVal = xyData.xData[j];
                double yVal = xyData.yData[j];
                if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
                    currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
            }
        }

    return currLimits;
}


void PFO_2DXYData::SetReportTitles()
{
    secondReport->showReport = cursorReport->showReport;

    char titStr[80];
    MakeString(titStr, GetID(), "/XY", 80);
    cursorReport->ReportTitleSetup(titStr);

    MakeString(titStr, GetID(), "/Data", 80);
    secondReport->ReportTitleSetup(titStr);
}

