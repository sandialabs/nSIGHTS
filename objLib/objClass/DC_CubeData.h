///////////////////////////////////////////////////////////////////////////////////
//
// DC_CubeData.h
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
//      Data defined over regular 3D grid.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_CUBEDATA_H
#define DC_CUBEDATA_H

#include <genClass/SC_PointArray.h>
#include <genClass/SC_CoordArray.h>

#include <objClass/DC_DataCoord.h>


class DC_GridData;

class DC_CubeData {
    public:
        enum                ExtractAxes {eaX = 0, eaY = 1, eaZ = 2};


        DC_DataCoord        xData;
        DC_DataCoord        yData;
        DC_DataCoord        zData;
        SC_DoubleCube       cData;      //  may contain nulls
                                        // NOTE: indexing is cData[xIndx][yIndx][zIndx];

        enum                {cubeIDLen = DC_DataCoord::dataIDLen};
        char                cubeID[cubeIDLen];

    public:
                    DC_CubeData();
                    DC_CubeData(const DC_CubeData& a);
                    ~DC_CubeData();

        DC_CubeData&  operator=(const DC_CubeData& a);

        bool        Alloc(int nX, int nY, int nZ);  // allocs and fills to size with nulls, X,Y set to index
        void        DeAlloc();

        bool        CreateFrom(DC_CubeData& a);  // assigns with alloc checks
        bool        CreateFrom(const DC_DataCoord& x,
                               const DC_DataCoord& y,
                               const DC_DataCoord& z);  // assigns with alloc checks


        bool        IsEmpty()                               const {return xData.IsEmpty() || yData.IsEmpty() || zData.IsEmpty();}
        bool        IsCompatible(const DC_CubeData& a)      const;

        void        SetID(const char id[]);
        void        GetID(char id[], int  maxIDlen) const;
        const char* GetID()                         const {return cubeID;}

        void        SetXID(const char id[]);
        void        GetXID(char id[], int  maxIDlen) const;
        const char* GetXID()                        const {return xData.GetID();}

        void        SetYID(const char id[]);
        void        GetYID(char id[], int  maxIDlen) const;
        const char* GetYID()                        const {return yData.GetID();}

        void        SetZID(const char id[]);
        void        GetZID(char id[], int  maxIDlen) const;
        const char* GetZID()                        const {return zData.GetID();}

        //  single subscript access
        int         Size()                          const {return xData.Size() * yData.Size() * zData.Size();}
        void        GetXYZIndex(int indx,
                                int& xIndx,
                                int& yIndx,
                                int& zIndx)          const;

        double&     operator[](int indx);
        double&     operator[](int indx)            const;
        void        GetData(SC_DoubleArray& cOut)   const;
        void        SetData(const SC_DoubleArray& cIn);
        Coord3D     GetCoord(int indx)              const;

        bool        TransformCoord(Coord3D& inPt)   const; // applies log transform if defined
        void        UnTransformCoord(Coord3D& inPt) const; // reverses log transform if defined

        // basic block extraction
        void        GetBlockCoords(int              indx,
                                   SC_CoordArray&   topCoords,
                                   SC_CoordArray&   botCoords) const;

        bool        GetIndex(const Coord3D& inCoord,                // returns true if in cube
                                   int&     indx) const;

        //  other data extraction
        bool        ExtractGrid(const double&       otherVal,
                                      ExtractAxes   gridX,
                                      ExtractAxes   gridY,
                                     bool       logInterp,
                                     DC_GridData&   gridOutput) const;


    private:
        void        InitCommon();
        void        LocalCopy(const DC_CubeData& a);


            int         GetSingleIndex(int xIndx,
                                       int yIndx,
                                       int zIndx) const;

            double      GetXMid(int xIndx) const;
            double      GetYMid(int yIndx) const;
            double      GetZMid(int zIndx) const;

            const DC_DataCoord* GetAxes(ExtractAxes ax) const;

    };

#endif //DC_CUBEDATA_H

