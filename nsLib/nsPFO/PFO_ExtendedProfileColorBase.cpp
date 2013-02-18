///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ExtendedProfileColorBase.cpp
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
//      Plots grid lines and labels at sequence intervals.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genClass/C_Common.h>
#include <genClass/C_Graphic.h>
#include <genClass/SC_Triangulation.h>

#include <nsClass/DO_ExtendedProfile.h>

#include <nsPFO/PFO_ExtendedProfileColorBase.h>


PFO_ExtendedProfileColorBase :: PFO_ExtendedProfileColorBase(const char oID[], PlotDefC& assPlt) :
    PFO_ColorBase(oID, assPlt)
{
    InitCommon(assPlt);
}

PFO_ExtendedProfileColorBase::PFO_ExtendedProfileColorBase(const PFO_ExtendedProfileColorBase& a, PlotDefC& assPlt) :
    PFO_ColorBase(a, assPlt), PSC_3DExtendedProfileData(a)
{
    InitCommon(assPlt);
    LocalCopy(a);
}

PFO_ExtendedProfileColorBase :: ~PFO_ExtendedProfileColorBase ()
{
}

void PFO_ExtendedProfileColorBase::InitCommon(PlotDefC& assPlt)
{
    PSC_3DExtendedProfileData::InitCommon(*this);

    AddInPort(inputDataObjRef, typeid(DO_ExtendedProfile));

    DoStatusChk();
}

PFO_ExtendedProfileColorBase& PFO_ExtendedProfileColorBase::operator= (const PFO_ExtendedProfileColorBase& a)
{
    if (&a != this)
    {
        PFO_ColorBase::operator=(a);
        PSC_3DExtendedProfileData::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_ExtendedProfileColorBase::LocalCopy(const PFO_ExtendedProfileColorBase& a)
{
}


void  PFO_ExtendedProfileColorBase:: DoStatusChk()
{
    autoLimitToData = (baseAutoDataSource == 1);
    PSC_3DExtendedProfileData::StatusChkSetup(*this);

    PFO_ColorBase::DoStatusChk();
    if (StatusNotOK())
        return;

    if (!CheckInputFO(inputDataObjRef, "Grid Data"))
        return;

    extendedProfileData = static_cast<DO_ExtendedProfile*> (GetInPortData(inputDataObjRef))->profileResults;

    PSC_3DExtendedProfileData::DoStatusChk(*this, *extendedProfileData);

    // check and set triang
    SC_Triangulation& currTri = extendedProfileData->GetTriangulation();
    if (!currTri.TriangulationOK())
    {
        SetObjErrMsg("cannot triangulate ??");
        return;
    }

    if (limitIsMaster && autoLimits)
    {
        if (autoLimitToData)
        {
            SetRangeInView();
        }
        else
        {
            SC_DoubleArray blockData;
            extendedProfileData->GetData(blockData);
            rangeLimit.InitLimit(blockData);
        }
    }

    CheckRangeLimits();

    recalcOnViewChange   = (limitIsMaster && autoLimits && autoLimitToData);

    colorLimitDO.SetValueLabel(GetID());
}


void PFO_ExtendedProfileColorBase::SetRangeInView()
{
    Limit3D currLimits = GetCurrentViewLimits();
    //  if no limits set ??
    if (RealIsNull(currLimits.minLim.cX))
    {
        SetObjErrMsg("no limits (???) - cannot set range");
        return;
    }

    // if 2D set Z limits to null for limit check
    if (!is3DObject)
    {
        currLimits.minLim.cZ = nullReal;
        currLimits.maxLim.cZ = nullReal;
    }

    bool first = true;
    Coord3D pointCoord;
    for (int i = 0; i < extendedProfileData->Size(); i++)
    {
        pointCoord = GetCoord(i);
        MapCoords(pointCoord);
        if (currLimits.WithinLimits(pointCoord))
        {
            double gridZ = (*extendedProfileData)[i];
            if (first)
            {
                rangeLimit.InitLimit(gridZ);
                first = false;
            }
            else
                rangeLimit.AddToLimit(gridZ);
        }
    }

    if (first)
        SetObjErrMsg("no data in view - cannot set range");
}


void PFO_ExtendedProfileColorBase:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    CalcDataColors();

    SetDefaultID(GetID());
}

void  PFO_ExtendedProfileColorBase::ViewChangeRecalc()
{
    DoStatusChk();
    if (StatusOK())
        CalcDataColors();
}

Limit3D PFO_ExtendedProfileColorBase::GetPlotObjLimits()
{
    Limit3D  currLimits;

    DoStatusChk();
    if (StatusNotOK())
        return currLimits;

    bool xLin, yLin, zLin;
    if (!is3DObject)
        GetAxesTypes(xLin, yLin); 
    else
        GetAxesTypes(xLin, yLin, zLin);

    Coord3D pointCoord;
    for (int i = 0; i < extendedProfileData->Size(); i++)
    {
        pointCoord = GetCoord(i);
        MapCoords(pointCoord);

        double xVal = pointCoord.cX;
        double yVal = pointCoord.cY;
        if (!is3DObject)
        {
            if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps))) 
                currLimits.AddToLimits(pointCoord); 
        }
        else
        {
            double zVal = pointCoord.cZ;
            if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)) && (zLin || (zVal > stdEps)))
                currLimits.AddToLimits(pointCoord);
        }
    }
    return currLimits;
}


void PFO_ExtendedProfileColorBase::CalcDataColors()
{
    SC_DoubleArray blockData;
    extendedProfileData->GetData(blockData);
    rangeLimit.MapDataToRange(blockData, extendedProfileColors, clipToRange, colorMap->ncolorMap);
}

Coord3D PFO_ExtendedProfileColorBase::GetCoord(int indx)
{
    return Coord3D(extendedProfileData->GetPoint(indx), GetZVal(indx, *extendedProfileData));
}

