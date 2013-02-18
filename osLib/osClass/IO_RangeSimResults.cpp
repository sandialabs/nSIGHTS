///////////////////////////////////////////////////////////////////////////////////
//
// IO_RangeSimResults.cpp
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
#include <string.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <osClass/IO_RangeSimResults.h>

const char*     IO_RangeSimResults::fileHeader = "nSIGHTS Simulator Results Range Binary Data File";
const int       IO_RangeSimResults::writeVersion = 0;

IO_RangeSimResults::IO_RangeSimResults() : BufFileC(4096)
{
    fileName[0] = '\0';
    readVersion = -1;
    fileIsCubes = true;
}
IO_RangeSimResults::~IO_RangeSimResults()
{
}

bool IO_RangeSimResults::IsCompatible(bool newRunIsCube)
{
    return (fileIsCubes && newRunIsCube) || ((!fileIsCubes) && (!newRunIsCube));

}
bool IO_RangeSimResults::OKtoAdd(bool addRunIsCube)
{
    bool fileOK = OKforReading();
    if (fileOK)
        return IsCompatible(addRunIsCube);

    return true;
}

bool IO_RangeSimResults::OKforReading()
{
    return ReadFileHeader(false) && (nRuns > 0);
}

bool IO_RangeSimResults::ReadSimResults(DC_RangeSimResults& results)
{
    try
    {
        ReadFileHeader(true);

        results.rangeIsCube = fileIsCubes;

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


bool IO_RangeSimResults::AddSimRun(const RangeSimRunResults& results,
                                    bool                     addExisting)
{
    GenAppClearMsgs();
    bool fileOK = addExisting && ReadFileHeader(false);
    if (fileOK && (!IsCompatible(results.runIsCube)))
    {
        GenAppErrorMsg("RangeSimResults", "File contents incompatible");
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
            WriteStdFileHeader(fileHeader, StringLength(fileHeader) + 1, writeVersion, 0);
            WriteInt(0);                        // 0 runs
            WriteBool(results.runIsCube);
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
        WriteStdFileHeader(fileHeader, StringLength(fileHeader) + 1, writeVersion, 0);
        WriteInt(nRuns);
        WriteBool(results.runIsCube);
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




bool IO_RangeSimResults::ReadFileHeader(bool leaveOpen)
{
    try
    {
        OpenFile(fileName, true);
        int dummy;
        ReadStdFileHeader(fileHeader, StringLength(fileHeader) + 1, readVersion, dummy);

        if (readVersion  != writeVersion)
            BufferError("Old or bad version number");

        ReadInt(nRuns);
        ReadBool(fileIsCubes);

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


void IO_RangeSimResults::WriteDataCoord(const DC_DataCoord& outData)
{
    WriteString(outData.dataID, DC_DataCoord::dataIDLen);
    WriteBool(outData.dataIsLog);
    WriteDoubleArray(outData);

}
void IO_RangeSimResults::ReadDataCoord(DC_DataCoord& inData)
{
    ReadString(inData.dataID, DC_DataCoord::dataIDLen);
    ReadBool(inData.dataIsLog);
    ReadDoubleArray(inData);
}


void IO_RangeSimResults::ReadOneOutput(RangeSimOutput& outputRes)
{
    ReadString(outputRes.outputID, RangeSimOutput::outputIDLen);
    ReadDoubleArray(outputRes.gridCubeData);
}

void IO_RangeSimResults::ReadOneRun(RangeSimRunResults& runRes)
{
    ReadString(runRes.runID, RangeSimRunResults::runIDLen);

    ReadBool(runRes.runIsCube);
    ReadDataCoord(runRes.xData);
    ReadDataCoord(runRes.yData);
    if (runRes.runIsCube)
        ReadDataCoord(runRes.zData);

    //  next record contains output start records
    ReadNextRecord();
    SC_IntArray ouputStartRecs;
    ReadIntArray(ouputStartRecs);
    runRes.AllocAndSetSize(ouputStartRecs.Size());

    // individual case results
    for (int i = 0; i < runRes.Size(); i++)
    {
        ReadAbsolute(ouputStartRecs[i]);
        ReadOneOutput(runRes[i]);
    }
}


void IO_RangeSimResults::WriteOneOutput(const RangeSimOutput& outputRes,
                                         int&                nextRec)
{
    BufferReset(nextRec);
    WriteString(outputRes.outputID, RangeSimOutput::outputIDLen);
    WriteDoubleArray(outputRes.gridCubeData, nextRec);
}

void IO_RangeSimResults::WriteOneRun(const RangeSimRunResults& runRes,
                                        int&             nextRec)
{
    // run header
    WriteString(runRes.runID, RangeSimRunResults::runIDLen);

    WriteBool(runRes.runIsCube);
    WriteDataCoord(runRes.xData);
    WriteDataCoord(runRes.yData);
    if (runRes.runIsCube)
        WriteDataCoord(runRes.zData);
    FlushBuffer(nextRec);

    //  next record contains case start records
    BufferReset(nextRec);
    SC_IntArray outputStartRecs(runRes.Size(), -1);
    int runStartRec = nextRec;
    WriteIntArray(outputStartRecs, nextRec);

    // individual case results
    for (int i = 0; i < runRes.Size(); i++)
    {
        outputStartRecs[i] = nextRec;
        WriteOneOutput(runRes[i], nextRec);
    }

    //  update case start
    BufferReset(runStartRec);
    WriteIntArray(outputStartRecs);
    FlushBuffer();
}

