///////////////////////////////////////////////////////////////////////////////////
//
// PSC_ColorBase.cpp
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
//      maps data to color ranges 
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/C_Graphic.h>

#include <genClass/U_Real.h>

#include <genClass/DO_ColorMap.h>

#include <genPlotClass/PSC_ColorBase.h>


PSC_ColorBase :: PSC_ColorBase()
{
    limitIsMaster       = true;
    autoLimits          = true;
    baseAutoDataSource  = 0;
    clipToRange         = true;
}

PSC_ColorBase::PSC_ColorBase(const PSC_ColorBase& a)
{
    LocalCopy(a);
}

PSC_ColorBase :: ~PSC_ColorBase ()
{
}

void PSC_ColorBase::InitCommon(FuncObjC& inFO)
{
    inFO.AddInPort(colorMapObjRef,         typeid(DO_ColorMap));
    inFO.AddMSInPort(limitMasterObjRef,    typeid(DO_ColorLimit), limitIsMaster);

    inFO.AddOutPort(colorLimitDO);

    colorLimitDO.limitData = &rangeLimit;
}

PSC_ColorBase& PSC_ColorBase::operator= (const PSC_ColorBase& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  PSC_ColorBase::LocalCopy(const PSC_ColorBase& a)
{
    inputDataObjRef     = a.inputDataObjRef;
    colorMapObjRef      = a.colorMapObjRef;
    limitMasterObjRef   = a.limitMasterObjRef;

    limitIsMaster       = a.limitIsMaster;
    autoLimits          = a.autoLimits;
    baseAutoDataSource  = a.baseAutoDataSource;
    rangeLimit          = a.rangeLimit;
    clipToRange         = a.clipToRange;
}


void PSC_ColorBase::DoStatusChk(FuncObjC& inFO)
{
    colorMap      = 0;
    masterLimits  = 0;
    oneColor      = false;

    if (!limitIsMaster)
    {
        if (!inFO.CheckInputFO(limitMasterObjRef,  "Limit master"))
            return;

        DO_ColorLimit* limitDO = static_cast<DO_ColorLimit*>(inFO.GetInPortData(limitMasterObjRef));
        masterLimits = limitDO->limitData;
        colorMap = limitDO->colorMapData;
        clipToRange = limitDO->clipToRange;
        rangeLimit = *masterLimits;
    }
    else
    {
        if (!inFO.CheckInputFO(colorMapObjRef, "Color map"))
            return;

        colorMap = (static_cast<DO_ColorMap*>(inFO.GetInPortData(colorMapObjRef)))->colorMapData;
    }

    oneColor = colorMap->AllSameColor();

    colorLimitDO.colorMapData = colorMap;
    colorLimitDO.clipToRange = clipToRange;
}

void PSC_ColorBase::CheckRangeLimits(FuncObjC& inFO)
{
    if (limitIsMaster)
    {
        if (RealIsNull(rangeLimit.minLimit) || RealIsNull(rangeLimit.maxLimit))
        {
            inFO.SetObjErrMsg("min and/or max range is null ??");
            return;
        }

        if (rangeLimit.logRange && (rangeLimit.maxLimit <stdEps))
            inFO.SetObjErrMsg("log range with max < 0.0");

        if (rangeLimit.maxLimit < rangeLimit.minLimit)
            inFO.SetObjErrMsg("max range < min range");

        rangeLimit.ResetLogLimit();
    }
}

