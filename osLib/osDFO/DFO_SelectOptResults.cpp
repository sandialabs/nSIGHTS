///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectOptResults.cpp
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
//      For calculating fits, and for reading, selecting, and operating on optimizer results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/SC_DoubleArray.h>

#include <osDFO/DFO_SelectOptResults.h>


DFO_SelectOptResults :: DFO_SelectOptResults() : DFO_SelectOpt("SelectOptResults", false, true)
{
    startEndResults         = serNone;
    includeOptResults       = true;
    includeCaseParameters   = false;
    includeOptStatus        = false;
    InitCommon();
}

DFO_SelectOptResults::DFO_SelectOptResults(const DFO_SelectOptResults& a) : DFO_SelectOpt(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectOptResults::InitCommon()
{
    tableData.SetID("Output table");
    tableDO.tableData = &tableData;
    AddOutPort(tableDO);

    optStartEndDC.SetID("Opt start/end table");
    optStartEndDO.tableData = &optStartEndDC;
    AddOutPort(optStartEndDO);

    includeSubFits  = false;

    DoStatusChk();
}

DFO_SelectOptResults& DFO_SelectOptResults::operator= (const DFO_SelectOptResults& a)
{
    if (&a != this)
    {
        DFO_SelectOpt::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectOptResults::LocalCopy(const DFO_SelectOptResults& a)
{
    includeOptResults       = a.includeOptResults;
    includeCaseParameters   = a.includeCaseParameters;
    includeOptStatus        = a.includeOptStatus;
}

void  DFO_SelectOptResults:: DoStatusChk()
{
    DFO_SelectOpt::DoStatusChk();
    indexMS.indexDO.SetTypeLabel("Selected Output");
    if (StatusNotOK())
        return;

    if (!(includeOptResults || includeCaseParameters || includeOptStatus))
        SetObjErrMsg("no output selected");

    ncaseVar = 0;
    if (includeCaseParameters)
    {
        ncaseVar = (*optSimResultsDC)[runIndex[0]].caseDescriptions.Size();
        for (int i = 1; i < runIndex.Size(); i++)
            if ((*optSimResultsDC)[runIndex[i]].caseDescriptions.Size() != ncaseVar)
            {
                SetObjErrMsg("selected runs have different case vars");
                return;
            }
    }

}

void DFO_SelectOptResults:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int ntableCol = 0;
    if (includeOptResults)
    {
        ntableCol += optSimResultsDC->noptVar;
        ntableCol += optSimResultsDC->ncalcVar;
    }
    if (includeCaseParameters)
        ntableCol += ncaseVar;
    if (includeOptStatus)
        ntableCol += 10;

/*
            bool                    fitCompletedOK;     // true if no error on fit
            int                     nsimsRequired;      // # of simulations to complete optimization
            int                     nlmIteration;       // # of LM iterations to complete optimization

            double                  fitValue;           // final result for fit
            double                  derivSpan;          // final derivative span

            double                  sumSquaredError;    // sse -- should be same as fitval for covar runs
            double                  meanSquaredError;   // mse
            double                  estimatedVariance;

            double                  fitDelta;           // sum of normalized difference from start point
*/

    if (!tableData.Alloc(ntableCol, runIndex.Size(), 80))
    {
        SetObjErrMsg("Table allocation problem");
        return;
    }

    if (startEndResults == serNone)
    {
        optStartEndDC.ClearAll();
        optStartEndDC.SetID("No start/end output");
    }
    else
    {
        int ncol = optSimResultsDC->noptVar;
        int nrow = runIndex.Size();

        if (IsSegTable())
            nrow *= 3;
        else
            ncol *= 2;

        if (!optStartEndDC.Alloc(ncol, nrow, 80))
        {
            SetObjErrMsg("Start/end table allocation problem");
            return;
        }
        optStartEndDC.SetID("Opt start/end table");
        optStartEndDC.dataTable.SetSizeToAlloc();
    }


    //  set column headers
    int firstRun = runIndex[0];
    if (includeOptResults)
    {
        OptVarDescriptionArray& currVars =  (*optSimResultsDC)[firstRun].varDescriptions;    // size n opt var
        for (int i = 0; i < currVars.Size(); i++)
            tableData.columnDesc += currVars[i].varDesc;

        CaseVarDescriptionArray& currCalcVars =  (*optSimResultsDC)[firstRun].calcDescriptions;    // size n calc var
        for (int i = 0; i < currCalcVars.Size(); i++)
            tableData.columnDesc += currCalcVars[i].caseVarShortDesc;
    }

    if (includeCaseParameters)
    {
        CaseVarDescriptionArray& currCaseVars =  (*optSimResultsDC)[firstRun].caseDescriptions;    // size n opt var
        for (int i = 0; i < currCaseVars.Size(); i++)
            tableData.columnDesc += currCaseVars[i].caseVarShortDesc;
    }

    if (includeOptStatus)
    {
        tableData.columnDesc +=  "FitValue";
        tableData.columnDesc +=  "SSE";
        tableData.columnDesc +=  "MSE";
        tableData.columnDesc +=  "EstVar";

        tableData.columnDesc +=  "Delta";

        tableData.columnDesc +=  "DerivSpan";
        tableData.columnDesc +=  "#sims";
        tableData.columnDesc +=  "#LMIter";

        tableData.columnDesc +=  "FitOK";
        tableData.columnDesc +=  "FitIndex";
    }

    if (startEndResults != serNone)
    {
        OptVarDescriptionArray& currVars =  (*optSimResultsDC)[firstRun].varDescriptions;    // size n opt var
        char tempStr[80], colStr[80];
        for (int i = 0; i < currVars.Size(); i++)
        {
            if (IsRegTable() )
                CopyString(tempStr, currVars[i].shortVarDesc, 80);
            else
                MakeString(tempStr, "N(", currVars[i].shortVarDesc, ")", 80);

            if (IsSegTable())
            {
                optStartEndDC.columnDesc += tempStr;
            }
            else
            {
                MakeString(colStr, "S(", tempStr, ")", 80);
                optStartEndDC.columnDesc += colStr;
                MakeString(colStr, "E(", tempStr, ")", 80);
                optStartEndDC.columnDesc += colStr;
            }
        }
    }


    for (int i = 0; i < runIndex.Size(); i++)
    {
        OptCaseResults& currCase = (*optSimResultsDC)[runIndex[i]].runResults[caseIndex[i]];
        OptSingleFit& currFit = currCase.caseResults[fitIndex[i]];

        tableData.rowDesc += currCase.caseDesc;

        int nextCol = 0;
        if (includeOptResults)
        {
            for (int j = 0; j < currFit.finalParData.Size(); j++)
                tableData.dataTable[nextCol++] += currFit.finalParData[j];

            for (int j = 0; j < currFit.calcParData.Size(); j++)
                tableData.dataTable[nextCol++] += currFit.calcParData[j];
        }

        if (includeCaseParameters)
        {
            for (int j = 0; j < currCase.caseParVals.Size(); j++)
                tableData.dataTable[nextCol++] += currCase.caseParVals[j];
        }

        if (includeOptStatus)
        {
            tableData.dataTable[nextCol++] += currFit.fitValue;
            tableData.dataTable[nextCol++] += currFit.sumSquaredError;
            tableData.dataTable[nextCol++] += currFit.meanSquaredError;
            tableData.dataTable[nextCol++] += currFit.estimatedVariance;
            tableData.dataTable[nextCol++] += currFit.fitDelta;
            tableData.dataTable[nextCol++] += currFit.derivSpan;
            tableData.dataTable[nextCol++] += double(currFit.nsimsRequired);
            tableData.dataTable[nextCol++] += double(currFit.niteration);
            tableData.dataTable[nextCol++] += double(currFit.fitCompletedOK);
            tableData.dataTable[nextCol++] += double(fitIndex[i] + 1);
        }

        if (startEndResults == serNone)
            continue;

        SC_DoubleArray *startVals, *endVals;
        if (IsRegTable())
        {
            startVals = &currFit.startParData;
            endVals = &currFit.finalParData;
        }
        else
        {
            startVals = &currFit.startNormParData;
            endVals = &currFit.finalNormParData;
        }

        int nvar = endVals->Size();
        if (IsSegTable())
        {
            char tempStr[80];
            MakeString(tempStr, currCase.caseDesc, "/S", 80);
            optStartEndDC.rowDesc += tempStr;
            MakeString(tempStr, currCase.caseDesc, "/E", 80);
            optStartEndDC.rowDesc += tempStr;
            optStartEndDC.rowDesc += "Null";

            nextCol = 0;
            for (int j = 0; j < nvar; j++)
            {
                optStartEndDC.dataTable[nextCol] += (*startVals)[j];
                optStartEndDC.dataTable[nextCol] += (*endVals)[j];
                optStartEndDC.dataTable[nextCol++] += nullReal;
            }
        }
        else
        {
            optStartEndDC.rowDesc += currCase.caseDesc;
            nextCol = 0;
            for (int j = 0; j < nvar; j++)
            {
                optStartEndDC.dataTable[nextCol++] += (*startVals)[j];
                optStartEndDC.dataTable[nextCol++] += (*endVals)[j];
            }
        }
    }

    tableData.dataTable.SetSize(ntableCol);
}

