///////////////////////////////////////////////////////////////////////////////////
//
// DC_CubeData.cpp
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
//      Data defined over a regular 3D grid.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <objClass/DC_GridData.h>

#include <objClass/DC_CubeData.h>

DC_CubeData :: DC_CubeData()
{
    SetID("Cube");
    SetXID("X");
    SetYID("Y");
    SetZID("Z");
    InitCommon();
}

DC_CubeData::DC_CubeData(const DC_CubeData& a) :
    xData(a.xData), yData(a.yData), zData(a.zData), cData(a.cData)
{
    LocalCopy(a);
    InitCommon();
}

void DC_CubeData::InitCommon()
{
}

bool DC_CubeData::IsCompatible(const DC_CubeData& a) const
{
    return (xData.IsCompatible(a.xData) && yData.IsCompatible(a.yData) && zData.IsCompatible(a.zData));
}

DC_CubeData :: ~DC_CubeData()
{
}

DC_CubeData&  DC_CubeData::operator=(const DC_CubeData& a)
{
    if (&a != this)
    {
        xData = a.xData;
        yData = a.yData;
        zData = a.zData;
        cData = a.cData;
        LocalCopy(a);
    }
    return *this;
}

void DC_CubeData::LocalCopy(const DC_CubeData& a)
{
    CopyString(cubeID, a.cubeID, cubeIDLen);
}

void DC_CubeData :: DeAlloc()
{
    xData.DeAlloc();
    yData.DeAlloc();
    zData.DeAlloc();
    cData.DeAlloc();
}


bool DC_CubeData::Alloc(int nX, int nY, int nZ)
{
    try
    {
        xData.Alloc(nX);
        yData.Alloc(nY);
        zData.Alloc(nZ);
        cData.CubeAlloc(nX, nY, nZ);
    }
    catch (SC_Array::AllocationError& )
    {
        DeAlloc();
        return false;
    }

    cData.SetNull();
    return true;
}


bool DC_CubeData::CreateFrom(DC_CubeData& a)
{
    try
    {
        xData = a.xData;
        yData = a.yData;
        zData = a.zData;
        cData = a.cData;
    }
    catch (SC_Array::AllocationError& )
    {
        DeAlloc();
        return false;
    }

    LocalCopy(a);

    return true;
}

bool DC_CubeData::CreateFrom(const DC_DataCoord& x,
                             const DC_DataCoord& y,
                             const DC_DataCoord& z)
{
    try
    {
        xData = x;
        yData = y;
        zData = z;
        cData.CubeAlloc(xData.Size(), yData.Size(), zData.Size());
    }
    catch (SC_Array::AllocationError& )
    {
        DeAlloc();
        return false;
    }
    cData.SetNull();
    SetID("Created");
    return true;
}



void DC_CubeData::SetID(const char id[])
{
    CopyString(cubeID, id, cubeIDLen);
}

void DC_CubeData::GetID(char id[],
                      int  maxIDlen) const
{
    CopyString(id, cubeID, maxIDlen);
}

void DC_CubeData::SetXID(const char id[])
{
    xData.SetID(id);
}

void DC_CubeData::GetXID(char id[],
                      int  maxIDlen) const
{
    xData.GetID(id, maxIDlen);
}
void DC_CubeData::SetYID(const char id[])
{
    yData.SetID(id);
}

void DC_CubeData::GetYID(char id[],
                      int  maxIDlen) const
{
    yData.GetID(id, maxIDlen);
}

void DC_CubeData::SetZID(const char id[])
{
    zData.SetID(id);
}

void DC_CubeData::GetZID(char id[],
                      int  maxIDlen) const
{
    zData.GetID(id, maxIDlen);
}

void DC_CubeData::GetXYZIndex(int  indx,
                              int& xIndx,
                              int& yIndx,
                              int& zIndx) const
{
    int xyMod = zData.Size() * yData.Size();
    xIndx = indx / xyMod;

    indx -= xIndx * xyMod;

    yIndx = indx / zData.Size();

    zIndx = indx % zData.Size();
}

int DC_CubeData::GetSingleIndex(int  xIndx,
                                int  yIndx,
                                int  zIndx) const
{

    return xIndx * zData.Size() * yData.Size() + yIndx * zData.Size() + zIndx;
}

double& DC_CubeData::operator[] (int indx)
{
    int xIndx, yIndx, zIndx;
    GetXYZIndex(indx, xIndx, yIndx, zIndx);
    return cData[xIndx][yIndx][zIndx];
}

double& DC_CubeData::operator[](int indx) const
{
    int xIndx, yIndx, zIndx;
    GetXYZIndex(indx, xIndx, yIndx, zIndx);
    return cData[xIndx][yIndx][zIndx];
}


void DC_CubeData::GetData(SC_DoubleArray& cOut) const
{
    cOut.AllocAndSetSize(Size());
    int next = 0;
    for (int i = 0; i < xData.Size(); i++)
        for (int j = 0; j < yData.Size(); j++)
            for (int k = 0; k < zData.Size(); k++)
               cOut[next++] = cData[i][j][k];
}

void  DC_CubeData::SetData(const SC_DoubleArray& cIn)
{
    if (cIn.Size() != Size())
        GenAppInternalError("DC_CubeData::SetData");

    int next = 0;
    for (int i = 0; i < xData.Size(); i++)
        for (int j = 0; j < yData.Size(); j++)
            for (int k = 0; k < zData.Size(); k++)
               cData[i][j][k] =  cIn[next++];

}

Coord3D DC_CubeData::GetCoord(int indx) const
{
    int xIndx, yIndx, zIndx;
    GetXYZIndex(indx, xIndx, yIndx, zIndx);
    return Coord3D(xData[xIndx], yData[yIndx], zData[zIndx]);
}



bool DC_CubeData::GetIndex(const Coord3D& inPt,                 // returns true if in cube
                                 int&  indx) const
{
    int xi, yi, zi;
    if (xData.GetIndex(inPt.cX, xi) && yData.GetIndex(inPt.cY, yi) && zData.GetIndex(inPt.cZ, zi))
    {
        indx = GetSingleIndex(xi, yi, zi);
        return true;
    }
    return false;
}


bool DC_CubeData::TransformCoord(Coord3D& inPt) const
{
    return xData.TransformVal(inPt.cX) && yData.TransformVal(inPt.cY) && zData.TransformVal(inPt.cZ);
}

void DC_CubeData::UnTransformCoord(Coord3D& inPt) const
{
    xData.UnTransformVal(inPt.cX);
    yData.UnTransformVal(inPt.cY);
    zData.UnTransformVal(inPt.cZ);
}


// basic block extraction
double DC_CubeData::GetXMid(int xIndx) const
{
    return xData.GetMid(xIndx);
}

double DC_CubeData::GetYMid(int yIndx) const
{
    return yData.GetMid(yIndx);
}

double DC_CubeData::GetZMid(int zIndx) const
{
    return zData.GetMid(zIndx);
}


void DC_CubeData::GetBlockCoords(int              indx,
                                 SC_CoordArray&   topCoords,
                                 SC_CoordArray&   botCoords) const
{
    double xs, xe, ys, ye, zs, ze;
    int xIndx, yIndx, zIndx;
    GetXYZIndex(indx, xIndx, yIndx, zIndx);
    xData.GetStartEnd(xIndx, xs, xe);
    yData.GetStartEnd(yIndx, ys, ye);
    zData.GetStartEnd(zIndx, zs, ze);

    topCoords.Alloc(4);
    topCoords += Coord3D(xs, ys, ze);
    topCoords += Coord3D(xs, ye, ze);
    topCoords += Coord3D(xe, ye, ze);
    topCoords += Coord3D(xe, ys, ze);

    botCoords = topCoords;
    for (int i = 0; i < 4; i++)
        botCoords[i].cZ = zs;

}


bool DC_CubeData::ExtractGrid(const double&     otherVal,
                                    ExtractAxes gridX,
                                    ExtractAxes gridY,
                                    bool        logInterp,
                                    DC_GridData&    gridOutput) const
{
    ExtractAxes gridZ = ExtractAxes(3 - gridX - gridY);

    int stIndx;
    double tVal;
    bool isOK;
    switch (gridZ) {
        case eaX :  {
            isOK = xData.GetInterpT(otherVal, stIndx, tVal);
            gridOutput.SetID(xData.GetID());
            break;
        }
        case eaY :  {
            isOK = yData.GetInterpT(otherVal, stIndx, tVal);
            gridOutput.SetID(yData.GetID());
            break;
        }
        case eaZ :  {
            isOK = zData.GetInterpT(otherVal, stIndx, tVal);
            gridOutput.SetID(zData.GetID());
            break;
        }
    }

    const DC_DataCoord*  xC = GetAxes(gridX);
    const DC_DataCoord*  yC = GetAxes(gridY);


    if ((!isOK) || (!gridOutput.CreateFrom(*xC, *yC)))
        return false;


    double stVal, eVal;

    for (int i = 0; i < gridOutput.xData.Size(); i++)
        for (int j = 0; j < gridOutput.yData.Size(); j++)
        {
            gridOutput.gData[i][j] = nullReal;

            switch (gridZ) {
                case eaX :  {
                    if (gridX == eaY)
                    {
                        stVal = cData[stIndx][i][j];
                        eVal = cData[stIndx + 1][i][j];
                    }
                    else
                    {
                        stVal = cData[stIndx][i][j];
                        eVal = cData[stIndx + 1][i][j];
                    }
                    break;
                }
                case eaY :  {
                    if (gridX == eaX)
                    {
                        stVal = cData[i][stIndx][j];
                        eVal = cData[i][stIndx + 1][j];
                    }
                    else
                    {
                        stVal = cData[j][stIndx][i];
                        eVal = cData[j][stIndx + 1][i];
                    }
                    break;
                }
                case eaZ :  {
                    if (gridX == eaX)
                    {
                        stVal = cData[i][j][stIndx];
                        eVal = cData[i][j][stIndx + 1];
                    }
                    else
                    {
                        stVal = cData[j][i][stIndx];
                        eVal = cData[j][i][stIndx + 1];
                    }

                break;
                }
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

            gridOutput.gData[i][j]  = resVal;
        }


    return true;
}


const DC_DataCoord* DC_CubeData::GetAxes(ExtractAxes ax) const
{
    switch (ax) {
        case eaX : return &xData;
        case eaY : return &yData;
        case eaZ : return &zData;
    }
    return 0;
}

