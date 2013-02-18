///////////////////////////////////////////////////////////////////////////////////
//
// PD_2DXYPS.cpp
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
//      merges OGL_2DXY with PD_2DXY.  Implements axes drawing routines.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genClassPS/PS_Color.h>

#include <genPlotClass/C_PlotObj.h>

#include <genPlotClassPS/PC_Platform2DPS.h>
#include <genPlotClassPS/PC_ReportPS.h>

#include <genPlotClassPS/PD_2DXYPS.h>


//  derived for plot type specific implementation of Init and Reset

class PC_Platform2DXYPS : public PC_Platform2DPS {
    private:
        PD_2DXYPS&      xy2DplotDef;
    public:

                        PC_Platform2DXYPS(PD_2DXYPS& assPlt, OGL2DBase& ogl2DObj) :
                            xy2DplotDef(assPlt), PC_Platform2DPS("XY Plot", assPlt, ogl2DObj) {};
                        ~PC_Platform2DXYPS(){};

        //  virtuals form PC_Platform
        void            InitGraphicsWindow();
        void            ResetGraphicsWindow();
};


void PC_Platform2DXYPS:: InitGraphicsWindow()
{
    //  base class initialization
    PC_Platform2DPS :: InitGraphicsWindow();

    initComplete = true;
}

void PC_Platform2DXYPS:: ResetGraphicsWindow()
{
    //  base class reset
    PC_Platform2DPS :: ResetGraphicsWindow();
    if (xy2DplotDef.IsMasterPlot())
        xy2DplotDef.AdjustMargins();
}


PD_2DXYPS :: PD_2DXYPS() : OGL2DXY(*(static_cast<PD_2DXY*>(this)))
{
    InitCommon();
}

PD_2DXYPS :: ~PD_2DXYPS ()
{
    delete cursorReport;
}

PD_2DXYPS :: PD_2DXYPS(const PD_2DXYPS& a) : OGL2DXY(*(static_cast<PD_2DXY*>(this))), PD_2DXY(a)
{
    InitCommon();
}

void PD_2DXYPS::InitCommon()
{
    //  create and initialize all PS data
    cursorReport = new PC_ReportPS(*this);
    platformData = new PC_Platform2DXYPS(*this, *this);
}

PD_2DXYPS& PD_2DXYPS::operator= (const PD_2DXYPS& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}

void  PD_2DXYPS::FullCopy(const PD_2DXYPS& a)
{
    PD_2DXY::FullCopy(a);
    LocalCopy(a);
}

void  PD_2DXYPS::LocalCopy(const PD_2DXYPS& a)
{
    //  no properties yet
}


OGLBase* PD_2DXYPS::GetOGLBase()
{
    return this;
}


void PD_2DXYPS::ResetSize()
{
    if (IsMasterPlot())
        AdjustMargins();
    static_cast<PC_PlatformPS*>(platformData)->DoResize();
}

void  PD_2DXYPS::InitRenderer()
{
    //  switch context
    if (currOGLBase)
        currOGLBase->MakeCurrent();
    // set clear color
    SetClearColor(defaultPenSet->GetColor(plotBackgroundPen));
    Clear();
}

void PD_2DXYPS::StartLineSection()
{
    SetLineAntiAlias();
}
void PD_2DXYPS::EndLineSection()
{
    DisableLineAntiAlias();
}

void  PD_2DXYPS::SetupDataView()
{
    SetClearColor(defaultPenSet->GetColor(plotBackgroundPen));
    InitView();
}

void  PD_2DXYPS::SetupAnnoView()
{
    InitAnnoView();
}

void  PD_2DXYPS::CloseRenderer()
{
    Flush();
}


void PD_2DXYPS::DrawXAxes(const double& yVal,
                               const double& yaxOffset)
{
    Point2D stPoint(convXaxisMin, yVal + yaxOffset);
    Point2D endPoint(convXaxisMax, yVal + yaxOffset);
    DrawLine(stPoint, endPoint);
}

void PD_2DXYPS::DrawXTic(const double& xVal,
                              const double& yVal,
                              const double& yaxOffset,
                              const double& ticLen,
                              const double& ticOffset)
{
    double xTran = TransformX(xVal);
    if (Limit::WithinOneLimit(convXaxisMin, convXaxisMax, xTran))
    {
        double ypt = yVal + yaxOffset - ticOffset;
        Point2D stPoint(xTran, ypt);
        Point2D endPoint(xTran, ypt + ticLen);
        DrawLine(stPoint, endPoint);
    }
}

void PD_2DXYPS::DrawXGrid(const double& xVal,
                          const double& yminOffset,
                          const double& ymaxOffset,
                          const double& ticOffset)
{
    double xTran = TransformX(xVal);
    if (plotAnnotation.DoYAxis() && (xTran == convXaxisMin))
        return;
    if (plotAnnotation.DoYAxis() && plotAnnotation.framePlot && (xTran == convXaxisMax))
        return;

    if (Limit::WithinOneLimit(convXaxisMin, convXaxisMax, xTran))
    {
        Point2D stPoint(xTran, convYaxisMin + yminOffset - ticOffset);
        Point2D endPoint(xTran, convYaxisMax + ymaxOffset + ticOffset);
        DrawLine(stPoint, endPoint);
    }
}


void PD_2DXYPS::DrawYAxes(const double& xVal,
                               const double& xaxOffset)
{
    Point2D stPoint(xVal + xaxOffset, convYaxisMin);
    Point2D endPoint(xVal + xaxOffset, convYaxisMax);
    DrawLine(stPoint, endPoint);
}

void PD_2DXYPS::DrawYTic(const double& xVal,
                              const double& yVal,
                              const double& xaxOffset,
                              const double& ticLen,
                               const double& ticOffset)

{
    double yTran = TransformY(yVal);
    if (Limit::WithinOneLimit(convYaxisMin, convYaxisMax, yTran))
    {
        double xpt = xVal + xaxOffset - ticOffset;
        Point2D stPoint(xpt, yTran);
        Point2D endPoint(xpt + ticLen, yTran);
        DrawLine(stPoint, endPoint);
    }
}

void PD_2DXYPS::DrawYGrid(const double& yVal,
                               const double& xminOffset,
                               const double& xmaxOffset,
                               const double& ticOffset)
{
    double yTran = TransformY(yVal);
    if (plotAnnotation.DoXAxis() && (yTran == convYaxisMin))
        return;
    if (plotAnnotation.DoXAxis() && plotAnnotation.framePlot && (yTran == convYaxisMax))
        return;

    if (Limit::WithinOneLimit(convYaxisMin, convYaxisMax, yTran))
    {
        Point2D stPoint(convXaxisMin + xminOffset - ticOffset, yTran);
        Point2D endPoint(convXaxisMax + xmaxOffset + ticOffset, yTran);
        DrawLine(stPoint, endPoint);
    }
}


double PD_2DXYPS::GetMinorIncVal(const SC_DoubleArray& majorVals,
                                 const SC_DoubleArray& minorVals,
                                       int             majorIndex,
                                       int             minorIndex,
                                       bool            isLog,
                                       bool            isReversed)

{
    double incVal;
    if (majorIndex < 0)
    {
        if (isLog)
            if (isReversed)
                incVal = majorVals[0] * minorVals[minorIndex];
            else
                incVal = majorVals[0] / 10.0 * minorVals[minorIndex];
        else
                incVal = majorVals[0] - minorVals[minorIndex];

    }
    else
    {
        if (isLog)
            if (isReversed)
                incVal =  majorVals[majorIndex] / 10.0 * minorVals[minorIndex];
            else
                incVal =  majorVals[majorIndex]  * minorVals[minorIndex];
        else
                incVal =  majorVals[majorIndex] + minorVals[minorIndex];
    }
    return incVal;
}

void PD_2DXYPS::DrawPlotAxes()
{

    if (!plotAnnotation.DoAnno())
    {
        SetClipPlanes();
        return;
    }

    SetIncrFont();

    ClearClipPlanes();

    SetAxesLocals();
    SetLineSolid();

    xAxis.SetMajorIncs();
    xAxis.SetMinorIncs();
    yAxis.SetMajorIncs();
    yAxis.SetMinorIncs();

    SC_DoubleArray xMajorIncs, xMinorIncs, yMajorIncs, yMinorIncs;
    xAxis.GetAxesMajorIncs(xMajorIncs);
    xAxis.GetAxesMinorIncs(xMinorIncs);
    yAxis.GetAxesMajorIncs(yMajorIncs);
    yAxis.GetAxesMinorIncs(yMinorIncs);

    Point2D axesOffComp, labelOffComp;
    bool dummy;
    GetAxesOffsets(axesOffComp, labelOffComp, dummy);

    Point2D majorTicLength = GetPixelComponents(plotAnnotation.majorTicLength);
    Point2D minorTicLength = GetPixelComponents(plotAnnotation.minorTicLength);
    Point2D majorTicOffset = GetPixelComponents(plotAnnotation.majorTicOffset);
    Point2D minorTicOffset = GetPixelComponents(plotAnnotation.minorTicOffset);

    // set Z val
    SetAnnoLow();

    if (plotAnnotation.drawAxesLines)
    {
        AxesDrawSetup();
        HardCopyBlockStart(24);         // 4 axes + 2 caps per axes * 2

        // x axis
        if (plotAnnotation.DoXAxis())
        {
            DrawXAxes(convYaxisMin, -axesOffComp.pY);
            if (plotAnnotation.framePlot)
                DrawXAxes(convYaxisMax, axesOffComp.pY);
        }

        // y axis
        if (plotAnnotation.DoYAxis())
        {
            DrawYAxes(convXaxisMin, -axesOffComp.pX);
            if (plotAnnotation.framePlot)
                DrawYAxes(convXaxisMax, axesOffComp.pX);
        }

        if (plotAnnotation.axesOffset > 0)
        {
            // slight kluge -- tic routines require untransformed X&Ys
            double xmin, xmax, ymin, ymax;
            xAxis.GetAxisLimits(xmin, xmax);
            yAxis.GetAxisLimits(ymin, ymax);

            // draw caps on axes

            if (plotAnnotation.DoXAxis())
            {
                DrawXTic(xmin, convYaxisMin, -axesOffComp.pY, axesOffComp.pY, 0.0);
                DrawXTic(xmax, convYaxisMin, -axesOffComp.pY, axesOffComp.pY, 0.0);

                if (plotAnnotation.framePlot)
                {
                    DrawXTic(xmin, convYaxisMax, axesOffComp.pY, -axesOffComp.pY, 0.0);
                    DrawXTic(xmax, convYaxisMax, axesOffComp.pY, -axesOffComp.pY, 0.0);
                }
            }

            if (plotAnnotation.DoYAxis())
            {
                DrawYTic(convXaxisMin, ymin, -axesOffComp.pX, axesOffComp.pX, 0.0);
                DrawYTic(convXaxisMin, ymax, -axesOffComp.pX, axesOffComp.pX, 0.0);

                if (plotAnnotation.framePlot)
                {
                    DrawYTic(convXaxisMax, ymin, axesOffComp.pX, -axesOffComp.pX, 0.0);
                    DrawYTic(convXaxisMax, ymax, axesOffComp.pX, -axesOffComp.pX, 0.0);
                }
            }
        }
        HardCopyBlockEnd();
        SetAnnoLow();
    }

    double axmaxOffset = 0.0;

    if (plotAnnotation.DoXAxis() && (xAxis.axisMajorIncs != PC_Axes::aitNone))
        if (xAxis.axisMajorIncs == PC_Axes::aitGrid)
        {
            MajorGridDrawSetup();
            HardCopyBlockStart(xMajorIncs.Size() * 2);

            if (plotAnnotation.framePlot)
                axmaxOffset = axesOffComp.pY;

            for (int i = 0; i < xMajorIncs.Size(); i++)
                DrawXGrid(xMajorIncs[i], -axesOffComp.pY, axmaxOffset, majorTicOffset.pY);

            HardCopyBlockEnd();
            SetAnnoLow();
        }
        else
        {
            MajorTicDrawSetup();
            HardCopyBlockStart(xMajorIncs.Size() * 4);
            int i;
            for (i = 0; i < xMajorIncs.Size(); i++)
            {
                DrawXTic(xMajorIncs[i], convYaxisMin, -axesOffComp.pY, majorTicLength.pY, majorTicOffset.pY);
                if (plotAnnotation.framePlot)
                    DrawXTic(xMajorIncs[i], convYaxisMax, axesOffComp.pY, -majorTicLength.pY, -majorTicOffset.pY);
            }
            HardCopyBlockEnd();
        }

    if (plotAnnotation.DoYAxis() && (yAxis.axisMajorIncs != PC_Axes::aitNone))
        if (yAxis.axisMajorIncs == PC_Axes::aitGrid)
        {
            MajorGridDrawSetup();

            if (plotAnnotation.framePlot)
                axmaxOffset = axesOffComp.pX;
            HardCopyBlockStart(yMajorIncs.Size() * 2);

            for (int i = 0; i < yMajorIncs.Size(); i++)
            {

                DrawYGrid(yMajorIncs[i], -axesOffComp.pX, axmaxOffset, majorTicOffset.pX);
            }

            HardCopyBlockEnd();
            SetAnnoLow();
        }
        else
        {
            MajorTicDrawSetup();
            HardCopyBlockStart(yMajorIncs.Size() * 4);
            for (int i = 0; i < yMajorIncs.Size(); i++)
            {
                DrawYTic(convXaxisMin, yMajorIncs[i], -axesOffComp.pX, majorTicLength.pX, majorTicOffset.pX);
                if (plotAnnotation.framePlot)
                    DrawYTic(convXaxisMax, yMajorIncs[i],  axesOffComp.pX, -majorTicLength.pX, -majorTicOffset.pX);
            }
            HardCopyBlockEnd();
        }


    if (plotAnnotation.DoXAxis() && (xAxis.axisMinorIncs != PC_Axes::aitNone))
        if (xAxis.axisMinorIncs == PC_Axes::aitGrid)
        {
            MinorGridDrawSetup();

            if (plotAnnotation.framePlot)
                axmaxOffset = axesOffComp.pY;

            HardCopyBlockStart((xMajorIncs.Size() + 1) * xMinorIncs.Size() * 2);

            for (int i = -1; i < xMajorIncs.Size(); i++)
                for (int j = 0; j < xMinorIncs.Size(); j++)
                    DrawXGrid(GetMinorIncVal(xMajorIncs, xMinorIncs, i, j, doLogX, convXaxisMax < convXaxisMin),
                                            -axesOffComp.pY, axmaxOffset, minorTicOffset.pX);

            HardCopyBlockEnd();
            SetAnnoLow();
        }
        else
        {
            MinorTicDrawSetup();
            HardCopyBlockStart((xMajorIncs.Size() + 1) * xMinorIncs.Size() * 4);
            for (int i = -1; i < xMajorIncs.Size(); i++)
                for (int j = 0; j < xMinorIncs.Size(); j++)
                {
                    double incVal = GetMinorIncVal(xMajorIncs, xMinorIncs, i, j, doLogX, convXaxisMax < convXaxisMin);
                    DrawXTic(incVal, convYaxisMin, -axesOffComp.pY, minorTicLength.pY, minorTicOffset.pY);
                    if (plotAnnotation.framePlot)
                        DrawXTic(incVal, convYaxisMax, axesOffComp.pY, -minorTicLength.pY, -minorTicOffset.pY);
                }
            HardCopyBlockEnd();
        }


    if (plotAnnotation.DoYAxis() && (yAxis.axisMinorIncs != PC_Axes::aitNone))
        if (yAxis.axisMinorIncs == PC_Axes::aitGrid)
        {
            MinorGridDrawSetup();

            if (plotAnnotation.framePlot)
                axmaxOffset = axesOffComp.pX;
            HardCopyBlockStart((yMajorIncs.Size() + 1) * yMinorIncs.Size() * 2);

            for (int i = -1; i < yMajorIncs.Size(); i++)
                for (int j = 0; j < yMinorIncs.Size(); j++)
                    DrawYGrid(GetMinorIncVal(yMajorIncs, yMinorIncs, i, j, doLogY, convYaxisMax < convYaxisMin),
                                    -axesOffComp.pX, axmaxOffset, minorTicOffset.pX);

            HardCopyBlockEnd();
            SetAnnoLow();
        }
        else
        {
            MinorTicDrawSetup();
            HardCopyBlockStart((yMajorIncs.Size() + 1) * yMinorIncs.Size() * 4);
            for (int i = -1; i < yMajorIncs.Size(); i++)
                for (int j = 0; j < yMinorIncs.Size(); j++)
                {
                    double incVal = GetMinorIncVal(yMajorIncs, yMinorIncs, i, j, doLogY, convYaxisMax < convYaxisMin);
                    DrawYTic(convXaxisMin, incVal,  -axesOffComp.pX, minorTicLength.pX, minorTicOffset.pX);
                    if (plotAnnotation.framePlot)
                        DrawYTic(convXaxisMax, incVal,  axesOffComp.pX, -minorTicLength.pX, -minorTicOffset.pX);
                }
            HardCopyBlockEnd();
        }

    double maxHeight = 0.0;
    double xaxYpos = convYaxisMin - axesOffComp.pY - labelOffComp.pY - majorTicOffset.pY;
    double maxWidth = 0.0;
    double yaxXpos = convXaxisMin - axesOffComp.pX - labelOffComp.pX - majorTicOffset.pX;

    if (plotAnnotation.labelIncrements)
    {
        SetDrawColor(defaultPenSet->GetColor(plotAnnotation.axesDataPen));

        char labStr[80];
        ExtendedLabelData labInfo;

        if (plotAnnotation.DoXAxis())
        {
            // need to go through all labels once to calc over top on exponents if horiz
            double exponentShift = 0.0;
            for (int i = 0; i < xMajorIncs.Size(); i++)
            {
                double xTran = TransformX(xMajorIncs[i]);
                if (Limit::WithinOneLimit(convXaxisMin, convXaxisMax, xTran))
                {
                    xAxis.axisIncFormat.RealToString(xMajorIncs[i], labStr, 80);

                    if (xAxis.incrFont.fontRotation == PSC_Font::fr_Horiz)
                    {
                        if (GetExtendedStringSizeInfo(xAxis.incrFont, labStr, labInfo))
                        {
                            if (labInfo.aboveTop > exponentShift)
                                exponentShift = labInfo.aboveTop;
                            double totHeight = labInfo.height + labInfo.aboveTop;
                            if ((fabs(totHeight) > fabs(maxHeight)))
                                maxHeight = totHeight;
                        }
                    }
                    else
                    {
                        double w, h;
                        if (GetStringSizeInfo(xAxis.incrFont, labStr, w, h))
                        {
                            if (fabs(w) > fabs(maxHeight))
                                maxHeight = w;
                        }
                    }
                }
            }

            //set alignment for printing
            HorizontalTextAlignment halign;
            VerticalTextAlignment valign;
            if (xAxis.incrFont.fontRotation == PSC_Font::fr_Horiz)
            {
                halign = hta_Center;
                valign = vta_Top;
            }
            else
            {
                valign = vta_Center;
                if (xAxis.incrFont.fontRotation == PSC_Font::fr_VertLeft)
                    halign = hta_Right;
                else
                    halign = hta_Left;
            }

            // now print label
            double xincLabYPos = xaxYpos - exponentShift;
            for (int i = 0; i < xMajorIncs.Size(); i++)
            {
                double xTran = TransformX(xMajorIncs[i]);
                if (Limit::WithinOneLimit(convXaxisMin, convXaxisMax, xTran))
                {
                    xAxis.axisIncFormat.RealToString(xMajorIncs[i], labStr, 80);

                    PrintString(xAxis.incrFont, labStr, halign, valign,
                                xTran, xincLabYPos);
                }
            }
        }

        if (plotAnnotation.DoYAxis())
        {
            // need to go through all labels once to calc over top on exponents if not horiz
            double exponentShift = 0.0;
            for (int i = 0; i < yMajorIncs.Size(); i++)
            {
                double yTran = TransformY(yMajorIncs[i]);
                if (Limit::WithinOneLimit(convYaxisMin, convYaxisMax, yTran))
                {
                    yAxis.axisIncFormat.RealToString(yMajorIncs[i], labStr, 80);

                    if (yAxis.incrFont.fontRotation != PSC_Font::fr_Horiz)
                    {
                        if (GetExtendedStringSizeInfo(yAxis.incrFont, labStr, labInfo))
                        {
                            if (labInfo.aboveTop > exponentShift)
                                exponentShift = labInfo.aboveTop;
                            double totHeight = labInfo.height + labInfo.aboveTop;
                            if ((fabs(totHeight) > fabs(maxWidth)))
                                maxWidth = totHeight;
                        }
                    }
                    else
                    {
                        double w, h;
                        if ((GetStringSizeInfo(yAxis.incrFont, labStr, w, h))
                            && (fabs(w) > fabs(maxWidth)))
                                maxWidth = w;
                    }
                }
            }

            //set alignment for printing
            HorizontalTextAlignment halign;
            VerticalTextAlignment valign;
            if (yAxis.incrFont.fontRotation == PSC_Font::fr_Horiz)
            {
                halign = hta_Right;
                valign = vta_Center;
            }
            else
            {
                halign = hta_Center;
                if (yAxis.incrFont.fontRotation == PSC_Font::fr_VertLeft)
                    valign = vta_Bottom;
                else
                    valign = vta_Top;
            }

            for (int i = 0; i < yMajorIncs.Size(); i++)
            {
                double yTran = TransformY(yMajorIncs[i]);
                if (Limit::WithinOneLimit(convYaxisMin, convYaxisMax, yTran))
                {
                    yAxis.axisIncFormat.RealToString(yMajorIncs[i], labStr, 80);
                    PrintString(yAxis.incrFont, labStr, halign, valign,
                                yaxXpos, yTran);
                }
            }
        }
    }

    plotAnnotation.xaxesLabelPos.pX = (convXaxisMin + convXaxisMax) / 2.0;
    plotAnnotation.xaxesLabelPos.pY = xaxYpos - maxHeight - labelOffComp.pY;
    plotAnnotation.yaxesLabelPos.pX = yaxXpos - maxWidth - labelOffComp.pX * 2.0;
    plotAnnotation.yaxesLabelPos.pY = (convYaxisMin + convYaxisMax) / 2.0;

    if (plotAnnotation.labelAxes)
    {
        if (plotAnnotation.DoXAxis() && IsNotNull(plotAnnotation.xaxesLabel))
        {
            PrintString(plotAnnotation.labelFont, plotAnnotation.xaxesLabel,
                hta_Center, vta_Top,
                plotAnnotation.xaxesLabelPos.pX, plotAnnotation.xaxesLabelPos.pY);
        }

        if (plotAnnotation.DoYAxis() && IsNotNull(plotAnnotation.yaxesLabel))
        {
            PSC_Font yLabFont(plotAnnotation.labelFont);
            yLabFont.fontRotation = PSC_Font::fr_VertLeft;

            PrintString(yLabFont, plotAnnotation.yaxesLabel, hta_Center, vta_Bottom,
                plotAnnotation.yaxesLabelPos.pX, plotAnnotation.yaxesLabelPos.pY);
        }
    }

    SetClipPlanes();
}

void PD_2DXYPS::SetAxesLocals()
{
    xAxis.GetAxisLimits(convXaxisMin, convXaxisMax);
    yAxis.GetAxisLimits(convYaxisMin, convYaxisMax);
    convXaxisMin = TransformX(convXaxisMin);
    convYaxisMin = TransformY(convYaxisMin);
    convXaxisMax = TransformX(convXaxisMax);
    convYaxisMax = TransformY(convYaxisMax);
}

void  PD_2DXYPS::SetClipPlanes()
{
    SetAxesLocals();

    double planeEqn[4];

    planeEqn[1] = 0.0;
    planeEqn[2] = 0.0;
    //  left X
    if (convXaxisMin < convXaxisMax)
    {
        planeEqn[0] = 1.0;
        planeEqn[3] = -convXaxisMin;
    }
    else
    {
        planeEqn[0] = -1.0;
        planeEqn[3] = convXaxisMin;
    }
    SetClipPlane(0, planeEqn);

    //  right X
    if (convXaxisMin < convXaxisMax)
    {
        planeEqn[0] = -1.0;
        planeEqn[3] = convXaxisMax;
    }
    else
    {
        planeEqn[0] = 1.0;
        planeEqn[3] = -convXaxisMax;
    }
    SetClipPlane(1, planeEqn);

    planeEqn[0] = 0.0;

    // bottom
    if (convYaxisMin < convYaxisMax)
    {
        planeEqn[1] = 1.0;
        planeEqn[3] = -convYaxisMin;
    }
    else
    {
        planeEqn[1] = -1.0;
        planeEqn[3] = convYaxisMin;
    }
    SetClipPlane(2, planeEqn);

    // top
    if (convYaxisMin < convYaxisMax)
    {
        planeEqn[1] = -1.0;
        planeEqn[3] = convYaxisMax;
    }
    else
    {
        planeEqn[1] = 1.0;
        planeEqn[3] = -convYaxisMax;
    }
    SetClipPlane(3, planeEqn);
}

