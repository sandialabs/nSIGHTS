///////////////////////////////////////////////////////////////////////////////////
//
// C_Common.h
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
//              Contains basic 2D (Point2D) and 3D (Coord3D) claseses used for plotting.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_COMMON_H
#define C_COMMON_H

namespace genCommonTypes {

  const int  stdInputStrLen = 15;        // length of most strings and string/numeric input field
  const int  stdFileStrLen  = 255;       // length of file name strings
  const int  stdMsgStrLen   = 80;        // max length of messages

  static const double    nullReal      = -1E+300;   //  null value for double data (coords etc)
  static const double    pointCoordEps = 1E-30;     //  default epsilon value for pt/coord comparison

  static const char*   nullStr = " --- ";

  enum Plane3D {p3D_XY, p3D_XZ, p3D_YZ};  // horizontal, vert common Y, vert common X

  //  standard 2D coordinate
  class Coord3D;

  class Point2D {
  public:
    double      pX;
    double      pY;

  public:
    Point2D() {pX = nullReal; pY = nullReal;}
    Point2D(const double& initVal) {pX = initVal; pY = initVal;}
    Point2D(const double& x, const double& y) {pX = x; pY = y;}
    Point2D(const Point2D& a){pX = a.pX; pY = a.pY;};
    Point2D(const Coord3D& a);

    Point2D&    operator= (const Point2D& a){pX = a.pX; pY = a.pY; return *this;}
    Point2D&    operator= (const Coord3D& a);  // type conversion XY to XY

    void        ToPlotPlane(const Coord3D& worldCoord,  // maps XYZ coord sys to XY
                            Plane3D  plotPlane);  //according to plane
    //  p3D_XY - coord XY to XY
    //  p3D_XZ - coord X to pX, coord Z to pY
    //  p3D_YZ - coord Y to pX, coord Z to pY

    bool        PointIsNull() const;
    bool        SamePoint(const Point2D& test, const double eps = pointCoordEps) const;
    bool        operator== (const Point2D& a) const {return SamePoint(a);}
    double      Distance(const Point2D& toPoint) const;

  };


  //  standard 3D coordinate
  class Coord3D {
  public:
  public:
    //  used by vector geometry routines adapted from gle library -- see below
    static  double  degenerateTolerance;


    double      cX;
    double      cY;
    double      cZ;

  public:
    Coord3D() {cX = nullReal; cY = nullReal; cZ = nullReal;}
    Coord3D(const Coord3D& a) {cX = a.cX; cY = a.cY; cZ = a.cZ;}
    Coord3D(const double& initVal) {cX = initVal; cY = initVal; cZ = initVal;}
    Coord3D(const double& x, const double& y, const double& z) {cX = x; cY = y; cZ = z;}
    Coord3D(const double& x, const double& y, const double& z, Plane3D inPlane); // maps like FromModelPlane below
    Coord3D(const Point2D& a) {cX = a.pX; cY = a.pY; cZ = nullReal;}
    Coord3D(const Point2D& xy, const double& z) {cX = xy.pX; cY = xy.pY; cZ = z;}

    Coord3D&    operator= (const Coord3D& a) {cX = a.cX; cY = a.cY; cZ = a.cZ; return *this;}
    Coord3D&    operator= (const double& a)  {cX = a; cY = a; cZ = a; return *this;}
    Coord3D&    operator= (const Point2D& a) {cX = a.pX; cY = a.pY; cZ = nullReal; return *this;}  // type conversion XY to XY

    Coord3D&    operator+= (const Coord3D& a) {cX += a.cX; cY += a.cY; cZ += a.cZ; return *this;}
    Coord3D&    operator-= (const Coord3D& a) {cX -= a.cX; cY -= a.cY; cZ -= a.cZ; return *this;}
    Coord3D&    operator*= (const Coord3D& a) {cX *= a.cX; cY *= a.cY; cZ *= a.cZ; return *this;}
    Coord3D&    operator/= (const Coord3D& a) {cX /= a.cX; cY /= a.cY; cZ /= a.cZ; return *this;}

    Coord3D&    operator*= (double a) {cX *= a; cY *= a; cZ *= a; return *this;}
    Coord3D&    operator/= (double a) {cX /= a; cY /= a; cZ /= a; return *this;}


    friend Coord3D  operator+  (const Coord3D& a, const Coord3D& b) {return Coord3D(a.cX + b.cX, a.cY + b.cY, a.cZ + b.cZ);}
    friend Coord3D  operator-  (const Coord3D& a, const Coord3D& b) {return Coord3D(a.cX - b.cX, a.cY - b.cY, a.cZ - b.cZ);}
    friend Coord3D  operator*  (const Coord3D& a, const Coord3D& b) {return Coord3D(a.cX * b.cX, a.cY * b.cY, a.cZ * b.cZ);}
    friend Coord3D  operator/  (const Coord3D& a, const Coord3D& b) {return Coord3D(a.cX / b.cX, a.cY / b.cY, a.cZ / b.cZ);}

    friend Coord3D  operator*  (const Coord3D& a, const double& b) {return Coord3D(a.cX * b, a.cY * b, a.cZ * b);}

    friend Coord3D  CrossProduct(const Coord3D& a, const Coord3D& b)
    {return Coord3D ( a.cY * b.cZ - a.cZ * b.cY,
                      a.cZ * b.cX - a.cX * b.cZ,
                      a.cX * b.cY - a.cY * b.cX);}
    friend double   DotProduct(const Coord3D& a, const Coord3D& b)
    {return a.cX * b.cX + a.cY * b.cY + a.cZ * b.cZ;}



    void        FromModelPlane(Plane3D modelPlane); // converts model coord orientation to world
    void        ToModelPlane(Plane3D modelPlane);   // converts world to model

    void        ToPlotPlane(Plane3D plotPlane);     // converts world coord orientation to plot
    void        FromPlotPlane(Plane3D plotPlane);   // converts plot coord orientation to world
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

    bool        CoordIsNull() const;
    bool        SameCoord(const Coord3D& test, const double eps = pointCoordEps) const;
    bool        operator== (const Coord3D& a) const {return SameCoord(a);}
    double      Distance(const Coord3D& otherCoord) const;

    //  used for case when Coord3D is vector data
    double      Magnitude() const;
    double      Magnitude(Plane3D geomPlane) const;

    double      Dot(const Coord3D& a) const {return DotProduct(*this, a);}
    Coord3D     Cross(const Coord3D& a) const {return CrossProduct(*this, a);}
    void        NormalVector(const Coord3D& a, const Coord3D& b, const Coord3D& c);

    Coord3D     Unit() const;
    void        Normalize();

    // routines below are geometry routines adapted from the gle extrusion library
    // created by Linas Vepstas  1990

    void        Reflect(const Coord3D& n);
    // Vector reflection -- assumes n is of unit length
    // Takes vector reflects it against reflector n
    void        Reflect(const Coord3D& v,
                        const Coord3D& n);
    // Vector reflection -- assumes n is of unit length
    // Takes vector v, reflects it against reflector n

    void        Perpendicular(const Coord3D& n);
    //  Vector perpendicular -- assumes that n is of unit length
    //  subtracts out any component parallel to n

    void        Perpendicular(const Coord3D& v,
                              const Coord3D& n);
    //  Vector perpendicular -- assumes that n is of unit length
    //  accepts vector v, subtracts out any component parallel to n



    bool        PlaneIntersect(const Coord3D&  p,
                               const Coord3D&  n,
                               const Coord3D&  v1,
                               const Coord3D&  v2);

    /*
     * PlaneIntersect is designed to compute the
     * intersection of a line (defined by the points v1 and v2) and a plane
     * (defined as plane which is normal to the vector n, and contains the
     * point p).  Both return the point sect, which is the point of
     * interesection.
     *
     * HACK ALERT
     * The intersection parameter t has the nice property that if t>1,
     * then the intersection is "in front of" p1, and if t<0, then the
     * intersection is "behind" p2. Unfortunately, as the intersecting plane
     * and the line become parallel, t wraps through infinity -- i.e. t can
     * become so large that t becomes "greater than infinity" and comes back
     * as a negative number (i.e. winding number hopped by one unit).  We
     * have no way of detecting this situation without adding gazzillions
     * of lines of code of topological algebra to detect the winding number;
     * and this would be incredibly difficult, and ruin performance.
     *
     * Thus, we've installed a cheap hack for use by the "cut style" drawing
     * routines. If t proves to be a large negative number (more negative
     * than -5), then we assume that t was positive and wound through
     * infinity.  This makes most cuts look good, without introducing bogus
     * cuts at infinity.
     */


    bool        BisectingPlane(const Coord3D&  v1,
                               const Coord3D&  v2,
                               const Coord3D&  v3);
    /*
     * computes a normal vector that
     * describes the bisecting plane between three points (v1, v2 and v3).
     * This bisecting plane has the following properties:
     * 1) it contains the point v2
     * 2) the angle it makes with v21 == v2 - v1 is equal to the angle it
     *    makes with v32 == v3 - v2
     * 3) it is perpendicular to the plane defined by v1, v2, v3.
     *
     * Having input v1, v2, and v3, it returns a unit vector n.
     *
     * In some cases, the user may specify degenerate points, and still
     * expect "reasonable" or "obvious" behaviour.  The "expected"
     * behaviour for these degenerate cases is:
     *
     * 1) if v1 == v2 == v3, then return n=0
     * 2) if v1 == v2, then return v32 (normalized).
     * 3) if v2 == v3, then return v21 (normalized).
     * 4) if v1, v2 and v3 co-linear, then return v21 (normalized).
     *
     * Mathematically, these special cases "make sense" -- we just have to
     * code around potential divide-by-zero's in the code below.
     */


  };

  //  namespace for static routine
  class Limit {
  public:
    static bool  WithinOneLimit(const double& min,
                                const double& max,
                                const double& val);
  };

  class SC_CoordArray;
  class SC_PointArray;

  class Limit3D : public Limit {
  public:
    Coord3D minLim;
    Coord3D maxLim;

  public:
    Limit3D(){};
    Limit3D(Coord3D& initVal){minLim = initVal; maxLim = initVal;}

    void    AddToLimits(const Coord3D& cVal);
    void    AddToLimits(const Point2D& pVal); // zVal is null real
    void    AddToLimits(const Point2D& pVal,
                        const double&  zVal);

    void    AddToLimits(const SC_CoordArray& cVals);
    void    AddToLimits(const SC_PointArray& pVals); // zVal is null real
    void    AddToLimits(const SC_PointArray& pVals,
                        const double&        zVal);


    void    AddToLimits(const Limit3D& limVal);
    bool    WithinLimits(const Coord3D& cVal) const;
    bool    WithinLimits(const SC_CoordArray& cVals, bool allInLimits = false) const;



    double  Dx() const {return (maxLim.cX - minLim.cX);}
    double  Dy() const {return (maxLim.cY - minLim.cY);}
    double  Dz() const {return (maxLim.cZ - minLim.cZ);}
  };

  class Limit2D : public Limit{
  public:
    Point2D     minLim;
    Point2D     maxLim;
  public:
    Limit2D(){}
    Limit2D(Point2D& initVal){minLim = initVal; maxLim = initVal;}
    Limit2D(const Limit2D& a){minLim = a.minLim; maxLim = a.maxLim;}
    Limit2D(const Limit3D& a){minLim = a.minLim; maxLim = a.maxLim;}

    Limit2D&    operator= (const Limit2D& a) {minLim = a.minLim; maxLim = a.maxLim; return *this;}
    Limit2D&    operator= (const Limit3D& a) {minLim = a.minLim; maxLim = a.maxLim; return *this;}

    void        AddToLimits(const Point2D&  pVal);
    void        AddToLimits(const SC_PointArray& pVals);
    void        AddToLimits(const Limit2D&  limVal);
    bool        WithinLimits(const Point2D& pVal) const;

    double      Dx() const {return (maxLim.pX - minLim.pX);}
    double      Dy() const {return (maxLim.pY - minLim.pY);}

  };

  class Line2D {
  public:
    Point2D     stPt;
    Point2D     endPt;

  public:
    Line2D(){}
    Line2D(const Line2D& a) {stPt = a.stPt; endPt = a.endPt;}
    Line2D(const Point2D& st,
           const Point2D& en) {stPt = st; endPt = en;}
    Line2D&     operator= (const Line2D& a) {stPt = a.stPt; endPt = a.endPt; return *this;}

    double      Length() const {return stPt.Distance(endPt);}

    bool        Intersect(const Line2D&  otherLine,
                          const double&  detEps = 1.0E-06) const; //  returns true if segments intersect

    bool        Intersect(const Line2D&  otherLine,               // true if segments intersect
                          Point2D& intersectPoint,
                          const double&  detEps = 1.0E-06) const; // intersect Point is non-null if segments
    // are not parallel
    bool        Normal(const Point2D&  normalPoint,
                       Point2D&  pointOnLine,
                       const double&   detEps = 1.0E-06) const;  // true if line has length > detEps, point on this where normal
    // from normalPoint intersects
    bool        Normal(const Point2D&  normalPoint,
                       double&   pointOnLine,
                       const double&   detEps = 1.0E-06) const;  // parametric form

    bool        Intersect(const Line2D&  otherLine,              // true if lines arent parallel
                          double&  tVal,                         //  parametric point on this
                          double&  sVal,                         //  parametric point on otherLine
                          const double&   detEps = 1.0E-06) const;

    Point2D     PointOnLine(const double& tVal)          const;  // returns point calculated at t

    bool        PointIsOnLine(const Point2D&  testPoint,   // returns true if point is within distEps of extended line
                              double&   tVal,
                              const double&   distEps = 1.0E-06) const;

    bool        PointIsOnLine(const Point2D&  testPoint,  // returns true if point is within distEps of line segment
                              const double&   distEps = 1.0E-06) const;

  };

  class Line3D {
  public:
    Coord3D     stPt;
    Coord3D     endPt;
  public:
    Line3D(){}
    Line3D(const Line2D& a) {stPt = a.stPt; endPt = a.endPt;}
    Line3D(const Line3D& a) {stPt = a.stPt; endPt = a.endPt;}
    Line3D(const Coord3D& st,
           const Coord3D& en) {stPt = st; endPt = en;}
    Line3D&     operator= (const Line3D& a) {stPt = a.stPt; endPt = a.endPt; return *this;}

    double      Length() const {return stPt.Distance(endPt);}

    Coord3D     PointOnLine(const double& tVal)          const;  // returns point calculated at t

    double      AngleBetweenLines(const Line3D& otherLine) const;
  };

  extern bool RealIsNull(const double& rVal);


};

using namespace genCommonTypes;


#endif // C_COMMON_H

