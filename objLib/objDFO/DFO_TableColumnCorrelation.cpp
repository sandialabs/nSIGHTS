///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableColumnCorrelation.cpp
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

#include <objClass/DC_Curve.h>

#include <objDFO/DFO_TableColumnCorrelation.h>

DFO_TableColumnCorrelation :: DFO_TableColumnCorrelation() :
    FuncObjC("Correlate Table Columns")
{
    selectedYCol = 0;
    logX = false;
    logY = false;
    InitCommon();
}

DFO_TableColumnCorrelation::DFO_TableColumnCorrelation(const DFO_TableColumnCorrelation& a) :
    FuncObjC(a),
    DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableColumnCorrelation :: ~DFO_TableColumnCorrelation ()
{
}

void DFO_TableColumnCorrelation::InitCommon()
{
    outputLineDC.SetID("Correlation line");
    outputLineDO.xyData = &outputLineDC;
    outputDataDC.SetID("Correlation data");
    outputDataDO.xyData = &outputDataDC;

    DSC_TableColSel::AddPort(*this);
    AddOutPort(outputLineDO);
    AddOutPort(outputDataDO);
    AddOutPort(pearsonRDO);
    AddOutPort(spearmanRDO);
    AddOutPort(xcolumnIDDO);
    AddOutPort(ycolumnIDDO);
    AddOutPort(lineSlopeDO);
    AddOutPort(lineYInterceptDO);

    pearsonRDO.Reset("Pearson R");
    spearmanRDO.Reset("Spearman R");
    xcolumnIDDO.Reset("X Data ID");
    ycolumnIDDO.Reset("Y Data ID");
    lineSlopeDO.Reset("Fit Line Slope");
    lineYInterceptDO.Reset("Fit Line Y intercept");

    DoStatusChk();
}

DFO_TableColumnCorrelation& DFO_TableColumnCorrelation::operator= (const DFO_TableColumnCorrelation& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableColumnCorrelation::LocalCopy(const DFO_TableColumnCorrelation& a)
{
    selectedYCol   = a.selectedYCol;
    logX = a.logX;
    logY = a.logY;
}


void  DFO_TableColumnCorrelation:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
    if (StatusOK())
    {
        if ((inputData->dataTable.Size() <= selectedYCol) || (selectedYCol < 0))
            SetObjErrMsg("Y column index out of range");
    }
}

void DFO_TableColumnCorrelation:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;

    xcolumnIDDO.SetValueLabel(inputData->GetTitle(selectedCol));
    ycolumnIDDO.SetValueLabel(inputData->GetTitle(selectedYCol));

    const SC_DoubleArray& xIn = *selectedColData;
    const SC_DoubleArray& yIn = inputData->dataTable[selectedYCol];

    SC_DoubleArray& xCorr = outputDataDC.xData;
    SC_DoubleArray& yCorr = outputDataDC.yData;

    xCorr.Alloc(xIn.Size());
    yCorr.Alloc(xIn.Size());

    int nOK = 0;
    int i;
    for (i = 0; i < xIn.Size(); i++)
    {
        double nextX = xIn[i];
        double nextY = yIn[i];

        if (RealIsNull(nextX) || RealIsNull(nextY))
            continue;

        if (logX)
        {
            if (nextX < stdEps)
                continue;
            nextX = log10(nextX);
        }

        if (logY)
        {
            if (nextY < stdEps)
                continue;
            nextY = log10(nextY);
        }

        xCorr[nOK] = nextX;
        yCorr[nOK++] = nextY;
    }

    if (nOK < 2)
    {
        SetObjErrMsg("Less than two points to correlate");
        return;
    }
    xCorr.SetSize(nOK);
    yCorr.SetSize(nOK);

    pearsonRDO.InitLabelAndValue(PearsonR(xCorr, yCorr));
    spearmanRDO.InitLabelAndValue(SpearmanR(xCorr, yCorr));

    // make correlation line
    DC_Curve corrLine;
    corrLine.curveType = Curve::ctPolynomial;
    corrLine.polyOrder = 1;

    SC_SetupErr crvErr;
    outputLineDC.CreateFrom(xCorr, yCorr);
    if (!corrLine.CreateCurve(outputLineDC, crvErr))
    {
        SetObjErrMsg(crvErr.GetErrorText());
        return;
    }
    outputLineDC.xData.Sort(true);
    corrLine.GetCurveY(outputLineDC.xData, outputLineDC.yData);

    // unlog data
    if (logX)
    {
        outputLineDC.xData.InvLog10();
        outputDataDC.xData.InvLog10();
    }

    if (logY)
    {
        outputLineDC.yData.InvLog10();
        outputDataDC.yData.InvLog10();
    }
    lineSlopeDO.InitLabelAndValue(corrLine.GetPolyCoefficients()[1]);
    lineYInterceptDO.InitLabelAndValue(corrLine.GetPolyCoefficients()[0]);
}

