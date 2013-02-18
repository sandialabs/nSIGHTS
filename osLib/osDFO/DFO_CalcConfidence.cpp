///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CalcConfidence.cpp
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
//      Calculates confidence of optimization results, using one result as the best case.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/SC_DoubleArray.h>

#include <osDFO/DFO_CalcConfidence.h>


DFO_CalcConfidence :: DFO_CalcConfidence() : FuncObjC("Calc Opt Confidence")
{
    runIndex = 0;
    fitIndex = 0;
    bestCaseIndex = 0;

    autoBestCase = true;

    InitCommon();
}

DFO_CalcConfidence::DFO_CalcConfidence(const DFO_CalcConfidence& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_CalcConfidence::InitCommon()
{
    AddInPort(optSimResultsObjRef, typeid(DO_OptSimResults));

    AddOutPort(outputConfidenceDO);
    outputConfidenceDO.tableData = &outputConfidenceDC;

    DoStatusChk();
}

DFO_CalcConfidence& DFO_CalcConfidence::operator= (const DFO_CalcConfidence& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_CalcConfidence::LocalCopy(const DFO_CalcConfidence& a)
{
    optSimResultsObjRef = a.optSimResultsObjRef;
    runIndex = a.runIndex;
    fitIndex = a.fitIndex;
    bestCaseIndex = a.bestCaseIndex;

    autoBestCase = a.autoBestCase;
}

void  DFO_CalcConfidence:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (!CheckInputFO(optSimResultsObjRef, "opt input source"))
        return;
    optSimResultsDC = static_cast<DO_OptSimResults*>(GetInPortData(optSimResultsObjRef))->optResults;

    /*if (optSimResultsDC->IsSingle())
    {
        SetObjErrMsg("must be results with multiple cases");
        return;
    }*/

    if (optSimResultsDC->IsEmpty())
    {
        SetObjErrMsg("no runs in output ???");
        return;
    }

    if ((runIndex < 0) || (runIndex >= optSimResultsDC->GetnRuns()))
    {
        SetObjErrMsg("run index out of range");
        return;
    }

    OptSimRunResults& selRun = (*optSimResultsDC)[runIndex];

    if ((fitIndex < 0) || (fitIndex >= selRun.GetnFits()))
    {
        SetObjErrMsg("fit index out of range");
        return;
    }

    if (!autoBestCase)
    {
        if ((bestCaseIndex < 0) || (bestCaseIndex >= selRun.GetnCases()))
        {
            SetObjErrMsg("best fit case index out of range");
            return;
        }
    }
}

void DFO_CalcConfidence:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    OptSimRunResults& selRun = (*optSimResultsDC)[runIndex];
    OptCaseResultsArray& allCases =  selRun.runResults;

    //number of cases selected - a confidence will be calculated for each selected case
    int nCases = selRun.GetnCases();

    if (autoBestCase)
    {
        double minS = 1.0E100;
        for (int i = 0; i < nCases; i++)
        {
            OptCaseResults& currCase = allCases[i];
            OptSingleFit& currFit = currCase.caseResults[fitIndex];
            if (currFit.sumSquaredError < minS)
            {
                minS = currFit.sumSquaredError;
                bestCaseIndex = i;
            }
        }

    }

    OptCaseResults& bestCase = allCases[bestCaseIndex];
    OptSingleFit& bestFit = bestCase.caseResults[fitIndex];

    /*if (bestFit.GetnFitComponent() > 1)
        ApplyWarningMsg(calcType, "Results may be incorrect for composite fits");*/

    double bestS = bestFit.sumSquaredError;
    if (bestS <= 0.0)
    {
        SetObjErrMsg("Expecting best SSE to be greater than 0");
        return;
    }

    int n = bestFit.GetnFitPoints();  //number of data points - should be same for all cases
    int p = optSimResultsDC->noptVar;    //number of parameters
    int nCols = p + 2;  //add parameters as well as confidence values to table

    if (!outputConfidenceDC.Alloc(nCols, nCases, 80)) //1 column, nCases rows, titles with 80 characters
    {
        SetObjErrMsg("table allocation??");
        return;
    }
    outputConfidenceDC.SetSizeToAlloc();

    for (int i = 0; i < p; i++)
        outputConfidenceDC.SetTitle(i, selRun.varDescriptions[i].shortVarDesc);

    for (int i = 0; i < nCases; i++)
        CopyString(outputConfidenceDC.rowDesc[i], allCases[i].caseDesc, 80);

    SC_DoubleArray& alpha = outputConfidenceDC.dataTable[p];  //confidence is after parameter values  in table

    char errMsg[80];
    if (distribution == dF)
    {
        outputConfidenceDC.SetTitle(p, "F-test Alpha");
        outputConfidenceDC.SetTitle(p + 1, "F-test Confidence (1-alpha)");
        outputConfidenceDC.SetID("F-test Confidence");

        SC_DoubleArray fValue(nCases);
        double factor = double(n - p) / double(p);

        for (int i = 0; i < nCases; i++)
        {
            OptCaseResults& currCase = allCases[i];
            OptSingleFit& currFit = currCase.caseResults[fitIndex];
            double currS = currFit.sumSquaredError;
            if (currS <= 0.0)
            {
                SetObjErrMsg("Expecting all SSE to be greater than 0");
                return;
            }

            fValue[i] = factor * fabs(currS - bestS) / currS;

            //add parameters to table
            for (int j = 0; j < p; j++)
                outputConfidenceDC.dataTable[j][i] = currFit.finalParData[j];
        }

        if (!CalcQF(fValue, p, n - p, alpha, errMsg, 80))
        {
            SetObjErrMsg(errMsg);
            return;
        }

    }
    else if (distribution == dChiSqr)
    {
        outputConfidenceDC.SetTitle(p, "Log-Likelihood Alpha");
        outputConfidenceDC.SetTitle(p + 1, "Log-Likelihood Confidence (1-alpha)");
        outputConfidenceDC.SetID("Log-Likelihood Confidence");

        SC_DoubleArray chiSqValue(nCases);
        double logBestS = log(bestS);

        for (int i = 0; i < nCases; i++)
        {
            OptCaseResults& currCase = allCases[i];
            OptSingleFit& currFit = currCase.caseResults[fitIndex];
            double currS = currFit.sumSquaredError;
            if (currS <= 0.0)
            {
                SetObjErrMsg("Expecting all SSE to be greater than 0");
                return;
            }

            chiSqValue[i] = n * fabs(log(currS) - logBestS);

            //add parameters to table
            for (int j = 0; j < p; j++)
                outputConfidenceDC.dataTable[j][i] = currFit.finalParData[j];

        }

        if (!CalcQChiSquared(chiSqValue, p, alpha, errMsg, 80))
        {
            SetObjErrMsg(errMsg);
            return;
        }
    }

    SC_DoubleArray& conf = outputConfidenceDC.dataTable[p + 1];
    for (int i = 0; i < nCases; i++)
        conf[i] = 1.0 - alpha[i];

}

