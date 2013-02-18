///////////////////////////////////////////////////////////////////////////////////
//
// SC_PointArray.cpp
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

#include <genClass/SC_LineArray.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/SC_IntArray.h>

#include <genClass/SC_PointArray.h>

namespace genCommonTypes {

    SC_PointArray& SC_PointArray::operator= (const SC_CoordArray& a)
    {
        Alloc(a.Size());
        nlistData = a.Size();
        for ( int i = 0; i < nlistData; i++)
            {
                tListData[i].pX = a[i].cX;
                tListData[i].pY = a[i].cY;
            }
        return *this;
    }

    void  SC_PointArray::CalcMinMax(Point2D& minPt,
                                    Point2D& maxPt) const
    {
        minPt = Point2D();
        maxPt = minPt;
        for (int i = 0; i < Size(); i++)
            {
                if (tListData[i].PointIsNull())
                    continue;

                if ( minPt.PointIsNull())
                    {
                        minPt = tListData[i];
                        maxPt = minPt;
                        continue;
                    }

                double xPt = tListData[i].pX;
                double yPt = tListData[i].pY;
                if (xPt < minPt.pX)
                    minPt.pX = xPt;
                else if (xPt > maxPt.pX)
                    maxPt.pX = xPt;

                if (yPt < minPt.pY)
                    minPt.pY = yPt;
                else if (yPt > maxPt.pY)
                    maxPt.pY = yPt;
            }
    }


    bool SC_PointArray::MayOverlap(const Limit2D& limits)
    {
        int  overX  = 0;
        int  underX = 0;
        int  overY  = 0;
        int  underY = 0;

        for (int i = 0; i < Size(); i++)
            {
                double currX = (*this)[i].pX;
                double currY = (*this)[i].pY;

                if (currX < limits.minLim.pX)
                    underX++;
                else if (currX > limits.maxLim.pX)
                    overX++;

                if (currY < limits.minLim.pY)
                    underY++;
                else if (currY > limits.maxLim.pY)
                    overY++;
            }

        return  ((underX < Size()) && (overX < Size()) ||
                 (underY < Size()) && (overY < Size()));
    }


    void SC_PointArray::Clean(const double ptEps)
    {
        int npts = Size();
        int i;
        for (i = 0; i < npts - 1; i++)
            {
                Point2D& currPt = (*this)[i];
                if (currPt.PointIsNull())
                    continue;

                for (int j = i + 1; j < npts; j++)
                    {
                        Point2D& otherPt = (*this)[j];
                        if ((!otherPt.PointIsNull()) && (currPt.Distance(otherPt) < ptEps))
                            otherPt = Point2D();
                    }
            }

        int nOK = 0;
        for (i = 0; i < npts; i++)
            if (!(*this)[i].PointIsNull())
                (*this)[nOK++] = (*this)[i];
        SetSize(nOK);
    }




    void SC_PointArray::GetLine(int     indx,           // gets line segments
                                Line2D& segLine) const // seg[Size() - 1] goes from end to st
    {
        segLine.stPt = (*this)[indx];
        if (indx == UpperBound())
            segLine.endPt = (*this)[0];
        else
            segLine.endPt = (*this)[indx + 1];
    }

    bool  SC_PointArray::ConvertLines(const SC_Line2DArray& inLines,
                                      const double&          ptEps)
    {
        if ( inLines.IsEmpty())
            {
                SetEmpty();
                return true;
            }

        Alloc(inLines.Size() * 2);

        // add first seg
        (*this) += inLines[0].stPt;
        (*this) += inLines[0].endPt;

        Point2D segStart, segEnd, stPoly, endPoly;
        SC_BoolArray linesDone;
        linesDone.AllocAndFill(inLines.Size(), false);

        int nlinesDone = 1;
        while (nlinesDone < inLines.Size())
            {
                bool endFound = false;
                stPoly = (*this)[0];
                endPoly = LastIndex();

                for (int i = 1; i < inLines.Size(); i++)
                    {
                        if (linesDone[i])
                            continue;

                        segStart = inLines[i].stPt;
                        segEnd = inLines[i].endPt;

                        if (nlinesDone == (inLines.Size() - 1))
                            {
                                //  check that its not a seg connecting start and end
                                if ((segStart.SamePoint(stPoly, ptEps) && segEnd.SamePoint(endPoly, ptEps)) ||
                                    (segStart.SamePoint(endPoly, ptEps) && segEnd.SamePoint(stPoly, ptEps)))
                                    return true;
                            }

                        if (segStart.SamePoint(stPoly, ptEps))
                            {
                                InsertItem(segEnd, 0);
                                endFound = true;
                            }
                        else if (segStart.SamePoint(endPoly, ptEps))
                            {
                                (*this) += segEnd;
                                endFound = true;
                            }
                        else if (segEnd.SamePoint(stPoly, ptEps))
                            {
                                InsertItem(segStart, 0);
                                endFound = true;
                            }
                        else if (segEnd.SamePoint(endPoly, ptEps))
                            {
                                (*this) += segStart;
                                endFound = true;
                            }

                        if ( endFound)
                            {
                                linesDone[i] = true;
                                nlinesDone++;
                                break;
                            }
                    }

                if (!endFound)
                    return false;
            }

        return true;
    }

    bool  SC_PointArray::ConvertMultiLines(const SC_Line2DArray& inLines,
                                           const double&          ptEps)
    {
        if ( inLines.IsEmpty())
            {
                SetEmpty();
                return true;
            }

        Alloc(inLines.Size() * 2);

        // add first seg
        (*this) += inLines[0].stPt;
        (*this) += inLines[0].endPt;

        Point2D segStart, segEnd, stPoly, endPoly;
        SC_BoolArray linesDone;
        linesDone.AllocAndFill(inLines.Size(), false);

        int nlinesDone = 1;
        int nlinesCurrPoly = 1;
        int stPolyIndx = 0;
        while (nlinesDone < inLines.Size())
            {
                bool endFound = false;
                stPoly = (*this)[stPolyIndx];
                endPoly = LastIndex();

                for (int i = 1; i < inLines.Size(); i++)
                    {
                        if (linesDone[i])
                            continue;

                        segStart = inLines[i].stPt;
                        segEnd = inLines[i].endPt;

                        if (nlinesDone == (inLines.Size() - 1))
                            {
                                //  check that its not a seg connecting start and end
                                if ((segStart.SamePoint(stPoly, ptEps) && segEnd.SamePoint(endPoly, ptEps)) ||
                                    (segStart.SamePoint(endPoly, ptEps) && segEnd.SamePoint(stPoly, ptEps)))
                                    {
                                        linesDone[i] = true;
                                        nlinesDone++;
                                        nlinesCurrPoly++;
                                        break;
                                    }
                            }

                        if (segStart.SamePoint(stPoly, ptEps))
                            {
                                InsertItem(segEnd, stPolyIndx);
                                endFound = true;
                            }
                        else if (segStart.SamePoint(endPoly, ptEps))
                            {
                                (*this) += segEnd;
                                endFound = true;
                            }
                        else if (segEnd.SamePoint(stPoly, ptEps))
                            {
                                InsertItem(segStart, stPolyIndx);
                                endFound = true;
                            }
                        else if (segEnd.SamePoint(endPoly, ptEps))
                            {
                                (*this) += segStart;
                                endFound = true;
                            }

                        if ( endFound)
                            {
                                linesDone[i] = true;
                                nlinesDone++;
                                nlinesCurrPoly++;
                                break;
                            }
                    }

                if (!endFound)
                    {
                        //check previous poly more than one line - if not, remove poly
                        if (nlinesCurrPoly == 1)
                            {
                                (*this).SetSize((*this).Size() - 3);
                            }
                        //start new poly
                        nlinesCurrPoly = 0;
                        for (int i = 1; i < inLines.Size(); i++)
                            {
                                if (linesDone[i])
                                    continue;
                                (*this) += Point2D();
                                (*this) += inLines[i].stPt;
                                (*this) += inLines[i].endPt;
                                stPolyIndx = Size() - 1;
                                break;
                            }
                    }
            }

        return true;
    }

    /*
      int InsidePolygon(Point *polygon,int N,Point p)
      {
      int counter = 0;
      int i;
      double xinters;
      Point p1,p2;

      p1 = polygon[0];
      for (i=1;i<=N;i++) {
      p2 = polygon[i % N];
      if (p.y > MIN(p1.y,p2.y)) {
      if (p.y <= MAX(p1.y,p2.y)) {
      if (p.x <= MAX(p1.x,p2.x)) {
      if (p1.y != p2.y) {
      xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
      if (p1.x == p2.x || p.x <= xinters)
      counter++;
      }
      }
      }
      }
      p1 = p2;
      }

      if (counter % 2 == 0)
      return(OUTSIDE);
      else
      return(INSIDE);
      }
    */

    bool SC_PointArray::PointInPoly(const double&    xPt,
                                    const double&    yPt) const
    {
        /*
         *
         * Code off net - Randolph Franklin's algorithm PNPOLY
         *
         *     int pnpoly(int npol, float *xp, float *yp, float x, float y)
         int i, j, c = 0;
         for (i = 0, j = npol-1; i < npol; j = i++) {
         if ((((yp[i] <= y) && (y < yp[j])) ||
         ((yp[j] <= y) && (y < yp[i]))) &&
         (x < (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))
         c = !c;
         }
         return c;
        */

        int npol = Size();
        if (npol < 3)
            return false;

        bool c = false;
        for (int i = 0, j = npol-1; i < npol; j = i++)
            {
                double ypi = tListData[i].pY;
                double xpi = tListData[i].pX;
                double ypj = tListData[j].pY;
                double xpj = tListData[j].pX;
                if ((((ypi <= yPt) && (yPt < ypj)) ||
                     ((ypj <= yPt) && (yPt < ypi))) &&
                    (xPt < (xpj - xpi) * (yPt - ypi) / (ypj - ypi) + xpi))
                    c = !c;
            }
        return c;

    }


    bool SC_PointArray::PointInPoly(const Point2D&    pt) const
    {
        return PointInPoly(pt.pX, pt.pY);
    }

    double SC_PointArray::CalcPolyArea() const
    {
        if (Size() < 3)
            return 0.0;

        //  area calculation  from Programmers Geometry - sect 4.6
        //  calculate area of trapezia under each side down to a y axis
        //  y origin is reset to be equal to y position of last point

        double yOrigin = tListData[nlistData - 1].pY;

        double xOld    = tListData[nlistData - 1].pX;
        double yOld    = 0.0;
        double area    = 0.0;

        for ( int i = 0; i < nlistData; i++)
            {
                double x = tListData[i].pX;
                double y = tListData[i].pY - yOrigin;
                area += (xOld - x) * (yOld + y);
                xOld = x;
                yOld = y;
            }
        return area / 2.0;
    }

    void SC_PointArray::SetPolyClockwise()
    {
        if (Size() < 3)
            return;

        if (CalcPolyArea() < 0.0)
            {
                Point2D tmpPoint;
                int lastPoint = Size() - 1;
                int midPoint = Size() / 2;

                for (int i = 0; i < midPoint; i++)
                    {
                        tmpPoint = tListData[i];
                        tListData[i] = tListData[lastPoint];
                        tListData[lastPoint--] = tmpPoint;
                    }
            }
    }



    Point2D SC_PointArray::CalcPolyCentroid() const
    //  centroid calculation  from Programmers Geometry - sect 4.7
    {
        double xcg = 0.0;
        double ycg = 0.0;

        double xcom    = tListData[nlistData - 1].pX;
        double ycom    = tListData[nlistData - 1].pY;

        double xold    = tListData[0].pX;
        double yold    = tListData[0].pY;
        double area    = 0.0;

        for (int i = 1; i < (nlistData - 1); i++)
            {
                double x = tListData[i].pX;
                double y = tListData[i].pY;
                double aretri = (xcom - x) * (yold - ycom) + (xold - xcom) * (y - ycom);
                xcg += aretri * (x + xold);
                ycg += aretri * (y + yold);
                xold = x;
                yold = y;
                area += aretri;
            }

        if (area < stdEps)
            {
                // changed for FB Case 88
                double xavg    = tListData[0].pX;
                double yavg    = tListData[0].pY;
                for (int i = 1; i < nlistData; i++)
                    {
                        xavg += tListData[i].pX;
                        yavg += tListData[i].pY;
                    }
                return Point2D(xavg / double(nlistData), yavg / double(nlistData));
            }

        double areainv = 1.0 / area;
        xcg = (xcg * areainv + xcom) / 3.0;
        ycg = (ycg * areainv + ycom) / 3.0;
        return Point2D(xcg, ycg);
    }
    static const double detEps = 1E-6;

    bool  SC_PointArray::IntersectLine(const Line2D& inLine,  // crops inLine to portion in polygon
                                       Line2D& outLine,
                                       bool    extendStart,
                                       bool    extendEnd) const
    {
        Line2D  polySeg;
        outLine = inLine;

        //  kluge for lines through vertices
        double  tVals[8];
        int ntVal = 0;
        int ntOK = 0;

        //    Point2D testPt(173500.0, 237200.0);
        //    bool doPrint = (inLine.stPt.SamePoint(testPt, 1.0) || inLine.endPt.SamePoint(testPt, 1.0));

        for (int i = 0; i < Size(); i++)
            {
                //  side of polygon
                GetLine(i, polySeg);
                double tVal, sVal;
                // inline through side ?
                if ((!inLine.Intersect(polySeg, tVal, sVal, detEps)) || (sVal < 0.0) || (sVal > 1.0))
                    continue;

                // yes, determine if intersection may be OK
                bool setNext = false;
                if (extendStart && (tVal < 0.0))
                    setNext = true;
                if (extendEnd && (tVal > 1.0))
                    setNext = true;
                if ((tVal >= 0.0) && (tVal <= 1.0))
                    setNext = true;

                if (setNext)
                    {
                        if (ntVal == 8)
                            GenAppInternalError("SC_PointArray::IntersectLine");

                        //  OK add to list of tVals
                        tVals[ntVal++] = tVal;

                        //  check if it may be vertice ?
                        if ((sVal > 0.001) && (sVal < 0.999))
                            {
                                ntOK++;
                                // at least two not on vertices
                                if ( ntOK==2 )
                                    break;
                            }
                    }
            }

        // less than two OK -- check endpoints
        if (ntOK < 2)
            {
                if (PointInPoly(inLine.stPt))
                    tVals[ntVal++] = 0.0;
                if (PointInPoly(inLine.endPt))
                    tVals[ntVal++] = 1.0;
            }

        //  most likely case
        if (ntVal == 2)
            {
                if (tVals[0] < tVals[1])
                    {
                        outLine.stPt = inLine.PointOnLine(tVals[0]);
                        outLine.endPt = inLine.PointOnLine(tVals[1]);
                    }
                else
                    {
                        outLine.endPt = inLine.PointOnLine(tVals[0]);
                        outLine.stPt = inLine.PointOnLine(tVals[1]);
                    }
                return true;
            }

        if (ntVal < 2)
            return false;

        //  sort tVals
        for (int j = 0; j < (ntVal - 1); j++)
            for (int k =  j + 1; k < ntVal; k++)
                if (tVals[j] > tVals[k])
                    {
                        double tmp = tVals[j];
                        tVals[j] = tVals[k];
                        tVals[k] = tmp;
                    }
        //
        outLine.stPt = inLine.PointOnLine(tVals[0]);
        outLine.endPt = inLine.PointOnLine(tVals[ntVal - 1]);
        return true;

        //  sort tVals -- stupid sort

        /*

          bool stCheck = false;
          if (stSet)
          outLine.stPt = inLine.PointOnLine(stVal);
          else
          {
          outLine.stPt = inLine.stPt;
          stSet = PointInPoly(inLine.stPt);
          stCheck = true;
          }

          if (endSet)
          outLine.endPt = inLine.PointOnLine(endVal);
          else
          {
          outLine.endPt = inLine.endPt;
          endSet = PointInPoly(inLine.endPt);
          if (!(endSet || stCheck))
          {
          endSet = PointInPoly(inLine.stPt);
          if (endSet)
          {
          //  swap start and end
          outLine.endPt = outLine.stPt;
          outLine.stPt = inLine.stPt;
          }
          }
          }

          return stSet && endSet;
        */
    }


    bool  SC_PointArray::GetNextSegment(int&             segStart,
                                        SC_PointArray&   nextSegment) const
    {
        nextSegment.SetEmpty();
        if (segStart >= Size())
            return false;

        bool pointOK = false;
        for (int i = segStart; i < Size(); i++)
            if (tListData[i].PointIsNull())
                {
                    // one or more points found -- return segment
                    if (pointOK)
                        {
                            segStart = i;
                            return true;
                        }
                }
            else
                {
                    pointOK = true;
                    nextSegment += tListData[i];
                }

        segStart = Size();

        return !nextSegment.IsEmpty();
    }


    void SC_PointArray::Offset(const Point2D&   offsetPt)
    {
        for (int i = 0; i < Size(); i++)
            if (!tListData[i].PointIsNull())
                {
                    tListData[i].pX += offsetPt.pX;
                    tListData[i].pY += offsetPt.pY;
                }
    }


    void SC_PointArray::Transpose()
    {
        for (int i = 0; i < Size(); i++)
            {
                double temp = tListData[i].pX;
                tListData[i].pX = tListData[i].pY;
                tListData[i].pY = temp;
            }
    }



    SC_PointArray::PolyEdge::PolyEdge(const PolyEdge& a) :
        interPt(a.interPt), edgeIndx(a.edgeIndx), dist(a.dist)
    {
    }

    SC_PointArray::PolyEdge& SC_PointArray::PolyEdge::operator=(const PolyEdge& a)
    {
        if (&a != this)
            {
                interPt = a.interPt;
                edgeIndx = a.edgeIndx;
                dist = a.dist;
            }
        return *this;
    }

    void SC_PointArray::PolyEdgeArray::SortEdges()
    {
        for (int i = 0; i < UpperBound(); i++)
            for (int j = i + 1; j < Size(); j++)
                if ((*this)[j].dist < (*this)[i].dist)
                    {
                        PolyEdge tmp((*this)[i]);
                        (*this)[i] = (*this)[j];
                        (*this)[j] = tmp;
                    }
    }


    void SC_PointArray::AddIfNew(const Point2D&  newPt)
    {
        for (int i = 0; i < Size(); i++)
            if (newPt.SamePoint((*this)[i]))
                return;

        (*this) += newPt;
    }




    void SC_PointArray::CheckPoly2(                 int                     startEdge,
                                                    int                     endEdge,
                                                    const SC_PointArray& poly1,
                                                    SC_PointArray& addPoly) const

    {
        if (endEdge < startEdge)
            endEdge += Size();

        int ptIndx;
        for (int i = startEdge; i <= endEdge; i++)
            {
                if (i < Size())
                    ptIndx = i;
                else
                    ptIndx = i - Size();
                if (poly1.PointInPoly((*this)[ptIndx]))
                    addPoly.AddIfNew((*this)[ptIndx]);
            }

        /*
          IF end_edge < start_edge THEN
          end_edge := end_edge + n_poly_2;

          FOR i : WORD := start_edge TO end_edge DO BEGIN
          IF i > n_poly_2 THEN
          pt_num := i - n_poly_2
          ELSE
          pt_num := i;
          IF inside(poly_2^[pt_num], poly_1, n_poly_1, poly_1_map, x_max_1) THEN
          add_to_poly(poly_2^[pt_num]);

          END;
        */
    }


    bool SC_PointArray::PolyIntersect(const SC_PointArray& poly1,  // ret true if intersect
                                      const SC_PointArray& poly2)
    {

        Alloc((poly1.Size() + poly2.Size()) * 2);

        PolyEdgeArray edges;
        edges.Alloc(poly2.Size());

        PolyEdge interEdge;
        // modified pascal code

        Line2D  line1, line2;

        int lastEdge  = -1;
        int firstEdge = -1;

        for (int j = 0; j < poly1.Size(); j++)
            {
                poly1.GetLine(j, line1);
                if (poly2.PointInPoly(line1.stPt))
                    AddIfNew(line1.stPt);
                /*
                  FOR j : WORD := 1 TO n_poly_1 DO BEGIN
                  edge_start := poly_1^[j];
                  IF j < n_poly_1 THEN
                  edge_end   := poly_1^[j + 1]
                  ELSE
                  edge_end   := poly_1^[1];

                  IF inside(edge_start, poly_2, n_poly_2, poly_2_map, x_max_2) THEN
                  add_to_poly(edge_start);
                */
                edges.SetEmpty();
                int k;
                for (k = 0; k < poly2.Size(); k++)
                    {
                        poly2.GetLine(k, line2);
                        if (line2.Intersect(line1, interEdge.interPt, detEps))
                            {
                                interEdge.edgeIndx = k;
                                interEdge.dist= line1.stPt.Distance(interEdge.interPt);
                                edges += interEdge;
                            }
                    }

                /*
                  -- find all edges
                  n_edge := 0;
                  FOR k : WORD := 1 TO n_poly_2 DO BEGIN

                  IF k < n_poly_2 THEN
                  k_end := k + 1
                  ELSE
                  k_end := 1;

                  IF edge_intersect(edge_start, edge_end,
                  poly_2^[k], poly_2^[k_end],
                  parallel, inter_point) THEN
                  BEGIN
                  n_edge := n_edge + 1;
                  WITH poly_edges^[n_edge] DO BEGIN
                  i_point := inter_point;
                  edge_num := k;
                  edge_dist :=
                  SQR(inter_point.poly_x - edge_start.poly_x) +
                  SQR(inter_point.poly_y - edge_start.poly_y);
                  END;
                  END;
                  END;
                */
                if (edges.IsEmpty())
                    continue;

                edges.SortEdges();

                for (k = 0; k < edges.Size(); k++)
                    {
                        int currEdge = edges[k].edgeIndx;
                        if ((lastEdge >= 0) && (lastEdge != currEdge))
                            poly2.CheckPoly2(lastEdge + 1, currEdge, poly1, *this);
                        lastEdge = currEdge;
                        if (firstEdge < 0)
                            firstEdge = currEdge;
                        AddIfNew(edges[k].interPt);
                    }

                /*          IF n_edge = 0 THEN
                            CYCLE;

                            poly_edge_sort(poly_edges, n_edge);

                            FOR k : WORD := 1 TO n_edge DO
                            WITH poly_edges^[k] DO BEGIN
                            IF (last_edge > 0) AND (last_edge <> edge_num) THEN
                            check_poly_2(last_edge + 1, edge_num);
                            last_edge := edge_num;
                            IF first_edge = 0 THEN
                            first_edge := last_edge;

                            add_to_poly(i_point);
                            END;

                */

            }

        if (firstEdge < 0)
            poly2.CheckPoly2(0, poly2.UpperBound(), poly1, *this);
        else
            poly2.CheckPoly2(lastEdge + 1, firstEdge, poly1, *this);

        /*
          IF first_edge = 0 THEN
          check_poly_2(1, n_poly_2)
          ELSE
          IF last_edge <> first_edge THEN
          check_poly_2(last_edge + 1, first_edge);
        */

        return Size() > 0;
    }


    /*

      VAR
      max_inter, last_edge, k_end, first_edge, n_edge : WORD;
      edge_start, edge_end, inter_point : poly_point;
      poly_1_map, poly_2_map : poly_map_ptr;
      x_max_1, x_max_2 : REAL8;
      poly_edges : poly_edge_ptr;
      parallel   : BOOLEAN;

      PROCEDURE add_to_poly(CONST  pt_to_add : poly_point);
      BEGIN
      FOR i : WORD := 1 TO n_inter DO
      IF same_poly_point(pt_to_add, inter_poly^[i]) THEN
      RETURN;

      n_inter := n_inter + 1;
      inter_poly^[n_inter] := pt_to_add;
      END;


      PROCEDURE check_poly_2(start_edge : WORD;
      end_edge   : WORD);
      VAR
      pt_num : WORD;
      BEGIN
      IF end_edge < start_edge THEN
      end_edge := end_edge + n_poly_2;

      FOR i : WORD := start_edge TO end_edge DO BEGIN
      IF i > n_poly_2 THEN
      pt_num := i - n_poly_2
      ELSE
      pt_num := i;
      IF inside(poly_2^[pt_num], poly_1, n_poly_1, poly_1_map, x_max_1) THEN
      add_to_poly(poly_2^[pt_num]);

      END;
      END;


      BEGIN
      poly_intersect := FALSE;
      poly_1_map := NIL;
      poly_2_map := NIL;
      poly_edges := NIL;
      null_poly_ptr(inter_poly);

      IF NOT poly_map_set_up(poly_1, n_poly_1, poly_1_map, x_max_1,
      poly_2, n_poly_2, poly_2_map, x_max_2) THEN
      GOTO proc_exit;

      n_inter := 0;
      max_inter := (n_poly_1 + n_poly_2) * 2;
      IF NOT set_poly_ptr(inter_poly, max_inter) THEN
      GOTO proc_exit;

      IF NOT check_and_get_memory(RETYPE(poly_edges, Pointer), SIZEOF_ELEMENTOF(poly_edges^) * n_poly_2) THEN
      GOTO proc_exit;

      last_edge  := 0;
      first_edge := 0;
      FOR j : WORD := 1 TO n_poly_1 DO BEGIN
      edge_start := poly_1^[j];
      IF j < n_poly_1 THEN
      edge_end   := poly_1^[j + 1]
      ELSE
      edge_end   := poly_1^[1];

      IF inside(edge_start, poly_2, n_poly_2, poly_2_map, x_max_2) THEN
      add_to_poly(edge_start);

      -- find all edges
      n_edge := 0;
      FOR k : WORD := 1 TO n_poly_2 DO BEGIN

      IF k < n_poly_2 THEN
      k_end := k + 1
      ELSE
      k_end := 1;

      IF edge_intersect(edge_start, edge_end,
      poly_2^[k], poly_2^[k_end],
      parallel, inter_point) THEN
      BEGIN
      n_edge := n_edge + 1;
      WITH poly_edges^[n_edge] DO BEGIN
      i_point := inter_point;
      edge_num := k;
      edge_dist :=
      SQR(inter_point.poly_x - edge_start.poly_x) +
      SQR(inter_point.poly_y - edge_start.poly_y);
      END;
      END;
      END;

      IF n_edge = 0 THEN
      CYCLE;

      poly_edge_sort(poly_edges, n_edge);

      FOR k : WORD := 1 TO n_edge DO
      WITH poly_edges^[k] DO BEGIN
      IF (last_edge > 0) AND (last_edge <> edge_num) THEN
      check_poly_2(last_edge + 1, edge_num);
      last_edge := edge_num;
      IF first_edge = 0 THEN
      first_edge := last_edge;

      add_to_poly(i_point);
      END;
      END;

      IF first_edge = 0 THEN
      check_poly_2(1, n_poly_2)
      ELSE
      IF last_edge <> first_edge THEN
      check_poly_2(last_edge + 1, first_edge);

      poly_intersect := (n_inter > 0);


    */


    //  removes extra in-line points
    void SC_PointArray::RemoveInteriorPoints(SC_IntArray& pointIndexes,
                                             const double& tolerance)
    {
        if (Size() < 3)
            return;

        // null check
        int i;
        for (i = 0; i < Size(); i++)
            if (tListData[i].PointIsNull())
                return;


        Point2D lastOK, curr, next;
        Line2D testLine;

        //  first point always OK
        lastOK = tListData[0];
        int nextIndex = 1;

        for (i = 1; i < UpperBound(); i++)
            {
                curr = tListData[i];
                next = tListData[i + 1];
                // degenerate ?
                if ((curr == lastOK) || (curr == next))
                    continue;

                double currDist = curr.Distance(lastOK);
                double nextDist = next.Distance(lastOK);

                double tVal = nextDist/currDist;
                testLine.stPt = lastOK;
                testLine.endPt = curr;
                // next is further away and on line
                if ((tVal >= 1.0) && (next.SamePoint(testLine.PointOnLine(tVal), tolerance)))
                    continue;

                //  next nearer than curr or not on line
                pointIndexes[nextIndex] = pointIndexes[i];
                tListData[nextIndex++] = curr;
                lastOK = curr;
            }

        // add end point
        pointIndexes[nextIndex] = pointIndexes[UpperBound()];
        tListData[nextIndex++] = next;

        // set size
        pointIndexes.SetSize(nextIndex);
        SetSize(nextIndex);
    }


};                                      // end namespace

