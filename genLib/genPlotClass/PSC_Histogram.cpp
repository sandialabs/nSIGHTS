///////////////////////////////////////////////////////////////////////////////////
//
// PSC_Histogram.cpp
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

#include <genPlotClass/PSC_Histogram.h>


PSC_Histogram::PSC_Histogram()
{
    SetDefault();
}

PSC_Histogram::PSC_Histogram(const PSC_Histogram& a)
: PSC_EdgeSpec(a)
{
    LocalCopy(a);
}


PSC_Histogram& PSC_Histogram::operator= (const PSC_Histogram& a)
{
    if (&a != this)
    {
        PSC_EdgeSpec::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void PSC_Histogram::LocalCopy(const PSC_Histogram& a)
{
    orientationX        = a.orientationX;
    baselineIsValue     = a.baselineIsValue;
    baselineValue       = a.baselineValue;
    positionIsValue     = a.positionIsValue;
    widthSpec           = a.widthSpec;
    widthLineThk        = a.widthLineThk;
    widthSpecValue      = a.widthSpecValue;
    widthSpecPixel      = a.widthSpecPixel;
    widthSpecPct        = a.widthSpecPct;
    indexOffset         = a.indexOffset;
}


void PSC_Histogram::SetLegendData(PSC_SeriesSpec& legendSpec)
{
    legendSpec.seriesPlotType = psl_LineOnly;
    legendSpec.lineType = plt_Solid;
    if (widthSpec == wsLine)
        legendSpec.lineThickness = widthLineThk;
    else
        legendSpec.lineThickness = 6;
}


void PSC_Histogram::SetDefault()
{
    orientationX    = true;
    baselineIsValue = false;
    baselineValue   = 0.0;
    positionIsValue = true;
    widthSpec       = wsPctAvail;
    widthLineThk    = 2;
    widthSpecValue  = 1.0;
    widthSpecPixel  = 6;
    widthSpecPct    = 50.0;
    indexOffset     = 0.0;
}


