///////////////////////////////////////////////////////////////////////////////////
//
// DC_ExtendedProfile.cpp
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

#include <genClass/SC_Triangulation.h>
#include <genClass/U_Real.h>

#include <nsClass/DC_ExtendedProfile.h>

DC_ExtendedProfile::DC_ExtendedProfile()
{
    triPoints = 0;
    triangulation = 0;
}

DC_ExtendedProfile::~DC_ExtendedProfile()
{
    ClearTriangulation();
}

DC_ExtendedProfile::DC_ExtendedProfile(const DC_ExtendedProfile& a)
: ExtendedProfileTimeData(a), ExtendedProfileGeomData(a)
{
    triPoints = 0;
    triangulation = 0;
    LocalCopy(a);
}


DC_ExtendedProfile&  DC_ExtendedProfile::operator=(const DC_ExtendedProfile& a)
{
    if (&a != this)
    {
        ExtendedProfileTimeData::operator=(a);
        ExtendedProfileGeomData::operator =(a);
        LocalCopy(a);
    }
    return *this;
}

void DC_ExtendedProfile::LocalCopy(const DC_ExtendedProfile& a)
{
}

SC_Triangulation& DC_ExtendedProfile::GetTriangulation() const
{
    if (triangulation != 0)
        return *triangulation;

    CreateTriangulation();

    return *triangulation;
}

void DC_ExtendedProfile::UpdateTriangulation()
{
    if ((triPoints == 0) || (triPoints->Size() != Size()))
    {
        ClearTriangulation();
        CreateTriangulation();
    }
    else
        UpdateTriPoints();
}


void DC_ExtendedProfile::UpdateTriPoints() const
{
    Point2D nextPoint;
    int nTri = 0;

    double lastConstZ = constantVertZPos.LastIndex();

    for (int i = 0; i < nradial; i++)
    {
        nextPoint.pX = nodeRadii[i];

        double dVar = 0.0;
        if (nvariableVert > 0)
            dVar = (waterTablePos[i] - lastConstZ) / double(nvariableVert);

        double currY = 0.0;
        for (int j = 0; j < ntotalVert; j++)
        {
            if (j < nconstantVert)
                currY  = constantVertZPos[j];
            else
                currY  += dVar;

            nextPoint.pY = currY;
            (*triPoints)[nTri++] = nextPoint;

        }
    }

}

void DC_ExtendedProfile::CreateTriangulation() const
{
    triPoints = new SC_PointArray(Size());

    UpdateTriPoints();

    triangulation = new SC_Triangulation(*triPoints);

    triangulation->BuildFromRegularGrid(nradial, ntotalVert);

    triangulation->BuildEdgeAssociation();
}


void DC_ExtendedProfile::ClearTriangulation() const
{
    if (triPoints != 0)
    {
        delete triangulation;
        delete triPoints;
        triangulation = 0;
        triPoints = 0;
    }
}


void DC_ExtendedProfile::GetXYIndex(int indx,
                             int& xIndx,
                             int& yIndx) const
{
    xIndx = indx / ntotalVert;
    yIndx = indx % ntotalVert;
}


double& DC_ExtendedProfile::operator[] (int indx)
{
    int xIndx, yIndx;
    GetXYIndex(indx, xIndx, yIndx);
    return nodalPressures[yIndx][xIndx];
}

double& DC_ExtendedProfile::operator[](int indx) const
{
    int xIndx, yIndx;
    GetXYIndex(indx, xIndx, yIndx);
    return nodalPressures[yIndx][xIndx];
}


void DC_ExtendedProfile::GetData(SC_DoubleArray& gOut) const
{
    gOut.AllocAndSetSize(Size());
    int next = 0;
    for (int i = 0; i < nradial; i++)
        for (int j = 0; j < ntotalVert; j++)
            gOut[next++] = nodalPressures[j][i];
}

Point2D DC_ExtendedProfile::GetPoint(int indx) const
{
    if (triPoints != 0)
        return (*triPoints)[indx];

    return Point2D(0.0, 0.0);
}

bool DC_ExtendedProfile::GetIndex(const Point2D& inPt,                  // returns true if in grid
                                 int&  indx) const
{
    if ((inPt.pX < nodeRadii[0]) || (inPt.pX > nodeRadii.LastIndex()) || (inPt.pY < 0.0))
        return false;

    // brute force
    double currDist = fabs(inPt.pX - nodeRadii[0]);
    int closestX = 0;
    for (int i = 1; i < nradial; i++)
    {
        double nextDist = fabs(inPt.pX - nodeRadii[i]);
        if (nextDist > currDist)
            break;
        currDist = nextDist;
        closestX = i;
    }

    double maxY = constantVertZPos.LastIndex();
    double dVar;
    if (nvariableVert > 0)
    {
        maxY = waterTablePos[closestX];
        dVar = (maxY - constantVertZPos.LastIndex()) / double(nvariableVert);
    }


    if (inPt.pY > maxY)
        return false;

    double nextY = dVar;
    if (nconstantVert > 1)
    {
        nextY = constantVertZPos[1];
    }

    int closestY = 0;
    currDist = inPt.pY;
    for (int i = 1; i < ntotalVert; i++)
    {
        double nextDist = fabs(inPt.pY - nextY);
        if (nextDist > currDist)
            break;

        currDist = nextDist;
        closestY = i;

        if (i < (nconstantVert - 1))
            nextY = constantVertZPos[i + 1];
        else
            nextY  += dVar;
    }


    indx = closestX * ntotalVert + closestY;

    return true;
}

