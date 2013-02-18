///////////////////////////////////////////////////////////////////////////////////
//
// PFO_CubeColorBlockGL.cpp
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

#include <genPFOGL/GL_Line.h>

#include <objPFOGL/PFO_CubeColorBlockGL.h>


PFO_CubeColorBlockGL :: PFO_CubeColorBlockGL(PlotDefC& assPlt) :
    PFO_CubeColorBlock(assPlt),
    OGLBaseObj(this)
{
    InitCommon(assPlt);
}

PFO_CubeColorBlockGL::PFO_CubeColorBlockGL(const PFO_CubeColorBlockGL& a, PlotDefC& assPlt) :
    PFO_CubeColorBlock(a, assPlt),
    OGLBaseObj(this)
{
    InitCommon(assPlt);
}

PFO_CubeColorBlockGL::~PFO_CubeColorBlockGL()
{
}

PFO_CubeColorBlockGL& PFO_CubeColorBlockGL::operator= (const PFO_CubeColorBlockGL& a)
{
    if (&a != this)
        PFO_CubeColorBlock::FullCopy(a);
    return *this;
}

void  PFO_CubeColorBlockGL::InitCommon(PlotDefC& assPlt)
{
    usePolyOffset = true;
}


void  PFO_CubeColorBlockGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

#ifdef _GLPRHARDCOPY_
    if (DoingHardCopy())
    {
        int glpNum = vertexCount;
        if (edgeSpec.plotEdges)
        {
            glpNum += polygonCount * 2;
            GL_Line::SetLine(edgeSpec.edgeThk);
        }
        glpBegin(GLP_3D_COLOR, glpNum);
        objectBase->SetHardCopyParameters();
    }
    bool doGprGroup = DoingHardCopy() && edgeSpec.plotEdges;
#endif


    polygonObj.SetPolygonFilled(polyOffset);
    SC_CoordArray topCoords, bottomCoords, sides;
    sides.AllocAndSetSize(4);

    vertexCount = 0;
    polygonCount = 0;

    SC_IntArray& currIndx = GetIndexes();

    double polyReduction = reductionFactor;
    if (DoingVRMLOutput())
        polyReduction *= objectBase->GetVRMLEdgeFactor();

    for (int i = 0; i < colorMap->ncolorMap; i++)
    {
        SC_ColorSpec polyColor = colorMap->GetColor(i);

        for (int j = 0; j < currIndx.Size(); j++ )
        {
            if (cubeColors[j] != i)
                continue;

            GetCoords(currIndx[j], topCoords, bottomCoords, polyReduction);

            vertexCount += topCoords.Size() * 6;
            polygonCount += topCoords.Size() + 2;

            // all polys
            polygonObj.SetPolygonFilled(polyOffset);


            // use best approach if not hard copy
            if (!DoingHardCopy())
            {
                polygonObj.SetColor(polyColor);
                polygonObj.DrawPolygon(topCoords, bottomCoords);

                if (edgeSpec.plotEdges)
                {
                    if (DoingVRMLOutput())
                        GetCoords(currIndx[j], topCoords, bottomCoords, reductionFactor);

                    polygonObj.SetColor(edgeSpec.edgePen);
                    polygonObj.SetPolygonLine(edgeSpec.edgeThk, edgeSpec.edgePolyOffset);
                    polygonObj.DrawPolygon(topCoords, bottomCoords);
                }
                continue;
            }


            //  hard copy case only

#ifdef _GLPRHARDCOPY_
            // draw top poly
            if (doGprGroup)
            {
                GL_Polygon::SetColor(edgeSpec.edgePen);
                glpBeginGroup(GLP_OVERLAY_GROUP);
            }

            GL_Polygon::SetColor(polyColor);
            polygonObj.DrawPolygon(topCoords);

            if (doGprGroup)
                glpEndGroup();

            if (doGprGroup)
            {
                GL_Polygon::SetColor(edgeSpec.edgePen);
                glpBeginGroup(GLP_OVERLAY_GROUP);
            }

            GL_Polygon::SetColor(polyColor);
            polygonObj.DrawPolygon(bottomCoords);

            if (doGprGroup)
                glpEndGroup();

            // all sides
            for (int k = 0; k <  topCoords.Size(); k++)
            {
                sides[0] = topCoords[k];
                if (k == 0)
                {
                    sides[1] = topCoords.LastIndex();
                    sides[2] = bottomCoords.LastIndex();
                }
                else
                {
                    sides[1] = topCoords[k - 1];
                    sides[2] = bottomCoords[k - 1];
                }
                sides[3] = bottomCoords[k];

                if (doGprGroup)
                {
                    GL_Polygon::SetColor(edgeSpec.edgePen);
                    glpBeginGroup(GLP_OVERLAY_GROUP);
                }

                GL_Polygon::SetColor(polyColor);
                polygonObj.DrawPolygon(sides);

                if (doGprGroup)
                    glpEndGroup();

            }

#endif
        }
    }

    GL_Base::HardCopyBlockEnd();

    CloseDrawObject();
}


void PFO_CubeColorBlockGL::GetCoords(int            indx,
                                 SC_CoordArray& topCoords,
                                 SC_CoordArray& bottomCoords,
                                 const double&  reduceFactor)
{

    cubeData->GetBlockCoords(indx, topCoords, bottomCoords);

    if (reduceFactor < 0.999)
    {
        Line3D pointLine;
        pointLine.stPt = cubeData->GetCoord(indx);

        for (int k = 0; k < topCoords.Size(); k++)
        {
            pointLine.endPt = topCoords[k];
            topCoords[k] = pointLine.PointOnLine(reduceFactor);
            pointLine.endPt = bottomCoords[k];
            bottomCoords[k] = pointLine.PointOnLine(reduceFactor);
        }
    }
    MapCoords(topCoords);
    MapCoords(bottomCoords);
}

