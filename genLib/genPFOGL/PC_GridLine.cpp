///////////////////////////////////////////////////////////////////////////////////
//
// PC_GridLine.cpp
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

#include <algorithm>
#include <genClass/U_Real.h>

#include <genPFOGL/PC_GridLine.h>

PC_GridLine::PC_GridLine(OGL2DBase& inPlot, bool xGrid)
    : PC_Label(inPlot), gridIsX(xGrid)
{
    labelAtAxes = false;
    labelAtStart = false;
};


bool PC_GridLine::CreateGridLine(const double&   gVal,
                                 const Limit3D&  plotLimits)
{
    bool onPlot;
    if (gridIsX)
    {
        onPlot = Limit::WithinOneLimit(plotLimits.minLim.cX,  plotLimits.maxLim.cX, gVal);
        gridLineStart = Point2D(gVal, plotLimits.minLim.cY);
        gridLineEnd = Point2D(gVal, plotLimits.maxLim.cY);
    }
    else
    {
        onPlot = Limit::WithinOneLimit(plotLimits.minLim.cY,  plotLimits.maxLim.cY, gVal);
        gridLineStart = Point2D(plotLimits.minLim.cX, gVal);
        gridLineEnd = Point2D(plotLimits.maxLim.cX, gVal);
    }
    return ((onPlot) && (currPlot.TransformForXY(gridLineStart)) && (currPlot.TransformForXY(gridLineEnd)));
}


void PC_GridLine::SetLabelPos(const double&   tVal)
{
    labelAtStart = (tVal < 0.001);
    labelAtAxes = labelAtStart || (tVal > 0.999);
    Line2D tempLine(gridLineStart, gridLineEnd);
    labelPos = tempLine.PointOnLine(tVal);
}

void PC_GridLine::AdjustForAxes()
{
    Point2D axesOffset, labOffset;
    bool isFramed;
    currPlot.GetAxesOffsets(axesOffset, labOffset, isFramed);
    if (gridIsX)
    {
        gridLineStart.pY -= axesOffset.pY;
        if (isFramed)
            gridLineEnd.pY += axesOffset.pY;

        if (labelAtAxes)
            if (labelAtStart)
            {
                labelPos = gridLineStart;
                labelPos.pY -= labOffset.pY;
            }
            else
                labelPos = gridLineEnd;
    }
    else
    {
        gridLineStart.pX -= axesOffset.pX;
        if (isFramed)
            gridLineEnd.pX += axesOffset.pX;
        if (labelAtAxes)
            if (labelAtStart)
            {
                labelPos = gridLineStart;
                labelPos.pX -= labOffset.pX;
            }
            else
                labelPos = gridLineEnd;
    }
}

void PC_GridLine::SwapMetrics(double& xVal, double& yVal)
{
    xVal = fabs(xVal);
    yVal = fabs(yVal);
    std::swap(xVal, yVal);

    if (gridLineStart.pY > gridLineEnd.pY)
        yVal = -yVal;

    if (gridLineStart.pX > gridLineEnd.pX)
        xVal = -xVal;
}

void PC_GridLine::DrawBlankedLine(const PSC_Font&               labelFont,
                                  const char*                   labelText,
                                        HorizontalTextAlignment halign,
                                        VerticalTextAlignment   valign)
{

    double lineMult = currPlot.GetDefaultLineMultiplier();
    ExtendedLabelData textInfo;
    if (!currPlot.GetExtendedStringSizeInfo(labelFont, labelText, lineMult, textInfo))
        return;

    double xSpace, ySpace;
    if (!currPlot.GetFrameIncrement(labelFont, textInfo.aboveTop, textInfo.belowBase, xSpace, ySpace))
        return;

    double blankX = textInfo.width;
    double blankY = textInfo.height;

    if (labelFont.IsVertical())
    {
        SwapMetrics(blankX, blankY);

        HorizontalTextAlignment saveH = halign;
        VerticalTextAlignment saveV = valign;
        // map to horizontal
        if (labelFont.fontRotation == PSC_Font::fr_VertLeft)
        {
            switch (saveV)
            {
                case vta_Top    : {halign = hta_Left; break;}
                case vta_Center : {halign = hta_Center; break;}
                case vta_Bottom : {halign = hta_Right; break;}
            }

            switch (saveH)
            {
                case hta_Left   : {valign = vta_Bottom; break;}
                case hta_Center : {valign = vta_Center; break;}
                case hta_Right  : {valign = vta_Top; break;}
            }
        }
        else
        {
            switch (saveV)
            {
                case vta_Top    : {halign = hta_Right; break;}
                case vta_Center : {halign = hta_Center; break;}
                case vta_Bottom : {halign = hta_Left; break;}
            }

            switch (saveH)
            {
                case hta_Left   : {valign = vta_Top; break;}
                case hta_Center : {valign = vta_Center; break;}
                case hta_Right  : {valign = vta_Bottom; break;}
            }
        }
    }

    // now as if horiz label with horiz relative align

    Line2D lineA(gridLineStart, labelPos);
    Line2D lineB(labelPos, gridLineEnd);
    bool doA, doB;
    if (gridIsX)
    {
        // adjust for possible offset effects
        lineA.endPt.pX = lineA.stPt.pX;
        lineB.stPt.pX = lineB.endPt.pX;
        switch (valign)
        {
            case vta_Top    : {lineA.endPt.pY -= blankY; break;}
            case vta_Center : {lineB.stPt.pY += blankY / 2.0;
                               lineA.endPt.pY -= blankY / 2.0; break;}
            case vta_Bottom : {lineB.stPt.pY += blankY; break;}
        }

        lineA.endPt.pY -= ySpace;
        lineB.stPt.pY += ySpace;

        if (gridLineStart.pY > gridLineEnd.pY)
        {
            if (lineB.stPt.pY > lineA.stPt.pY)
                lineB.stPt.pY = lineA.stPt.pY;
            if (lineA.endPt.pY < lineB.endPt.pY)
                lineA.endPt.pY = lineB.endPt.pY;
            doA = lineA.stPt.pY > lineA.endPt.pY;
            doB = lineB.stPt.pY > lineB.endPt.pY;
        }
        else
        {
            if (lineB.stPt.pY < lineA.stPt.pY)
                lineB.stPt.pY = lineA.stPt.pY;
            if (lineA.endPt.pY > lineB.endPt.pY)
                lineA.endPt.pY = lineB.endPt.pY;
            doA = lineA.stPt.pY < lineA.endPt.pY;
            doB = lineB.stPt.pY < lineB.endPt.pY;
        }

    }
    else
    {
        // adjust for possible offset effects
        lineA.endPt.pY = lineA.stPt.pY;
        lineB.stPt.pY = lineB.endPt.pY;
        switch (halign)
        {
            case hta_Left    : {lineB.stPt.pX += blankX; break;}
            case hta_Center : {lineB.stPt.pX += blankX / 2.0;
                               lineA.endPt.pX -= blankX / 2.0; break;}
            case hta_Right : {lineA.endPt.pX -= blankX; break;}
        }

        lineA.endPt.pX -= xSpace;
        lineB.stPt.pX += xSpace;

        if (gridLineStart.pX > gridLineEnd.pX)
        {
            if (lineB.stPt.pX > lineA.stPt.pX)
                lineB.stPt.pX = lineA.stPt.pX;
            if (lineA.endPt.pX < lineB.endPt.pX)
                lineA.endPt.pX = lineB.endPt.pX;
            doA = lineA.stPt.pX > lineA.endPt.pX;
            doB = lineB.stPt.pX > lineB.endPt.pX;
        }
        else
        {
            if (lineB.stPt.pX < lineA.stPt.pX)
                lineB.stPt.pX = lineA.stPt.pX;
            if (lineA.endPt.pX > lineB.endPt.pX)
                lineA.endPt.pX = lineB.endPt.pX;
            doA = lineA.stPt.pX < lineA.endPt.pX;
            doB = lineB.stPt.pX < lineB.endPt.pX;
        }
    }


    if (doA || doB)
    {
        currPlot.ClearClipPlanes();
        currPlot.HardCopyBlockStart(4);
        if (doA)
            currPlot.DrawLine(lineA.stPt, lineA.endPt);
        if (doB)
            currPlot.DrawLine(lineB.stPt, lineB.endPt);
        currPlot.HardCopyBlockEnd();
        currPlot.SetClipPlanes();
    }

}

void PC_GridLine::DrawGridLine()
{
    currPlot.ClearClipPlanes();
    currPlot.HardCopyBlockStart(2);
    currPlot.DrawLine(gridLineStart, gridLineEnd);
    currPlot.HardCopyBlockEnd();
    currPlot.SetClipPlanes();
}





