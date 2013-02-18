///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CalcConfidenceGrid.cpp
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
//      Calculates confidence of grid point results, where the grid value is the fit result, using one grid point as the best case.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/SC_DoubleArray.h>

#include <osDFO/DFO_CalcConfidenceGrid.h>


DFO_CalcConfidenceGrid :: DFO_CalcConfidenceGrid() : FuncObjC("Calc Grid Confidence")
{
    bestCaseXIndex = 0;
    bestCaseYIndex = 0;
    autoBestCase = true;
    nDataPts = 1;
    nParam = 1;

    InitCommon();
}

DFO_CalcConfidenceGrid::DFO_CalcConfidenceGrid(const DFO_CalcConfidenceGrid& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_CalcConfidenceGrid::InitCommon()
{
    AddInPort(inputGridResultsObjRef, typeid(DO_GridData));

    AddOutPort(outputGridDO);
    outputGridDO.gridData = &outputGridDC;

    AddOutPort(outputAlphaGridDO);
    outputAlphaGridDO.gridData = &outputAlphaGridDC;

    DoStatusChk();
}

DFO_CalcConfidenceGrid& DFO_CalcConfidenceGrid::operator= (const DFO_CalcConfidenceGrid& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_CalcConfidenceGrid::LocalCopy(const DFO_CalcConfidenceGrid& a)
{
    inputGridResultsObjRef = a.inputGridResultsObjRef;

    bestCaseXIndex = a.bestCaseXIndex;
    bestCaseYIndex = a.bestCaseYIndex;
    autoBestCase = a.autoBestCase;

    nDataPts = a.nDataPts;
    nParam = a.nParam;
}

void  DFO_CalcConfidenceGrid:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (!CheckInputFO(inputGridResultsObjRef, "test grid input source"))
        return;
    inputGridResultsDC = static_cast<DO_GridData*>(GetInPortData(inputGridResultsObjRef))->gridData;

    if (inputGridResultsDC->IsEmpty())
    {
        SetObjErrMsg("no output ???");
        return;
    }

    if (!autoBestCase)
    {
        if ((bestCaseXIndex < 0) || (bestCaseXIndex >= inputGridResultsDC->xData.Size()))
        {
            SetObjErrMsg("best fit case X index out of range");
            return;
        }
        if ((bestCaseYIndex < 0) || (bestCaseYIndex >= inputGridResultsDC->yData.Size()))
        {
            SetObjErrMsg("best fit case Y index out of range");
            return;
        }
    }
}

void DFO_CalcConfidenceGrid:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    //number of cases selected - a confidence will be calculated for each selected case
    int nx = inputGridResultsDC->xData.Size();
    int ny = inputGridResultsDC->yData.Size();

    if (autoBestCase)
    {
        double minS = 1.0E100;
        for (int i = 0; i < nx; i++)
        {
            for (int j = 0; j < ny; j++)
            {
                double sse = inputGridResultsDC->gData[i][j];
                if (sse < minS)
                {
                    minS = sse;
                    bestCaseXIndex = i;
                    bestCaseYIndex = j;
                }
            }
        }

    }

    double bestS = inputGridResultsDC->gData[bestCaseXIndex][bestCaseYIndex];
    if (bestS <= 0.0)
    {
        SetObjErrMsg("Expecting best SSE to be greater than 0");
        return;
    }
    int n = nDataPts;  //number of data points - should be same for all cases

    if (!outputGridDC.CreateFrom(*inputGridResultsDC))
    {
        SetObjErrMsg("grid allocation??");
        return;
    }

    if (!outputAlphaGridDC.CreateFrom(*inputGridResultsDC))
    {
        SetObjErrMsg("alpha grid allocation??");
        return;
    }

    char errMsg[80];
    for (int i = 0; i < nx; i++)
    {
        SC_DoubleArray& alpha = outputGridDC.gData[i];
        int p = nParam;  //number of parameters

        if (distribution == dF)
        {
            SC_DoubleArray fValue(ny);
            double factor = double(n - p) / double(p);

            for (int j = 0; j < ny; j++)
            {
                double currS = inputGridResultsDC->gData[i][j];
                if (currS <= 0.0)
                {
                    SetObjErrMsg("Expecting all SSE to be greater than 0");
                    return;
                }
                fValue[j] = factor * fabs(currS - bestS) / currS;
            }

            if (!CalcQF(fValue, p, n - p, alpha, errMsg, 80))
            {
                SetObjErrMsg(errMsg);
                return;
            }
        }
        else if (distribution == dChiSqr)
        {
            SC_DoubleArray chiSqValue(ny);
            double logBestS = log(bestS);

            for (int j = 0; j < ny; j++)
            {
                double currS = inputGridResultsDC->gData[i][j];
                if (currS <= 0.0)
                {
                    SetObjErrMsg("Expecting all SSE to be greater than 0");
                    return;
                }
                chiSqValue[j] = n * fabs(log(currS) - logBestS);
            }

            if (!CalcQChiSquared(chiSqValue, p, alpha, errMsg, 80))
            {
                SetObjErrMsg(errMsg);
                return;
            }
        }
        SC_DoubleArray& conf = outputAlphaGridDC.gData[i];
        for (int k = 0; k < ny; k++)
            conf[k] = 1.0 - alpha[k];
    }

    if (distribution == dF)
    {
        outputGridDC.SetID("F-test alpha");
        outputAlphaGridDC.SetID("F-test confidence (1-alpha)");
    }
    else if (distribution == dChiSqr)
    {
        outputGridDC.SetID("Log-likelihood alpha");
        outputAlphaGridDC.SetID("Log-likelihood confidence (1-alpha)");
    }
}

