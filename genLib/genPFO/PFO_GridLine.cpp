///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridLine.cpp
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
//      adds a labeled grid line at a specific location on an XY plot.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <genPFO/PFO_GridLine.h>


PFO_GridLine :: PFO_GridLine(PlotDefC& assPlt)
    : PlotObjC("Grid Line", assPlt),
      gridValue("GridVal")
{
    gridIsX             = true;
    gridLinePen         = 6;
    gridLineThickness   = 1;
    gridLineType        = plt_Solid;

    SetToNull(gridLabel);

    InitCommon();
}

PFO_GridLine::PFO_GridLine(const PFO_GridLine& a, PlotDefC& assPlt) :
    PlotObjC(a, assPlt), gridValue(a.gridValue)
{
    LocalCopy(a);
    InitCommon();
}

PFO_GridLine :: ~PFO_GridLine ()
{
}

void PFO_GridLine::InitCommon()
{
    reportOK        = false;                // no reporting
    usePolyOffset   = true;

    // no out port for Z
    AddMSInPort(gridValue.valueMasterObjRef, typeid(DO_Real), gridValue.valueIsMaster);

    DoStatusChk();
}

PFO_GridLine& PFO_GridLine::operator= (const PFO_GridLine& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

bool PFO_GridLine::ContainsLinesOnly()
{
    return true;
}


void  PFO_GridLine::LocalCopy(const PFO_GridLine& a)
{
    gridIsX             = a.gridIsX;
    gridLinePen         = a.gridLinePen;
    gridLineThickness   = a.gridLineThickness;
    gridLineType        = a.gridLineType;
    labelFormat         = a.labelFormat;
    labelFont           = a.labelFont;
    valueFormat         = a.valueFormat;

    CopyString(gridLabel, a.gridLabel, maxLabelLen);
}


void  PFO_GridLine:: DoStatusChk()
{
    // base class check
    PlotObjC::DoStatusChk();

    gridValue.DoCheck("Grid value", *this);
    if (StatusNotOK())
        return;

    gVal = gridValue.GetCurrentValue();

}


void PFO_GridLine:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

Limit3D PFO_GridLine::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (! doPlot))
        return currLimits;

    if (gridIsX)
    {
        currLimits.minLim.cX = gVal;
        currLimits.maxLim.cX = gVal;
    }
    else
    {
        currLimits.minLim.cY = gVal;
        currLimits.maxLim.cY = gVal;
    }

    return currLimits;

}

