///////////////////////////////////////////////////////////////////////////////////
//
// PSC_ColorLegendSpec.cpp
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
//      Platform independent base classes for plot definitions and plot functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genClass/DO_ColorLimit.h>

#include <genPlotClass/PSC_ColorLegendSpec.h>


PSC_ColorLegendSpec :: PSC_ColorLegendSpec()
{
    SetDefault();
}

PSC_ColorLegendSpec::PSC_ColorLegendSpec(const PSC_ColorLegendSpec& a)
{
    LocalCopy(a);
}

PSC_ColorLegendSpec :: ~PSC_ColorLegendSpec ()
{
}

void PSC_ColorLegendSpec::InitCommon(FuncObjC& inFO)
{
    inFO.AddInPort(colorLegendObjRef,      typeid(DO_ColorLimit));

    colorMap        = 0;
    colorLimits     = 0;
    clipToRange     = true;
}

PSC_ColorLegendSpec& PSC_ColorLegendSpec::operator= (const PSC_ColorLegendSpec& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  PSC_ColorLegendSpec::LocalCopy(const PSC_ColorLegendSpec& a)
{
    colorLegendObjRef = a.colorLegendObjRef;

    orientation         = a.orientation   ;
    dataLabelAboveLeft  = a.dataLabelAboveLeft;
    incLabelAboveLeft   = a.incLabelAboveLeft;
    overrideLabel       = a.overrideLabel;
    CopyString(newLabel, a.newLabel, newLabelLen);

    barLength       = a.barLength     ;
    barAspect       = a.barAspect     ;
    ticsAtIncs      = a.ticsAtIncs    ;
    dataTypeLabel   = a.dataTypeLabel ;
}

void PSC_ColorLegendSpec::SetDefault()
{
    orientation         = PSC_Font::fr_Horiz;
    dataLabelAboveLeft  = true;
    incLabelAboveLeft   = true;
    overrideLabel       = false;
    SetToNull(newLabel);

    barLength       = 25.0;          //  wrt to 0-100 space
    barAspect       = 20.0;          //  width = length / aspect
    ticsAtIncs      = true;
    dataTypeLabel   = true;
}


void  PSC_ColorLegendSpec:: DoStatusChk(AnnoObjC& inFO)
{
    colorMap        = 0;           // color map
    colorLimits     = 0;

    if (!inFO.CheckInputFO(colorLegendObjRef, "ColorLegend"))
        return;

    DO_ColorLimit* tmpDO = static_cast<DO_ColorLimit*>(inFO.GetInPortData(colorLegendObjRef));
    colorMap      = tmpDO->colorMapData;
    colorLimits   = tmpDO->limitData;
    clipToRange   = tmpDO->clipToRange;

    if (fabs(colorLimits->maxLimit - colorLimits->minLimit) < stdEps)
        inFO.SetObjErrMsg("color min and max are the same");

    if (overrideLabel)
        CopyString(inFO.legendTitle, newLabel, PSC_AnnoSpec::legendTitleLen);
    else
        CopyString(inFO.legendTitle, tmpDO->dataValueLabel, PSC_AnnoSpec::legendTitleLen);

}

