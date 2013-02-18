///////////////////////////////////////////////////////////////////////////////////
//
// SC_CoordArray.h
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

#ifndef SC_COORDARRAY_H
#define SC_COORDARRAY_H

#include <genClass/C_Common.h>
#include <genClass/T_SC_Array.h>

namespace genCommonTypes {

    class SC_PointArray;

    class SC_CoordArray: public T_SC_Array<Coord3D> {
    public :
        SC_CoordArray() : T_SC_Array<Coord3D>() {};               //default cons
        SC_CoordArray(int nAll) : T_SC_Array<Coord3D>(nAll) {};   //allocates to size nAll
        SC_CoordArray(Coord3D* buffer, int bufSize):T_SC_Array<Coord3D>(buffer, bufSize) {}

        //  converts points to coord with specified z
        void            ConvertPoints(const SC_PointArray& inPoints,
                                      const double&        pointZ);

        //  converts points to coord with specified z, converts from model to world
        void            ConvertPoints(const SC_PointArray& inPoints,
                                      const double&        pointZ,
                                      Plane3D        modelPlane);
        //  converts coords from model to world
        void            FromModelPlane(Plane3D  modelPlane);

        //  converts world to plot
        void            ToPlotPlane(Plane3D  plotPlane);


        //  type specific methods
        void            Reset(int maxList);         // ReAllocs to maxlist if necc.
        // clears all null coords in list
        void            Clear();

        void            SetNull();  // fills to alloc with nullReal, sets size to alloc

        void            SetCoordZ(const double& zVal);  // sets Z for all coord in list

        void            GetLine(int     indx,             // gets line segments
                                Line3D& segLine) const;   // seg[Size() - 1] goes from end to st

        // adapted from gle extrusion library
        void            NextNonDegeneratePoint(int&         inext,
                                               double&      len,
                                               Coord3D&     diff) const;


        //  gets next sequence of non-null points starting at start, returns false if segStart >= size
        //  or if not non null points found after segstart -- nextSegment is assumed allocd to current size
        bool            GetNextSegment(int&             segStart,
                                       SC_CoordArray&   nextSegment) const;

        void            Offset(const Coord3D&   offsetCoord);  // adds offset point to all elements
        // skips nulls

        //  extracts portion of segment between t and s  (both 0 - 1)
        //  may be single or multiple segment
        //  returns true if line was extracted  (t < 1, s > 0, not a point)
        //  extrSegment is assumed allocd to current size
        bool            ExtractSegment(const double&    tVal,  //  0 - 1
                                       const double&    sVal,  // 0 - 1
                                       SC_CoordArray&   extrSegment) const;

        //  specialized version for continous lines
        //  must be single segment
        //  returns true if line was extracted  (t < 1, s > 0, not a point)
        //  extrSegment is assumed allocd to current size
        bool            ExtractSimpleSegment(const double&    tVal,  //  0 - 1
                                             const double&    sVal,  // 0 - 1
                                             SC_CoordArray&   extrSegment) const;

        //  specialized version for continous lines with total length precalc
        //  must be single segment
        //  returns true if point was extracted  (t < 1, s > 0, not a point)
        //  extrSegment is assumed allocd to current size
        bool            ExtractCoord(const double&    tVal,  //  0 - 1
                                     Coord3D&  tCoord) const;

        //  removes extra in-line points -- assumes no nulls
        void            RemoveInteriorPoints(const double& tolerance);

        //calculate polygon centroid
        Coord3D         CalcPolyCentroid() const;

        double          PolyLength() const;
        // sums n segment lengths - includes segment from 0 to UB

        double          SegmentLength() const;
        // sums n - 1 segment lengths - does NOT includes segment from 0 to UB
    };

    class SC_CoordMatrix : public T_SC_Array<SC_CoordArray> {

    public :

        SC_CoordMatrix() : T_SC_Array<SC_CoordArray>() {};               //default cons
        SC_CoordMatrix(int nAll) : T_SC_Array<SC_CoordArray>(nAll) {};   //allocates to size nAll
        SC_CoordMatrix(int nCols, int nRows);  // does MatrixAllocAndSetSize

        //  type specific methods
        void            MatrixAlloc(int nCols, int nRows);
        void            MatrixSetSize(int nCols, int nRows);
        void            MatrixAllocAndSetSize(int nCols, int nRows);
        void            SetNull();  // fills all rows to alloc with nulls sets size to alloc

    };

    class SC_CoordCube  : public T_SC_Array<SC_CoordMatrix> {

    public :

        SC_CoordCube() : T_SC_Array<SC_CoordMatrix>() {};               //default cons
        SC_CoordCube(int nAll) : T_SC_Array<SC_CoordMatrix>(nAll) {};   //allocates to size nAll

        //  type specific methods
        void            CubeAlloc(int nPage, int nCols, int nRows);
        // allocates nPage SC_CoordMatrix
    };

}

#endif // SC_COORDARRAY_H

