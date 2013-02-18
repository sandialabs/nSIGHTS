///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableLabels.cpp
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
//      Plotting objects for covariance data
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>
#include <genClass/SC_PointArray.h>

#include <osPFO/PFO_TableLabels.h>


PFO_TableLabels :: PFO_TableLabels(PlotDefC& assPlt)
    : PlotObjC("Table Labels", assPlt)
{
    matrixPlotOption = mpoFull;

    InitCommon();
}

PFO_TableLabels::PFO_TableLabels(const PFO_TableLabels& a, PlotDefC& assPlt) : PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_TableLabels :: ~PFO_TableLabels ()
{
}

void PFO_TableLabels::InitCommon()
{
    reportOK        = false;                // no reporting
    usePolyOffset   = is3DObject;

    AddInPort(tableObjRef,  typeid(DO_TableData));

    DoStatusChk();
}

PFO_TableLabels& PFO_TableLabels::operator= (const PFO_TableLabels& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}



void  PFO_TableLabels::LocalCopy(const PFO_TableLabels& a)
{
    tableObjRef = a.tableObjRef;
    matrixPlotOption = a.matrixPlotOption;
}

void  PFO_TableLabels:: DoStatusChk()
{
    // base class check
    PlotObjC::DoStatusChk();

    if (!CheckInputFO(tableObjRef, "Table Label Data"))
        return;
    DO_TableData* objData = static_cast<DO_TableData*>(GetInPortData(tableObjRef));
    tableLabelsDC = objData->tableData;
}

void PFO_TableLabels:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
       return;
}

Limit3D PFO_TableLabels::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    currLimits.AddToLimits(Point2D(0.0, 0.0));
    currLimits.AddToLimits(Point2D(double(tableLabelsDC->GetNCols()), double(tableLabelsDC->GetNRows())));

    return currLimits;

}

