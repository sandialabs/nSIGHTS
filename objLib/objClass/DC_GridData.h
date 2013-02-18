///////////////////////////////////////////////////////////////////////////////////
//
// DC_GridData.h
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
//      Data defined over a regular 2D grid.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_GRIDDATA_H
#define DC_GRIDDATA_H

#include <genClass/SC_PointArray.h>
#include <genClass/SC_CoordArray.h>

#include <objClass/DC_DataCoord.h>

class SC_Triangulation;
class DC_XYData;

class DC_GridData {
    public:
        DC_DataCoord        xData;
        DC_DataCoord        yData;
        SC_DoubleMatrix     gData;      //  may contain nulls
                                        // NOTE: indexing is zData[xIndx][yIndx];

        enum                {gridIDLen = DC_DataCoord::dataIDLen};
        char                gridID[gridIDLen];

    private:
        mutable SC_PointArray*      triPoints;          // support for triangulation
        mutable SC_Triangulation*   triangulation;      // not created until GetTriangulation() called

    public:
                    DC_GridData();
                    DC_GridData(const DC_GridData& a);
                    ~DC_GridData();

        DC_GridData&  operator=(const DC_GridData& a);

        bool        Alloc(int nX, int nY);  // allocs and fills to size with nulls, X,Y set to index
        void        DeAlloc();

        bool        CreateFrom(const DC_GridData& a);  // assigns with alloc checks
        bool        CreateFrom(const DC_DataCoord& x,
                               const DC_DataCoord& y);  // assigns with alloc checks

        bool        IsEmpty()                               const {return xData.IsEmpty() || yData.IsEmpty();}
        bool        IsCompatible(const DC_GridData& a)      const;

        void        SetID(const char id[]);
        void        GetID(char id[], int  maxIDlen) const;
        const char* GetID()                         const {return gridID;}

        void        SetXID(const char id[]);
        void        GetXID(char id[], int  maxIDlen) const;
        const char* GetXID()                        const {return xData.GetID();}

        void        SetYID(const char id[]);
        void        GetYID(char id[], int  maxIDlen) const;
        const char* GetYID()                        const {return yData.GetID();}

        //  single subscript access
        int         Size()                          const {return xData.Size() * yData.Size();}
        void        GetXYIndex(int indx,
                               int& xIndx,
                               int& yIndx)          const;

        double&     operator[](int indx);
        double&     operator[](int indx)            const;
        void        GetData(SC_DoubleArray& gOut)   const;
        void        SetData(const SC_DoubleArray& gIn);
        Point2D     GetPoint(int indx)              const;
        Coord3D     GetCoord(int indx)              const;

        //  triangulation support
        SC_Triangulation&   GetTriangulation()  const;
        bool        TransformPoint(Point2D& inPt) const; // applies log transform if defined
        bool        TransformCoord(Coord3D& inPt) const; // applies log transform if defined
        void        UnTransformPoint(Point2D& inPt) const; // reverses log transform if defined
        void        UnTransformCoord(Coord3D& inPt) const; // reverses log transform if defined


        // basic block extraction
        void        GetBlockPoints(int              indx,
                                   SC_PointArray&   points) const;      // based on X & Y only
        void        GetBlockCoords(int              indx,
                                   SC_CoordArray&   coords) const;      // with Z

        bool        GetIndex(const Point2D& inPt,                   // returns true if in grid
                                   int&  indx) const;

        //  other data extraction
        bool        ExtractXY(const double&     xyVal,
                                    bool        valIsX,
                                    bool        logInterp,
                                    DC_XYData&  xyOutput) const;

        bool        ExtractPt(const Point2D&    ptVal,
                                    bool        logInterp,
                                    double&     zVal) const;

                    // brute force approach -- may yield different values
                    // than triangulation

        //  operations
        void        Decimate(int decModulus);

        void        FixDropouts(int maxFix, bool logInterp);


    private:
        void        InitCommon();
        void        LocalCopy(const DC_GridData& a);

        void        CreateTriangulation() const;
        void        ClearTriangulation();

        int         GetSingleIndex(int xIndx,
                                   int yIndx) const;

        void        SetCorners(int indx,
                               double& xs,
                               double& xe,
                               double& ys,
                               double& ye) const;

        double      GetXMid(int xIndx) const;
        double      GetYMid(int yIndx) const;

};

#endif //DC_GRIDDATA_H

