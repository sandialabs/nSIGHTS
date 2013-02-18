///////////////////////////////////////////////////////////////////////////////////
//
// PFO_3DXYData.cpp
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
//    Plots data from one or more XY data sets using symbols and/or lines on
// selected plane/value on a 3D XYZ plot
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <objPFO/PFO_3DXYData.h>


PFO_3DXYData :: PFO_3DXYData(PlotDefC& assPlt)
    : PlotObjC("XY Data Series", assPlt),
      zValue("XY ZVal")
{
    SetDefault();
    InitCommon();
}

PFO_3DXYData::PFO_3DXYData(const PFO_3DXYData& a, PlotDefC& assPlt) : PlotObjC(a, assPlt), zValue(a.zValue)
{
    LocalCopy(a);
    InitCommon();
}

PFO_3DXYData :: ~PFO_3DXYData ()
{
}

void PFO_3DXYData::InitCommon()
{
    reportOK        = false;
    usePolyOffset   = true;

    //  base class
    AddInPort(xyDataObjRef,  typeid(DO_XYData));
    // no out port for Z
    AddMSInPort(zValue.valueMasterObjRef, typeid(DO_Real), zValue.valueIsMaster);
    AddOutPort(seriesLegendData);

    DoStatusChk();
}

PFO_3DXYData& PFO_3DXYData::operator= (const PFO_3DXYData& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void  PFO_3DXYData::FullCopy(const PFO_3DXYData& a)
{
    PlotObjC::FullCopy(a);
    LocalCopy(a);
}

void  PFO_3DXYData::LocalCopy(const PFO_3DXYData& a)
{
    xyDataObjRef            = a.xyDataObjRef;
    xySeries                = a.xySeries;
    zValue                  = a.zValue;
    coordMap                = a.coordMap;
    seriesLabel             = a.seriesLabel;
}


void PFO_3DXYData::SetDefault()
{
    //  properties
    xySeries                = PSC_3DSeriesSpec();
    seriesLabel             = PSC_LegendOverride();
}

void  PFO_3DXYData:: DoStatusChk()
{
    xyDataDO    = DO_XYData();

    // base class check
    PlotObjC::DoStatusChk();

    if (CheckInputFO(xyDataObjRef, "Input XY"))
        xyDataDO = *(static_cast<DO_XYData*>(GetInPortData(xyDataObjRef)));

    zValue.DoCheck("Z value", *this);
    if (StatusNotOK())
        return;

    // set up output
    seriesLegendData.Alloc(1);
    seriesLegendData.AddSeries(seriesLabel.GetLabel(xyDataDO.xyData->dataID), xySeries);
}


void PFO_3DXYData:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

bool PFO_3DXYData::ContainsLinesOnly()
{
    return !xySeries.IsSolid();
}

Limit3D PFO_3DXYData::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if ((StatusNotOK()) || (! doPlot))
        return currLimits;

    bool xLin, yLin, zLin;
    GetAxesTypes(xLin, yLin, zLin);

    double zVal = zValue.GetCurrentValue();
    const DC_XYData& xyData = *xyDataDO.xyData;

    for (int i = 0; i < xyData.Size(); i++)
    {
        Coord3D addCoord(xyData.xData[i], xyData.yData[i], zVal);
        coordMap.MapCoords(addCoord);

        if ((xLin || (addCoord.cX > stdEps)) && (yLin || (addCoord.cY > stdEps)) && (zLin || (addCoord.cZ > stdEps)))
            currLimits.AddToLimits(addCoord);
    }

    return currLimits;
}

