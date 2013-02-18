///////////////////////////////////////////////////////////////////////////////////
//
// SC_PolyStore.cpp
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

#include <math.h>

#include <genClass/SC_PointArray.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/SC_IntArray.h>

#include <genClass/SC_PolyStore.h>

SC_PolyStore::SC_PolyStore()
{
    maxPolyPoint    = 0;
}

SC_PolyStore::SC_PolyStore(const SC_PolyStore& a)
{
    pointData     = a.pointData;
    pointZ        = a.pointZ;
    polyPointIndx = a.polyPointIndx;
    polyStart     = a.polyStart;
    polyN         = a.polyN;
    maxPolyPoint  = a.maxPolyPoint;
}

SC_PolyStore::~SC_PolyStore()
{
    DeAlloc();
}

SC_PolyStore& SC_PolyStore::operator= (const SC_PolyStore& a)
{
    if (this != &a)
        {
            DeAlloc();

            pointData     = a.pointData;
            pointZ        = a.pointZ;
            polyPointIndx = a.polyPointIndx;
            polyStart     = a.polyStart;
            polyN         = a.polyN;
            maxPolyPoint  = a.maxPolyPoint;
        }
    return *this;
}

bool SC_PolyStore::Alloc(int np, int npP, int npI)
{
    DeAlloc();
    pointData.SetFixed();
    pointZ.SetFixed();
    polyPointIndx.SetFixed();

    try
        {
            polyStart.Alloc(np);

            polyN.AllocAndFill(np, 0);
            polyN.SetEmpty();

            pointData.Alloc(npP);
            pointZ.Alloc(npP);
            polyPointIndx.Alloc(npI);
        }
    catch (SC_Array::AllocationError)
        {
            DeAlloc();
            return false;
        }

    return true;
}

bool SC_PolyStore::Alloc(int np)
{
    DeAlloc();
    pointData.SetResizable();
    pointZ.SetResizable();
    polyPointIndx.SetResizable();
    try
        {
            polyStart.Alloc(np);

            polyN.AllocAndFill(np, 0);
            polyN.SetEmpty();

            pointData.Alloc(np * 3);
            pointZ.Alloc(np * 3);
            polyPointIndx.Alloc(np * 3);
        }
    catch (SC_Array::AllocationError)
        {
            DeAlloc();
            return false;
        }

    return true;
}


void SC_PolyStore::DeAlloc()
{
    polyStart.DeAlloc();
    polyN.DeAlloc();
    polyPointIndx.DeAlloc();
    pointData.DeAlloc();
    pointZ.DeAlloc();
    maxPolyPoint    = 0;
}

void SC_PolyStore::SetMaxPolyPoint()
{
    maxPolyPoint    = 0;
    for ( int i = 0; i < polyN.Size(); i++)
        maxPolyPoint = (polyN[i] > maxPolyPoint) ? polyN[i] : maxPolyPoint;
}

void SC_PolyStore::ApplyReverseMap(const SC_IntArray& reverseMap)
{
    polyStart.ApplyReverseMap(reverseMap);
    polyN.ApplyReverseMap(reverseMap);
}

//  quick implementations for now - may replace with storage efficient (i.e. sharing points) later


void SC_PolyStore::AddPolyPointsQuick(int                    polyIndx,
                                      const SC_PointArray&   polyData,
                                      const double&          polyZ)
{
    if (polyData.Size() > maxPolyPoint)
        maxPolyPoint    = polyData.Size();

    polyN[polyIndx] = polyData.Size();
    polyStart[polyIndx] = polyPointIndx.Size();

    for ( int i = 0; i < polyData.Size(); i++)
        {
            polyPointIndx += pointData.Size();
            pointData += polyData[i];
            pointZ += polyZ;
        }

    if (polyIndx >= polyN.Size())
        {
            polyN.SetSize(polyIndx + 1);
            polyStart.SetSize(polyIndx + 1);
        }
}

void SC_PolyStore::AddPolyPointsQuick(int                    polyIndx,
                                      const SC_CoordArray&   polyData)
{
    if (polyData.Size() > maxPolyPoint)
        maxPolyPoint    = polyData.Size();
    polyN[polyIndx] = polyData.Size();
    polyStart[polyIndx] = polyPointIndx.Size();

    Point2D tmpPt;                      // gnu C++ has problems with automatic conversion of Coord3D to Point2D
                                        // need temp var to make it explicit.
    for ( int i = 0; i < polyData.Size(); i++)
        {
            polyPointIndx += pointData.Size();
            tmpPt.pX = polyData[i].cX;      // gnu C++ bug fix
            tmpPt.pY = polyData[i].cY;      // gnu C++ bug fix

            pointData += tmpPt;             //   was polyData[i] // gnu bug fix
            pointZ += polyData[i].cZ;
        }

    if (polyIndx >= polyN.Size())
        {
            polyN.SetSize(polyIndx + 1);
            polyStart.SetSize(polyIndx + 1);
        }

}

void  SC_PolyStore::AddPolyPoints(int                    polyIndx,
                                  const SC_PointArray&   polyData)
{
    AddPolyPointsQuick(polyIndx, polyData, 0.0);
}
void  SC_PolyStore::AddPolyPoints(int                    polyIndx,
                                  const SC_PointArray&   polyData,
                                  const double&          polyZ)
{
    AddPolyPointsQuick(polyIndx, polyData, polyZ);
}
void  SC_PolyStore::AddPolyPoints(int                    polyIndx,
                                  const SC_CoordArray&   polyData)
{
    AddPolyPointsQuick(polyIndx, polyData);
}



void SC_PolyStore::SetPolyClockwise(int polyIndx)
{
    if (polyN[polyIndx] < 3)
        return;

    SC_PointArray tempPoly;
    GetPolyPoints(polyIndx, tempPoly);

    if (tempPoly.CalcPolyArea() < 0.0)
        {
            int npoly = polyN[polyIndx];
            SC_IntArray tempIndex;
            tempIndex.Alloc(npoly);

            int polyEnd = polyStart[polyIndx] + npoly - 1;
            int i;
            for (i = 0; i < npoly; i++)
                tempIndex[i] = polyPointIndx[polyEnd --];
            for (i = 0; i < npoly; i++)
                polyPointIndx[polyStart[polyIndx] + i] = tempIndex[i];
        }
}


void SC_PolyStore::GetPolyPoints(int           polyIndx,
                                 SC_PointArray& polyData) const
{
    int nPts = polyN[polyIndx];
    if (nPts < 1)
        {
            polyData.DeAlloc();
            return;
        }

    polyData.Alloc(nPts);

    int ptIndx = polyStart[polyIndx];
    for ( int i = 0; i < nPts; i++)
        polyData += pointData[polyPointIndx[ptIndx++]];

}

void SC_PolyStore::GetPolyPoints(int           polyIndx,
                                 SC_CoordArray& polyData) const
{
    int nPts = polyN[polyIndx];
    polyData.Alloc(nPts);

    int ptIndx = polyStart[polyIndx];
    Coord3D tmpCoord;
    for ( int i = 0; i < nPts; i++)
        {
            int nextIndx = polyPointIndx[ptIndx++];
            tmpCoord = pointData[nextIndx];
            tmpCoord.cZ = pointZ[nextIndx];
            polyData += tmpCoord;
        }
}


bool SC_PolyStore::PointInPoly(const double& xPt,
                               const double& yPt,
                               int           polyNum) const
{
    SC_PointArray polyData;
    GetPolyPoints(polyNum, polyData);
    return polyData.PointInPoly(xPt, yPt);
}

bool SC_PolyStore::PointInPoly(const Point2D& inPt,
                               int            polyNum) const
{
    SC_PointArray polyData;
    GetPolyPoints(polyNum, polyData);
    return polyData.PointInPoly(inPt);
}


void SC_PolyStore::PolyCleanup(const double& tolerance)
{
    SetMaxPolyPoint();

    SC_BoolArray keepPoints(pointData.Size(), false);
    SC_PointArray tempPoints(maxPolyPoint);
    SC_IntArray   tempIndexes(maxPolyPoint);

    int i;
    for (i = 0; i < polyN.Size(); i++)
        {
            int nCurr = polyN[i];

            tempPoints.SetSize(nCurr);
            tempIndexes.SetSize(nCurr);
            int ptIndx = polyStart[i];
            int j;
            for (j = 0; j < nCurr; j++)
                {
                    int nextIndx = polyPointIndx[ptIndx++];
                    tempPoints[j] = pointData[nextIndx];
                    tempIndexes[j] = nextIndx;
                }

            tempPoints.RemoveInteriorPoints(tempIndexes, tolerance);

            for (j = 0; j < tempIndexes.Size(); j++)
                keepPoints[tempIndexes[j]] = true;


            if (tempIndexes.Size() == nCurr)
                continue;

            nCurr = tempIndexes.Size();
            polyN[i] = nCurr;
            ptIndx = polyStart[i];
            for (j = 0; j < nCurr; j++)
                polyPointIndx[ptIndx++] = tempIndexes[j];

            SetPolyClockwise(i);
        }

    if (keepPoints.GetSetCount() == pointData.Size())
        return;

    SC_IntArray reverseMap, forwardMap;
    keepPoints.MakeReverseMap(reverseMap);
    keepPoints.MakeForwardMap(forwardMap);

    pointData.ApplyForwardMap(forwardMap);
    pointZ.ApplyForwardMap(forwardMap);

    for (i = 0; i < polyPointIndx.Size(); i++)
        polyPointIndx[i] = reverseMap[polyPointIndx[i]];

}

