///////////////////////////////////////////////////////////////////////////////////
//
// DO_SeriesLegendData.cpp
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
//      Platform independent base classes for plot definitions and plot functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genPlotClass/DO_SeriesLegendData.h>

char* DO_SeriesLegendData::typeDesc = "SeriesLegend";

DO_SeriesLegendData::DO_SeriesLegendData()
{
    dataNames.SetStringLen(maxDataNamesLen);
}

void DO_SeriesLegendData::Alloc(int nAll)
{
    dataNames.Alloc(nAll);
    dataSeries.Alloc(nAll);
    specIsSeries.Alloc(nAll);
    dataArrow.Alloc(nAll);
};

void DO_SeriesLegendData::Clear()
{
    dataNames.SetEmpty();
    dataSeries.SetEmpty();
    specIsSeries.SetEmpty();
    dataArrow.SetEmpty();
}

void DO_SeriesLegendData::AddSeries(const char            serName[],
                                          const PSC_SeriesSpec&  serData)
{
    dataNames += serName;
    dataSeries += serData;
    specIsSeries += true;
    dataArrow += PSC_ArrowSpec();
}

void DO_SeriesLegendData::AddArrow(const char            serName[],
                                        int             arrowPen,
                                        const PSC_ArrowSpec&  arrowData)
{
    dataNames += serName;
    PSC_SeriesSpec temp;
    temp.seriesPen = arrowPen;
    dataSeries += temp;
    specIsSeries += false;
    dataArrow += arrowData;
}

const PSC_SeriesSpec& DO_SeriesLegendData::GetSeries(int indx) const
{
    if (!specIsSeries[indx])
        GenAppInternalError("DO_SeriesLegendData::GetSeries");

    return dataSeries[indx];
}

const PSC_ArrowSpec& DO_SeriesLegendData::GetArrow(int indx) const
{
    if (specIsSeries[indx])
        GenAppInternalError("DO_SeriesLegendData::GetArrow");
    return dataArrow[indx];
}

bool DO_SeriesLegendData::DataOK()
{
    bool dataOK = false;

    if ( dataNames.IsEmpty())
        SetErrMsg("No data in legend");
    else
        dataOK = true;

    return dataOK;
}

