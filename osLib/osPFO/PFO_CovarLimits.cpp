///////////////////////////////////////////////////////////////////////////////////
//
// PFO_CovarLimits.cpp
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
//      Plots confidence limits of 2D or 3D plots.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osClass/DO_CovarArray.h>

#include <genPlotClass/C_PlotDef.h>
#include <genPlotClass/PSC_SeriesSpec.h>

#include <osPFO/PFO_CovarLimits.h>


PFO_CovarLimits :: PFO_CovarLimits(PlotDefC& assPlt)
    : PlotObjC("Covar Limits", assPlt),
      xDataIndx(0),
      yDataIndx(1),
      zDataIndx(2)
{
    covarPlotType   = cptSingle;
    covarPlotFormat = cpfLine;
    cLimits         = DC_Covar::cl_954;

    covarPen        = 3;
    covarLineThk    = 1;

    tubeData.adjustForScaling = true;

    ndualPts        = 500;

    ntripleSlice    = 20;
    ntripleMeridian = 20;


    InitCommon();
}

PFO_CovarLimits::PFO_CovarLimits(const PFO_CovarLimits& a, PlotDefC& assPlt) : PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_CovarLimits :: ~PFO_CovarLimits ()
{
}

void PFO_CovarLimits::InitCommon()
{
    reportOK        = false;                // no reporting
    usePolyOffset   = is3DObject;

    //  base class
    AddInPort(covarArrayObjRef,  typeid(DO_CovarArray));
    AddOutPort(seriesLegendData);

    if (!is3DObject)
    {
        AddOutPort(covarXYDO);
        covarXYDO.xyData = &covarXYDC;
        covarXYDC.SetID("CovarPts");

        AddOutPort(covarXYArrayDO);
        covarXYArrayDO.xyDataArray = &covarXYArrayDC;
    }


    DoStatusChk();
}

PFO_CovarLimits& PFO_CovarLimits::operator= (const PFO_CovarLimits& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}



void  PFO_CovarLimits::LocalCopy(const PFO_CovarLimits& a)
{
    covarArrayObjRef   = a.covarArrayObjRef;

    xDataIndx       = a.xDataIndx       ;
    yDataIndx       = a.yDataIndx       ;
    zDataIndx       = a.zDataIndx       ;

    covarPlotType   = a.covarPlotType   ;
    covarPlotFormat = a.covarPlotFormat ;
    cLimits         = a.cLimits         ;

    covarPen        = a.covarPen        ;
    covarLineThk    = a.covarLineThk    ;
    tubeData        = a.tubeData        ;

    ndualPts        = a.ndualPts        ;
    ntripleSlice    = a.ntripleSlice    ;
    ntripleMeridian = a.ntripleMeridian ;
}



void  PFO_CovarLimits:: DoStatusChk()
{
    // base class check
    PlotObjC::DoStatusChk();

    if (!CheckInputFO(covarArrayObjRef, "Covariance Data"))
        return;
    DO_CovarArray* objData = static_cast<DO_CovarArray*>(GetInPortData(covarArrayObjRef));
    covarArrayDC  = objData->covarArrayData;
    covarHeaderDC = objData->covarHeader;

    //  check parameters
    int maxIndx = covarHeaderDC->Size();
    bool indexBad = (xDataIndx == yDataIndx) || (xDataIndx >= maxIndx) || (yDataIndx >= maxIndx);
    if (is3DObject)
        indexBad = indexBad || (xDataIndx == zDataIndx) || (zDataIndx == yDataIndx) || (zDataIndx >= maxIndx);

    if (indexBad)
        SetObjErrMsg("selected vars must be different");

    seriesLegendData.Alloc(1);

    PSC_SeriesSpec temp;
    temp.seriesPlotType = psl_LineOnly;
    temp.seriesPen = covarPen;
    temp.lineThickness = covarLineThk;
    if (covarPlotFormat != cpfLine)
        temp.lineThickness = 10;

    static const char*  defStr[] = {"99.0% confidence limits",
                                    "95.4% confidence limits",
                                    "90.0% confidence limits",
                                    "68.3% confidence limits"};

    seriesLegendData.AddSeries(legendOverride.GetLabel(defStr[int(cLimits)]), temp);
}


void PFO_CovarLimits:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
       return;


    // check for at least one valid
    if (is3DObject)
    {
        switch (covarPlotType)
        {
            case cptSingle : {
                SC_CoordArray st, end;
                for (int i = 0; i < covarArrayDC->Size(); i++)
                    if (Get3DSingle(i, st, end))
                        return;
                break;
            }
            case cptDual : {
                SC_CoordArray ell;
                for (int i = 0; i < covarArrayDC->Size(); i++)
                {
                    if (Get3DDual(i, p3D_XY, ell))
                        return;
                    if (Get3DDual(i, p3D_XZ, ell))
                        return;
                    if (Get3DDual(i, p3D_YZ, ell))
                        return;
                }
                break;
            }
            case cptTriple : {
                SC_CoordMatrix ell;
                for (int i = 0; i < covarArrayDC->Size(); i++)
                {
                    if (Get3DTriple(i, ell))
                        return;
                }
                break;
            }

        }
    }
    else
    {
        int maxCovar = covarArrayDC->Size();
        int nextOK = 0;
        covarXYDC.SetEmpty();
        covarXYArrayDC.Alloc(maxCovar);
        if (covarPlotType == cptSingle)
        {
            SC_PointArray st, end;
            for (int i = 0; i < maxCovar; i++)
                if (Get2DSingle(i, st, end))
                {
                    DC_XYData& nextXY = covarXYArrayDC[nextOK++];
                    covarXYArrayDC.SetSize(nextOK);

                    nextXY.Alloc(4);
                    nextXY.SetID((*covarArrayDC)[i].covarID);
                    nextXY.xData += st[0].pX;
                    nextXY.xData += st[1].pX;
                    nextXY.xData += end[0].pX;
                    nextXY.xData += end[1].pX;
                    nextXY.yData += st[0].pY;
                    nextXY.yData += st[1].pY;
                    nextXY.yData += end[0].pY;
                    nextXY.yData += end[1].pY;
                }
        }
        else
        {
            SC_PointArray ell;
            for (int i = 0; i < maxCovar; i++)
                if (Get2DDual(i, ell))
                {
                    DC_XYData& nextXY = covarXYArrayDC[nextOK++];
                    covarXYArrayDC.SetSize(nextOK);
                    nextXY.Alloc(ell.Size());
                    nextXY.SetID((*covarArrayDC)[i].covarID);
                    for (int j = 0; j < ell.Size(); j++)
                    {
                        nextXY.xData += ell[j].pX;
                        nextXY.yData += ell[j].pY;
                    }
                }
        }

        if (nextOK > 0)
        {
            covarXYDC = covarXYArrayDC[0];
            return;
        }
    }

    SetObjErrMsg("covariance matrix problems -- cannot calculate limits");
}

bool PFO_CovarLimits::Get2DSingle(int indx,
                                      SC_PointArray& st,
                                      SC_PointArray& end)
{
    DC_Covar& curr = (*covarArrayDC)[indx];
    if ((!curr.CovarOK(xDataIndx)) || (!curr.CovarOK(yDataIndx)))
        return false;

    curr.CalcSingleLimits(cLimits, xDataIndx, yDataIndx, st, end);

    covarHeaderDC->DeNormalize(st, xDataIndx, yDataIndx);
    covarHeaderDC->DeNormalize(end, xDataIndx, yDataIndx);

    return true;
}

bool PFO_CovarLimits::Get2DDual(int indx,
                                SC_PointArray& ellipse)
{
    DC_Covar& curr = (*covarArrayDC)[indx];
    if ((!curr.CovarOK(xDataIndx)) || (!curr.CovarOK(yDataIndx)))
        return false;

    try {
        curr.CalcDualLimits(cLimits, xDataIndx, yDataIndx, ndualPts, ellipse);
    }
    catch (MathError&)
    {
        return false;
    }

    covarHeaderDC->DeNormalize(ellipse, xDataIndx, yDataIndx);

    return true;
}


bool PFO_CovarLimits::Get3DSingle(int            indx,
                                  SC_CoordArray& st,
                                  SC_CoordArray& end)
{
    DC_Covar& curr = (*covarArrayDC)[indx];
    if ((!curr.CovarOK(xDataIndx)) || (!curr.CovarOK(yDataIndx)) || (!curr.CovarOK(zDataIndx)))
        return false;

    curr.CalcSingleLimits(cLimits, xDataIndx, yDataIndx, zDataIndx, st, end);

    covarHeaderDC->DeNormalize(st, xDataIndx, yDataIndx, zDataIndx);
    covarHeaderDC->DeNormalize(end, xDataIndx, yDataIndx, zDataIndx);

    return true;
}

bool  PFO_CovarLimits::Get3DDual(int            indx,
                                 Plane3D        dualPlane,
                                 SC_CoordArray& ell)
{
    DC_Covar& curr = (*covarArrayDC)[indx];
    if ((!curr.CovarOK(xDataIndx)) || (!curr.CovarOK(yDataIndx)) || (!curr.CovarOK(zDataIndx)))
        return false;

    try {
        switch (dualPlane)
        {
            case p3D_XY :  {
                curr.CalcDualLimits(cLimits, xDataIndx, yDataIndx, zDataIndx, ndualPts, ell);
                covarHeaderDC->DeNormalize(ell, xDataIndx, yDataIndx, zDataIndx);
                break;
            }
            case p3D_XZ :  {
                curr.CalcDualLimits(cLimits, xDataIndx, zDataIndx, yDataIndx, ndualPts, ell);
                covarHeaderDC->DeNormalize(ell, xDataIndx, zDataIndx, yDataIndx);
                break;
            }
            case p3D_YZ :  {
                curr.CalcDualLimits(cLimits, yDataIndx, zDataIndx, xDataIndx, ndualPts, ell);
                covarHeaderDC->DeNormalize(ell, yDataIndx, zDataIndx, xDataIndx);
                break;
            }
        }
    }
    catch (MathError&)
    {
        return false;
    }

    ell.FromModelPlane(dualPlane);
    return true;
}

bool PFO_CovarLimits::Get3DTriple(int indx,
                                  SC_CoordMatrix& ellipse)
{
    DC_Covar& curr = (*covarArrayDC)[indx];
    if ((!curr.CovarOK(xDataIndx)) || (!curr.CovarOK(yDataIndx)) || (!curr.CovarOK(zDataIndx)))
        return false;

    try {
        curr.CalcTripleLimits(cLimits, xDataIndx, yDataIndx, zDataIndx, ntripleMeridian, ntripleSlice, ellipse);
    }
    catch (MathError&)
    {
        return false;
    }

    for (int i = 0; i < ellipse.Size(); i++)
        covarHeaderDC->DeNormalize(ellipse[i], xDataIndx, yDataIndx, zDataIndx);

    return true;
}


Limit3D PFO_CovarLimits::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    if (is3DObject)
    {
        switch (covarPlotType)
        {
            case cptSingle : {
                SC_CoordArray st, end;
                for (int i = 0; i < covarArrayDC->Size(); i++)
                    if (Get3DSingle(i, st, end))
                    {
                        currLimits.AddToLimits(st);
                        currLimits.AddToLimits(end);
                    }
                break;
            }
            case cptDual : {
                SC_CoordArray ell;
                for (int i = 0; i < covarArrayDC->Size(); i++)
                {
                    if (Get3DDual(i, p3D_XY, ell))
                        currLimits.AddToLimits(ell);
                    if (Get3DDual(i, p3D_XZ, ell))
                        currLimits.AddToLimits(ell);
                    if (Get3DDual(i, p3D_YZ, ell))
                        currLimits.AddToLimits(ell);
                }
                break;
            }
            case cptTriple : {
                SC_CoordMatrix ell;
                for (int i = 0; i < covarArrayDC->Size(); i++)
                {
                    if (Get3DTriple(i, ell))
                        for (int j = 0; j < ell.Size(); j++)
                            currLimits.AddToLimits(ell[j]);
                }

                break;
            }

        }
    }
    else
    {
        if (covarPlotType == cptSingle)
        {
            SC_PointArray st, end;
            for (int i = 0; i < covarArrayDC->Size(); i++)
                if (Get2DSingle(i, st, end))
                {
                    currLimits.AddToLimits(st);
                    currLimits.AddToLimits(end);
                }
        }
        else
        {
            SC_PointArray ell;
            for (int i = 0; i < covarArrayDC->Size(); i++)
                if (Get2DDual(i, ell))
                    currLimits.AddToLimits(ell);
        }
    }

    return currLimits;
}

