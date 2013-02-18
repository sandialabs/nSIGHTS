///////////////////////////////////////////////////////////////////////////////////
//
// PFO_Scatterplot.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <saPFO/PFO_Scatterplot.h>



PFO_Scatterplot :: PFO_Scatterplot(PlotDefC& assPlt)
    : PlotObjC("SA Scatterplot", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_Scatterplot::PFO_Scatterplot(const PFO_Scatterplot& a, PlotDefC& assPlt) : PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_Scatterplot :: ~PFO_Scatterplot ()
{
}

void PFO_Scatterplot::InitCommon()
{
    reportOK = false;

    AddInPort(scatterDataObjRef,      typeid(DO_SAPlotTable));
    AddUseFlagInPort(xgridLineObjRef, typeid(DO_SAPlotTable), plotXGridLines);
    AddUseFlagInPort(ygridLineObjRef, typeid(DO_SAPlotTable), plotYGridLines);

    AddOutPort(seriesLegendData);

    seriesLegendData.Alloc(1);

    DoStatusChk();
}

PFO_Scatterplot& PFO_Scatterplot::operator= (const PFO_Scatterplot& a)
{
    if (&a != this)
    {
        PlotObjC::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_Scatterplot::LocalCopy(const PFO_Scatterplot& a)
{
    scatterDataObjRef = a.scatterDataObjRef;
    xgridLineObjRef   = a.xgridLineObjRef;
    ygridLineObjRef   = a.ygridLineObjRef;

    plotXGridLines  = a.plotXGridLines;
    plotYGridLines  = a.plotYGridLines;
    plotAxesLabels  = a.plotAxesLabels;
    scatterSpec     = a.scatterSpec;
}


void PFO_Scatterplot::SetDefault()
{
    plotXGridLines  = false;
    plotYGridLines  = false;
    plotAxesLabels  = false;

    plotDVIndx = 0;
    plotIVIndx = 0;

    scatterSpec = PSC_SymbolSpec();
    scatterSpec.symbolType = ps_Square;
}

void  PFO_Scatterplot:: DoStatusChk()
{
    // base class check
    PlotObjC::DoStatusChk();

    // check input objects
    if (!CheckInputFO(scatterDataObjRef, "Scatterplot data"))
        return;

    const DO_SAPlotTable* scatterDO = static_cast<DO_SAPlotTable*>(GetInPortData(scatterDataObjRef));
    nplotDV = scatterDO->nDV;
    nplotIV = scatterDO->nIV;

    if ((nplotDV == 0) || (nplotIV == 0))
        SetObjErrMsg("not a scatter data table (maybe grid lines?)");
    else if (plotDVIndx >= nplotDV)
        SetObjErrMsg("DV to plot out of range");
    else if (plotIVIndx >= nplotIV)
        SetObjErrMsg("IV to plot out of range");

    if (StatusNotOK())
        return;

    scatterDataDC = scatterDO->tableData;
    dvLabel = scatterDataDC->columnDesc[plotDVIndx + nplotIV];
    ivLabel = scatterDataDC->columnDesc[plotIVIndx];

    if (plotXGridLines)
    {
        if (!CheckInputFO(xgridLineObjRef, "X grid lines"))
            return;

        const DO_SAPlotTable* xgridDO = static_cast<DO_SAPlotTable*>(GetInPortData(xgridLineObjRef));

        if (nplotIV != xgridDO->nIV)
            SetObjErrMsg("X grid line incompatible with scatter data");

        xGridLinesDC = xgridDO->tableData;
    }
    else
        xGridLinesDC = 0;


    if (plotYGridLines)
    {
        if (!CheckInputFO(ygridLineObjRef, "Y grid lines"))
            return;

        const DO_SAPlotTable* ygridDO = static_cast<DO_SAPlotTable*>(GetInPortData(ygridLineObjRef));

        if ((nplotDV != ygridDO->nDV))
            SetObjErrMsg("Y grid line incompatible with scatter data");

        yGridLinesDC = ygridDO->tableData;
    }
    else
        yGridLinesDC = 0;

    seriesLegendData.Clear();
    if (StatusNotOK())
        return;

    char legTitle[80];
    MakeString(legTitle, dvLabel, "/", ivLabel, 80);

    seriesLegendData.AddSeries(legTitle, scatterSpec);
}


void PFO_Scatterplot:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

Limit3D PFO_Scatterplot::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    {
        const SC_DoubleArray& xData = scatterDataDC->dataTable[plotIVIndx];
        const SC_DoubleArray& yData = scatterDataDC->dataTable[plotDVIndx + nplotIV];

        for (int j = 0; j < xData.Size(); j++)
        {
            double xVal = xData[j];
            double yVal = yData[j];
            if ((xLin || (xVal > stdEps)) && (yLin || (yVal > stdEps)))
                currLimits.AddToLimits(Coord3D(xVal, yVal, nullReal));
        }
    }

    if (plotXGridLines)
    {
        const SC_DoubleArray& xData = xGridLinesDC->dataTable[plotIVIndx];
        for (int j = 0; j < xData.Size(); j++)
        {
            double xVal = xData[j];
            if ((!RealIsNull(xVal)) && (xLin || (xVal > stdEps)))
                currLimits.AddToLimits(Coord3D(xVal, nullReal, nullReal));
        }
    }

    if (plotYGridLines)
    {
        const SC_DoubleArray& yData = yGridLinesDC->dataTable[plotDVIndx];

        for (int j = 0; j < yData.Size(); j++)
        {
            double yVal = yData[j];
            if ((!RealIsNull(yVal)) && (yLin || (yVal > stdEps)))
                currLimits.AddToLimits(Coord3D(nullReal, yVal, nullReal));
        }
    }


    return currLimits;
}

