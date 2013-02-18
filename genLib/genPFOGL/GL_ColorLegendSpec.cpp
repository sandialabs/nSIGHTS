///////////////////////////////////////////////////////////////////////////////////
//
// GL_ColorLegendSpec.cpp
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

#include <genClass/U_Real.h>

#include <genPFOGL/GL_Polygon.h>
#include <genPFOGL/GL_Line.h>

#include <genPFOGL/GL_Base.h>

#include <genPFOGL/GL_ColorLegendSpec.h>

GL_ColorLegendSpec :: GL_ColorLegendSpec(PSC_ColorLegendSpec& inSpec, AnnoObjC& inObj, OGLBase&  inBase) :
    PSC_ColorLegendSpec(inSpec), objectBase(inBase), PSC_AnnoSpec(inObj)
{
    colorMap      = inSpec.colorMap;
    colorLimits   = inSpec.colorLimits;
    clipToRange   = inSpec.clipToRange;

    labelFont              = inObj.legendFont;
    labelFont.fontRotation = inSpec.orientation;
    aspectCorrection        = double(inObj.assocPlot->plotVheight) / double(inObj.assocPlot->plotUwidth);

    incLabels.SetStringLen(80);
}

GL_ColorLegendSpec::~GL_ColorLegendSpec()
{
}


void  GL_ColorLegendSpec :: CalculateLegendPosition()
{
    colorBarRect.ul       = ulLocation;
    double barWidth     = barLength / barAspect;
    incXtra             = 0.25 * barWidth;

    if (orientation == PSC_Font::fr_Horiz)
    {
        barWidth /= aspectCorrection;
        colorBarRect.lr.pX = colorBarRect.ul.pX + barLength;
        if ( colorBarRect.lr.pX > AnnoObjC::maxAnnoCoord)
        {
            colorBarRect.ul.pX = AnnoObjC::maxAnnoCoord - barLength;
            colorBarRect.lr.pX = AnnoObjC::maxAnnoCoord;
        }

        colorBarRect.lr.pY = colorBarRect.ul.pY - barWidth;
        if ( colorBarRect.lr.pY < 0.0)
        {
            colorBarRect.lr.pY = 0.0;
            colorBarRect.ul.pY = barWidth;
        }
    }
    else
    {
        barWidth *= aspectCorrection;
        colorBarRect.lr.pX = colorBarRect.ul.pX + barWidth;
        if ( colorBarRect.lr.pX > AnnoObjC::maxAnnoCoord)
        {
            colorBarRect.ul.pX = AnnoObjC::maxAnnoCoord - barWidth;
            colorBarRect.lr.pX = AnnoObjC::maxAnnoCoord;
        }

        colorBarRect.lr.pY = colorBarRect.ul.pY - barLength;
        if ( colorBarRect.lr.pY < 0.0)
        {
            colorBarRect.lr.pY = 0.0;
            colorBarRect.ul.pY = barLength;
        }
    }
}



void  GL_ColorLegendSpec :: DrawLegendBase()
{
    //  need to calc max extents
    fullFrame = colorBarRect;

    double labWidth, labHeight;

    // need to process first in case there are any sub super commands which should affect incs
    ExtendedLabelData labInfo;
    if (dataTypeLabel)
    {
        if (!objectBase.GetExtendedStringSizeInfo(labelFont, legendTitle, labInfo))
            return;
    }

    if (labelFont.IsHorizontal())
    {
        //  need length of first and last incs to add to fullframe
        if (!objectBase.GetStringSizeInfo(labelFont, incLabels[0], labWidth, labHeight))
            return;
        fullFrame.ul.pX -= labWidth / 2.0;

        if (!objectBase.GetExtendedStringSizeInfo(labelFont, incLabels.LastIndex(), labInfo))
            return;
        fullFrame.lr.pX += labInfo.width / 2.0;

        double labSpace = labHeight / 2.0;
        // add height -- overTop possible for exponential forms
        if (incLabelAboveLeft)
        {
            incLabelPos = fullFrame.ul.pY + labSpace;
            if (ticsAtIncs)
                incLabelPos += incXtra;
            fullFrame.ul.pY = incLabelPos + labInfo.height + labInfo.aboveTop;
        }
        else
        {
            incLabelPos = fullFrame.lr.pY - labSpace - labInfo.height;
            if (ticsAtIncs)
                incLabelPos -= incXtra;
            fullFrame.lr.pY = incLabelPos - labInfo.belowBase;
        }

        if (dataTypeLabel)
        {
           if (!objectBase.GetExtendedStringSizeInfo(labelFont, legendTitle, labInfo))
                return;

            if (dataLabelAboveLeft)
            {
                mainLabelPos = fullFrame.ul.pY + labSpace + labInfo.belowBase;
                fullFrame.ul.pY = mainLabelPos + labInfo.height + labInfo.aboveTop;
            }
            else
            {
                mainLabelPos  = fullFrame.lr.pY - labSpace - labInfo.aboveTop - labInfo.height;
                fullFrame.lr.pY = mainLabelPos  - labInfo.belowBase;
            }
        }
    }
    else
    {
        double maxLength = 0.0;
        for (int i = 0; i < incLabels.Size(); i++)
        {
            if (!objectBase.GetStringSizeInfo(legendFont, incLabels[i], labWidth, labHeight))
                return;

            if (labWidth > maxLength)
                maxLength = labWidth;
        }

        if (!objectBase.GetExtendedStringSizeInfo(legendFont, incLabels[0], labInfo))
            return;
        fullFrame.lr.pY -= labInfo.height / 2.0 + labInfo.belowBase;

        if (!objectBase.GetExtendedStringSizeInfo(legendFont, incLabels.LastIndex(), labInfo))
            return;
        fullFrame.ul.pY += labInfo.height / 2.0 + labInfo.aboveTop;

        double labSpace;
        if (!objectBase.GetSpaceIncrement(legendFont, labSpace))
            return;
        labSpace /= 2.0;

        if (incLabelAboveLeft)
        {
            incLabelPos = fullFrame.ul.pX - labSpace;
            if (ticsAtIncs)
                incLabelPos -= incXtra;
            fullFrame.ul.pX = incLabelPos - maxLength;
        }
        else
        {
            incLabelPos = fullFrame.lr.pX + labSpace;
            if (ticsAtIncs)
                incLabelPos += incXtra;
            fullFrame.lr.pX = incLabelPos + maxLength;
        }

        if (dataTypeLabel)
        {
            if (!objectBase.GetExtendedStringSizeInfo(labelFont, legendTitle, labInfo))
                return;

            if (dataLabelAboveLeft)
            {
                mainLabelPos = fullFrame.ul.pX - labSpace;
                if (orientation == PSC_Font::fr_VertLeft)
                {
                    mainLabelPos -= labInfo.belowBase;
                    fullFrame.ul.pX = mainLabelPos - labInfo.height - labInfo.aboveTop;
                }
                else
                {
                    mainLabelPos -= labInfo.height  + labInfo.aboveTop;
                    fullFrame.ul.pX = mainLabelPos - labInfo.belowBase;
                }
            }
            else
            {
                mainLabelPos = fullFrame.lr.pX + labSpace;
                if (orientation == PSC_Font::fr_VertLeft)
                {
                    mainLabelPos += labInfo.height + labInfo.aboveTop;
                    fullFrame.lr.pX = mainLabelPos + labInfo.belowBase;
                }
                else
                {
                    mainLabelPos += labInfo.belowBase;
                    fullFrame.lr.pX = mainLabelPos + labInfo.height + labInfo.aboveTop;
                }
            }
        }
    }

    //  draw frame
    GL_Base::SetColor(legendPen);

    if (frameLegend || opaqueBackground)
    {
        double xInc, yInc;
        if (!objectBase.GetFrameIncrement(legendFont, 0.0, 0.0, xInc, yInc))
            return;
        fullFrame.IncreaseBoundingBox(xInc, yInc);
    }

    GL_Polygon  rectObject;
    rectObject.DrawRectangle(fullFrame, opaqueBackground, frameLegend, frameThick);
}


void GL_ColorLegendSpec :: DrawMainLabel()
{
    //  draw label first to get any codes in effect
    if (dataTypeLabel)
    {
        if (labelFont.IsHorizontal())
        {
            double labelX = 0.5 * (colorBarRect.ul.pX + colorBarRect.lr.pX);
            objectBase.PrintString(labelFont, legendTitle, hta_Center, vta_Bottom, labelX, mainLabelPos);
        }
        else
        {
            double labelY = 0.5 * (colorBarRect.ul.pY + colorBarRect.lr.pY);
            objectBase.PrintString(labelFont, legendTitle, hta_Center, vta_Bottom, mainLabelPos, labelY);
        }
    }

}


void GL_ColorLegendSpec::DrawIncLabels()
{
    //  draw text increments
    GL_Line lineObj;
    double labDelta;
    if (labelFont.IsHorizontal())
        labDelta = colorBarRect.lr.pX - colorBarRect.ul.pX;
    else
        labDelta = colorBarRect.ul.pY - colorBarRect.lr.pY;

    for (int i = 0; i < incLabels.Size(); i++)
    {
        if ((incTVal[i] < -0.0001) || (incTVal[i] > 1.0001))
            continue;

        double barOffset = incTVal[i] * labDelta;

        Point2D ticStart, ticEnd;
        if (labelFont.IsHorizontal())
        {
            ticStart.pX = colorBarRect.ul.pX + barOffset;
            ticEnd.pX   = ticStart.pX;

            if (incLabelAboveLeft)
            {
                ticStart.pY = colorBarRect.lr.pY;
                ticEnd.pY   = colorBarRect.ul.pY + incXtra;
            }
            else
            {
                ticStart.pY = colorBarRect.ul.pY;
                ticEnd.pY   = colorBarRect.lr.pY - incXtra;
            }

        }
        else
        {
            ticStart.pY = colorBarRect.lr.pY + barOffset;
            ticEnd.pY   = ticStart.pY;

            if (incLabelAboveLeft)
            {
                ticStart.pX = colorBarRect.lr.pX;
                ticEnd.pX   = colorBarRect.ul.pX - incXtra;
            }
            else
            {
                ticStart.pX = colorBarRect.ul.pX;
                ticEnd.pX   = colorBarRect.lr.pX + incXtra;
            }
        }

        if (ticsAtIncs)
        {
            // draw line
            GL_Line::SetLine(1);
            lineObj.DrawAnnoLine(ticStart, ticEnd);
        }

        if (labelFont.IsHorizontal())
        {
            objectBase.PrintString(legendFont, incLabels[i], hta_Center, vta_Bottom, ticEnd.pX, incLabelPos);
        }
        else
        {
            if (incLabelAboveLeft)
                objectBase.PrintString(legendFont, incLabels[i], hta_Right, vta_Center, incLabelPos, ticEnd.pY);
            else
                objectBase.PrintString(legendFont, incLabels[i], hta_Left, vta_Center, incLabelPos, ticEnd.pY);
        }
    }

}


void GL_ColorLegendSpec::DrawColorBar()
{
    //  draw color rectangles
    // all filled
    GL_Polygon  rectObject;
    rectObject.SetPolygonFilled(0.0);

    PC_Rectangle  colorRect = colorBarRect;
    double dcolorRect  = barLength / double(colorMap-> ncolorMap);
    if (orientation == PSC_Font::fr_Horiz)
        colorRect.lr.pX = colorRect.ul.pX + dcolorRect;
    else
        colorRect.ul.pY = colorBarRect.lr.pY + dcolorRect;


    for (int i = 0; i < colorMap->ncolorMap; i++)
    {
        GL_Base::SetColor(colorMap->GetColor(i));
        rectObject.DrawFilledRectangle(colorRect);

        if (orientation == PSC_Font::fr_Horiz)
        {
            colorRect.ul.pX = colorRect.lr.pX;
            colorRect.lr.pX += dcolorRect;
        }
        else
        {
            colorRect.lr.pY  = colorRect.ul.pY;
            colorRect.ul.pY += dcolorRect;
        }
    }

    GL_Base::SetColor(legendPen);
    //  draw rectangle around color bar
    rectObject.SetPolygonLine(1, 0.0);
    GL_Line::SetLineType(plt_Solid);
    rectObject.DrawRectangle(colorBarRect);
}

