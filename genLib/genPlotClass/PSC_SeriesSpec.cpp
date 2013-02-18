///////////////////////////////////////////////////////////////////////////////////
//
// PSC_SeriesSpec.cpp
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

#include <genPlotClass/PSC_SeriesSpec.h>

PSC_SeriesSpec::PSC_SeriesSpec()
{
    seriesPlotType = psl_LineOnly;
    seriesPen      = 1;

    symbolType     = ps_Square;
    symbolSize     = 8;

    lineThickness  = 1;
    lineType       = plt_Solid;
    mitreLines     = false;
}

PSC_SeriesSpec::PSC_SeriesSpec(const PSC_SeriesSpec& a)
{
    LocalCopy(a);
}
PSC_SeriesSpec::PSC_SeriesSpec(const PSC_LineSpec& a)
{
    LocalCopy(a);
}
PSC_SeriesSpec::PSC_SeriesSpec(const PSC_SymbolSpec& a)
{
    LocalCopy(a);
}



PSC_SeriesSpec&  PSC_SeriesSpec::operator= (const PSC_SeriesSpec& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

PSC_SeriesSpec&  PSC_SeriesSpec::operator= (const PSC_LineSpec& a)
{
    LocalCopy(a);
    return *this;
}

PSC_SeriesSpec&  PSC_SeriesSpec::operator= (const PSC_SymbolSpec& a)
{
    LocalCopy(a);
    return *this;
}

void  PSC_SeriesSpec::LocalCopy(const PSC_SeriesSpec& a)
{
    seriesPlotType = a.seriesPlotType ;
    seriesPen      = a.seriesPen      ;

    symbolType     = a.symbolType     ;
    symbolSize     = a.symbolSize     ;

    lineThickness  = a.lineThickness  ;
    lineType       = a.lineType       ;
    mitreLines     = a.mitreLines;
}

void  PSC_SeriesSpec::LocalCopy(const PSC_LineSpec& a)
{
    seriesPlotType = psl_LineOnly;
    seriesPen      = a.linePen;

    symbolType     = ps_Square;
    symbolSize     = 8;

    lineThickness  = a.lineThickness  ;
    lineType       = a.lineType       ;
    mitreLines     = a.mitreLines;
}
void  PSC_SeriesSpec::LocalCopy(const PSC_SymbolSpec& a)
{
    seriesPlotType = psl_SymbolOnly;

    seriesPen      = a.symbolPen      ;
    symbolType     = a.symbolType     ;
    symbolSize     = a.symbolSize     ;

    lineThickness  = 1;
    lineType       = plt_Solid;
    mitreLines     = false;
}


bool  PSC_SeriesSpec::IsSolid()
{
    return (seriesPlotType != psl_LineOnly) && PlotSymbolOps::SymbolIsFilled(symbolType);

}

