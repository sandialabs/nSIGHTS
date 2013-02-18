///////////////////////////////////////////////////////////////////////////////////
//
// OGL_2DXY.cpp
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
//      adds coordinate transforms for log axes.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <math.h>

#include <genPlotClass/PD_2DXY.h>

#include <genPlotClassPS/OGL_2DXY.h>

OGL2DXY::OGL2DXY(PD_2DXY& inPlotDef) :  OGL2DBase(inPlotDef), plot2DXYDef(inPlotDef)
{
}


void OGL2DXY::InitView()
{
    SetCurrentView();

    doLogX = (!plot2DXYDef.xAxis.axesAreLinear);
    doLogY = (!plot2DXYDef.yAxis.axesAreLinear);

    //  set spans
    Coord3D minL = currView.viewLimits.minLim;
    Coord3D maxL = currView.viewLimits.maxLim;

    xaxisLinearOffset = minL.cX;
    yaxisLinearOffset = minL.cY;

    TransformForAxes(minL);
    TransformForAxes(maxL);

    int width, height;
    plotBase.CalcAvailablePixels(width, height);
    xPerPixel = (maxL.cX - minL.cX) / double(width);
    yPerPixel = (maxL.cY - minL.cY) / double(height);

    double minX = minL.cX - xPerPixel * double(plotBase.leftMargin);
    double maxX = maxL.cX + xPerPixel * double(plotBase.plotUwidth - plotBase.leftMargin - width);

    double minY = minL.cY - yPerPixel * double(plotBase.bottomMargin);
    double maxY = maxL.cY + yPerPixel * double(plotBase.plotVheight - plotBase.bottomMargin - height);

    ClearClipPlanes();

    //  change projection to XY for annotation
    glMatrixMode(GL_PROJECTION);        //  set projection  mode
    glLoadIdentity();                   //  clear existing

    glOrtho(minX, maxX, minY, maxY, zannoMin, zannoMax);

    glViewport(plotBase.plotUOffset, plotBase.plotVOffset, plotBase.plotUwidth, plotBase.plotVheight);

    //set scissors for clear
    ScissorSetup();

    // for composite plots call needs to be here
    HardCopySetViewport();

    //  clear modelview
    //  set initial translation/rotation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // and clear viewport
    Clear();
}

bool OGL2DXY::TransformForAxes(Coord3D& inCoord) const
{
    bool transOK = true;
    if (doLogX)
    {
        if (inCoord.cX < logAxesMin)
            transOK = false;
        inCoord.cX = log10(inCoord.cX);
    }
    else
        inCoord.cX -= xaxisLinearOffset;

    if (doLogY)
    {
        if (inCoord.cY < logAxesMin)
            transOK = false;
        inCoord.cY = log10(inCoord.cY);
    }
    else
        inCoord.cY -= yaxisLinearOffset;

    inCoord.cZ = currZval;

    return transOK;
}

bool OGL2DXY::TransformForXY(Point2D& inPoint) const
{
    bool transOK = true;
    if (doLogX)
    {
        if (inPoint.pX < logAxesMin)
            transOK = false;
        inPoint.pX = log10(inPoint.pX);
    }
    else
        inPoint.pX -= xaxisLinearOffset;

    if (doLogY)
    {
        if (inPoint.pY < logAxesMin)
            transOK = false;
        inPoint.pY = log10(inPoint.pY);
    }
    else
        inPoint.pY -= yaxisLinearOffset;

    return transOK;
}

double  OGL2DXY::TransformX(const double& xVal)
{
    if (doLogX)
        return log10(xVal);
    return xVal - xaxisLinearOffset;
}

double OGL2DXY::TransformY(const double& yVal)
{
    if (doLogY)
        return log10(yVal);
    return yVal - yaxisLinearOffset;
}

