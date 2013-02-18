///////////////////////////////////////////////////////////////////////////////////
//
// SC_PointArray.h
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

#ifndef SC_POINTARRAY_H
#define SC_POINTARRAY_H

#include <genClass/C_Common.h>
#include <genClass/T_SC_Array.h>
#include <genClass/SC_CoordArray.h>

class SC_IntArray;

namespace genCommonTypes {

    class SC_Line2DArray;

    class SC_PointArray: public T_SC_Array<genCommonTypes::Point2D> {

    private:
        //  used by poly intersect routine
        class PolyEdge {
        public:
            Point2D     interPt;
            int         edgeIndx;
            double  dist;

        public:
            PolyEdge() {edgeIndx = 0; dist = 0.0;}
            PolyEdge(const PolyEdge& a);
            PolyEdge&   operator=(const PolyEdge& a);
        };

        class PolyEdgeArray: public T_SC_Array<PolyEdge> {
        public:
            void    SortEdges();
        };


    public:
        SC_PointArray():T_SC_Array<genCommonTypes::Point2D>() {}
        SC_PointArray(const SC_PointArray& a):T_SC_Array<Point2D>(a) {}
        SC_PointArray(int nAll):T_SC_Array<Point2D>(nAll) {}
        SC_PointArray(Point2D* buffer, int bufSize):T_SC_Array<Point2D>(buffer, bufSize) {}

        //  type specific operators
        SC_PointArray&   operator= (const SC_CoordArray& a);  // copy and convert operator

        //  type specific methods
        void            CalcMinMax(Point2D& minPt,
                                   Point2D& maxPt) const;
        void            GetLine(int     indx,             // gets line segments
                                Line2D& segLine) const;   // seg[Size() - 1] goes from end to st

        bool            ConvertLines(const SC_Line2DArray& inLines,
                                     const double&          ptEps);  // sorts lines into points

        bool            ConvertMultiLines(const SC_Line2DArray& inLines,
                                          const double&          ptEps);


        //
        bool            MayOverlap(const Limit2D& limits);  // checks for ambigous overlaps


        void            Clean(const double ptEps = 1E-5); // removes nulls and duplicates -- brute force on dups

        //  algorithms below assume point list is closed polygon  - i.e there is a line
        //  segment connecting points[nPts - 1] and points[0]
        bool            PointInPoly(const double&   xPt,
                                    const double&   yPt) const;
        bool            PointInPoly(const Point2D&   pt) const;
        //  returns true if X,Y is within polygon
        //  assumes polygon is clockwise

        double          CalcPolyArea() const;
        //  area enclosed in polygon
        //  area will be -ve if polygon points are not clockwise

        void            SetPolyClockwise();  // if -ve area then reverses points

        bool            PolyIntersect(const SC_PointArray& poly1,  // ret true if intersect
                                      const SC_PointArray& poly2);

        void            Transpose();  // swaps X and Y

        Point2D         CalcPolyCentroid() const;
        //  returns centroid of poly

        bool            IntersectLine(const Line2D& inLine,             // crops inLine to portion in polygon
                                      Line2D&       outLine,
                                      bool          extendStart,        // if true then extends endpoint
                                      bool          extendEnd) const;   // to polygon border if endpoint is inside

        //  gets next sequence of non-null points starting at start, returns false if segStart >= size
        //  or if not non null points found after segstart -- nextSegment is assumed allocd to current size
        bool            GetNextSegment(int&             segStart,
                                       SC_PointArray&   nextSegment) const;

        void            Offset(const Point2D&   offsetPt);  // adds offset point to all elements

        //  removes extra in-line points -- assumes no nulls
        void            RemoveInteriorPoints(SC_IntArray& pointIndexes, // carries along
                                             const double& tolerance);

    private:

        //  poly intersect support
        void            AddIfNew(const Point2D&  newPt);

        void            CheckPoly2(     int         startEdge,
                                        int         endEdge,
                                        const   SC_PointArray& poly1,
                                        SC_PointArray& addPoly) const;

    };

};

#endif // SC_POINTARRAY_H

