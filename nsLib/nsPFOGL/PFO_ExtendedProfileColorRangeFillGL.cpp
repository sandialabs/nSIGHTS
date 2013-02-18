///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ExtendedProfileColorRangeFillGL.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "NsLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>
#include <genClass/SC_PlaneCalcs.h>

#include <genClass/SC_Triangulation.h>

#include <genPlotClassPS/PC_ReportPS.h>


#include <nsPFOGL/PFO_ExtendedProfileColorRangeFillGL.h>

PFO_ExtendedProfileColorRangeFillGL :: PFO_ExtendedProfileColorRangeFillGL(PlotDefC& assPlt) :
    PFO_ExtendedProfileColorRangeFill(assPlt),
    OGLBaseObj(this)
{
    InitCommon(assPlt);
}


PFO_ExtendedProfileColorRangeFillGL::PFO_ExtendedProfileColorRangeFillGL(const PFO_ExtendedProfileColorRangeFillGL& a, PlotDefC& assPlt) :
    PFO_ExtendedProfileColorRangeFill(a, assPlt),
    OGLBaseObj(this)
{
    InitCommon(assPlt);
}

PFO_ExtendedProfileColorRangeFillGL::~PFO_ExtendedProfileColorRangeFillGL()
{
}

PFO_ExtendedProfileColorRangeFillGL& PFO_ExtendedProfileColorRangeFillGL::operator= (const PFO_ExtendedProfileColorRangeFillGL& a)
{
    if (&a != this)
        PFO_ExtendedProfileColorRangeFill::FullCopy(a);
    return *this;
}


void  PFO_ExtendedProfileColorRangeFillGL::InitCommon(PlotDefC& assPlt)
{
    usePolyOffset = true;

    edgeOffset = Coord3D(0.0);
    polyVals.AllocAndFill(9, 0.0);
    polyValid.Alloc(9);
    assocCoord.AllocAndSetSize(3);
    plotCoord.AllocAndSetSize(3);
    polyCoord.Alloc(9);
    if (!is3DObject)
    {
        cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
        reportOK     = true;
    }
}

void  PFO_ExtendedProfileColorRangeFillGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    SC_IntArray     assocNodes(3);
    SC_DoubleArray  assocVals(3);

    double mapMin = rangeLimit.GetMinAct();
    double mapMax = rangeLimit.GetMaxAct();
    double dMap = rangeLimit.GetdAct();


    //  get limits of plot - plotXYZ
    Limit3D currLimits = GetCurrentViewLimits();
    // set Z limits to null for 2D limit check
    if (!is3DObject)
    {
        currLimits.minLim.cZ = nullReal;
        currLimits.maxLim.cZ = nullReal;
    }
    Coord3D nodeCoord;

#ifdef _GLPRHARDCOPY_
    if (DoingHardCopy())
    {
        polyObj.SetPolygonLine(edgeSpec.edgeThk, edgeSpec.edgePolyOffset);
        glpBegin(GLP_3D_COLOR, vertexCount);
        objectBase->SetHardCopyParameters();  // for edge line thick
    }
#endif

    vertexCount = 0;

    SC_Triangulation& gridTri = extendedProfileData->GetTriangulation();

    Coord3D temp;

    bool xLin, yLin, zLin;
    if (!is3DObject)
        GetAxesTypes(xLin, yLin); 
    else
        GetAxesTypes(xLin, yLin, zLin);


    for (int i = 0; i < gridTri.GetNTriangles(); i++)
    {
        gridTri.GetTriangleNodes(i, assocNodes);
        //  check that at least one node of tri is within limits
        bool triOK = false;
        for (int j = 0; j < 3; j++)
        {
            assocCoord[j] = GetCoord(assocNodes[j]);
            temp = assocCoord[j];
            MapCoords(temp);

            if (((!xLin) && (temp.cX < stdEps)) || ((!yLin) && (temp.cY < stdEps))) 
            {
                triOK = false;
                break;
            }

            if (is3DObject && (!zLin) && (temp.cZ < stdEps))
            {
                triOK = false;
                break;
            }

            if (currLimits.WithinLimits(temp))
                triOK = true;
        }

        if (!triOK)
            continue;

        int snglColor = extendedProfileColors[assocNodes[0]];
        bool snglOK = true;
        int minColor = -1;
        int maxColor = -1;
        for (int j = 0; j < 3; j++)
        {
            int triNode = assocNodes[j];
            assocVals[j] = (*extendedProfileData)[triNode];

            //  skip if tri id node is null
            if (RealIsNull(assocVals[j]))
            {
                triOK = false;
                break;
            }

            if (rangeLimit.logRange)
            {
                //  skip tri if log and <= 0.0
                if (assocVals[j] < stdEps)
                {
                    if (clipToRange)
                    {
                        triOK = false;
                        break;
                    }
                    else
                        assocVals[j] = rangeLimit.minLimit;

                }
                assocVals[j] = log10(assocVals[j]);
            }

            int currColor = extendedProfileColors[triNode];
            if (assocVals[j] < mapMin)
            {
                currColor = 0;
                if (clipToRange)
                    snglOK = false;
            }
            else if (assocVals[j] > mapMax)
            {
                currColor = colorMap->ncolorMap;
                if (clipToRange)
                    snglOK = false;
            }

            if (snglColor != currColor)
                snglColor = -1;

            if (minColor < 0)
                minColor = currColor;
            if (currColor < minColor)
                minColor = currColor;
            if (currColor > maxColor)
                maxColor = currColor;
        }

        if (!triOK)
            continue;

        plotCoord = assocCoord;
        MapCoords(plotCoord);

        // edge
        if (edgeSpec.plotEdges)
        {
            if (!is3DObject)
                GL_Base::SetDataLayerOffset(plotLayer);
            polyObj.SetColor(edgeSpec.edgePen);
            polyObj.SetOffsetCoord(edgeOffset);

            polyObj.SetPolygonLine(edgeSpec.edgeThk, edgeSpec.edgePolyOffset);
            polyObj.DrawPolygon(plotCoord);
            vertexCount += 3;
            polyObj.SetOffsetCoord(offsetCoord);
        }

        if (!is3DObject)
            GL_Base::SetDataLayer(plotLayer);
        polyObj.SetPolygonFilled(polyOffset);

        //  do easy case first
        if ((snglColor >= 0) && snglOK)
        {
            polyObj.SetColor(colorMap->GetColor(snglColor));
            polyObj.DrawPolygon(plotCoord);
            vertexCount += 3;
            continue;
        }

        if (minColor > 0)
            minColor--;

        // special case for less than min
        if ((!clipToRange) && (minColor == 0))
        {
            polyValid.FillToAlloc(false);
            for (int j = 0; j < 3; j++)
            {
                if (assocVals[j] < mapMin)
                    AddInterPoint(0.0, j);
                int endIndx = (j + 1) % 3;

                double dEdge = assocVals[endIndx] - assocVals[j];
                if (fabs(dEdge) > stdEps)
                    AddInterPoint((mapMin - assocVals[j]) / dEdge, j);
            }
            BuildAndDrawPoly(0);
        }

        if (maxColor < colorMap->ncolorMap)
            maxColor++;
        double stCVal = mapMin + double(minColor) * dMap;

        for (int k = minColor; k < maxColor; k++)
        {
            double endCVal = stCVal + dMap;
            double stVal = assocVals[0];
            polyValid.FillToAlloc(false);
            for (int j = 0; j < 3; j++)
            {
                int endIndx = (j + 1) % 3;
                double endVal = assocVals[endIndx];

                if ((stVal >= stCVal) && (stVal < endCVal))
                    AddInterPoint(0.0, j);

                double dEdge = endVal - stVal;
                if (fabs(dEdge) > stdEps)
                {
                    AddInterPoint((stCVal - stVal) / dEdge, j);
                    AddInterPoint((endCVal - stVal) / dEdge, j);
                }
                stVal = endVal;
            }
            BuildAndDrawPoly(k);
            stCVal += dMap;
        }

        // special case for >  than max
        if (maxColor == colorMap->ncolorMap)
        {
            polyValid.FillToAlloc(false);
            for (int j = 0; j < 3; j++)
            {
                if ((!clipToRange) && (assocVals[j] > mapMax))
                    AddInterPoint(0.0, j);
                int endIndx = (j + 1) % 3;

                double dEdge = assocVals[endIndx] - assocVals[j];
                if (fabs(dEdge) > stdEps)
                    AddInterPoint((mapMax - assocVals[j]) / dEdge, j);
            }
            BuildAndDrawPoly(colorMap->ncolorMap - 1);
        }

    }
    GL_Base::HardCopyBlockEnd();

    CloseDrawObject();
}



void PFO_ExtendedProfileColorRangeFillGL::AddInterPoint(const double& xVal, int edgeIndex)
{
    if (( xVal < 0.0) || (xVal > 1.0))
        return;

    edgeIndex = edgeIndex % 3;
    int edgeStart = edgeIndex * 3;

    if (xVal > 0.999)
    {
        edgeStart += 3;
        edgeStart %= 9;
    }
    else
        if (xVal > 0.001)
        {
            edgeStart++;
            if (polyValid[edgeStart])
                edgeStart++;
            polyVals[edgeStart] = xVal;
        }
    polyValid[edgeStart] = true;
}

void PFO_ExtendedProfileColorRangeFillGL::CheckInterEdges(int edgeIndex)
{
    // see if both end points are set
    int firstPt = edgeIndex * 3;
    int mid1 = firstPt + 1;
    int mid2 = firstPt + 2;
    int endPt = (firstPt + 3) % 9;

    if (polyValid[firstPt] && polyValid[endPt])
    {
        polyValid[mid1] = false;
        polyValid[mid2] = false;
        return;
    }

    // both mid points set ? -- extra checks required
    if (polyValid[mid1] && polyValid[mid2])
    {
        // swap mid points if mis ordered
        if (polyVals[mid1] > polyVals[mid2])
        {
            double tmp = polyVals[mid1];
            polyVals[mid1] = polyVals[mid2];
            polyVals[mid2] = tmp;
        }

        // check for other pathologies
        if (polyValid[firstPt])
            polyValid[mid1] = false;

        if (polyValid[endPt])
            polyValid[mid2] = false;
    }


}

void  PFO_ExtendedProfileColorRangeFillGL::BuildAndDrawPoly(int colorIndx)
{

    int j;
    for (j = 0; j < 3; j++)
        CheckInterEdges(j);

    // build polys
    Line3D  edgeLine;
    polyCoord.SetEmpty();
    for (j = 0; j < 9; j++)
        if (polyValid[j])
        {
            int stIndx = j / 3;
            if ((j % 3) == 0)
                polyCoord += assocCoord[stIndx];
            else
            {
                int endIndx = (stIndx + 1) % 3;
                edgeLine.stPt = assocCoord[stIndx];
                edgeLine.endPt = assocCoord[endIndx];
                polyCoord += edgeLine.PointOnLine(polyVals[j]);
            }
        }

    if (polyCoord.Size() > 2)
    {
        GL_Polygon polyObj;
        polyObj.SetColor(colorMap->GetColor(colorIndx));
        MapCoords(polyCoord);
        polyObj.DrawPolygon(polyCoord);
        vertexCount += polyCoord.Size();
    }
}


void PFO_ExtendedProfileColorRangeFillGL::ReportCursorData(const Coord3D& cursorPos)
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


    SC_Triangulation& gridTri = extendedProfileData->GetTriangulation();
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
    if (extendedProfileData->GetIndex(cursorPt, indx))
    {
        CopyString(yStr, "???", 50);

        bool valOK = true;
        SC_PointArray   triPoints(3);
        SC_IntArray     triNodes(3);
        int currTri = gridTri.GetContainingTriangle(cursorPt, indx, triPoints, triNodes);

//          IntToString(currTri, xStr, 3, 50);

        if (currTri >= 0)
        {
            SC_PlaneCalcs   planeCoord;
            // set tri Z values
            for (int j = 0; j < 3; j++)
            {
                double zval = (*extendedProfileData)[triNodes[j]];
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


        int xIndx, yIndx;
        extendedProfileData->GetXYIndex(indx, xIndx, yIndx);

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

