///////////////////////////////////////////////////////////////////////////////////
//
// DFO_GridTile.cpp
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

#include <genClass/U_Str.h>

#include <objDFO/DFO_GridTile.h>

DFO_GridTile::GridInputData::GridInputData() :
    doGrid(false)
{
}
DFO_GridTile::GridInputData::GridInputData(const GridInputData& a)
{
    LocalCopy(a);
}
DFO_GridTile::GridInputData& DFO_GridTile::GridInputData::operator= (const GridInputData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void DFO_GridTile::GridInputData::LocalCopy(const GridInputData& a)
{
    doGrid     = a.doGrid;
    gridObjRef = a.gridObjRef;
}

DFO_GridTile :: DFO_GridTile()
    : FuncObjC("Tile Grids"), inputGridDC(32)
{
    logInterpolate  = true;
    dropoutFixCount = 0;

    InitCommon();
}

DFO_GridTile::DFO_GridTile(const DFO_GridTile& a)
    : FuncObjC(a), inputGridDC(32)
{
    LocalCopy(a);
    InitCommon();
}

DFO_GridTile :: ~DFO_GridTile ()
{
}

void DFO_GridTile::InitCommon()
{
    AddInPort(baseGridObjRef, typeid(DO_GridData));
    AddOutPort(outputGridDO);

    for (int i = 0; i < maxInputGrid; i++)
        AddUseFlagInPort(gridsToAdd[i].gridObjRef, typeid(DO_GridData), gridsToAdd[i].doGrid);

    outputGridDO.gridData = &outputGridDC;

    DoStatusChk();
}

DFO_GridTile& DFO_GridTile::operator= (const DFO_GridTile& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_GridTile::LocalCopy(const DFO_GridTile& a)
{
    logInterpolate  = a.logInterpolate;
    dropoutFixCount = a.dropoutFixCount;

    for (int i = 0; i < maxInputGrid; i++)
        gridsToAdd[i] = a.gridsToAdd[i];
}


void  DFO_GridTile:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    if (!CheckInputFO(baseGridObjRef, "base grid"))
        return;

    inputBaseGridDC = static_cast<DO_GridData*>(GetInPortData(baseGridObjRef))->gridData;

    // check input objects
    inputGridDC.SetEmpty();

    for (int i = 0; i < maxInputGrid; i++)
        if (gridsToAdd[i].doGrid)
        {
            if (!CheckInputFO(gridsToAdd[i].gridObjRef, "input table"))
                return;

            const DC_GridData* nextGrd = static_cast<DO_GridData*>(GetInPortData(gridsToAdd[i].gridObjRef))->gridData;

            inputGridDC += nextGrd;
        }

}


void DFO_GridTile::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
    {
        outputGridDC.DeAlloc();
        return;
    }

    if (!outputGridDC.CreateFrom(*inputBaseGridDC))
    {
        SetObjErrMsg("memory allocation ??");
        return;
    }

    outputGridDC.gData.MatrixFill(nullReal);


    for (int k = 0; k < inputGridDC.Size(); k++)
    {
        const DC_GridData& inputGrd = *(inputGridDC[k]);

        for ( int i = 0; i < outputGridDC.xData.Size(); i++)
        {
            double xVal = outputGridDC.xData[i];
            for ( int j = 0; j < outputGridDC.yData.Size(); j++)
            {
                double yVal = outputGridDC.yData[j];
                double temp;
                if (inputGrd.ExtractPt(Point2D(xVal, yVal), logInterpolate, temp))
                    outputGridDC.gData[i][j] = temp;
            }
        }
    }

    outputGridDC.FixDropouts(dropoutFixCount, logInterpolate);
    outputGridDC.SetID(GetID());
}


