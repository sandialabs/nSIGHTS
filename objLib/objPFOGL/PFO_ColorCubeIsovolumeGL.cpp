///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ColorCubeIsovolumeGL.cpp
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
#include <genPFOGL/GL_Polygon.h>

#include <objPFOGL/PFO_ColorCubeIsovolumeGL.h>


PFO_ColorCubeIsovolumeGL :: PFO_ColorCubeIsovolumeGL(PlotDefC& assPlt) :
                        PFO_ColorCubeIsovolume(assPlt),
                        OGLBaseObj(this)
{
}
PFO_ColorCubeIsovolumeGL::PFO_ColorCubeIsovolumeGL(const PFO_ColorCubeIsovolumeGL& a, PlotDefC& assPlt) :
                        PFO_ColorCubeIsovolume(a, assPlt),
                        OGLBaseObj(this)
{
}
PFO_ColorCubeIsovolumeGL::~PFO_ColorCubeIsovolumeGL()
{
}

PFO_ColorCubeIsovolumeGL& PFO_ColorCubeIsovolumeGL::operator= (const PFO_ColorCubeIsovolumeGL& a)
{
    if (&a != this)
        PFO_ColorCubeIsovolume::FullCopy(a);
    return *this;
}

void PFO_ColorCubeIsovolumeGL::DoPoly(int objIndex)
{
    GetTriPoly(objIndex);
    polygonObj.DrawPolygon(isoCalc.polyCoords);
}
void PFO_ColorCubeIsovolumeGL::SetColors()
{
    if (usePen)
        GL_Polygon::SetColor(isothermPen);
    else
        GL_Polygon::SetColor(colorMap->GetColor(cubeColors[0]));
    polygonObj.SetPolygonFilled(polyOffset);
}

void PFO_ColorCubeIsovolumeGL::DrawTransObject(int objIndex)
{
    if (InitDrawObject())
    {
        SetColors();
        DoPoly(objIndex);
        CloseDrawObject();
    }
}

void  PFO_ColorCubeIsovolumeGL::DrawPlotObject()
{
    if (!InitDrawObject())
        return;

#ifdef _GLPRHARDCOPY_
    if (DoingHardCopy())
    {
        int glpNum = vertexCount;
        glpBegin(GLP_3D_COLOR, glpNum);
        objectBase->SetHardCopyParameters();
    }
#endif

    vertexCount = 0;
    SetColors();
    for (int i = 0; i < triToPlot.Size(); i++)
    {
        DoPoly(i);
        vertexCount += isoCalc.polyCoords.Size();
    }
    GL_Base::HardCopyBlockEnd();

    CloseDrawObject();
}




