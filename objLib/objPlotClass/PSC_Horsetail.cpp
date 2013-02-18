///////////////////////////////////////////////////////////////////////////////////
//
// PSC_Horsetail.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/DO_ColorMap.h>

#include <objPlotClass/PSC_Horsetail.h>


PSC_Horsetail::PSC_Horsetail()
{
    SetDefault();
}

PSC_Horsetail::PSC_Horsetail(const PSC_Horsetail& a)
{
    LocalCopy(a);
}


PSC_Horsetail& PSC_Horsetail::operator= (const PSC_Horsetail& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void PSC_Horsetail::LocalCopy(const PSC_Horsetail& a)
{
    colorMapDataObjRef = a.colorMapDataObjRef;
    plotWithColorMap   = a.plotWithColorMap;
    fixedColorPen      = a.fixedColorPen;
    lineData           = a.lineData;
    legendData         = a.legendData;
}

void  PSC_Horsetail::DoCheck(FuncObjC&  specFO)
{
    seriesLegendData.Clear();
    colorMap  = 0;
    if (plotWithColorMap && specFO.CheckInputFO(colorMapDataObjRef, "Color map"))
        colorMap = static_cast<DO_ColorMap*>(specFO.GetInPortData(colorMapDataObjRef))->colorMapData;

    // set up output
    if (specFO.StatusOK())
    {
        seriesLegendData.Alloc(1);
        lineData.seriesPen = fixedColorPen;
        seriesLegendData.AddSeries(legendData.GetLabel(specFO.GetID()), lineData);
    }
}

void PSC_Horsetail::AddPort(FuncObjC&   specFO)
{
    specFO.AddUseFlagInPort(colorMapDataObjRef,  typeid(DO_ColorMap), plotWithColorMap);
    specFO.AddOutPort(seriesLegendData);
}


void PSC_Horsetail::SetDefault()
{
    plotWithColorMap = true;
    fixedColorPen    = 4;
    lineData         = PSC_SeriesSpec();
    legendData       = PSC_LegendOverride();
}


