///////////////////////////////////////////////////////////////////////////////////
//
// PFO_XYHorsetail.cpp
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

#include <genClass/U_Real.h>

#include <objClass/DO_XYDataArray.h>

#include <objPFO/PFO_XYHorsetail.h>


PFO_XYHorsetail :: PFO_XYHorsetail(PlotDefC& assPlt)
    : PlotObjC("XY Horsetail", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_XYHorsetail::PFO_XYHorsetail(const PFO_XYHorsetail& a, PlotDefC& assPlt) :
    PlotObjC(a, assPlt), PSC_Horsetail(a)
{
    LocalCopy(a);
    InitCommon();
}

PFO_XYHorsetail :: ~PFO_XYHorsetail ()
{
}

void PFO_XYHorsetail::InitCommon()
{
    reportOK = false;

    AddInPort(xyDataObjRef, typeid(DO_XYDataArray));

    PSC_Horsetail::AddPort(*this);

    DoStatusChk();
}

PFO_XYHorsetail& PFO_XYHorsetail::operator= (const PFO_XYHorsetail& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        PSC_Horsetail::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_XYHorsetail::LocalCopy(const PFO_XYHorsetail& a)
{
    xyDataObjRef = a.xyDataObjRef;
}


void  PFO_XYHorsetail:: DoStatusChk()
{
    PlotObjC::DoStatusChk();

    PSC_Horsetail::DoCheck(*this);

    if (CheckInputFO(xyDataObjRef, "XY data "))
        xyDataDC = static_cast<DO_XYDataArray*>(GetInPortData(xyDataObjRef))->xyDataArray;

}


void PFO_XYHorsetail:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

bool PFO_XYHorsetail::ContainsLinesOnly()
{
    return true;
}


Limit3D PFO_XYHorsetail::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    for (int i = 0; i < xyDataDC->Size(); i++)
    {
        SC_DoubleArray& xData = (*xyDataDC)[i].xData;
        SC_DoubleArray& yData = (*xyDataDC)[i].yData;
        for (int j = 0; j < xData.Size(); j++)
        {
            double xVal = xData[j];
            double yVal = yData[j];
            if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
                currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
        }
    }

    return currLimits;
}

