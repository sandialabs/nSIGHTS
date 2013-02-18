///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ColorDataLegendGL.cpp
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

#include <genPFOGL/PFO_ColorDataLegendGL.h>

PFO_ColorDataLegendGL :: PFO_ColorDataLegendGL(PlotDefC& assPlt) :
                        PFO_ColorDataLegend(assPlt),
                        OGLBaseObj(this)
{
}
PFO_ColorDataLegendGL::PFO_ColorDataLegendGL(const PFO_ColorDataLegendGL& a, PlotDefC& assPlt) :
                        PFO_ColorDataLegend(a, assPlt),
                        OGLBaseObj(this)
{
}
PFO_ColorDataLegendGL::~PFO_ColorDataLegendGL()
{
}

PFO_ColorDataLegendGL& PFO_ColorDataLegendGL::operator= (const PFO_ColorDataLegendGL& a)
{
    PFO_ColorDataLegend::operator=(a);
    return *this;
}


void PFO_ColorDataLegendGL::GetIncVals(SC_StringArray& incLabels,
                                       SC_DoubleArray& incTVal)
{
    mappedColors.Alloc(labelColorMap.Size());
    incLabels.Alloc(labelColorMap.Size());
    incTVal.Alloc(labelColorMap.Size());

    int maxColor = colorMap->ncolorMap;
    int i;
    mappedSelection = -1;
    for (i = 0; i < labelColorMap.Size(); i++)
        if ((labelColorMap[i] >= 0) && (labelColorMap[i] < maxColor))
        {
            if (selectedIndex == i)
                mappedSelection = incLabels.Size();
            incLabels += labelArrayDO->dataLabels[i];
            mappedColors += labelColorMap[i];
        }

    double dT = 1.0 / double(mappedColors.Size());
    incTVal[0] = dT / 2.0;
    for (i = 1; i < mappedColors.Size(); i++)
        incTVal[i] = incTVal[i - 1] + dT;
    incTVal.SetSize(mappedColors.Size());

    if (orientation != PSC_Font::fr_Horiz)
    {
        // tvals reversed
        for (i = 0; i < mappedColors.Size(); i++)
            incTVal[i] = 1.0 - incTVal[i];
    }

}


void PFO_ColorDataLegendGL::DrawColorBar(GL_ColorLegendSpec& inSpec)
{
    //  draw color rectangles
    // all filled

    PC_Rectangle  colorRect = inSpec.colorBarRect;
    double dcolorRect  = barLength / double(mappedColors.Size());
    double dreduce = dcolorRect * (1.0 - reductionFactor) / 2.0;

    PC_Rectangle  selectedRect = inSpec.fullFrame;

    if (orientation == PSC_Font::fr_Horiz)
    {
        selectedRect.ul.pX  = colorRect.ul.pX;
        selectedRect.lr.pX  = colorRect.ul.pX + dcolorRect;

        colorRect.lr.pX = colorRect.ul.pX + dcolorRect - dreduce;
        colorRect.ul.pX += dreduce;
    }
    else
    {
        selectedRect.ul.pY  = colorRect.ul.pY;
        selectedRect.lr.pY  = colorRect.ul.pY - dcolorRect;

        // kluge extra increment
        if (!frameLegend)
        {
            double labSpace;
            if (!objectBase->GetSpaceIncrement(legendFont, labSpace))
                return;
            selectedRect.ul.pX  -= labSpace / 4.0;
            selectedRect.lr.pX  += labSpace / 2.0;
        }

        colorRect.lr.pY = inSpec.colorBarRect.ul.pY - dcolorRect + dreduce;
        colorRect.ul.pY -= dreduce;
    }

    GL_Polygon  rectObject;
    for (int i = 0; i < mappedColors.Size(); i++)
    {
        rectObject.SetPolygonFilled(0.0);
        GL_Base::SetColor(colorMap->GetColor(mappedColors[i]));
        rectObject.DrawFilledRectangle(colorRect);

        if (reductionFactor < 0.999)
        {
            GL_Base::SetColor(legendPen);
            rectObject.SetPolygonLine(1, 0.0);
            rectObject.DrawRectangle(colorRect);
        }

        if (frameSelection && (i == mappedSelection))
        {
            GL_Base::SetColor(legendPen);
            rectObject.DrawRectangle(selectedRect, false, true, frameThick);
        }

        if (orientation == PSC_Font::fr_Horiz)
        {
            colorRect.ul.pX += dcolorRect;
            colorRect.lr.pX += dcolorRect;

            selectedRect.ul.pX  += dcolorRect;
            selectedRect.lr.pX  += dcolorRect;
        }
        else
        {
            colorRect.lr.pY -= dcolorRect;
            colorRect.ul.pY -= dcolorRect;

            selectedRect.ul.pY  -= dcolorRect;
            selectedRect.lr.pY  -= dcolorRect;
        }

    }

    //  draw rectangle around color bar
    if (dreduce < 0.001)
    {
        GL_Base::SetColor(legendPen);
        rectObject.SetPolygonLine(1, 0.0);
        rectObject.DrawRectangle(inSpec.colorBarRect);
    }
}



void  PFO_ColorDataLegendGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    GL_ColorLegendSpec legendSpec(*this, *this, *objectBase);

    GetIncVals(legendSpec.incLabels, legendSpec.incTVal);

    legendSpec.CalculateLegendPosition();
    legendSpec.DrawLegendBase();
    DrawColorBar(legendSpec);
    legendSpec.DrawMainLabel();
    legendSpec.DrawIncLabels();

    GL_Base::GetAnnoPixelXY(legendSpec.fullFrame.ul, rectLimits.ulXpos, rectLimits.ulYpos);
    GL_Base::GetAnnoPixelXY(legendSpec.fullFrame.lr, rectLimits.lrXpos, rectLimits.lrYpos);
    SetRectOffset(legendSpec.fullFrame.ul);

    CloseDrawObject();

}

