///////////////////////////////////////////////////////////////////////////////////
//
// OGL_2DBase.cpp
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
//      plotting space for 2D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genPlotClassPS/OGL_2DBase.h>

const double  OGL2DBase::zdataMin   = 0.0;
const double  OGL2DBase::zdataMax   = 1.0;
const double  OGL2DBase::zannoMin   = -1.0;
const double  OGL2DBase::zannoMax   = 2.0;

OGL2DBase::OGL2DBase(PD_2D& a) : OGLBase(a), plot2Dbase(a)
{
    currZval  = 0.0;
}


void OGL2DBase::DrawLine(const Coord3D& stPoint,
                         const Coord3D& endPoint) const
{
    if (stPoint.SameCoord(endPoint))
        return;

    glBegin(GL_LINES);
        glVertex3d(GLdouble(stPoint.cX), GLdouble(stPoint.cY), GLdouble(currZval));
        glVertex3d(GLdouble(endPoint.cX), GLdouble(endPoint.cY), GLdouble(currZval));
    glEnd();

}

void OGL2DBase::DrawLine(const Point2D& stPoint,
                         const Point2D& endPoint) const
{
    if (stPoint.SamePoint(endPoint))
        return;

    glBegin(GL_LINES);
        glVertex3d(GLdouble(stPoint.pX), GLdouble(stPoint.pY), GLdouble(currZval));
        glVertex3d(GLdouble(endPoint.pX), GLdouble(endPoint.pY), GLdouble(currZval));
    glEnd();

}


void OGL2DBase::SetDataLayer(int layer)
{
    currZval = zdataMin + double(layer) * (zdataMax - zdataMin) / double(PD_2D::maxPlotLayers);
    rasterZpos = currZval;
}

void  OGL2DBase::SetDataLayerOffset(int layer)
{
    SetDataLayer(layer);
    currZval += (zdataMax - zdataMin) / double(3 * PD_2D::maxPlotLayers);
    rasterZpos = currZval;
}

// scaling for axes independent data like symbols and stroke fonts
Coord3D OGL2DBase::GetIndependentScale(const double& inSize) const
{
    return Coord3D(xPerPixel * inSize, yPerPixel * inSize, 1.0);
}

Point2D  OGL2DBase::GetPixelComponents(int nPixels) const
{
    double inDbl = double(nPixels);
    return Point2D(xPerPixel * inDbl, yPerPixel * inDbl);
}

void OGL2DBase::SetPixelScaling() // pushes existing matrix
{
    Coord3D minL = currView.viewLimits.minLim;
    Coord3D maxL = currView.viewLimits.maxLim;
    TransformForAxes(minL);
    TransformForAxes(maxL);

    int width, height;
    plotBase.CalcAvailablePixels(width, height);
    tranXMin = minL.cX;
    pixelsPerXUnit = double(width) / (maxL.cX - tranXMin);
    tranYMin = minL.cY;
    pixelsPerYUnit = double(height) / (maxL.cY - tranYMin);

    glPushMatrix();
    glTranslated(tranXMin, tranYMin, 0.0);
    glScaled(1.0/ pixelsPerXUnit, 1.0 / pixelsPerYUnit, 1.0);
}

bool OGL2DBase::GetPixelXY(Point2D& inPoint) const
{
    if (!TransformForXY(inPoint))
        return false;

    inPoint.pX = (inPoint.pX - tranXMin) *  pixelsPerXUnit;
    inPoint.pY = (inPoint.pY - tranYMin) *  pixelsPerYUnit;
    return true;
}

bool OGL2DBase::GetPixelXY(Coord3D& inCoord) const
{
    if (!TransformForAxes(inCoord))
        return false;

    inCoord.cX = (inCoord.cX - tranXMin) *  pixelsPerXUnit;
    inCoord.cY = (inCoord.cY - tranYMin) *  pixelsPerYUnit;
    return true;

}


void  OGL2DBase::AxesDrawSetup()
{
    SetDrawColor(plot2Dbase.defaultPenSet->GetColor(plot2Dbase.plotAnnotation.axesDataPen));
    SetLine(plot2Dbase.plotAnnotation.axesLineWidth);
    SetLineSolid();
    SetGridHigh();
}

void  OGL2DBase::MajorGridDrawSetup()
{
    SetDrawColor(plot2Dbase.defaultPenSet->GetColor(plot2Dbase.plotAnnotation.gridLineDataPen));
    SetLine(plot2Dbase.plotAnnotation.majorGridWidth);
    SetLineType(plot2Dbase.plotAnnotation.majorGridLineType);
    if (plot2Dbase.plotAnnotation.gridOverData)
        SetGridHigh();
}
void  OGL2DBase::MajorTicDrawSetup()
{
    SetDrawColor(plot2Dbase.defaultPenSet->GetColor(plot2Dbase.plotAnnotation.axesDataPen));
    SetLine(plot2Dbase.plotAnnotation.majorTicWidth);
}
void  OGL2DBase::MinorGridDrawSetup()
{
    SetDrawColor(plot2Dbase.defaultPenSet->GetColor(plot2Dbase.plotAnnotation.gridLineDataPen));
    SetLine(plot2Dbase.plotAnnotation.minorGridWidth);
    SetLineType(plot2Dbase.plotAnnotation.minorGridLineType);
    if (plot2Dbase.plotAnnotation.gridOverData)
        SetGridHigh();

}
void  OGL2DBase::MinorTicDrawSetup()
{
    SetDrawColor(plot2Dbase.defaultPenSet->GetColor(plot2Dbase.plotAnnotation.axesDataPen));
    SetLine(plot2Dbase.plotAnnotation.minorTicWidth);
}


void OGL2DBase::GetAxesOffsets(Point2D& axOffset,
                               Point2D& labOffset,
                               bool&    axesIsFramed) const
{
    axOffset = GetPixelComponents(plot2Dbase.plotAnnotation.axesOffset);
    labOffset = GetPixelComponents(5);
    axesIsFramed = plot2Dbase.plotAnnotation.framePlot;
}

void OGL2DBase::SetLineAntiAlias()
{
    if (PS_OGLBase::Get2DAntiAliasLines() && (!DoingHardCopy()))
    {
        glEnable(GL_LINE_SMOOTH);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }

}

