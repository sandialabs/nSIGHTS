///////////////////////////////////////////////////////////////////////////////////
//
// PFO_XYHorsetailGL.cpp
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

#include <objPFOGL/PFO_XYHorsetailGL.h>


PFO_XYHorsetailGL :: PFO_XYHorsetailGL(PlotDefC& assPlt) :
                            PFO_XYHorsetail(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_XYHorsetailGL::PFO_XYHorsetailGL(const PFO_XYHorsetailGL& a, PlotDefC& assPlt) :
                            PFO_XYHorsetail(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
void  PFO_XYHorsetailGL :: InitCommon(PlotDefC& assPlt)
{
    reportOK     = false;
}


PFO_XYHorsetailGL::~PFO_XYHorsetailGL()
{
}

PFO_XYHorsetailGL& PFO_XYHorsetailGL::operator= (const PFO_XYHorsetailGL& a)
{
    if (&a != this)
        PFO_XYHorsetail::operator=(a);
    return *this;
}

void  PFO_XYHorsetailGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    GL_Line currLine;

    currLine.SetLine(lineData.lineThickness);
    currLine.SetLineType(lineData.lineType);
    currLine.SetColor(fixedColorPen);

    int nextPen = 0;
    int maxPen = 0;
    if (plotWithColorMap)
        maxPen = colorMap->ncolorMap;

    for (int i = 0; i < xyDataDC->Size(); i++)
    {
        if (plotWithColorMap)
        {
            currLine.SetColor(colorMap->GetColor(nextPen++));
            nextPen = nextPen % maxPen;
        }
        currLine.DrawLines((*xyDataDC)[i].xData, (*xyDataDC)[i].yData);
    }

    CloseDrawObject();
}



