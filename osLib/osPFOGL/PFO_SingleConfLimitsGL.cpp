///////////////////////////////////////////////////////////////////////////////////
//
// PFO_SingleConfLimitsGL.cpp
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
//      Implements code to draw osPFO functional objects on a plot.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClass/U_Real.h>

#include <genPFOGL/GL_Line.h>
#include <genPFOGL/GL_Symbol.h>

#include <genPlotClassPS/PC_ReportPS.h>

#include <osPFOGL/PFO_SingleConfLimitsGL.h>


PFO_SingleConfLimitsGL :: PFO_SingleConfLimitsGL(PlotDefC& assPlt) :
                            PFO_SingleConfLimits(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_SingleConfLimitsGL::PFO_SingleConfLimitsGL(const PFO_SingleConfLimitsGL& a, PlotDefC& assPlt) :
                            PFO_SingleConfLimits(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}


PFO_SingleConfLimitsGL::~PFO_SingleConfLimitsGL()
{
}

PFO_SingleConfLimitsGL& PFO_SingleConfLimitsGL::operator= (const PFO_SingleConfLimitsGL& a)
{
    if (&a != this)
        PFO_SingleConfLimits::FullCopy(a);
    return *this;
}

void PFO_SingleConfLimitsGL::InitCommon(PlotDefC& assPlt)
{
    cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
    reportOK     = true;
}

void  PFO_SingleConfLimitsGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    cursorTrackData.Init();
    SC_DoubleArray xReport(maxCovar);
    SC_DoubleArray yReport(maxCovar);
    SC_IntArray dataIndexes(maxCovar);
    int indxStart = 0;

    GL_Line currLine;
    currLine.SetColor(covarPen);
    currLine.SetLine(covarLineThk);

    GL_Symbol currSym;
    currSym.SetColor(covarPen);
    currSym.SetSymProperties(bestEstSymbol);

    SC_IntArray symbolColors;
    if (plotWithColorMap)
        rangeLimit.MapDataToRange(colorData, symbolColors, clipToRange, colorMap->ncolorMap);

    /*double dxPen = 0;
    if (plotWithColorMap)
    {
        int maxPen = colorMap->ncolorMap - 1;
        if (maxCovar > 1)
            dxPen = maxPen / (maxCovar - 1.0);
    }*/

    //calculate offset
    double offset = 0.0;
    switch (widthSpec)
    {
        case wsSpecVal : {
            offset = widthSpecValue;
            break;
        }
        case wsSpecPixel : {
            Coord3D perPixelScale = objectBase->GetIndependentScale(1);
            offset = widthSpecPixel * perPixelScale.cX;
            break;
        }
        case wsPctAvail : {
            offset = widthSpecPct / 100.0;
            break;
        }
    }

    if (offset > 1.0)
        offset = 1.0;

    double dx = 0.0;
    SC_DoubleArray xPoints(2);
    SC_DoubleArray yPoints(2);
    SC_PointArray  bestPoints(1);

    for (int i = 0; i < maxParam; i++)
    {
        DC_XYData& currXY = covarXYArrayDC[i];

        //double nextPen = 0;   //reset color map

        double indx = i;
        if (!allParameters)
            indx = xDataIndx;

        //calculate dx only if multiple
        if (currXY.Size() > 2)
        {
            dx = offset / (currXY.Size() / 2.0 - 1.0) ;
            indx -= (offset / 2.0);
        }

        for (int k = 0; k < currXY.Size(); k = k + 2)
        {
            currXY.xData[k] = indx + (k / 2.0 * dx);
            currXY.xData[k + 1] = indx + (k / 2.0 * dx);
            if (plotConfLimits)
            {
                xPoints.SetEmpty();
                yPoints.SetEmpty();
                xPoints += currXY.xData[k];
                xPoints += currXY.xData[k + 1];
                yPoints += currXY.yData[k];
                yPoints += currXY.yData[k + 1];

                if (plotWithColorMap)
                {
                    if (symbolColors[k / 2] >= 0)
                    {
                        currLine.SetColor(colorMap->GetColor(symbolColors[k / 2]));
                        currLine.DrawLines(xPoints, yPoints);
                    }

                }
                else
                    currLine.DrawLines(xPoints, yPoints);
            }
        }

        //plot best estimate and cursor reporting stuff
        Point2D& point = bestPoints[0];

        for (int j = 0; j < maxCovar; j++)
        {
            DC_Covar& curr = (*covarArrayDC)[j];
            if (allParameters)
                point = Point2D(currXY.xData[2 * j], curr.covarFitVals[i]);
            else
                point = Point2D(currXY.xData[2 * j], curr.covarFitVals[xDataIndx]);

            if (cursorReport->showReport)
            {
                xReport[j] = point.pX;
                yReport[j] = point.pY;
                dataIndexes[j] = indxStart++;
            }

            if (plotBestEstimate)
            {
                if (plotWithColorMap)
                {
                    if (symbolColors[j] >= 0)
                    {
                        currSym.SetColor(colorMap->GetColor(symbolColors[j]));
                        currSym.DrawSymbols(bestPoints, bestEstSymbol.symbolType);
                    }
                }
                else
                    currSym.DrawSymbols(bestPoints, bestEstSymbol.symbolType);
            }
        }  //end maxCovar loop

        if (cursorReport->showReport)
            cursorTrackData.AddSeriesDataToTrack(xReport, yReport, dataIndexes);
    }

    CloseDrawObject();
}

void PFO_SingleConfLimitsGL::ReportCursorData(const Coord3D& cursorPos)
{
    if ((!doPlot) || (!cursorReport->showReport) || StatusNotOK())
    {
        //  unmanage if set up
        cursorReport->ReportTitleSetup(GetID());
        return;
    }

    if (!cursorReport->reportInitialized)
    {
        cursorReport->ReportInit("Label:");
        cursorReport->ReportTitleSetup(GetID());
        cursorReport->ReportLabelSetup(0, "Par:");
        if (colorMapIsIndex)
            cursorReport->ReportLabelSetup(1, "Best:");
        else
            cursorReport->ReportLabelSetup(1, "SSE:");
    }

    int dataIndx;
    if (!cursorTrackData.GetClosestIndex(cursorPos.cX, cursorPos.cY, dataIndx))
        return;

    int paramIndx = dataIndx / maxCovar;
    int fitIndx = dataIndx - (paramIndx * maxCovar);

    if (!allParameters)
        paramIndx = xDataIndx;

    cursorReport->ReportUpdate(0, (*covarHeaderDC)[paramIndx].shortVarDesc);

    double value = 0.0;
    if (colorMapIsIndex)
        value = (*covarArrayDC)[fitIndx].covarFitVals[paramIndx];
    else
        value = (*covarArrayDC)[fitIndx].sumSquaredError;

    char outStr[80];
    SC_RealFormat conv;

    conv.RealToString(value, outStr, 80);
    cursorReport->ReportUpdate(1, outStr);

}

