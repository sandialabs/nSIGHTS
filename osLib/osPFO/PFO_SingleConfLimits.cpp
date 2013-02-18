///////////////////////////////////////////////////////////////////////////////////
//
// PFO_SingleConfLimits.cpp
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

#include <osClass/DO_CovarArray.h>
#include <genClass/DO_ColorMap.h>

#include <genPlotClass/C_PlotDef.h>
#include <genPlotClass/PSC_SeriesSpec.h>

#include <osPFO/PFO_SingleConfLimits.h>


PFO_SingleConfLimits :: PFO_SingleConfLimits(PlotDefC& assPlt)
    : PFO_ColorBase("Single Conf Limits", assPlt),
      xDataIndx(0)
{
    allParameters   = true;
    cLimits         = DC_Covar::cl_954;

    plotWithColorMap=false;
    covarPen        = 3;

    plotConfLimits  = true;
    covarLineThk    = 1;

    plotBestEstimate = true;
    bestEstSymbol.symbolSize = 8;

    widthSpec       = wsLine;
    widthSpecValue  = 1.0;
    widthSpecPixel  = 6;
    widthSpecPct    = 50.0;

    colorMapIsIndex = true;

    InitCommon();
}

PFO_SingleConfLimits::PFO_SingleConfLimits(const PFO_SingleConfLimits& a, PlotDefC& assPlt)
: PFO_ColorBase(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_SingleConfLimits :: ~PFO_SingleConfLimits ()
{
}

void PFO_SingleConfLimits::InitCommon()
{
    reportOK        = true;
    usePolyOffset   = is3DObject;

    //  base class
    AddInPort(inputDataObjRef,  typeid(DO_CovarArray));
    //AddUseFlagInPort(colorMapDataObjRef,  typeid(DO_ColorMap), plotWithColorMap);

    AddOutPort(seriesLegendData);

    AddOutPort(covarXYArrayDO);
    covarXYArrayDO.xyDataArray = &covarXYArrayDC;
    covarXYArrayDC.SetID("CovarPts");

    DoStatusChk();
}

PFO_SingleConfLimits& PFO_SingleConfLimits::operator= (const PFO_SingleConfLimits& a)
{
    if (&a != this)
    {
        PFO_ColorBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void  PFO_SingleConfLimits::FullCopy(const PFO_SingleConfLimits& a)
{
    PFO_ColorBase::FullCopy(a);
    LocalCopy(a);
}

void  PFO_SingleConfLimits::LocalCopy(const PFO_SingleConfLimits& a)
{
    //covarArrayObjRef  = a.covarArrayObjRef;

    xDataIndx           = a.xDataIndx       ;

    allParameters       = a.allParameters   ;
    cLimits             = a.cLimits         ;

    covarPen            = a.covarPen        ;
    plotWithColorMap    = a.plotWithColorMap;
    //colorMapDataObjRef    = a.colorMapDataObjRef;

    plotConfLimits      = a.plotConfLimits  ;
    covarLineThk        = a.covarLineThk    ;

    plotBestEstimate    = a.plotBestEstimate;
    bestEstSymbol       = a.bestEstSymbol   ;

    widthSpec           = a.widthSpec       ;
    widthSpecValue      = a.widthSpecValue  ;
    widthSpecPixel      = a.widthSpecPixel  ;
    widthSpecPct        = a.widthSpecPct    ;

    colorMapIsIndex     = a.colorMapIsIndex ;
}



void  PFO_SingleConfLimits:: DoStatusChk()
{
    // base class check
    PFO_ColorBase::DoStatusChk();

    if (!CheckInputFO(inputDataObjRef, "Covariance Data"))
        return;
    DO_CovarArray* objData = static_cast<DO_CovarArray*>(GetInPortData(inputDataObjRef));
    covarArrayDC  = objData->covarArrayData;
    covarHeaderDC = objData->covarHeader;
    maxCovar = covarArrayDC->Size();

    //  check parameters
    int maxIndx = covarHeaderDC->Size();
    if (!allParameters)
    {
        bool indexBad = (xDataIndx >= maxIndx);

        if (indexBad)
            SetObjErrMsg("selected vars must be different");
    }

    if (plotWithColorMap)
    {
        //if (!CheckInputFO(colorMapDataObjRef, "Color map"))
        //  return;
        //colorMap = static_cast<DO_ColorMap*>(GetInPortData(colorMapDataObjRef))->colorMapData;

        colorData.AllocAndSetSize(maxCovar);
        for (int i = 0; i < maxCovar; i++)
        {
            if (colorMapIsIndex)
                colorData[i] = double(i);
            else
                colorData[i] = (*covarArrayDC)[i].sumSquaredError;
        }

        if (limitIsMaster && autoLimits)
            rangeLimit.InitLimit(colorData);

        CheckRangeLimits();

        if (colorMapIsIndex)
            colorLimitDO.SetValueLabel("Index");
        else
            colorLimitDO.SetValueLabel("SSE");

    }

    bestEstSymbol.symbolPen = covarPen;

    int num = 1;
    if (plotConfLimits && plotBestEstimate)
        num = 2;
    seriesLegendData.Alloc(num);

    if (plotConfLimits || plotBestEstimate)
    {
        PSC_SeriesSpec temp;
        temp.seriesPen = covarPen;

        //conf limits line legend
        if (plotConfLimits)
        {
            temp.seriesPlotType = psl_LineOnly;
            temp.lineThickness = covarLineThk;

            static const char*  limStr[] = {"99.0% confidence limits",
                                            "95.4% confidence limits",
                                            "90.0% confidence limits",
                                            "68.3% confidence limits"};

            seriesLegendData.AddSeries(legendOverride.GetLabel(limStr[int(cLimits)]), temp);
        }

        //best estimate legend
        if (plotBestEstimate)
        {
            temp.seriesPlotType = psl_SymbolOnly;
            temp.symbolType = bestEstSymbol.symbolType;
            temp.symbolSize = bestEstSymbol.symbolSize;

            static const char*  symStr = "best estimate";

            seriesLegendData.AddSeries(legendOverride.GetLabel(symStr), temp);
        }
    }
    else
        seriesLegendData.Clear();
}


void PFO_SingleConfLimits:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
       return;


    maxParam = 1;
    if (allParameters)
        //each covar matrix should always have same number of parameters
        maxParam = (*covarArrayDC)[0].covarFitVals.Size();

    covarXYArrayDC.AllocAndSetSize(maxParam);
    SC_PointArray val(2);
    int nextOK = 0;

    for (int i = 0; i < maxParam; i++)
    {
        DC_XYData& nextXY = covarXYArrayDC[i];
        nextXY.Alloc(2 * maxCovar);
        nextXY.SetID((*covarHeaderDC)[i].shortVarDesc);

        int indx = i;
        if (!allParameters)
            indx = xDataIndx;

        for (int j = 0; j < maxCovar; j++)
        {
            if (Get2DSingle(j, indx, val))
            {
                nextXY.xData += val[0].pX;
                nextXY.xData += val[1].pX;
                nextXY.yData += val[0].pY;
                nextXY.yData += val[1].pY;

                nextOK++;
            }
        }
    }

    if (nextOK > 0)
        return;

    SetObjErrMsg("covariance matrix problems -- cannot calculate limits");
}

bool PFO_SingleConfLimits::Get2DSingle(int covarIndx,
                                       int paramIndx,
                                       SC_PointArray& val)
{
    DC_Covar& curr = (*covarArrayDC)[covarIndx];
    val[0].pX = paramIndx;
    val[1].pX = paramIndx;

    if (curr.CovarOK(paramIndx))
    {
        curr.CalcSingleLimits(cLimits, paramIndx, val[0].pY, val[1].pY);
    }
    else
    {
        val[0].pY = 0.0;
        val[1].pY = 1.0;
    }
    return true;
}

Limit3D PFO_SingleConfLimits::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    for (int i = 0; i < maxParam; i++)
    {
        DC_XYData& currXY = covarXYArrayDC[i];
        for (int j = 0; j < currXY.Size(); j++)
            currLimits.AddToLimits(Point2D(currXY.xData[j], currXY.yData[j]));

    }

    return currLimits;
}

