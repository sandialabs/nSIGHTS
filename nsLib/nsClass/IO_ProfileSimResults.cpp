///////////////////////////////////////////////////////////////////////////////////
//
// IO_ProfileSimResults.cpp
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
//      File I/O for DC_ProfileSimResults
//
///////////////////////////////////////////////////////////////////////////////////

#include <string.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <nsClass/IO_ProfileSimResults.h>

const char*     IO_ProfileSimResults::fileHeader = "nSIGHTS Simulator Results Profile Binary Data File";
const int       IO_ProfileSimResults::writeVersion = 2;  // modified 2D profile
//const int       IO_ProfileSimResults::writeVersion = 1;  // added 2D profile
//const int       IO_ProfileSimResults::writeVersion = 0;

IO_ProfileSimResults::IO_ProfileSimResults() : BufFileC(4096)
{
    fileName[0] = '\0';
    readVersion = -1;
}
IO_ProfileSimResults::~IO_ProfileSimResults()
{
}


bool IO_ProfileSimResults::OKforReading()
{
    return ReadFileHeader(false) && (nRuns > 0);
}

bool IO_ProfileSimResults::ReadSimResults(DC_ProfileSimResults& results)
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


bool IO_ProfileSimResults::AddSimRun(const ProfileSimRunResults& results,
                                    bool             addExisting)
{
    GenAppClearMsgs();

    bool fileOK = addExisting && ReadFileHeader(false);
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




bool IO_ProfileSimResults::ReadFileHeader(bool leaveOpen)
{
    try
    {
        OpenFile(fileName, true);
        int dummy;
        ReadStdFileHeader(fileHeader, StringLength(fileHeader) + 1, readVersion, dummy);

        if (readVersion > writeVersion)
            BufferError("Future version number ??");

        ReadInt(nRuns);

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


void IO_ProfileSimResults::ReadOneCase(ProfileSimCaseResults& caseRes)
{
    ReadString(caseRes.caseID, ProfileSimCaseResults::caseIDLen);
    ReadDouble(caseRes.staticPressure);

    if (readVersion < 1)
        caseRes.caseDataIsExtended = false;
    else
        ReadBool(caseRes.caseDataIsExtended);

    caseRes.caseValues.AllocAndSetSize(ReadInt());
    for (int i = 0; i < caseRes.caseValues.Size(); i++)
        ReadDouble(caseRes.caseValues[i]);

    if (!caseRes.caseDataIsExtended)
    {
        DC_GridData& currGrid = caseRes.caseData;
        ReadString(currGrid.gridID, DC_GridData::gridIDLen);
        ReadString(currGrid.xData.dataID, DC_DataCoord::dataIDLen);
        ReadString(currGrid.yData.dataID, DC_DataCoord::dataIDLen);
        ReadBool(currGrid.xData.dataIsLog);
        ReadBool(currGrid.yData.dataIsLog);

        int nX = ReadInt();
        int nY = ReadInt();
        if (!currGrid.Alloc(nX, nY))
            BufferError("couldn't alloc Grid");

        ReadDoubleArray(currGrid.xData);
        ReadDoubleArray(currGrid.yData);
        for (int i = 0; i < currGrid.Size(); i++)
            ReadDouble(currGrid[i]);
    }
    else
    {
        ExtendedProfile& currPro = caseRes.extendedCaseData;
        ReadInt(currPro.ntotalVert);
        ReadInt(currPro.nradial);
        ReadInt(currPro.nconstantVert);
        ReadInt(currPro.nvariableVert);

        if (readVersion < 2)
        {
            double constVert;
            ReadDouble(constVert);
            currPro.constantVertZPos.Alloc(currPro.nconstantVert + 1);
            currPro.constantVertZPos += 0.0;
            if (currPro.nconstantVert > 0)
            {
                double dVert = constVert / double(currPro.nconstantVert);
                double currY = dVert;
                for (int i = 0; i < currPro.nconstantVert; i++)
                {
                    currPro.constantVertZPos += currY;
                    currY += dVert;
                }
            }
            currPro.nconstantVert++;
            currPro.nvariableVert--;
        }
        else
        {
            ReadDoubleArray(currPro.constantVertZPos);
        }

        ReadDoubleArray(currPro.nodeRadii);
        int nextended = ReadInt();
        currPro.AllocAndSetSize(nextended);
        for (int i = 0; i < nextended; i++)
        {
            ExtendedProfileTimeData& currSing = currPro[i];
            ReadDouble(currSing.profileTime);
            if (currPro.nvariableVert > 0)
                ReadDoubleArray(currSing.waterTablePos);
            ReadDoubleMatrix(currSing.nodalPressures);
        }
    }
}

void IO_ProfileSimResults::ReadOneRun(ProfileSimRunResults& runRes)
{
    ReadString(runRes.runID, ProfileSimRunResults::runIDLen);

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


void IO_ProfileSimResults::WriteOneCase(const ProfileSimCaseResults& caseRes,
                                         int&                nextRec)
{
    BufferReset(nextRec);
    WriteString(caseRes.caseID, ProfileSimCaseResults::caseIDLen);
    WriteDouble(caseRes.staticPressure);
    WriteBool(caseRes.caseDataIsExtended);

    WriteInt(caseRes.caseValues.Size());
    for (int i = 0; i < caseRes.caseValues.Size(); i++)
        WriteDouble(caseRes.caseValues[i]);

    if (!caseRes.caseDataIsExtended)
    {
        const DC_GridData& currGrid = caseRes.caseData;
        WriteString(currGrid.gridID, DC_GridData::gridIDLen);
        WriteString(currGrid.xData.dataID, DC_DataCoord::dataIDLen);
        WriteString(currGrid.yData.dataID, DC_DataCoord::dataIDLen);

        WriteBool(currGrid.xData.dataIsLog);
        WriteBool(currGrid.yData.dataIsLog);

        WriteInt(currGrid.xData.Size());
        WriteInt(currGrid.yData.Size());
        WriteDoubleArray(currGrid.xData);
        WriteDoubleArray(currGrid.yData);
        for (int i = 0; i < currGrid.Size(); i++)
            WriteDouble(currGrid[i]);
    }
    else
    {
        const ExtendedProfile& currPro = caseRes.extendedCaseData;
        WriteInt(currPro.ntotalVert);
        WriteInt(currPro.nradial);
        WriteInt(currPro.nconstantVert);
        WriteInt(currPro.nvariableVert);
//      WriteDouble(currPro.constantVertThick);  // v1
        WriteDoubleArray(currPro.constantVertZPos);
        WriteDoubleArray(currPro.nodeRadii);
        WriteInt(currPro.Size());
        for (int i = 0; i < currPro.Size(); i++)
        {
            const ExtendedProfileTimeData& currSing = currPro[i];
            WriteDouble(currSing.profileTime);
            if (currPro.nvariableVert > 0)
                WriteDoubleArray(currSing.waterTablePos);
            WriteDoubleMatrix(currSing.nodalPressures);
        }
    }


    FlushBuffer(nextRec);
}

void IO_ProfileSimResults::WriteOneRun(const ProfileSimRunResults& runRes,
                                        int&             nextRec)
{
    // run header
    WriteString(runRes.runID, ProfileSimRunResults::runIDLen);

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

