///////////////////////////////////////////////////////////////////////////////////
//
// PFO_SeriesLegend.cpp
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
//      creates legend box showing line types and symbols with legend.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genPlotClass/C_PlotDef.h>

#include <genPFO/PFO_SeriesLegend.h>

PFO_SeriesLegend::SeriesLegend& PFO_SeriesLegend::SeriesLegend::operator= (const SeriesLegend& a)
{
    if (&a != this)
    {
        seriesObjRef = a.seriesObjRef;
        doSeries = a.doSeries;
        dataSource = 0;
    }
    return *this;
}


PFO_SeriesLegend :: PFO_SeriesLegend(PlotDefC& assPlt)
    : AnnoObjC("Line & Symbol Legend", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_SeriesLegend::PFO_SeriesLegend(const PFO_SeriesLegend& a, PlotDefC& assPlt) : AnnoObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_SeriesLegend :: ~PFO_SeriesLegend ()
{
}

void PFO_SeriesLegend::InitCommon()
{
    for ( int i = 0; i < maxSeriesLegend; i++)
        AddUseFlagInPort(legendData[i].seriesObjRef, typeid(DO_SeriesLegendData), legendData[i].doSeries);

    DoStatusChk();
}

PFO_SeriesLegend& PFO_SeriesLegend::operator= (const PFO_SeriesLegend& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void  PFO_SeriesLegend::FullCopy(const PFO_SeriesLegend& a)
{
    AnnoObjC::FullCopy(a);
    LocalCopy(a);
}

void  PFO_SeriesLegend::LocalCopy(const PFO_SeriesLegend& a)
{
    seriesSize = a.seriesSize;
    seriesNpts = a.seriesNpts;
    for (int i = 0; i < maxSeriesLegend; i++)
        legendData[i] = a.legendData[i];
}


void PFO_SeriesLegend::SetDefault()
{
    seriesSize = 10.0;
    seriesNpts = 3;
    for ( int i = 0; i < maxSeriesLegend; i++)
        legendData[i].doSeries = false;
    AnnoObjC::SetDefault();
}


void  PFO_SeriesLegend:: DoStatusChk()
{
    AnnoObjC::DoStatusChk();
    if (objStatus != os_OK)
        return;

    bool serFound = false;
    for ( int i = 0; i < maxSeriesLegend; i++)
        if (legendData[i].doSeries)
        {
            if (!CheckInputFO(legendData[i].seriesObjRef, "Series"))
                return;
            legendData[i].dataSource = (DO_SeriesLegendData*) GetInPortData(legendData[i].seriesObjRef);
            serFound = true;
        }

    if (!serFound)
        SetObjErrMsg("no data selected");
}

