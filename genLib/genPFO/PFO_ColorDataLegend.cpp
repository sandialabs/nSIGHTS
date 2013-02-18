///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ColorDataLegend.cpp
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
//      Basic plotting objects used by all applications, primarily concerned with plot annotation:
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/SC_DoubleArray.h>

#include <genPlotClass/C_PlotDef.h>

#include <genClass/DO_ColorLimit.h>
#include <genClass/DO_Real.h>

#include <genPFO/PFO_ColorDataLegend.h>


PFO_ColorDataLegend :: PFO_ColorDataLegend(PlotDefC& assPlt)
    : AnnoObjC("Labelled Colors", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_ColorDataLegend::PFO_ColorDataLegend(const PFO_ColorDataLegend& a, PlotDefC& assPlt)
    : AnnoObjC(a, assPlt), PSC_ColorLegendSpec(a)
{
    LocalCopy(a);
    InitCommon();
}

PFO_ColorDataLegend :: ~PFO_ColorDataLegend ()
{
}

void PFO_ColorDataLegend::InitCommon()
{
    PSC_ColorLegendSpec::InitCommon(*this);
    AddInPort(labelArrayObjRef,      typeid(DO_LabelArray));
    AddUseFlagInPort(selectionObjRef, typeid(DO_Real), frameSelection);

    DoStatusChk();
}

PFO_ColorDataLegend& PFO_ColorDataLegend::operator= (const PFO_ColorDataLegend& a)
{
    if (&a != this)
    {
        AnnoObjC::operator =(a);
        PSC_ColorLegendSpec::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_ColorDataLegend::LocalCopy(const PFO_ColorDataLegend& a)
{
    frameSelection = a.frameSelection;
    selectionObjRef = a.selectionObjRef;

    labelArrayObjRef = a.labelArrayObjRef;
    reductionFactor  = a.reductionFactor;
}

void PFO_ColorDataLegend::SetDefault()
{
    AnnoObjC::SetDefault();
    PSC_ColorLegendSpec::SetDefault();
    reductionFactor = 1.0;
    frameSelection = false;
}


void  PFO_ColorDataLegend:: DoStatusChk()
{
    selectedIndex = 0;
    labelArrayDO = 0;
    AnnoObjC::DoStatusChk();
    if (StatusNotOK())
        return;
    PSC_ColorLegendSpec::DoStatusChk(*this);
    if (StatusNotOK())
        return;

    if (CheckInputFO(labelArrayObjRef, "legend labels"))
    {
        labelArrayDO = static_cast<DO_LabelArray*>(GetInPortData(labelArrayObjRef));

        SC_CoordArray& inCoords = labelArrayDO->dataCoords;

        SC_DoubleArray inX(labelArrayDO->Size());
        for ( int i = 0; i < labelArrayDO->Size(); i++)
            inX[i] = inCoords[i].cX;

        if (!colorLimits->MapDataToRange(inX, labelColorMap, true, colorMap->ncolorMap))
            SetObjErrMsg("no label X values in color limit range");

        if (frameSelection && CheckInputFO(selectionObjRef, "selection value"))
        {
            double selectedVal = static_cast<DO_Real*>(GetInPortData(selectionObjRef))->realVal;
            for (int i = 0; i < inX.Size(); i++)
                if (RealsAreSame(selectedVal, inX[i]))
                    selectedIndex = i;
        }
    }

}

