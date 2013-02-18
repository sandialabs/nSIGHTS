///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridColorBlockFillGL.cpp
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

#include <genPlotClassPS/PC_ReportPS.h>

#include <genPFOGL/GL_Line.h>

#include <objPFOGL/PFO_GridColorBlockFillGL.h>


PFO_GridColorBlockFillGL :: PFO_GridColorBlockFillGL(PlotDefC& assPlt) :
    PFO_GridColorBlockFill(assPlt),
    OGLBaseObj(this)
{
    InitCommon(assPlt);
}

PFO_GridColorBlockFillGL::PFO_GridColorBlockFillGL(const PFO_GridColorBlockFillGL& a, PlotDefC& assPlt) :
    PFO_GridColorBlockFill(a, assPlt),
    OGLBaseObj(this)
{
    InitCommon(assPlt);
}

PFO_GridColorBlockFillGL::~PFO_GridColorBlockFillGL()
{
}

PFO_GridColorBlockFillGL& PFO_GridColorBlockFillGL::operator= (const PFO_GridColorBlockFillGL& a)
{
    if (&a != this)
        PFO_GridColorBlockFill::FullCopy(a);
    return *this;
}

void  PFO_GridColorBlockFillGL::InitCommon(PlotDefC& assPlt)
{
    usePolyOffset = true;
    if (!is3DObject)
    {
        cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
        reportOK     = true;
    }
}

void PFO_GridColorBlockFillGL::DrawTransObject(int objIndex)
{
    if (InitDrawObject())
    {
        DrawOneBlock(objIndex);
        CloseDrawObject();
    }
}


void  PFO_GridColorBlockFillGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

#ifdef _GLPRHARDCOPY_
    if (DoingHardCopy())
    {
        int glpNum = gridData->Size() * 4;
        if (edgeSpec.plotEdges)
        {
            glpNum *= 2;
            GL_Line::SetLine(edgeSpec.edgeThk);
        }
        glpBegin(GLP_3D_COLOR, glpNum);
        objectBase->SetHardCopyParameters();
    }
#endif

    for (int i = 0; i < gridData->Size(); i++)
        DrawOneBlock(i);

    GL_Base::HardCopyBlockEnd();

    CloseDrawObject();
}


void PFO_GridColorBlockFillGL::DrawOneBlock(int blockIndex)
{
    if (gridColors[blockIndex] < 0)
        return;

    // all polys
    GetBlockCoords(blockIndex);

    if (!is3DObject)
        GL_Base::SetDataLayer(plotLayer);

#ifdef _GLPRHARDCOPY_
    // draw poly
    if (edgeSpec.plotEdges && DoingHardCopy())
    {
        GL_Polygon::SetColor(edgeSpec.edgePen);
        glpBeginGroup(GLP_OVERLAY_GROUP);
    }
#endif

    // draw filled block
    rectObject.SetColor(colorMap->GetColor(gridColors[blockIndex]));
    rectObject.SetPolygonFilled(polyOffset);
    rectObject.DrawPolygon(blockCoords);

#ifdef _GLPRHARDCOPY_
    if (DoingHardCopy())
    {
        if (edgeSpec.plotEdges)
            glpEndGroup();
        return;
    }
#endif
    // draw edge
    if (edgeSpec.plotEdges)
    {
        if (!is3DObject)
            GL_Base::SetDataLayerOffset(plotLayer);
        rectObject.SetColor(edgeSpec.edgePen);
        rectObject.SetPolygonLine(edgeSpec.edgeThk, edgeSpec.edgePolyOffset);
        rectObject.DrawPolygon(blockCoords);
    }
}

void PFO_GridColorBlockFillGL::ReportCursorData(const Coord3D& cursorPos)
{
    if ((! doPlot) || (!cursorReport->showReport) || (StatusNotOK()))
    {
        //  unmanage if set up
        cursorReport->ReportTitleSetup(GetID());
        return;
    }

    if (!cursorReport->reportInitialized)
    {
        cursorReport->ReportInit("Block:");
        cursorReport->ReportTitleSetup(GetID());
        cursorReport->ReportLabelSetup(0, "Block:");
        cursorReport->ReportLabelSetup(1, "Val:");
    }

    char    xStr[50];
    char    yStr[50];
    Point2D cursorPt(cursorPos);
    int indx;
    if (gridData->GetIndex(cursorPt, indx))
    {
        int xIndx, yIndx;
        gridData->GetXYIndex(indx, xIndx, yIndx);

        IntToString(xIndx, xStr, 3, 50);
        ConcatString(xStr, "/", 50);
        ConcatInt(yIndx, xStr, 3, 50);

        // z value string
        reportFormat.RealToString((*gridData)[indx], yStr, 40);
    }
    else
    {
        CopyString(xStr, "n/a", 50);
        CopyString(yStr, "n/a", 50);
    }

    cursorReport->ReportUpdate(0, xStr);
    cursorReport->ReportUpdate(1, yStr);
}

