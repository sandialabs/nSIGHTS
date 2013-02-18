///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ColorLegend.cpp
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
//      legend bar for objects plotted according to a color map. 
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genPlotClass/C_PlotDef.h>

#include <genClass/DO_ColorLimit.h>

#include <genPFO/PFO_ColorLegend.h>


PFO_ColorLegend :: PFO_ColorLegend(PlotDefC& assPlt)
    : AnnoObjC("Color Bar", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_ColorLegend::PFO_ColorLegend(const PFO_ColorLegend& a, PlotDefC& assPlt)
    : AnnoObjC(a, assPlt), PSC_ColorLegendSpec(a)
{
    LocalCopy(a);
    InitCommon();
}

PFO_ColorLegend :: ~PFO_ColorLegend ()
{
}

void PFO_ColorLegend::InitCommon()
{
    PSC_ColorLegendSpec::InitCommon(*this);
    AddInPort(labelArrayObjRef,      typeid(DO_LabelArray));

    DoStatusChk();
}

PFO_ColorLegend& PFO_ColorLegend::operator= (const PFO_ColorLegend& a)
{
    if (&a != this)
    {
        AnnoObjC::operator =(a);
        PSC_ColorLegendSpec::operator =(a);
        LocalCopy(a);
    }
    return *this;
}



void  PFO_ColorLegend::LocalCopy(const PFO_ColorLegend& a)
{
    autoIncs            = a.autoIncs;
    includeSE           = a.includeSE;
    nmanualIncs         = a.nmanualIncs;
    legendFormat        = a.legendFormat;
    specIncs            = a.specIncs;
    useLabelX           = a.useLabelX;
    labelArrayObjRef    = a.labelArrayObjRef;
}

void PFO_ColorLegend::SetDefault()
{
    AnnoObjC::SetDefault();
    PSC_ColorLegendSpec::SetDefault();
    autoIncs        = true;          //  auto increment
    nmanualIncs     = 3;             //  # of manual
    legendFormat    = SC_RealFormat();
    specIncs        = false;            // specified increments
    useLabelX       = false;
    includeSE       = false;
}


void  PFO_ColorLegend:: DoStatusChk()
{
    labelArrayDO = 0;
    AnnoObjC::DoStatusChk();
    if (StatusNotOK())
        return;
    PSC_ColorLegendSpec::DoStatusChk(*this);
    if (StatusNotOK())
        return;

    if (specIncs)
    {
        if (CheckInputFO(labelArrayObjRef, "legend labels"))
            labelArrayDO = static_cast<DO_LabelArray*>(GetInPortData(labelArrayObjRef));
    }
}

