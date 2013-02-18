///////////////////////////////////////////////////////////////////////////////////
//
// PFO_UserLabelsGL.cpp
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

#include <genPFOGL/GL_Polygon.h>

#include <genPFOGL/PFO_UserLabelsGL.h>

PFO_UserLabelsGL :: PFO_UserLabelsGL(PlotDefC& assPlt)
                        : OGLBaseObj(this), PFO_UserLabels(assPlt)
{
}

PFO_UserLabelsGL::PFO_UserLabelsGL(const PFO_UserLabelsGL& a, PlotDefC& assPlt) :
                        PFO_UserLabels(a, assPlt), OGLBaseObj(this)
{}

PFO_UserLabelsGL::~PFO_UserLabelsGL()
{}

PFO_UserLabelsGL& PFO_UserLabelsGL::operator= (const PFO_UserLabelsGL& a)
{
    if (&a != this)
        PFO_UserLabels::FullCopy(a);
    return *this;
}

void  PFO_UserLabelsGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    GL_Base::SetColor(legendPen);
    PC_Rectangle frameLimits;
    frameLimits.ul = ulLocation;
    frameLimits.lr = ulLocation;

    //  get max width & height
    double maxWidth = 0.0;
    double totHeight = 0.0;
    double labWidth[maxUserLabels];
    double nextStart[maxUserLabels];
    double labHeight, aboveTop, belowBase;
    int i;
    int lastOK = -1;
    double lineMult = objectBase->GetDefaultLineMultiplier();

    for (i = 0; i < maxUserLabels; i++)
        if (labelData[i].doLabel)
        {
            ExtendedLabelData labInfo;

            // kluge for old version compatibility: if empty, then add one blank
            char tempLabel[UserLabel::userLabelLen];
            CopyString(tempLabel, labelData[i].userLabel, UserLabel::userLabelLen);
            if (StringLength(tempLabel) == 0)
                CopyString(tempLabel, " " , UserLabel::userLabelLen);

            if (!objectBase->GetExtendedStringSizeInfo(legendFont, tempLabel, lineMult, labInfo))
                return;

            labWidth[i] = labInfo.width;
            nextStart[i] = labInfo.nextYStart;
            if (labInfo.width > maxWidth)
                maxWidth = labInfo.width;
            totHeight += nextStart[i];

            // only if first row
            if (lastOK < 0)
                aboveTop = labInfo.aboveTop;

            //  keep for last row adjustment
            belowBase = labInfo.belowBase;
            labHeight = labInfo.height;

            lastOK = i;
        }

    // shouldn't happen ...
    if (lastOK < 0)
    {
        CloseDrawObject();
        return;
    }

    // compensate for last
    totHeight -= nextStart[lastOK];
    totHeight += labHeight;

    switch (legendFont.fontRotation) {
        case PSC_Font::fr_Horiz : {
            frameLimits.lr.pY -= totHeight;
            frameLimits.lr.pX += maxWidth;
            break;
          }
        case PSC_Font::fr_VertLeft : {
            frameLimits.ul.pY += maxWidth;
            frameLimits.lr.pX += totHeight;
            break;
          }
        case PSC_Font::fr_VertRight : {
            frameLimits.lr.pY -= maxWidth;
            frameLimits.ul.pX -= totHeight;
            break;
          }
    }

    if (frameLegend || opaqueBackground)
    {
        double xInc, yInc;
        if (!objectBase->GetFrameIncrement(legendFont, aboveTop, belowBase, xInc, yInc))
            return;
        frameLimits.IncreaseBoundingBox(xInc, yInc);
    }
    else
    {
        // adjust box for over/under
        switch (legendFont.fontRotation) {
            case PSC_Font::fr_Horiz : {
                frameLimits.ul.pY += aboveTop;
                frameLimits.lr.pY -= belowBase;
                break;
              }
            case PSC_Font::fr_VertLeft : {
                frameLimits.ul.pX -= aboveTop;
                frameLimits.lr.pX += belowBase;
                break;
            }
            case PSC_Font::fr_VertRight : {
                frameLimits.lr.pX += aboveTop;
                frameLimits.ul.pX -= belowBase;
                break;
            }
        }
    }


    GL_Polygon  rectObject;
    rectObject.DrawRectangle(frameLimits, opaqueBackground, frameLegend, frameThick);
    lineMult = objectBase->GetDefaultLineMultiplier();

    switch (legendFont.fontRotation) {
        case PSC_Font::fr_Horiz : {
            double labYstart  = ulLocation.pY;
            for (i = 0; i < maxUserLabels; i++)
                if (labelData[i].doLabel)
                {
                    double labXstart = ulLocation.pX;
                    switch (labelData[i].labAlign)
                    {
                        case hta_Left   : {break;}
                        case hta_Center : {labXstart += (maxWidth - labWidth[i]) / 2.0; break;}
                        case hta_Right  : {labXstart += (maxWidth - labWidth[i]); break;}
                    }
                    objectBase->PrintString(legendFont, labelData[i].userLabel, lineMult, labXstart, labYstart, labelData[i].labAlign);
                    labYstart -= nextStart[i];
                }
            break;
          }
        case PSC_Font::fr_VertLeft : {
            double labXstart  = ulLocation.pX;
            for (i = 0; i < maxUserLabels; i++)
                if (labelData[i].doLabel)
                {
                    double labYstart = ulLocation.pY;
                    switch (labelData[i].labAlign)
                    {
                        case hta_Left   : {break;}
                        case hta_Center : {labYstart += (maxWidth - labWidth[i]) / 2.0; break;}
                        case hta_Right  : {labYstart += (maxWidth - labWidth[i]); break;}
                    }
                    objectBase->PrintString(legendFont, labelData[i].userLabel, lineMult, labXstart, labYstart);
                    labXstart += nextStart[i];
                }
            break;
          }
        case PSC_Font::fr_VertRight : {
            double labXstart  = ulLocation.pX;
            for (i = 0; i < maxUserLabels; i++)
                if (labelData[i].doLabel)
                {
                    double labYstart = ulLocation.pY;
                    switch (labelData[i].labAlign)
                    {
                        case hta_Left   : {break;}
                        case hta_Center : {labYstart -= (maxWidth - labWidth[i]) / 2.0; break;}
                        case hta_Right  : {labYstart -= (maxWidth - labWidth[i]); break;}
                    }
                    objectBase->PrintString(legendFont, labelData[i].userLabel, lineMult, labXstart, labYstart);
                    labXstart -= nextStart[i];
                }
            break;
          }
    }


    GL_Base::GetAnnoPixelXY(frameLimits.ul, rectLimits.ulXpos, rectLimits.ulYpos);
    GL_Base::GetAnnoPixelXY(frameLimits.lr, rectLimits.lrXpos, rectLimits.lrYpos);
    SetRectOffset(frameLimits.ul);

    CloseDrawObject();
}

