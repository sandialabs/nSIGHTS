///////////////////////////////////////////////////////////////////////////////////
//
// PD_3DXYZ.cpp
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
//      derived from PD_3D - defines 3D plot with lin/log axes
//
//      defines basic 3D plot
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotObj.h>

#include <genPlotClass/PD_3DXYZ.h>

static const char* id = "3D-XYZ";

PD_3DXYZ :: PD_3DXYZ() : PD_3D(id)
{
    axesLabel.labelFont.fontSize = 14.0;
    axesLabel.labelFont.weightMedium = false;

    axesLabel.incrementFont.fontSize = 14;

    xAxis.axisIncFormat.digitsPrecision = 0;
    yAxis.axisIncFormat.digitsPrecision = 0;
    zAxis.axisIncFormat.digitsPrecision = 0;


    xyaxesLengthRatio = 1.0;
    xzaxesLengthRatio = 0.5;
    DoStatusChk();
}

PD_3DXYZ :: PD_3DXYZ(const PD_3DXYZ& a) : PD_3D(a)
{
    LocalCopy(a);
}

PD_3DXYZ :: ~PD_3DXYZ ()
{
}

PD_3DXYZ& PD_3DXYZ::operator= (const PD_3DXYZ& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}

void  PD_3DXYZ::FullCopy(const PD_3DXYZ& a)
{
    PD_3D::FullCopy(a);
    LocalCopy(a);
    PD_3DXYZ::LocalCopy(a);
}

void  PD_3DXYZ::LocalCopy(const PD_3DXYZ& a)
{
    xyaxesLengthRatio = a.xyaxesLengthRatio;
    xzaxesLengthRatio = a.xzaxesLengthRatio;

    xAxis = a.xAxis;
    yAxis = a.yAxis;
    zAxis = a.zAxis;
}

void  PD_3DXYZ::DoStatusChk()
{
    PD_3D::DoStatusChk();
}

void  PD_3DXYZ::SetViewFromAxes()
{
    //  set up initial view using scale/el/and az from current
    PC_View initialView = GetCurrentView();

    // get axes limits
    xAxis.GetAxisLimits(initialView.viewLimits.minLim.cX, initialView.viewLimits.maxLim.cX);
    yAxis.GetAxisLimits(initialView.viewLimits.minLim.cY, initialView.viewLimits.maxLim.cY);
    zAxis.GetAxisLimits(initialView.viewLimits.minLim.cZ, initialView.viewLimits.maxLim.cZ);

    // put it on stack
    plotViews.InitStack(initialView);
}

void PD_3DXYZ::SetAxesFromView()
{
    Limit3D currLimits = GetCurrentViewLimits();
    xAxis.autoScaleAxes = false;
    xAxis.SetAxisLimits(currLimits.minLim.cX, currLimits.maxLim.cX);
    yAxis.autoScaleAxes = false;
    yAxis.SetAxisLimits(currLimits.minLim.cY, currLimits.maxLim.cY);
    zAxis.autoScaleAxes = false;
    zAxis.SetAxisLimits(currLimits.minLim.cZ, currLimits.maxLim.cZ);

    setxAxis = xAxis;
    setyAxis = yAxis;
    setzAxis = zAxis;
    axesHaveBeenSet = true;
}

void PD_3DXYZ::RestoreSetAxes()
{
    if (!axesHaveBeenSet)
        return;

    xAxis = setxAxis;
    yAxis = setyAxis;
    zAxis = setzAxis;
    SetViewFromAxes();
    ResetView();
}


void PD_3DXYZ::SetAxesAutoLimits()
{
    if (!(xAxis.autoScaleAxes || yAxis.autoScaleAxes || zAxis.autoScaleAxes))
        return;

    // reset to defaults
    if (xAxis.autoScaleAxes)
        xAxis.SetAxisLimits(0.0, 10.0);
    if (yAxis.autoScaleAxes)
        yAxis.SetAxisLimits(0.0, 10.0);
    if (zAxis.autoScaleAxes)
        zAxis.SetAxisLimits(0.0, 10.0);

    //  world XYZ
    Limit3D plotLimits = CalcPlotLimits();

    if (! plotLimits.minLim.CoordIsNull())
    {
        if ((!RealIsNull(plotLimits.minLim.cX)) && xAxis.autoScaleAxes)
        {
            xAxis.SetAxisLimits(plotLimits.minLim.cX, plotLimits.maxLim.cX);
            xAxis.AdjustAxisLimits();
        }

        if ( (!RealIsNull(plotLimits.minLim.cY)) && yAxis.autoScaleAxes)
        {
            yAxis.SetAxisLimits(plotLimits.minLim.cY, plotLimits.maxLim.cY);
            yAxis.AdjustAxisLimits();
        }

        if ( (!RealIsNull(plotLimits.minLim.cZ)) && zAxis.autoScaleAxes)
        {
            zAxis.SetAxisLimits(plotLimits.minLim.cZ, plotLimits.maxLim.cZ);
            zAxis.AdjustAxisLimits();
        }
    }
}

bool PD_3DXYZ::SetAxesFromExtents()
{
    // reset to defaults
    xAxis.SetAxisLimits(0.0, 10.0);
    yAxis.SetAxisLimits(0.0, 10.0);
    zAxis.SetAxisLimits(0.0, 10.0);

    //  world XYZ
    Limit3D plotLimits = CalcPlotLimits();

    if (plotLimits.minLim.CoordIsNull())
        return false;

    if (!RealIsNull(plotLimits.minLim.cX))
        xAxis.SetAxisLimits(plotLimits.minLim.cX, plotLimits.maxLim.cX);

    if (!RealIsNull(plotLimits.minLim.cY))
        yAxis.SetAxisLimits(plotLimits.minLim.cY, plotLimits.maxLim.cY);

    if (!RealIsNull(plotLimits.minLim.cZ))
        zAxis.SetAxisLimits(plotLimits.minLim.cZ, plotLimits.maxLim.cZ);

    return true;
}

void PD_3DXYZ :: CalcOutput(FOcalcType  calcType)
{
    PlotDefC::CalcOutput(calcType);
    PD_3DXYZ::DoStatusChk();
    if (objStatus != os_OK)
        return;

    // initial view -- if not auto is OK
    SetViewFromAxes();
}


void PD_3DXYZ :: ResetView()
{
    //  get view off top off stack
    PC_View newView = plotViews.TopStack();

    //  bad setup
    if (RealIsNull(newView.viewLimits.minLim.cX))
        xAxis.SetAxisLimits(0.0, 10.0);
    else
        xAxis.SetAxisLimits(newView.viewLimits.minLim.cX, newView.viewLimits.maxLim.cX);

    if (RealIsNull(newView.viewLimits.minLim.cY))
        yAxis.SetAxisLimits(0.0, 10.0);
    else
        yAxis.SetAxisLimits(newView.viewLimits.minLim.cY, newView.viewLimits.maxLim.cY);

    if (RealIsNull(newView.viewLimits.minLim.cZ))
        zAxis.SetAxisLimits(0.0, 10.0);
    else
        zAxis.SetAxisLimits(newView.viewLimits.minLim.cZ, newView.viewLimits.maxLim.cZ);

    //  draw
    PlotDefC::ResetView();
}

Coord3D PD_3DXYZ::GetNewViewCenter()
{
    PC_View initialView = GetCurrentView();

    //  get ref for clarity
    Coord3D& viewMax = initialView.viewLimits.maxLim;
    Coord3D& viewMin = initialView.viewLimits.minLim;

    // transform for axes
    if (!xAxis.axesAreLinear)
    {
        viewMax.cX = log10(viewMax.cX);
        viewMin.cX = log10(viewMin.cX);
    }
    if (!yAxis.axesAreLinear)
    {
        viewMax.cY = log10(viewMax.cY);
        viewMin.cY = log10(viewMin.cY);
    }
    if (!zAxis.axesAreLinear)
    {
        viewMax.cZ = log10(viewMax.cZ);
        viewMin.cZ = log10(viewMin.cZ);
    }

    //  get current centre coord
    Coord3D centreCoord((viewMax.cX  + viewMin.cX) / 2.0,
                        (viewMax.cY  + viewMin.cY) / 2.0,
                        (viewMax.cZ  + viewMin.cZ) / 2.0);
    //  translate
    centreCoord.cX -= initialView.translation.cX  * (viewMax.cX - viewMin.cX);
    centreCoord.cY -= initialView.translation.cY  * (viewMax.cY - viewMin.cY);
    centreCoord.cZ -= initialView.translation.cZ  * (viewMax.cZ - viewMin.cZ);

    if (!xAxis.axesAreLinear)
        centreCoord.cX = InvLgt(centreCoord.cX);
    if (!yAxis.axesAreLinear)
        centreCoord.cY = InvLgt(centreCoord.cY);
    if (!zAxis.axesAreLinear)
        centreCoord.cZ = InvLgt(centreCoord.cZ);

    return centreCoord;
}


PC_ViewOpRec PD_3DXYZ::GetPlotViewOps()
{
    PC_ViewOpRec SP3D;
    if ( xAxis.axesAreLinear)
        SP3D.xOp = vo_Lin;
    else
        SP3D.xOp = vo_Log;

    if (yAxis.axesAreLinear)
        SP3D.yOp = vo_Lin;
    else
        SP3D.yOp = vo_Log;

    if (zAxis.axesAreLinear)
        SP3D.zOp = vo_Lin;
    else
        SP3D.zOp = vo_Log;

    return SP3D;
}


void PD_3DXYZ::GetAxesTypes(bool& xIsLinear,
                         bool& yIsLinear,
                         bool& zIsLinear)
{
    xIsLinear = xAxis.axesAreLinear;
    yIsLinear = yAxis.axesAreLinear;
    zIsLinear = zAxis.axesAreLinear;
}

