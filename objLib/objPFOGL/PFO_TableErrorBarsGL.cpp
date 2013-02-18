///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableErrorBarsGL.cpp
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
//    Implements code to draw objPFO functional objects on a plot.
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/U_Msg.h>

#include <genPFOGL/GL_Line.h>
#include <genPlotClassPS/PD_2DXYPS.h>

#include <objPFOGL/PFO_TableErrorBarsGL.h>


PFO_TableErrorBarsGL :: PFO_TableErrorBarsGL(PlotDefC& assPlt) :
                            PFO_TableErrorBars(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_TableErrorBarsGL::PFO_TableErrorBarsGL(const PFO_TableErrorBarsGL& a, PlotDefC& assPlt) :
                            PFO_TableErrorBars(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
void  PFO_TableErrorBarsGL :: InitCommon(PlotDefC& assPlt)
{
    reportOK     = false;
}


PFO_TableErrorBarsGL::~PFO_TableErrorBarsGL()
{
}

PFO_TableErrorBarsGL& PFO_TableErrorBarsGL::operator= (const PFO_TableErrorBarsGL& a)
{
    if (&a != this)
        PFO_TableErrorBars::operator=(a);
    return *this;
}

void PFO_TableErrorBarsGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    int nvertex = tableDataDC->GetNRows();
    if ((errorBarFormat == ebfBox) || (errorBarType == ebtXandY))
        nvertex *= 4;
    else
        nvertex *= 2;

    if ((errorBarFormat != ebfBox) && lineHasCrossTick)
        nvertex *= 3;

    GL_Base::StdHardCopyBlockStart(nvertex);
    const SC_DoubleMatrix& errorData = tableDataDC->dataTable;

    GL_Line currLine;
    currLine.SetLineProperties(barLineSpec);

    PD_2DXYPS*  currXY = dynamic_cast<PD_2DXYPS*>(assocPlot);
    if (currXY == 0)
        GenAppInternalError("PFO_TableErrorBarsGL::DrawPlotObject");
    currPlot = static_cast<OGL2DBase*>(objectBase);
    crossTickDelta = currPlot->GetPixelComponents(crossTickPixels);
    tickZ = currPlot->GetZVal();

    double xVal, yVal, xMin, xMax, yMin, yMax;
    for (int i = 0; i < tableDataDC->GetNRows(); i++)
    {
        if (errorBarFormat == ebfBox)
        {
            if (errorBarData == ebtMinMax)
            {
                xMin = errorData[xminIndx][i];
                yMin = errorData[yminIndx][i];

                xMax = errorData[xmaxIndx][i];
                yMax = errorData[ymaxIndx][i];
            }
            else
            {
                xVal = errorData[xdataIndx][i];
                if (RealIsNull(yVal))
                    continue;
                yVal = errorData[ydataIndx][i];
                if (RealIsNull(yVal))
                    continue;

                double dx = errorData[dxIndx ][i] / 2.0;
                double dy = errorData[dyIndx ][i] / 2.0;
                xMin = xVal - dx;
                xMax = xVal + dx;
                yMin = yVal - dy;
                yMax = yVal + dy;
            }
            currLine.DrawLine(Point2D(xMin, yMin), Point2D(xMin, yMax));
            currLine.DrawLine(Point2D(xMin, yMax), Point2D(xMax, yMax));
            currLine.DrawLine(Point2D(xMax, yMax), Point2D(xMax, yMin));
            currLine.DrawLine(Point2D(xMax, yMin), Point2D(xMin, yMin));
        }
        else
        {
            if (errorBarType != ebtXOnly)
            {
                xVal = errorData[xdataIndx][i];
                if (errorBarData == ebtMinMax)
                {
                    yMin = errorData[yminIndx][i];
                    yMax = errorData[ymaxIndx][i];
                }
                else
                {
                    yVal = errorData[ydataIndx][i];
                    if (RealIsNull(yVal))
                        continue;
                    double dy = errorData[dyIndx ][i] / 2.0;
                    yMin = yVal - dy;
                    yMax = yVal + dy;
                }
                Point2D bottom(xVal, yMin);
                Point2D top(xVal, yMax);
                currLine.DrawLine(bottom, top);

                if (lineHasCrossTick)
                {
                    XCrossTick(top);
                    XCrossTick(bottom);
                }

            }

            if (errorBarType != ebtYOnly)
            {
                yVal = errorData[ydataIndx][i];
                if (errorBarData == ebtMinMax)
                {
                    xMin = errorData[xminIndx][i];
                    xMax = errorData[xmaxIndx][i];
                }
                else
                {
                    xVal = errorData[xdataIndx][i];
                    if (RealIsNull(xVal))
                        continue;
                    double dx = errorData[dxIndx ][i] / 2.0;
                    xMin = xVal - dx;
                    xMax = xVal + dx;
                }
                currLine.DrawLine(Point2D(xMin, yVal), Point2D(xMax, yVal));

                Point2D left(xMin, yVal);
                Point2D right(xMax, yVal);
                currLine.DrawLine(left, right);

                if (lineHasCrossTick)
                {
                    YCrossTick(left);
                    YCrossTick(right);
                }
            }
        }
    }

    GL_Base::HardCopyBlockEnd();

    CloseDrawObject();
}


bool PFO_TableErrorBarsGL::OKforCrossTick(Point2D& inPt)
{
    if (RealIsNull(inPt.pX) || RealIsNull(inPt.pX))
        return false;

    return currPlot->TransformForXY(inPt);
}

void PFO_TableErrorBarsGL::XCrossTick(Point2D& inPt)
{
    if (OKforCrossTick(inPt))
    {
        glBegin(GL_LINES);
            glVertex3d(GLdouble(inPt.pX - crossTickDelta.pX), GLdouble(inPt.pY), GLdouble(tickZ));
            glVertex3d(GLdouble(inPt.pX + crossTickDelta.pX), GLdouble(inPt.pY), GLdouble(tickZ));
        glEnd();
    }
}

void PFO_TableErrorBarsGL::YCrossTick(Point2D& inPt)
{
    if (OKforCrossTick(inPt))
    {
        glBegin(GL_LINES);
            glVertex3d(GLdouble(inPt.pX), GLdouble(inPt.pY - crossTickDelta.pY), GLdouble(tickZ));
            glVertex3d(GLdouble(inPt.pX), GLdouble(inPt.pY + crossTickDelta.pY), GLdouble(tickZ));
        glEnd();
    }
}

