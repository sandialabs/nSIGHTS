///////////////////////////////////////////////////////////////////////////////////
//
// DC_GridData.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>
#include <genClass/SC_Triangulation.h>
#include <genClass/DC_XYData.h>

#include <objClass/DC_GridData.h>

DC_GridData :: DC_GridData()
{
    SetID("Grid");
    SetXID("X");
    SetYID("Y");
    InitCommon();
}

DC_GridData::DC_GridData(const DC_GridData& a) :
    xData(a.xData), yData(a.yData), gData(a.gData)
{
    LocalCopy(a);
    InitCommon();
}

void DC_GridData::InitCommon()
{
    triPoints = 0;
    triangulation = 0;
}

bool DC_GridData::IsCompatible(const DC_GridData& a) const
{
    return (xData.IsCompatible(a.xData) && yData.IsCompatible(a.yData));
}

DC_GridData :: ~DC_GridData()
{
    ClearTriangulation();
}

DC_GridData&  DC_GridData::operator=(const DC_GridData& a)
{
    if (&a != this)
    {
        xData = a.xData;
        yData = a.yData;
        gData = a.gData;
        LocalCopy(a);
        ClearTriangulation();
    }
    return *this;
}

void DC_GridData::LocalCopy(const DC_GridData& a)
{
    SetID(a.gridID);
}

void DC_GridData :: DeAlloc()
{
    xData.DeAlloc();
    yData.DeAlloc();
    gData.DeAlloc();
    ClearTriangulation();
}


bool DC_GridData::Alloc(int nX, int nY)
{
    try
    {
        xData.Alloc(nX);
        yData.Alloc(nY);
        gData.MatrixAlloc(nX, nY);
    }
    catch (SC_Array::AllocationError& )
    {
        DeAlloc();
        return false;
    }

    gData.SetNull();
    ClearTriangulation();
    return true;
}


bool DC_GridData::CreateFrom(const DC_GridData& a)
{
    try
    {
        xData = a.xData;
        yData = a.yData;
        gData = a.gData;
    }
    catch (SC_Array::AllocationError& )
    {
        DeAlloc();
        return false;
    }

    LocalCopy(a);
    ClearTriangulation();

    return true;
}


bool DC_GridData::CreateFrom(const DC_DataCoord& x,
                             const DC_DataCoord& y)
{
    try
    {
        xData = x;
        yData = y;
        gData.MatrixAllocAndSetSize(xData.Size(), yData.Size());
    }
    catch (SC_Array::AllocationError& )
    {
        DeAlloc();
        return false;
    }
    gData.SetNull();
    ClearTriangulation();
    SetID("Created");
    return true;
}


void DC_GridData::SetID(const char id[])
{
    CopyString(gridID, id, gridIDLen);
}

void DC_GridData::GetID(char id[],
                      int  maxIDlen) const
{
    CopyString(id, gridID, maxIDlen);
}

void DC_GridData::SetXID(const char id[])
{
    xData.SetID(id);
}

void DC_GridData::GetXID(char id[],
                      int  maxIDlen) const
{
    xData.GetID(id, maxIDlen);
}
void DC_GridData::SetYID(const char id[])
{
    yData.SetID(id);
}

void DC_GridData::GetYID(char id[],
                      int  maxIDlen) const
{
    yData.GetID(id, maxIDlen);
}

void DC_GridData::GetXYIndex(int indx,
                             int& xIndx,
                             int& yIndx) const
{
    xIndx = indx / yData.Size();
    yIndx = indx % yData.Size();
}

int DC_GridData::GetSingleIndex(int xIndx,
                               int yIndx) const
{
    return xIndx * yData.Size() + yIndx;
}

double& DC_GridData::operator[] (int indx)
{
    int xIndx, yIndx;
    GetXYIndex(indx, xIndx, yIndx);
    return gData[xIndx][yIndx];
}

double& DC_GridData::operator[](int indx) const
{
    int xIndx, yIndx;
    GetXYIndex(indx, xIndx, yIndx);
    return gData[xIndx][yIndx];
}


void DC_GridData::GetData(SC_DoubleArray& gOut) const
{
    gOut.AllocAndSetSize(Size());
    int next = 0;
    for (int i = 0; i < xData.Size(); i++)
        for (int j = 0; j < yData.Size(); j++)
            gOut[next++] = gData[i][j];
}

void DC_GridData::SetData(const SC_DoubleArray& gIn)
{
    if (gIn.Size() != Size())
        GenAppInternalError("DC_GridData::SetData");
    int next = 0;
    for (int i = 0; i < xData.Size(); i++)
        for (int j = 0; j < yData.Size(); j++)
            gData[i][j] =  gIn[next++];
}

Point2D DC_GridData::GetPoint(int indx) const
{
    int xIndx, yIndx;
    GetXYIndex(indx, xIndx, yIndx);
    return Point2D(xData[xIndx], yData[yIndx]);
}

Coord3D DC_GridData::GetCoord(int indx) const
{
    int xIndx, yIndx;
    GetXYIndex(indx, xIndx, yIndx);
    return Coord3D(xData[xIndx], yData[yIndx], gData[xIndx][yIndx]);
}



bool DC_GridData::GetIndex(const Point2D& inPt,                 // returns true if in grid
                                 int&  indx) const
{
    int xi, yi;
    if (xData.GetIndex(inPt.pX, xi) && yData.GetIndex(inPt.pY, yi))
    {
        indx = GetSingleIndex(xi, yi);
        return true;
    }
    return false;
}


SC_Triangulation& DC_GridData::GetTriangulation() const
{
    if (triangulation != 0)
        return *triangulation;

    CreateTriangulation();

    return *triangulation;
}



void DC_GridData::CreateTriangulation() const
{
    triPoints = new SC_PointArray(Size());
    triPoints->SetEmpty();
    Point2D nextPoint;
    for (int i = 0; i < xData.Size(); i++)
    {
        nextPoint.pX = xData[i];
        xData.TransformVal(nextPoint.pX);

        for (int j = 0; j < yData.Size(); j++)
        {
            nextPoint.pY = yData[j];
            yData.TransformVal(nextPoint.pY);
            (*triPoints) += nextPoint;
        }
    }

    triangulation = new SC_Triangulation(*triPoints);

    triangulation->BuildFromRegularGrid(xData.Size(), yData.Size());

    triangulation->BuildEdgeAssociation();
}

void DC_GridData::ClearTriangulation()
{
    if (triPoints != 0)
    {
        delete triangulation;
        delete triPoints;
        triangulation = 0;
        triPoints = 0;
    }
}

bool DC_GridData::TransformPoint(Point2D& inPt) const
{
    return xData.TransformVal(inPt.pX) && yData.TransformVal(inPt.pY);
    return true;
}

bool DC_GridData::TransformCoord(Coord3D& inPt) const
{
    return xData.TransformVal(inPt.cX) && yData.TransformVal(inPt.cY);
}

void DC_GridData::UnTransformPoint(Point2D& inPt) const
{
    xData.UnTransformVal(inPt.pX);
    yData.UnTransformVal(inPt.pY);
}

void DC_GridData::UnTransformCoord(Coord3D& inPt) const
{
    xData.UnTransformVal(inPt.cX);
    yData.UnTransformVal(inPt.cY);
}


// basic block extraction
double DC_GridData::GetXMid(int xIndx) const
{
    return xData.GetMid(xIndx);
}


double DC_GridData::GetYMid(int yIndx) const
{
    return yData.GetMid(yIndx);
}

void DC_GridData::SetCorners(int indx,
                             double& xs,
                             double& xe,
                             double& ys,
                             double& ye) const
{
    int xIndx, yIndx;
    GetXYIndex(indx, xIndx, yIndx);
    xData.GetStartEnd(xIndx, xs, xe);
    yData.GetStartEnd(yIndx, ys, ye);
}

void DC_GridData::GetBlockPoints(int                indx,
                                 SC_PointArray&     points) const
{
    double xs, xe, ys, ye;
    SetCorners(indx, xs, xe, ys, ye);

    points.Alloc(4);
    points += Point2D(xs, ys);
    points += Point2D(xs, ye);
    points += Point2D(xe, ye);
    points += Point2D(xe, ys);
}

void DC_GridData::GetBlockCoords(int                indx,
                                 SC_CoordArray&     coords) const

{
    double xs, xe, ys, ye;
    SetCorners(indx, xs, xe, ys, ye);

    double z = (*this)[indx];

    coords.Alloc(4);
    coords += Coord3D(xs, ys, z);
    coords += Coord3D(xs, ye, z);
    coords += Coord3D(xe, ye, z);
    coords += Coord3D(xe, ys, z);
}


bool DC_GridData::ExtractPt(const Point2D&  ptVal,
                                  bool      logInterp,
                                  double&   zVal) const

{
    int stX, stY;
    double xT, yT;
    if ((!xData.GetInterpT(ptVal.pX, stX, xT)) ||(!yData.GetInterpT(ptVal.pY, stY, yT)))
        return false;

    double z11, z12, z21, z22;
    z11 = gData[stX][stY];
    z12 = gData[stX][stY + 1];
    z21 = gData[stX + 1][stY];
    z22 = gData[stX + 1][stY + 1];

    if (RealIsNull(z11) || RealIsNull(z12) || RealIsNull(z21) || RealIsNull(z22))
        return false;

    if (logInterp)
    {
        if (!(IsPositive(z11) && IsPositive(z12) && IsPositive(z21) && IsPositive(z22)))
            return false;

        z11 = log10(z11);
        z12 = log10(z12);
        z21 = log10(z21);
        z22 = log10(z22);
    }


    double z1 = z11 + xT * (z21 - z11);
    double z2 = z12 + xT * (z22 - z12);

    zVal = z1 + yT * (z2 - z1);

    if (logInterp)
        zVal = InvLgt(zVal);

    return true;
}



bool DC_GridData::ExtractXY(const double&    xyVal,
                                  bool       valIsX,
                                  bool       logInterp,
                                  DC_XYData& xyOutput) const
{
    int stIndx;
    double tVal;
    bool isOK;
    const SC_DoubleArray*  outX;
    if (valIsX)
    {
        isOK = xData.GetInterpT(xyVal, stIndx, tVal);
        outX = &yData;
        xyOutput.SetID(xData.GetID());
    }
    else
    {
        isOK = yData.GetInterpT(xyVal, stIndx, tVal);
        outX = &xData;
        xyOutput.SetID(yData.GetID());
    }

    if ((!isOK) || (!xyOutput.CreateFrom(*outX, *outX)))
        return false;


    double stVal, eVal;
    for (int i = 0; i < xyOutput.Size(); i++)
    {
        xyOutput.yData[i] = nullReal;

        if (valIsX)
        {
            stVal = gData[stIndx][i];
            eVal = gData[stIndx + 1][i];
        }
        else
        {
            stVal = gData[i][stIndx];
            eVal = gData[i][stIndx + 1];
        }

        if (logInterp)
        {
            if ((stVal < stdEps) || (eVal < stdEps))
                continue;
            stVal = log10(stVal);
            eVal = log10(eVal);
        }

        double resVal = stVal * (1.0 - tVal)  + eVal * tVal;
        if (logInterp)
            resVal = InvLgt(resVal);

        xyOutput.yData[i] = resVal;
    }

    xyOutput.CleanUp();

    return xyOutput.Size() > 0;
}


void DC_GridData::Decimate(int decModulus)
{
    if (decModulus < 2)
        return;

    // make copy
    DC_GridData temp = (*this);

    // this will change
    ClearTriangulation();

    // decimate the coordinates
    xData.Decimate(decModulus);
    yData.Decimate(decModulus);

    //  re allocate the grid
    gData.MatrixAllocAndSetSize(xData.Size(), yData.Size());

    // fill grid
    int nextCol = 0;
    for (int i = 0; i < temp.gData.Size(); i++)
    {
        bool doCol = (i == 0) || ((i % decModulus) == 0) || (i == temp.gData.UpperBound());
        if (doCol)
        {
            SC_DoubleArray& currCol = gData[nextCol++];
            SC_DoubleArray& tempCol = temp.gData[i];
            int nextRow = 0;
            for (int j = 0; j < tempCol.Size(); j++)
            {
                bool doRow = (j == 0) || ((j % decModulus) == 0) || (j == tempCol.UpperBound());
                if (doRow)
                    currCol[nextRow++] = tempCol[j];
            }
        }
    }

}


void DC_GridData::FixDropouts(int maxFix, bool logInterp)
{
    if (maxFix < 1)
        return;

    int nfix = 1;
    while (nfix > 0) {

        nfix = 0;

        // along rows
        int i;
        for (i = 0; i < xData.Size(); i++)
        {
            int startOK = -1;
            int ndropout = 0;

            for (int j = 0; j < yData.Size(); j++)
            {
                double currVal = gData[i][j];
                if (RealIsNull(currVal))
                {
                    ndropout++;
                }
                else
                {
                    if ((ndropout > 0) && (startOK >= 0) && (ndropout <= maxFix))
                    {
                        // fix
                        double startVal = gData[i][startOK];
                        if ((!logInterp) || (IsPositive(currVal) && IsPositive(startVal)))
                        {
                            if (logInterp)
                            {
                                currVal = log10(currVal);
                                startVal = log10(startVal);
                            }

                            double startT = yData[startOK];
                            double endT = yData[j];
                            if (yData.dataIsLog)
                            {
                                startT = log10(startT);
                                endT = log10(endT);
                            }

                            double mVal = (currVal - startVal) / (endT - startT);
                            for (int k = startOK + 1; k < j; k++)
                            {
                                double currT = yData[k];
                                if (yData.dataIsLog)
                                    currT = log10(currT);
                                double dropVal = startVal + (currT - startT) * mVal;
                                if (logInterp)
                                    dropVal = InvLgt(dropVal);
                                gData[i][k] = dropVal;
                                nfix++;
                            }
                        }
                    }
                    startOK = j;
                    ndropout = 0;
                }
            }
        }

        // now columns

        for (i = 0; i < yData.Size(); i++)
        {
            int startOK = -1;
            int ndropout = 0;

            for (int j = 0; j < xData.Size(); j++)
            {
                double currVal = gData[j][i];
                if (RealIsNull(currVal))
                {
                    ndropout++;
                }
                else
                {
                    if ((ndropout > 0) && (startOK >= 0) && (ndropout <= maxFix))
                    {
                        // fix
                        double startVal = gData[startOK][i];
                        if ((!logInterp) || (IsPositive(currVal) && IsPositive(startVal)))
                        {
                            if (logInterp)
                            {
                                currVal = log10(currVal);
                                startVal = log10(startVal);
                            }

                            double startT = xData[startOK];
                            double endT = xData[j];
                            if (xData.dataIsLog)
                            {
                                startT = log10(startT);
                                endT = log10(endT);
                            }

                            double mVal = (currVal - startVal) / (endT - startT);
                            for (int k = startOK + 1; k < j; k++)
                            {
                                double currT = xData[k];
                                if (xData.dataIsLog)
                                    currT = log10(currT);
                                double dropVal = startVal + (currT - startT) * mVal;
                                if (logInterp)
                                    dropVal = InvLgt(dropVal);
                                gData[k][i] = dropVal;
                                nfix++;
                            }
                        }
                    }
                    startOK = j;
                    ndropout = 0;
                }
            }
        }
    }

}

