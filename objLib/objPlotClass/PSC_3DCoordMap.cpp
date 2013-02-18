///////////////////////////////////////////////////////////////////////////////////
//
// PSC_3DCoordMap.cpp
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

#include <genClass/U_Real.h>

#include <objClass/DO_GridData.h>

#include <objPlotClass/PSC_3DCoordMap.h>

PSC_3DCoordMap :: PSC_3DCoordMap()
{
    xMap = cmtX;
    yMap = cmtY;
    zMap = cmtZ;
}

PSC_3DCoordMap::PSC_3DCoordMap(const PSC_3DCoordMap& a)
{
    LocalCopy(a);
}



PSC_3DCoordMap& PSC_3DCoordMap::operator= (const PSC_3DCoordMap& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void  PSC_3DCoordMap::LocalCopy(const PSC_3DCoordMap& a)
{
    xMap = a.xMap;
    yMap = a.yMap;
    zMap = a.zMap;
}


bool PSC_3DCoordMap::SetupOK()
{
    int mapChk[3];

    mapChk[0] = 0;
    mapChk[1] = 0;
    mapChk[2] = 0;

    mapChk[xMap]++;
    mapChk[yMap]++;
    mapChk[zMap]++;

    return (mapChk[0] == 1) && (mapChk[1] == 1) && (mapChk[2] == 1);
}


void PSC_3DCoordMap::MapCoords(Coord3D& inCoord)
{
    Coord3D temp;
    switch (xMap)
    {
        case cmtX : {temp.cX = inCoord.cX; break;}
        case cmtY : {temp.cY = inCoord.cX; break;}
        case cmtZ : {temp.cZ = inCoord.cX; break;}
    }
    switch (yMap)
    {
        case cmtX : {temp.cX = inCoord.cY; break;}
        case cmtY : {temp.cY = inCoord.cY; break;}
        case cmtZ : {temp.cZ = inCoord.cY; break;}
    }
    switch (zMap)
    {
        case cmtX : {temp.cX = inCoord.cZ; break;}
        case cmtY : {temp.cY = inCoord.cZ; break;}
        case cmtZ : {temp.cZ = inCoord.cZ; break;}
    }

    inCoord = temp;
}


void PSC_3DCoordMap::MapCoords(SC_CoordArray& inCoords)
{
    for (int i = 0; i < inCoords.Size(); i++)
        MapCoords(inCoords[i]);
}


