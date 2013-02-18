///////////////////////////////////////////////////////////////////////////////////
//
// C_Geometry.cpp
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
// DESCRIPTION: Platform independent and application independent base claseess and
//              support classes.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/C_Geometry.h>

// **************************************** member classes

GeomMap::GeomMap(const GeomMap& a)
{
    LocalCopy(a);
}

GeomMap& GeomMap::operator= (const GeomMap& a)
{
    if (this != &a)
        LocalCopy(a);

    return *this;
}

void GeomMap::LocalCopy(const GeomMap& a)
{
    nassocGeom          = a.nassocGeom;
    assocGeomStartIndx  = a.assocGeomStartIndx;
    assocGeomMap        = a.assocGeomMap;
}

void GeomMap::DeAlloc()
{
    nassocGeom.DeAlloc();
    assocGeomStartIndx.DeAlloc();
    assocGeomMap.DeAlloc();
}

void GeomMap::Alloc(int nprimaryGeom,
                    int nassocGeomIndx)
{
    assocGeomMap.SetFixed();
    nassocGeom.Alloc(nprimaryGeom);
    assocGeomStartIndx.Alloc(nprimaryGeom);
    assocGeomMap.Alloc(nassocGeomIndx);
}

void GeomMap::Alloc(int nprimaryGeom)
{
    assocGeomMap.SetResizable();
    nassocGeom.Alloc(nprimaryGeom);
    assocGeomStartIndx.Alloc(nprimaryGeom);
    assocGeomMap.Alloc(nprimaryGeom * 3);
}

void GeomMap::AllocForAdd(int nprimaryGeom,
                          int maxAssoc)
{
    maxAssocMap = maxAssoc;

    assocGeomMap.SetFixed();
    nassocGeom.AllocAndSetSize(nprimaryGeom);
    assocGeomStartIndx.AllocAndSetSize(nprimaryGeom);
    assocGeomMap.AllocAndSetSize(nprimaryGeom * maxAssoc);
    int nextSt = 0;
    for (int i = 0; i < nprimaryGeom; i++)
        {
            nassocGeom[i] = 0;
            assocGeomStartIndx[i] = nextSt;
            nextSt += maxAssoc;
        }
}

void GeomMap::AllocForCount(int nprimaryGeom)
{
    nassocGeom.AllocAndFill(nprimaryGeom, 0);
    assocGeomStartIndx.AllocAndSetSize(nprimaryGeom);
}

void GeomMap::AllocFromCount()
{
    maxAssocMap = nassocGeom.GetMax();
    int nmapAlloc = nassocGeom.GetCount() + maxAssocMap;
    assocGeomMap.AllocAndSetSize(nmapAlloc);
    int nextSt = 0;
    for (int i = 0; i < nassocGeom.Size(); i++)
        {
            assocGeomStartIndx[i] = nextSt;
            nextSt += nassocGeom[i];
            nassocGeom[i] = 0;
        }
}


void GeomMap::AddAssocGeom(int  primaryGeomIndx,
                           int  assocIndex)
{
    int& nassoc = nassocGeom[primaryGeomIndx];
    if (nassoc == maxAssocMap)
        {
            int maxPrimary = assocGeomStartIndx.AllocSize();
            // resize
            SC_IntArray temp(assocGeomMap);
            maxAssocMap *= 2;
            assocGeomMap.AllocAndSetSize(maxAssocMap * maxPrimary);

            int nextSt = 0;
            for (int i = 0; i < maxPrimary; i++)
                {
                    int oldSt = assocGeomStartIndx[i];
                    assocGeomStartIndx[i] = nextSt;
                    int nassoc = nassocGeom[i];
                    for (int j = 0; j < nassoc; j++)
                        assocGeomMap[nextSt + j] = temp[oldSt++];
                    nextSt += maxAssocMap;
                }
        }

    assocGeomMap[assocGeomStartIndx[primaryGeomIndx] + nassoc] = assocIndex;
    nassoc++;
}


void GeomMap::AddAssocGeom(int  primaryGeomIndx,
                           const SC_IntArray& assocIndex)
{
    for (int i = 0; i < assocIndex.Size(); i++)
        AddAssocGeom(primaryGeomIndx, assocIndex[i]);
}


void GeomMap::AddCompress()
{
    int nextSt = 0;
    for (int i = 0; i < assocGeomStartIndx.Size(); i++)
        {
            int oldSt = assocGeomStartIndx[i];
            assocGeomStartIndx[i] = nextSt;
            int nassoc = nassocGeom[i];
            for (int j = 0; j < nassoc; j++)
                assocGeomMap[nextSt++] = assocGeomMap[oldSt++];
        }
    assocGeomMap.SetSize(nextSt);
}

void GeomMap::GetAssocGeom(int           primaryGeomIndx,
                           SC_IntArray&  assocGeomIndexes) const
{
    int nAssoc = nassocGeom[primaryGeomIndx];
    int stIndx = assocGeomStartIndx[primaryGeomIndx];

    assocGeomIndexes.AllocAndSetSize(nAssoc);
    for (int i = 0; i < nAssoc; i++)
        assocGeomIndexes[i] = assocGeomMap[stIndx++];
}

void GeomMap::GetBufferedAssocGeom(int            primaryGeomIndx,
                                   SC_IntArray&   assocGeomIndexes) const
{
    assocGeomIndexes.BufferAlloc(&assocGeomMap[assocGeomStartIndx[primaryGeomIndx]], nassocGeom[primaryGeomIndx]);
}

void GeomMap::SetAssocGeom(int                  primaryGeomIndx,
                           const SC_IntArray&   assocGeomIndexes)
{
    int nset = assocGeomIndexes.Size();
    nassocGeom[primaryGeomIndx] = nset;
    assocGeomStartIndx[primaryGeomIndx] = assocGeomMap.Size();

    if (nassocGeom.Size() <= primaryGeomIndx)
        {
            nassocGeom.SetSize(primaryGeomIndx + 1);
            assocGeomStartIndx.SetSize(primaryGeomIndx + 1);
        }

    if (nset > maxAssocMap)
        maxAssocMap = nset;

    for (int i = 0; i < nset; i++)
        assocGeomMap += assocGeomIndexes[i];
}

void GeomMap::SetAssocGeom(const SC_IntMatrix&   assocGeomIndexes)
{
    Alloc(assocGeomIndexes.Size(), assocGeomIndexes.MatrixSize());
    for (int i = 0; i < assocGeomIndexes.Size(); i++)
        SetAssocGeom(i, assocGeomIndexes[i]);
}

void GeomMap::UpdateAssocGeom(int            primaryGeomIndx,
                              const SC_IntArray&   assocGeomIndexes)
{
    int nset = assocGeomIndexes.Size();
    if (nassocGeom[primaryGeomIndx] < nset)
        GenAppInternalError("GeomMap::UpdateAssocGeom");

    nassocGeom[primaryGeomIndx] = nset;
    int stIndx = assocGeomStartIndx[primaryGeomIndx];

    for (int i = 0; i < nset; i++)
        assocGeomMap[stIndx++] = assocGeomIndexes[i];
}



void GeomMap::ReverseOrder(int primaryGeomIndx)
{
    SC_IntArray currentOrder;
    GetAssocGeom(primaryGeomIndx, currentOrder);

    int nAssoc = nassocGeom[primaryGeomIndx];
    int stIndx = assocGeomStartIndx[primaryGeomIndx] + nAssoc - 1;
    for (int i = 0; i < nAssoc; i++)
        assocGeomMap[stIndx--] = currentOrder[i];
}


void GeomMap::MakeAssocReverseMap(const SC_IntArray&   primaryReverseMap,
                                  SC_IntArray&         assocReverseMap,
                                  int                  maxAssocGeom)
{
    assocReverseMap.AllocAndFill(maxAssocGeom, -1);
    for (int i = 0; i < Size(); i++)
        if (primaryReverseMap[i] >= 0)
            {
                int stIndx = assocGeomStartIndx[i];
                for (int j = 0; j < nassocGeom[i]; j++)
                    assocReverseMap[assocGeomMap[stIndx++]] = 1;
            }

    assocReverseMap.SetReverseOK();
}




void GeomMap::MakePrimaryReverseMap(SC_IntArray&         primaryReverseMap,
                                    const SC_IntArray&   assocReverseMap)
{
    primaryReverseMap.AllocAndFill(Size(), -1);
    for (int i = 0; i < Size(); i++)
        {
            int stIndx = assocGeomStartIndx[i];
            bool allOK = true;
            for (int j = 0; j < nassocGeom[i]; j++)
                if (assocReverseMap[assocGeomMap[stIndx++]] < 0)
                    {
                        allOK = false;
                        break;
                    }
            if (allOK)
                primaryReverseMap[i] = 1;
        }
    primaryReverseMap.SetReverseOK();
}


void GeomMap::ApplyReverseMap(const SC_IntArray&   primaryReverseMap,
                              const SC_IntArray&   assocReverseMap)
{
    nassocGeom.ApplyReverseMap(primaryReverseMap);
    assocGeomStartIndx.ApplyReverseMap(primaryReverseMap);
    for (int i = 0; i < MapSize(); i++)
        {
            int currMap = assocGeomMap[i];
            if (currMap >= 0)
                assocGeomMap[i] = assocReverseMap[currMap];
        }
}

