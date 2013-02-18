///////////////////////////////////////////////////////////////////////////////////
//
// SC_PlaneCalcs.h
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

#ifndef SC_PLANECALCS_H
#define SC_PLANECALCS_H

#include <genClass/C_Common.h>
#include <genClass/SC_CoordArray.h>

namespace genCommonTypes {

    class SC_PlaneCalcs: public SC_CoordArray {
    private:
        Coord3D buffer[4];

        mutable     bool isVertical;
        static const double triCalcEps;

        bool    intersectInPoly; // Intersect returns true if point is inside poly
        // default is false

    public:
        //  plane equation of form aX + bY + cZ + d = 0
        mutable     double a;
        mutable     double b;
        mutable     double c;
        mutable     double d;

    public :
        SC_PlaneCalcs();
        SC_PlaneCalcs(int nAlloc);

        inline bool     IsVertical() {return isVertical;}

        bool            CalcPlane(const SC_CoordArray& inTri);  // calcs equation for inTri
        bool            CalcPlane() const;  // assumes coords already set

        double          CalcPlaneZ(const double& planeX, const double& planeY) const;

        double          CalcPlaneZ(const Point2D& planeXY) const;

        void            CalcPlaneZ(Coord3D& planeXYZ) const;  // sets .cZ

        void            SetFromLimit(const Limit3D& inLim,
                                     Plane3D  planeOrientation,
                                     const double&  planeZ);


        //  distance require CalcPlane be called
        double          Distance(const Coord3D& ptXYZ) const;

        //  line intersect require CalcPlane be called

        bool            Intersect(const Line3D& line,
                                  Coord3D& iCoord) const;
        // returns true if iCoord between st and end

        void            IntersectInPolyOnly(bool inPoly); // sets private local

        bool            Intersect(const Line3D& line,
                                  double& tVal) const;
        // returns false if line is parallel to plane
        // or if intersectInPoly is true and intersection is inside
        // tval is between 0 and 1 iff intersect is between st & end

        bool            Intersect(const Coord3D& lineSt,
                                  const Coord3D& lineEnd,
                                  Coord3D& iCoord) const;
        // as above

        bool            Intersect(const Coord3D& lineSt,
                                  const Coord3D& lineEnd,
                                  double&  tVal) const;
        // as above

        // works for tris & quads  -- does tiling to tris
        double          Area();

        // works for tris & quads  -- does tiling to tris
        bool            Intersects(const SC_PlaneCalcs& otherPoly,
                                   const double&        eps = triCalcEps);
        bool            Intersects(const SC_PlaneCalcs& otherPoly,
                                   Coord3D&       intersectPt1,
                                   Coord3D&       intersectPt2,
                                   const double&        eps = triCalcEps);

        bool            PointInPoly(const Coord3D&  inCoord) const;
        // returns true if inCoord is on plane and inside poly

    private:

        static bool         TriIntersect(const Coord3D& V0,
                                         const Coord3D& V1,
                                         const Coord3D& V2,
                                         const Coord3D& U0,
                                         const Coord3D& U1,
                                         const Coord3D& U2,
                                         Coord3D&       isectpt1,
                                         Coord3D&       isectpt2,
                                         const bool     calcIntersectLine,
                                         const double&  eps = triCalcEps);

        // was macro from hell
        static bool     ComputeIntervals(const double& VV0,
                                         const double& VV1,
                                         const double& VV2,
                                         const double& D0,
                                         const double& D1,
                                         const double& D2,
                                         const double& D0D1,
                                         const double& D0D2,
                                         double&       A,
                                         double&       B,
                                         double&       C,
                                         double&       X0,
                                         double&       X1);
        static bool         ComputeIntervalsIsectLine(const Coord3D& V0,
                                                      const Coord3D& V1,
                                                      const Coord3D& V2,
                                                      const double& VV0,
                                                      const double& VV1,
                                                      const double& VV2,
                                                      const double& D0,
                                                      const double& D1,
                                                      const double& D2,
                                                      const double& D0D1,
                                                      const double& D0D2,
                                                      double&       isect0,
                                                      double&       isect1,
                                                      Coord3D&      isectpoint0,
                                                      Coord3D&      isectpoint1);
        static void         Isect2(const Coord3D& VTX0,
                                   const Coord3D& VTX1,
                                   const Coord3D& VTX2,
                                   const double&  VV0,
                                   const double&  VV1,
                                   const double&  VV2,
                                   const double&  D0,
                                   const double&  D1,
                                   const double&  D2,
                                   double&        isect0,
                                   double&        isect1,
                                   Coord3D&      isectpoint0,
                                   Coord3D&      isectpoint1);

        static void         TriSort(double& a, double& b);
        static void         TriSort(double& a, double& b, double& smallest);

        static bool         CoplanarTriTri(const Coord3D& N,
                                           const Coord3D& V0,
                                           const Coord3D& V1,
                                           const Coord3D& V2,
                                           const Coord3D& U0,
                                           const Coord3D& U1,
                                           const Coord3D& U2);

        static bool         EdgeEdgeTest(const double& Ax,
                                         const double& Ay,
                                         const Point2D& V0,
                                         const Point2D& U0,
                                         const Point2D& U1);

        static bool         EdgeAgainstTriEdges(const Point2D& V0,
                                                const Point2D& V1,
                                                const Point2D& U0,
                                                const Point2D& U1,
                                                const Point2D& U2);

        static bool         PointInTri(const Point2D& V0,
                                       const Point2D& U0,
                                       const Point2D& U1,
                                       const Point2D& U2);


    };

}

#endif // SC_PLANECALCS_H

