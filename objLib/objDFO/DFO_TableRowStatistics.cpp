///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableRowStatistics.cpp
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
//      Calculates statistics for each row in a table.  Produces a new table
//  containing columns with each statistical measure selected.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genClass/DC_XYCDF.h>
#include <genClass/SC_Statistics.h>

#include <objDFO/DFO_TableRowStatistics.h>


DFO_TableRowStatistics :: DFO_TableRowStatistics() : FuncObjC("Table Row Statistics")
{
    skipXColumn = true;
    xcolumnIndex = 0;

    outputAllStats = false;
    outputConfidenceLimits = false;
    outputMedian = false;
    outputCDF99 = false;
    outputCDF95 = false;
    outputCDF90 = false;

    quantileType = DC_XYCDF::qtWeibull;
    genericQuantileValue = 0.5;

    InitCommon();
}

DFO_TableRowStatistics::DFO_TableRowStatistics(const DFO_TableRowStatistics& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}


void DFO_TableRowStatistics::InitCommon()
{
    statisticsTableDO.tableData = &statisticsTableDC;
    AddOutPort(statisticsTableDO);

    AddInPort(inputTableObjRef,   typeid(DO_TableData));

    DoStatusChk();
}

DFO_TableRowStatistics& DFO_TableRowStatistics::operator= (const DFO_TableRowStatistics& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableRowStatistics::LocalCopy(const DFO_TableRowStatistics& a)
{
    inputTableObjRef    = a.inputTableObjRef;
    skipXColumn         = a.skipXColumn;
    xcolumnIndex        = a.xcolumnIndex;

    outputAllStats          = a.outputAllStats;
    outputConfidenceLimits  = a.outputConfidenceLimits;
    outputMedian            = a.outputMedian;

    outputCDF99             = a.outputCDF99;
    outputCDF95             = a.outputCDF95;
    outputCDF90             = a.outputCDF90;

    quantileType            = a.quantileType;
    genericQuantileValue    = a.genericQuantileValue;
}


void  DFO_TableRowStatistics:: DoStatusChk()
{
    statisticsTableDC.Alloc(0, 0);
    FuncObjC::DoStatusChk();

    //  get data
    if (!CheckInputFO(inputTableObjRef, "Input table"))
        return;

    if (StatusNotOK())
        return;

    inputTableDC = static_cast<DO_TableData*>(GetInPortData(inputTableObjRef))->tableData;

    nrows = inputTableDC->GetNRows();
    ncols = inputTableDC->GetNCols();

    if ((nrows == 0) || (ncols == 0))
        SetObjErrMsg("no rows or columns in input table");

    if (skipXColumn && (xcolumnIndex >= ncols))
        SetObjErrMsg("X column index out of range");

    if (skipXColumn && (ncols == 1))
        SetObjErrMsg("no stats columns after X column is removed");


    bool doCDF = outputCDF99 || outputCDF95 || outputCDF90;
    if (doCDF)
    {
        if (!DC_XYCDF::QuantileOK(quantileType, genericQuantileValue))
        {
            SetObjErrMsg("quantile must be between 0 and 1");
        }
    }
}


void DFO_TableRowStatistics:: CalcOutput(FOcalcType  calcType)
{
    statisticsTableDC.ClearAll();
    DoStatusChk();
    if (StatusNotOK())
        return;

    const int nallStats = SC_Statistics::maxNormStatisticType - 2;  // except med & mode

    int noutCols = 3;   // mean, min, max
    if (outputAllStats)
        noutCols = nallStats;

    if (outputConfidenceLimits)
        noutCols += 2;

    if (outputMedian)
        noutCols += 2;  // med & mode

    if (outputCDF99)
        noutCols += 2;
    if (outputCDF95)
        noutCols += 2;
    if (outputCDF90)
        noutCols += 2;

    if (skipXColumn)
        noutCols++;

    if (!statisticsTableDC.Alloc(noutCols, nrows, 60))
    {
        SetObjErrMsg("Table allocation problem");
        return;
    }

    if (skipXColumn)
        statisticsTableDC.columnDesc += inputTableDC->GetTitle(xcolumnIndex);

    if (outputAllStats)
    {
        for (int i = 0; i < nallStats; i++)
            statisticsTableDC.columnDesc += SC_Statistics::opIDs[i];
    }
    else
    {
        statisticsTableDC.columnDesc += "Mean";
        statisticsTableDC.columnDesc += "Min";
        statisticsTableDC.columnDesc += "Max";
    }

    if (outputConfidenceLimits)
    {
        statisticsTableDC.columnDesc += "Upper95";
        statisticsTableDC.columnDesc += "Lower95";
    }



    if (outputMedian)
    {
        statisticsTableDC.columnDesc += "Median";
        statisticsTableDC.columnDesc += "Mode";
    }

    if (outputCDF99)
    {
        statisticsTableDC.columnDesc += "CDF99";
        statisticsTableDC.columnDesc += "CDF01";
    }

    if (outputCDF95)
    {
        statisticsTableDC.columnDesc += "CDF95";
        statisticsTableDC.columnDesc += "CDF05";
    }

    if (outputCDF90)
    {
        statisticsTableDC.columnDesc += "CDF90";
        statisticsTableDC.columnDesc += "CDF10";
    }

    SC_Statistics stats;
    DC_XYCDF cdfData;

    SC_DoubleArray& currData = cdfData.xData;
    currData.Alloc(ncols);

    bool doCDF = outputCDF99 || outputCDF95 || outputCDF90;
    if (doCDF)
        cdfData.yData.AllocAndSetSize(ncols);

    for (int i = 0; i < nrows; i++)
    {
        currData.SetEmpty();

        for (int j = 0; j < ncols; j++)
        {
            if (skipXColumn && (xcolumnIndex == j))
                continue;
            currData += inputTableDC->dataTable[j][i];
        }

        stats.CalcStatistics(currData);

        int nextCol = 0;

        if (skipXColumn)
        {
            statisticsTableDC.dataTable[nextCol++] += inputTableDC->dataTable[xcolumnIndex][i];
        }

        if (outputAllStats)
        {
            for (int k = 0; k < nallStats; k++)
                statisticsTableDC.dataTable[nextCol++] += stats.realResults[k];
        }
        else
        {
            statisticsTableDC.dataTable[nextCol++] += stats.realResults[SC_Statistics::soMean];
            statisticsTableDC.dataTable[nextCol++] += stats.realResults[SC_Statistics::soMin];
            statisticsTableDC.dataTable[nextCol++] += stats.realResults[SC_Statistics::soMax];
        }

        if (outputConfidenceLimits)
        {
            double mean = stats.realResults[SC_Statistics::soMean];
            double stddev = stats.realResults[SC_Statistics::soStdDev];

            statisticsTableDC.dataTable[nextCol++] += mean + 1.96 * stddev;
            statisticsTableDC.dataTable[nextCol++] += mean - 1.96 * stddev;
        }

        if (outputMedian)
        {
            stats.CalcMedian(currData);
            stats.CalcMode(currData);
            statisticsTableDC.dataTable[nextCol++] += stats.realResults[SC_Statistics::soMedian];
            statisticsTableDC.dataTable[nextCol++] += stats.realResults[SC_Statistics::soMode];
        }

        if (doCDF)
        {
            // sort if not done already for median
            if (!outputMedian)
            {
                currData.Cleanup();
                currData.Sort(true);
            }
            int N = currData.Size();
            if (N < 2)
            {
                if (outputCDF99)
                {
                    statisticsTableDC.dataTable[nextCol++] += nullReal;
                    statisticsTableDC.dataTable[nextCol++] += nullReal;
                }
                if (outputCDF95)
                {
                    statisticsTableDC.dataTable[nextCol++] += nullReal;
                    statisticsTableDC.dataTable[nextCol++] += nullReal;
                }
                if (outputCDF90)
                {
                    statisticsTableDC.dataTable[nextCol++] += nullReal;
                    statisticsTableDC.dataTable[nextCol++] += nullReal;
                }
                continue;
            }

            cdfData.SetSize(N);
            cdfData.SetCDFQuantile(quantileType, genericQuantileValue);

            cdfData.SetValid();  // assume OK - no log processing
            if (outputCDF99)
            {
                statisticsTableDC.dataTable[nextCol++] += cdfData.GetDataVal(0.99, false);
                statisticsTableDC.dataTable[nextCol++] += cdfData.GetDataVal(0.01, false);
            }

            if (outputCDF95)
            {
                statisticsTableDC.dataTable[nextCol++] += cdfData.GetDataVal(0.95, false);
                statisticsTableDC.dataTable[nextCol++] += cdfData.GetDataVal(0.05, false);
            }

            if (outputCDF90)
            {
                statisticsTableDC.dataTable[nextCol++] += cdfData.GetDataVal(0.90, false);
                statisticsTableDC.dataTable[nextCol++] += cdfData.GetDataVal(0.10, false);
            }

        }
    }

    if (inputTableDC->rowDesc.IsNotEmpty())
        statisticsTableDC.rowDesc = inputTableDC->rowDesc;
    else
        statisticsTableDC.rowDesc.SetIndexed("STAT",0);

    statisticsTableDC.SetID("Row statistics");
    statisticsTableDC.dataTable.SetSize(noutCols);
}


