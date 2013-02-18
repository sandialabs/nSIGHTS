///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridColorPointGL.cpp
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

#include <genClass/U_Msg.h>

#include <genPlotClassPS/PC_ReportPS.h>

#include <genPFOGL/GL_Symbol.h>

#include <objPFOGL/PFO_GridColorPointGL.h>


PFO_GridColorPointGL :: PFO_GridColorPointGL(PlotDefC& assPlt) :
    PFO_GridColorPoint(assPlt),
    OGLBaseObj(this)
{
    InitCommon(assPlt);
}

PFO_GridColorPointGL::PFO_GridColorPointGL(const PFO_GridColorPointGL& a, PlotDefC& assPlt) :
    PFO_GridColorPoint(a, assPlt),
    OGLBaseObj(this)
{
    InitCommon(assPlt);
}

PFO_GridColorPointGL::~PFO_GridColorPointGL()
{
}

PFO_GridColorPointGL& PFO_GridColorPointGL::operator= (const PFO_GridColorPointGL& a)
{
    if (&a != this)
        PFO_GridColorPoint::FullCopy(a);
    return *this;
}

void  PFO_GridColorPointGL::InitCommon(PlotDefC& assPlt)
{
    usePolyOffset = true;
}

void  PFO_GridColorPointGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    //  set for max
    SC_CoordArray pointCoords(gridData->Size());

    //  set up points for each color map
    Coord3D pointCoord;

    GL_Symbol oglSym;
    oglSym.SetSymScale(pointSymSize);

    for (int i = 0; i < colorMap->ncolorMap; i++)
    {
        //  reset polygon
        pointCoords.SetEmpty();
        //  add points for color
        for (int j = 0; j < gridData->Size(); j++ )
        {
            if (gridColors[j] != i)
                continue;

            pointCoord = GetCoord(j);
            MapCoords(pointCoord);

            pointCoords += pointCoord;
        }

        if (!pointCoords.IsEmpty())
            oglSym.DrawSymbols(pointCoords, edgeSpec, plotLayer, polyOffset, colorMap->GetColor(i), pointSymbol);
    }

    CloseDrawObject();
}


