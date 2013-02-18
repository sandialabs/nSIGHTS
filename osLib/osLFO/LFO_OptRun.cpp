///////////////////////////////////////////////////////////////////////////////////
//
// LFO_OptRun.cpp
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
//      lists summary data for an optimization run.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osLFO/LFO_OptRun.h>


LFO_OptRun :: LFO_OptRun(ListDefC& assLst)
    : LFO_OptBase("OptRun", assLst)
{
    listRuns        = true;

    csActual        = false;
    csEstimated     = true;
    csFitComponent  = false;

    mosListAll          = false;
    mosListBest         = true;
    mosListStatistics   = false;

    listRunSummary                      = true;
    listFitValues                       = true;
    listParameterEstimates              = true;
    listEstimatedDataVariances          = false;
    listParameterCorrelations           = true;
    listNormalizedCovarianceMatrices    = false;
    listDenormalizedCovarianceMatrices  = false;
    list95ConfidenceLimits              = true;
    listJacobian                        = false;
    listModelStructure                  = false;
    listCovarianceMatrixCorrelations    = false;

    InitCommon();
}

LFO_OptRun::LFO_OptRun(const LFO_OptRun& a, ListDefC& assLst) : LFO_OptBase(a, assLst)
{
    LocalCopy(a);
    InitCommon();
}

LFO_OptRun :: ~LFO_OptRun ()
{
}

void LFO_OptRun::InitCommon()
{
    selectedOpts.SetResizable(10);
    selectedFit.SetResizable(10);
    selectedCases.SetResizable(10);
    runsToList.SetResizable(10);
    casesToList.SetResizable(10);

    AddInPort(optSimResultsObjRef, typeid(DO_OptSimResults));
    DoStatusChk();
}

LFO_OptRun& LFO_OptRun::operator= (const LFO_OptRun& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void LFO_OptRun::LocalCopy(const LFO_OptRun& a)
{
    optSimResultsObjRef                 = a.optSimResultsObjRef;

    listRuns                            = a.listRuns;
    runsToList                          = a.runsToList;
    casesToList                         = a.casesToList;

    csActual                            = a.csActual;
    csEstimated                         = a.csEstimated;
    csFitComponent                      = a.csFitComponent;

    mosListAll                          = a.mosListAll;
    mosListBest                         = a.mosListBest;
    mosListStatistics                   = a.mosListStatistics;

    listRunSummary                      = a.listRunSummary;
    listFitValues                       = a.listFitValues;
    listParameterEstimates              = a.listParameterEstimates;
    listEstimatedDataVariances          = a.listEstimatedDataVariances;
    listParameterCorrelations           = a.listParameterCorrelations;
    listNormalizedCovarianceMatrices    = a.listNormalizedCovarianceMatrices;
    listDenormalizedCovarianceMatrices  = a.listDenormalizedCovarianceMatrices;
    list95ConfidenceLimits              = a.list95ConfidenceLimits;
    listJacobian                        = a.listJacobian;
    listModelStructure                  = a.listModelStructure;
    listCovarianceMatrixCorrelations    = a.listCovarianceMatrixCorrelations;
}


void LFO_OptRun::DoStatusChk()
{
    selectedOpts.SetEmpty();
    selectedFit.SetEmpty();
    selectedCases.SetEmpty();

    FuncObjC::DoStatusChk();
    if (!CheckInputFO(optSimResultsObjRef, "Optimization results"))
        return;

    if (listRuns)
    {
        if (runsToList.IsEmpty())
            runsToList += 0;
    }
    else
    {
        if (casesToList.IsEmpty())
            casesToList += 0;
    }


    DC_OptSimResults& optSimResultsDC = *(static_cast<DO_OptSimResults*>(GetInPortData(optSimResultsObjRef))->optResults);

    runHeader = optSimResultsDC;

    int maxRun = optSimResultsDC.Size();

    if (listRuns)
    {
        for (int i = 0; i < runsToList.Size(); i++)
        {
            int runIndex = runsToList[i];
            int indx = 0;

            for (int j = 0; (j < maxRun) && (indx <= runIndex); j++)
            {
                int nFit = optSimResultsDC[j].GetnFits();
                int nCase = optSimResultsDC[j].GetnCases();
                for (int k = 0; k < nFit; k++)
                {
                    if (indx++ == runIndex)
                    {
                        for (int l = 0; l < nCase; l++)
                        {
                            selectedOpts += &optSimResultsDC[j];
                            selectedFit += k;
                            selectedCases += l;
                        }
                        break;
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < casesToList.Size(); i++)
        {
            int caseIndex = casesToList[i];
            int indx = 0;
            for (int j = 0; (j < maxRun) && (indx <= caseIndex); j++)
            {
                int nFit = optSimResultsDC[j].GetnFits();
                int nCase = optSimResultsDC[j].GetnCases();
                for (int k = 0; (k < nFit) && (indx <= caseIndex); k++)
                {
                    for (int l = 0; l < nCase; l++)
                    {
                        if (indx++ == caseIndex)
                        {
                            selectedOpts += &optSimResultsDC[j];
                            selectedCases += l;
                            selectedFit += k;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (selectedOpts.IsEmpty())
        SetObjErrMsg("no runs selected ??");


    bool listOK = listRunSummary || listFitValues || listParameterEstimates;
    if (runHeader.containsCovariance)
    {
        listOK = listOK || listEstimatedDataVariances || listParameterCorrelations
                        || listNormalizedCovarianceMatrices || listDenormalizedCovarianceMatrices
                        || list95ConfidenceLimits || listCovarianceMatrixCorrelations;
    }
    if (runHeader.containsJacobian)
    {
        listOK = listOK || listJacobian  || listModelStructure;
    }

    if (!listOK)
        SetObjErrMsg("no valid listings selected");

    if (!runHeader.IsSingle())
    {
        listOK = mosListAll || mosListBest || mosListStatistics;

        if (!listOK)
            SetObjErrMsg("no multiple optimization selections set");
    }

    if (runHeader.containsCovariance)
    {
        listOK = csActual || csEstimated || csFitComponent;
        if (!listOK)
            SetObjErrMsg("no covariance selections set");
    }

}


void LFO_OptRun::ListOptOK(const OptSingleFit& currFit)
{
    if ((!currFit.fitCompletedOK) && (!badOptMsg))
    {
        AddLine("** Optimization not successful ** ");
        AddBlankLine();
        badOptMsg = true;
    }
}


void LFO_OptRun::ListRunSummary(const OptSimRunResults& currRun,
                                      int               fitIndex)
{
    OptSingleFit& currFit = currRun.runResults[singleFitCase].caseResults[fitIndex];
    ListOptOK(currFit);

    AddStdText("Algorithm", currRun.optDescription);

    AddStdInt("# of optimized parameters", runHeader.noptVar);

    if (!runHeader.IsSingle())
        if (currFit.fitCompletedOK)
            AddStdInt("Best fit case index", singleFitCase + 1);
        else
            AddStdInt("Minimum fit value case index", singleFitCase + 1);

    OptFitDescription& fitDesc = currRun.fitDescriptions[fitIndex];

    if (runHeader.containsCovariance)
    {
        AddBlankLine();
        int npts = currFit.GetnFitPoints();
        AddStdInt("# of Chi-squared points", npts);
        AddStdInt("Degrees of freedom", npts - runHeader.noptVar);
        AddBoolText("Covariance calculation", currRun.fullCovarianceCalc, "2nd Order", "1st Order");
    }

    if (runHeader.IsSample())
    {
        AddBlankLine();
        AddStdInt("# of sampled parameters", currRun.caseDescriptions.Size());
        AddStdInt("# of samples", currRun.runResults.Size());
    }

    {
        AddBlankLine();
        AddLine("Parameter Normalizations");

        SC_IntArray colWidths(4, 15);
        colWidths[0] = 10;
        colWidths[1] = 5;
        TableSetup(colWidths, 3);
        SetTableCol("Name", "Type", "0 Value", "1 Value");

        SetDefaultFormat(SC_SciFormat(4));

        for (int i = 0; i < runHeader.noptVar; i++)
        {
            OptVarDescription& currVar = currRun.varDescriptions[i];

            SetTableCol(0, currVar.shortVarDesc);
            if (currVar.parIsLinear)
                SetTableCol(1, "Lin");
            else
                SetTableCol(1, "Log");

            SetTableReal(2, currVar.userMinLim);
            SetTableReal(3, currVar.userMaxLim);
            AddNextLine();
        }
        AddBlankLine();
    }


    if (runHeader.IsSample() || runHeader.IsPerturb())
    {

        if (runHeader.IsSample())
            AddLine("Sampled Parameter Descriptions");
        else
            AddLine("Perturbed Parameter Statistics");

        SC_IntArray colWidths(8, 10);
        colWidths[1] = 8;
        colWidths[2] = 5;
        colWidths[6] = 8;
        colWidths[7] = 8;

        TableSetup(colWidths, 3);

        SetTableCol(0, "Name");
        SetTableCol(1, "Units");
        SetTableCol(2, "Type");
        SetTableCol(3, "Mean");
        SetTableCol(4, "Max");
        SetTableCol(5, "Min");
        SetTableCol(6, "MaxIndx");
        SetTableCol(7, "MinIndx");
        AddNextLine();

        int nvars = currRun.caseDescriptions.Size();

        SC_DoubleArray caseData(ncurrCase);
        SC_Statistics  stats;

        SetDefaultFormat(SC_SciFormat(4));
        for (int i = 0; i < nvars; i++)
        {
            CaseVarDescription& currVar = currRun.caseDescriptions[i];

            for (int j = 0; j < ncurrCase; j++)
                caseData[j] = currRun.runResults[currentCases[j]].caseParVals[i];
            stats.CalcStatistics(caseData, !currVar.varIsLinear);

            SetTableCol(0, currVar.caseVarShortDesc);
            SetTableCol(1, currVar.caseVarUnits);
            if (currVar.varIsLinear)
                SetTableCol(2, "Lin");
            else
                SetTableCol(2, "Log");

            SetTableReal(3, stats.realResults[SC_Statistics::soMean]);
            SetTableReal(4, stats.realResults[SC_Statistics::soMax]);
            SetTableReal(5, stats.realResults[SC_Statistics::soMin]);

            SetTableInt(6, int(stats.intResults[SC_Statistics::soMaxIndx]));
            SetTableInt(7, int(stats.intResults[SC_Statistics::soMinIndx]));

            AddNextLine();
        }

    }

}


void LFO_OptRun::SetChisqTable(const OptSingleFit& currFit)
{
    SetTableReal(1, currFit.fitValue);
    SetTableReal(2, currFit.sumSquaredError);
    SetTableReal(3, currFit.meanSquaredError);
    SetTableReal(4, currFit.derivSpan);
    SetTableInt(5, currFit.nsimsRequired);
    SetTableReal(6, currFit.fitDelta);
    SetTableBool(7, currFit.fitCompletedOK, "OK", "Bad");
    AddNextLine();
}

void LFO_OptRun::SetChisqStatsTable(const SC_StatisticsArray& stats,
                                          SC_Statistics::StatisticOp op)
{
    SetTableCol(0, SC_Statistics::opIDs[op]);
    for (int i = 0; i < 6; i++)
    {
        if (i != 4)
            SetTableReal(i + 1, stats[i].realResults[op]);
        else
            SetTableReal(i + 1, stats[i].realResults[op], SC_DecFormat(0));
    }
    SetTableCol(7, "n/a");
    AddNextLine();
}

void LFO_OptRun::SetNonChisqTable(const OptSingleFit& currFit)
{
    SetTableReal(1, currFit.fitValue);
    SetTableInt(2, currFit.nsimsRequired);
    SetTableReal(3, currFit.fitDelta);
    SetTableBool(4, currFit.fitCompletedOK, "OK", "Bad");
    AddNextLine();
}



void LFO_OptRun::SetNonChisqStatsTable(const SC_StatisticsArray&  stats,
                                          SC_Statistics::StatisticOp op)
{
    SetTableCol(0, SC_Statistics::opIDs[op]);
    for (int i = 0; i < 3; i++)
    {
        if (i != 1)
            SetTableReal(i + 1, stats[i].realResults[op]);
        else
            SetTableReal(i + 1, stats[i].realResults[op], SC_DecFormat(0));
    }
    SetTableCol(4, "n/a");
    AddNextLine();
}

bool LFO_OptRun::SingleFitOnly()
{
    return (ncurrCase == 1) || runHeader.IsSingle() || (!(mosListAll  || mosListStatistics));
}


void LFO_OptRun::ListFitValues(const OptSimRunResults& currRun,
                                     int               fitIndex)
{
    TextSubTitle("Fit Value Data");

    if (SingleFitOnly())
    {
        OptSingleFit& currFit = currRun.runResults[singleFitCase].caseResults[fitIndex];

        ListOptOK(currFit);

        AddStdInt("# of simulations", currFit.nsimsRequired);
        AddStdInt("# of optimizer iterations", currFit.niteration);

        SetDefaultFormat(SC_SciFormat(5));
        if (runHeader.containsCovariance)
        {
            AddStdReal("Chi-squared value", currFit.fitValue);
            AddStdReal("Sum of squared errors", currFit.sumSquaredError);
            AddStdReal("Mean squared errors", currFit.meanSquaredError);
            AddStdReal("Derivative span", currFit.derivSpan);
        }
        else
        {
            AddStdReal("Best-fit value", currFit.fitValue);
        }


        AddNextLine();
        return;
    }

    SetDefaultFormat(SC_SciFormat(5));

    if (runHeader.containsCovariance)
    {
        SC_IntArray colWidths(8, 10);
        colWidths[0] = 6;
        colWidths[5] = 5;
        colWidths[7] = 6;


        TableSetup(colWidths, 3);
        SetTableCol(0, "CaseID");
        SetTableCol(1, "ChiSq");
        SetTableCol(2, "SSE");
        SetTableCol(3, "MSE");
        SetTableCol(4, "dSpan");
        SetTableCol(5, "#Sim");
        SetTableCol(6, "ParDelta");
        SetTableCol(7, "Status");

        AddNextLine();
        AddNextLine();

        if (mosListBest)
        {
            SetTableCol(0, "Best");
            SetChisqTable(currRun.runResults[singleFitCase].caseResults[fitIndex]);
            AddNextLine();
        }

        // stats
        if (mosListStatistics)
        {

            SC_DoubleMatrix statData;
            statData.MatrixAlloc(6, ncurrCase);

            if (!allCasesOK)
                AddLine("OK Statistics");

            int i;
            for (i = 0; i < ncurrCase; i++)
            {
                OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
                if (!currFit.fitCompletedOK)
                    continue;

                statData[0] += currFit.fitValue;
                statData[1] += currFit.sumSquaredError;
                statData[2] += currFit.meanSquaredError;
                statData[3] += currFit.derivSpan;
                statData[4] += double(currFit.nsimsRequired);
                statData[5] += currFit.fitDelta;
            }

            SC_StatisticsArray chisqStats(6);
            for (i = 0; i < 6; i++)
                chisqStats[i].CalcStatistics(statData[i], false);

            SetChisqStatsTable(chisqStats, SC_Statistics::soMax);
            SetChisqStatsTable(chisqStats, SC_Statistics::soMin);
            SetChisqStatsTable(chisqStats, SC_Statistics::soMean);
            SetChisqStatsTable(chisqStats, SC_Statistics::soStdDev);
            AddNextLine();


            if (!allCasesOK)
            {
                AddLine("All Statistics");
                for (i = 0; i < 6; i++)
                    statData[i].SetEmpty();

                for (i = 0; i < ncurrCase; i++)
                {
                    OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];

                    statData[0] += currFit.fitValue;
                    statData[1] += currFit.sumSquaredError;
                    statData[2] += currFit.meanSquaredError;
                    statData[3] += currFit.derivSpan;
                    statData[4] += double(currFit.nsimsRequired);
                    statData[5] += currFit.fitDelta;
                }

                SC_StatisticsArray chisqStats(6);
                for (i = 0; i < 6; i++)
                    chisqStats[i].CalcStatistics(statData[i], false);

                SetChisqStatsTable(chisqStats, SC_Statistics::soMax);
                SetChisqStatsTable(chisqStats, SC_Statistics::soMin);
                SetChisqStatsTable(chisqStats, SC_Statistics::soMean);
                SetChisqStatsTable(chisqStats, SC_Statistics::soStdDev);
                AddNextLine();

            }
        }

        if (mosListAll)
        {
            for (int i = 0; i < ncurrCase; i++)
            {
                OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
                SetTableInt(0, currentCases[i] + 1, 3);
                SetChisqTable(currFit);
            }
            AddNextLine();
        }
    }
    else
    {
        SC_IntArray colWidths(5, 15);
        colWidths[0] = 6;
        colWidths[4] = 6;

        SetDefaultFormat(SC_SciFormat(5));

        TableSetup(colWidths, 3);
        SetTableCol(0, "CaseID");
        SetTableCol(1, "FitVal");
        SetTableCol(2, "#Sim");
        SetTableCol(3, "ParDelta");
        SetTableCol(4, "Status");

        AddNextLine();
        AddNextLine();

        if (mosListBest)
        {
            SetTableCol(0, "Best");
            SetNonChisqTable(currRun.runResults[singleFitCase].caseResults[fitIndex]);
            AddNextLine();
        }

        // stats
        if (mosListStatistics)
        {
            if (!allCasesOK)
                AddLine("OK Statistics");

            SC_DoubleMatrix statData;
            statData.MatrixAlloc(3, ncurrCase);

            int i;
            for (i = 0; i < ncurrCase; i++)
            {
                OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
                if (!currFit.fitCompletedOK)
                    continue;

                statData[0] += currFit.fitValue;
                statData[1] += double(currFit.nsimsRequired);
                statData[2] += currFit.fitDelta;
            }

            SC_StatisticsArray chisqStats(3);
            for (i = 0; i < 3; i++)
                chisqStats[i].CalcStatistics(statData[i], false);

            SetNonChisqStatsTable(chisqStats, SC_Statistics::soMax);
            SetNonChisqStatsTable(chisqStats, SC_Statistics::soMin);
            SetNonChisqStatsTable(chisqStats, SC_Statistics::soMean);
            SetNonChisqStatsTable(chisqStats, SC_Statistics::soStdDev);
            AddNextLine();

            if (!allCasesOK)
            {
                AddLine("All Statistics");

                for (i = 0; i < 3; i++)
                    statData[i].SetEmpty();

                for (i = 0; i < ncurrCase; i++)
                {
                    OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];

                    statData[0] += currFit.fitValue;
                    statData[1] += double(currFit.nsimsRequired);
                    statData[2] += currFit.fitDelta;
                }

                SC_StatisticsArray chisqStats(3);
                for (i = 0; i < 3; i++)
                    chisqStats[i].CalcStatistics(statData[i], false);

                SetNonChisqStatsTable(chisqStats, SC_Statistics::soMax);
                SetNonChisqStatsTable(chisqStats, SC_Statistics::soMin);
                SetNonChisqStatsTable(chisqStats, SC_Statistics::soMean);
                SetNonChisqStatsTable(chisqStats, SC_Statistics::soStdDev);
                AddNextLine();
            }

        }

        if (mosListAll)
        {
            for (int i = 0; i < ncurrCase; i++)
            {
                OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
                SetTableInt(0, currentCases[i] + 1, 3);
                SetNonChisqTable(currFit);
            }
            AddNextLine();
        }
    }


}


void LFO_OptRun::ListParameterEstimates(const OptSimRunResults& currRun,
                                                int             fitIndex)
{
    TextSubTitle("Fitted Parameter Value Data");

    SetDefaultFormat(SC_SciFormat(5));

    if (SingleFitOnly())
    {
        OptSingleFit& currFit = currRun.runResults[singleFitCase].caseResults[fitIndex];
        ListOptOK(currFit);

        for (int i = 0; i < runHeader.noptVar; i++)
            AddUnitConst(currRun.varDescriptions[i].varDesc, currFit.finalParData[i],
                            currRun.varDescriptions[i].varUnits);

        for (int i = 0; i < runHeader.ncalcVar; i++)
            AddUnitConst(currRun.calcDescriptions[i].caseVarDesc, currFit.calcParData[i],
                currRun.calcDescriptions[i].caseVarUnits);

        AddNextLine();
        return;
    }


    int nRows = 0;
    if (mosListBest)
        nRows += 2;
    if (mosListStatistics)
        nRows += 15;
    if (mosListAll)
        nRows += ncurrCase + 1;

    SC_StringArray  colTitles, colUnits, rowTitles;

    int nvar = runHeader.noptVar + runHeader.ncalcVar;

    colTitles.Alloc(nvar);
    colUnits.Alloc(nvar);
    rowTitles.Alloc(nRows);

    SC_DoubleMatrix tableData;
    tableData.MatrixAlloc(nvar, nRows);
    tableData.SetSize(nvar);

    for (int i = 0; i < runHeader.noptVar; i++)
    {
        colTitles += currRun.varDescriptions[i].shortVarDesc;
        colUnits += currRun.varDescriptions[i].varUnits;
    }
    for (int i = 0; i < runHeader.ncalcVar; i++)
    {
        colTitles += currRun.calcDescriptions[i].caseVarShortDesc;
        colUnits += currRun.calcDescriptions[i].caseVarUnits;
    }

    if (mosListBest)
    {
        OptSingleFit& currFit = currRun.runResults[singleFitCase].caseResults[fitIndex];
        rowTitles += "Best";
        rowTitles.nlistData++;

        int nextCol = 0;
        for (int i = 0; i < runHeader.noptVar; i++)
        {
            tableData[nextCol] += currFit.finalParData[i];
            tableData[nextCol] += nullReal;  // blank
            nextCol++;
        }
        for (int i = 0; i < runHeader.ncalcVar; i++)
        {
            tableData[nextCol] += currFit.calcParData[i];
            tableData[nextCol] += nullReal;  // blank
            nextCol++;
        }
    }

    if (mosListStatistics)
    {
        SC_DoubleMatrix statData;
        statData.MatrixAlloc(nvar, ncurrCase);

        for (int j = 0; j < ncurrCase; j++)
        {
            OptSingleFit& currFit = currRun.runResults[currentCases[j]].caseResults[fitIndex];
            if (currFit.fitCompletedOK)
            {
                int nextVar = 0;
                for (int i = 0; i < runHeader.noptVar; i++)
                    statData[nextVar++] += currFit.finalParData[i];
                for (int i = 0; i < runHeader.ncalcVar; i++)
                    statData[nextVar++] += currFit.calcParData[i];
            }
        }

        SC_StatisticsArray parStats(nvar);
        for (int i = 0; i < nvar; i++)
            parStats[i].CalcStatistics(statData[i], false);

        for (int i = 0; i < nvar; i++)
        {
            tableData[i] += parStats[i].realResults[SC_Statistics::soMax];
            tableData[i] += parStats[i].realResults[SC_Statistics::soMin];
            tableData[i] += parStats[i].realResults[SC_Statistics::soMean];
            tableData[i] += parStats[i].realResults[SC_Statistics::soStdDev];
            tableData[i] += nullReal;  // blank
        }


        if (!allCasesOK)
        {
            // titles for prev OK
            rowTitles += "OK Max";
            rowTitles += "OK Min";
            rowTitles += "OK Mean";
            rowTitles += "OK StdDev";
            rowTitles.nlistData++;

            for (int i = 0; i < nvar; i++)
                statData[i].SetEmpty();

            for (int j = 0; j < ncurrCase; j++)
            {
                OptSingleFit& currFit = currRun.runResults[currentCases[j]].caseResults[fitIndex];
                int nextVar = 0;
                for (int i = 0; i < runHeader.noptVar; i++)
                    statData[nextVar++] += currFit.finalParData[i];
                for (int i = 0; i < runHeader.ncalcVar; i++)
                    statData[nextVar++] += currFit.calcParData[i];
            }

            SC_StatisticsArray parStats(runHeader.noptVar);
            for (int i = 0; i < nvar; i++)
                parStats[i].CalcStatistics(statData[i], false);

            for (int i = 0; i < nvar; i++)
            {
                tableData[i] += parStats[i].realResults[SC_Statistics::soMax];
                tableData[i] += parStats[i].realResults[SC_Statistics::soMin];
                tableData[i] += parStats[i].realResults[SC_Statistics::soMean];
                tableData[i] += parStats[i].realResults[SC_Statistics::soStdDev];
                tableData[i] += nullReal;  // blank
            }

            rowTitles += "All Max";
            rowTitles += "All Min";
            rowTitles += "All Mean";
            rowTitles += "All StdDev";
            rowTitles.nlistData++;
        }
        else
        {
            rowTitles += "Max";
            rowTitles += "Min";
            rowTitles += "Mean";
            rowTitles += "StdDev";
            rowTitles.nlistData++;
        }

    }


    if (mosListAll)
    {
        for (int j = 0; j < ncurrCase; j++)
        {
            OptSingleFit& currFit = currRun.runResults[currentCases[j]].caseResults[fitIndex];

            char caseStr[40];
            IntToString(j + 1, caseStr, 3, 40);
            if (currFit.fitCompletedOK)
                ConcatString(caseStr, "/OK", 40);
            else
                ConcatString(caseStr, "/Bad", 40);

            rowTitles += caseStr;

            int nextVar = 0;
            for (int i = 0; i < runHeader.noptVar; i++)
                tableData[nextVar++] += currFit.finalParData[i];
            for (int i = 0; i < runHeader.ncalcVar; i++)
                tableData[nextVar++] += currFit.calcParData[i];
        }
    }


    ListMatrix("Run", colTitles, colUnits, rowTitles, tableData, 5, 10, 12);
    AddNextLine();
}



void LFO_OptRun::GetEstVarData(const OptSingleFit& currFit,
                                     int              fitIndex,
                                     int              fitCompIndex,
                                     FitVarData&      varData)
{
    if (fitCompIndex < 0)
    {
        varData.estVar = currFit.estimatedVariance;
        varData.sse = currFit.sumSquaredError;
        varData.npts = currFit.GetnFitPoints();
    }
    else
    {
        OptFitComponent& fitComp = currFit.fitComponents[fitCompIndex];
        varData.estVar = fitComp.fitEstimatedVariance;
        varData.sse = fitComp.fitSumSquaredErrors;
        varData.npts = fitComp.nfitPoints;
    }
    varData.stdDev = sqrt(varData.sse / double(varData.npts - runHeader.noptVar));
}

void LFO_OptRun::SetEstVarTable(const char* fitID,
                               const FitVarData& varData)
{
    SetTableCol(0, fitID);
    SetTableReal(1, varData.estVar);
    SetTableReal(2, varData.sse);
    SetTableReal(3, varData.stdDev);
    SetTableInt(4, varData.npts);
    AddNextLine();
}


void LFO_OptRun::EstVarTableSetup()
{
    SC_IntArray colWidths(5, 14);
    colWidths[0] = 6;
    colWidths[4] = 10;

    TableSetup(colWidths, 3);
    SetTableCol(0, "FitID");
    SetTableCol(1, "EstVar");
    SetTableCol(2, "SSE");
    SetTableCol(3, "StdDev");
    SetTableCol(4, "#pts");

    AddNextLine();
    AddNextLine();
}


void LFO_OptRun::SetVarEstStatsTable(const SC_StatisticsArray&  stats,
                                              SC_Statistics::StatisticOp op)
{
    FitVarData varData;

    varData.estVar = stats[0].realResults[op];
    varData.sse = stats[1].realResults[op];
    varData.stdDev = stats[2].realResults[op];
    varData.npts = int(stats[3].realResults[op]);

    SetEstVarTable(SC_Statistics::opIDs[op], varData);
}



void LFO_OptRun::ListEstimatedDataVariances(const OptSimRunResults& currRun,
                                                int             fitIndex)
{
    TextSubTitle("Estimated Variance Data");

    SetDefaultFormat(SC_SciFormat(5));

    FitVarData  varData;

    if (SingleFitOnly())
    {
        OptSingleFit& currFit = currRun.runResults[singleFitCase].caseResults[fitIndex];

        if (currFit.fitComponents.Size() == 1)
        {
            GetEstVarData(currFit, fitIndex, 0, varData);

            AddStdReal("Estimated variance", varData.estVar);
            AddStdReal("SSE of estimate", varData.sse);
            AddStdReal("Std. dev. of estimate", varData.stdDev);
            AddStdInt("# of points in fit", varData.npts);
        }
        else
        {
            EstVarTableSetup();

            GetEstVarData(currFit, fitIndex, -1, varData);
            SetEstVarTable("Full-fit", varData);

            for (int i = 0; i < currFit.fitComponents.Size(); i++)
            {
                GetEstVarData(currFit, fitIndex, i, varData);
                SetEstVarTable(currRun.GetSubFitID(fitIndex, i), varData);
            }
        }
        AddBlankLine();
        return;
    }


    // # of points based on best fit
    {
        OptSingleFit& currFit = currRun.runResults[singleFitCase].caseResults[fitIndex];
        GetEstVarData(currFit, fitIndex, -1, varData);
        AddStdInt("Full-fit # of points in fit", varData.npts);

        if (currFit.fitComponents.Size() > 1)
        {
            for (int i = 0; i < currFit.fitComponents.Size(); i++)
            {
                GetEstVarData(currFit, fitIndex, i, varData);
                char tempStr[80];
                MakeString(tempStr, "# of pts - ", currRun.GetSubFitID(fitIndex, i), 80);
                AddStdInt(tempStr, varData.npts);
            }
        }

        AddBlankLine();
    }


    EstVarTableSetup();


    int nFitComp = currRun.fitDescriptions[fitIndex].fitComponentDescription.Size();

    for (int i = -1; i < nFitComp; i++)
    {
        if (i < 0)
        {
            AddText(" ** Full - Fit **", 0);
        }
        else
        {
            char tempStr[80];
            MakeString(tempStr, " ** ", currRun.GetSubFitID(fitIndex, i), " ** ", 80);
            AddText(tempStr, 0);
        }
        AddNextLine();
        AddBlankLine();


        if (mosListBest)
        {
            OptSingleFit& currFit = currRun.runResults[singleFitCase].caseResults[fitIndex];
            GetEstVarData(currFit, fitIndex, i, varData);
            SetEstVarTable("BestFit" , varData);
            AddBlankLine();
        }

        if (mosListStatistics)
        {
            SC_DoubleMatrix statData;
            statData.MatrixAlloc(4, ncurrCase);

            int j;
            for (j = 0; j < ncurrCase; j++)
            {
                OptSingleFit& currFit = currRun.runResults[currentCases[j]].caseResults[fitIndex];
                if (currFit.fitCompletedOK)
                {
                    GetEstVarData(currFit, fitIndex, i, varData);
                    statData[0] += varData.estVar;
                    statData[1] += varData.sse;
                    statData[2] += varData.stdDev;
                    statData[3] += double(varData.npts);
                }
            }

            SC_StatisticsArray varStats(4);
            for (j = 0; j < 4; j++)
                varStats[j].CalcStatistics(statData[j], false);

            SetVarEstStatsTable(varStats, SC_Statistics::soMax);
            SetVarEstStatsTable(varStats, SC_Statistics::soMin);
            SetVarEstStatsTable(varStats, SC_Statistics::soMean);
            SetVarEstStatsTable(varStats, SC_Statistics::soStdDev);

            AddBlankLine();
        }

        if (mosListAll)
        {
            for (int j = 0; j < ncurrCase; j++)
            {
                OptSingleFit& currFit = currRun.runResults[currentCases[j]].caseResults[fitIndex];
                if (currFit.fitCompletedOK)
                {
                    char caseStr[40];
                    IntToString(j + 1, caseStr, 3, 40);
                    GetEstVarData(currFit, fitIndex, i, varData);
                    SetEstVarTable(caseStr , varData);
                }
            }
            AddBlankLine();
        }

        if (nFitComp == 1)
            break;

    }
}


void LFO_OptRun::GetCorrelationData(const OptSimRunResults& currRun,
                                          int            fitIndex,
                                          int            parIndex,
                                          SC_DoubleArray& corrData)
{
    corrData.Alloc(ncurrCase);
    if (parIndex < runHeader.noptVar)
    {
        for (int i = 0; i < ncurrCase; i++)
        {
            OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
            if (currFit.fitCompletedOK)
                corrData += currFit.finalParData[parIndex];
        }

        return;
    }
    parIndex -= runHeader.noptVar;

    if (parIndex < runHeader.ncalcVar)
    {
        for (int i = 0; i < ncurrCase; i++)
        {
            OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
            if (currFit.fitCompletedOK)
                corrData += currFit.calcParData[parIndex];
        }
        return;
    }
    parIndex -= runHeader.ncalcVar;

    if (runHeader.IsSample() && (parIndex < currRun.caseDescriptions.Size()))
    {
        for (int i = 0; i < ncurrCase; i++)
        {
            OptCaseResults& currCase = currRun.runResults[currentCases[i]];
            if (currCase.caseResults[fitIndex].fitCompletedOK)
                corrData += currCase.caseParVals[parIndex];
        }
        return;
    }

    for (int i = 0; i < ncurrCase; i++)
    {
        OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
        if (currFit.fitCompletedOK)
            corrData += currFit.fitValue;
    }
}




void LFO_OptRun::ListParameterCorrelations(const OptSimRunResults& currRun,
                                            int             fitIndex)
{
    TextSubTitle("Parameter Correlations");

    if (ncurrCase == 1)
    {
        AddLine("No correlations for single case");
        return;
    }

    SetDefaultFormat(SC_DecFormat(3));

    int ncorrPar = runHeader.noptVar + runHeader.ncalcVar + 1;
    if (runHeader.IsSample())
        ncorrPar += currRun.caseDescriptions.Size();

    SC_DoubleMatrix pearsonData, spearmanData;
    pearsonData.MatrixAllocAndSetSize(ncorrPar, ncorrPar);
    spearmanData.MatrixAllocAndSetSize(ncorrPar, ncorrPar);

    SC_StringArray titles;
    titles.Alloc(ncorrPar);

    for (int i = 0; i < runHeader.noptVar; i++)
        titles += currRun.varDescriptions[i].shortVarDesc;
    for (int i = 0; i < runHeader.ncalcVar; i++)
        titles += currRun.calcDescriptions[i].caseVarShortDesc;

    if (runHeader.IsSample())
    {
        for (int i = 0; i < currRun.caseDescriptions.Size(); i++)
            titles += currRun.caseDescriptions[i].caseVarShortDesc;
    }
    titles += "FitValue";


    SC_DoubleArray corrX, corrY;

    for (int i = 0; i < ncorrPar; i++)
    {
        GetCorrelationData(currRun, fitIndex, i, corrX);
        for (int j = i + 1; j < ncorrPar; j++)
        {
            GetCorrelationData(currRun, fitIndex, j, corrY);
            pearsonData[j][i] = PearsonR(corrX, corrY);
            spearmanData[j][i] = SpearmanR(corrX, corrY);
        }
        pearsonData[i][i] = 1.0;
        spearmanData[i][i] = 1.0;
    }

    ListUtriMatrix("PearsonR", titles, pearsonData, 7, 10, 8);
    AddBlankLine();

    ListUtriMatrix("SpearmanR", titles, spearmanData, 7, 10, 8);
}


void LFO_OptRun::SetCovar(const OptSimRunResults& currRun,
                          const OptSingleFit&     currFit,
                                int          covarIndex,
                                bool         denorm)
{
    covarData.SetCovar(currRun, currFit, covarIndex);
    if (denorm)
        covarData.DeNormalize();
}



void LFO_OptRun::SetCovarStats(const  StatsMatrix&      statsMatrix,
                                const char*             fitID,
                                const char*             subFitID,
                                      int               index,
                                      SC_Statistics::StatisticOp op)
{
    for (int j = 0; j < runHeader.noptVar; j++)
        for (int k = j; k < runHeader.noptVar; k++)
            covarData.covarMatrix[j][k] = statsMatrix[j][k].realResults[op];

    covarData.SetID(NULL, SC_Statistics::opIDs[op], fitID, subFitID, index);
    ListCovar(covarData);
}

void LFO_OptRun::ListCovarStats(const OptSimRunResults& currRun,
                                      int               fitIndex,
                                const char*             fitID,
                                const char*             subFitID,
                                      int               covarIndex,
                                      bool              denorm)
{
    StatsMatrix statsMatrix;
    statsMatrix.AllocAndSetSize(runHeader.noptVar);
    int i, j, k;
    for (i = 0; i < runHeader.noptVar; i++)
        statsMatrix[i].AllocAndSetSize(runHeader.noptVar);

    SC_DoubleCube statsData;
    statsData.CubeAlloc(runHeader.noptVar, runHeader.noptVar, ncurrCase);

    for (i = 0; i < ncurrCase; i++)
    {
        OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
        if (currFit.fitCompletedOK)
        {
            SetCovar(currRun, currFit, covarIndex, denorm);
            for (j = 0; j < runHeader.noptVar; j++)
                for (k = j; k < runHeader.noptVar; k++)
                    statsData[j][k] += covarData.covarMatrix[j][k];
        }
    }

    for (j = 0; j < runHeader.noptVar; j++)
        for (k = j; k < runHeader.noptVar; k++)
            statsMatrix[j][k].CalcStatistics(statsData[j][k], false);

    SetCovarStats(statsMatrix, fitID, subFitID, covarIndex, SC_Statistics::soMax);
    SetCovarStats(statsMatrix, fitID, subFitID, covarIndex, SC_Statistics::soMin);
    SetCovarStats(statsMatrix, fitID, subFitID, covarIndex, SC_Statistics::soMean);
    SetCovarStats(statsMatrix, fitID, subFitID, covarIndex,  SC_Statistics::soStdDev);
}


void LFO_OptRun::ListAllCovar(const OptSimRunResults&   currRun,
                                    int                 fitIndex,
                                const char*             fitID,
                                const char*             subFitID,
                                      int               covarIndex,
                                      bool              denorm)
{
    for (int i = 0; i < ncurrCase; i++)
    {
        OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
        if (currFit.fitCompletedOK)
        {
            SetCovar(currRun, currFit, covarIndex, denorm);

            char caseID[20];
            IntToString(currentCases[i] + 1, caseID, 3, 20);

            covarData.SetID(NULL, caseID, fitID, subFitID, covarIndex);

            ListCovar(covarData);
        }
    }
}


void LFO_OptRun::ListCovarianceMatrices(const OptSimRunResults& currRun,
                                            int             fitIndex,
                                            bool            denorm)
{
    if (denorm)
        TextSubTitle("Denormalized Covariance Matrices");
    else
        TextSubTitle("Covariance Matrices");

    OptFitComponentDescriptionArray& fitDescs = currRun.fitDescriptions[fitIndex].fitComponentDescription;
    const char* fitID = currRun.GetFitID(fitIndex);

    if (runHeader.IsSingle() || mosListBest)
    {
        OptSingleFit& currFit = currRun.runResults[singleFitCase].caseResults[fitIndex];

        const char* caseID = "Best";
        if (runHeader.IsSingle())
            caseID = "Only";

        if (csActual)
        {
            SetCovar(currRun, currFit, -1, denorm);
            covarData.SetActID(NULL, caseID, fitID);
            ListCovar(covarData);
        }
        if (csEstimated)
        {
            SetCovar(currRun, currFit, 0, denorm);
            covarData.SetEstID(NULL, caseID, fitID);
            ListCovar(covarData);
        }

        if (csFitComponent)
        {
            for (int i = 0; i < fitDescs.Size(); i++)
            {
                SetCovar(currRun, currFit, i + 1, denorm);
                covarData.SetSubFitID(NULL, caseID, fitID, fitDescs[i].fitID);
                ListCovar(covarData);
            }
        }

        if (runHeader.IsSingle())
            return;
    }

    if (mosListStatistics)
    {
        if (csActual)
            ListCovarStats(currRun, fitIndex, fitID, NULL, -1, denorm);

        if (csEstimated)
            ListCovarStats(currRun, fitIndex, fitID, NULL, 0, denorm);

        if (csFitComponent)
        {
            for (int i = 0; i < fitDescs.Size(); i++)
                ListCovarStats(currRun, fitIndex, fitID, fitDescs[i].fitID, i + 1, denorm);
        }
    }

    if (mosListAll)
    {
        if (csActual)
            ListAllCovar(currRun, fitIndex, fitID, NULL, -1, denorm);

        if (csEstimated)
            ListAllCovar(currRun, fitIndex, fitID, NULL, 0, denorm);

        if (csFitComponent)
        {
            for (int i = 0; i < fitDescs.Size(); i++)
                ListAllCovar(currRun, fitIndex, fitID, fitDescs[i].fitID, i + 1, denorm);
        }
    }


}


void LFO_OptRun::ListAll95(const OptSimRunResults&  currRun,
                                    int             fitIndex,
                                const char*             covarID,
                                      int               covarIndex)
{
    for (int i = 0; i < ncurrCase; i++)
    {
        OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
        if (currFit.fitCompletedOK)
        {
            SetCovar(currRun, currFit, covarIndex, false);

            char caseID[20];
            IntToString(currentCases[i] + 1, caseID, 3, 20);

            List95(covarID, caseID, covarData);
        }
    }
}


void LFO_OptRun::List95ConfidenceLimits(const OptSimRunResults& currRun,
                                            int             fitIndex)
{
    TextSubTitle("95% Confidence Intervals");

    OptFitComponentDescriptionArray& fitDescs = currRun.fitDescriptions[fitIndex].fitComponentDescription;

    SetList95();

    if (runHeader.IsSingle() || mosListBest)
    {
        OptSingleFit& currFit = currRun.runResults[singleFitCase].caseResults[fitIndex];

        const char* runID = "Best";
        if (runHeader.IsSingle())
            runID = "Only";

        if (csActual)
        {
            SetCovar(currRun, currFit, -1, false);
            List95("Act", runID, covarData);
        }
        if (csEstimated)
        {
            SetCovar(currRun, currFit, 0, false);
            List95("Est", runID, covarData);
        }

        if (csFitComponent)
        {
            for (int i = 0; i < fitDescs.Size(); i++)
            {
                SetCovar(currRun, currFit, i + 1, false);
                List95(fitDescs[i].fitID, runID, covarData);
            }
        }

        if (runHeader.IsSingle())
            return;
    }


    if (mosListAll)
    {
        if (csActual)
            ListAll95(currRun, fitIndex, "Act", -1);

        if (csEstimated)
            ListAll95(currRun, fitIndex, "Est", 0);

        if (csFitComponent)
        {
            for (int i = 0; i < fitDescs.Size(); i++)
                ListAll95(currRun, fitIndex, fitDescs[i].fitID, i + 1);
        }
    }

}


void LFO_OptRun::ListJac(const OptSimRunResults&    currRun,
                               int              caseIndex,
                               int              fitIndex,
                               const char*      runID)
{
    AddText(runID, 0);
    AddNextLine();
    if (jacobData.ExtractFrom(currRun, singleFitCase, fitIndex))
        LFO_OptBase::ListJacobian(jacobData);
    else
    {
        AddText("Jacobian data bad", 0);
        AddNextLine();
    }
}

void LFO_OptRun::ListJacobian(const OptSimRunResults& currRun,
                                            int             fitIndex)
{
    TextSubTitle("Jacobian Analyses");

    if (runHeader.IsSingle() || mosListBest)
    {
        const char* runID = "Best Fit";
        if (runHeader.IsSingle())
            runID = "Only Run";
        ListJac(currRun, singleFitCase, fitIndex, runID);

        if (runHeader.IsSingle())
            return;
    }


    if (mosListAll)
    {
        for (int i = 0; i < ncurrCase; i++)
        {
            OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
            if (currFit.fitCompletedOK)
            {
                char caseID[20];
                IntToString(currentCases[i] + 1, caseID, 3, 20);
                ListJac(currRun, i, fitIndex, caseID);
            }
        }
    }
}

void LFO_OptRun::ListAllCovarCorr(const OptSimRunResults&   currRun,
                                        int             fitIndex,
                                        const char*     fitID,
                                        const char*     subFitID,
                                        int             covarIndex)
{
    for (int i = 0; i < ncurrCase; i++)
    {
        OptSingleFit& currFit = currRun.runResults[currentCases[i]].caseResults[fitIndex];
        if (currFit.fitCompletedOK)
        {
            SetCovar(currRun, currFit, covarIndex, false);

            char caseID[20];
            IntToString(currentCases[i] + 1, caseID, 3, 20);
            covarData.SetID(NULL, caseID, fitID, subFitID, covarIndex);

            ListCovarCorr(covarData);
        }
    }
}


void LFO_OptRun::ListCovarianceMatrixCorrelations(const OptSimRunResults& currRun,
                                                        int             fitIndex)
{
    TextSubTitle("Covariance Correlation Matrices");

    OptFitComponentDescriptionArray& fitDescs = currRun.fitDescriptions[fitIndex].fitComponentDescription;
    const char* fitID = currRun.GetFitID(fitIndex);

    if (runHeader.IsSingle() || mosListBest)
    {
        OptSingleFit& currFit = currRun.runResults[singleFitCase].caseResults[fitIndex];

        const char* caseID = "Best";
        if (runHeader.IsSingle())
            caseID = "Only";


        if (csActual)
        {
            SetCovar(currRun, currFit, -1, false);
            covarData.SetID(NULL, caseID, fitID, NULL, -1);

            ListCovarCorr(covarData);
        }
        if (csEstimated)
        {
            SetCovar(currRun, currFit, 0, false);
            covarData.SetID(NULL, caseID, fitID, NULL, 0);

            ListCovarCorr(covarData);
        }

        if (csFitComponent)
        {
            for (int i = 0; i < fitDescs.Size(); i++)
            {
                SetCovar(currRun, currFit, i + 1, false);
                covarData.SetID(NULL, caseID, fitID, fitDescs[i].fitID, i + 1);
                ListCovarCorr(covarData);
            }
        }

        if (runHeader.IsSingle())
            return;
    }


    if (mosListAll)
    {
        if (csActual)
            ListAllCovarCorr(currRun, fitIndex, fitID, NULL, -1);

        if (csEstimated)
            ListAllCovarCorr(currRun, fitIndex, fitID, NULL, 0);

        if (csFitComponent)
        {
            for (int i = 0; i < fitDescs.Size(); i++)
                ListAllCovarCorr(currRun, fitIndex, fitID, fitDescs[i].fitID, i + 1);
        }
    }


}



void LFO_OptRun::CreateListing()
{
    OptSimRunResults* nextRun = selectedOpts[0];
    int nextIndex = 0;
    bool idListed = false;
    while (nextRun && (nextIndex < selectedOpts.Size()))
    {
        OptSimRunResults& currRun = *nextRun;

        while (nextIndex < selectedOpts.Size())
        {
            if (selectedOpts[nextIndex] != nextRun)
            {
                nextRun = selectedOpts[nextIndex++];
                break;
            }
            nextIndex++;
        }

        // for each fit
        for (int i = 0; i < currRun.GetnFits(); i++)
        {
            // collect cases for fit/run combo
            currentCases.Alloc(currRun.GetnCases());
            int j;
            for (j = 0; j < selectedOpts.Size(); j++)
                if ((selectedOpts[j] == &currRun) && (selectedFit[j] == i))
                    currentCases += selectedCases[j];

            if (currentCases.IsEmpty())
                continue;

            ncurrCase = currentCases.Size();
            bool singleCase = (ncurrCase == 1);

            char caseStr[80];
            IntToString(ncurrCase, caseStr, 80);
            switch (runHeader.runType)
            {
                case OptSimResultsHeader::rtSingle : {
                    CopyString(caseStr, "Single Run", 80);
                    break;
                }
                case OptSimResultsHeader::rtPerturb : {
                    if (singleCase)
                        CopyString(caseStr, "Pert#", 80);
                    else
                        ConcatString(caseStr, " Perturbations", 80);
                    break;
                }
                case OptSimResultsHeader::rtMultiple : {
                    if (singleCase)
                        CopyString(caseStr, "Case#", 80);
                    else
                        ConcatString(caseStr, " Cases", 80);
                    break;
                }
                case OptSimResultsHeader::rtSample : {
                    if (singleCase)
                        CopyString(caseStr, "Sample#", 80);
                    else
                        ConcatString(caseStr, " Samples", 80);
                    break;
                }
            }

            int currCaseIndex = currentCases[0];
            if (singleCase)
                ConcatInt(currCaseIndex + 1, caseStr, 80);


            int currFitIndex = i;

            char titleStr[80];
            if (idListed)
                MakeString(titleStr, currRun.runID,"/", currRun.GetFitID(currFitIndex), " : ", caseStr, 80);
            else
                MakeString(titleStr, GetID(), ":", currRun.runID,"/", currRun.GetFitID(currFitIndex), " : ", caseStr, 80);
            idListed = true;

            TextStdTitle(titleStr);

            singleFitCase = -1;
            double bestFitValue;

            allCasesOK = true;
            bool okFound = false;

            for (j = 0; j < ncurrCase; j++)
            {
                OptSingleFit& currFit = currRun.runResults[currentCases[j]].caseResults[currFitIndex];

                if (currFit.fitCompletedOK)
                    okFound = true;
                else
                    allCasesOK = false;

                if ((singleFitCase < 0) || (currFit.fitValue < bestFitValue))
                {
                    singleFitCase = currentCases[j];
                    bestFitValue = currFit.fitValue;
                }
            }


            badOptMsg = false;
            if (!okFound)
            {
                if (ncurrCase > 1)
                    AddLine("No successful optimizations in list set");
            }

            if (listRunSummary)
                ListRunSummary(currRun, currFitIndex);

            if (listFitValues)
                ListFitValues(currRun, currFitIndex);

            if (listParameterEstimates)
                ListParameterEstimates(currRun, currFitIndex);

            bool covarOK = okFound && runHeader.containsCovariance;
            if (covarOK && listEstimatedDataVariances)
                ListEstimatedDataVariances(currRun, currFitIndex);

            if (covarOK && listParameterCorrelations)
                ListParameterCorrelations(currRun, currFitIndex);

            if (covarOK && listNormalizedCovarianceMatrices)
                ListCovarianceMatrices(currRun, currFitIndex, false);

            if (covarOK && listDenormalizedCovarianceMatrices)
                ListCovarianceMatrices(currRun, currFitIndex, true);

            if (covarOK && list95ConfidenceLimits)
                List95ConfidenceLimits(currRun, currFitIndex);

            bool jacobOK = okFound && runHeader.containsJacobian;
            if (jacobOK && listJacobian)
                ListJacobian(currRun, currFitIndex);

            if (covarOK && listCovarianceMatrixCorrelations)
                ListCovarianceMatrixCorrelations(currRun, currFitIndex);
        }
    }
}

