///////////////////////////////////////////////////////////////////////////////////
//
// SC_LineArray.cpp
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

#include <genClass/SC_LineArray.h>
#include <genClass/SC_PointArray.h>
#include <genClass/SC_CoordArray.h>

namespace genCommonTypes {

    void  SC_Line2DArray::ConvertPoints(const SC_PointArray& inPoints,
                                        bool           isPolygon)
    {
        //  max size
        int nLine = inPoints.Size() - 1;
        if (isPolygon)
            nLine++;
        Alloc(nLine);
        Line2D nextLine;
        for (int i = 0; i < nLine; i++)
            {
                inPoints.GetLine(i, nextLine);
                if (nextLine.stPt.PointIsNull() || nextLine.endPt.PointIsNull())
                    continue;
                (*this) += nextLine;
            }
    }

    void  SC_Line2DArray::ConvertCoords(const SC_CoordArray& inCoords,
                                        bool           isPolygon)
    {
        //  max size
        int nLine = inCoords.Size() - 1;
        if (isPolygon)
            nLine++;
        Alloc(nLine);
        Line3D nextLine;
        Line2D next2DLine;
        for (int i = 0; i < nLine; i++)
            {
                inCoords.GetLine(i, nextLine);
                if (nextLine.stPt.CoordIsNull() || nextLine.endPt.CoordIsNull())
                    continue;
                next2DLine.stPt = nextLine.stPt;
                next2DLine.endPt = nextLine.endPt;
                (*this) += next2DLine;
            }
    }

    bool SC_Line2DArray::SegmentIsPresent(const Line2D& inSeg,
                                          const double& ptEps) const
    {
        const Point2D& segStart = inSeg.stPt;
        const Point2D& segEnd = inSeg.endPt;
        for (int i = 0; i < Size(); i++)
            {
                const Point2D& lineStart = (*this)[i].stPt;
                const Point2D& lineEnd = (*this)[i].endPt;
                if ((segStart.SamePoint(lineStart, ptEps) && segEnd.SamePoint(lineEnd, ptEps)) ||
                    (segStart.SamePoint(lineEnd, ptEps) && segEnd.SamePoint(lineStart,ptEps)))
                    return true;
            }
        return false;
    }


    void SC_Line2DMatrix::MatrixAlloc(int nLines, int nPtsPerLine, bool setResizable)
    {
        Alloc(nLines);
        for (int i =0; i < nLines; i++)
            {
                if (setResizable)
                    (*this)[i].SetResizable();
                (*this)[i].Alloc(nPtsPerLine);
            }
    }


    void  SC_Line3DArray::ConvertCoords(const SC_CoordArray& inCoords,
                                        bool           isPolygon)
    {
        //  max size
        int nLine = inCoords.Size() - 1;
        if (isPolygon)
            nLine++;
        Alloc(nLine);
        Line3D nextLine;
        for (int i = 0; i < nLine; i++)
            {
                inCoords.GetLine(i, nextLine);
                if (nextLine.stPt.CoordIsNull() || nextLine.endPt.CoordIsNull())
                    continue;
                (*this) += nextLine;
            }
    }

    void SC_Line3DArray::Convert2DLines(const SC_Line2DArray&  in2D,
                                        const double&          lineZ)
    {
        Alloc(in2D.Size());
        for (int i = 0; i < in2D.Size(); i++)
            {
                Line3D nextLine(in2D[i]);
                nextLine.stPt.cZ = lineZ;
                nextLine.endPt.cZ = lineZ;
                (*this) += nextLine;
            }
    }

    void SC_Line3DArray::Convert2DLines(const SC_Line2DArray&  in2D)
    {
        Convert2DLines(in2D, 0.0);
    }


};                                      // end namespace

