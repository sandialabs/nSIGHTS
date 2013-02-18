///////////////////////////////////////////////////////////////////////////////////
//
// GL_PixelCursorTrack.cpp
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
//      Implements renderer dependent code to actually draw genPFO functional
// objects on a plot.  Is platform independent but
// relies on platform dependent code in genPlotClassPS.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Msg.h>

#include <genPlotClass/PD_2D.h>

#include <genPFOGL/GL_PixelCursorTrack.h>


GL_PixelCursorTrack::GL_PixelCursorTrack()
{
    xCursor.SetResizable();
    yCursor.SetResizable();
    cursorIndx.SetResizable();
    xCursor.Alloc(10);
    yCursor.Alloc(10);
    cursorIndx.Alloc(10);
    assoc2DPlot = 0;
}
GL_PixelCursorTrack::~GL_PixelCursorTrack()
{
}

void GL_PixelCursorTrack::Init()
{
    xCursor.SetEmpty();
    yCursor.SetEmpty();
    cursorIndx.SetEmpty();
    assoc2DPlot = dynamic_cast<PD_2D*> (defAssocPlot);
    if (assoc2DPlot == 0)
        GenAppInternalError("GL_PixelCursorTrack::Init");
}

void GL_PixelCursorTrack::AddSeriesDataToTrack(const SC_PointArray&   seriesData,
                                               const SC_IntArray&     indexData)
{
    if (assoc2DPlot == 0)
        GenAppInternalError("GL_PixelCursorTrack::AddSeriesDataToTrack");

    int xPix, yPix;
    for (int i = 0; i < indexData.Size(); i++)
        if (assoc2DPlot->UserXYtoPixelXY(seriesData[i], xPix, yPix))
        {
            xCursor += xPix;
            yCursor += yPix;
            cursorIndx += indexData[i];
        }
}

void GL_PixelCursorTrack::AddSeriesDataToTrack(const SC_DoubleArray&  xData,
                                               const SC_DoubleArray&  yData,
                                               const SC_IntArray&     indexData)
{
    if (assoc2DPlot == 0)
        GenAppInternalError("GL_PixelCursorTrack::AddSeriesDataToTrack(2)");

    int xPix, yPix;
    for ( int i = 0; i < indexData.Size(); i++)
        if (assoc2DPlot->UserXYtoPixelXY(Point2D(xData[i], yData[i]), xPix, yPix))
        {
            xCursor += xPix;
            yCursor += yPix;
            cursorIndx += indexData[i];
        }
}

void GL_PixelCursorTrack::AddPointDataToTrack(const Point2D&   pointData,
                                                    int        indx)
{
    if (assoc2DPlot == 0)
        GenAppInternalError("GL_PixelCursorTrack::AddSeriesDataToTrack(3)");

    int xPix, yPix;
    if (assoc2DPlot->UserXYtoPixelXY(pointData, xPix, yPix))
    {
        xCursor += xPix;
        yCursor += yPix;
        cursorIndx += indx;
    }

}

void GL_PixelCursorTrack::AddPointDataToTrack(const double&    xData,
                                              const double&    yData,
                                                    int        indx)
{
    AddPointDataToTrack(Point2D(xData, yData), indx);
}

bool GL_PixelCursorTrack::GetClosestIndex(const double&    xData,
                                          const double&    yData,
                                                 int&       indx)
{
    if (assoc2DPlot == 0)
        GenAppInternalError("GL_PixelCursorTrack::GetClosestIndex");

    // brute force approach for now

    int xPix, yPix;
    if ((xCursor.IsEmpty()) || (!assoc2DPlot->UserXYtoPixelXY(Point2D(xData, yData), xPix, yPix)))
        return false;

    indx = cursorIndx[0];
    int dx = (xPix - xCursor[0]);
    int dy = (yPix - yCursor[0]);
    int currDist = dx * dx + dy * dy;

    for (int i = 1; i < xCursor.Size(); i++)
    {
        dx = (xPix - xCursor[i]);
        dy = (yPix - yCursor[i]);
        int nextDist = dx * dx + dy * dy;
        if (nextDist < currDist)
        {
            indx = cursorIndx[i];
            currDist = nextDist;
            if (currDist == 0)
                return true;
        }
    }

    return true;
}


