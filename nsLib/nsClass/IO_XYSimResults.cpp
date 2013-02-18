///////////////////////////////////////////////////////////////////////////////////
//
// IO_XYSimResults.cpp
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
//      File I/O for DC_XYSimResults
//
///////////////////////////////////////////////////////////////////////////////////

//
//
// Last Modification Date:
//
// Revision History:
//
#include <string.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <nsClass/IO_XYSimResults.h>

const char*     IO_XYSimResults::fileHeader = "nSIGHTS Simulator Results XY Binary Data File";
const int       IO_XYSimResults::writeVersion = 2; // added caseIndex
//const int         IO_XYSimResults::writeVersion = 1;

IO_XYSimResults::IO_XYSimResults() : BufFileC(4096)
{
    fileName[0] = '\0';
    readVersion = -1;
}
IO_XYSimResults::~IO_XYSimResults()
{
}

bool IO_XYSimResults::IsCompatible(int nxyIn, int ncaseIn)
{
    return (nxyData == nxyIn) && (ncaseValues == ncaseIn);
}


bool IO_XYSimResults::OKforReading()
{
    return ReadFileHeader(false) && (nRuns > 0);
}

bool IO_XYSimResults::OKtoAdd(int nxyIn, int ncaseIn)
{
    return OKforReading() && (readVersion > 0) && IsCompatible(nxyIn, ncaseIn);
}

bool IO_XYSimResults::ReadSimResults(DC_XYSimResults& results)
{
    try
    {
        ReadFileHeader(true);

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
        GenAppErrorMsg("ReadSimResults", err.errMsg);
        CloseFile();
    }

    return false;
}


bool IO_XYSimResults::AddSimRun(const XYSimRunResults& results,
                                    bool             addExisting)
{
    GenAppClearMsgs();
    bool fileOK = addExisting && ReadFileHeader(false);
    if (fileOK && (!IsCompatible(results.GetnXY(), results.GetnCaseVals())))
    {
        GenAppErrorMsg("XYSimResults", "File contents incompatible");
        return false;
    }


    try
    {
        SC_IntArray runStartRecs;
        runStartRecs.SetResizable();
        int nextRec, runIndexRecord;
        if (fileOK)
        {
            ReadFileHeader(true);
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
            nxyData     = results.GetnXY();
            ncaseValues = results.GetnCaseVals();
            WriteFileHeader();

            FlushBuffer(nextRec);

            runIndexRecord = nextRec;

            // initial header
            BufferReset(nextRec);
            WriteInt(0);
            WriteInt(0);
            WriteInt(0);
            FlushBuffer(nextRec);

            // backup and set nextRec
            BufferReset(runIndexRecord);
            WriteInt(nextRec);
            WriteInt(0);
            WriteInt(0);

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
        WriteFileHeader();
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
        GenAppErrorMsg("WriteSimResults", err.errMsg);
     }

    CloseFile();
    return false;
}


bool IO_XYSimResults::ReadFileHeader(bool leaveOpen)
{
    try
    {
        OpenFile(fileName, true);
        int dummy;
        ReadStdFileHeader(fileHeader, StringLength(fileHeader) + 1, readVersion, dummy);

        if (readVersion  > writeVersion)
            BufferError("Future version number ??");

        ReadInt(nRuns);

        if ((readVersion == 0) && (nRuns > 1))
            BufferError("Cannot read version 0 file with multiple runs");

        if (readVersion > 0)
        {
            ReadInt(nxyData);
            ReadInt(ncaseValues);
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

void IO_XYSimResults::WriteFileHeader()
{
    WriteStdFileHeader(fileHeader, StringLength(fileHeader) + 1, writeVersion, 0);
    WriteInt(nRuns);
    WriteInt(nxyData);
    WriteInt(ncaseValues);
}


void IO_XYSimResults::ReadOneCase(XYSimCaseResults& caseRes)
{
    ReadString(caseRes.caseID, XYSimCaseResults::caseIDLen);
    ReadDouble(caseRes.staticPressure);
    if (readVersion > 1)
        ReadInt(caseRes.caseIndex);
    else
        caseRes.caseIndex = -1;

    caseRes.caseValues.AllocAndSetSize(ReadInt());
    for (int i = 0; i < caseRes.caseValues.Size(); i++)
        ReadDouble(caseRes.caseValues[i]);

    caseRes.caseData.AllocAndSetSize(ReadInt());
    for (int i = 0; i < caseRes.caseData.Size(); i++)
    {
        DC_XYData& currXY = caseRes.caseData[i];
        ReadString(currXY.dataID, DC_XYData::dataIDLen);

        int nXY = ReadInt();
        if (!currXY.Alloc(nXY))
            BufferError("couldn't alloc XY");

        currXY.xData.SetSize(nXY);
        currXY.yData.SetSize(nXY);
        for (int j = 0; j < currXY.Size(); j++)
        {
            ReadDouble(currXY.xData[j]);
            ReadDouble(currXY.yData[j]);
        }
    }
}

void IO_XYSimResults::ReadOneRun(XYSimRunResults& runRes)
{
    ReadString(runRes.runID, XYSimRunResults::runIDLen);

    ReadStringArray(runRes.caseValDesc);
    ReadStringArray(runRes.caseValShortDesc);
    ReadStringArray(runRes.caseSeqData.sequenceIDs);
    ReadDoubleArray(runRes.caseSeqData.sequenceStartTimes);

    //  next record contains case start records
    ReadNextRecord();
    SC_IntArray caseStartRecs;
    ReadIntArray(caseStartRecs);
    runRes.AllocAndSetSize(caseStartRecs.Size());

    // individual case results
    for (int i = 0; i < runRes.Size(); i++)
    {
        ReadAbsolute(caseStartRecs[i]);
        ReadOneCase(runRes[i]);
    }
}


void IO_XYSimResults::WriteOneCase(const XYSimCaseResults& caseRes,
                                         int&                nextRec)
{
    BufferReset(nextRec);
    WriteString(caseRes.caseID, XYSimCaseResults::caseIDLen);
    WriteDouble(caseRes.staticPressure);
    WriteInt(caseRes.caseIndex);  // added v2

    WriteInt(caseRes.caseValues.Size());
    for (int i = 0; i < caseRes.caseValues.Size(); i++)
        WriteDouble(caseRes.caseValues[i]);

    WriteInt(caseRes.caseData.Size());
    for (int i = 0; i < caseRes.caseData.Size(); i++)
    {
        DC_XYData& currXY = caseRes.caseData[i];
        WriteString(currXY.dataID, DC_XYData::dataIDLen);
        WriteInt(currXY.Size());
        for (int j = 0; j < currXY.Size(); j++)
        {
            WriteDouble(currXY.xData[j]);
            WriteDouble(currXY.yData[j]);
        }
    }
    FlushBuffer(nextRec);
}

void IO_XYSimResults::WriteOneRun(const XYSimRunResults& runRes,
                                        int&             nextRec)
{
    // run header
    WriteString(runRes.runID, XYSimRunResults::runIDLen);

    WriteStringArray(runRes.caseValDesc);
    WriteStringArray(runRes.caseValShortDesc);
    WriteStringArray(runRes.caseSeqData.sequenceIDs);
    WriteDoubleArray(runRes.caseSeqData.sequenceStartTimes);
    FlushBuffer(nextRec);

    //  next record contains case start records
    BufferReset(nextRec);
    SC_IntArray caseStartRecs(runRes.Size(), -1);
    int runStartRec = nextRec;
    WriteIntArray(caseStartRecs, nextRec);

    // individual case results
    for (int i = 0; i < runRes.Size(); i++)
    {
        caseStartRecs[i] = nextRec;
        WriteOneCase(runRes[i], nextRec);
    }

    //  update case start
    BufferReset(runStartRec);
    WriteIntArray(caseStartRecs);
    FlushBuffer();
}

