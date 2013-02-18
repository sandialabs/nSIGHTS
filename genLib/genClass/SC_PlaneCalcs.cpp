///////////////////////////////////////////////////////////////////////////////////
//
// SC_PlaneCalcs.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genClass/SC_PlaneCalcs.h>


namespace genCommonTypes {

    SC_PlaneCalcs::SC_PlaneCalcs()
        :       SC_CoordArray(buffer, 4),
                isVertical(false), intersectInPoly(false)
    {
        SetSize(3);
    }

    SC_PlaneCalcs::SC_PlaneCalcs(int nAlloc)
        :       SC_CoordArray(buffer, 4),
                isVertical(false), intersectInPoly(false)
    {
        AllocAndSetSize(nAlloc);
        isVertical = false;
    }

    bool SC_PlaneCalcs::CalcPlane(const SC_CoordArray& inTri)
    {
        SC_CoordArray::operator=(inTri);

        return CalcPlane();
    }


    bool SC_PlaneCalcs::CalcPlane() const
    {
        // ok for 4 - assume 4 is on same plane
        if ((Size() < 3) || (Size() > 4))
            return false;

        for (int i = 0; i < 3; i++)
            if ((*this)[i].CoordIsNull())
                return false;


        const Coord3D& j = (*this)[0];
        const Coord3D& k = (*this)[1];
        const Coord3D& l = (*this)[2];

        double xkj = k.cX - j.cX;
        double ykj = k.cY - j.cY;
        double zkj = k.cZ - j.cZ;

        double xlj = l.cX - j.cX;
        double ylj = l.cY - j.cY;
        double zlj = l.cZ - j.cZ;

        a = ykj * zlj - zkj * ylj;
        b = zkj * xlj - xkj * zlj;
        c = xkj * ylj - ykj * xlj;
        d = -(k.cX * a + k.cY * b + k.cZ * c);

        isVertical = (fabs(c) < stdEps);

        // check 4th point
        if (Size() == 4)
            {
                return (Distance((*this)[3]) < stdEps);
            }


        return true;
    }


    double  SC_PlaneCalcs::CalcPlaneZ(const double& planeX, const double& planeY) const
    {
        if (isVertical)
            return nullReal;

        return -(a * planeX + b * planeY + d) / c;
    }

    double SC_PlaneCalcs::CalcPlaneZ(const Point2D& planeXY) const
    {
        return CalcPlaneZ(planeXY.pX, planeXY.pY);
    }

    void SC_PlaneCalcs::CalcPlaneZ(Coord3D& planeXYZ) const
    {
        planeXYZ.cZ = CalcPlaneZ(planeXYZ.cX, planeXYZ.cY);
    }

    void SC_PlaneCalcs::SetFromLimit(const Limit3D& planeLimits,
                                     Plane3D  planeOrientation,
                                     const double&  planeZ)
    {
        SetSize(4);
        (*this)[0] = planeLimits.minLim;
        (*this)[2] = planeLimits.maxLim;

        //      p3D_XY, p3D_XZ, p3D_YZ
        switch (planeOrientation) {
        case p3D_XY : {
            (*this)[1].cX = planeLimits.minLim.cX;
            (*this)[1].cY = planeLimits.maxLim.cY;
            (*this)[3].cX = planeLimits.maxLim.cX;
            (*this)[3].cY = planeLimits.minLim.cY;
            for (int i = 0; i < 4; i++)
                (*this)[i].cZ = planeZ;
            break;
        }
        case p3D_XZ : {
            (*this)[1].cX = planeLimits.minLim.cX;
            (*this)[1].cZ = planeLimits.maxLim.cZ;
            (*this)[3].cX = planeLimits.maxLim.cX;
            (*this)[3].cZ = planeLimits.minLim.cZ;
            for (int i = 0; i < 4; i++)
                (*this)[i].cY = planeZ;
            break;
        }
        case p3D_YZ : {
            (*this)[1].cY = planeLimits.minLim.cY;
            (*this)[1].cZ = planeLimits.maxLim.cZ;
            (*this)[3].cY = planeLimits.maxLim.cY;
            (*this)[3].cZ = planeLimits.minLim.cZ;
            for (int i = 0; i < 4; i++)
                (*this)[i].cX = planeZ;
            break;
        }
        }
    }

    double SC_PlaneCalcs::Distance(const Coord3D& ptXYZ) const
    {
        double sr = a * ptXYZ.cX + b * ptXYZ.cY + c * ptXYZ.cZ + d;
        return sqrt(sr * sr / (a * a + b * b + c * c));
    }

    bool SC_PlaneCalcs::Intersect(const Line3D& line,
                                  Coord3D& iCoord) const
    {
        return Intersect(line.stPt, line.endPt, iCoord);
    }

    bool SC_PlaneCalcs::Intersect(const Line3D& line,
                                  double& tVal) const
    {
        return Intersect(line.stPt, line.endPt, tVal);
    }

    void SC_PlaneCalcs::IntersectInPolyOnly(bool inPoly)
    {
        intersectInPoly = inPoly;
    }

    bool SC_PlaneCalcs::PointInPoly(const Coord3D&  inCoord) const
    // returns true if inCoord is on plane and inside poly
    // http://astronomy.swin.edu.au/~pbourke/geometry/insidepoly/
    {
        double anglesum=0;
        Coord3D p1,p2;

        for (int i = 0; i < Size(); i++)
            {
                p1 = (*this)[i] - inCoord;
                p2 = NextIndex(i) - inCoord;

                double m1m2 = p1.Magnitude() * p2.Magnitude();

                if (m1m2 <= stdEps)
                    return true; /* We are on a node, consider this inside */

                double costheta = DotProduct(p1, p2)  / m1m2;

                anglesum += acos(costheta);
            }

        return RealsAreSame(anglesum, 2.0 * pi, 1.0E-10);
    }


    bool SC_PlaneCalcs::Intersect(const Coord3D& lineSt,
                                  const Coord3D& lineEnd,
                                  double&  tVal) const
    {
        double f = lineEnd.cX - lineSt.cX;
        double g = lineEnd.cY - lineSt.cY;
        double h = lineEnd.cZ - lineSt.cZ;

        double denom = (a * f + b * g + c * h);
        if (fabs(denom) < stdEps)
            return false;

        tVal = - (a * lineSt.cX + b * lineSt.cY + c * lineSt.cZ + d) / denom;

        if (!intersectInPoly)
            return true;

        Coord3D iCoord(lineSt);
        iCoord.cX += tVal * f;
        iCoord.cY += tVal * g;
        iCoord.cZ += tVal * h;

        return PointInPoly(iCoord);
    }

    bool SC_PlaneCalcs::Intersect(const Coord3D& lineSt,
                                  const Coord3D& lineEnd,
                                  Coord3D& iCoord) const
    {
        double f = lineEnd.cX - lineSt.cX;
        double g = lineEnd.cY - lineSt.cY;
        double h = lineEnd.cZ - lineSt.cZ;

        double denom = (a * f + b * g + c * h);
        if (fabs(denom) < stdEps)
            return false;

        double t = - (a * lineSt.cX + b * lineSt.cY + c * lineSt.cZ + d) / denom;
        if ((t < 0.0) || (t > 1.0))
            return false;

        iCoord = lineSt;
        iCoord.cX += t * f;
        iCoord.cY += t * g;
        iCoord.cZ += t * h;

        if (!intersectInPoly)
            return true;

        return PointInPoly(iCoord);
    }


    double SC_PlaneCalcs::Area()
    {
        if ((Size() < 3) || (Size() > 4))
            GenAppInternalError("SC_PlaneCalcs::Area");

        Coord3D& K = (*this)[0];
        Coord3D& L = (*this)[1];
        Coord3D& M = (*this)[2];

        // programmers geometry pg 120

        double tr1 = 0.5 * sqrt(
                                Sqr((L.cY - K.cY) * (M.cZ - K.cZ) - (L.cZ - K.cZ) * (M.cY - K.cY)) +
                                Sqr((L.cZ - K.cZ) * (M.cX - K.cX) - (L.cX - K.cX) * (M.cZ - K.cZ)) +
                                Sqr((L.cX - K.cX) * (M.cY - K.cY) - (L.cY - K.cY) * (M.cX - K.cX)));

        if (Size() == 3)
            return tr1;

        Coord3D& N = (*this)[3];

        // FB 293 bug - was vertices L, N, M
        //      double tr2 = 0.5 * sqrt(
        //                      Sqr((L.cY - N.cY) * (M.cZ - N.cZ) - (L.cZ - N.cZ) * (M.cY - N.cY)) +
        //                      Sqr((L.cZ - N.cZ) * (M.cX - N.cX) - (L.cX - N.cX) * (M.cZ - N.cZ)) +
        //                      Sqr((L.cX - N.cX) * (M.cY - N.cY) - (L.cY - N.cY) * (M.cX - N.cX)));

        // should be vertices K, N, M
        double tr2 = 0.5 * sqrt(
                                Sqr((K.cY - N.cY) * (M.cZ - N.cZ) - (K.cZ - N.cZ) * (M.cY - N.cY)) +
                                Sqr((K.cZ - N.cZ) * (M.cX - N.cX) - (K.cX - N.cX) * (M.cZ - N.cZ)) +
                                Sqr((K.cX - N.cX) * (M.cY - N.cY) - (K.cY - N.cY) * (M.cX - N.cX)));

        return tr1 + tr2;
    }

    bool SC_PlaneCalcs::Intersects(const SC_PlaneCalcs& otherPoly,
                                   const double&                eps)
    {
        //return Intersects(otherPoly, dummy1, dummy2, false);
        if ((Size() < 3) || (Size() > 4) || (otherPoly.Size() < 3) || (otherPoly.Size() > 4))
            GenAppInternalError("SC_PlaneCalcs::Intersects");

        Coord3D dummy1, dummy2;

        bool tr1OK = TriIntersect((*this)[0], (*this)[1], (*this)[2],
                                  otherPoly[0], otherPoly[1], otherPoly[2],
                                  dummy1, dummy2, false, eps);
        if (tr1OK)
            return true;

        if (Size() == 3)
            {
                if (otherPoly.Size() == 3)
                    return false;
                return TriIntersect((*this)[0], (*this)[1], (*this)[2],
                                    otherPoly[0], otherPoly[2], otherPoly[3],
                                    dummy1, dummy2, false, eps);
            }

        bool tr2OK = TriIntersect((*this)[0], (*this)[2], (*this)[3],
                                  otherPoly[0], otherPoly[1], otherPoly[2],
                                  dummy1, dummy2, false, eps);

        if (tr2OK || (otherPoly.Size() == 3))
            return tr2OK;


        return TriIntersect((*this)[0], (*this)[2], (*this)[3],
                            otherPoly[0], otherPoly[2], otherPoly[3],
                            dummy1, dummy2, false, eps);
    }

    bool SC_PlaneCalcs::Intersects(const SC_PlaneCalcs& otherPoly,
                                   Coord3D&             intersectPt1,
                                   Coord3D&             intersectPt2,
                                   const double&        eps)

    {
        bool calcIntersectLine = true;
        if ((Size() < 3) || (Size() > 4) || (otherPoly.Size() < 3) || (otherPoly.Size() > 4))
            GenAppInternalError("SC_PlaneCalcs::Intersects");

        bool tr1OK = TriIntersect((*this)[0], (*this)[1], (*this)[2],
                                  otherPoly[0], otherPoly[1], otherPoly[2],
                                  intersectPt1, intersectPt2, calcIntersectLine, eps);

        Coord3D tempPt1, tempPt2;
        bool tr2OK = false;
        if (Size() == 3)
            {
                if (otherPoly.Size() == 3)
                    {
                        if (tr1OK)
                            return true;
                        else
                            return false;
                    }
                tr2OK = TriIntersect((*this)[0], (*this)[1], (*this)[2],
                                     otherPoly[0], otherPoly[2], otherPoly[3],
                                     tempPt1, tempPt2, calcIntersectLine, eps);
            }
        else
            {
                tr2OK = TriIntersect((*this)[0], (*this)[2], (*this)[3],
                                     otherPoly[0], otherPoly[1], otherPoly[2],
                                     tempPt1, tempPt2, calcIntersectLine, eps);

                if (!tr2OK && (otherPoly.Size() == 4))
                    tr2OK = TriIntersect((*this)[0], (*this)[2], (*this)[3],
                                         otherPoly[0], otherPoly[2], otherPoly[3],
                                         tempPt1, tempPt1, calcIntersectLine, eps);
            }

        if (tr1OK && tr2OK)
            {
                if (intersectPt1.SameCoord(tempPt1, 1E-3))
                    intersectPt1 = tempPt2;
                else if (intersectPt1.SameCoord(tempPt2, 1E-3))
                    intersectPt1 = tempPt1;
                else if (intersectPt2.SameCoord(tempPt1, 1E-3))
                    intersectPt2 = tempPt2;
                else if (intersectPt2.SameCoord(tempPt2, 1E-3))
                    intersectPt2 = tempPt1;
                return true;
            }
        else if (tr1OK)
            return true;
        else if (tr2OK)
            {
                intersectPt1 = tempPt1;
                intersectPt2 = tempPt2;
                return true;
            }
        else
            return false;
    }


    /*

      Extracted/converted  from

      * Triangle/triangle intersection test routine,
      * by Tomas Moller, 1997.
      * See article "A Fast Triangle-Triangle Intersection Test",
      * Journal of Graphics Tools, 2(2), 1997
      * updated: 2001-06-20 (added line of intersection)
      *
      * int tri_tri_intersect(float V0[3],float V1[3],float V2[3],
      *                       float U0[3],float U1[3],float U2[3])
      *
      * parameters: vertices of triangle 1: V0,V1,V2
      *             vertices of triangle 2: U0,U1,U2
      * result    : returns 1 if the triangles intersect, otherwise 0
      *
      * Here is a version withouts divisions (a little faster)
      * int NoDivTriTriIsect(float V0[3],float V1[3],float V2[3],
      *                      float U0[3],float U1[3],float U2[3]);
      *
      * This version computes the line of intersection as well (if they are not coplanar):
      * int tri_tri_intersect_with_isectline(float V0[3],float V1[3],float V2[3],
      *                                      float U0[3],float U1[3],float U2[3],int *coplanar,
      *                                      float isectpt1[3],float isectpt2[3]);
      * coplanar returns whether the tris are coplanar
      * isectpt1, isectpt2 are the endpoints of the line of intersection
      */


    const double SC_PlaneCalcs::triCalcEps = 1.0E-10;

    bool SC_PlaneCalcs::TriIntersect(const Coord3D& V0,
                                     const Coord3D& V1,
                                     const Coord3D& V2,
                                     const Coord3D& U0,
                                     const Coord3D& U1,
                                     const Coord3D& U2,
                                     Coord3D&     isectpt1,
                                     Coord3D&     isectpt2,
                                     const bool calcIntersectLine,
                                     const double&              eps)
    {
        /*
          float E1[3],E2[3];
          float N1[3],N2[3],d1,d2;
          float du0,du1,du2,dv0,dv1,dv2;
          float D[3];
          float isect1[2], isect2[2];
          float du0du1,du0du2,dv0dv1,dv0dv2;
          short index;
          float vp0,vp1,vp2;
          float up0,up1,up2;
          float bb,cc,max;
          float a,b,c,x0,x1;
          float d,e,f,y0,y1;
          float xx,yy,xxyy,tmp;
        */

        /* compute plane equation of triangle(V0,V1,V2)
           SUB(E1,V1,V0);
           SUB(E2,V2,V0);
           CROSS(N1,E1,E2);
           d1=-DOT(N1,V0);
           plane equation 1: N1.X+d1=0 */

        Coord3D E1 = V1 - V0;
        Coord3D E2 = V2 - V0;
        Coord3D N1 = CrossProduct(E1, E2);
        double d1 = -DotProduct(N1, V0);

        /*      put U0,U1,U2 into plane equation 1 to compute signed distances to the plane
                du0=DOT(N1,U0)+d1;
                du1=DOT(N1,U1)+d1;
                du2=DOT(N1,U2)+d1;
        */
        double du0 = DotProduct(N1, U0) + d1;
        double du1 = DotProduct(N1, U1) + d1;
        double du2 = DotProduct(N1, U2) + d1;


        /*coplanarity robustness check
          #if USE_EPSILON_TEST==TRUE
          if(FABS(du0)<EPSILON) du0=0.0;
          if(FABS(du1)<EPSILON) du1=0.0;
          if(FABS(du2)<EPSILON) du2=0.0;
          #endif
          du0du1=du0*du1;
          du0du2=du0*du2;
        */
        if (fabs(du0) < eps)
            du0 = 0.0;
        if (fabs(du1) < eps)
            du1 = 0.0;
        if (fabs(du2) < eps)
            du2 = 0.0;

        double du0du1 = du0 * du1;
        double du0du2 = du0 * du2;

        //  if(du0du1>0.0f && du0du2>0.0f) /* same sign on all of them + not equal 0 ? */
        //    return 0;                    /* no intersection occurs */

        if((du0du1>0.0) && (du0du2>0.0))
            return false;

        /*      compute plane of triangle (U0,U1,U2)
                SUB(E1,U1,U0);
                SUB(E2,U2,U0);
                CROSS(N2,E1,E2);
                d2=-DOT(N2,U0);
                plane equation 2: N2.X+d2=0 */

        E1 = U1 - U0;
        E2 = U2 - U0;
        Coord3D N2 = CrossProduct(E1, E2);
        double d2 = -DotProduct(N2, U0);

        /*      put V0,V1,V2 into plane equation 2
                dv0=DOT(N2,V0)+d2;
                dv1=DOT(N2,V1)+d2;
                dv2=DOT(N2,V2)+d2;
        */
        double dv0 = DotProduct(N2, V0) + d2;
        double dv1 = DotProduct(N2, V1) + d2;
        double dv2 = DotProduct(N2, V2) + d2;

        /*      if USE_EPSILON_TEST==TRUE
                if(FABS(dv0)<EPSILON) dv0=0.0;
                if(FABS(dv1)<EPSILON) dv1=0.0;
                if(FABS(dv2)<EPSILON) dv2=0.0;
                #endif
                dv0dv1=dv0*dv1;
                dv0dv2=dv0*dv2;
        */
        if (fabs(dv0) < eps)
            dv0 = 0.0;
        if (fabs(dv1) < eps)
            dv1 = 0.0;
        if (fabs(dv2) < eps)
            dv2 = 0.0;

        double dv0dv1 = dv0 * dv1;
        double dv0dv2 = dv0 * dv2;

        //  if(dv0dv1>0.0f && dv0dv2>0.0f) /* same sign on all of them + not equal 0 ? */
        //    return 0;                    /* no intersection occurs */

        if((dv0dv1 > 0.0) && (dv0dv2 > 0.0)) /* same sign on all of them + not equal 0 ? */
            return false;                    /* no intersection occurs */


        /*      compute direction of intersection line
                CROSS(D,N1,N2);
        */
        Coord3D D = CrossProduct(N1, N2);

        /* compute and index to the largest component of D
           max=(float)FABS(D[0]);
           index=0;
           bb=(float)FABS(D[1]);
           cc=(float)FABS(D[2]);
           if(bb>max) max=bb,index=1;
           if(cc>max) max=cc,index=2; */

        double max = fabs(D.cX);
        int index = 0;
        double bb = fabs(D.cY);
        if (bb > max)
            {
                max = bb;
                index = 1;
            }
        double cc = fabs(D.cZ);
        if (cc > max)
            {
                max = cc;
                index = 2;
            }


        /*      this is the simplified projection onto L
                vp0=V0[index];
                vp1=V1[index];
                vp2=V2[index];

                up0=U0[index];
                up1=U1[index];
                up2=U2[index];
        */
        double vp0, vp1, vp2, up0, up1, up2;
        switch (index) {
        case 0 : {
            vp0 = V0.cX; vp1 = V1.cX; vp2 = V2.cX;
            up0 = U0.cX; up1 = U1.cX; up2 = U2.cX;
            break;
        }
        case 1 : {
            vp0 = V0.cY; vp1 = V1.cY; vp2 = V2.cY;
            up0 = U0.cY; up1 = U1.cY; up2 = U2.cY;
            break;
        }
        case 2 : {
            vp0 = V0.cZ; vp1 = V1.cZ; vp2 = V2.cZ;
            up0 = U0.cZ; up1 = U1.cZ; up2 = U2.cZ;
            break;
        }
        }

        if (!calcIntersectLine)
            {
                /*      compute interval for triangle 1
                        NEWCOMPUTE_INTERVALS(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv2,a,b,c,x0,x1);

                        compute interval for triangle 2
                        NEWCOMPUTE_INTERVALS(up0,up1,up2,du0,du1,du2,du0du1,du0du2,d,e,f,y0,y1);
                */
                double a, b, c, d, x0, x1;
                if (!ComputeIntervals(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv2,a,b,c,x0,x1))
                    return CoplanarTriTri(N1, V0, V1, V2, U0, U1, U2);

                double e, f, y0, y1;
                if (!ComputeIntervals(up0,up1,up2,du0,du1,du2,du0du1,du0du2,d,e,f,y0,y1))
                    return CoplanarTriTri(N1, V0, V1, V2, U0, U1, U2);

                /*
                  xx=x0*x1;
                  yy=y0*y1;
                  xxyy=xx*yy;

                  tmp=a*xxyy;
                  isect1[0]=tmp+b*x1*yy;
                  isect1[1]=tmp+c*x0*yy;

                  tmp=d*xxyy;
                  isect2[0]=tmp+e*xx*y1;
                  isect2[1]=tmp+f*xx*y0;
                */
                double xx = x0 * x1;
                double yy = y0 * y1;
                double xxyy = xx * yy;

                double tmp = a * xxyy;

                double isect1[2], isect2[2];
                isect1[0] = tmp + b * x1 * yy;
                isect1[1] = tmp + c * x0 * yy;

                tmp = d * xxyy;
                isect2[0] = tmp + e * xx * y1;
                isect2[1] = tmp + f * xx *y0;


                /*      SORT(isect1[0],isect1[1]);
                        SORT(isect2[0],isect2[1]);
                */
                TriSort(isect1[0], isect1[1]);
                TriSort(isect2[0], isect2[1]);


                //  if (isect1[1]<isect2[0] || isect2[1]<isect1[0]) return 0;
                //  return 1;

                if ((isect1[1] < isect2[0]) || (isect2[1]<isect1[0]))
                    return false;
            }
        else
            {
                /* for case of calculating line of intersection:
                   compute interval for triangle 1
                   *coplanar=compute_intervals_isectline(V0,V1,V2,vp0,vp1,vp2,dv0,dv1,dv2,
                   dv0dv1,dv0dv2,&isect1[0],&isect1[1],isectpointA1,isectpointA2);
                   if(*coplanar) return coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2);


                   compute interval for triangle 2
                   compute_intervals_isectline(U0,U1,U2,up0,up1,up2,du0,du1,du2,
                   du0du1,du0du2,&isect2[0],&isect2[1],isectpointB1,isectpointB2);

                */
                double isect1[2], isect2[2];
                Coord3D isectpointA1, isectpointA2;
                if (!ComputeIntervalsIsectLine(V0,V1,V2,vp0,vp1,vp2,dv0,dv1,dv2,
                                               dv0dv1,dv0dv2,isect1[0],isect1[1],isectpointA1,isectpointA2))
                    return CoplanarTriTri(N1, V0, V1, V2, U0, U1, U2);

                Coord3D isectpointB1, isectpointB2;
                if (!ComputeIntervalsIsectLine(U0,U1,U2,up0,up1,up2,du0,du1,du2,
                                               du0du1,du0du2,isect2[0],isect2[1],isectpointB1,isectpointB2))
                    return CoplanarTriTri(N1, V0, V1, V2, U0, U1, U2);

                /*      if calculating line of intersection:
                        SORT2(isect1[0],isect1[1],smallest1);
                        SORT2(isect2[0],isect2[1],smallest2);

                */
                double smallest1 = 0;
                double smallest2 = 0;
                TriSort(isect1[0], isect1[1], smallest1);
                TriSort(isect2[0], isect2[1], smallest2);

                /*
                  if(isect1[1]<isect2[0] || isect2[1]<isect1[0]) return 0;

                  at this point, we know that the triangles intersect

                  if(isect2[0]<isect1[0])
                  {
                  if(smallest1==0) { SET(isectpt1,isectpointA1); }
                  else { SET(isectpt1,isectpointA2); }

                  if(isect2[1]<isect1[1])
                  {
                  if(smallest2==0) { SET(isectpt2,isectpointB2); }
                  else { SET(isectpt2,isectpointB1); }
                  }
                  else
                  {
                  if(smallest1==0) { SET(isectpt2,isectpointA2); }
                  else { SET(isectpt2,isectpointA1); }
                  }
                  }
                  else
                  {
                  if(smallest2==0) { SET(isectpt1,isectpointB1); }
                  else { SET(isectpt1,isectpointB2); }

                  if(isect2[1]>isect1[1])
                  {
                  if(smallest1==0) { SET(isectpt2,isectpointA2); }
                  else { SET(isectpt2,isectpointA1); }
                  }
                  else
                  {
                  if(smallest2==0) { SET(isectpt2,isectpointB2); }
                  else { SET(isectpt2,isectpointB1); }
                  }
                  }
                */
                if ((isect1[1] < isect2[0]) || (isect2[1]<isect1[0]))
                    return false;

                if(isect2[0] < isect1[0])
                    {
                        if (smallest1 == 0)
                            isectpt1 = isectpointA1;
                        else
                            isectpt1 = isectpointA2;

                        if (isect2[1] < isect1[1])
                            {
                                if (smallest2 == 0)
                                    isectpt2 = isectpointB2;
                                else
                                    isectpt2 = isectpointB1;
                            }
                        else
                            {
                                if (smallest1 == 0)
                                    isectpt2 = isectpointA2;
                                else
                                    isectpt2 = isectpointA1;
                            }
                    }
                else
                    {
                        if (smallest2 == 0)
                            isectpt1 = isectpointB1;
                        else
                            isectpt1 = isectpointB2;

                        if (isect2[1] > isect1[1])
                            {
                                if (smallest1 == 0)
                                    isectpt2 = isectpointA2;
                                else
                                    isectpt2 = isectpointA1;
                            }
                        else
                            {
                                if (smallest2 == 0)
                                    isectpt2 = isectpointB2;
                                else
                                    isectpt2 = isectpointB1;
                            }
                    }

            }

        return true;
    }

    void SC_PlaneCalcs::TriSort(double& a, double& b)
    {
        if( a > b)
            {
                double c;
                c = a;
                a = b;
                b = c;
            }
    }

    void SC_PlaneCalcs::TriSort(double& a, double& b, double& smallest)
    {
        if( a > b)
            {
                double c;
                c = a;
                a = b;
                b = c;
                smallest = 1;
            }
        else
            smallest = 0;
    }

    bool SC_PlaneCalcs::ComputeIntervals(const double& VV0,
                                         const double& VV1,
                                         const double& VV2,
                                         const double& D0,
                                         const double& D1,
                                         const double& D2,
                                         const double& D0D1,
                                         const double& D0D2,
                                         double& A,
                                         double& B,
                                         double& C,
                                         double& X0,
                                         double& X1)
    {

        if(D0D1 > 0.0)
            {
                // here we know that D0D2<=0.0
                // that is D0, D1 are on the same side, D2 on the other or on the plane
                A=VV2; B=(VV0-VV2)*D2; C=(VV1-VV2)*D2; X0=D2-D0; X1=D2-D1;
            }
        else if (D0D2 > 0.0)
            {
                // here we know that d0d1<=0.0
                A=VV1; B=(VV0-VV1)*D1; C=(VV2-VV1)*D1; X0=D1-D0; X1=D1-D2;
            }
        else if ((D1 * D2 > 0.0) || (D0 != 0.0))
            {
                // here we know that d0d1<=0.0 or that D0!=0.0
                A=VV0; B=(VV1-VV0)*D0; C=(VV2-VV0)*D0; X0=D0-D1; X1=D0-D2;
            }
        else if(D1 != 0.0)
            {
                A=VV1; B=(VV0-VV1)*D1; C=(VV2-VV1)*D1; X0=D1-D0; X1=D1-D2;
            }
        else if(D2 != 0.0)
            {
                A=VV2; B=(VV0-VV2)*D2; C=(VV1-VV2)*D2; X0=D2-D0; X1=D2-D1;
            }
        else
            return false;

        return true;
    }

    bool SC_PlaneCalcs::ComputeIntervalsIsectLine(  const Coord3D& VERT0,
                                                    const Coord3D& VERT1,
                                                    const Coord3D& VERT2,
                                                    const double& VV0,
                                                    const double& VV1,
                                                    const double& VV2,
                                                    const double& D0,
                                                    const double& D1,
                                                    const double& D2,
                                                    const double& D0D1,
                                                    const double& D0D2,
                                                    double& isect0,
                                                    double& isect1,
                                                    Coord3D& isectpoint0,
                                                    Coord3D& isectpoint1)
    {
        if (D0D1 > 0.0)
            {
                // here we know that D0D2<=0.0
                // that is D0, D1 are on the same side, D2 on the other or on the plane
                Isect2(VERT2,VERT0,VERT1,VV2,VV0,VV1,D2,D0,D1,isect0,isect1,isectpoint0,isectpoint1);
            }
        else if (D0D2 > 0.0)
            {
                // here we know that d0d1<=0.0
                Isect2(VERT1,VERT0,VERT2,VV1,VV0,VV2,D1,D0,D2,isect0,isect1,isectpoint0,isectpoint1);
            }
        else if ((D1 * D2 > 0.0) || (D0 != 0.0))
            {
                // here we know that d0d1<=0.0 or that D0!=0.0
                Isect2(VERT0,VERT1,VERT2,VV0,VV1,VV2,D0,D1,D2,isect0,isect1,isectpoint0,isectpoint1);
            }
        else if (D1 != 0.0)
            {
                Isect2(VERT1,VERT0,VERT2,VV1,VV0,VV2,D1,D0,D2,isect0,isect1,isectpoint0,isectpoint1);
            }
        else if (D2 != 0.0)
            {
                Isect2(VERT2,VERT0,VERT1,VV2,VV0,VV1,D2,D0,D1,isect0,isect1,isectpoint0,isectpoint1);
            }
        else
            {
                // triangles are coplanar
                return false;
            }
        return true;
    }

    void SC_PlaneCalcs::Isect2(const Coord3D& VTX0,
                               const Coord3D& VTX1,
                               const Coord3D& VTX2,
                               const double&        VV0,
                               const double&        VV1,
                               const double&        VV2,
                               const double&        D0,
                               const double&        D1,
                               const double&        D2,
                               double&      isect0,
                               double&      isect1,
                               Coord3D&     isectpoint0,
                               Coord3D&     isectpoint1)
    {
        double tmp = D0 / (D0 - D1);
        isect0 = VV0 + (VV1 - VV0) * tmp;

        Coord3D diff;
        diff = VTX1 - VTX0;
        diff *= tmp;
        isectpoint0 = diff + VTX0;

        tmp = D0 / (D0 - D2);
        isect1 = VV0 + (VV2 - VV0) * tmp;

        diff = VTX2 - VTX0;
        diff *= tmp;
        isectpoint1 = VTX0 + diff;
    }


    //int coplanar_tri_tri(float N[3],float V0[3],float V1[3],float V2[3],
    //                   float U0[3],float U1[3],float U2[3])


    bool SC_PlaneCalcs::CoplanarTriTri(const Coord3D& N,
                                       const Coord3D& V0,
                                       const Coord3D& V1,
                                       const Coord3D& V2,
                                       const Coord3D& U0,
                                       const Coord3D& U1,
                                       const Coord3D& U2)
    {

        /*   float A[3];
             short i0,i1;
             first project onto an axis-aligned plane, that maximizes the area
             of the triangles, compute indices: i0,i1.
             A[0]=fabs(N[0]);
             A[1]=fabs(N[1]);
             A[2]=fabs(N[2]);
        */

        Coord3D  A;
        A.cX = fabs(N.cX);
        A.cY = fabs(N.cY);
        A.cZ = fabs(N.cZ);

        // exceptionally ugly kluge to force fit
        Point2D pV0, pV1, pV2, pU0, pU1, pU2;

        if (A.cX > A.cY)
            {
                if (A.cX > A.cZ)
                    {
                        // x is greatest
                        pV0.pX = V0.cY;
                        pV1.pX = V1.cY;
                        pV2.pX = V2.cY;
                        pU0.pX = U0.cY;
                        pU1.pX = U1.cY;
                        pU2.pX = U2.cY;

                        pV0.pY = V0.cZ;
                        pV1.pY = V1.cZ;
                        pV2.pY = V2.cZ;
                        pU0.pY = U0.cZ;
                        pU1.pY = U1.cZ;
                        pU2.pY = U2.cZ;
                    }
                else
                    {
                        // z is greatest
                        pV0.pX = V0.cX;
                        pV1.pX = V1.cX;
                        pV2.pX = V2.cX;
                        pU0.pX = U0.cX;
                        pU1.pX = U1.cX;
                        pU2.pX = U2.cX;

                        pV0.pY = V0.cY;
                        pV1.pY = V1.cY;
                        pV2.pY = V2.cY;
                        pU0.pY = U0.cY;
                        pU1.pY = U1.cY;
                        pU2.pY = U2.cY;
                    }
            }
        else
            {
                if (A.cZ > A.cY)
                    {
                        // z is greatest
                        pV0.pX = V0.cX;
                        pV1.pX = V1.cX;
                        pV2.pX = V2.cX;
                        pU0.pX = U0.cX;
                        pU1.pX = U1.cX;
                        pU2.pX = U2.cX;

                        pV0.pY = V0.cY;
                        pV1.pY = V1.cY;
                        pV2.pY = V2.cY;
                        pU0.pY = U0.cY;
                        pU1.pY = U1.cY;
                        pU2.pY = U2.cY;
                    }
                else
                    {
                        // y is greates
                        pV0.pX = V0.cX;
                        pV1.pX = V1.cX;
                        pV2.pX = V2.cX;
                        pU0.pX = U0.cX;
                        pU1.pX = U1.cX;
                        pU2.pX = U2.cX;

                        pV0.pY = V0.cZ;
                        pV1.pY = V1.cZ;
                        pV2.pY = V2.cZ;
                        pU0.pY = U0.cZ;
                        pU1.pY = U1.cZ;
                        pU2.pY = U2.cZ;
                    }
            }


        /*

          if(A[0]>A[1])
          {
          if(A[0]>A[2])
          {
          i0=1;      /* A[0] is greatest
          i1=2;
          }
          else
          {
          i0=0;      /* A[2] is greatest
          i1=1;
          }
          }
          else   /* A[0]<=A[1]
          {
          if(A[2]>A[1])
          {
          i0=0;      /* A[2] is greatest
          i1=1;
          }
          else
          {
          i0=0;      /* A[1] is greatest
          i1=2;
          }
          }
        */

        return  EdgeAgainstTriEdges(pV0, pV1, pU0,  pU1, pU2) ||
            EdgeAgainstTriEdges(pV1, pV2, pU0,  pU1, pU2) ||
            EdgeAgainstTriEdges(pV2, pV0, pU0,  pU1, pU2) ||
            PointInTri(pV0, pU0,  pU1, pU2) ||
            PointInTri(pV1, pU0,  pU1, pU2);


        /*      test all edges of triangle 1 against the edges of triangle 2
                EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2);
                EDGE_AGAINST_TRI_EDGES(V1,V2,U0,U1,U2);
                EDGE_AGAINST_TRI_EDGES(V2,V0,U0,U1,U2); */

        /*      finally, test if tri1 is totally contained in tri2 or vice versa
                POINT_IN_TRI(V0,U0,U1,U2);
                POINT_IN_TRI(U0,V0,V1,V2); */

    }


    bool SC_PlaneCalcs::EdgeEdgeTest(const double& Ax,
                                     const double& Ay,
                                     const Point2D& V0,
                                     const Point2D& U0,
                                     const Point2D& U1)
    {
        /*
          Bx=U0[i0]-U1[i0];
          By=U0[i1]-U1[i1];
          Cx=V0[i0]-U0[i0];
          Cy=V0[i1]-U0[i1];
          f=Ay*Bx-Ax*By;
          d=By*Cx-Bx*Cy; */

        double Bx = U0.pX - U1.pX;
        double By = U0.pY - U1.pY;
        double Cx = V0.pX - U0.pX;
        double Cy = V0.pX - U0.pX;
        double f = Ay * Bx - Ax * By;
        double d = By * Cx - Bx * Cy;

        if(((f > 0.0) && (d >= 0.0) && (d <= f)) || ((f < 0.0) && (d <= 0.0) && (d >= f)))
            {
                double e = Ax * Cy - Ay * Cx;
                if(f > 0.0)
                    {
                        if((e >= 0.0) && (e <= f))
                            return true;
                    }
                else
                    {
                        if (( e <= 0.0) && (e >= f))
                            return true;
                    }
            }
        return false;
    }



    bool SC_PlaneCalcs::EdgeAgainstTriEdges(const Point2D& V0,
                                            const Point2D& V1,
                                            const Point2D& U0,
                                            const Point2D& U1,
                                            const Point2D& U2)
    {
        /*      float Ax,Ay,Bx,By,Cx,Cy,e,d,f;
                Ax=V1[i0]-V0[i0];
                Ay=V1[i1]-V0[i1];*/

        double Ax = V1.pX - V0.pX;
        double Ay = V1.pY - V0.pY;

        return  EdgeEdgeTest(Ax, Ay, V0, U0, U1) ||
            EdgeEdgeTest(Ax, Ay, V0, U1, U2) ||
            EdgeEdgeTest(Ax, Ay, V0, U2, U0);

        /* test edge U0,U1 against V0,V1
           EDGE_EDGE_TEST(V0,U0,U1);
           /* test edge U1,U2 against V0,V1
           EDGE_EDGE_TEST(V0,U1,U2);
           /* test edge U2,U1 against V0,V1/
           EDGE_EDGE_TEST(V0,U2,U0);
        */
    }

    //  POINT_IN_TRI(V0,U0,U1,U2)

    bool SC_PlaneCalcs::PointInTri(const Point2D& V0,
                                   const Point2D& U0,
                                   const Point2D& U1,
                                   const Point2D& U2)
    {
        //  float a,b,c,d0,d1,d2;
        /* is T1 completly inside T2?
           /* check if V0 is inside tri(U0,U1,U2)
           a=U1[i1]-U0[i1];
           b=-(U1[i0]-U0[i0]);
           c=-a*U0[i0]-b*U0[i1];
           d0=a*V0[i0]+b*V0[i1]+c; */

        double a = U1.pY - U0.pY;
        double b = -(U1.pX - U0.pX);
        double c = -a * U0.pX - b * U0.pY;
        double d0 = a * V0.pX + b * V0.pY + c;


        /*  a=U2[i1]-U1[i1];
            b=-(U2[i0]-U1[i0]);
            c=-a*U1[i0]-b*U1[i1];
            d1=a*V0[i0]+b*V0[i1]+c; */

        a = U2.pY - U1.pY;
        b = -(U2.pX - U1.pX);
        c = -a * U1.pX - b * U1.pY;
        double d1 = a * V0.pX + b * V0.pY + c;


        /*      a=U0[i1]-U2[i1];
                b=-(U0[i0]-U2[i0]);
                c=-a*U2[i0]-b*U2[i1];
                d2=a*V0[i0]+b*V0[i1]+c; */

        a = U0.pY - U2.pY;
        b = -(U0.pX - U2.pX);
        c = -a * U2.pX - b * U2.pY;
        double d2 = a * V0.pX + b * V0.pY + c;


        /*  if(d0*d1>0.0)
            {
            if(d0*d2>0.0) return 1;
            }*/


        return ((d0 * d1 > 0.0) && (d0 * d2 > 0.0));

    }


};                                      // end namespace

