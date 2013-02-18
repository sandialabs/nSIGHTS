///////////////////////////////////////////////////////////////////////////////////
//
// LFO_TableStatistics.cpp
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

#include <genClass/SC_Statistics.h>

#include <genClass/DO_TableData.h>
#include <genDFO/DFO_Statistics.h>

#include <objLFO/LFO_TableStatistics.h>

LFO_TableStatistics :: LFO_TableStatistics(ListDefC& assLst)
    : ListObjC("List Table Statistics", assLst)
{
    listTableID         = false;
    listColumnHeaders   = true;
    logData             = false;
    tableColWidth       = 20;
    maxListWidth        = 128;
    tableDataFormat     = SC_SciFormat(5);
    useLog10            = false;

    presentTransformed = DFO_Statistics::defaultPresentTransformed;

    InitCommon();
}

LFO_TableStatistics::LFO_TableStatistics(const LFO_TableStatistics& a, ListDefC& assLst) : ListObjC(a, assLst)
{
    LocalCopy(a);
    InitCommon();
}

LFO_TableStatistics :: ~LFO_TableStatistics ()
{
}

void LFO_TableStatistics::InitCommon()
{
    AddInPort(tableObjRef, typeid(DO_TableData));
    DoStatusChk();
}

LFO_TableStatistics& LFO_TableStatistics::operator= (const LFO_TableStatistics& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void LFO_TableStatistics::LocalCopy(const LFO_TableStatistics& a)
{
    tableObjRef         = a.tableObjRef;
    listTableID         = a.listTableID;
    listColumnHeaders   = a.listColumnHeaders;
    logData             = a.logData;
    tableColWidth       = a.tableColWidth;
    maxListWidth        = a.maxListWidth;
    tableDataFormat     = a.tableDataFormat;
    useLog10            = a.useLog10;
    presentTransformed  = a.presentTransformed;
}


void LFO_TableStatistics::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(tableObjRef, "Table");
}


void LFO_TableStatistics::CreateListing()
{
    const DC_TableData* inTableDC = static_cast<DO_TableData*>(GetInPortData(tableObjRef))->tableData;

    if (tableColWidth > listingLineLen)
        tableColWidth = listingLineLen - 1;

    char tempStr[80];
    if (listTableID)
    {
        MakeString(tempStr, "Table: ", inTableDC->GetID(), 80);
        TextStdTitle(tempStr);
    }

    if (logData)
    {
        if (useLog10)
            AddLine("Statistics calculated on log10 data");
        else
            AddLine("Statistics calculated on natural log data");
        if (presentTransformed)
        {
            AddLine("Min, Max, Mean, Median, Mode and Range are presented as log data");
        }
        AddBlankLine();
    }

    int maxPageCols = maxListWidth / tableColWidth;
    if (maxPageCols < 2)
    {
        maxPageCols = 2;
        maxListWidth = 50;
        tableColWidth = 20;
    }

    SC_IntArray colWidth(maxPageCols, tableColWidth);
    SC_BoolArray leftJust(maxPageCols, false);
    leftJust[0] = true;

    int dataColStart = 1;
    colWidth[0] = 15;
    TableSetup(colWidth, leftJust, 0);

    int stCol = 0;
    int maxTableCols = inTableDC->GetNCols();

    SC_StatisticsArray tableStats(maxTableCols);
    for (int i = 0; i < maxTableCols; i++)
    {
        SC_Statistics& currStat = tableStats[i];
        SC_DoubleArray colData(inTableDC->dataTable[i]);

        double medianVal = nullReal;
        double modeVal = nullReal;
        if (logData)
        {
            // copy and take log if necessary
            SC_DoubleArray tmpData(colData);
            if (useLog10)
                tmpData.Log10();
            else
                tmpData.Log();

            // med & mode need stats on log
            currStat.CalcStatistics(tmpData);
            currStat.CalcMedian(tmpData);
            currStat.CalcMode(tmpData);

            medianVal = currStat.realResults[SC_Statistics::soMedian];
            modeVal = currStat.realResults[SC_Statistics::soMode];
        }

        currStat.CalcStatistics(colData, logData, useLog10);
        if (!logData)
        {
            currStat.CalcMode(colData);
            currStat.CalcMedian(colData);
        }
        else
        {
            // reset med & mode
            currStat.realResults[SC_Statistics::soMedian] = medianVal;
            currStat.realResults[SC_Statistics::soMode] = modeVal;
        }

        UnLogIfNecessary(currStat, SC_Statistics::soMin);
        UnLogIfNecessary(currStat, SC_Statistics::soMax);
        UnLogIfNecessary(currStat, SC_Statistics::soMean);
        UnLogIfNecessary(currStat, SC_Statistics::soMedian);
        UnLogIfNecessary(currStat, SC_Statistics::soMode);
        currStat.realResults[SC_Statistics::soRange] = currStat.realResults[SC_Statistics::soMax] - currStat.realResults[SC_Statistics::soMin];
    }


    while (stCol < maxTableCols)
    {
        int ncolsWrite = maxPageCols - dataColStart;
        if (stCol + ncolsWrite > maxTableCols)
            ncolsWrite = maxTableCols - stCol;

        if (listColumnHeaders)
        {
            for (int j = 0; j < ncolsWrite; j++)
                SetTableCol(j + dataColStart, inTableDC->GetTitle(stCol + j));
            AddNextLine();
            AddBlankLine();
        }

        for (int i = 0; i <  SC_Statistics::maxStatisticType; i++)
        {
            if (logData && (i == SC_Statistics::soMode))
                continue;

            SetTableCol(0, SC_Statistics::opIDs[i]);

            for (int j = 0; j < ncolsWrite; j++)
            {
                double statVal = tableStats[stCol + j].realResults[i];

                if (!tableStats[stCol + j].resultsOK)
                {
                    SetTableCol(j + dataColStart, "n/a");
                }
                else if(RealIsNull(statVal)) {
                    SetTableCol(j + dataColStart, "--");
                }
                else if(SC_Statistics::IsInteger(i)) {
                    SetTableReal(j + dataColStart, statVal, SC_DecFormat(0));
                }
                else if (i == SC_Statistics::soPercentNonNull) {
                    SetTableReal(j + dataColStart, statVal, SC_DecFormat(3));
                }
                else {
                    SetTableReal(j + dataColStart, statVal, tableDataFormat);
                }
            }
            AddNextLine();
        }

        stCol +=  ncolsWrite;
        AddBlankLine();
    }

}

void LFO_TableStatistics::UnLogIfNecessary(SC_Statistics& data, SC_Statistics::StatisticOp valueOp)
{
    if ((!logData) || presentTransformed)
        return;

    double& dataRef = data.realResults[valueOp];

    if (useLog10)
    {
        dataRef = InvLgt(dataRef);
    }
    else
    {
        dataRef = exp(dataRef);
    }
}


