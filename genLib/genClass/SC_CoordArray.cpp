///////////////////////////////////////////////////////////////////////////////////
//
// SC_CoordArray.cpp
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

#include <genClass/SC_PointArray.h>
#include <genClass/SC_DoubleArray.h>

#include <genClass/SC_CoordArray.h>


namespace genCommonTypes {

    void  SC_CoordArray::SetNull()
    {
        Coord3D nullCoord;
        FillToAlloc(nullCoord);
    }

    void  SC_CoordArray::ConvertPoints(const SC_PointArray& inPoints,
                                       const double&        pointZ)
    {
        AllocAndSetSize(inPoints.Size());
        for ( int i = 0; i < Size(); i++)
            tListData[i] = Coord3D(inPoints[i], pointZ);
    }

    void  SC_CoordArray::ConvertPoints(const SC_PointArray& inPoints,
                                       const double&        pointZ,
                                       Plane3D              modelPlane)
    {
        AllocAndSetSize(inPoints.Size());
        for ( int i = 0; i < Size(); i++)
            {
                tListData[i] = Coord3D(inPoints[i], pointZ);
                tListData[i].FromModelPlane(modelPlane);
            }
    }

    void  SC_CoordArray::FromModelPlane(Plane3D modelPlane)
    {
        if (modelPlane != p3D_XY)
            for (int i = 0; i < Size(); i++)
                tListData[i].FromModelPlane(modelPlane);
    }

    void  SC_CoordArray::ToPlotPlane(Plane3D plotPlane)
    {
        if (plotPlane != p3D_XY)
            for ( int i = 0; i < Size(); i++)
                tListData[i].ToPlotPlane(plotPlane);
    }


    void SC_CoordArray:: Reset(int  maxList)
    {
        if (maxList > nlistAlloc)
            ReAlloc(maxList);

        int nOK = 0;
        for ( int i = 0; i < nlistData; i++)
            if (! tListData[i].CoordIsNull())
                {
                    tListData[nOK] = tListData[i];
                    nOK++;
                }
        nlistData = nOK;

    }
    void SC_CoordArray:: Clear()
    {
        if (nlistAlloc == 0)
            ReAlloc(1);

        tListData[0] = Coord3D();
        nlistData = 0;
    }

    void SC_CoordArray::SetCoordZ(const double& zVal)
    {
        for (int i = 0; i < nlistData; i++)
            if (!tListData[i].CoordIsNull())
                tListData[i].cZ = zVal;
    }

    void SC_CoordArray::GetLine(int     indx,           // gets line segments
                                Line3D& segLine) const  // seg[Size() - 1] goes from end to st
    {
        segLine.stPt = (*this)[indx];
        if (indx == (Size() - 1))
            segLine.endPt = (*this)[0];
        else
            segLine.endPt = (*this)[indx + 1];
    }


    void  SC_CoordArray::NextNonDegeneratePoint(int&         inext,
                                                double&      len,
                                                Coord3D&     diff) const
    {
        double slen;
        do
            {
                // get distance to next point
                diff = (*this)[inext + 1] - (*this) [inext];
                len = diff.Magnitude();

                Coord3D sum = (*this)[inext + 1] + (*this) [inext];
                slen = sum.Magnitude();
                slen *= Coord3D::degenerateTolerance;
                inext ++;
            } while ((len <= slen) && (inext < Size() -1 ));
    }


    bool  SC_CoordArray::GetNextSegment(int&             segStart,
                                        SC_CoordArray&   nextSegment) const
    {
        nextSegment.SetEmpty();
        if (segStart >= Size())
            return false;

        bool coordOK = false;
        for (int i = segStart; i < Size(); i++)
            if (tListData[i].CoordIsNull())
                {
                    // one or more Coords found -- return segment
                    if (coordOK)
                        {
                            segStart = i;
                            return true;
                        }
                }
            else
                {
                    coordOK = true;
                    if ((!nextSegment.IsFull()) || nextSegment.IsResizable())
                        nextSegment += tListData[i];
                }

        segStart = Size();

        return !nextSegment.IsEmpty();
    }


    void SC_CoordArray::Offset(const Coord3D&   offsetCoord)
    {
        for (int i = 0; i < Size(); i++)
            if (!tListData[i].CoordIsNull())
                tListData[i] += offsetCoord;
    }

    //  returns true if line was extracted  (t < 1, s > 0, not a point)
    bool SC_CoordArray::ExtractSegment(const double&    tVal,  //  0 - 1
                                       const double&    sVal,  // 0 - 1
                                       SC_CoordArray&   extrSegment) const
    {
        extrSegment.SetEmpty();
        if ((tVal >= 1.0)|| (sVal <= 0.0) || (sVal <= tVal) || (Size() < 2))
            return false;

        // get segment lengths
        SC_DoubleArray  segLen(Size(), 0.0);
        double totalLen = 0.0;
        int i;
        for (i = 1; i < Size(); i++)
            if ((!tListData[i].CoordIsNull()) && (!tListData[i - 1].CoordIsNull()))
                {
                    segLen[i - 1] = tListData[i- 1].Distance(tListData[i]);
                    totalLen += segLen[i - 1];
                }

        // bunch of points or degenerate segment
        if (totalLen < stdEps)
            return false;

        double tDist = 0.0;
        if (tVal > 0.0)
            tDist = totalLen * tVal;

        double sDist = totalLen;
        if (sVal < 1.0)
            sDist = totalLen * sVal;

        Line3D extrLine;

        // find start point
        int startSeg = -1;
        for (i = 0; i < Size() - 1; i++)
            {
                if (segLen[i] > tDist)
                    {
                        extrLine.stPt = tListData[i];
                        extrLine.endPt = tListData[i + 1];
                        startSeg = i;
                        extrSegment += extrLine.PointOnLine(tDist / segLen[i]);

                        // check for s in same seg
                        if (segLen[i] > sDist)
                            {
                                extrSegment += extrLine.PointOnLine(sDist / segLen[i]);
                                //  end of processing exit
                                return true;
                            }

                        sDist -= segLen[i];
                        break;
                    }

                tDist -= segLen[i];
                sDist -= segLen[i];
            }

        if (startSeg < 0)
            GenAppInternalError("SC_CoordArray::ExtractSegment 1");

        // add points to end
        for (i = startSeg + 1; i < Size() - 1; i++)
            {
                //  add start point
                extrSegment += tListData[i];
                if (segLen[i] > sDist)
                    {
                        extrLine.stPt = tListData[i];
                        extrLine.endPt = tListData[i + 1];
                        extrSegment += extrLine.PointOnLine(sDist / segLen[i]);
                        //  end of processing exit
                        return true;
                    }
                sDist -= segLen[i];
            }

        // add last point
        extrSegment += LastIndex();
        return true;
    }

    //  specialized version for continous lines
    //  must be single segment
    //  returns true if line was extracted  (t < 1, s > 0, not a point)
    //  extrSegment is assumed allocd to current size
    bool SC_CoordArray::ExtractSimpleSegment(const double&    tVal,  // 0 - 1
                                             const double&    sVal,  // 0 - 1
                                             SC_CoordArray&   extrSegment) const
    {
        extrSegment.SetEmpty();
        if ((tVal > 1.05)|| (sVal < -0.05) || (sVal <= tVal) || (Size() < 2))
            return false;


        double totalLen = SegmentLength();

        double tDist = 0.0;
        if (tVal > 0.0)
            tDist = totalLen * tVal;

        double sDist = totalLen;
        if (sVal < 1.0)
            sDist = totalLen * sVal;

        Line3D extrLine;

        // find start point
        int startSeg = -1;
        for (int i = 0; i < UpperBound(); i++)
            {
                double segLen = (*this)[i].Distance((*this)[i+1]);
                if (segLen > tDist)
                    {
                        extrLine.stPt = tListData[i];
                        extrLine.endPt = tListData[i + 1];
                        startSeg = i;
                        extrSegment += extrLine.PointOnLine(tDist / segLen);

                        // check for s in same seg
                        if (segLen > sDist)
                            {
                                extrSegment += extrLine.PointOnLine(sDist / segLen);
                                //  end of processing exit
                                return true;
                            }

                        sDist -= segLen;
                        break;
                    }

                tDist -= segLen;
                sDist -= segLen;
            }

        if (startSeg < 0)
            GenAppInternalError("SC_CoordArray::ExtractSegment 1");

        // add points to end
        for (int i = startSeg + 1; i < UpperBound(); i++)
            {
                //  add start point
                extrSegment += tListData[i];
                double segLen = (*this)[i].Distance((*this)[i+1]);
                if (segLen > sDist)
                    {
                        extrLine.stPt = tListData[i];
                        extrLine.endPt = tListData[i + 1];
                        extrSegment += extrLine.PointOnLine(sDist / segLen);
                        //  end of processing exit
                        return true;
                    }
                sDist -= segLen;
            }

        // add last point
        extrSegment += LastIndex();
        return true;
    }

    //  specialized version for continous lines with total length precalc
    //  must be single segment
    //  returns true if point was extracted  (t < 1, s > 0, not a point)
    //  extrSegment is assumed allocd to current size
    bool SC_CoordArray::ExtractCoord(const double&    tVal,  //  0 - 1
                                     Coord3D&  tCoord) const
    {
        tCoord = Coord3D();
        if ((tVal > 1.05)|| (tVal < -0.05) || (Size() < 2))
            return false;

        if (tVal >= 1.0)
            {
                tCoord = LastIndex();
                return true;
            }
        if (tVal <= 0.0)
            {
                tCoord = (*this)[0];
                return true;
            }

        double totalLen = SegmentLength();

        double tDist = 0.0;
        if (tVal > 0.0)
            tDist = totalLen * tVal;

        Line3D extrLine;

        for (int i = 0; i < UpperBound(); i++)
            {
                double segLen = (*this)[i].Distance((*this)[i+1]);
                if (segLen > tDist)
                    {
                        extrLine.stPt = tListData[i];
                        extrLine.endPt = tListData[i + 1];
                        tCoord = extrLine.PointOnLine(tDist / segLen);
                        return true;
                    }
                tDist -= segLen;
            }

        tCoord = LastIndex();
        return true;
    }


    //  removes extra in-line points
    void SC_CoordArray::RemoveInteriorPoints(const double& tolerance)
    {
        if (Size() < 3)
            return;

        // null check
        int i;
        for (i = 0; i < Size(); i++)
            if (tListData[i].CoordIsNull())
                return;


        Coord3D lastOK, curr, next;
        Line3D testLine;

        //  first point always OK
        lastOK = tListData[0];
        int nextIndex = 1;

        for (i = 1; i < (Size() - 1); i++)
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
                if ((tVal >= 1.0) && (next.SameCoord(testLine.PointOnLine(tVal), tolerance)))
                    continue;

                //  next nearer than curr or not on line
                tListData[nextIndex++] = curr;
                lastOK = curr;
            }

        // add end point
        tListData[nextIndex++] = next;

        // set size
        SetSize(nextIndex);
    }

    Coord3D SC_CoordArray::CalcPolyCentroid() const
    {
        //from web: http://www.psc.edu/~burkardt/src/geometry/geometry.f90
        //claims to be based on Adrian Bowyer and John Woodwark,
        //A Programmer's Geometry, but I can't find it there
        //it is a similar algorithm to the 2D version in Point2D (NJC)

        double xcg = 0.0;
        double ycg = 0.0;
        double zcg = 0.0;

        double xcom    = tListData[nlistData - 1].cX;
        double ycom    = tListData[nlistData - 1].cY;
        double zcom    = tListData[nlistData - 1].cZ;

        double xold    = tListData[0].cX;
        double yold    = tListData[0].cY;
        double zold    = tListData[0].cZ;

        double area    = 0.0;

        Coord3D coordA, coordB, coordNorm;

        for (int i = 1; i < (nlistData - 1); i++)
            {
                double x = tListData[i].cX;
                double y = tListData[i].cY;
                double z = tListData[i].cZ;
                coordA.cX = x - xold;
                coordA.cY = y - yold;
                coordA.cZ = z - zold;
                coordB.cX = xcom - xold;
                coordB.cY = ycom - yold;
                coordB.cZ = zcom - zold;
                coordNorm = CrossProduct(coordA, coordB);
                double norm  = sqrt(coordNorm.cX * coordNorm.cX
                                    + coordNorm.cY * coordNorm.cY
                                    + coordNorm.cZ * coordNorm.cZ);
                double aretri = 0.5 * norm;
                xcg += aretri * (x + xold + xcom) / 3.0;
                ycg += aretri * (y + yold + ycom) / 3.0;
                zcg += aretri * (z + zold + zcom) / 3.0;
                xold = x;
                yold = y;
                zold = z;
                area += aretri;
            }

        if (area < stdEps)
            {
                return Coord3D(0.0);
            }

        double areainv = 1.0 / area;
        xcg = xcg * areainv;
        ycg = ycg * areainv;
        zcg = zcg * areainv;

        return Coord3D(xcg, ycg, zcg);
    }

    double  SC_CoordArray::PolyLength() const
    {
        return SegmentLength() + (*this)[0].Distance((*this)[UpperBound()]);
    }

    double  SC_CoordArray::SegmentLength() const
    {
        double segLen = 0.0;
        for (int i = 0; i < UpperBound(); i++)
            segLen += (*this)[i].Distance((*this)[i + 1]);
        return segLen;
    }


    //*******************************************************
    SC_CoordMatrix::SC_CoordMatrix(int nCols, int nRows)
    {
        MatrixAllocAndSetSize(nCols, nRows);
    }


    void SC_CoordMatrix:: MatrixAlloc(int nCols, int nRows)
    {
        DeAlloc();
        if ((nCols == 0) && (nRows == 0))
            return;

        if (nCols < 1)
            GenAppInternalError("SC_CoordMatrix::ChkAlloc");

        // set columns
        Alloc(nCols);

        //  each column
        for (int i = 0; i < nCols; i++)
            tListData[i].Alloc(nRows);
    }

    void  SC_CoordMatrix::MatrixAllocAndSetSize(int nCols, int nRows)
    {
        MatrixAlloc(nCols, nRows);
        MatrixSetSize(nCols, nRows);
    }

    void SC_CoordMatrix::MatrixSetSize(int nCols, int nRows)
    {
        SetSize(nCols);
        for (int i = 0; i < nCols; i++)
            tListData[i].SetSize(nRows);
    }

    void  SC_CoordMatrix::SetNull()
    {
        SetSize(AllocSize());
        for ( int i = 0; i < Size(); i++)
            tListData[i].SetNull();
    }


    void SC_CoordCube:: CubeAlloc(int nPage, int nCols, int nRows)
    {
        DeAlloc();
        if ((nPage == 0) && (nCols == 0) && (nRows == 0))
            return;

        if ((nCols < 1) || (nPage < 1))
            GenAppInternalError("SC_CoordMatrix::ChkAlloc");

        // set pages
        Alloc(nPage);

        //  each column
        for (int i = 0; i < nPage; i++)
            tListData[i].MatrixAlloc(nCols, nRows);
    }


};                                      // end namespace

