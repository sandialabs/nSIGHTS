///////////////////////////////////////////////////////////////////////////////////
//
// PD_2DXY.cpp
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
//      derived from PD_2D - defines 2D plot with lin/log axes
//
//      defines basic 2D plot
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genPlotClass/PD_2DXY.h>


PD_2DXY :: PD_2DXY() :  PD_2D("2D-XY")
{
    // for RR
    plotAnnotation.labelFont.fontSize = 14.0;
    plotAnnotation.labelFont.weightMedium = false;
    plotAnnotation.incrementFont.fontSize = 14;
    leftMargin = 100;

    xAxis.axisIncFormat.digitsPrecision = 0;
    yAxis.axisIncFormat.digitsPrecision = 0;

    DoStatusChk();
}

PD_2DXY :: PD_2DXY(const PD_2DXY& a) : PD_2D(a)
{
    LocalCopy(a);
    DoStatusChk();
}

PD_2DXY :: ~PD_2DXY ()
{
}

PD_2DXY& PD_2DXY::operator= (const PD_2DXY& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void  PD_2DXY::FullCopy(const PD_2DXY& a)
{
    PlotDefC::FullCopy(a);
    LocalCopy(a);
}

void  PD_2DXY::LocalCopy(const PD_2DXY& a)
{
    xAxis = a.xAxis;
    yAxis = a.yAxis;
}


void  PD_2DXY::DoStatusChk()
{
    PlotDefC::DoStatusChk();
}

void  PD_2DXY::SetViewFromAxes()
{
    Limit3D initialView;
    xAxis.GetAxisLimits(initialView.minLim.cX, initialView.maxLim.cX);
    yAxis.GetAxisLimits(initialView.minLim.cY, initialView.maxLim.cY);

    initialView.minLim.cZ = nullReal;
    initialView.maxLim.cZ = nullReal;

    // put it on stack
    plotViews.InitStack(initialView);
}

void PD_2DXY::SetAxesFromView()
{
    Limit3D currLimits = GetCurrentViewLimits();
    xAxis.autoScaleAxes = false;
    xAxis.SetAxisLimits(currLimits.minLim.cX, currLimits.maxLim.cX);
    yAxis.autoScaleAxes = false;
    yAxis.SetAxisLimits(currLimits.minLim.cY, currLimits.maxLim.cY);
    setxAxis = xAxis;
    setyAxis = yAxis;
    axesHaveBeenSet = true;
}

void PD_2DXY::RestoreSetAxes()
{
    if (!axesHaveBeenSet)
        return;

    xAxis = setxAxis;
    yAxis = setyAxis;
    SetViewFromAxes();
    ResetView();
}


void PD_2DXY::SetAxesAutoLimits()
{
    if (!(xAxis.autoScaleAxes || yAxis.autoScaleAxes))
        return;

    // reset to defaults
    if (xAxis.autoScaleAxes)
        xAxis.SetAxisLimits(0.0, 10.0);
    if (yAxis.autoScaleAxes)
        yAxis.SetAxisLimits(0.0, 10.0);

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
    }

}


bool PD_2DXY::SetAxesFromExtents()
{
   // reset to defaults
    xAxis.SetAxisLimits(0.0, 10.0);
    yAxis.SetAxisLimits(0.0, 10.0);

    //  world XYZ
    Limit3D plotLimits = CalcPlotLimits();
    if (plotLimits.minLim.CoordIsNull())
        return false;

    if (!RealIsNull(plotLimits.minLim.cX))
    {
        if (fabs(plotLimits.minLim.cX - plotLimits.maxLim.cX) < stdEps)
        {
            plotLimits.minLim.cX -= 1.0;
            plotLimits.maxLim.cX += 1.0;
        }
        xAxis.SetAxisLimits(plotLimits.minLim.cX, plotLimits.maxLim.cX);
    }

    if (!RealIsNull(plotLimits.minLim.cY))
    {
        if (fabs(plotLimits.minLim.cY - plotLimits.maxLim.cY) < stdEps)
        {
            plotLimits.minLim.cY -= 1.0;
            plotLimits.maxLim.cY += 1.0;
        }
        yAxis.SetAxisLimits(plotLimits.minLim.cY, plotLimits.maxLim.cY);
    }
    return true;
}
void PD_2DXY :: CalcOutput(FOcalcType  calcType)
{
    PD_2D::CalcOutput(calcType);

    xFormat = xAxis.cursorReportFormat;
    yFormat = yAxis.cursorReportFormat;

}


void PD_2DXY :: ResetView()
{
    //  get view off top off stack
    Limit3D newView = GetCurrentViewLimits();
    //  bad setup
    if (RealIsNull(newView.minLim.cX))
        xAxis.SetAxisLimits(0.0, 10.0);
    else
        xAxis.SetAxisLimits(newView.minLim.cX, newView.maxLim.cX);

    if (RealIsNull(newView.minLim.cY))
        yAxis.SetAxisLimits(0.0, 10.0);
    else
        yAxis.SetAxisLimits(newView.minLim.cY, newView.maxLim.cY);

    PlotDefC::ResetView();
}

PC_ViewOpRec PD_2DXY::GetPlotViewOps()
{
    PC_ViewOpRec SP2D;
    if ( xAxis.axesAreLinear)
        SP2D.xOp = vo_Lin;
    else
        SP2D.xOp = vo_Log;

    if (yAxis.axesAreLinear)
        SP2D.yOp = vo_Lin;
    else
        SP2D.yOp = vo_Log;

    SP2D.zOp = vo_na;

    return SP2D;
}


//  user to pixel conversion from PD_2D
bool PD_2DXY::UserXYtoPixelXY(const Point2D& userPt,
                              int&     userPixX,
                              int&     userPixY)
{
    if ((!xAxis.InAxisLimits(userPt.pX)) || (!yAxis.InAxisLimits(userPt.pY)))
        return false;

    int xAvail, yAvail;
    CalcAvailablePixels( xAvail, yAvail);

    userPixX = int(xAxis.AxisSpanValue(userPt.pX) * double(xAvail)) + leftMargin;

    userPixY = int((1.0 - yAxis.AxisSpanValue(userPt.pY)) * double(yAvail)) + topMargin;

    return true;
}


bool PD_2DXY::PixelXYtoUserXY(int      userPixX,
                              int      userPixY,
                              Point2D& userPt)
{
    int xAvail, yAvail;
    CalcAvailablePixels( xAvail, yAvail);
    userPixX -= leftMargin;
    userPixY -= topMargin;

    if ((userPixX < 0) || (userPixX > xAvail)||(userPixY < 0) || (userPixY > yAvail))
        return false;

    userPt.pX = xAxis.AxisUserValue(double(userPixX) / double(xAvail));
    userPt.pY = yAxis.AxisUserValue(1.0 - double(userPixY) / double(yAvail));

    return true;
}


void PD_2DXY::GetAxesTypes(bool& xIsLinear,
                         bool& yIsLinear,
                         bool& zIsLinear)
{
    xIsLinear = xAxis.axesAreLinear;
    yIsLinear = yAxis.axesAreLinear;
    zIsLinear = true;
}

void PD_2DXY::SetIncrFont()
{
    PSC_Font temp(xAxis.incrFont);
    xAxis.incrFont = plotAnnotation.incrementFont;
    xAxis.incrFont.fontRotation = temp.fontRotation;

    temp = yAxis.incrFont;
    yAxis.incrFont = plotAnnotation.incrementFont;
    yAxis.incrFont.fontRotation = temp.fontRotation;

}

