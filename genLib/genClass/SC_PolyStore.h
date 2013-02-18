///////////////////////////////////////////////////////////////////////////////////
//
// SC_PolyStore.h
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

#ifndef SC_POLYSTORE_H
#define SC_POLYSTORE_H

#include <genClass/SC_PointArray.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_CoordArray.h>
#include <genClass/SC_IntArray.h>


class SC_PolyStore {
public:
    SC_PointArray   pointData;        //  all polygon points in one list
    SC_DoubleArray  pointZ;           //  Z values - bit of a kluge

    SC_IntArray     polyPointIndx;    //  indexes of points in pointData
    SC_IntArray     polyStart;        //  start index of polygon data in polyPointIndx
    SC_IntArray     polyN;            //  # of polygon points for poly

    int             maxPolyPoint;       //  max # of points in any poly

public:
    SC_PolyStore();
    SC_PolyStore(const SC_PolyStore& a);
    ~SC_PolyStore();

    SC_PolyStore&   operator= (const SC_PolyStore& a);  // copy operator

    int             Size() const {return polyN.Size();}
    bool            IsEmpty() const {return polyN.Size() == 0;}

    //  full alloc
    bool            Alloc(int np, int npP, int npI);
    // np - polyStart, polyN
    // npP - polyPointData,
    // npI - polyPointIndx

    // resizable alloc
    bool            Alloc(int np);

    void            SetMaxPolyPoint();
    void            SetPolyClockwise(int polyIndx);
    void            PolyCleanup(const double& tolerance); // removes excess interior points reduces to required size



    //  adds poly to structure
    void            AddPolyPoints(int                    polyIndx,  // Z = 0.0
                                  const SC_PointArray&   polyData);

    void            AddPolyPoints(int                    polyIndx,  // Z = const
                                  const SC_PointArray&   polyData,
                                  const double&          polyZ);

    void            AddPolyPoints(int                   polyIndx,
                                  const SC_CoordArray&  polyData);


    // extracts polgon
    void            GetPolyPoints(int              polyIndx,
                                  SC_PointArray&   polyData) const;
    void            GetPolyPoints(int              polyIndx,
                                  SC_CoordArray&   polyData) const;

    // various poly ops
    bool            PointInPoly(const double& xPt,
                                const double& yPt,
                                int           polyNum) const;
    bool            PointInPoly(const Point2D& inPt,
                                int            polyNum) const;
    //  returns true if X,Y is within polygon # polyNum
    //  false if polyNum out of range or X,Y not within

    void            DeAlloc();
    // releases all memory

    //  reduction extraction support
    void            ApplyReverseMap(const SC_IntArray& reverseMap);


private:
    // quick implementations to match public signature
    void            AddPolyPointsQuick(int                    polyIndx,
                                       const SC_PointArray&   polyData,
                                       const double&          polyZ);
    void            AddPolyPointsQuick(int                    polyIndx,
                                       const SC_CoordArray&   polyData);


};

#endif // SC_POLYSTORE_H

