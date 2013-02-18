///////////////////////////////////////////////////////////////////////////////////
//
// PFO_DataLabelsGL.cpp
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

#include <genClass/SC_DoubleArray.h>
#include <genClass/DO_Real.h>
#include <genClass/U_Str.h>

#include <genPFOGL/PC_Rectangle.h>

#include <genPFOGL/GL_Polygon.h>

#include <genPFOGL/PFO_DataLabelsGL.h>

PFO_DataLabelsGL :: PFO_DataLabelsGL(PlotDefC& assPlt) :
                        PFO_DataLabels(assPlt),
                        OGLBaseObj(this)
{
    InitCommon();
}
PFO_DataLabelsGL::PFO_DataLabelsGL(const PFO_DataLabelsGL& a, PlotDefC& assPlt) :
                        PFO_DataLabels(a, assPlt),
                        OGLBaseObj(this)
{
    InitCommon();
}

void  PFO_DataLabelsGL::InitCommon()
{
    labelDescText.SetStringLen(160);
    labelDescText.Alloc(maxDataLabels);
    labelValText.SetStringLen(160);
    labelValText.Alloc(maxDataLabels);
    labelUnitText.SetStringLen(160);
    labelUnitText.Alloc(maxDataLabels);
}
PFO_DataLabelsGL::~PFO_DataLabelsGL()
{
}

PFO_DataLabelsGL& PFO_DataLabelsGL::operator= (const PFO_DataLabelsGL& a)
{
    if (&a != this)
        PFO_DataLabels::operator=(a);
    return *this;
}

bool PFO_DataLabelsGL::CheckTextSize(const char               inLab[],
                                           ExtendedLabelData& labData,
                                           ExtendedLabelData& maxLabData,
                                           double&            currWidth,
                                           double&            maxWidth)
{
    if (!objectBase->GetExtendedStringSizeInfo(legendFont, inLab, lineMult, labData))
        return false;

    currWidth = labData.width;
    if (currWidth > maxWidth)
        maxWidth = currWidth;

    maxLabData.GetMax(labData);

    return true;
}


void  PFO_DataLabelsGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    //  clear at start
    labelDescText.Clear();
    labelValText.Clear();
    labelUnitText.Clear();
    int i;
    for (i = 0; i < maxDataLabels; i++)
        if (labelData[i].doLabel)
        {
            if (labelData[i].overrideMainLabel)
            {
                labelDescText += labelData[i].mainLabel;
                TrimLeft(labelDescText.LastIndex());
            }
            else
                labelDescText += labelData[i].dataSource->dataTypeLabel;

            labelUnitText += labelData[i].unitsLabel;
            TrimLeft(labelUnitText.LastIndex());

            if (labelData[i].overrideFormat)
            {
                DO_Real* realSource = dynamic_cast <DO_Real*> (labelData[i].dataSource);
                if (realSource != 0)
                {
                    char valStr[80];
                    labelData[i].labelFormat.RealToString(realSource->realVal, valStr, 80);
                    labelValText += valStr;
                    continue;
                }
            }
            labelValText += labelData[i].dataSource->dataValueLabel;
        }

    GL_Base::SetColor(legendPen);

    PC_Rectangle frameLimits;
    frameLimits.ul = ulLocation;
    frameLimits.lr = ulLocation;

    // get basic spacer data
    double xSpace, ySpace;
    if (!objectBase->GetFrameIncrement(legendFont, 0.0, 0.0, xSpace, ySpace))
        return;

    double maxValWidth = 0.0;
    if (minDataFieldWidth > 0)
        maxValWidth = xSpace * double(minDataFieldWidth) * 2.0;

    double descLabWidth[maxDataLabels];
    double valLabWidth[maxDataLabels];
    double unitLabWidth[maxDataLabels];
    double nextRowStart[maxDataLabels];

    double maxDescWidth = 0.0;
    double maxUnitWidth = 0.0;

    lineMult = objectBase->GetDefaultLineMultiplier();

    // label first
    double firstStart = 0.0;
    double firstAboveTop = 0.0;
    bool first = true;
    if (legendTitle[0]  != '\0')
    {
        ExtendedLabelData labData;
        if (!objectBase->GetExtendedStringSizeInfo(legendFont, legendTitle, lineMult, labData))
            return;
//        if (labData.width > maxDescWidth)
//            maxDescWidth = labData.width;

        firstStart = labData.nextYStart;
        frameLimits.lr.pY -= firstStart;
        firstAboveTop = labData.aboveTop;
        first = false;
    }


    ExtendedLabelData maxLabData;
    for (i = 0; i < labelDescText.Size(); i++)
    {
        // just need row maxes
        maxLabData.Clear();

        ExtendedLabelData labData;
        if  (!CheckTextSize(labelDescText[i], labData, maxLabData, descLabWidth[i],  maxDescWidth))
            return;
        if  (!CheckTextSize(labelValText[i],  labData, maxLabData, valLabWidth[i],  maxValWidth))
            return;
        if  (!CheckTextSize(labelUnitText[i], labData, maxLabData, unitLabWidth[i],  maxUnitWidth))
            return;

        if (first)
        {
            firstAboveTop = maxLabData.aboveTop;
            first = false;
        }

        nextRowStart[i] = maxLabData.nextYStart;

        if (i < labelDescText.UpperBound())
            frameLimits.lr.pY -= maxLabData.nextYStart;
    }

    // for last row
    frameLimits.lr.pY -= maxLabData.height;
    if (!(frameLegend||opaqueBackground))
    {
        frameLimits.lr.pY -= maxLabData.belowBase;
        frameLimits.ul.pY += firstAboveTop;
    };

    double descLabXpos = frameLimits.lr.pX;
    frameLimits.lr.pX += maxDescWidth * 1.05;
    double valLabXpos = frameLimits.lr.pX;
    frameLimits.lr.pX += maxValWidth;

    double unitLabXpos = frameLimits.lr.pX;
    // any unit text ?
    if (labelUnitText.GetMaxStringLen() > 0)
    {
        unitLabXpos += xSpace * 3.0;
        frameLimits.lr.pX = unitLabXpos + maxUnitWidth;
    }

    if (frameLegend || opaqueBackground)
    {
        double xInc, yInc;
        if (!objectBase->GetFrameIncrement(legendFont, firstAboveTop, maxLabData.belowBase, xInc, yInc))
            return;
        frameLimits.IncreaseBoundingBox(xInc, yInc);
    }
    GL_Polygon  rectObject;
    rectObject.DrawRectangle(frameLimits, opaqueBackground, frameLegend, frameThick);

    lineMult = objectBase->GetDefaultLineMultiplier();
    double labYstart  = ulLocation.pY;;
    if (legendTitle[0]  != '\0')
    {
        objectBase->PrintString(legendFont, legendTitle, lineMult, descLabXpos, labYstart);
        labYstart  -= firstStart;
    }

    int objIndx = 0;
    for (i = 0; i < maxDataLabels; i++)
        if (labelData[i].doLabel)
        {
            //  write Desc text
            objectBase->PrintString(legendFont, labelDescText[objIndx], lineMult, descLabXpos, labYstart);

            double labXstart = valLabXpos;
            switch (labelData[i].labAlign)
            {
                case hta_Left   : {break;}
                case hta_Center : {labXstart += (maxValWidth - valLabWidth[objIndx]) / 2.0; break;}
                case hta_Right  : {labXstart += (maxValWidth - valLabWidth[objIndx]); break;}
            }
            objectBase->PrintString(legendFont, labelValText[objIndx], lineMult, labXstart, labYstart, labelData[i].labAlign);

            objectBase->PrintString(legendFont, labelUnitText[objIndx], lineMult, unitLabXpos, labYstart);

            labYstart -= nextRowStart[objIndx];
            objIndx++;
        }

    GL_Base::GetAnnoPixelXY(frameLimits.ul, rectLimits.ulXpos, rectLimits.ulYpos);
    GL_Base::GetAnnoPixelXY(frameLimits.lr, rectLimits.lrXpos, rectLimits.lrYpos);
    SetRectOffset(frameLimits.ul);

    CloseDrawObject();
}

