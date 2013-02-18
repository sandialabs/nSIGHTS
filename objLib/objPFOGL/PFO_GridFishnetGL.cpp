///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridFishnetGL.cpp
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

#include <genPFOGL/GL_Line.h>
#include <genPFOGL/GL_Extrusion.h>

#include <objPFOGL/PFO_GridFishnetGL.h>


PFO_GridFishnetGL :: PFO_GridFishnetGL(PlotDefC& assPlt) :
                            PFO_GridFishnet(assPlt),
                            OGLBaseObj(this)
{
}
PFO_GridFishnetGL::PFO_GridFishnetGL(const PFO_GridFishnetGL& a, PlotDefC& assPlt) :
                            PFO_GridFishnet(a, assPlt),
                            OGLBaseObj(this)
{
}


PFO_GridFishnetGL::~PFO_GridFishnetGL()
{
}

PFO_GridFishnetGL& PFO_GridFishnetGL::operator= (const PFO_GridFishnetGL& a)
{
    if (&a != this)
        PFO_GridFishnet::FullCopy(a);
    return *this;
}

void PFO_GridFishnetGL::Draw2D()
{
    SC_PointArray tempLine(2);
    GL_Line currLine;

    int i;
    tempLine[0].pY = gridDataDC->yData[0];
    tempLine[1].pY = gridDataDC->yData.LastIndex();
    int lastPlotted = 0;
    for (i = 0; i < gridDataDC->xData.Size(); i++)
        if ((i % xplotModulus) == 0)
        {
            tempLine[0].pX = gridDataDC->xData[i];
            tempLine[1].pX = gridDataDC->xData[i];
            currLine.DrawLines(tempLine);
            lastPlotted = i;
        }

    if (plotLastX && (lastPlotted != gridDataDC->xData.UpperBound()))
    {
        tempLine[0].pX = gridDataDC->xData.LastIndex();
        tempLine[1].pX = gridDataDC->xData.LastIndex();
        currLine.DrawLines(tempLine);
    }


    tempLine[0].pX = gridDataDC->xData[0];
    tempLine[1].pX = gridDataDC->xData.LastIndex();
    lastPlotted = 0;
    for (i = 0; i < gridDataDC->yData.Size(); i++)
        if ((i % yplotModulus) == 0)
        {
            tempLine[0].pY = gridDataDC->yData[i];
            tempLine[1].pY = gridDataDC->yData[i];
            currLine.DrawLines(tempLine);
            lastPlotted = i;
        }

    if (plotLastY && (lastPlotted != gridDataDC->yData.UpperBound()))
    {
        tempLine[0].pY = gridDataDC->yData.LastIndex();
        tempLine[1].pY = gridDataDC->yData.LastIndex();
        currLine.DrawLines(tempLine);
    }
}

void PFO_GridFishnetGL::GetXLines(int            xIndex,
                                  SC_CoordArray& xLine)
{
    xLine.Alloc(gridDataDC->yData.Size());
    Coord3D temp;
    double xVal = gridDataDC->xData[xIndex];
    for ( int i = 0; i < gridDataDC->yData.Size(); i++)
    {
        double zVal = gridDataDC->gData[xIndex][i];
        if (RealIsNull(zVal))
        {
            temp = Coord3D();
        }
        else
        {
            temp.cX = xVal;
            temp.cY = gridDataDC->yData[i];
            temp.cZ = zVal;
            coordMap.MapCoords(temp);
        }
        xLine += temp;
    }
}

void PFO_GridFishnetGL::GetYLines(int            yIndex,
                                  SC_CoordArray& yLine)
{
    yLine.Alloc(gridDataDC->xData.Size());
    Coord3D temp;
    double yVal = gridDataDC->yData[yIndex];
    for ( int i = 0; i < gridDataDC->xData.Size(); i++)
    {
        double zVal = gridDataDC->gData[i][yIndex];
        if (RealIsNull(zVal))
        {
            temp = Coord3D();
        }
        else
        {
            temp.cX = gridDataDC->xData[i];
            temp.cY = yVal;
            temp.cZ = zVal;
            coordMap.MapCoords(temp);
        }
        yLine += temp;
    }
}

void PFO_GridFishnetGL::Draw3DData(const SC_CoordArray& data)
{
    if (plotAsTubes)
    {
        PSC_EdgeSpec noEdges;
        noEdges.plotEdges = false;
        GL_Extrusion tubeExt(polyOffset, assocPlot->defaultPenSet->GetColor(gridPen), noEdges);
        tubeExt.DrawExtrusion(data, tubeData);
    }
    else
    {
        GL_Line currLine;
        currLine.DrawLines(data);
    }
}

void PFO_GridFishnetGL::Draw3D()
{
    SC_CoordArray coords;

    int i;
    int lastPlotted = 0;
    for (i = 0; i < gridDataDC->xData.Size(); i++)
        if ((i % xplotModulus) == 0)
        {
            GetXLines(i, coords);
            Draw3DData(coords);
            lastPlotted = i;
        }

    if (plotLastX && (lastPlotted != gridDataDC->xData.UpperBound()))
    {
        GetXLines(gridDataDC->xData.UpperBound(), coords);
        Draw3DData(coords);
    }

    lastPlotted = 0;
    for (i = 0; i < gridDataDC->yData.Size(); i++)
        if ((i % yplotModulus) == 0)
        {
            GetYLines(i, coords);
            Draw3DData(coords);
            lastPlotted = i;
        }

    if (plotLastY && (lastPlotted != gridDataDC->yData.UpperBound()))
    {
        GetYLines(gridDataDC->yData.UpperBound(), coords);
        Draw3DData(coords);
    }


}


void  PFO_GridFishnetGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    GL_Line::SetColor(gridPen);
    GL_Line::SetLine(gridLineThk);
    GL_Line::SetLineType(gridLineType);
    if (is3DObject)
    {
        Draw3D();
    }
    else
    {
        Draw2D();
    }

    CloseDrawObject();
}

