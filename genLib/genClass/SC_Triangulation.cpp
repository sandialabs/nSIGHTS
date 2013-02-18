///////////////////////////////////////////////////////////////////////////////////
//
// SC_Triangulation.cpp
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
// DESCRIPTION: Generic support classes: triangulates scattered or gridded
//              XY data for contouring
//
///////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genClass/SC_BoolArray.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_Triangulation.h>


int SC_Triangulation::TriangleEdge::GetOtherTri(int inTri)
{
    if (inTri == tri1)
        return tri2;

    //    if (inTri != tri2)
    //        GenAppInternalError("SC_Triangulation::TriangleEdge::GetOtherTri");

    return tri1;
}


// **************************************** main class


SC_Triangulation:: SC_Triangulation(const SC_PointArray& inRef) : nodePoints(inRef)
{
    maxAssocTri = 0;
}

SC_Triangulation:: SC_Triangulation(const SC_Triangulation& a) : nodePoints(a.nodePoints)
{
    LocalCopy(a);
}

SC_Triangulation&  SC_Triangulation::operator= (const SC_Triangulation& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void SC_Triangulation::LocalCopy(const SC_Triangulation& a)
{
    triAssocNode        = a.triAssocNode    ;
    triCentroids        = a.triCentroids    ;
    edgeStartIndexes    = a.edgeStartIndexes;
    edgeList            = a.edgeList        ;
    triAssocEdges       = a.triAssocEdges   ;
    nodeAssocTri        = a.nodeAssocTri    ;
    maxAssocTri         = a.maxAssocTri;
}


SC_Triangulation:: ~SC_Triangulation()
{
    DeAlloc();
}


void SC_Triangulation::DeAlloc()
{
    triAssocNode.DeAlloc();
    triCentroids.DeAlloc();
    edgeStartIndexes.DeAlloc();
    edgeList.DeAlloc();
    triAssocEdges.DeAlloc();
}

void SC_Triangulation::GetTriangleBoundaryPoints(int            triIndx,
                                                 SC_PointArray& triNodePoints) const
{
    SC_IntArray triNodeIndexes;
    triAssocNode.GetBufferedAssocGeom(triIndx, triNodeIndexes);
    triNodePoints[0] = nodePoints[triNodeIndexes[0]];
    triNodePoints[1] = nodePoints[triNodeIndexes[1]];
    triNodePoints[2] = nodePoints[triNodeIndexes[2]];
}


void SC_Triangulation::AddTriangle(int            triIndex,
                                   SC_IntArray&   assocNodes,
                                   bool           update)
{
    SC_PointArray nodePoly(3);
    nodePoly[0] = nodePoints[assocNodes[0]];
    nodePoly[1] = nodePoints[assocNodes[1]];
    nodePoly[2] = nodePoints[assocNodes[2]];

    // make clockwise if necessary
    if (nodePoly.CalcPolyArea() < 0.0)
        {
            // swap 0 and 2
            int tmp = assocNodes[0];
            assocNodes[0] = assocNodes[2];
            assocNodes[2] = tmp;

            Point2D tmpP(nodePoly[0]);
            nodePoly[0] = nodePoly[2];
            nodePoly[2] = tmpP;
        }

    if (update)
        triAssocNode.UpdateAssocGeom(triIndex, assocNodes);
    else
        triAssocNode.SetAssocGeom(triIndex, assocNodes);

    if (nodePoly.CalcPolyArea() < stdEps)
        triCentroids[triIndex] = Point2D();
    else
        triCentroids[triIndex] = nodePoly.CalcPolyCentroid();
}


void SC_Triangulation::Delauney(const SC_IntArray& okNodes)
{

    /*
     *  performs delauney triangulation on node2Dpoints
     *  algorithm provided by Paul Trgina, DES
     *  originally translated from Watson, D.F. ACORD: Automatic contouring of raw data
     *  Computers & Geosciences, 8, 97-101, 1982
     *
     */

    int n = okNodes.Size();
    if (n < 3)
        return;

    int size = 2 * n + 1;
    n +=3;

    SC_IntArray index, istack, x_tmp(size), y_tmp(size);
    index.SetResizable(4 * size);
    istack.SetResizable();
    istack.AllocAndIndex(4 * size);

    SC_DoubleArray x_circum(size * 4), y_circum(size * 4), r2_circum(size * 4), x(n), y(n);

    x[0] = x[1] = -1.0; x[2] = 5.0;
    y[0] = y[2] = -1.0; y[1] = 5.0;
    x_circum[0] = y_circum[0] = 2.0;
    r2_circum[0] = 18.0;

    int ix[3], iy[3];
    ix[0] = ix[1] = 0; ix[2] = 1;
    iy[0] = 1; iy[1] = iy[2] = 2;

    index[0] = 0; index[1] = 1; index[2] = 2;

    double xmin = nodePoints[okNodes[0]].pX;
    double xmax = xmin;
    double ymin = nodePoints[okNodes[0]].pY;
    double ymax = ymin;

    int i, j;
    for (i = 3, j = 0; i < n; i++, j++)
        {
            x[i] = nodePoints[okNodes[j]].pX;
            y[i] = nodePoints[okNodes[j]].pY;
            if (x[i] > xmax)
                xmax = x[i];
            if (x[i] < xmin)
                xmin = x[i];
            if (y[i] > ymax)
                ymax = y[i];
            if (y[i] < ymin)
                ymin = y[i];
        }

    // normalize
    double dx = (xmax - xmin);
    double dy = (ymax - ymin);
    double datax;
    if (dx > dy)
        datax = 1.0 / dx;
    else
        datax = 1.0 / dy;
    for (i = 3; i < n; i++)
        {
            x[i] = (x[i] - xmin) * datax;
            y[i] = (y[i] - ymin) * datax;
        }

    int isp, id;
    isp = id = 1;
    for ( int nuc = 3; nuc < n; nuc++)
        {
            int km = 0;

            // loop through existing 3-tuples
            for (int jt = 0; jt < isp; jt++)
                {
                    int ij = 3 * jt;
                    // test if new data within circumcircle
                    dx = x[nuc] - x_circum[jt];
                    double dsq = r2_circum[jt] - dx * dx;
                    if (dsq < 0.0)
                        continue;

                    dy = y[nuc] - y_circum[jt];
                    dsq -= dy * dy;
                    if (dsq < 0.0)
                        continue;

                    // delete 3-tuple but save edges
                    id--;
                    istack[id] = jt;

                    // add edges to x/y_tmp but delete if already listed.
                    for (i = 0; i < 3; i++)
                        {
                            int l1 = ix[i];
                            int l2 = iy[i];
                            bool done = false;
                            if ( km > 0)
                                {
                                    int kmt = km;
                                    for (j = 0; (!done) && (j < kmt); j++)
                                        {
                                            if (index[ij + l1] != x_tmp[j])
                                                continue;
                                            if (index[ij + l2] != y_tmp[j])
                                                continue;
                                            km--;
                                            if (j >= km)
                                                {
                                                    done = true;
                                                    continue;
                                                }
                                            for (int k = j; k < km; k++)
                                                {
                                                    int k1 = k + 1;
                                                    x_tmp[k] = x_tmp[k1];
                                                    y_tmp[k] = y_tmp[k1];
                                                    done = true;
                                                }
                                        }
                                }
                            if (! done)
                                {
                                    x_tmp[km] = index[ij + l1];
                                    y_tmp[km] = index[ij + l2];
                                    km++;
                                }
                        }
                }

            //  form new 3-tuples
            int maxkt = x_circum.AllocSize();
            int maxistack = istack.AllocSize();


            for ( i = 0; i < km; i++)
                {
                    if (id >= maxistack)
                        return;

                    int kt = istack[id];
                    if (kt >= maxkt)
                        return;

                    int ij = 3 * kt;
                    id++;

                    // calculate the circumcircle centre & radius
                    // squared of ktert[i, *] and place in tetr[kt, *]
                    double det[2][3];
                    for (int jz = 0; jz < 2; jz++)
                        {
                            int i2 = (jz == 0) ? x_tmp[i] : y_tmp[i];
                            det[jz][0] = x[i2] - x[nuc];
                            det[jz][1] = y[i2] - y[nuc];
                            det[jz][2] = 0.5 * (det[jz][0] * (x[i2] + x[nuc]) +
                                                det[jz][1] * (y[i2] + y[nuc]));
                        }

                    double dd = 1.0 / (det[0][0] * det[1][1] - det[0][1] * det[1][0]);

                    x_circum[kt] = (det[0][2] * det[1][1] - det[1][2] * det[0][1]) * dd;
                    y_circum[kt] = (det[0][0] * det[1][2] - det[1][0] * det[0][2]) * dd;

                    dx = x[nuc] - x_circum[kt];
                    dy = y[nuc] - y_circum[kt];
                    r2_circum[kt] = dx * dx + dy * dy;

                    if ((ij + 3) >= index.AllocSize())
                        {
                            index.SetSizeToAlloc();
                            index.ReAlloc(ij * 2 + 1);
                        }

                    index[ij++] = x_tmp[i];
                    index[ij++] = y_tmp[i];
                    index[ij] = nuc;
                }

            isp += 2;

        }

    for ( int jt = i = 0; jt < isp; jt++)
        {
            int ij = 3 * jt;
            if ((index[ij] < 3) || (r2_circum[jt] > 1.0))
                continue;

            index[i++] = index[ij++] - 3;
            index[i++] = index[ij++] - 3;
            index[i++] = index[ij++] - 3;
        }

    size = i/3;

    if (size == 0)
        return;

    triCentroids.AllocAndSetSize(size);
    triAssocNode.Alloc(size, size * 3);

    SC_IntArray assocNodes(3);
    SC_PointArray nodePoly(3);

    int ij = 0;
    for (i = 0; i < size; i++)
        {
            assocNodes[0] = okNodes[index[ij++]];
            assocNodes[1] = okNodes[index[ij++]];
            assocNodes[2] = okNodes[index[ij++]];
            AddTriangle(i, assocNodes, false);
        }

    int maxNode = 0;
    for ( i = 0; i < okNodes.Size(); i++)
        if (okNodes[i] > maxNode)
            maxNode = okNodes[i];

    BuildEdgeList(maxNode);

    nodeAssocTri.DeAlloc();
}

void SC_Triangulation::BuildFromElements(const GeomMap&  elemMap)
{
    int nelem = elemMap.Size();
    int i;
    int ntri = 0;
    for (i = 0; i < nelem; i++)
        {
            int nelPts = elemMap.nassocGeom[i];
            if (nelPts == 3)
                {
                    ntri++;
                }
            else if (nelPts == 4)
                {
                    ntri += 2;
                }
            else
                GenAppInternalError("SC_Triangulation::BuildFromElements");
        }

    triCentroids.AllocAndSetSize(ntri);
    triAssocNode.Alloc(ntri, ntri * 3);

    SC_IntArray assocNodes, tempTri(3);
    tempTri.SetSize(3);
    ntri = 0;

    for (i = 0; i < nelem; i++)
        {
            elemMap.GetBufferedAssocGeom(i, assocNodes);
            if (assocNodes.Size() == 3)
                {
                    AddTriangle(ntri++, assocNodes, false);
                }
            else
                {
                    tempTri[0] = assocNodes[0];
                    tempTri[1] = assocNodes[1];
                    tempTri[2] = assocNodes[2];
                    AddTriangle(ntri++, tempTri, false);

                    tempTri[0] = assocNodes[0];
                    tempTri[1] = assocNodes[2];
                    tempTri[2] = assocNodes[3];
                    AddTriangle(ntri++, tempTri, false);
                }
        }

    BuildEdgeList(nodePoints.Size());
    nodeAssocTri.DeAlloc();
}


void SC_Triangulation::ResetRectangleBias(RectangleBiasType rbt)
{
    int nTri = GetNTriangles();
    SC_BoolArray processed(nTri, false);
    SC_IntArray triNodes(3);

    bool trisChanged = false;
    for (int i = 0; i < nTri; i++)
        {
            if (processed[i])
                continue;

            processed[i] = true;

            int longEdge = IsRightTriangle(i);
            if (longEdge < 0)
                continue;

            int otherTri = edgeList[longEdge].GetOtherTri(i);
            if ((otherTri < 0) || (processed[otherTri]))
                continue;

            int otherLongEdge = IsRightTriangle(otherTri);
            if (otherLongEdge != longEdge)
                {
                    if (otherLongEdge < 0)
                        processed[otherTri] = true;
                    continue;
                }


            processed[otherTri] = true;

            int stNode = edgeList[longEdge].stNode;
            int endNode = edgeList[longEdge].endNode;

            // is slope OK already ?
            // vertical is OK
            double dX = nodePoints[endNode].pX - nodePoints[stNode].pX;
            if (fabs(dX) < 1.0E-06)
                continue;
            double dY = nodePoints[endNode].pY - nodePoints[stNode].pY;

            bool slopePositive = ((fabs(dX) < 1.0E-06) || (dY / dX > 0.0));

            bool rectOK = false;
            switch (rbt) {
            case rbtPositive : {
                rectOK = slopePositive;
                break;
            }
            case rbtNegative : {
                rectOK = ! slopePositive;
                break;
            }
            case rbtCentred : {
                double quadValue = nodePoints[stNode].pX * nodePoints[stNode].pY;
                if (fabs(quadValue) < 1.0E-10)
                    quadValue = nodePoints[endNode].pX * nodePoints[endNode].pY;
                bool quadrantPositive = (quadValue > 0.0);

                rectOK = (slopePositive && (!quadrantPositive)) || ((!slopePositive) && quadrantPositive);
            }
            }

            if (rectOK)
                continue;

            // must reset two tris
            int node1 = GetOtherNode(i, stNode, endNode);
            int node2 = GetOtherNode(otherTri, stNode, endNode);

            // triangle 1
            triNodes[0]     = node1;
            triNodes[1]     = node2;
            triNodes[2]     = stNode;
            AddTriangle(i, triNodes, true);

            // triangle 1
            triNodes[0]     = node1;
            triNodes[1]     = node2;
            triNodes[2]     = endNode;
            AddTriangle(otherTri, triNodes, true);

            trisChanged = true;
        }

    if (trisChanged)
        {
            BuildEdgeList(nodePoints.Size());
            BuildEdgeAssociation();
        }
}


int     SC_Triangulation::IsRightTriangle(int triIndex)
{
    SC_IntArray triNodes;
    SC_DoubleArray  edgeSqrLengths(3);

    triAssocNode.GetBufferedAssocGeom(triIndex, triNodes);

    int endNode, longSt, longEnd;

    int longestEdgeIndex;
    double longestSqrEdge = -1.0;
    for (int j = 0; j < 3; j++)
        {
            int stNode = triNodes[j];
            if (j == 2)
                endNode =  triNodes[0];
            else
                endNode =  triNodes[j + 1];

            edgeSqrLengths[j] = Sqr(nodePoints[stNode].Distance(nodePoints[endNode]));
            if (longestSqrEdge < edgeSqrLengths[j])
                {
                    longestSqrEdge = edgeSqrLengths[j];
                    longestEdgeIndex = j;
                    longSt = stNode;
                    longEnd = endNode;
                }
        }

    edgeSqrLengths[longestEdgeIndex] = 0.0;
    if (!RealsAreSame(longestSqrEdge, edgeSqrLengths[0] + edgeSqrLengths[1] + edgeSqrLengths[2], longestSqrEdge / 1.0E+06))
        return - 1;

    // return  edge index
    int longEdge = GetAssocEdge(longSt, longEnd);
    if (longEdge < 0)
        GenAppInternalError("SC_Triangulation::IsRightTriangle");

    return longEdge;
}

int     SC_Triangulation::GetOtherNode(int triIndex, int stNode, int endNode)
{
    SC_IntArray triNodes;
    triAssocNode.GetBufferedAssocGeom(triIndex, triNodes);
    for (int j = 0; j < 3; j++)
        {
            int otherNode = triNodes[j];
            if ((otherNode != stNode) && (otherNode != endNode))
                return otherNode;
        }

    GenAppInternalError("SC_Triangulation::GetOtherNode");
    return -1;
}

void SC_Triangulation::GetBoundaryPointIndexes(SC_IntArray& bndIndexes) const
{
    int npts = nodePoints.Size();
    SC_IntArray connectedNodeA(npts, -1), connectedNodeB(npts, -1);

    // go through edge list
    int nEdges = edgeList.Size();
    int nextNode = -1;
    for (int i = 0; i < nEdges; i++)
        {
            TriangleEdge& currEdge = edgeList[i];
            if ((currEdge.tri1 < 0) || (currEdge.tri2 < 0))
                {
                    nextNode = currEdge.stNode;
                    if (connectedNodeA[nextNode] < 0)
                        {
                            connectedNodeA[nextNode] = currEdge.endNode;
                        }
                    else
                        {
                            if (connectedNodeB[nextNode] < 0)
                                connectedNodeB[nextNode] = currEdge.endNode;
                        }
                    nextNode = currEdge.endNode;
                    if (connectedNodeA[nextNode] < 0)
                        {
                            connectedNodeA[nextNode] = currEdge.stNode;
                        }
                    else
                        {
                            if (connectedNodeB[nextNode] < 0)
                                connectedNodeB[nextNode] = currEdge.stNode;
                        }
                }
        }

    bndIndexes.Alloc(npts + 1); // extra for tri/rect worst case
    while (nextNode >= 0)
        {
            bndIndexes += nextNode;
            int lastNode = nextNode;
            nextNode = connectedNodeA[lastNode];
            if (nextNode < 0)
                {
                    nextNode = connectedNodeB[lastNode];
                    connectedNodeB[lastNode] = -1;
                }
            else
                connectedNodeA[lastNode] = -1;

            if (nextNode < 0)
                break;

            if (connectedNodeA[nextNode] == lastNode)
                connectedNodeA[nextNode] = -1;
            if (connectedNodeB[nextNode] == lastNode)
                connectedNodeB[nextNode] = -1;

        }

}


void  SC_Triangulation::BuildFromRegularGrid(int nx,
                                             int ny)
{
    int nTri = (nx - 1) * (ny - 1) * 2;
    triCentroids.AllocAndSetSize(nTri);
    triAssocNode.Alloc(nTri, nTri * 3);

    SC_IntArray assocNodes(3);
    SC_PointArray nodePoly(3);

    int colStart = 0;
    int nextColStart = ny;

    nTri = 0;
    for ( int i = 0; i < (nx - 1); i++)
        {
            for ( int j = 0; j < (ny - 1); j++)
                {
                    assocNodes[0] = colStart + j;
                    assocNodes[1] = nextColStart + j + 1;
                    assocNodes[2] = nextColStart + j;
                    AddTriangle(nTri++, assocNodes, false);

                    assocNodes[0] = colStart + j + 1;
                    assocNodes[1] = nextColStart + j + 1;
                    assocNodes[2] = colStart + j;
                    AddTriangle(nTri++, assocNodes, false);
                }
            colStart += ny;
            nextColStart += ny;
        }

    BuildEdgeList(nx * ny);
    nodeAssocTri.DeAlloc();
}

void SC_Triangulation::ApplyReverseMap(const SC_IntArray& nodeReverseMap)
{
    SC_IntArray triReverseMap;
    triAssocNode.MakePrimaryReverseMap(triReverseMap, nodeReverseMap);

    triCentroids.ApplyReverseMap(triReverseMap);
    triAssocNode.ApplyReverseMap(triReverseMap, nodeReverseMap);

    // rebuild edges
    BuildEdgeList(nodeReverseMap.GetMax());
    BuildEdgeAssociation();

    // clear existing association
    nodeAssocTri.DeAlloc();
}


void SC_Triangulation::InternalBlockCleanup(const SC_PolyStore& nodeBlocks)
{
    //  make list of OK edges
    int nEdges = edgeList.Size();

    int nTri = GetNTriangles();
    SC_BoolArray triOK(nTri, true);

    SC_PointArray stBlock;
    SC_PointArray endBlock;
    SC_PointArray otherBlock;
    Line2D nodeLine;
    Line2D resultLine;
    bool allOK = true;
    int i;

    for (i = 0; i < nEdges; i++)
        {
            TriangleEdge& currEdge = edgeList[i];
            // skip edges that are already deleted or are external
            if ((currEdge.tri1 < 0) || (currEdge.tri2 < 0))
                continue;

            // set node line endpoints
            nodeLine.stPt = nodePoints[currEdge.stNode];
            nodeLine.endPt = nodePoints[currEdge.endNode];
            double fullDist = nodeLine.Length();
            double remDist = fullDist;

            //  get blocks for end points
            nodeBlocks.GetPolyPoints(currEdge.stNode, stBlock);
            if (stBlock.IntersectLine(nodeLine, resultLine, false, false))
                remDist -= resultLine.Length();

            nodeBlocks.GetPolyPoints(currEdge.endNode, endBlock);
            if (endBlock.IntersectLine(nodeLine, resultLine, false, false))
                remDist -= resultLine.Length();

            // check for simple case
            if ((remDist / fullDist) < 0.0001)
                continue;

            // get other block for tri 1
            int otherNode = GetEdgeOtherNode(i, currEdge.tri1);
            nodeBlocks.GetPolyPoints(otherNode, otherBlock);
            double otherLength = 0.0;
            if (otherBlock.IntersectLine(nodeLine, resultLine, false, false))
                {
                    otherLength = resultLine.Length();
                    remDist -= otherLength;
                }

            // get other block for tri 2
            otherNode = GetEdgeOtherNode(i, currEdge.tri2);
            nodeBlocks.GetPolyPoints(otherNode, otherBlock);
            if (otherBlock.IntersectLine(nodeLine, resultLine, false, false))
                remDist -= resultLine.Length();

            if ((remDist / fullDist) > 0.05)
                {
                    triOK[currEdge.tri1] = false;
                    if (currEdge.tri2 >= 0)
                        triOK[currEdge.tri2] = false;
                    allOK = false;
                }
        }

    if ( !allOK)
        DoCleanup(triOK);
}

void SC_Triangulation::EdgeBlockCleanup(const SC_PolyStore& nodeBlocks)
{
    //  make list of OK edges
    int nEdges = edgeList.Size();

    SC_BoolArray triOK(GetNTriangles());

    SC_PointArray stBlock;
    SC_PointArray endBlock;
    Line2D nodeLine;
    Line2D resultLine;
    bool allOK;

    // repeat for outside edges until none found
    while (true)
        {
            // reset to OK
            triOK.FillToSize(true);
            allOK = true;
            for (int i = 0; i < nEdges; i++)
                {
                    TriangleEdge& currEdge = edgeList[i];
                    // skip edges that are already deleted or are not outside
                    if ((currEdge.tri1 < 0) || (currEdge.tri2 >= 0))
                        continue;

                    // set node line endpoints
                    nodeLine.stPt = nodePoints[currEdge.stNode];
                    nodeLine.endPt = nodePoints[currEdge.endNode];
                    double fullDist = nodeLine.Length();
                    double remDist = fullDist;

                    nodeBlocks.GetPolyPoints(currEdge.stNode, stBlock);
                    if (stBlock.IntersectLine(nodeLine, resultLine, false, false))
                        remDist -= resultLine.Length();

                    nodeBlocks.GetPolyPoints(currEdge.endNode, endBlock);
                    if (endBlock.IntersectLine(nodeLine, resultLine, false, false))
                        remDist -= resultLine.Length();

                    if ((remDist / fullDist) > 0.01)
                        {
                            triOK[currEdge.tri1] = false;
                            allOK = false;
                        }
                }

            if (allOK)
                return;

            DoCleanup(triOK);
        }
}

void SC_Triangulation::DoCleanup(SC_BoolArray& okNodes)
{
    int size = GetNTriangles();
    int nOK = okNodes.GetSetCount();

    // copy
    GeomMap tempMap(triAssocNode);

    // alloc to new size
    triAssocNode.Alloc(nOK, nOK * 3);

    //  update tri lists
    SC_IntArray triMap(size, -1);

    SC_IntArray assocNodes;
    nOK = 0;
    for (int i = 0; i < size; i++)
        if (okNodes[i])
            {
                triMap[i] = nOK;
                triCentroids[nOK] = triCentroids[i];
                tempMap.GetBufferedAssocGeom(i, assocNodes);
                triAssocNode.SetAssocGeom(nOK, assocNodes);
                nOK++;
            }
    triCentroids.SetSize(nOK);

    //  update edge lists
    for (int i = 0; i < edgeList.Size(); i++)
        {
            TriangleEdge& currEdge = edgeList[i];

            // edge OK ?
            if (currEdge.tri1 < 0)
                continue;

            // update
            currEdge.tri1 = triMap[currEdge.tri1];

            // did 2nd exist
            if (currEdge.tri2 < 0)
                continue;

            // yes so update
            currEdge.tri2 = triMap[currEdge.tri2];

            //  if first was bad, set first to second
            if (currEdge.tri1 < 0)
                {
                    currEdge.tri1 = currEdge.tri2;
                    currEdge.tri2 = -1;
                }
        }

}


// gets other node of triangle assoc with edge
int  SC_Triangulation::GetEdgeOtherNode(int edgeIndx,
                                        int triIndex)
{
    if (triIndex < 0)
        return -1;

    TriangleEdge& currEdge = edgeList[edgeIndx];

    SC_IntArray assocNodes;
    triAssocNode.GetBufferedAssocGeom(triIndex, assocNodes);

    for (int i = 0; i < 3; i++)
        {
            int currNode = assocNodes[i];
            if ((currNode != currEdge.stNode) && (currNode != currEdge.endNode))
                return currNode;
        }
    GenAppInternalError("SC_Triangulation::GetEdgeOtherNode");
    return -1;
}


void  SC_Triangulation::AddEdge(int stNode,
                                int endNode,
                                int triIndex)
{
    // swap nodes if necessary
    if (endNode < stNode)
        {
            int tmp = stNode;
            stNode = endNode;
            endNode = tmp;
        }


    TriangleEdge newEdge;
    newEdge.stNode  = stNode;
    newEdge.endNode = endNode;
    newEdge.tri1    = triIndex;

    int nextE = edgeStartIndexes[stNode];

    if (nextE < 0)
        {
            // first in list for this node
            edgeStartIndexes[stNode] = edgeList.Size();
            edgeList += newEdge;
            return;
        }

    int lastE;
    while (nextE >= 0)
        {
            // does edge match ?
            if (edgeList[nextE].endNode == endNode)
                {
                    // sanity check  - usually indicates two or more nodes with same coordinate
                    if (edgeList[nextE].tri2 >= 0)
                        {
                            using namespace std;
                            cout << "Triangulation problem - multiple nodes at same coord ?? - rectify before using " << endl;
                            cout << "start node " << stNode << " X " << nodePoints[stNode].pX << " Y " << nodePoints[stNode].pY << endl;
                            cout << "end node   " << endNode << " X " << nodePoints[endNode].pX << " Y " << nodePoints[endNode].pY << endl;
                            return;
                            // GenAppInternalError("SC_Triangulation::AddEdge");
                        }
                    edgeList[nextE].tri2 = triIndex;
                    return;
                }
            lastE = nextE;
            nextE = edgeList[nextE].nextEdge;
        }

    // must be new edge so add
    edgeList[lastE].nextEdge = edgeList.Size();
    edgeList += newEdge;
}


int SC_Triangulation::GetAssocEdge(int stNode,
                                   int endNode)
{
    if (stNode > endNode)
        {
            int tmp = stNode;
            stNode = endNode;
            endNode = tmp;
        }

    int nextE = edgeStartIndexes[stNode];
    while (nextE >= 0)
        {
            if (edgeList[nextE].endNode == endNode)
                return nextE;

            nextE = edgeList[nextE].nextEdge;
        }

    return -1;
}

void SC_Triangulation::BuildEdgeList(int maxNode)
{

    edgeStartIndexes.AllocAndFill(maxNode, -1);
    TriangleEdge nullEdge;

    int nTri = GetNTriangles();
    edgeList.AllocAndFill(nTri * 3, nullEdge);
    edgeList.SetEmpty();

    SC_IntArray assocNodes;

    for (int i = 0; i < nTri; i++)
        {
            triAssocNode.GetBufferedAssocGeom(i, assocNodes);
            int stIndx = assocNodes[2];
            for (int j = 0; j < 3; j++)
                {
                    int endIndx = assocNodes[j];
                    AddEdge(stIndx, endIndx, i);
                    stIndx = endIndx;
                }
        }
}


void SC_Triangulation::BuildEdgeAssociation()
{
    int nTri = GetNTriangles();
    triAssocEdges.Alloc(nTri, nTri * 3);
    SC_IntArray assocNodes;
    SC_IntArray assocEdges(3);

    for (int i = 0; i < nTri; i++)
        {
            triAssocNode.GetBufferedAssocGeom(i, assocNodes);
            // for each node get assoc edges
            int endNode;
            for (int j = 0; j < 3; j++)
                {
                    int stNode = assocNodes[j];
                    if (j == 2)
                        endNode =  assocNodes[0];
                    else
                        endNode =  assocNodes[j + 1];

                    assocEdges[j] = GetAssocEdge(stNode, endNode);
                    // sanity check
                    if (assocEdges[j] < 0)
                        GenAppInternalError("SC_Triangulation::BuildEdgeAssociation");
                }
            triAssocEdges.SetAssocGeom(i, assocEdges);
        }
}


bool  SC_Triangulation::CalculateEdgePoints(const SC_DoubleArray& nodeData,
                                            const double&         cVal,
                                            SC_IntArray&    intersectTri) const
{
    intersectTri.AllocAndFill(GetNTriangles(), 0);

    bool interFound = false;
    for (int i = 0; i < edgeList.Size(); i++)
        {
            TriangleEdge& currEdge = edgeList[i];

            // edge OK ?
            if (currEdge.tri1 < 0)
                continue;

            currEdge.cPosOK = false;
            double stVal = nodeData[currEdge.stNode];
            double endVal = nodeData[currEdge.endNode];
            // data OK ?
            if (RealIsNull(stVal) || RealIsNull(endVal))
                {
                    currEdge.cPos = nullReal;
                    continue;
                }
            // calc vars
            currEdge.cPos = (cVal - stVal) / (endVal - stVal);

            // very close to start -- assume start -- may be pathological case to deal with later
            if (fabs(currEdge.cPos) < 1.0E-7)
                {
                    currEdge.cPos = 0.0;
                    currEdge.cPosOK = true;
                }
            else
                currEdge.cPosOK = (currEdge.cPos > 0.0) && (currEdge.cPos < 0.999999);

            if (currEdge.cPosOK)
                {
                    intersectTri[currEdge.tri1]++;
                    if (currEdge.tri2 >= 0)
                        intersectTri[currEdge.tri2]++;
                    interFound = true;
                }

        }
    return interFound;
}


void  SC_Triangulation::GetTriangleOKEdges(int triIndex,
                                           int& edge1,
                                           int& edge2) const
{
    // edges of tri
    SC_IntArray assocEdges;
    triAssocEdges.GetBufferedAssocGeom(triIndex, assocEdges);

    edge1 = -1;
    edge2 = -1;
    for (int i = 0; i < 3; i++)
        if (edgeList[assocEdges[i]].cPosOK)
            if (edge1 < 0)
                edge1 = assocEdges[i];
            else
                {
                    edge2 = assocEdges[i];
                    break;
                }
}

int  SC_Triangulation::GetTriangleOtherEdge(int triIndex,
                                            int edge1)const
{
    int e1, e2;
    GetTriangleOKEdges(triIndex, e1, e2);
    if (edge1 == e1)
        e1 = e2;
    return e1;
}


bool SC_Triangulation::GetConnectedEdgeList(const SC_DoubleArray& nodeData,
                                            const double&         cVal,
                                            SC_IntArray&    orderedEdges)const
{
    SC_IntArray intersectTri;
    if (!CalculateEdgePoints(nodeData, cVal, intersectTri))
        return false;

    int i, j;
    SC_IntArray assocEdges;

    // adjust for pathological cases
    for (i = 0; i < intersectTri.Size(); i++)
        {
            int ninter = intersectTri[i];
            if ((ninter == 2) || (ninter == 0))
                continue;

            // other may be pathological -- first try for one good edge
            // (i.e cPosOK and cPs > 0.0)
            triAssocEdges.GetBufferedAssocGeom(i, assocEdges);

            int okEdgeIndex = -1;
            for (j = 0; j < 3; j++)
                {
                    TriangleEdge& testEdge = edgeList[assocEdges[j]];
                    if (testEdge.cPosOK && (testEdge.cPos > 0.0) && (testEdge.cPos < 1.0))
                        {
                            okEdgeIndex = assocEdges[j];
                            break;
                        }
                }

            //      case #1 -- cval OK on one edge and through opposite node
            if (okEdgeIndex >= 0)
                {
                    // get other edge (either one)
                    int otherEdgeIndex;
                    for (int j = 0; j < 3; j++)
                        if (assocEdges[j] != okEdgeIndex)
                            {
                                otherEdgeIndex = assocEdges[j];
                                break;
                            }

                    TriangleEdge& currEdge = edgeList[otherEdgeIndex];

                    // check to make sure cpos may be OK
                    if (!RealIsNull(currEdge.cPos))
                        {
                            // set to OK
                            currEdge.cPosOK = true;
                            // and set intersect count OK
                            intersectTri[i] = 2;
                        }
                }

            // other pathological case - stNodeVal == endNodeVal == cVal
            // will deal with later

        }


    // compress to valid tris only
    int nlineSegs = 0;
    for (i = 0; i < intersectTri.Size(); i++)
        if (intersectTri[i] == 2)
            intersectTri[nlineSegs++] = i;

    if (nlineSegs == 0)
        return false;

    SC_BoolArray triDone(GetNTriangles(), true);
    for (i = 0; i < nlineSegs; i++)
        triDone[intersectTri[i]] = false;

    //  longest line could have nline segments
    SC_IntArray line1Index(nlineSegs + 1), line2Index(nlineSegs + 1);

    // worst case could have n segs * 2 end points + nsegs delimiters
    orderedEdges.Alloc(nlineSegs * 3);

    while (true)
        {
            //  any tris left to process ?
            int stTri = -1;
            for (i = 0; i < nlineSegs; i++)
                if (!triDone[intersectTri[i]])
                    {
                        stTri = intersectTri[i];
                        break;
                    }
            if (stTri < 0)
                return true;


            // get edges for first triangle
            int lastEdge;
            GetTriangleOKEdges(stTri, line1Index[0], lastEdge);
            line1Index[1] = lastEdge;
            line1Index.SetSize(2);
            triDone[stTri] = true;

            // follow line
            int nextTri = edgeList[lastEdge].GetOtherTri(stTri);
            while ((nextTri >= 0) && (!triDone[nextTri]))
                {
                    triDone[nextTri] = true;
                    lastEdge = GetTriangleOtherEdge(nextTri, lastEdge);
                    line1Index += lastEdge;
                    nextTri = edgeList[lastEdge].GetOtherTri(nextTri);
                }

            // see if there are any other edges attached to the start
            line2Index.SetSize(0);
            lastEdge = line1Index[0];
            nextTri = edgeList[lastEdge].GetOtherTri(stTri);
            while ((nextTri >= 0) && (!triDone[nextTri]))
                {
                    triDone[nextTri] = true;
                    lastEdge = GetTriangleOtherEdge(nextTri, lastEdge);
                    line2Index += lastEdge;
                    nextTri = edgeList[lastEdge].GetOtherTri(nextTri);
                }

            //  build ordered list
            //  if not start, add seperator
            if (!orderedEdges.IsEmpty())
                orderedEdges += -1;

            // start at end of list 2
            for (i = line2Index.Size(); i > 0; i--)
                orderedEdges += line2Index[i - 1];

            // add list 1
            for (i = 0; i < line1Index.Size(); i++)
                orderedEdges += line1Index[i];
        }
}

// special interpolation support
bool SC_Triangulation::NodeTriMappingOK()const
{
    return (nodeAssocTri.Size() > 0);
}
void SC_Triangulation::CreateNodeTriMapping()
{
    // count tri on each node
    nodeAssocTri.Alloc(nodePoints.Size());
    nodeAssocTri.nassocGeom.FillToAlloc(0);
    nodeAssocTri.assocGeomStartIndx.SetSizeToAlloc();

    SC_IntArray assocNodes;

    int nTri = GetNTriangles();
    int i;
    // count all tris
    for (i = 0; i < nTri; i++)
        {
            triAssocNode.GetBufferedAssocGeom(i, assocNodes);
            for (int j = 0; j < 3; j++)
                nodeAssocTri.nassocGeom[assocNodes[j]]++;
        }

    // count and set start indexes
    int fullCount = 0;
    maxAssocTri = 0;
    for (i = 0; i < nodePoints.Size(); i++)
        {
            nodeAssocTri.assocGeomStartIndx[i] = fullCount;
            int nassoc = nodeAssocTri.nassocGeom[i];
            if (nassoc > maxAssocTri)
                maxAssocTri = nassoc;
            fullCount += nassoc;
        }

    // allocate map size
    nodeAssocTri.assocGeomMap.Alloc(fullCount);
    nodeAssocTri.assocGeomMap.FillToAlloc(-1);

    // fill map
    for (i = 0; i < nTri; i++)
        {
            triAssocNode.GetBufferedAssocGeom(i, assocNodes);
            for (int j = 0; j < 3; j++)
                {
                    int nextNode = assocNodes[j];
                    int nextStart = nodeAssocTri.assocGeomStartIndx[nextNode];
                    for (int k = 0; k < nodeAssocTri.nassocGeom[nextNode]; k++)
                        {
                            if (nodeAssocTri.assocGeomMap[nextStart] < 0)
                                {
                                    nodeAssocTri.assocGeomMap[nextStart] = i;
                                    break;
                                }
                            nextStart++;
                        }
                }
        }
}


int  SC_Triangulation::GetContainingTriangle(const Point2D&    inPoint,            //ret -1 if not OK
                                             int               nodeIndex) const
{
    SC_IntArray triNodes(3);
    SC_PointArray triPoints(3);
    return GetContainingTriangle(inPoint, nodeIndex, triPoints, triNodes);
}


void SC_Triangulation::GetConnectedNodes(int            nodeIndex,
                                         SC_IntArray&   connectedNodeIndexes) const
{
    connectedNodeIndexes.Alloc(maxAssocTri * 3);
    connectedNodeIndexes += nodeIndex;

    SC_IntArray assocTri, assocNode;
    nodeAssocTri.GetBufferedAssocGeom(nodeIndex, assocTri);
    for (int i = 0; i < assocTri.Size(); i++)
        {
            triAssocNode.GetBufferedAssocGeom(assocTri[i], assocNode);
            for (int j = 0; j < 3; j++)
                connectedNodeIndexes.AddIfNotFound(assocNode[j]);
        }
}


int SC_Triangulation::GetContainingTriangle(const Point2D&    inPoint,         //ret -1 if not OK
                                            int               nodeIndex,
                                            SC_PointArray&    triPoints,       // should be allocd to 3
                                            SC_IntArray&      triNodes) const
{
    SC_IntArray assocTri;
    nodeAssocTri.GetBufferedAssocGeom(nodeIndex, assocTri);
    for (int i = 0; i < assocTri.Size(); i++)
        {
            GetTriangleBoundaryPoints(assocTri[i], triPoints);
            triPoints.SetPolyClockwise();
            if (triPoints.PointInPoly(inPoint))
                {
                    triAssocNode.GetAssocGeom(assocTri[i], triNodes);
                    return assocTri[i];
                }
        }
    return -1;
}

//int SC_Triangulation::GetContainingTriangle
//            nodeLine.stPt = nodePoints[currEdge.stNode];
//            nodeLine.endPt = nodePoints[currEdge.endNode];


void SC_Triangulation::GetNodeCoord(int node2DIndx, Coord3D&  nodeCoord) const
{
    nodeCoord = nodePoints[node2DIndx];
}

