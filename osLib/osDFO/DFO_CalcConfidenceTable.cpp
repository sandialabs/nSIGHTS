///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CalcConfidenceTable.cpp
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
//      Calculates confidence of a table containing fit results, using one row as the best case.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/SC_DoubleArray.h>

#include <osDFO/DFO_CalcConfidenceTable.h>


DFO_CalcConfidenceTable :: DFO_CalcConfidenceTable() : FuncObjC("Calc Table Confidence")
{
    bestCaseIndex = 0;
    autoBestCase = true;
    nDataPts = 1;
    nParam = 1;

    InitCommon();
}

DFO_CalcConfidenceTable::DFO_CalcConfidenceTable(const DFO_CalcConfidenceTable& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_CalcConfidenceTable::InitCommon()
{
    DSC_TableColSel::AddPort(*this);

    AddOutPort(outputConfidenceDO);
    outputConfidenceDO.tableData = &outputConfidenceDC;

    DoStatusChk();
}

DFO_CalcConfidenceTable& DFO_CalcConfidenceTable::operator= (const DFO_CalcConfidenceTable& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        DSC_FandChiConfidence::operator =(a);
        DSC_TableColSel::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_CalcConfidenceTable::LocalCopy(const DFO_CalcConfidenceTable& a)
{
    bestCaseIndex = a.bestCaseIndex;
    autoBestCase = a.autoBestCase;

    nDataPts = a.nDataPts;
    nParam = a.nParam;
}

void  DFO_CalcConfidenceTable:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
    if (StatusOK())
    {
        if ((bestCaseIndex < 0) || (bestCaseIndex >= inputData->GetNRows()))
        {
            SetObjErrMsg("best case index out of range");
            return;
        }
    }
}

void DFO_CalcConfidenceTable:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    //number of cases selected - a confidence will be calculated for each selected case
    int nCases = inputData->GetNRows();

    if (autoBestCase)
    {
        double minS = 1.0E100;
        for (int i = 0; i < nCases; i++)
        {
            if ((*selectedColData)[i] < minS)
            {
                minS = (*selectedColData)[i];
                bestCaseIndex = i;
            }
        }

    }

    double bestS = (*selectedColData)[bestCaseIndex];
    if (bestS <= 0.0)
    {
        SetObjErrMsg("Expecting best SSE to be greater than 0");
        return;
    }
    int n = nDataPts;  //number of data points - should be same for all cases

    if (!outputConfidenceDC.Alloc(2, nCases, 80)) //2 columns, nCases rows, titles with 80 characters
    {
        SetObjErrMsg("table allocation??");
        return;
    }
    outputConfidenceDC.SetSizeToAlloc();
    outputConfidenceDC.rowDesc = inputData->rowDesc;

    SC_DoubleArray& alpha = outputConfidenceDC.dataTable[0];
    int p = nParam;  //number of parameters

    char errMsg[80];
    if (distribution == dF)
    {
        outputConfidenceDC.SetID("F-test Confidence");
        outputConfidenceDC.SetTitle(0, "F-test Alpha");
        outputConfidenceDC.SetTitle(1, "F-test Confidence (1-alpha)");

        SC_DoubleArray fValue(nCases);
        double factor = double(n - p) / double(p);

        for (int i = 0; i < nCases; i++)
        {
            double currS = (*selectedColData)[i];
            if (currS <= 0.0)
            {
                SetObjErrMsg("Expecting all SSE to be greater than 0");
                return;
            }
            fValue[i] = factor * fabs(currS - bestS) / currS;
        }

        if (!CalcQF(fValue, p, n - p, alpha, errMsg, 80))
        {
            SetObjErrMsg(errMsg);
            return;
        }
    }
    else if (distribution == dChiSqr)
    {
        outputConfidenceDC.SetID("Log-Likelihood Confidence");
        outputConfidenceDC.SetTitle(0, "Log-Likelihood Alpha");
        outputConfidenceDC.SetTitle(1, "Log-Likelihood Confidence (1-alpha)");

        SC_DoubleArray chiSqValue(nCases);
        double logBestS = log(bestS);

        for (int i = 0; i < nCases; i++)
        {
            double currS = (*selectedColData)[i];
            if (currS <= 0.0)
            {
                SetObjErrMsg("Expecting all SSE to be greater than 0");
                return;
            }
            chiSqValue[i] = n * fabs(log(currS) - logBestS);
        }

        if (!CalcQChiSquared(chiSqValue, p, alpha, errMsg, 80))
        {
            SetObjErrMsg(errMsg);
            return;
        }
    }

    SC_DoubleArray& conf = outputConfidenceDC.dataTable[1];
    for (int i = 0; i < nCases; i++)
        conf[i] = 1.0 - alpha[i];

}

