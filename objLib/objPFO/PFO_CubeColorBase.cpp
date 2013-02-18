///////////////////////////////////////////////////////////////////////////////////
//
// PFO_CubeColorBase.cpp
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
//    Plotting functional objects for cube, grid, tables and XY data.
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genClass/C_Common.h>
#include <genClass/C_Graphic.h>
#include <genClass/SC_Triangulation.h>

#include <objClass/DO_CubeData.h>

#include <objPFO/PFO_CubeColorBase.h>


PFO_CubeColorBase :: PFO_CubeColorBase(const char oID[], PlotDefC& assPlt, bool useIndx) :
    PFO_ColorBase(oID, assPlt), useIndexes(useIndx)
{
    InitCommon(assPlt);
}

PFO_CubeColorBase::PFO_CubeColorBase(const PFO_CubeColorBase& a, PlotDefC& assPlt) :
    PFO_ColorBase(a, assPlt), PSC_3DCubeData(a), useIndexes(a.useIndexes)
{
    InitCommon(assPlt);
    LocalCopy(a);
}

PFO_CubeColorBase :: ~PFO_CubeColorBase ()
{
}

void PFO_CubeColorBase::InitCommon(PlotDefC& assPlt)
{
    if (useIndexes)
        PSC_3DCubeData::InitCommon(*this);

    AddInPort(inputDataObjRef, typeid(DO_CubeData));

    DoStatusChk();
}

PFO_CubeColorBase& PFO_CubeColorBase::operator= (const PFO_CubeColorBase& a)
{
    if (&a != this)
    {
        PFO_ColorBase::operator=(a);
        PSC_3DCubeData::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_CubeColorBase::LocalCopy(const PFO_CubeColorBase& a)
{
}


void  PFO_CubeColorBase:: DoStatusChk()
{
    autoLimitToData = (baseAutoDataSource == 1);

    PFO_ColorBase::DoStatusChk();
    if (StatusNotOK())
        return;

    if (!CheckInputFO(inputDataObjRef, "Cube Data"))
        return;

    cubeData = static_cast<DO_CubeData*> (GetInPortData(inputDataObjRef))->cubeData;

    if (useIndexes)
        PSC_3DCubeData::DoStatusChk(*this, *cubeData);
    else
        if (!coordMap.SetupOK())
            SetObjErrMsg("one or more coord maps map to same");

    if (limitIsMaster && autoLimits)
    {
        if (autoLimitToData)
        {
            SetRangeInView();
        }
        else
        {
            SC_DoubleArray blockData;
            cubeData->GetData(blockData);
            rangeLimit.InitLimit(blockData);
        }
    }

    CheckRangeLimits();

    recalcOnViewChange   = (limitIsMaster && autoLimits && autoLimitToData);

    colorLimitDO.SetValueLabel(cubeData->GetID());
}


void PFO_CubeColorBase::SetRangeInView()
{
    Limit3D currLimits = GetCurrentViewLimits();
    //  if no limits set ??
    if (RealIsNull(currLimits.minLim.cX))
    {
        SetObjErrMsg("no limits (???) - cannot set range");
        return;
    }

    bool first = true;
    Coord3D pointCoord;

    SC_IntArray& currIndexes = GetIndexes();

    for (int i = 0; i < currIndexes.Size(); i++)
    {
        int cubeIndx = currIndexes[i];
        pointCoord = GetCoord(cubeIndx);
        MapCoords(pointCoord);
        if (currLimits.WithinLimits(pointCoord))
        {
            double cubeZ = (*cubeData)[cubeIndx];
            if (first)
            {
                rangeLimit.InitLimit(cubeZ);
                first = false;
            }
            else
                rangeLimit.AddToLimit(cubeZ);
        }
    }

    if (first)
        SetObjErrMsg("no data in view - cannot set range");
}


void PFO_CubeColorBase:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    CalcDataColors();

    SetDefaultID(cubeData->GetID());
}

void  PFO_CubeColorBase::ViewChangeRecalc()
{
    DoStatusChk();
    if (StatusOK())
        CalcDataColors();
}

Limit3D PFO_CubeColorBase::GetPlotObjLimits()
{
    Limit3D  currLimits;

    DoStatusChk();
    if (StatusNotOK())
        return currLimits;

    SC_IntArray& currIndexes = GetIndexes();

    Coord3D pointCoord;
    for (int i = 0; i < currIndexes.Size(); i++)
    {
        pointCoord = GetCoord(currIndexes[i]);
        MapCoords(pointCoord);
        currLimits.AddToLimits(pointCoord);
    }
    return currLimits;
}


void PFO_CubeColorBase::CalcDataColors()
{
    if (!useIndexes)
        return;

    SC_IntArray& currIndexes = GetIndexes();

    SC_DoubleArray blockData(currIndexes.Size());
    for (int i = 0; i < currIndexes.Size(); i++)
        blockData[i] = (*cubeData)[currIndexes[i]];

    rangeLimit.MapDataToRange(blockData, cubeColors, clipToRange, colorMap->ncolorMap);
}

Coord3D PFO_CubeColorBase::GetCoord(int indx)
{
    return cubeData->GetCoord(indx);
}

