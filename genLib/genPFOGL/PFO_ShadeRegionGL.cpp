///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ShadeRegionGL.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPFOGL/GL_Line.h>
#include <genPFOGL/GL_StippleSpec.h>
#include <genPFOGL/GL_Polygon.h>
#include <genPFOGL/PC_Label.h>

#include <genPFOGL/PFO_ShadeRegionGL.h>


PFO_ShadeRegionGL :: PFO_ShadeRegionGL(PlotDefC& assPlt) :
                            PFO_ShadeRegion(assPlt),
                            OGLBaseObj(this)
{
}
PFO_ShadeRegionGL::PFO_ShadeRegionGL(const PFO_ShadeRegionGL& a, PlotDefC& assPlt) :
                            PFO_ShadeRegion(a, assPlt),
                            OGLBaseObj(this)
{
}


PFO_ShadeRegionGL::~PFO_ShadeRegionGL()
{
}

PFO_ShadeRegionGL& PFO_ShadeRegionGL::operator= (const PFO_ShadeRegionGL& a)
{
    if (&a != this)
        PFO_ShadeRegion::operator=(a);

    return *this;
}


void  PFO_ShadeRegionGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    Limit3D plotLimits = GetCurrentViewLimits();
    OGL2DBase& currPlot = static_cast<OGL2DBase&>(*objectBase);

    double xrectMin = plotLimits.minLim.cX;
    double xrectMax = plotLimits.maxLim.cX;
    switch (xRegionSpec) {
        case rst_None : {
            break;
        }
        case rst_lessThanMin : {
            xrectMax = regionXMinVal;
            break;
        }
        case rst_betweenMinAndMax : {
            xrectMin = regionXMinVal;
            xrectMax = regionXMaxVal;
            break;
        }
        case rst_greaterThanMax : {
            xrectMin = regionXMaxVal;
            break;
        }
    }

    double yrectMin = plotLimits.minLim.cY;
    double yrectMax = plotLimits.maxLim.cY;
    switch (yRegionSpec) {
        case rst_None : {
            break;
        }
        case rst_lessThanMin : {
            yrectMax = regionYMinVal;
            break;
        }
        case rst_betweenMinAndMax : {
            yrectMin = regionYMinVal;
            yrectMax = regionYMaxVal;
            break;
        }
        case rst_greaterThanMax : {
            yrectMin = regionYMaxVal;
            break;
        }
    }

    Point2D rectLL(xrectMin, yrectMin);
    Point2D rectUR(xrectMax, yrectMax);

    if (!(currPlot.TransformForXY(rectLL) && currPlot.TransformForXY(rectUR)))
        return;

    if (shadeRegion)
    {
        //  draw color rectangles
        // all filled
        GL_Polygon  rectObject;
        rectObject.SetPolygonFilled(0.0);

        if (shadeIsColorMap)
        {
            int index = IMin(shadeCMIndex, colorMap->ncolorMap - 1);
            GL_Base::SetColor(colorMap->GetColor(index));
        }
        else
            GL_Base::SetColor(shadePen);

        currPlot.HardCopyBlockStart(4);

        SC_PointArray rectVertexes(4);
        rectVertexes.SetEmpty();
        rectVertexes += Point2D(xrectMin, yrectMin);
        rectVertexes += Point2D(xrectMax, yrectMin);
        rectVertexes += Point2D(xrectMax, yrectMax);
        rectVertexes += Point2D(xrectMin, yrectMax);

        rectObject.DrawPolygon(rectVertexes);
        currPlot.HardCopyBlockEnd();
    }


    if (stippleRegion)
    {
        if (shadeRegion)
            currPlot.SetDataLayerOffset(plotLayer);
        GL_StippleSpec stipple;
        stipple.SetProperties(regionStipple);

#ifdef _GLPRHARDCOPY_
        if (DoingHardCopy())
        {
            glpBegin(GLP_3D_COLOR, vertexCount);
            objectBase->SetHardCopyParameters();
        }
#endif

        vertexCount = 0;
        stipple.DrawStipple(Point2D(xrectMin, yrectMin), Point2D(xrectMax, yrectMax), vertexCount);

        GL_Base::HardCopyBlockEnd();
    }

    if (frameRegion)
    {
        if (stippleRegion || shadeRegion)
            currPlot.SetDataLayerOffset(plotLayer);

        GL_Line frame;
        frame.SetLineProperties(frameLine);

        SC_PointArray rectVertexes(5);
        rectVertexes.SetEmpty();
        rectVertexes += Point2D(xrectMin, yrectMin);
        rectVertexes += Point2D(xrectMax, yrectMin);
        rectVertexes += Point2D(xrectMax, yrectMax);
        rectVertexes += Point2D(xrectMin, yrectMax);
        rectVertexes += Point2D(xrectMin, yrectMin);

        frame.SetLineMitreing(frameLine.mitreLines);
        frame.DrawLines(rectVertexes);
    }


    if (labelRegion && IsNotNull(regionLabel))
    {
        currPlot.SetDataLayerOffset(plotLayer);
        GL_Base::SetColor(labelPen);

        PC_Label currLab(currPlot);

        Line2D xLine(rectLL, Point2D(rectUR.pX, rectLL.pY));
        currLab.labelPos.pX = xLine.PointOnLine(labelXPosition / 100.0).pX;

        Line2D yLine(rectLL, Point2D(rectLL.pX, rectUR.pY));
        currLab.labelPos.pY = yLine.PointOnLine(labelYPosition / 100.0).pY;

        currLab.OffsetLabelPos(labelFont, labelOffset, halign, valign);

        currLab.PrintClippedLabel(labelFont, regionLabel, halign, valign);
    }




    CloseDrawObject();
}

