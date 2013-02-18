///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableHorsetail.cpp
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
//    Plots a horsetail plot based on all columns in a table (a single column is
// selected for all X values).
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objPFO/PFO_TableHorsetail.h>


PFO_TableHorsetail :: PFO_TableHorsetail(PlotDefC& assPlt)
    : PlotObjC("Table Horsetail", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_TableHorsetail::PFO_TableHorsetail(const PFO_TableHorsetail& a, PlotDefC& assPlt) :
    PlotObjC(a, assPlt), DSC_TableColSel(a), PSC_Horsetail(a)
{
    LocalCopy(a);
    InitCommon();
}

PFO_TableHorsetail :: ~PFO_TableHorsetail ()
{
}

void PFO_TableHorsetail::InitCommon()
{
    reportOK = false;

    DSC_TableColSel::AddPort(*this);
    PSC_Horsetail::AddPort(*this);

    DoStatusChk();
}

PFO_TableHorsetail& PFO_TableHorsetail::operator= (const PFO_TableHorsetail& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        DSC_TableColSel::operator=(a);
        PSC_Horsetail::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_TableHorsetail::LocalCopy(const PFO_TableHorsetail& a)
{
}


void  PFO_TableHorsetail:: DoStatusChk()
{
    PlotObjC::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
    PSC_Horsetail::DoCheck(*this);
}


void PFO_TableHorsetail:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

bool PFO_TableHorsetail::ContainsLinesOnly()
{
    return true;
}

Limit3D PFO_TableHorsetail::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    const SC_DoubleArray& xData = *selectedColData;
    int maxCol = inputData->GetNCols();
    int nx = xData.Size();
    for (int i = 0; i < maxCol; i++)
        if (i != selectedCol)
        {
            SC_DoubleArray& yData = inputData->dataTable[i];
            for (int j = 0; j < nx; j++)
            {
                double xVal = xData[j];
                double yVal = yData[j];
                if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
                    currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
            }
        }

    return currLimits;
}

