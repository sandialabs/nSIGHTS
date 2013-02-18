///////////////////////////////////////////////////////////////////////////////////
//
// PFO_SeriesLegendGL.cpp
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

#include <genPFOGL/GL_Symbol.h>
#include <genPFOGL/GL_Arrow.h>
#include <genPFOGL/GL_Polygon.h>

#include <genPFOGL/PFO_SeriesLegendGL.h>


PFO_SeriesLegendGL :: PFO_SeriesLegendGL(PlotDefC& assPlt) :
                        PFO_SeriesLegend(assPlt),
                        OGLBaseObj(this)
{
}
PFO_SeriesLegendGL::PFO_SeriesLegendGL(const PFO_SeriesLegendGL& a, PlotDefC& assPlt) :
                        PFO_SeriesLegend(a, assPlt),
                        OGLBaseObj(this)
{
}
PFO_SeriesLegendGL::~PFO_SeriesLegendGL()
{
}

PFO_SeriesLegendGL& PFO_SeriesLegendGL::operator= (const PFO_SeriesLegendGL& a)
{
    if (&a != this)
        PFO_SeriesLegend::FullCopy(a);
    return *this;
}


void  PFO_SeriesLegendGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    PC_Rectangle frameLimits;
    frameLimits.ul = ulLocation;
    frameLimits.lr = ulLocation;

    // find max widths & heights
    double maxSymboldX = 0.0;


    SC_DoubleArray nextRowStart, rowHeight;
    nextRowStart.SetResizable(10);
    rowHeight.SetResizable(10);

    double firstAboveTop = 0.0;

    double lineMult = objectBase->GetDefaultLineMultiplier();
    ExtendedLabelData labData, maxLabData, titleLabData;

    double firstStart = 0.0;
    bool first = true;
    if (IsNotNull(legendTitle))
    {
        if (!objectBase->GetExtendedStringSizeInfo(legendFont, legendTitle, lineMult, titleLabData))
            return;

        firstStart = titleLabData.nextYStart;
        frameLimits.lr.pY -= firstStart;
        firstAboveTop = titleLabData.aboveTop;
        first = false;
    }


    for (int i = 0; i < maxSeriesLegend; i++)
        if (legendData[i].doSeries)
        {
            const DO_SeriesLegendData& legendSpec = *(legendData[i].dataSource);

            for ( int j = 0; j < legendSpec.Size(); j++)
            {
                if (!objectBase->GetExtendedStringSizeInfo(legendFont, legendSpec.GetName(j), lineMult, labData))
                    return;

                int legSize;
                if (legendSpec.IsSeries(j))
                    legSize = legendSpec.GetSeries(j).symbolSize;
                else
                    legSize = legendSpec.GetArrow(j).headSize;

                if (first)
                {
                    firstAboveTop = labData.aboveTop;
                    first = false;
                }

                Coord3D symScale = objectBase->GetIndependentScale(legSize);
                if (symScale.cY > labData.height)
                {
                    double dh = symScale.cY - labData.height;
                    labData.height += dh;
                    labData.nextYStart += dh;
                }

                nextRowStart += labData.nextYStart;
                rowHeight += labData.height;

                maxLabData.GetMax(labData);

                if (symScale.cX > maxSymboldX)
                    maxSymboldX = symScale.cX;

                frameLimits.lr.pY -= labData.nextYStart;
            }
        }

    // fix for last
    frameLimits.lr.pY += labData.nextYStart;
    frameLimits.lr.pY -= labData.height;
    if (!(frameLegend || opaqueBackground))
    {
        frameLimits.lr.pY -= labData.belowBase;
        frameLimits.ul.pY += firstAboveTop;
    }

    double dataXstart = ulLocation.pX + maxSymboldX / 2.0;
    double seriesXlen = seriesSize + maxSymboldX;
    double descXstart = dataXstart + seriesXlen;

    if (IsNotNull(legendTitle))
    {
        titleLabData.width -= seriesXlen;
        maxLabData.GetMax(titleLabData);
    }

    frameLimits.lr.pX = descXstart + maxLabData.width;

    if (frameLegend || opaqueBackground)
    {
        double xInc, yInc;
        if (!objectBase->GetFrameIncrement(legendFont, firstAboveTop, labData.belowBase, xInc, yInc))
            return;
        frameLimits.IncreaseBoundingBox(xInc, yInc);
    }

    GL_Polygon  rectObject;
    rectObject.SetColor(legendPen);
    rectObject.DrawRectangle(frameLimits, opaqueBackground, frameLegend, frameThick);

    double labYstart  = ulLocation.pY;
    lineMult = objectBase->GetDefaultLineMultiplier();

    if (IsNotNull(legendTitle))
    {
        objectBase->PrintString(legendFont, legendTitle, lineMult, ulLocation.pX, labYstart);
        labYstart  -= firstStart;
    }

    // set series data
    SC_PointArray seriesPts;
    seriesPts.SetResizable(seriesNpts);

    double dx = 0.0;

    if (seriesNpts > 1)
        dx = seriesSize / (double(seriesNpts - 1));

    GL_Symbol oglSym;
    GL_Arrow oglArrow;

    int nLab = 0;
    for (int i = 0; i < maxSeriesLegend; i++)
        if (legendData[i].doSeries)
        {
            const DO_SeriesLegendData& legendSpec = *(legendData[i].dataSource);
            for (int j = 0; j < legendSpec.Size(); j++)
            {
                double seriesY = labYstart - rowHeight[nLab] / 2.0;

                if (legendSpec.IsSeries(j))
                {
                    const PSC_SeriesSpec& currSeries = legendSpec.GetSeries(j);

                    seriesPts.SetEmpty();
                    if (seriesNpts > 1)
                    {
                        for (int k = 0; k < seriesNpts; k++)
                            seriesPts += Point2D(dataXstart + double(k) * dx, seriesY);
                        oglSym.DrawAnnoSeriesSpec(currSeries, seriesPts);
                    }
                    else
                    {
                        PSC_SeriesSpec tempSeries;

                        // do lines
                        if (currSeries.seriesPlotType != psl_SymbolOnly)
                        {
                            tempSeries = currSeries;
                            tempSeries.seriesPlotType = psl_LineOnly;
                            seriesPts += Point2D(dataXstart, seriesY);
                            seriesPts += Point2D(dataXstart + seriesSize, seriesY);
                            oglSym.DrawAnnoSeriesSpec(tempSeries, seriesPts);
                        }

                        // and symbols if necessary
                        if (currSeries.seriesPlotType != psl_LineOnly)
                        {
                            tempSeries = currSeries;
                            tempSeries.seriesPlotType = psl_SymbolOnly;
                            seriesPts.SetEmpty();
                            seriesPts += Point2D(dataXstart + seriesSize / 2.0, seriesY);
                            oglSym.DrawAnnoSeriesSpec(tempSeries, seriesPts);
                        }
                    }
                }
                else
                {
                    GL_Base::SetColor(legendSpec.GetPen(j));
                    oglArrow.SetArrowProperties(legendSpec.GetArrow(j));
                    Point2D arrEnd(dataXstart, seriesY);
                    Point2D arrSt(dataXstart + seriesSize, seriesY);
                    oglArrow.DrawAnnoArrow(arrSt, arrEnd);
                }

                GL_Base::SetColor(legendPen);
                objectBase->PrintString(legendFont, legendSpec.GetName(j), lineMult, descXstart, labYstart);

                labYstart -= nextRowStart[nLab];
                nLab++;
            }
        }

    GL_Base::GetAnnoPixelXY(frameLimits.ul, rectLimits.ulXpos, rectLimits.ulYpos);
    GL_Base::GetAnnoPixelXY(frameLimits.lr, rectLimits.lrXpos, rectLimits.lrYpos);
    SetRectOffset(frameLimits.ul);

    CloseDrawObject();

}

