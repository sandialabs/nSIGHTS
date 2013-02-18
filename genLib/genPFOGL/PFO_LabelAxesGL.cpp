///////////////////////////////////////////////////////////////////////////////////
//
// PFO_LabelAxesGL.cpp
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
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genPlotClassPS/PC_ReportPS.h>
#include <genPlotClassPS/PD_2DXYPS.h>

#include <genPFOGL/GL_Line.h>

#include <genPFOGL/PFO_LabelAxesGL.h>


PFO_LabelAxesGL :: PFO_LabelAxesGL(PlotDefC& assPlt) :
                            PFO_LabelAxes(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}

PFO_LabelAxesGL::PFO_LabelAxesGL(const PFO_LabelAxesGL& a, PlotDefC& assPlt) :
                            PFO_LabelAxes(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}

void PFO_LabelAxesGL::InitCommon(PlotDefC& assPlt)
{
    if (!is3DObject)
    {
        cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
        reportOK     = true;
    }
}


PFO_LabelAxesGL::~PFO_LabelAxesGL()
{
}

PFO_LabelAxesGL& PFO_LabelAxesGL::operator= (const PFO_LabelAxesGL& a)
{
    if (&a != this)
        PFO_LabelAxes::operator=(a);

    return *this;
}



void PFO_LabelAxesGL::DrawPlotObject()
{
    if (!ClipToLimits())
        return;

    if (!InitDrawObject())
        return;


    PD_2DXYPS*  currXY = dynamic_cast<PD_2DXYPS*>(assocPlot);
    if (currXY == 0)
        GenAppInternalError("PFO_LabelAxesGL::DrawPlotObject");


    currXY->SetIncrFont();
    const PSC_Font& xIncrFont = currXY->xAxis.incrFont;
    const PSC_Font& yIncrFont = currXY->yAxis.incrFont;

    const PC_2DPlotAnno& plotAnnotation = currXY->plotAnnotation;

    OGL2DBase& currPlot = static_cast<OGL2DBase&>(*objectBase);

    currPlot.ClearClipPlanes();

    Point2D axesOffComp, labelOffComp;
    bool dummy;
    currPlot.GetAxesOffsets(axesOffComp, labelOffComp, dummy);

    Point2D majorTicLength = currPlot.GetPixelComponents(plotAnnotation.majorTicLength);
    Point2D minorTicLength = currPlot.GetPixelComponents(plotAnnotation.minorTicLength);
    Point2D majorTicOffset = currPlot.GetPixelComponents(plotAnnotation.majorTicOffset);
    Point2D minorTicOffset = currPlot.GetPixelComponents(plotAnnotation.minorTicOffset);

    double convXAxisMin, convXAxisMax, convYAxisMin, convYAxisMax;

    currXY->xAxis.GetAxisLimits(convXAxisMin, convXAxisMax);
    convXAxisMin = currXY->TransformX(convXAxisMin);
    convXAxisMax = currXY->TransformX(convXAxisMax);

    currXY->yAxis.GetAxisLimits(convYAxisMin, convYAxisMax);
    convYAxisMin = currXY->TransformY(convYAxisMin);
    convYAxisMax = currXY->TransformY(convYAxisMax);

    if (plotAnnotation.drawAxesLines)
    {
        currPlot.AxesDrawSetup();
        currPlot.HardCopyBlockStart(12);         // 2 axes + 2 caps per axes * 2

        if (isXAxis)
        {
            currXY->DrawXAxes(convYAxisMin, -axesOffComp.pY);
            if (plotAnnotation.framePlot)
                currXY->DrawXAxes(convYAxisMax, axesOffComp.pY);
        }
        else
        {
            currXY->DrawYAxes(convXAxisMin, -axesOffComp.pX);
            if (plotAnnotation.framePlot)
                currXY->DrawYAxes(convXAxisMax, axesOffComp.pX);

        }

        if (plotAnnotation.axesOffset > 0)
        {
            if (isXAxis)
            {
                // slight kluge -- tic routines require untransformed X&Ys
                double xmin, xmax;
                currXY->xAxis.GetAxisLimits(xmin, xmax);

                // draw caps on axes
                currXY->DrawXTic(xmin, convYAxisMin, -axesOffComp.pY, axesOffComp.pY, 0.0);
                currXY->DrawXTic(xmax, convYAxisMin, -axesOffComp.pY, axesOffComp.pY, 0.0);

                if (plotAnnotation.framePlot)
                {
                    currXY->DrawXTic(xmin, convYAxisMax, axesOffComp.pY, -axesOffComp.pY, 0.0);
                    currXY->DrawXTic(xmax, convYAxisMax, axesOffComp.pY, -axesOffComp.pY, 0.0);
                }
            }
            else
            {
                // slight kluge -- tic routines require untransformed X&Ys
                double ymin, ymax;
                currXY->yAxis.GetAxisLimits(ymin, ymax);

                // draw caps on axes
                currXY->DrawYTic(convXAxisMin, ymin, -axesOffComp.pX, axesOffComp.pX, 0.0);
                currXY->DrawYTic(convXAxisMin, ymax, -axesOffComp.pX, axesOffComp.pX, 0.0);

                if (plotAnnotation.framePlot)
                {
                    currXY->DrawYTic(convXAxisMax, ymin, axesOffComp.pX, -axesOffComp.pX, 0.0);
                    currXY->DrawYTic(convXAxisMax, ymax, axesOffComp.pX, -axesOffComp.pX, 0.0);
                }
            }
        }
        currPlot.HardCopyBlockEnd();
    }

    //minor incs
    double axmaxOffset = 0.0;
    int nincs = labelText.Size() - 1;

    PC_Axes::AxesIncType minorIncs;
    if (isXAxis)
        minorIncs = currXY->xAxis.axisMinorIncs;
    else
        minorIncs = currXY->yAxis.axisMinorIncs;

    if ((nincs > 0) && (minorIncs != PC_Axes::aitNone))
    {
        if (minorIncs == PC_Axes::aitGrid)
        {
            currPlot.MinorGridDrawSetup();
            currPlot.HardCopyBlockStart(nincs * 2);
            if (plotAnnotation.framePlot)
            {
                if (isXAxis)
                    axmaxOffset = axesOffComp.pY;
                else
                    axmaxOffset = axesOffComp.pX;
            }

        }
        else
        {
            currPlot.MinorTicDrawSetup();
            currPlot.HardCopyBlockStart(nincs * 4);
        }

        int nincChecked = 0;
        double incValue = 0.0;
        for (int i = 0; i < nincs; i++)
        {
            incValue = (labelVal[i] + labelVal[i + 1]) / 2.0;

            if (minorIncs == PC_Axes::aitGrid)
            {
                if (isXAxis)
                    currXY->DrawXGrid(incValue, -axesOffComp.pY, axmaxOffset, minorTicOffset.pY);
                else
                    currXY->DrawYGrid(incValue, -axesOffComp.pX, axmaxOffset, minorTicOffset.pX);
            }
            else
            {
                if (isXAxis)
                {
                    currXY->DrawXTic(incValue, convYAxisMin, -axesOffComp.pY, minorTicLength.pY, majorTicOffset.pY);
                    if (plotAnnotation.framePlot)
                        currXY->DrawXTic(incValue, convYAxisMax, axesOffComp.pY, -minorTicLength.pY, -majorTicOffset.pY);
                }
                else
                {
                    currXY->DrawYTic(convXAxisMin, incValue, -axesOffComp.pX, minorTicLength.pX, majorTicOffset.pX);
                    if (plotAnnotation.framePlot)
                        currXY->DrawYTic(convXAxisMax, incValue, axesOffComp.pX, -minorTicLength.pX, -majorTicOffset.pX);
                }
            }

        }
        currPlot.HardCopyBlockEnd();
    }

    //major incs
    nincs++;
    PC_Axes::AxesIncType majorIncs;
    if (isXAxis)
        majorIncs = currXY->xAxis.axisMajorIncs;
    else
        majorIncs = currXY->yAxis.axisMajorIncs;

    if ((nincs > 0) && (majorIncs != PC_Axes::aitNone))
    {
        if (majorIncs == PC_Axes::aitGrid)
        {
            currPlot.MajorGridDrawSetup();
            currPlot.HardCopyBlockStart(nincs * 2);
            if (plotAnnotation.framePlot)
            {
                if (isXAxis)
                    axmaxOffset = axesOffComp.pY;
                else
                    axmaxOffset = axesOffComp.pX;
            }
        }
        else
        {
            currPlot.MajorTicDrawSetup();
            currPlot.HardCopyBlockStart(nincs * 4);
        }

        for (int i = 0; i < nincs; i++)
        {
            if (majorIncs == PC_Axes::aitGrid)
            {
                if (isXAxis)
                    currXY->DrawXGrid(labelVal[i], -axesOffComp.pY, axmaxOffset, majorTicOffset.pY);
                else
                    currXY->DrawYGrid(labelVal[i], -axesOffComp.pX, axmaxOffset, majorTicOffset.pX);
            }
            else
            {
                if (isXAxis)
                {
                    currXY->DrawXTic(labelVal[i], convYAxisMin, -axesOffComp.pY, majorTicLength.pY, majorTicOffset.pY);
                    if (plotAnnotation.framePlot)
                        currXY->DrawXTic(labelVal[i], convYAxisMax, axesOffComp.pY, -majorTicLength.pY, -majorTicOffset.pY);
                }
                else
                {
                    currXY->DrawYTic(convXAxisMin, labelVal[i], -axesOffComp.pX, majorTicLength.pX, majorTicOffset.pX);
                    if (plotAnnotation.framePlot)
                        currXY->DrawYTic(convXAxisMax, labelVal[i], axesOffComp.pX, -majorTicLength.pX, -majorTicOffset.pX);
                }
            }
        }
        currPlot.HardCopyBlockEnd();
    }

    // increment labels

    double maxHeight = 0.0;
    double maxWidth = 0.0;
    double ypos, xpos;

    if (isXAxis)
        ypos = convYAxisMin - axesOffComp.pY - labelOffComp.pY - majorTicOffset.pY;
    else
        xpos = convXAxisMin - axesOffComp.pX - labelOffComp.pX - majorTicOffset.pX;

    GL_Base::SetColor(plotAnnotation.axesDataPen);

    HorizontalTextAlignment halign;
    VerticalTextAlignment valign;
    if (isXAxis)
    {
        if (xIncrFont.fontRotation == PSC_Font::fr_Horiz)
        {
            halign = hta_Center;
            valign = vta_Top;
        }
        else
        {
            valign = vta_Center;
            if (xIncrFont.fontRotation == PSC_Font::fr_VertLeft)
                halign = hta_Right;
            else
                halign = hta_Left;
        }
    }
    else
    {
        if (yIncrFont.fontRotation == PSC_Font::fr_Horiz)
        {
            halign = hta_Right;
            valign = vta_Center;
        }
        else
        {
            halign = hta_Center;
            if (yIncrFont.fontRotation == PSC_Font::fr_VertLeft)
                valign = vta_Bottom;
            else
                valign = vta_Top;
        }
    }

    double w, h;
    ExtendedLabelData labInfo;

    for (int i = 0; i < nincs; i++)
    {
        if (!labelInLimits[i])
            continue;

        if (isXAxis)
        {
            if (xIncrFont.fontRotation == PSC_Font::fr_Horiz)
            {
                if (currPlot.GetExtendedStringSizeInfo(xIncrFont,
                                                    labelText[i], labInfo))
                {
                    double totHeight = labInfo.height + labInfo.aboveTop;
                    maxHeight = DMax(maxHeight, totHeight);
                }
            }
            else
            {
                if (currPlot.GetStringSizeInfo(xIncrFont, labelText[i], w, h))
                    maxHeight = DMax(maxHeight, w);
            }

            xpos = currXY->TransformX(labelVal[i]);

            currPlot.PrintString(xIncrFont, labelText[i],
                                    halign, valign, xpos, ypos);
        }
        else
        {
            if (yIncrFont.fontRotation == PSC_Font::fr_Horiz)
            {
                if (currPlot.GetStringSizeInfo(yIncrFont, labelText[i], w, h))
                    maxWidth = DMax(maxWidth, w);
            }
            else
            {
                if (currPlot.GetExtendedStringSizeInfo(yIncrFont, labelText[i], labInfo))
                {
                    double totHeight = labInfo.height + labInfo.aboveTop;
                    maxWidth = DMax(maxWidth, totHeight);
                }
            }

            ypos = currXY->TransformY(labelVal[i]);

            currPlot.PrintString(yIncrFont, labelText[i],
                                    halign, valign, xpos, ypos);
        }
    }

    if (isXAxis)
    {
        plotAnnotation.xaxesLabelPos.pY = ypos - maxHeight - labelOffComp.pY;
        plotAnnotation.xaxesLabelPos.pX = (convXAxisMin + convXAxisMax) / 2.0;
    }
    else
    {
        plotAnnotation.yaxesLabelPos.pY = (convYAxisMin + convYAxisMax) / 2.0;
        plotAnnotation.yaxesLabelPos.pX = xpos - maxWidth - labelOffComp.pX * 2.0;
    }

    if (IsNotNull(axesLabel))
    {
        if (isXAxis)
        {
            currPlot.PrintString(plotAnnotation.labelFont, axesLabel,
                            hta_Center, vta_Top,
                            plotAnnotation.xaxesLabelPos.pX,
                            plotAnnotation.xaxesLabelPos.pY);
        }
        else
        {
            PSC_Font yLabFont(plotAnnotation.labelFont);
            yLabFont.fontRotation = PSC_Font::fr_VertLeft;

            currPlot.PrintString(yLabFont, axesLabel,
                            hta_Center, vta_Bottom,
                            plotAnnotation.yaxesLabelPos.pX,
                            plotAnnotation.yaxesLabelPos.pY);
        }
    }

    currPlot.SetClipPlanes();

    CloseDrawObject();
}

void PFO_LabelAxesGL::ReportCursorData(const Coord3D& cursorPos)
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
        cursorReport->ReportLabelSetup(0, "Label:");
        cursorReport->ReportLabelSetup(1, "Index:");
    }

    double pos;
    if (isXAxis)
        pos = cursorPos.cX;
    else
        pos = cursorPos.cY;

    RoundOff(pos, 0);
    int indx = int(pos);

    if ((indx >= 0) && (indx < labelVal.Size()))
    {

        cursorReport->ReportUpdate(0, labelText[indx]);

        char outStr[80];
        SC_RealFormat conv;
        conv.RealToString(labelVal[indx], outStr, 80);
        cursorReport->ReportUpdate(1, outStr);
    }
}

