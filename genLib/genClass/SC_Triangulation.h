///////////////////////////////////////////////////////////////////////////////////
//
// SC_Triangulation.h
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

#ifndef SC_TRIANGULATION_H
#define SC_TRIANGULATION_H

#include <genClass/C_Common.h>
#include <genClass/C_Geometry.h>

#include <genClass/SC_BoolArray.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_IntArray.h>
#include <genClass/SC_PolyStore.h>
#include <genClass/SC_PointArray.h>


class SC_Triangulation  {

public:
    // edge processing class
    class TriangleEdge {
    public:

        //  main members - permanent for each triangulation
        int     stNode;     // st always < end
        int     endNode;
        int     tri1;       // associated triangle index
        int     tri2;       // other assoc triangle.  if < 0, then edge is on outside edge

        //  bookkeeping for node/edge associations
        int     nextEdge;   // index of next edge with same stNode

        //  used as temporary vars by contouring routines
        bool    cPosOK;     // if true then edge is on contour
        double  cPos;       // normalized position of contour on line between st and end
        // only valid if cPosOK

    public:
        TriangleEdge(){endNode = -1; tri1 = -1; tri2 = -1; nextEdge = -1;}

        int     GetOtherTri(int inTri);

    };

    typedef T_SC_Array<TriangleEdge> TriangleEdgeArray;

    enum    RectangleBiasType   {rbtPositive, rbtNegative, rbtCentred};

private:
    const SC_PointArray&    nodePoints;

    GeomMap                 triAssocNode;        // nodes associated with each triangle
    SC_PointArray           triCentroids;        // XY coordinates of triangle centroid

    //  edge list stuff
    SC_IntArray             edgeStartIndexes;   // sized at nNodes, if index >= 0 then pts to start
    TriangleEdgeArray       edgeList;
    GeomMap                 triAssocEdges;      // edges associated with each triangle

    //  node/tri mapping -- just used/created for special interpolations
    GeomMap                 nodeAssocTri;      // tris associated with each node
    int                     maxAssocTri;       // max count used for alloc

public:

    SC_Triangulation(const SC_PointArray& inRef);
    SC_Triangulation(const SC_Triangulation& a);
    ~SC_Triangulation();
    SC_Triangulation&       operator= (const SC_Triangulation& a);


    void                    DeAlloc();

    void                    Delauney(const SC_IntArray& okNodes);           // automatic triangulation on irregular grid
    void                    ResetRectangleBias(RectangleBiasType rbt);      // for special case after Delauney on grid
                                                                            // with many rectangular layouts
                                                                            // sets all tris in rectangles so
                                                                            // cross edge is edge with +ve slope


    void                    BuildFromRegularGrid(int nx,
                                                 int ny);
    void                    BuildFromElements(const GeomMap&  elemMap);
    void                    InternalBlockCleanup(const SC_PolyStore& nodeBlocks);  // blocks used to delete suspect internal triangles
    void                    EdgeBlockCleanup(const SC_PolyStore& nodeBlocks);      // blocks used to delete outside edges which span more than 2 triangles
    void                    BuildEdgeAssociation();                                // must be called after all cleanups are complete
    void                    ApplyReverseMap(const SC_IntArray& nodeReverseMap);    // for subsets after copy
    // reverseMap[i] contains
    // new node number. if -1 then not in new triangulation

    //  status
    bool                    TriangulationOK()                   const {return triCentroids.Size() > 0;}
    int                     GetNTriangles()                     const {return triCentroids.Size();}
    Point2D                 GetTriangleCentroid(int triIndex)   const {return triCentroids[triIndex];}
    int                     GetNEdges()                             const {return edgeList.Size();}


    // access
    void                    GetTriangleNodes(int            triIndex,
                                             SC_IntArray&   triNodeIndexes) const
    {triAssocNode.GetAssocGeom(triIndex, triNodeIndexes);}

    void                    GetConnectedNodes(int            nodeIndex,
                                              SC_IntArray&   connectedNodeIndexes) const;
    // first node is nodeIndex

    void                    GetTriangleBoundaryPoints(int            triIndx,
                                                      SC_PointArray& triNodePoints) const; // assumed allocd to 3

    // gets other node of triangle assoc with edge
    int                     GetEdgeOtherNode(int edgeIndx,
                                             int triIndex);


    // basic contouring support
    bool                    CalculateEdgePoints(const SC_DoubleArray& nodeData,
                                                const double&         cVal,
                                                SC_IntArray&    intersectTri) const;     // sized to nTri, # of edges of tri with intersect

    bool                    GetConnectedEdgeList(const SC_DoubleArray& nodeData,
                                                 const double&         cVal,
                                                 SC_IntArray&    orderedEdges) const;

    const TriangleEdge&     GetEdge(int edgeIndex) const {return edgeList[edgeIndex];}

    void                    GetTriangleEdges(int            triIndex,
                                             SC_IntArray&   triEdgeIndexes) const
    {triAssocEdges.GetAssocGeom(triIndex, triEdgeIndexes);}

    // special interpolation support
    bool                    NodeTriMappingOK()  const;
    void                    CreateNodeTriMapping();
    int                     GetContainingTriangle(const Point2D&    inPoint,            //ret -1 if not OK
                                                  int               nodeIndex) const;
    int                     GetContainingTriangle(const Point2D&    inPoint,            //ret -1 if not OK
                                                  int               nodeIndex,
                                                  SC_PointArray& triPoints,       // should be allocd to 3
                                                  SC_IntArray&   triNode) const;  // should be allocd to 3

    void                    GetNodeCoord(int node2DIndx, Coord3D&  nodeCoord) const;

    // other utility stuff
    void                    GetBoundaryPointIndexes(SC_IntArray& bndIndexes) const;


private:
    void                    LocalCopy(const SC_Triangulation& a);


    void                    AddTriangle(int             triIndex,
                                        SC_IntArray&    assocNodes,
                                        bool            update);

    void                    BuildEdgeList(int maxNode);
    void                    AddEdge(int stNode,
                                    int endNode,
                                    int triIndex);

    int                     GetAssocEdge(int stNode,
                                         int endNode);
    void                    DoCleanup(SC_BoolArray& okNodes);  // if okNode[i] the i is ok

    //  for building connected edge lists
    void                    GetTriangleOKEdges(int triIndex,
                                               int& edge1,
                                               int& edge2) const;

    int                     GetTriangleOtherEdge(int triIndex,
                                                 int edge1) const;

    int                         IsRightTriangle(int triIndex);  // returns index of longest edge
    // -1 if right triangle
    int                         GetOtherNode(int triIndex, int stNode, int endNode);

};

#endif // SC_TRIANGULATION_H

