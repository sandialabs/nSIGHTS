///////////////////////////////////////////////////////////////////////////////////
//
// PFO_VariableArrayGridLines.cpp
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
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <objClass/DO_VariableArray.h>

#include <objPFO/PFO_VariableArrayGridLines.h>


PFO_VariableArrayGridLines :: PFO_VariableArrayGridLines(PlotDefC& assPlt)
    :PlotObjC("VariableArrayGridLines", assPlt)
{
    labelFormat.labelType = PSC_GridLineLabelFormat::ltText;
    labelFormat.labelPosition = 80.0;
    arrayIsX = true;
    InitCommon();
}

PFO_VariableArrayGridLines::PFO_VariableArrayGridLines(const PFO_VariableArrayGridLines& a, PlotDefC& assPlt)
    :PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_VariableArrayGridLines::~PFO_VariableArrayGridLines ()
{
}

void PFO_VariableArrayGridLines::InitCommon()
{
    reportOK = false;
    AddInPort(variableArrayObjRef, typeid(DO_VariableArray));
    DoStatusChk();
}

PFO_VariableArrayGridLines& PFO_VariableArrayGridLines::operator= (const PFO_VariableArrayGridLines& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_VariableArrayGridLines::LocalCopy(const PFO_VariableArrayGridLines& a)
{
    variableArrayObjRef = a.variableArrayObjRef;
    gridLineSpec        = a.gridLineSpec;
    labelFormat         = a.labelFormat;
    labelFont           = a.labelFont;
    valueFormat         = a.valueFormat;
    arrayIsX            = a.arrayIsX;
}


void  PFO_VariableArrayGridLines:: DoStatusChk()
{
    variableArrayDC = 0;

    // base class check
    PlotObjC::DoStatusChk();

        // check input objects
    CheckInputFO(variableArrayObjRef, "Variable array");
    if (StatusNotOK())
        return;

    variableArrayDC = &static_cast<const DO_VariableArray*>(GetInPortData(variableArrayObjRef))->GetArrayRef();
}


void PFO_VariableArrayGridLines:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

Limit3D PFO_VariableArrayGridLines::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (! doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    for (int i = 0; i < variableArrayDC->Size(); i++)
    {
        double varVal = (*variableArrayDC)[i].GetValue();
        if (arrayIsX)
        {
            if (xLin || (varVal > stdEps))
                currLimits.AddToLimits(Coord3D(varVal, nullReal, nullReal));
        }
        else
        {
            if (yLin || (varVal > stdEps))
                currLimits.AddToLimits(Coord3D(nullReal, varVal,  nullReal));
        }
    }

    return currLimits;
}

