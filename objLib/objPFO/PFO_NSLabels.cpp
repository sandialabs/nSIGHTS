///////////////////////////////////////////////////////////////////////////////////
//
// PFO_NSLabels.cpp
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

#include <objPFO/PFO_NSLabels.h>


PFO_NSLabels :: PFO_NSLabels(const char* pltType, PlotDefC& assPlt)
    : PlotObjC(pltType, assPlt)
{
    InitCommon();
}

PFO_NSLabels::PFO_NSLabels(const PFO_NSLabels& a, PlotDefC& assPlt)
    : PlotObjC(a, assPlt)
{
    InitCommon();
    LocalCopy(a);
}

PFO_NSLabels :: ~PFO_NSLabels ()
{
}

void PFO_NSLabels::InitCommon()
{
    reportOK        = false;
    usePolyOffset   = true;

    AddInPort(labelsObjRef,   typeid(DO_LabelArray));
    DoStatusChk();
}

PFO_NSLabels& PFO_NSLabels::operator= (const PFO_NSLabels& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }

    return *this;
}


void  PFO_NSLabels::LocalCopy(const PFO_NSLabels& a)
{
    labelsObjRef = a.labelsObjRef;
}

void  PFO_NSLabels:: DoStatusChk()
{
    PlotObjC::DoStatusChk();

    if (!CheckInputFO(labelsObjRef, "Labels data"))
        return;

    labelsObj = static_cast<DO_LabelArray*>(GetInPortData(labelsObjRef));

    if (!labelsObj->LabelsAvailable())
        SetObjErrMsg("No data in input list");
}

void PFO_NSLabels::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

Limit3D PFO_NSLabels::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK())
        return currLimits;

    SC_CoordArray& coords = labelsObj->dataCoords;
    for (int i = 0; i < coords.Size(); i++ )
        if (!coords[i].CoordIsNull())
            currLimits.AddToLimits(coords[i] + offsetCoord);

    return currLimits;
}

