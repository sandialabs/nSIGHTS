///////////////////////////////////////////////////////////////////////////////////
//
// GL_Histogram.cpp
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
#include <genClass/U_Real.h>

#include <genPFOGL/GL_Line.h>

#include <genPFOGL/GL_Histogram.h>


void  GL_Histogram::SetHistogramProperties(const PSC_Histogram&     inSpec,
                                           const SC_DoubleArray&    histoX,
                                                 int                nhisto,
                                                 int                plLayer)
{
    if (defAssocGL == 0)
        GenAppInternalError("GL_Histogram::SetHistogramProperties");

    bool xLin, yLin;
    defAssocPlot->GetAxesTypes(xLin, yLin);

    bool isLogAxis = !xLin;
    if (orientationX)
        isLogAxis = !yLin;

    PSC_Histogram::operator=(inSpec);
    nhistoToPlot = nhisto;
    plotLayer = plLayer;

    if (widthSpec == wsLine)
    {
        GL_Line::SetLine(widthLineThk);
    }
    else
    {
        GL_Line::SetLine(1);
        rectObject.SetPolygonFilled(0.0);

        switch (widthSpec) {
            case wsSpecVal : {
                polygondX = widthSpecValue;
                break;
             }
            case wsSpecPixel : {
                Coord3D perPixelScale = defAssocGL->GetIndependentScale(1);
                polygondX = widthSpecPixel;
                if (orientationX)
                    polygondX *= perPixelScale.cX;
                else
                    polygondX *= perPixelScale.cY;
                break;
             }
            case wsPctAvail : {
                polygondX = 1.0;
                if (positionIsValue)
                {
                    double avgDx = 0.0;
                    int nAvg = 0;
                    for (int i = 0; i < histoX.UpperBound(); i++)
                    {
                        double x1 = histoX[i];
                        double x2 = histoX[i + 1];

                        if (RealIsNull(x1) || RealIsNull(x2))
                            continue;

                        if (isLogAxis)
                        {
                            if ((!IsPositive(x1)) ||(!IsPositive(x2)))
                                continue;
                            x1 = Lgt(x1);
                            x2 = Lgt(x2);
                        }

                        avgDx += fabs(x2 - x1);
                        nAvg++;
                    }

                    if (nAvg > 0)
                        polygondX = avgDx / double(nAvg);
                }
                polygondX *= widthSpecPct / 100.0;
                break;
            }
        }
        polygondX /= 2.0;
    }

    if (!baselineIsValue)
    {
        if (orientationX)
            baselineValue = defAssocGL->GetCurrentView().viewLimits.minLim.cY;
        else
            baselineValue = defAssocGL->GetCurrentView().viewLimits.minLim.cX;
    }

}


void GL_Histogram::Draw3DHistogram(const Coord3D& endCoord)
{
}

void GL_Histogram::Draw2DHistogram(const Point2D& endPoint,
                                         int      histoPen)
{
    if (assoc2DGL == 0)
        GenAppInternalError("GL_Histogram::Draw2DHistogram");

    bool hardCopy = GL_Base::defAssocGL->DoingHardCopy();

    Coord3D stHist(endPoint, 0.0), endHist(endPoint, 0.0);
    if (orientationX)
        stHist.cY = baselineValue;
    else
        stHist.cX = baselineValue;

    if (widthSpec == wsLine)
    {
        GL_Line::SetColor(histoPen);
        DrawLine(stHist, endHist);
        return;
    }

    if ((!defAssocGL->TransformForAxes(stHist)) ||
        (!defAssocGL->TransformForAxes(endHist)))
            return;

    SC_CoordArray rectData(4);
    double zVal = assoc2DGL->GetZVal();

    rectData[0] = stHist;
    rectData[1] = stHist;
    rectData[2] = endHist;
    rectData[3] = endHist;

    if (orientationX)
    {
        rectData[0].cX -= polygondX;
        rectData[1].cX += polygondX;
        rectData[2].cX += polygondX;
        rectData[3].cX -= polygondX;
    }
    else
    {
        rectData[0].cY -= polygondX;
        rectData[1].cY += polygondX;
        rectData[2].cY += polygondX;
        rectData[3].cY -= polygondX;
    }

#ifdef _GLPRHARDCOPY_
    // draw poly
    if (plotEdges && hardCopy)
    {
        GL_Polygon::SetColor(edgePen);
        glpBeginGroup(GLP_OVERLAY_GROUP);
    }
#endif

    // draw filled block
    rectObject.SetColor(histoPen);
    rectObject.SetPolygonFilled(0.0);

    glBegin(GL_POLYGON);
        for(int i = 0; i <  4; i++)
        {
            glVertex3d(GLdouble(rectData[i].cX), GLdouble(rectData[i].cY), GLdouble(zVal));
        }
    glEnd();


#ifdef _GLPRHARDCOPY_
    if (hardCopy)
    {
        if (plotEdges)
            glpEndGroup();
        return;
    }
#endif
        // draw edge
    if (plotEdges)
    {
        GL_Base::SetDataLayerOffset(plotLayer);
        rectObject.SetColor(edgePen);
        rectObject.SetPolygonLine(edgeThk, edgePolyOffset);

        glBegin(GL_POLYGON);
            for(int i = 0; i <  4; i++)
            {
                glVertex3d(GLdouble(rectData[i].cX), GLdouble(rectData[i].cY), GLdouble(zVal));
            }
        glEnd();
    }


}


