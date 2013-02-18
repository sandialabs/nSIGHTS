///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridContourGL.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genPFOGL/GL_Line.h>
#include <genPFOGL/GL_Extrusion.h>

#include <objPFOGL/PFO_GridContourGL.h>

#include <genPlotClassPS/PC_ReportPS.h>

PFO_GridContourGL :: PFO_GridContourGL(PlotDefC& assPlt) :
    PFO_GridContour(assPlt),
    OGLBaseObj(this)
{
}


PFO_GridContourGL::PFO_GridContourGL(const PFO_GridContourGL& a, PlotDefC& assPlt) :
    PFO_GridContour(a, assPlt),
    OGLBaseObj(this)
{
}

PFO_GridContourGL::~PFO_GridContourGL()
{
}

PFO_GridContourGL& PFO_GridContourGL::operator= (const PFO_GridContourGL& a)
{
    if (&a != this)
        PFO_GridContour::FullCopy(a);
    return *this;
}



void  PFO_GridContourGL::DrawLines()
{
    GL_Line lineObj;
    for (int i = 0; i < contourLines.Size(); i++)
    {
        if (!contourLines[i].IsEmpty())
        {
            lineObj.SetColor(contourSpec[i].contourColor);
            lineObj.SetLine(contourSpec[i].lineThickness);
            lineObj.SetLineType(contourSpec[i].lineType);
            lineObj.SetLineMitreing(contourSpec[i].mitreLines);
            lineObj.DrawLines(contourLines[i]);
        }
    }
    lineObj.SetLineType(plt_Solid);
}

void  PFO_GridContourGL::Draw3DTubes()
{
    PSC_EdgeSpec noEdges;
    noEdges.plotEdges = false;
    for (int i = 0; i < contourLines.Size(); i++)
    {
        if (!contourLines[i].IsEmpty())
        {
            GL_Extrusion tubeExt(polyOffset, contourSpec[i].contourColor, noEdges);
            tubeExt.DrawExtrusion(contourLines[i], tubeData);
        }
    }
}

void  PFO_GridContourGL::DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    if (is3DObject && plotAsTubes)
    {
        Draw3DTubes();
    }
    else
    {
        DrawLines();
    }

    CloseDrawObject();
}


