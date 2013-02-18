///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridColorRangeFillGL.cpp
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
#include <genClass/SC_PlaneCalcs.h>

#include <genClass/SC_Triangulation.h>

#include <genPlotClassPS/PC_ReportPS.h>


#include <objPFOGL/PFO_GridColorRangeFillGL.h>

PFO_GridColorRangeFillGL :: PFO_GridColorRangeFillGL(PlotDefC& assPlt)
    : PFO_GridColorRangeFill(assPlt), OGLBaseObj(this), shadeTri(*this)
{
    InitCommon(assPlt);
}


PFO_GridColorRangeFillGL::PFO_GridColorRangeFillGL(const PFO_GridColorRangeFillGL& a, PlotDefC& assPlt)
    : PFO_GridColorRangeFill(a, assPlt), OGLBaseObj(this), shadeTri(*this)
{
    InitCommon(assPlt);
}

PFO_GridColorRangeFillGL::~PFO_GridColorRangeFillGL()
{
}

PFO_GridColorRangeFillGL& PFO_GridColorRangeFillGL::operator= (const PFO_GridColorRangeFillGL& a)
{
    if (&a != this)
        PFO_GridColorRangeFill::FullCopy(a);
    return *this;
}


void  PFO_GridColorRangeFillGL::InitCommon(PlotDefC& assPlt)
{
    usePolyOffset = true;
    edgeOffset = Coord3D(0.0);
    if (!is3DObject)
    {
        cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
        reportOK     = true;
    }
}

void  PFO_GridColorRangeFillGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

#ifdef _GLPRHARDCOPY_
    if (DoingHardCopy())
    {
        shadeTri.polyObj.SetPolygonLine(edgeSpec.edgeThk, edgeSpec.edgePolyOffset);
        glpBegin(GLP_3D_COLOR, shadeTri.vertexCount);
        objectBase->SetHardCopyParameters();  // for edge line thick
    }
#endif

    // for 2D & non trans case - sets limits for check
    SetupForGetCoord();
    // set Z limits to null for 2D limit check
    if (!is3DObject)
    {
        transLimits.minLim.cZ = nullReal;
        transLimits.maxLim.cZ = nullReal;
    }

    shadeTri.vertexCount = 0;
    int ntri = gridData->GetTriangulation().GetNTriangles();

    for (int i = 0; i < ntri; i++)
    {
        DrawOneTri(i);
    }
    GL_Base::HardCopyBlockEnd();

    CloseDrawObject();
}


void PFO_GridColorRangeFillGL::DrawTransObject(int objIndex)
{
    if (InitDrawObject())
    {
        DrawOneTri(objIndex);
        CloseDrawObject();
    }
}



void PFO_GridColorRangeFillGL::DrawOneTri(int triIndex)
{
    if (!TriLimitsOK(triIndex))
        return;

    for (int i = 0; i < 3; i++)
    {
        int triNode = assocNodes[i];
        shadeTri.SetTriData(i, assocCoord[i], (*gridData)[triNode], gridColors[triNode]);
    }

    if (!shadeTri.TriSetupOK())
        return;

    // edge
    if (edgeSpec.plotEdges)
    {
        if (!is3DObject)
            GL_Base::SetDataLayerOffset(plotLayer);
        GL_Base::SetOffsetCoord(edgeOffset);
        shadeTri.DrawTriEdges(edgeSpec);
        GL_Base::SetOffsetCoord(offsetCoord);
    }

    if (!is3DObject)
        GL_Base::SetDataLayer(plotLayer);
    shadeTri.polyObj.SetPolygonFilled(polyOffset);

    shadeTri.DrawOneTri();
}


void PFO_GridColorRangeFillGL::ReportCursorData(const Coord3D& cursorPos)
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
        cursorReport->ReportLabelSetup(1, "Value:");
    }

    SC_Triangulation& gridTri = gridData->GetTriangulation();
    if (!gridTri.NodeTriMappingOK())
    {
        gridTri.CreateNodeTriMapping();
        if (!gridTri.NodeTriMappingOK())
        {
            cursorReport->ReportUpdate(0, "no tri");
            cursorReport->ReportUpdate(1, "no tri");
            return;
        }
    }

    char    xStr[50];
    char    yStr[50];
    Point2D cursorPt(cursorPos);
    int indx;
    if (gridData->GetIndex(cursorPt, indx))
    {
        CopyString(yStr, "???", 50);

        if (gridData->TransformPoint(cursorPt))
        {
            bool valOK = true;
            SC_PointArray   triPoints(3);
            SC_IntArray     triNodes(3);
            int currTri = gridTri.GetContainingTriangle(cursorPt, indx, triPoints, triNodes);

            if (currTri >= 0)
            {
                SC_PlaneCalcs   planeCoord;
                // set tri Z values
                for (int j = 0; j < 3; j++)
                {
                    double zval = (*gridData)[triNodes[j]];
                    if (RealIsNull(zval) || (rangeLimit.logRange && (zval < stdEps)))
                    {
                        valOK = false;
                        break;
                    }
                    if (rangeLimit.logRange)
                        zval = log10(zval);
                    planeCoord[j] = Coord3D(triPoints[j], zval);
                }

                if (valOK)
                {
                    valOK = planeCoord.CalcPlane() && (!planeCoord.IsVertical());
                    if (valOK)
                    {
                        double yVal = planeCoord.CalcPlaneZ(cursorPt);
                        if (rangeLimit.logRange)
                            yVal = pow(10.0, yVal);
                        reportFormat.RealToString(yVal, yStr, 50);
                    }
                }
            }

        }

        int xIndx, yIndx;
        gridData->GetXYIndex(indx, xIndx, yIndx);

        IntToString(xIndx, xStr, 3, 50);
        ConcatString(xStr, "/", 50);
        ConcatInt(yIndx, xStr, 3, 50);
    }
    else
    {
        CopyString(xStr, "n/a", 50);
        CopyString(yStr, "n/a", 50);
    }

    cursorReport->ReportUpdate(0, xStr);
    cursorReport->ReportUpdate(1, yStr);
}

