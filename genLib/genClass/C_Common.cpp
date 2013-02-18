///////////////////////////////////////////////////////////////////////////////////
//
// C_Common.cpp
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
// DESCRIPTION: Platform independent and application independent base classes
//              and support classes.
//              Contains basic 2D (Point2D) and 3D (Coord3D) claseses used for plotting
//
///////////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genClass/SC_PointArray.h>
#include <genClass/SC_CoordArray.h>

#include <genClass/C_Common.h>

namespace genCommonTypes {

    Point2D::Point2D(const Coord3D& a) {pX = a.cX; pY = a.cY;}

    Point2D& Point2D::operator= (const Coord3D& a) {pX = a.cX; pY = a.cY; return*this;}  // type conversion

    bool  Point2D::PointIsNull() const
    {
        return ((pX == nullReal) && (pY == nullReal));
    }

    void  Point2D::ToPlotPlane(const Coord3D& worldCoord,  // maps XYZ coord sys to XY
                               Plane3D  plotPlane)         //according to plane
    //  p3D_XY - coord XY to XY
    //  p3D_XZ - coord X to pX, coord Z to pY
    //  p3D_YZ - coord Y to pX, coord Z to pY
    {
        switch (plotPlane) {
        case p3D_XY: {  pX = worldCoord.cX; pY = worldCoord.cY; break;}
        case p3D_XZ: {  pX = worldCoord.cX; pY = worldCoord.cZ; break;}
        case p3D_YZ: {  pX = worldCoord.cY; pY = worldCoord.cZ; break;}
        }
    }

    bool Point2D::SamePoint(const Point2D& test, const double eps) const
    {
        if (PointIsNull() || test.PointIsNull())
            return false;

        return (fabs(pX - test.pX) < eps) &&
            (fabs(pY - test.pY) < eps);
    }

    double  Point2D::Distance(const Point2D& toPoint) const
    {
        if (PointIsNull() || toPoint.PointIsNull())
            return 0.0;

        double dx = pX - toPoint.pX;
        double dy = pY - toPoint.pY;

        return sqrt(dx * dx + dy * dy);
    }


    bool Coord3D::CoordIsNull() const
    {
        return ((cX == nullReal) && (cY == nullReal) && (cZ == nullReal));
    }


    bool Coord3D::SameCoord(const Coord3D& test, const double eps) const
    {
        if (CoordIsNull() || test.CoordIsNull())
            return false;

        return (fabs(cX - test.cX) < eps) &&
            (fabs(cY - test.cY) < eps) &&
            (fabs(cZ - test.cZ) < eps);
    }

    double Coord3D::Distance(const Coord3D& otherCoord) const
    {
        if (CoordIsNull() || otherCoord.CoordIsNull())
            return 0.0;

        double dx = cX - otherCoord.cX;
        double dy = cY - otherCoord.cY;
        double dz = cZ - otherCoord.cZ;

        return sqrt(dx * dx + dy * dy + dz * dz);
    }

    //  XY and XZ do the same thing in all cases - different names for clarity in use
    //  p3D_XY - XYZ to XYZ
    //  p3D_XZ - in X to out X, in Y to out Z, in Z to out Y

    //  YZ is special case
    //  FromModelPlane(p3D_YZ)
    //      in X to out Y, in Y to out Z, in Z to out X
    //  ToModelPlane(p3D_YZ)
    //      in X to out Z, in Y to out X, in Z to out Y

    //  ToPlotPlane(p3D_YZ)
    //      same as ToModelPlane()
    //      in X to out Z, in Y to out X, in Z to out Y
    //  FromPlotPlane(p3D_YZ)
    //      same as FromModelPlane()
    //      in X to out Y, in Y to out Z, in Z to out X


    void Coord3D::FromModelPlane(Plane3D modelPlane) // converts model coord orientation to world
    //  p3D_XY - XYZ to XYZ
    //  p3D_XZ - in X to out X, in Y to out Z, in Z to out Y
    //  p3D_YZ - in X to out Y, in Y to out Z, in Z to out X
    {
        switch (modelPlane) {
        case p3D_XY: { break;}
        case p3D_XZ: {double inY = cY;
                cY = cZ;
                cZ = inY;
                break;}
        case p3D_YZ: {double inX = cX;
                double inY = cY;
                double inZ = cZ;
                cX = inZ;
                cY = inX;
                cZ = inY;
                break;}
        }
    }

    void Coord3D::ToModelPlane(Plane3D modelPlane) // converts plot to model
    //  p3D_XY - XYZ to XYZ
    //  p3D_XZ - in X to out X, in Y to out Z, in Z to out Y
    //  p3D_YZ - in X to out Z, in Y to out X, in Z to out Y
    {
        //  ToPlotPlane(p3D_YZ)
        switch (modelPlane) {
        case p3D_XY: { break;}
        case p3D_XZ: {double inY = cY;
                cY = cZ;
                cZ = inY;
                break;}
        case p3D_YZ: {double inX = cX;
                double inY = cY;
                double inZ = cZ;
                cX = inY;
                cY = inZ;
                cZ = inX;
                break;}
        }
    }

    void Coord3D::ToPlotPlane(Plane3D plotPlane) // converts plot to model
    {
        ToModelPlane(plotPlane);
    }


    void Coord3D::FromPlotPlane(Plane3D plotPlane)
    {
        FromModelPlane(plotPlane);
    }

    double Coord3D::Magnitude() const
    {
        if (CoordIsNull())
            return nullReal;

        return sqrt(cX * cX + cY * cY + cZ * cZ);
    }

    double Coord3D::Magnitude(Plane3D geomPlane) const
    {
        if (CoordIsNull())
            return nullReal;

        double planeVal;
        switch (geomPlane) {
        case p3D_XY: {planeVal = cX * cX + cY * cY; break;}
        case p3D_XZ: {planeVal = cX * cX + cZ * cZ; break;}
        case p3D_YZ: {planeVal = cZ * cZ + cY * cY; break;}
        }
        return sqrt(planeVal);
    }

    void Coord3D::NormalVector(const Coord3D& a, const Coord3D& b, const Coord3D& c)
    {
        Coord3D d1 = b - a;
        Coord3D d2 = c - b;

        (*this) = CrossProduct(d1, d2);
    }

    Coord3D Coord3D::Unit() const
    {
        double vMag = Magnitude();
        if (IsPositive(vMag))
            {
                double t = 1.0 / vMag;
                return (*this) * t;
            }

        return *this;
    }


    void Coord3D::Normalize()
    {
        double vMag = Magnitude();
        if (IsPositive(vMag))
            {
                double t = 1.0 / vMag;
                (*this) *= t;
            }
    }

    // Coord3D routines below are geometry routines adapted from the gle extrusion library
    // created by Linas Vepstas  1990

    /*
     * the degenerateTolerance represents the greatest amount by
     * which different scales in a graphics environment can differ before
     * they should be considered "degenerate".   That is, when one vector is
     * a million times longer than another, changces are that the second will
     * be less than a pixel long, and therefore was probably meant to be
     * degenerate (by the CAD package, etc.)  But what should this tolerance
     * be?  At least 1 in onethousand (since screen sizes are 1K pixels), but
     * les than 1 in 4 million (since this is the limit of single-precision
     * floating point accuracy).  Of course, if double precision were used,
     * then the tolerance could be increased.
     *
     * Potentially, this naive assumption could cause problems if the CAD
     * package attempts to zoom in on small details, and turns out, certain
     * points should not hvae been degenerate.  The problem presented here
     * is that the tolerance could run out before single-precision ran
     * out, and so the CAD packages would perceive this as a "bug".
     * One alternative is to fiddle around & try to tighten the tolerance.
     * However, the right alternative is to code the graphics pipeline in
     * double-precision (and tighten the tolerance).
     *
     * By the way, note that degenerateTolerance is a "dimensionless"
     * quantitiy -- it has no units -- it does not measure feet, inches,
     * millimeters or pixels.  It is used only in the computations of ratios
     * and relative lengths.
     */

    /*
     * Right now, the tolerance is set to 2 parts in a million, which
     * corresponds to a 19-bit distinction of mantissas. Note that
     * single-precsion numbers have 24 bit mantissas.
     */

    // modified from 2E-6
    double  Coord3D::degenerateTolerance = 1.0E-09;

    void  Coord3D::Reflect(const Coord3D& n)
    {
        double dot = 2.0 * Dot(n);
        cX -= dot * n.cX;
        cY -= dot * n.cY;
        cZ -= dot * n.cZ;
    }

    void Coord3D::Reflect(const Coord3D& v,
                          const Coord3D& n)
    {
        double dot = 2.0 * DotProduct(v, n);
        cX = v.cX - dot * n.cX;
        cY = v.cY - dot * n.cY;
        cZ = v.cZ - dot * n.cZ;
    }

    void Coord3D::Perpendicular(const Coord3D& n)
    {
        double dot = Dot(n);
        cX -= dot * n.cX;
        cY -= dot * n.cY;
        cZ -= dot * n.cZ;
    }

    void Coord3D::Perpendicular(const Coord3D& v,
                                const Coord3D& n)
    {
        double dot = DotProduct(v, n);
        cX = v.cX - dot * n.cX;
        cY = v.cY - dot * n.cY;
        cZ = v.cZ - dot * n.cZ;
    }

    bool Coord3D::PlaneIntersect(const Coord3D&  p,
                                 const Coord3D&  n,
                                 const Coord3D&  v1,
                                 const Coord3D&  v2)

    {
        double deno = (v1.cX - v2.cX) * n.cX + (v1.cY - v2.cY) * n.cY + (v1.cZ - v2.cZ) * n.cZ;
        if (fabs(deno) < 1.0E-15)
            {
                *this = v1;
                return false;
                // Intersect: Warning: line is coplanar with plane n"
            }

        double   numer = (p.cX - v2.cX) * n.cX + (p.cY - v2.cY) * n.cY + (p.cZ - v2.cZ) * n.cZ;

        double t = numer / deno;
        double omt = 1.0 - t;

        cX = t * v1.cX + omt * v2.cX;
        cY = t * v1.cY + omt * v2.cY;
        cZ = t * v1.cZ + omt * v2.cZ;

        /* if (t < -5.0) valid = 2; HACK ALERT See above */

        /* if t is HUGE, then plane and line are almost co-planar */
        if ((1.0 < t * degenerateTolerance) ||
            (-1.0 > t * degenerateTolerance))
            return false;

        return true;
    }

    bool Coord3D::BisectingPlane(const Coord3D&  v1,
                                 const Coord3D&  v2,
                                 const Coord3D&  v3)
    {
        Coord3D v21 = v2 - v1;
        Coord3D v32 = v3 - v2;
        double len21 = v21.Magnitude();
        double len32 = v32.Magnitude();

        if (len21 <= degenerateTolerance * len32)
            {
                // all are 0.0 ??
                if (fabs(len32) < 1.0E-15)
                    {
                        *this = 0.0;
                        return false;
                    }

                // return a normalized copy of v32 as bisector
                *this = v32;
                Normalize();
                return true;
            }

        if (len32 <= degenerateTolerance * len21)
            {
                // return a normalized copy of v21 as bisector
                *this = v21;
                Normalize();
                return true;
            }

        // normalize v21 to be of unit length
        v21.Normalize();

        // normalize v32 to be of unit length
        v32.Normalize();

        double dot = DotProduct(v32, v21);

        // if dot == 1 or -1, then points are colinear
        if ((dot >= (1.0 - degenerateTolerance))|| (dot <= (-1.0 + degenerateTolerance)))
            {
                *this = v21;
            }
        else
            {
                // go do the full computation
                cX = dot * (v32.cX + v21.cX) - v32.cX - v21.cX;
                cY = dot * (v32.cY + v21.cY) - v32.cY - v21.cY;
                cZ = dot * (v32.cZ + v21.cZ) - v32.cZ - v21.cZ;
                Normalize();
            }

        return true;
    }

    // ******************************************************************   limit processing

    bool Limit::WithinOneLimit(const double& min,
                               const double& max,
                               const double& val)
    {
        bool Ok = true;
        if (( val > nullReal) && (min > nullReal))
            if ( min < max)
                Ok = ((val >= min) && (val <= max));
            else
                Ok = ((val <= min) && (val >= max));
        return Ok;
    }


    void Limit3D::AddToLimits(const Coord3D& cVal)
    {
        if ( minLim.CoordIsNull())
            {
                minLim = cVal;
                maxLim = cVal;
                return;
            }

        if ( cVal.cX > nullReal)
            if ((cVal.cX < minLim.cX) || (minLim.cX == nullReal))
                minLim.cX = cVal.cX;
            else
                if (cVal.cX > maxLim.cX )
                    maxLim.cX = cVal.cX;

        if ( cVal.cY > nullReal)
            if ((cVal.cY < minLim.cY ) || (minLim.cY == nullReal))
                minLim.cY = cVal.cY;
            else
                if ( cVal.cY > maxLim.cY )
                    maxLim.cY = cVal.cY;

        if (cVal.cZ > nullReal)
            if ((cVal.cZ < minLim.cZ) || (minLim.cZ == nullReal))
                minLim.cZ = cVal.cZ;
            else
                if (cVal.cZ > maxLim.cZ )
                    maxLim.cZ = cVal.cZ;
    }

    void Limit3D::AddToLimits(const Point2D& pVal)
    {
        AddToLimits(Coord3D(pVal, nullReal));
    }

    void Limit3D::AddToLimits(const Point2D& pVal,
                              const double&  zVal)
    {
        AddToLimits(Coord3D(pVal, zVal));
    }

    void Limit3D::AddToLimits(const SC_CoordArray& cVals)
    {
        for (int i = 0; i < cVals.Size(); i++)
            AddToLimits(cVals[i]);
    }

    void Limit3D::AddToLimits(const SC_PointArray& pVals)
    {
        for (int i = 0; i < pVals.Size(); i++)
            AddToLimits(Coord3D(pVals[i], nullReal));
    }

    void Limit3D::AddToLimits(const SC_PointArray& pVals,
                              const double&            zVal)
    {
        for (int i = 0; i < pVals.Size(); i++)
            AddToLimits(Coord3D(pVals[i], zVal));
    }

    void Limit3D::AddToLimits(const Limit3D& limVal)
    {
        AddToLimits(limVal.minLim);
        AddToLimits(limVal.maxLim);
    }


    bool Limit3D::WithinLimits(const Coord3D& cVal) const
    {
        bool xOk = WithinOneLimit(minLim.cX, maxLim.cX, cVal.cX);
        bool yOk = WithinOneLimit(minLim.cY, maxLim.cY, cVal.cY);
        bool zOk = WithinOneLimit(minLim.cZ, maxLim.cZ, cVal.cZ);

        return (xOk && yOk && zOk);
    }

    bool Limit3D::WithinLimits(const SC_CoordArray& cVals, bool allInLimits) const
    {
        for (int i = 0; i < cVals.Size(); i++)
            {
                bool oneInLimit = WithinLimits(cVals[i]);
                if (allInLimits)
                    {
                        if (!oneInLimit)
                            return false;
                    }
                else
                    if (oneInLimit)
                        return true;
            }

        return allInLimits;
    }


    void Limit2D::AddToLimits(const Point2D& pVal)
    {
        if ( minLim.PointIsNull())
            {
                minLim = pVal;
                maxLim = pVal;
                return;
            }

        if ( pVal.pX > nullReal)
            if ((pVal.pX < minLim.pX) || (minLim.pX == nullReal))
                minLim.pX = pVal.pX;
            else
                if ( pVal.pX > maxLim.pX )
                    maxLim.pX = pVal.pX;

        if ( pVal.pY > nullReal)
            if ((pVal.pY < minLim.pY ) || (minLim.pX == nullReal))
                minLim.pY = pVal.pY;
            else
                if ( pVal.pY > maxLim.pY )
                    maxLim.pY = pVal.pY;

    }

    void Limit2D::AddToLimits(const SC_PointArray& pVals)
    {
        for (int i = 0; i < pVals.Size(); i++)
            AddToLimits(pVals[i]);
    }

    void Limit2D::AddToLimits(const Limit2D& limVal)
    {
        AddToLimits(limVal.minLim);
        AddToLimits(limVal.maxLim);
    }

    bool Limit2D::WithinLimits(const Point2D& pVal) const
    {
        bool xOk = WithinOneLimit(minLim.pX, maxLim.pX, pVal.pX);
        bool yOk = WithinOneLimit(minLim.pY, maxLim.pY, pVal.pY);
        return (xOk && yOk);
    }



    bool Line2D::Intersect(const Line2D&     otherLine,        // true if lines arent parallel
                           double&     tVal,                   //  parametric point on this
                           double&     sVal,
                           const double&     detEps)   const   //  parametric point on otherLine
    // scooped wholesale from Programmers Geometry. p 50
    {

        double dx1 = endPt.pX - stPt.pX;
        double dy1 = endPt.pY - stPt.pY;

        double dx2 = otherLine.endPt.pX - otherLine.stPt.pX;
        double dy2 = otherLine.endPt.pY - otherLine.stPt.pY;

        double dx12 = otherLine.stPt.pX - stPt.pX;
        double dy12 = otherLine.stPt.pY - stPt.pY;

        double det = dx2 * dy1 - dy2 * dx1;

        if (fabs(det)  < detEps)
            // lines are parallel
            {
                tVal = nullReal;
                sVal = nullReal;
                return false;
            }

        //  get parametric representation of line
        tVal = (dx2 * dy12 - dy2 * dx12) / det;
        sVal = (dx1 * dy12 - dy1 * dx12) / det;
        return true;
    }

    bool Line2D::Intersect(const Line2D&     otherLine,     // true if segments intersect
                           Point2D&    intersectPoint,
                           const double&     detEps) const  // intersect Point is non-null if segments
    // scooped wholesale from Programmers Geometry. p 50
    {
        double s, t;
        if (! Intersect(otherLine, t, s, detEps))
            {
                intersectPoint = Point2D();
                return false;
            }
        intersectPoint = PointOnLine(t);
        return ((s >= 0.0) && (s <= 1.0) && (t >= 0.0) && (t <= 1.0));
    }

    bool Line2D::Intersect(const Line2D&  otherLine,
                           const double&     detEps) const  // true if segments intersect
    {
        Point2D dummy;
        return Intersect(otherLine, dummy, detEps);
    }

    bool Line2D::Normal(const Point2D&  normalPoint,
                        Point2D&  pointOnLine,
                        const double&   detEps)      const
    {
        double tVal;
        if (Normal(normalPoint, tVal, detEps))
            {
                pointOnLine = PointOnLine(tVal);
                return true;
            }
        return false;
    }

    bool Line2D::Normal(const Point2D&  normalPoint,
                        double&   pointOnLine,
                        const double&   detEps)      const
    {
        // scooped wholesale from Programmers Geometry. p 48
        double dx1 = stPt.pX - normalPoint.pX;
        double dy1 = stPt.pY - normalPoint.pY;

        double dx2 = endPt.pX - stPt.pX;
        double dy2 = endPt.pY - stPt.pY;

        double denom = dx2 * dx2 + dy2 * dy2;
        if (denom < detEps)
            return false;

        pointOnLine = -(dx1 * dx2 + dy1 * dy2) / denom;
        return true;
    }

    bool Line2D::PointIsOnLine(const Point2D&  testPoint,   // returns true if point is within distEps of extended line
                               double&    tVal,
                               const double&    distEps) const
    {
        if (!Normal(testPoint, tVal, distEps * 0.001))
            return stPt.SamePoint(testPoint, distEps);

        Point2D normPt = PointOnLine(tVal);
        return testPoint.SamePoint(normPt, distEps);
    }

    bool Line2D::PointIsOnLine(const Point2D&  testPoint,   // returns true if point is within distEps of extended line
                               const double&   distEps) const
    {
        double tVal;
        if (!Normal(testPoint, tVal, distEps * 0.001))
            return stPt.SamePoint(testPoint, distEps);

        if ((tVal < 0.0) || (tVal > 1.0))
            return false;

        Point2D normPt = PointOnLine(tVal);
        return testPoint.SamePoint(normPt, distEps);
    }


    Point2D Line2D::PointOnLine(const double& tVal) const
    {
        Point2D pt;
        pt.pX = stPt.pX + tVal * (endPt.pX - stPt.pX);
        pt.pY = stPt.pY + tVal * (endPt.pY - stPt.pY);
        return pt;
    }

    Coord3D Line3D::PointOnLine(const double& tVal) const
    {
        Coord3D pt;
        pt.cX = stPt.cX + tVal * (endPt.cX - stPt.cX);
        pt.cY = stPt.cY + tVal * (endPt.cY - stPt.cY);
        pt.cZ = stPt.cZ + tVal * (endPt.cZ - stPt.cZ);
        return pt;
    }

    double Line3D::AngleBetweenLines(const Line3D& otherLine) const
    {
        double f1 = endPt.cX - stPt.cX;
        double g1 = endPt.cY - stPt.cY;
        double h1 = endPt.cZ - stPt.cZ;

        double f2 = otherLine.endPt.cX - otherLine.stPt.cX;
        double g2 = otherLine.endPt.cY - otherLine.stPt.cY;
        double h2 = otherLine.endPt.cZ - otherLine.stPt.cZ;

        double denom = (f1 * f1 + g1* g1 + h1 * h1) * (f2 * f2 + g2 * g2 + h2 * h2);
        if (denom < 1E-30)
            return -1;

        return acos((f1 * f2 + g1 * g2 + h1 * h2)/sqrt(denom));
    }


    bool RealIsNull(const double& rVal)
    {
        return(rVal == nullReal);
    }


};   // end namespace

