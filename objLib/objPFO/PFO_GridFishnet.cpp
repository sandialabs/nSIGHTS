///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridFishnet.cpp
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
//    Plots grid data as a fishnet on 3D plots.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objClass/DO_GridData.h>

#include <genPlotClass/C_PlotDef.h>
#include <genPlotClass/PSC_SeriesSpec.h>

#include <objPFO/PFO_GridFishnet.h>


PFO_GridFishnet :: PFO_GridFishnet(PlotDefC& assPlt)
    : PlotObjC("Grid Fishnet", assPlt)
{
    plotAsTubes = false;
    gridPen     = 3;
    gridLineThk = 1;
    gridLineType = plt_Solid;

    tubeData.adjustForScaling = true;

    xplotModulus = 1;
    yplotModulus = 1;
    plotLastX    = true;
    plotLastY    = true;

    InitCommon();
}

PFO_GridFishnet::PFO_GridFishnet(const PFO_GridFishnet& a, PlotDefC& assPlt) : PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_GridFishnet :: ~PFO_GridFishnet ()
{
}

void PFO_GridFishnet::InitCommon()
{
    reportOK        = false;                // no reporting
    usePolyOffset   = is3DObject;

    //  base class
    AddInPort(gridDataObjRef,  typeid(DO_GridData));
    AddOutPort(seriesLegendData);

    DoStatusChk();
}

PFO_GridFishnet& PFO_GridFishnet::operator= (const PFO_GridFishnet& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}



void  PFO_GridFishnet::LocalCopy(const PFO_GridFishnet& a)
{
    gridDataObjRef   = a.gridDataObjRef;
    plotAsTubes      = a.plotAsTubes   ;
    gridPen          = a.gridPen       ;
    gridLineThk      = a.gridLineThk   ;
    gridLineType     = a.gridLineType;
    tubeData         = a.tubeData      ;
    legendOverride   = a.legendOverride;
    coordMap         = a.coordMap;
    xplotModulus     = a.xplotModulus;
    yplotModulus     = a.yplotModulus;
    plotLastX        = a.plotLastX;
    plotLastY        = a.plotLastY;
}


void  PFO_GridFishnet:: DoStatusChk()
{
    // base class check
    PlotObjC::DoStatusChk();

    if (!coordMap.SetupOK())
        SetObjErrMsg("one or more coord maps map to same");

    if (!CheckInputFO(gridDataObjRef, "Covariance Data"))
        return;

    gridDataDC = static_cast<DO_GridData*>(GetInPortData(gridDataObjRef))->gridData;

    seriesLegendData.Alloc(1);

    PSC_SeriesSpec temp;
    temp.seriesPlotType = psl_LineOnly;
    temp.seriesPen = gridPen;
    temp.lineThickness = gridLineThk;
    if (plotAsTubes)
        temp.lineThickness = 10;

    seriesLegendData.AddSeries(legendOverride.GetLabel("Grid Fishnet"), temp);
}


void PFO_GridFishnet:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

bool PFO_GridFishnet::ContainsLinesOnly()
{
    return !plotAsTubes;
}


Limit3D PFO_GridFishnet::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    Coord3D temp;
    for ( int i = 0; i < gridDataDC->Size(); i++)
    {
        temp = gridDataDC->GetCoord(i);
        coordMap.MapCoords(temp);
        currLimits.AddToLimits(temp);
    }


    return currLimits;
}

