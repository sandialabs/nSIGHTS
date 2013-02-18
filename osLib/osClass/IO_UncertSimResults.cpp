///////////////////////////////////////////////////////////////////////////////////
//
// IO_UncertSimResults.cpp
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
//      OsLib supports sampling and processing/plotting of optimization output
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <iostream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/C_ConvFile.h>

#include <osClass/IO_UncertSimResults.h>

const char*     IO_UncertSimResults::fileHeader = "Uncertainty Simulation Table Binary Data File";
const int       IO_UncertSimResults::writeVersion = 0;

IO_UncertSimResults::IO_UncertSimResults() : BufFileC(4096)
{
    SetToNull(fileName);
    readVersion = -1;
}
IO_UncertSimResults::~IO_UncertSimResults()
{
}

bool IO_UncertSimResults::OKforReading()
{
    DC_UncertSimResults dummy;
    return ReadFileHeader(dummy, false) && (nRuns > 0);
}


bool IO_UncertSimResults::ReadUncertResults(DC_UncertSimResults& results)
{
    try
    {
        ReadFileHeader(results, true);

        ReadNextRecord();
        ReadInt();          // skip next rec
        SC_IntArray runStartRecs;
        ReadIntArray(runStartRecs);

        results.AllocAndSetSize(runStartRecs.Size());
        for (int i = 0; i < results.Size(); i++)
        {
            ReadAbsolute(runStartRecs[i]);
            ReadOneRun(results[i]);
        }

        CloseFile();
        return true;
    }
    catch (BufFileC::BufferFileError& err) {
        GenAppErrorMsg("ReadUncertResults", err.errMsg);
        CloseFile();
    }

    return false;
}


bool IO_UncertSimResults::AddUncertRun(const DC_UncertSimResults& resultHeader,
                                       const UncertRunResults&    results,
                                             bool                 addExisting)
{
    GenAppClearMsgs();
    bool openExisting = addExisting && ConvFileC::CFfileExists(fileName);

    DC_UncertSimResults currHeader;
    if (openExisting)
    {
        if (!ReadFileHeader(currHeader, false))
        {
            GenAppErrorMsg("UncertSimResults", "File contents incompatible");
            return false;
        }

        if ((currHeader.IsMultiple() && (!resultHeader.IsMultiple())) ||
            ((!currHeader.IsMultiple()) && resultHeader.IsMultiple()))
        {
            GenAppErrorMsg("UncertSimResults", "Cannot combine multiple & not multiple");
            return false;
        }

        if (currHeader.IsMultiple())
        {
            if (currHeader.multipleRunVariableIDs.Size() != resultHeader.multipleRunVariableIDs.Size())
                GenAppErrorMsg("UncertSimResults", "Multiple run results different size??");
        }
    }

    try
    {
        SC_IntArray runStartRecs;
        runStartRecs.SetResizable();
        int nextRec, runIndexRecord;
        if (openExisting)
        {
            ReadFileHeader(currHeader, true);
            ReadNextRecord();
            runIndexRecord = GetCurrentRec();
            nextRec = ReadInt();
            ReadIntArray(runStartRecs);
        }
        else
        {
            OpenFile(fileName, false);

            //  set up initial header info
            nRuns       = 0;
            WriteFileHeader(resultHeader);

            FlushBuffer(nextRec);

            runIndexRecord = nextRec;

            // initial header
            BufferReset(nextRec);
            WriteInt(0);
            FlushBuffer(nextRec);

            // backup and set nextRec
            BufferReset(runIndexRecord);
            WriteInt(nextRec);

            // reflush
            FlushBuffer();
        }

        int runStart = nextRec;

        //  set to next avail
        BufferReset(nextRec);

        // add the run to the end
        WriteOneRun(results, nextRec);
        nRuns++;

        // update header
        BufferReset(0);
        WriteFileHeader(resultHeader);
        FlushBuffer();

        //  update next avail record
        BufferReset(runIndexRecord);
        WriteInt(nextRec);

        // update run start records
        runStartRecs += runStart;
        WriteIntArray(runStartRecs);
        FlushBuffer();

        CloseFile();
        return true;
    }
    catch (BufFileC::BufferFileError& err) {
        GenAppErrorMsg("WriteUncertResults", err.errMsg);
     }

    CloseFile();
    return false;
}




bool IO_UncertSimResults::ReadFileHeader(DC_UncertSimResults&   results,
                                       bool leaveOpen)
{
    try
    {
        OpenFile(fileName, true);
        int dummy;
        ReadStdFileHeader(fileHeader, StringLength(fileHeader) + 1, readVersion, dummy);

        if (readVersion  > writeVersion)
            BufferError("Future version number ??");

        ReadInt(nRuns);
        results.uncertType = DC_UncertSimResults::UncertType(ReadInt());

        if (readVersion  > 0)
        {
            results.isMultipleRun = ReadBool();
            if (results.isMultipleRun)
            {
                ReadString(results.resultID, results.resultIDLen);
                ReadStringArray(results.multipleRunVariableIDs);
            }

        }

        if (!leaveOpen)
            CloseFile();

        return true;
    }
    catch (BufFileC::BufferFileError& re) {
        if (leaveOpen)
            throw re;
    }

    CloseFile();

    return false;
}

void IO_UncertSimResults::WriteFileHeader(const DC_UncertSimResults&    results)
{
    WriteStdFileHeader(fileHeader, StringLength(fileHeader) + 1, writeVersion, 0);
    WriteInt(nRuns);
    WriteInt(int(results.uncertType));

    WriteBool(results.isMultipleRun);
    if (results.isMultipleRun)
    {
        WriteString(results.resultID, results.resultIDLen);
        WriteStringArray(results.multipleRunVariableIDs);
    }
}

void IO_UncertSimResults::WriteTable(const DC_TableData& tableData)
{
    WriteString(tableData.tableID, DC_TableData::tableIDLen);
    WriteStringArray(tableData.columnDesc);
    WriteStringArray(tableData.rowDesc);
    WriteDoubleMatrix(tableData.dataTable);
}

void IO_UncertSimResults::ReadTable(DC_TableData& tableData)
{
    ReadString(tableData.tableID, DC_TableData::tableIDLen);
    ReadStringArray(tableData.columnDesc);
    ReadStringArray(tableData.rowDesc);
    ReadDoubleMatrix(tableData.dataTable);
}


void IO_UncertSimResults::WriteTableArray(const DC_TableArray& tableArray)
{
    WriteInt(tableArray.Size());
    for (int i = 0; i < tableArray.Size(); i++)
        WriteTable(tableArray[i]);
}

void IO_UncertSimResults::ReadTableArray(DC_TableArray& tableArray)
{
    tableArray.AllocAndSetSize(ReadInt());
    for (int i = 0; i < tableArray.Size(); i++)
        ReadTable(tableArray[i]);
}

void IO_UncertSimResults::ReadOneRun(UncertRunResults& runRes)
{
    ReadString(runRes.runID, UncertRunResults::runIDLen);

    ReadDoubleArray(runRes.multipleRunVariables);

    runRes.multipleRunScenarioIndex = ReadInt();

    ReadTableArray(runRes.resultTables);

}


void IO_UncertSimResults::WriteOneRun(const UncertRunResults&   runRes,
                                            int&                nextRec)
{
    // run header
    WriteString(runRes.runID, UncertRunResults::runIDLen);

    WriteDoubleArray(runRes.multipleRunVariables);

    // MPI support
    WriteInt(runRes.multipleRunScenarioIndex);

    WriteTableArray(runRes.resultTables);

    FlushBuffer(nextRec);
}

