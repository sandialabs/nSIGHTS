///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableHorsetailGL.cpp
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

#include <objPFOGL/PFO_TableHorsetailGL.h>


PFO_TableHorsetailGL :: PFO_TableHorsetailGL(PlotDefC& assPlt) :
                            PFO_TableHorsetail(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_TableHorsetailGL::PFO_TableHorsetailGL(const PFO_TableHorsetailGL& a, PlotDefC& assPlt) :
                            PFO_TableHorsetail(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
void  PFO_TableHorsetailGL :: InitCommon(PlotDefC& assPlt)
{
    reportOK     = false;
}


PFO_TableHorsetailGL::~PFO_TableHorsetailGL()
{
}

PFO_TableHorsetailGL& PFO_TableHorsetailGL::operator= (const PFO_TableHorsetailGL& a)
{
    if (&a != this)
        PFO_TableHorsetail::operator=(a);
    return *this;
}

void  PFO_TableHorsetailGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    GL_Line currLine;

    currLine.SetLine(lineData.lineThickness);
    currLine.SetLineType(lineData.lineType);
    currLine.SetColor(fixedColorPen);

    int maxCol = inputData->GetNCols();

    int nextPen = 0;
    int maxPen = 0;
    if (plotWithColorMap)
        maxPen = colorMap->ncolorMap;

    for (int i = 0; i < maxCol; i++)
        if (i != selectedCol)
        {
            if (plotWithColorMap)
            {
                currLine.SetColor(colorMap->GetColor(nextPen++));
                nextPen = nextPen % maxPen;
            }
            currLine.DrawLines(*selectedColData, inputData->dataTable[i]);
        }

    CloseDrawObject();
}



