///////////////////////////////////////////////////////////////////////////////////
//
// PC_ShadeTriangle.cpp
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
//      Implements renderer dependent code to actually draw genPFO functional
// objects on a plot.  Is platform independent but
// relies on platform dependent code in genPlotClassPS.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include <genClass/U_Real.h>

#include <genPFOGL/GL_Line.h>

#include <genPFOGL/PC_ShadeTriangle.h>


PC_ShadeTriangle :: PC_ShadeTriangle(const PSC_ColorBase& inRef)
: baseRef(inRef)
{
    triVals.AllocAndSetSize(3);
    triCoords.AllocAndSetSize(3);
    triColors.AllocAndSetSize(3);
    polyVals.AllocAndFill(9, 0.0);
    polyValid.Alloc(9);
    polyCoord.Alloc(9);
}


PC_ShadeTriangle::~PC_ShadeTriangle()
{
}


void PC_ShadeTriangle::SetTriData(        int       index,
                                    const Coord3D& inCoord,
                                    const double&   inVal,
                                          int       inColor)
{
    triCoords[index] = inCoord;
    triVals[index]   = inVal;
    triColors[index] = inColor;
}


bool PC_ShadeTriangle::TriSetupOK()
{
    for (int i = 0; i < 3; i++)
    {
        if (!baseRef.rangeLimit.TransformValueForRange(triVals[i], baseRef.clipToRange))
            return false;

        if (baseRef.oneColor && (triColors[i] != 0))
            return false;
    }
    return true;
}


void PC_ShadeTriangle::DrawTriEdges(const PSC_EdgeSpec& edgeSpec)
{
    if (edgeSpec.plotEdges)
    {
        GL_Base::SetColor(edgeSpec.edgePen);
        polyObj.SetPolygonLine(edgeSpec.edgeThk, edgeSpec.edgePolyOffset);
        polyObj.DrawPolygon(triCoords);
        vertexCount += 3;
    }
}

void PC_ShadeTriangle::DrawOneTri(PSC_ColorBase::ShadeClipSpec clipSpec,
                                  double        clipVal)
{
    int snglColor = triColors[0];
    int minColor = -1;
    int maxColor = -1;
    bool snglOK = true;

    double mapMin = baseRef.rangeLimit.GetMinAct();
    double mapMax = baseRef.rangeLimit.GetMaxAct();
    double dMap = baseRef.rangeLimit.GetdAct();

    double triMin = triVals[0];
    double triMax = triMin;

    const DC_ColorMap& currMap = *baseRef.colorMap;

    if ((clipSpec != PSC_ColorBase::scsNoClip) &&
        (!baseRef.rangeLimit.TransformValueForRange(clipVal, baseRef.clipToRange)))
        clipSpec = PSC_ColorBase::scsNoClip;

    for (int j = 0; j < 3; j++)
    {
        int currColor = triColors[j];
        if (triVals[j] < mapMin)
        {
            currColor = 0;
            if (baseRef.clipToRange)
                snglOK = false;
        }
        else if (triVals[j] > mapMax)
        {
            currColor = currMap.ncolorMap;
            if (baseRef.clipToRange)
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

        DMinMax(triVals[j], triMin, triMax);
    }

    if ((clipSpec == PSC_ColorBase::scsClipAbove) && (clipVal < triMin))
        return;
    if ((clipSpec == PSC_ColorBase::scsClipBelow) && (clipVal > triMax))
        return;

    //  do easy case first
    if ((snglColor >= 0) && snglOK)
    {
        polyObj.SetColor(currMap.GetColor(snglColor));
        if ((clipSpec == PSC_ColorBase::scsNoClip)
            || ((clipSpec == PSC_ColorBase::scsClipAbove) && (clipVal >= triMax))
            || ((clipSpec == PSC_ColorBase::scsClipBelow) && (clipVal <= triMin)))
        {
            polyObj.DrawPolygon(triCoords);
            vertexCount += 3;
        }
        else
        {
            double stVal = triVals[0];
            polyValid.FillToAlloc(false);
            for (int j = 0; j < 3; j++)
            {
                int endIndx = (j + 1) % 3;
                double endVal = triVals[endIndx];

                if (clipSpec == PSC_ColorBase::scsClipAbove)
                {
                    if (stVal < clipVal)
                        AddInterPoint(0.0, j);
                }
                else //scsClipBelow
                {
                    if (stVal > clipVal)
                        AddInterPoint(0.0, j);
                }

                double dEdge = endVal - stVal;
                if (fabs(dEdge) > stdEps)
                    AddInterPoint((clipVal - stVal) / dEdge, j);

                stVal = endVal;
            }

            BuildAndDrawPoly(snglColor);

        }
        return;
    }

    if (minColor > 0)
        minColor--;

    // special case for less than min
    if ((!baseRef.clipToRange) && (minColor == 0))
    {
        polyValid.FillToAlloc(false);
        for (int j = 0; j < 3; j++)
        {
            if (triVals[j] < mapMin)
            {
                if ((clipSpec == PSC_ColorBase::scsNoClip)
                  || ((clipSpec == PSC_ColorBase::scsClipBelow) && (triVals[j] >= clipVal))
                  || ((clipSpec == PSC_ColorBase::scsClipAbove) && (triVals[j] <= clipVal)))
                    AddInterPoint(0.0, j);
            }
            int endIndx = (j + 1) % 3;

            double minVal = mapMin;
            if ((clipSpec == PSC_ColorBase::scsClipBelow) && (minVal < clipVal))
                minVal = clipVal;
            double dEdge = triVals[endIndx] - triVals[j];
            if (fabs(dEdge) > stdEps)
                AddInterPoint((minVal - triVals[j]) / dEdge, j);
        }
        BuildAndDrawPoly(0);
    }

    if (maxColor < currMap.ncolorMap)
        maxColor++;


    for (int k = minColor; k < maxColor; k++)
    {
        double stCVal = mapMin + double(k) * dMap;
        double endCVal = stCVal + dMap;
        if (clipSpec == PSC_ColorBase::scsClipAbove)
        {
            if (stCVal > clipVal)
                return;
            if (endCVal > clipVal)
                endCVal = clipVal;
        }
        else if (clipSpec == PSC_ColorBase::scsClipBelow)
        {
            if (endCVal < clipVal)
                continue;
            if (stCVal < clipVal)
                stCVal = clipVal;
        }

        double stVal = triVals[0];
        polyValid.FillToAlloc(false);
        for (int j = 0; j < 3; j++)
        {
            int endIndx = (j + 1) % 3;
            double endVal = triVals[endIndx];

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
    }

    // special case for >  than max
    if (maxColor == currMap.ncolorMap)
    {
        polyValid.FillToAlloc(false);
        for (int j = 0; j < 3; j++)
        {
            if ((!baseRef.clipToRange) && (triVals[j] > mapMax))
            {
                if ((clipSpec == PSC_ColorBase::scsNoClip)
                  || ((clipSpec == PSC_ColorBase::scsClipBelow) && (triVals[j] >= clipVal))
                  || ((clipSpec == PSC_ColorBase::scsClipAbove) && (triVals[j] <= clipVal)))
                    AddInterPoint(0.0, j);
            }
            int endIndx = (j + 1) % 3;

            double maxVal = mapMax;
            if ((clipSpec == PSC_ColorBase::scsClipAbove) && (maxVal > clipVal))
                maxVal = clipVal;
            double dEdge = triVals[endIndx] - triVals[j];
            if (fabs(dEdge) > stdEps)
                AddInterPoint((maxVal - triVals[j]) / dEdge, j);
        }
        BuildAndDrawPoly(currMap.ncolorMap - 1);
    }
}

void PC_ShadeTriangle::AddInterPoint(const double& xVal,
                                          int     edgeIndex)
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


void PC_ShadeTriangle::CheckInterEdges(int edgeIndex)
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


void  PC_ShadeTriangle::BuildAndDrawPoly(int colorIndx)
{
    for (int j = 0; j < 3; j++)
        CheckInterEdges(j);

    // build polys
    Line3D  edgeLine;
    polyCoord.SetEmpty();
    for (int j = 0; j < 9; j++)
        if (polyValid[j])
        {
            int stIndx = j / 3;
            if ((j % 3) == 0)
                polyCoord += triCoords[stIndx];
            else
            {
                int endIndx = (stIndx + 1) % 3;
                edgeLine.stPt = triCoords[stIndx];
                edgeLine.endPt = triCoords[endIndx];
                polyCoord += edgeLine.PointOnLine(polyVals[j]);
            }
        }

    if (polyCoord.Size() > 2)
    {
        polyObj.SetColor(baseRef.colorMap->GetColor(colorIndx));
        polyObj.DrawPolygon(polyCoord);
        vertexCount += polyCoord.Size();
    }
}

