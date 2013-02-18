///////////////////////////////////////////////////////////////////////////////////
//
// PC_CalcIsothermPoly.cpp
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
//      Basic plotting objects used by all applications, primarily concerned with plot annotation:
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genPFO/PC_CalcIsothermPoly.h>

PC_CalcIsothermPoly :: PC_CalcIsothermPoly()
: currTriCoords(maxTriVertex), currTriData(maxTriVertex), polyCoords(maxTriEdges)
{
    SetTriType(ttPrism);
}

PC_CalcIsothermPoly :: ~PC_CalcIsothermPoly ()
{
}

void PC_CalcIsothermPoly::SetTriType(TriType inTriType)
{
    triType = inTriType;
    if (triType == ttTrapezoid)
    {
        ntriVertex = 4;
        ntriEdges  = 6;
        ntriFace   = 4;
    }
    else
    {
        ntriVertex = 6;
        ntriEdges = 9;
        ntriFace = 5;
    }
    currTriCoords.SetSize(ntriVertex);
    currTriData.SetSize(ntriVertex);
}

bool PC_CalcIsothermPoly::CalcTriPoly(const double& isoVal)
{
    isoValue = isoVal;

    polyCoords.SetEmpty();

    static const int prismStartVertex[] = {0, 1, 2, 0, 1, 2, 3, 4, 5};
    static const int prismEndVertex[]   = {1, 2, 0, 3, 4, 5, 4, 5, 3};
    static const int trapStartVertex[]  = {0, 1, 2, 0, 1, 2};
    static const int trapEndVertex[]    = {1, 2, 0, 3, 3, 3};

    const int* startVertex = prismStartVertex;
    const int* endVertex = prismEndVertex;
    if (triType == ttTrapezoid)
    {
        startVertex = trapStartVertex;
        endVertex = trapEndVertex;
    }

    tcount = 0;
    for (int j = 0; j < ntriEdges; j++)
        CalcVals(startVertex[j], endVertex[j], j);
    if (tcount < 3)
        return false;

    if (tcount == 3)
    {
        polyCoords.SetSize(3);
        return true;
    }

    nseg = 0;
    if (triType == ttTrapezoid)
    {

        const int face1[] = {0, 1, 2, -1};
        const int face2[] = {0, 4, 3, -1};
        const int face3[] = {1, 5, 4, -1};
        const int face4[] = {2, 3, 5, -1};

        AddEdgeSegments(face1);
        AddEdgeSegments(face2);
        AddEdgeSegments(face3);
        AddEdgeSegments(face4);
    }
    else
    {
        const int topFace[] = {0, 1, 2, -1};
        const int sideFace1[] = {0, 4, 6, 3, -1};
        const int sideFace2[] = {1, 4, 7, 5, -1};
        const int sideFace3[] = {2, 5, 8, 3, -1};
        const int botFace[] = {6, 7, 8, -1};

        AddEdgeSegments(topFace);
        AddEdgeSegments(sideFace1);
        AddEdgeSegments(sideFace2);
        AddEdgeSegments(sideFace3);
        AddEdgeSegments(botFace);
    }
    if (nseg < 4)
    {
        polyCoords.SetEmpty();
        return false;
    }

    // sort edges
    int stEdge = segStart[0];
    polyCoords[0] = cvals[stEdge];
    int nextEdge = segEnd[0];
    polyCoords[1] = cvals[nextEdge];
    segProcessed[0] = true;
    int npoly = 2;
    bool edgeFound = true;
    while (edgeFound)
    {
        edgeFound = false;
        for (int j = 1; j < nseg; j++)
            if (!segProcessed[j])
            {
                if (segStart[j] == nextEdge)
                {
                    nextEdge = segEnd[j];
                    edgeFound = true;
                }
                else if (segEnd[j] == nextEdge)
                {
                    nextEdge = segStart[j];
                    edgeFound = true;
                }
                if (edgeFound)
                {
                    segProcessed[j] = true;
                    polyCoords[npoly] = cvals[nextEdge];
                    npoly++;
                    break;
                }
            }
    }

    if (nextEdge == stEdge)
    {
        // last pt not regd
        npoly--;
        polyCoords.SetSize(npoly);
    }
    return true;
}


void PC_CalcIsothermPoly::AddEdgeSegments(const int edgeList[])
{
    int stEdge = -1;
    for (int i = 0; i < 4; i++)
    {
        int edge = edgeList[i];
        if (edge < 0)
            return;
        if (tok[edge])
        {
            if (stEdge < 0)
                stEdge = edge;
            else
            {
                segStart[nseg] = stEdge;
                segEnd[nseg] = edge;
                segProcessed[nseg] = false;
                nseg++;
                return;
            }
        }
    }
}


void PC_CalcIsothermPoly::CalcVals(int stIndx, int endIndx, int tindx)
{
    double denom = (currTriData[endIndx] - currTriData[stIndx]);
    if (fabs(denom) < stdEps)
    {
        tok[tindx] = false;
        return;
    }

    double tval =(isoValue - currTriData[stIndx])/ denom;

    if (triType == ttPrism)
    {
        // on top must be on line - defer points to sides
        if ((tindx < 3) || (tindx > 5))
            tok[tindx] = (tval > 0.0) && (tval <= 1.0);
        else
            tok[tindx] = (tval > 0.0) && (tval < 1.0);
    }
    else
        tok[tindx] = (tval > 0.0) && (tval <= 1.0);

    if (tok[tindx])
    {
        tLine.stPt = currTriCoords[stIndx];
        tLine.endPt = currTriCoords[endIndx];
        cvals[tindx] = tLine.PointOnLine(tval);
        polyCoords[tcount] = cvals[tindx];
        tcount++;
    }
}


