///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ShadeRegion.cpp
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

#include <genPlotClass/C_PlotDef.h>

#include <genPFO/PFO_ShadeRegion.h>


PFO_ShadeRegion :: PFO_ShadeRegion(PlotDefC& assPlt)
    : PlotObjC("Shade Region", assPlt),
      regionXMin("Region X Min"),
      regionXMax("Region X Max"),
      regionYMin("Region Y Min"),
      regionYMax("Region Y Max")
{

    xRegionSpec = rst_betweenMinAndMax;
    yRegionSpec = rst_None;
    shadeRegion         = true;

    labelRegion         = false;
    labelXPosition      = 50.0;
    labelYPosition      = 50.0;
    labelOffset         = 0;
    halign              = hta_Left;
    valign              = vta_Center;

    shadeIsColorMap = true;
    shadeCMIndex  = 0;
    shadePen      = 6;
    labelPen     = 6;
    SetToNull(regionLabel);

    stippleRegion = false;

    frameRegion = false;

    InitCommon();
}

PFO_ShadeRegion::PFO_ShadeRegion(const PFO_ShadeRegion& a, PlotDefC& assPlt) :
    PlotObjC(a, assPlt),
    regionXMin(a.regionXMin), regionXMax(a.regionXMax),
    regionYMin(a.regionYMin), regionYMax(a.regionYMax)
{
    LocalCopy(a);
    InitCommon();
}

PFO_ShadeRegion :: ~PFO_ShadeRegion ()
{
}

void PFO_ShadeRegion::InitCommon()
{
    reportOK        = false;                // no reporting
    usePolyOffset   = true;

    AddUseFlagInPort(regionXMin.valueMasterObjRef, typeid(DO_Real), checkXMin);
    AddUseFlagInPort(regionXMax.valueMasterObjRef, typeid(DO_Real), checkXMax);
    AddUseFlagInPort(regionYMin.valueMasterObjRef, typeid(DO_Real), checkYMin);
    AddUseFlagInPort(regionYMax.valueMasterObjRef, typeid(DO_Real), checkYMax);

    AddUseFlagInPort(colorMapObjRef,   typeid(DO_ColorMap), shadeIsColorMap);

    DoStatusChk();
}


PFO_ShadeRegion& PFO_ShadeRegion::operator= (const PFO_ShadeRegion& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_ShadeRegion::LocalCopy(const PFO_ShadeRegion& a)
{
    xRegionSpec =   a.xRegionSpec;
    yRegionSpec =   a.yRegionSpec;
    regionXMin =    a.regionXMin;
    regionXMax =    a.regionXMax;
    regionYMin =    a.regionYMin;
    regionYMax =    a.regionYMax;

    shadeRegion = a.shadeRegion;

    shadeIsColorMap = a.shadeIsColorMap;
    colorMapObjRef = a.colorMapObjRef;       // color map
    shadeCMIndex   = a.shadeCMIndex;
    shadePen = a.shadePen;

    labelRegion = a.labelRegion;
    labelPen    = a.labelPen;
    labelXPosition = a.labelXPosition;  // 0 to 100 along X limits
    labelYPosition = a.labelYPosition;  // 0 to 100 along Ylimits
    labelOffset = a.labelOffset;    // pixels from position
    halign = a.halign;
    valign = a.valign;
    labelFont = a.labelFont;

    CopyString(regionLabel, a.regionLabel, maxLabelLen);

    stippleRegion = a.stippleRegion;
    regionStipple = a.regionStipple;

    frameRegion = a.frameRegion;
}


void  PFO_ShadeRegion:: DoStatusChk()
{
    needXMin = ((xRegionSpec == rst_lessThanMin) || (xRegionSpec == rst_betweenMinAndMax));
    needXMax = (xRegionSpec == rst_greaterThanMax) || (xRegionSpec == rst_betweenMinAndMax);
    needYMin = (yRegionSpec == rst_lessThanMin) || (yRegionSpec == rst_betweenMinAndMax);
    needYMax = (yRegionSpec == rst_greaterThanMax) || (yRegionSpec == rst_betweenMinAndMax);

    checkXMin = needXMin && (!regionXMin.valueIsMaster);
    checkXMax = needXMax && (!regionXMax.valueIsMaster);
    checkYMin = needYMin && (!regionYMin.valueIsMaster);
    checkYMax = needYMax && (!regionYMax.valueIsMaster);

    // base class check
    PlotObjC::DoStatusChk();

    if ((xRegionSpec == rst_None) && (yRegionSpec == rst_None))
        SetObjErrMsg("X and/or Y region must be shaded");

    if (needXMin)
        regionXMin.DoCheck("Region X min", *this);
    if (needXMax)
        regionXMax.DoCheck("Region X max", *this);
    if (needYMin)
        regionYMin.DoCheck("Region Y min", *this);
    if (needYMax)
        regionYMax.DoCheck("Region Y max", *this);

    if (!(shadeRegion || labelRegion || stippleRegion || frameRegion))
        SetObjErrMsg("no shading, labeling, stippling, or frame?");


    if (shadeRegion && shadeIsColorMap && CheckInputFO(colorMapObjRef, "Color map"))
        colorMap = (static_cast<const DO_ColorMap*>(GetInPortData(colorMapObjRef)))->colorMapData;

   if (StatusNotOK())
        return;

    if (needXMin)
        regionXMinVal = regionXMin.GetCurrentValue();
    if (needXMax)
        regionXMaxVal = regionXMax.GetCurrentValue();
    if (needYMin)
        regionYMinVal = regionYMin.GetCurrentValue();
    if (needYMax)
        regionYMaxVal = regionYMax.GetCurrentValue();

}


void PFO_ShadeRegion:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

Limit3D PFO_ShadeRegion::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (! doPlot))
        return currLimits;

    if (needXMin || needXMax)
    {
        if (needXMin && needXMax)
        {
            currLimits.minLim.cX = regionXMinVal;
            currLimits.maxLim.cX = regionXMaxVal;
        }
        else if (needXMin)
        {
            currLimits.minLim.cX = regionXMinVal;
            currLimits.maxLim.cX = regionXMinVal;
        }
        else
        {
            currLimits.minLim.cX = regionXMaxVal;
            currLimits.maxLim.cX = regionXMaxVal;
        }
    }

    if (needYMin || needYMax)
    {
        if (needYMin && needYMax)
        {
            currLimits.minLim.cY = regionYMinVal;
            currLimits.maxLim.cY = regionYMaxVal;
        }
        else if (needYMin)
        {
            currLimits.minLim.cY = regionYMinVal;
            currLimits.maxLim.cY = regionYMinVal;
        }
        else
        {
            currLimits.minLim.cY = regionYMaxVal;
            currLimits.maxLim.cY = regionYMaxVal;
        }
    }


    return currLimits;

}


