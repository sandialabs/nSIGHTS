///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DAnalytic.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/SC_RealFormat.h>

#include <objPFO/PFO_2DAnalytic.h>


PFO_2DAnalytic :: PFO_2DAnalytic(PlotDefC& assPlt)
    : ActiveObjC("Analytic Line", assPlt, "Analytics")
{
    centerPtSymbol   = PSC_SymbolSpec(ps_FCircle, 8, 6);
    rotationPtSymbol = PSC_SymbolSpec(ps_FCircle, 6, 6);
    endPtSymbol      = PSC_SymbolSpec(ps_FCircle, 6, 6);
    lineSpec         = PSC_LineSpec(plt_Solid, 2, 6);

    InitCommon();
}

PFO_2DAnalytic::PFO_2DAnalytic(const PFO_2DAnalytic& a, PlotDefC& assPlt) : ActiveObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_2DAnalytic :: ~PFO_2DAnalytic ()
{
}

void PFO_2DAnalytic::InitCommon()
{
    AddOutPort(outputCxDO);
    AddOutPort(outputCyDO);
    AddOutPort(outputDxDO);
    AddOutPort(outputDyDO);
    AddOutPort(outputDDO);
    AddOutPort(outputMDO);
    AddOutPort(outputBDO);

    outputCxDO.Reset("Center X");
    outputCyDO.Reset("Center Y");
    outputDxDO.Reset("X length");
    outputDyDO.Reset("Y length");
    outputDDO.Reset("Length");
    outputMDO.Reset("Slope");
    outputBDO.Reset("Y intercept");

    DoStatusChk();
}

PFO_2DAnalytic& PFO_2DAnalytic::operator= (const PFO_2DAnalytic& a)
{
    if (&a != this)
    {
        ActiveObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_2DAnalytic::LocalCopy(const PFO_2DAnalytic& a)
{
    centerPtSymbol      = a.centerPtSymbol;
    rotationPtSymbol    = a.rotationPtSymbol;
    endPtSymbol         = a.endPtSymbol;
    lineSpec            = a.lineSpec;
}

void  PFO_2DAnalytic::DoStatusChk()
{
    ActiveObjC::DoStatusChk();

}


void PFO_2DAnalytic:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (lineCenter.PointIsNull())
    {
        Limit2D currLimits = GetCurrentViewLimits();
        GetAxesTypes(xLin, yLin);  // just to be sure ..
        Transform(currLimits.maxLim);
        Transform(currLimits.minLim);

        lineCenter.pX = (currLimits.maxLim.pX  + currLimits.minLim.pX) / 2.0;
        lineCenter.pY = (currLimits.maxLim.pY  + currLimits.minLim.pY) / 2.0;

        lineStart = lineCenter;
        lineStart.pX -= (currLimits.maxLim.pX  - currLimits.minLim.pX) / 4.0;

        UnTransform(lineStart);
        UnTransform(lineCenter);
    }

    CalcPoints();
}


Limit3D PFO_2DAnalytic::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    currLimits.AddToLimits(Coord3D(lineStart));
    currLimits.AddToLimits(Coord3D(lineCenter));
    currLimits.AddToLimits(Coord3D(lineEnd));

    return currLimits;
}


void PFO_2DAnalytic::CalcPoints()
{
    if (lineCenter.PointIsNull() || lineStart.PointIsNull())
        return;

    outputCxDO.InitLabelAndValue(lineCenter.pX);
    outputCyDO.InitLabelAndValue(lineCenter.pY);

    GetAxesTypes(xLin, yLin);

    // all calcs on transformed points
    Transform(lineCenter);
    Transform(lineStart);

    Line2D temp(lineStart, lineCenter);
    lineRotPointA = temp.PointOnLine(0.5);
    lineRotPointB = temp.PointOnLine(1.5);
    lineEnd = temp.PointOnLine(2.0);

    SC_SciFormat defForm(5);

    outputDxDO.InitLabelAndValue(fabs(lineEnd.pX - lineStart.pX), defForm);
    outputDyDO.InitLabelAndValue(fabs(lineEnd.pY - lineStart.pY), defForm);
    outputDDO.InitLabelAndValue(lineEnd.Distance(lineStart), defForm);

    double dx = (lineEnd.pX - lineStart.pX);
    if (fabs(dx) < stdEps)
    {
        outputMDO.InitLabelAndValue(nullReal, defForm);
        outputBDO.InitLabelAndValue(nullReal, defForm);
    }
    else
    {
        double m = (lineEnd.pY - lineStart.pY) / dx;
        outputMDO.InitLabelAndValue(m, defForm);

        double b = lineCenter.pY - m * lineCenter.pX;
        outputBDO.InitLabelAndValue(b, defForm);
    }

    // untran all points
    UnTransform(lineStart);
    UnTransform(lineRotPointA);
    UnTransform(lineCenter);
    UnTransform(lineRotPointB);
    UnTransform(lineEnd);

    UpdateUI();
}

void PFO_2DAnalytic::UpdateUI() {};

void PFO_2DAnalytic::Transform(Point2D& inPt)
{
    if (!xLin)
        inPt.pX = log10(inPt.pX);
    if (!yLin)
        inPt.pY = log10(inPt.pY);
}
void PFO_2DAnalytic::UnTransform(Point2D& inPt)
{
    if (!xLin)
        inPt.pX = InvLgt(inPt.pX);
    if (!yLin)
        inPt.pY = InvLgt(inPt.pY);
}


