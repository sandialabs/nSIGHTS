///////////////////////////////////////////////////////////////////////////////////
//
// PFO_CovarLimMatrix.cpp
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

#include <osPFO/PFO_CovarLimMatrix.h>


PFO_CovarLimMatrix :: PFO_CovarLimMatrix(PlotDefC& assPlt)
    : PFO_ColorBase("Covar Lim Matrix", assPlt)
{
    cLimits         = DC_Covar::cl_954;

    matrixPlotOption = mpoFull;

    plotEllipse = true;
    plotBestEst = false;
    plotBadCov = true;
    plotAreaLim = false;

    covarEllipseType = cetLine;
    covarLineThk    = 1;
    ndualPts        = 500;

    centerEllipse = false;

    bestEstSymbol.symbolSize = 8;

    plotWithColorMap = false;
    covarPen        = 3;
    colorAttribute  = caIndx;

    plotAreaLimPen = 6;
    plotAreaLimThk = 1;

    badCovXPen = 6;
    badCovXThk = 1;

    areaPct = 100.0;

    InitCommon();
}

PFO_CovarLimMatrix::PFO_CovarLimMatrix(const PFO_CovarLimMatrix& a, PlotDefC& assPlt)
: PFO_ColorBase(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_CovarLimMatrix :: ~PFO_CovarLimMatrix ()
{
}

void PFO_CovarLimMatrix::InitCommon()
{
    reportOK        = false;                // no reporting
    usePolyOffset   = is3DObject;

    AddInPort(inputDataObjRef,  typeid(DO_CovarArray));
    //AddUseFlagInPort(colorMapDataObjRef,  typeid(DO_ColorMap), plotWithColorMap);

    AddOutPort(seriesLegendData);

    DoStatusChk();
}

PFO_CovarLimMatrix& PFO_CovarLimMatrix::operator= (const PFO_CovarLimMatrix& a)
{
    if (&a != this)
    {
        PFO_ColorBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void  PFO_CovarLimMatrix::FullCopy(const PFO_CovarLimMatrix& a)
{
    PFO_ColorBase::FullCopy(a);
    LocalCopy(a);
}

void  PFO_CovarLimMatrix::LocalCopy(const PFO_CovarLimMatrix& a)
{
    //covarArrayObjRef  = a.covarArrayObjRef;
    cLimits             = a.cLimits         ;

    matrixPlotOption    = a.matrixPlotOption;

    plotEllipse         = a.plotEllipse     ;
    plotBestEst         = a.plotBestEst     ;
    plotBadCov          = a.plotBadCov      ;
    plotAreaLim         = a.plotAreaLim     ;

    covarEllipseType    = a.covarEllipseType;
    ndualPts            = a.ndualPts        ;
    covarLineThk        = a.covarLineThk    ;

    centerEllipse       = a.centerEllipse   ;

    bestEstSymbol       = a.bestEstSymbol   ;

    plotWithColorMap    = a.plotWithColorMap;
    covarPen            = a.covarPen        ;
    //colorMapDataObjRef    = a.colorMapDataObjRef;
    colorAttribute      = a.colorAttribute  ;

    plotAreaLimThk      = a.plotAreaLimThk  ;
    plotAreaLimPen      = a.plotAreaLimPen  ;

    badCovXPen          = a.badCovXPen      ;
    badCovXThk          = a.badCovXThk      ;

    areaPct             = a.areaPct         ;
}



void  PFO_CovarLimMatrix:: DoStatusChk()
{
    // base class check
    PFO_ColorBase::DoStatusChk();

    if (!CheckInputFO(inputDataObjRef, "Covariance Data"))
        return;
    DO_CovarArray* objData = static_cast<DO_CovarArray*>(GetInPortData(inputDataObjRef));
    covarArrayDC  = objData->covarArrayData;
    covarHeaderDC = objData->covarHeader;
    maxCovar = covarArrayDC->Size();

    if (plotWithColorMap)
    {
        if (!ColorAttributeIsDiag())
        {
            colorData.AllocAndSetSize(maxCovar);
            for (int i = 0; i < maxCovar; i++)
            {
                if (colorAttribute == caIndx)
                    colorData[i] = double(i);
                else if (colorAttribute == caSSE)
                    colorData[i] = (*covarArrayDC)[i].sumSquaredError;
            }

            if (limitIsMaster && autoLimits)
                rangeLimit.InitLimit(colorData);

            CheckRangeLimits();
        }

        if (colorAttribute == caIndx)
            colorLimitDO.SetValueLabel("Index");
        else if (colorAttribute == caSSE)
            colorLimitDO.SetValueLabel("SSE");
        else if (colorAttribute == caMainDiag)
            colorLimitDO.SetValueLabel("Main Diagonal Length");
        else if (colorAttribute == caOffDiag)
            colorLimitDO.SetValueLabel("Off Diagonal Length");
    }

    bestEstSymbol.symbolPen = covarPen;

    int num = 1;
    if (plotEllipse && plotBestEst)
        num = 2;
    seriesLegendData.Alloc(num);

    if (plotEllipse || plotBestEst)
    {
        PSC_SeriesSpec temp;
        temp.seriesPen = covarPen;

        //ellipse line legend
        if (plotEllipse)
        {
            temp.seriesPlotType = psl_LineOnly;
            temp.lineThickness = covarLineThk;

            static const char*  defStr[] = {"99.0% confidence limits",
                                            "95.4% confidence limits",
                                            "90.0% confidence limits",
                                            "68.3% confidence limits"};

            seriesLegendData.AddSeries(legendOverride.GetLabel(defStr[int(cLimits)]), temp);
        }

        if (plotBestEst)
        {
            //best estimate legend
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

void PFO_CovarLimMatrix:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
       return;

    //each covar matrix should always have same number of parameters
    maxParam = (*covarArrayDC)[0].covarFitVals.Size();

    if (plotWithColorMap && ColorAttributeIsDiag())
    {
        colorData.Alloc(maxCovar * maxParam * maxParam);
        colorIndx = 0;
        colorIndxMap.CubeAlloc(maxParam, maxParam, maxCovar);
    }


    percent = areaPct / 100.0;
    minPercent = (1.0 - percent) / 2.0;

    minDiag = 1E+10;
    maxDiag = 0.0;

    double aDiag, bDiag;

    minVal = 0.0;
    maxVal = 1.0;

    double minSingle, maxSingle;

    centerEllX.CubeAlloc(maxParam, maxParam, maxCovar);
    centerEllY.CubeAlloc(maxParam, maxParam, maxCovar);
    for (int i = 0; i < maxParam; i++)
    {
        centerEllX[i].MatrixFill(0.0);
        centerEllY[i].MatrixFill(0.0);
    }

    double minShift = 0.0;
    double maxShift = 0.0;

    SC_PointArray ell;

    for (int i = 0; i < maxParam; i++)
    {
        for (int j = i; j < maxParam; j++)
        {
            for (int k = 0; k < covarArrayDC->Size(); k++)
            {
                if (plotWithColorMap && ColorAttributeIsDiag())
                    colorIndxMap[i][j][k] = 0;
                if ((i == j) && Get2DSingle(i, k, minSingle, maxSingle))
                {
                    //find total min and max for complete data set
                    minVal = DMin(minSingle, minVal);
                    maxVal = DMax(maxSingle, maxVal);

                    aDiag = (maxSingle - minSingle) / 2.0;
                    bDiag = aDiag;

                    MinMaxDiag(aDiag, bDiag, i, j, k);
                }

                if ((i != j) && Get2DDual(i, j, k, ell, aDiag, bDiag))
                {
                    MinMaxDiag(aDiag, bDiag, i, j, k);

                    //find min max ellipse points
                    // to calc center pt for shift
                    // and if any outside 0-1, renormalize

                    double minX = 1E+10;
                    double maxX = -1E+10;
                    double minY = 1E+10;
                    double maxY = -1E+10;

                    for (int ii = 0; ii < ell.Size(); ii++)
                    {
                        if (ell[ii].pX < minX)
                            minX = ell[ii].pX;
                        if (ell[ii].pY < minY)
                            minY = ell[ii].pY;

                        if (ell[ii].pX > maxX)
                            maxX = ell[ii].pX;
                        if (ell[ii].pY > maxY)
                            maxY = ell[ii].pY;
                    }

                    if (centerEllipse)
                    {
                        centerEllX[i][j][k] = minX + ((maxX - minX) / 2.0);
                        centerEllY[i][j][k] = minY + ((maxY - minY) / 2.0);
                        minShift = DMin(minShift,
                                        (0.5 - centerEllX[i][j][k]),
                                        (0.5 - centerEllY[i][j][k]));
                        maxShift = DMax(maxShift,
                                        (0.5 - centerEllX[i][j][k]),
                                        (0.5 - centerEllY[i][j][k]));
                    }

                    //find total min and max for complete data set
                    minVal = DMin(minX, minY, minVal);
                    maxVal = DMax(maxX, maxY, maxVal);
                }//if get dual
            }//k covar
        }//j param
    }//i param

    if (centerEllipse)
    {
        minVal += minShift;
        maxVal += maxShift;
    }

    if (plotWithColorMap)
    {
        if (ColorAttributeIsDiag())
        {
            colorData.SetSize(colorIndx);

            if (limitIsMaster && autoLimits)
                rangeLimit.InitLimit(colorData);

            CheckRangeLimits();
        }
    }

}

void PFO_CovarLimMatrix::MinMaxDiag(const double aDiag, const double bDiag,
                                    const int i, const int j, const int k)
{
    if (ColorAttributeIsDiag())
    {
        double tempDiag;
        if (colorAttribute == caMainDiag)
            tempDiag = aDiag;
        else if (colorAttribute == caOffDiag)
            tempDiag = bDiag;

        //set plot colors if necessary
        if (plotWithColorMap)
        {
            colorIndxMap[i][j][k] = colorIndx;
            colorData[colorIndx++] = tempDiag;
        }

        //find min max diagonal
        if (tempDiag < minDiag)
            minDiag = tempDiag;
        if (tempDiag > maxDiag)
            maxDiag = tempDiag;
    }
}

bool PFO_CovarLimMatrix::Get2DSingle(int xIndx, int covarIndx, double& minimum, double& maximum)
{
    DC_Covar& curr = (*covarArrayDC)[covarIndx];
    if (!curr.CovarOK(xIndx))
        return false;

    curr.CalcSingleLimits(cLimits, xIndx, minimum, maximum);

    return true;

}

bool PFO_CovarLimMatrix::Get2DDual(int xIndx,
                                   int yIndx,
                                   int covarIndx,
                                   SC_PointArray& ellipse,
                                   double& aDiag,
                                   double& bDiag)
{
    DC_Covar& curr = (*covarArrayDC)[covarIndx];
    if ((!curr.CovarOK(xIndx)) || (!curr.CovarOK(yIndx)))
        return false;

    try
    {
        curr.CalcDualLimits(cLimits, xIndx, yIndx, ndualPts, ellipse, aDiag, bDiag);
    }
    catch (MathError&)
    {
        return false;
    }

    if (aDiag < bDiag)
        Swap(aDiag, bDiag);

    return true;
}

double PFO_CovarLimMatrix::Normalize(const double userVal)
{
    double norm = (userVal - minVal) / (maxVal - minVal);
    if (percent < 1.0)
        return Reduce(norm);
    else
        return norm;
}

double PFO_CovarLimMatrix::Reduce(const double userVal)
{
    return (userVal * percent) + minPercent;
}

double PFO_CovarLimMatrix::Shift(const int i, const int j, const int k, bool isX)
{
    if (!centerEllipse)
        return 0.0;

    if (isX)
        return (0.5 - Normalize(centerEllX[i][j][k]));
    else
        return (0.5 - Normalize(centerEllY[i][j][k]));
}

Limit3D PFO_CovarLimMatrix::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    SC_PointArray pts(2);

    for (int i = 0; i < maxParam; i++)
    {
        for (int j = i; j < maxParam; j++)
        {
            pts[0].pX = i - 0.5;
            pts[0].pY = j - 0.5;
            pts[1].pX = pts[0].pY;
            pts[1].pY = pts[0].pX;
        }
    }
    currLimits.AddToLimits(pts);

    return currLimits;
}

