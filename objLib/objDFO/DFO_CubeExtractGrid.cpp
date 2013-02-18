///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CubeExtractGrid.cpp
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
//      Extracts plane of data from cube.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <objDFO/DFO_CubeExtractGrid.h>

DFO_CubeExtractGrid :: DFO_CubeExtractGrid() :
    FuncObjC("Extract Grid from Cube"), extractValMS("Extract XYZ")
{
    gridX     = DC_CubeData::eaX;
    gridY     = DC_CubeData::eaY;
    logInterp = false;
    InitCommon();
}

DFO_CubeExtractGrid::DFO_CubeExtractGrid(const DFO_CubeExtractGrid& a) :
    FuncObjC(a), extractValMS(a.extractValMS), DSC_CubeInput(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_CubeExtractGrid :: ~DFO_CubeExtractGrid ()
{
}

void DFO_CubeExtractGrid::InitCommon()
{
    extractValMS.AddPort(*this);
    DSC_CubeInput::AddPort(*this);
    outputGridDO.gridData = &outputGridDC;
    AddOutPort(outputGridDO);
    DoStatusChk();
}

DFO_CubeExtractGrid& DFO_CubeExtractGrid::operator= (const DFO_CubeExtractGrid& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        DSC_CubeInput::operator=(a);
        extractValMS = a.extractValMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_CubeExtractGrid::LocalCopy(const DFO_CubeExtractGrid& a)
{
    gridX = a.gridX;
    gridY = a.gridY;
    logInterp = a.logInterp;
}

void  DFO_CubeExtractGrid::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    DSC_CubeInput::DoCheck(*this);
    extractValMS.DoCheck("Extract XY Source", *this);

    if (gridX == gridY)
        SetObjErrMsg("Grid X and Grid Y must be different");

    if (StatusNotOK())
        return;

    DC_DataCoord* currXYZ;

    DC_CubeData::ExtractAxes gridZ = DC_CubeData::ExtractAxes(3 - int(gridX) - int(gridY));
    switch (gridZ)
    {
        case DC_CubeData::eaX : {
            currXYZ = &(cubeInputDC->xData);
            break;
        }
        case DC_CubeData::eaY : {
            currXYZ = &(cubeInputDC->yData);
            break;
        }
        case DC_CubeData::eaZ : {
            currXYZ = &(cubeInputDC->zData);
            break;
        }
    }

    extractVal = extractValMS.GetCurrentValue();

    int dummy;
    if (!currXYZ->GetIndex(extractVal, dummy))
        SetObjErrMsg("value out of grid Z range");
}


void DFO_CubeExtractGrid:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (!cubeInputDC->ExtractGrid(extractVal, gridX, gridY, logInterp, outputGridDC))
        SetObjErrMsg("extraction error -- log ??");

    outputGridDC.SetID(GetID());

}

