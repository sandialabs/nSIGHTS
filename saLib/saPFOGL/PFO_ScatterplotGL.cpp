///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ScatterplotGL.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "SaLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genPlotClassPS/PD_2DXYPS.h>

#include <genPFOGL/GL_Symbol.h>
#include <genPFOGL/GL_Line.h>
#include <genPFOGL/PC_GridLine.h>

#include <saPFOGL/PFO_ScatterplotGL.h>


PFO_ScatterplotGL :: PFO_ScatterplotGL(PlotDefC& assPlt) :
                            PFO_Scatterplot(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_ScatterplotGL::PFO_ScatterplotGL(const PFO_ScatterplotGL& a, PlotDefC& assPlt) :
                            PFO_Scatterplot(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
void  PFO_ScatterplotGL :: InitCommon(PlotDefC& assPlt)
{
}


PFO_ScatterplotGL::~PFO_ScatterplotGL()
{
}

PFO_ScatterplotGL& PFO_ScatterplotGL::operator= (const PFO_ScatterplotGL& a)
{
    if (&a != this)
        PFO_Scatterplot::operator =(a);
    return *this;
}

void PFO_ScatterplotGL::DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    GL_Symbol currSeries;
    const SC_DoubleArray& xData = scatterDataDC->dataTable[plotIVIndx];
    const SC_DoubleArray& yData = scatterDataDC->dataTable[plotDVIndx + nplotIV];
    currSeries.DrawSeriesSpec(scatterSpec, xData, yData);

    PD_2DXYPS*  currXY = dynamic_cast<PD_2DXYPS*>(assocPlot);
    if (currXY == 0)
        GenAppInternalError("PFO_ScatterplotGL::DrawPlotObject");
    const PC_2DPlotAnno& plotAnnotation = currXY->plotAnnotation;
    OGL2DBase& currPlot = static_cast<OGL2DBase&>(*objectBase);
    Limit3D currLimits = GetCurrentViewLimits();

    GL_Base::SetColor(plotAnnotation.gridLineDataPen);
    GL_Line::SetLine(plotAnnotation.majorGridWidth);
    GL_Line::SetLineType(plt_Solid);

    if (plotXGridLines)
    {
        PC_GridLine currGrid(currPlot, true);
        const SC_DoubleArray& gData = xGridLinesDC->dataTable[plotIVIndx];

        for (int i = 0; i < gData.Size(); i++)
        {
            double gVal = gData[i];
            if (RealIsNull(gVal))
                break;

            if (!currGrid.CreateGridLine(gVal, currLimits))
                continue;

            currGrid.AdjustForAxes();
            currGrid.DrawGridLine();
        }
    }

    if (plotYGridLines)
    {
        PC_GridLine currGrid(currPlot, false);
        const SC_DoubleArray& gData = yGridLinesDC->dataTable[plotDVIndx];

        for (int i = 0; i < gData.Size(); i++)
        {
            double gVal = gData[i];
            if (RealIsNull(gVal))
                break;

            if (!currGrid.CreateGridLine(gVal, currLimits))
                continue;

            currGrid.AdjustForAxes();
            currGrid.DrawGridLine();
        }
    }

    if (plotAxesLabels)
    {
        currPlot.ClearClipPlanes();

        currPlot.PrintString(plotAnnotation.labelFont, ivLabel,
                            hta_Center, vta_Top,
                            plotAnnotation.xaxesLabelPos.pX,
                            plotAnnotation.xaxesLabelPos.pY);
        PSC_Font yLabFont(plotAnnotation.labelFont);
        yLabFont.fontRotation = PSC_Font::fr_VertLeft;

        currPlot.PrintString(yLabFont, dvLabel,
                        hta_Center, vta_Bottom,
                        plotAnnotation.yaxesLabelPos.pX,
                        plotAnnotation.yaxesLabelPos.pY);

        currPlot.SetClipPlanes();
    }

    CloseDrawObject();
}

