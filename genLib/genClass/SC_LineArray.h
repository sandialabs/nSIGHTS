///////////////////////////////////////////////////////////////////////////////////
//
// SC_LineArray.h
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

#ifndef SC_LINEARRAY_H
#define SC_LINEARRAY_H

#include <genClass/C_Common.h>
#include <genClass/T_SC_Array.h>

namespace genCommonTypes {

    class SC_PointArray;
    class SC_CoordArray;

    class SC_Line2DArray: public T_SC_Array<Line2D> {
    public :

        //  converts points to 2D lines
        //      each segment in inPoints forms a new line
        //      nullPoints are skipped and segs restarted at next valid
        //
        void        ConvertPoints(const SC_PointArray&  inPoints,
                                  bool            isPolygon);  // if is true segment from end to 0 is added
        void        ConvertCoords(const SC_CoordArray&  inCoords,
                                  bool            isPolygon);  // if is true segment from end to 0 is added

        bool        SegmentIsPresent(const Line2D& inSeg,
                                     const double& ptEps) const;  // returns true if segment is already in array

    };

    class SC_Line2DMatrix: public T_SC_Array<SC_Line2DArray> {
    public :

        SC_Line2DMatrix() : T_SC_Array<SC_Line2DArray>() {};               //default cons
        SC_Line2DMatrix(int nAll) : T_SC_Array<SC_Line2DArray>(nAll) {};   //allocates to size nAll

        //  type specific methods
        void            MatrixAlloc(int nLines, int nPtsPerLine, bool setResizable);

    };



    class SC_Line3DArray: public T_SC_Array<Line3D> {
    public :

        //  converts coord lists to 3D lines
        //      each segment in inPoints forms a new line
        //      nullPoints are skipped and segs restarted at next valid
        //
        void        ConvertCoords(const SC_CoordArray&  inCoords,
                                  bool            isPolygon);  // if is true segment from end to 0 is added

        void        Convert2DLines(const SC_Line2DArray&  in2D);  // Z set to 0.0
        void        Convert2DLines(const SC_Line2DArray&  in2D,
                                   const double&          lineZ); // Z set to value

    };

};

#endif
