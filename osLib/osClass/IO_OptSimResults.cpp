///////////////////////////////////////////////////////////////////////////////////
//
// IO_OptSimResults.cpp
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
//      binary file I/O for DC_OptSimResults.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <iostream>

#include <osClass/IO_OptSimResults.h>

const char*     IO_OptSimResults::fileHeader = "nSIGHTS Optimizer Binary Data File";
const int       IO_OptSimResults::writeVersion = 6;  // added caseIndex
//const int       IO_OptSimResults::writeVersion = 5;  // added new optimizers
//const int       IO_OptSimResults::writeVersion = 4;  // added calculated parameters
//const int       IO_OptSimResults::writeVersion = 3;  // dropped ntotalFitpoints
//const int       IO_OptSimResults::writeVersion = 2;  // unit strings
//const int       IO_OptSimResults::writeVersion = 1;  // added short var desc + run ID stuff
//const int       IO_OptSimResults::writeVersion = 0;

IO_OptSimResults::IO_OptSimResults() : BufFileC(4096)
{
    fileName[0] = '\0';
    readVersion = -1;
}
IO_OptSimResults::~IO_OptSimResults()
{
}

bool IO_OptSimResults::OKtoAdd(const OptSimResultsHeader&  addHeaderInfo)
{
    bool fileOK = OKforReading();
    if (fileOK && (!currInfo.AreCompatible(addHeaderInfo)))
        return false;

    return true;
}

bool IO_OptSimResults::OKforReading()
{
    return ReadFileHeader(false) && (nRuns > 0);
}

bool IO_OptSimResults::ReadSimResults(DC_OptSimResults& results)
{
    try
    {
        ReadFileHeader(true);
        results.OptSimResultsHeader::operator=(currInfo);

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



bool IO_OptSimResults::AddSimRun(const OptSimResultsHeader&  runHeaderInfo,
                                 const OptSimRunResults&     results,
                                       int                   maxCase,      // max case in run
                                       bool                  addExisting)
{
    GenAppClearMsgs();
    bool fileOK = addExisting && ReadFileHeader(false);
    if (fileOK && !currInfo.AreCompatible(runHeaderInfo))
    {
        GenAppErrorMsg("OptSimResults", "File contents incompatible");
        return false;
    }

//  std::cout << fileName << std::endl;

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
            nRuns = 0;
            currInfo = runHeaderInfo;
            WriteFileHeader();
            FlushBuffer(nextRec);

            runIndexRecord = nextRec;

            // initial run start rec data
            BufferReset(nextRec);
            WriteInt(0);                // next avail rec
            WriteInt(0);                // number of runs
            WriteInt(0);                // run 0
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
        WriteRunHeader(results, maxCase, nextRec);
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

bool IO_OptSimResults::AddNextCase(const OptCaseResults&   results)
{
    GenAppClearMsgs();
    try
    {
        SC_IntArray runStartRecs;
        ReadFileHeader(true);
        ReadNextRecord();
        int runIndexRecord = GetCurrentRec();
        int nextRec = ReadInt();
        ReadIntArray(runStartRecs);
        WriteNextCase(results, runStartRecs.LastIndex(), nextRec);

        //  update next rec
        BufferReset(runIndexRecord);
        WriteInt(nextRec);
        WriteIntArray(runStartRecs);
        FlushBuffer();

        CloseFile();
        return true;
    }
    catch (BufFileC::BufferFileError& err) {
        GenAppErrorMsg("Add Next Case", err.errMsg);
    }
    CloseFile();
    return false;
}



void IO_OptSimResults::WriteFileHeader()
{
    WriteStdFileHeader(fileHeader, StringLength(fileHeader) + 1, writeVersion, 0);
    WriteInt(nRuns);                        // 0 runs
    WriteInt(int(currInfo.runType));
    WriteBool(currInfo.containsCovariance);
    WriteBool(currInfo.containsResiduals);
    WriteBool(currInfo.containsJacobian);
    WriteInt(currInfo.noptVar);
    WriteInt(currInfo.ncalcVar); // add v4
}

bool IO_OptSimResults::ReadFileHeader(bool leaveOpen)
{
    try
    {
        OpenFile(fileName, true);
        int dummy;
        ReadStdFileHeader(fileHeader, StringLength(fileHeader) + 1, readVersion, dummy);

        if (readVersion  < 0)
            BufferError("Old or bad version number");

        ReadInt(nRuns);
        int rt;
        ReadInt(rt);
        currInfo.runType = OptSimResultsHeader::RunType(rt);
        ReadBool(currInfo.containsCovariance);
        ReadBool(currInfo.containsResiduals);
        ReadBool(currInfo.containsJacobian);
        ReadInt(currInfo.noptVar);
        if (readVersion  > 3)
            ReadInt(currInfo.ncalcVar);
        else
            currInfo.ncalcVar = 0;


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



void IO_OptSimResults::WriteFitComponent(const OptFitComponent&  fitComp)
{
    WriteDouble(fitComp.fitSumSquaredErrors);
    WriteDouble(fitComp.fitEstimatedVariance);
    WriteInt(fitComp.nfitPoints);
    if (currInfo.containsCovariance)
        WriteDoubleMatrix(fitComp.fitCovariance);
    if (currInfo.containsResiduals)
        WriteDoubleArray(fitComp.fitResiduals);
    if (currInfo.containsJacobian)
        WriteDoubleMatrix(fitComp.fitJacobian);
}


void  IO_OptSimResults::ReadFitComponent(OptFitComponent&    fitComp)
{
    ReadDouble(fitComp.fitSumSquaredErrors);
    ReadDouble(fitComp.fitEstimatedVariance);
    ReadInt(fitComp.nfitPoints);
    if (currInfo.containsCovariance)
        ReadDoubleMatrix(fitComp.fitCovariance);
    if (currInfo.containsResiduals)
        ReadDoubleArray(fitComp.fitResiduals);
    if (currInfo.containsJacobian)
        ReadDoubleMatrix(fitComp.fitJacobian);
}


void IO_OptSimResults::WriteSingleFit(const OptSingleFit&       singleFit,
                                            int&                nextRec)
{
    WriteBool(singleFit.fitCompletedOK);
    WriteInt(singleFit.nsimsRequired);
    WriteInt(singleFit.niteration);

    WriteDouble(singleFit.fitValue);
    WriteDouble(singleFit.derivSpan);
    WriteDouble(singleFit.sumSquaredError);
    WriteDouble(singleFit.meanSquaredError);
    WriteDouble(singleFit.estimatedVariance);
    WriteDoubleMatrix(singleFit.actualCovariance);
    WriteDoubleMatrix(singleFit.estimatedCovariance);


    WriteDoubleArray(singleFit.finalParData);
    WriteDoubleArray(singleFit.finalNormParData);

    WriteDoubleArray(singleFit.startParData);
    WriteDoubleArray(singleFit.startNormParData);

    WriteDoubleArray(singleFit.calcParData); // added v4

    WriteDouble(singleFit.fitDelta);

    WriteInt(singleFit.fitComponents.Size());
    for (int i = 0; i < singleFit.fitComponents.Size(); i++)
        WriteFitComponent(singleFit.fitComponents[i]);

    FlushBuffer(nextRec);
}


void IO_OptSimResults::ReadSingleFit(OptSingleFit&  singleFit)
{
    ReadBool(singleFit.fitCompletedOK);
    ReadInt(singleFit.nsimsRequired);
    ReadInt(singleFit.niteration);

    ReadDouble(singleFit.fitValue);
    ReadDouble(singleFit.derivSpan);
    ReadDouble(singleFit.sumSquaredError);
    ReadDouble(singleFit.meanSquaredError);
    ReadDouble(singleFit.estimatedVariance);
    ReadDoubleMatrix(singleFit.actualCovariance);
    ReadDoubleMatrix(singleFit.estimatedCovariance);

    ReadDoubleArray(singleFit.finalParData);
    ReadDoubleArray(singleFit.finalNormParData);

    ReadDoubleArray(singleFit.startParData);
    ReadDoubleArray(singleFit.startNormParData);

    if (readVersion > 3)
        ReadDoubleArray(singleFit.calcParData);

    ReadDouble(singleFit.fitDelta);

    int nfitComp = ReadInt();
    singleFit.fitComponents.AllocAndSetSize(nfitComp);
    for (int i = 0; i < singleFit.fitComponents.Size(); i++)
        ReadFitComponent(singleFit.fitComponents[i]);
}


void IO_OptSimResults::WriteSingleCase(const OptCaseResults&     singleCase,
                                             int&                nextRec)
{

    WriteString(singleCase.caseDesc, OptCaseResults::caseDescLen);
    WriteInt(singleCase.caseIndex); // added v6
    WriteDoubleArray(singleCase.caseParVals, nextRec);

    int startRec = nextRec;
    SC_IntArray fitStartRecs(singleCase.caseResults.Size(), -1);

    BufferReset(nextRec);
    WriteIntArray(fitStartRecs, nextRec);

    // individual fit results
    for (int i = 0; i < singleCase.caseResults.Size(); i++)
    {
        fitStartRecs[i] = nextRec;
        BufferReset(nextRec);
        WriteSingleFit(singleCase.caseResults[i], nextRec);
    }

    //  update case start
    BufferReset(startRec);
    WriteIntArray(fitStartRecs);
    FlushBuffer();
}


void IO_OptSimResults::ReadSingleCase(OptCaseResults&     singleCase)
{
    ReadString(singleCase.caseDesc, OptCaseResults::caseDescLen);
    if (readVersion > 5)
        ReadInt(singleCase.caseIndex);
    else
        singleCase.caseIndex = -1;

    ReadDoubleArray(singleCase.caseParVals);

    ReadNextRecord();
    SC_IntArray fitStartRecs;
    ReadIntArray(fitStartRecs);
    singleCase.caseResults.AllocAndSetSize(fitStartRecs.Size());

    // individual case results
    for (int i = 0; i < singleCase.caseResults.Size(); i++)
    {
        ReadAbsolute(fitStartRecs[i]);
        ReadSingleFit(singleCase.caseResults[i]);
    }
}


void IO_OptSimResults::ReadOneRun(OptSimRunResults& singleRun)
{
    //  first entry is data start rec
    int runStartRec = ReadInt();

    // info only
    int maxCase = ReadInt();

    //  remainder of first record is case start records
    SC_IntArray caseStartRecs;
    ReadIntArray(caseStartRecs);
    singleRun.runResults.AllocAndSetSize(caseStartRecs.Size());

    // go to the data
    ReadAbsolute(runStartRec);

    // run ID
    ReadString(singleRun.runID, OptSimRunResults::runIDLen);
    if (readVersion > 0)
    {
        if (readVersion < 5)
        {
            bool temp;
            ReadBool(temp);
            if (temp)
                CopyString(singleRun.optDescription, "Simplex", OptSimRunResults::optDescLen);
            else
                CopyString(singleRun.optDescription, "LevMar", OptSimRunResults::optDescLen);
        }
        else
            ReadString(singleRun.optDescription, OptSimRunResults::optDescLen);

        ReadBool(singleRun.fullCovarianceCalc);
    }

    //  read rest of run info
    // fits first
    ReadInt(singleRun.nFits);
    singleRun.fitDescriptions.AllocAndSetSize(singleRun.nFits);
    int i;
    for (i = 0; i < singleRun.nFits; i++)
    {
        OptFitDescription& currFit = singleRun.fitDescriptions[i];
        ReadString(currFit.fitID, OptFitDescription::fitIDLen);
        ReadDoubleArray(currFit.fitScaling);
        if (readVersion < 3)
        {
            int ntotalFitPoints;
            ReadInt(ntotalFitPoints);
        }
        int nfitComp = ReadInt();
        currFit.fitComponentDescription.AllocAndSetSize(nfitComp);
        for (int j = 0; j < nfitComp; j++)
        {
            OptFitComponentDescription& currFitComp = currFit.fitComponentDescription[j];
            ReadString(currFitComp.fitID, OptFitComponentDescription::fitIDLen);
            ReadInt(currFitComp.nfitPoints);
            ReadBool(currFitComp.fitWasL1);
            ReadBool(currFitComp.fitWasNormed);
            ReadDouble(currFitComp.fitChisqSigma);
            if (currInfo.containsResiduals || currInfo.containsJacobian)
                ReadDoubleArray(currFitComp.fitResidJacobX);
        }
    }

    // opt var desc
    singleRun.varDescriptions.AllocAndSetSize(currInfo.noptVar);
    for (i = 0; i < currInfo.noptVar; i++)
    {
        OptVarDescription& currVar = singleRun.varDescriptions[i];
        ReadString(currVar.varDesc, OptVarDescription::varDescLen);
        if (readVersion > 0)
        {
            ReadString(currVar.shortVarDesc, OptVarDescription::shortVarDescLen);
            if (readVersion > 1)
                ReadString(currVar.varUnits, OptVarDescription::varUnitsLen);
        }
        else
        {
            CopyString(currVar.shortVarDesc, "Par#", OptVarDescription::shortVarDescLen);
            ConcatInt(i, currVar.shortVarDesc, OptVarDescription::shortVarDescLen);
        }

        ReadDouble(currVar.userMinLim);
        ReadDouble(currVar.userMaxLim);
        ReadDouble(currVar.parSigma  );
        ReadBool(currVar.parIsLinear);
    }

    // case par desc
    ReadInt(singleRun.ncaseDesc);
    singleRun.caseDescriptions.AllocAndSetSize(singleRun.ncaseDesc);
    for (i = 0; i < singleRun.ncaseDesc; i++)
    {
        CaseVarDescription& currCaseVar = singleRun.caseDescriptions[i];
        ReadString(currCaseVar.caseVarDesc, CaseVarDescription::caseVarDescLen);
        ReadString(currCaseVar.caseVarShortDesc, CaseVarDescription::caseVarShortDescLen);
        if (readVersion > 0)
        {
            if (readVersion > 1)
                ReadString(currCaseVar.caseVarUnits, CaseVarDescription::caseVarUnitsLen);
            ReadBool(currCaseVar.varIsLinear);
        }
    }

    // calc par
    if (readVersion > 3)
    {
        singleRun.calcDescriptions.AllocAndSetSize(currInfo.ncalcVar);
        for (i = 0; i < currInfo.ncalcVar; i++)
        {
            CaseVarDescription& currCalcVar = singleRun.calcDescriptions[i];
            ReadString(currCalcVar.caseVarDesc, CaseVarDescription::caseVarDescLen);
            ReadString(currCalcVar.caseVarShortDesc, CaseVarDescription::caseVarShortDescLen);
            ReadString(currCalcVar.caseVarUnits, CaseVarDescription::caseVarUnitsLen);
            ReadBool(currCalcVar.varIsLinear);
        }
    }


    // and finally --- run cases
    for (i = 0; i < singleRun.runResults.Size(); i++)
    {
        ReadAbsolute(caseStartRecs[i]);
        ReadSingleCase(singleRun.runResults[i]);
    }
}

void IO_OptSimResults::WriteRunHeader(const OptSimRunResults& singleRun,
                                      int               maxCase,
                                      int&              nextRec)
{
    int startRec = nextRec;
    WriteInt(-1);     // start of data

    WriteInt(maxCase);                  // for info
    //  each run start
    SC_IntArray caseStartRecs(maxCase, -1);
    WriteIntArray(caseStartRecs, nextRec);

    // get current rec
    int dataStartRec = nextRec;

    // go back and reset run header data
    BufferReset(startRec);
    WriteInt(dataStartRec);

    //  reset number of runs & flush
    WriteInt(maxCase);
    WriteInt(0);
    FlushBuffer();

    // go back to run data rec and start writing
    BufferReset(dataStartRec);
    WriteString(singleRun.runID, OptSimRunResults::runIDLen);
    //  WriteBool(singleRun.simplexOpt); changed v5
    WriteString(singleRun.optDescription, OptSimRunResults::optDescLen); // added v5

    WriteBool(singleRun.fullCovarianceCalc);

    // fits first
    WriteInt(singleRun.nFits);
    int i;
    for (i = 0; i < singleRun.nFits; i++)
    {
        OptFitDescription& currFit = singleRun.fitDescriptions[i];
        WriteString(currFit.fitID, OptFitDescription::fitIDLen);
        WriteDoubleArray(currFit.fitScaling);
        WriteInt(currFit.fitComponentDescription.Size());
        for (int j = 0; j < currFit.fitComponentDescription.Size(); j++)
        {
            OptFitComponentDescription& currFitComp = currFit.fitComponentDescription[j];
            WriteString(currFitComp.fitID, OptFitComponentDescription::fitIDLen);
            WriteInt(currFitComp.nfitPoints);
            WriteBool(currFitComp.fitWasL1);
            WriteBool(currFitComp.fitWasNormed);
            WriteDouble(currFitComp.fitChisqSigma);
            if (currInfo.containsResiduals || currInfo.containsJacobian)
                WriteDoubleArray(currFitComp.fitResidJacobX);
        }
    }

    // opt var desc
    for (i = 0; i < currInfo.noptVar; i++)
    {
        OptVarDescription& currVar = singleRun.varDescriptions[i];
        WriteString(currVar.varDesc, OptVarDescription::varDescLen);
        WriteString(currVar.shortVarDesc, OptVarDescription::shortVarDescLen);
        WriteString(currVar.varUnits, OptVarDescription::varUnitsLen);

        WriteDouble(currVar.userMinLim);
        WriteDouble(currVar.userMaxLim);
        WriteDouble(currVar.parSigma  );
        WriteBool(currVar.parIsLinear);
    }

    // case par desc
    WriteInt(singleRun.ncaseDesc);
    for (i = 0; i < singleRun.ncaseDesc; i++)
    {
        const CaseVarDescription& currCaseVar = singleRun.caseDescriptions[i];
        WriteString(currCaseVar.caseVarDesc, CaseVarDescription::caseVarDescLen);
        WriteString(currCaseVar.caseVarShortDesc, CaseVarDescription::caseVarShortDescLen);
        WriteString(currCaseVar.caseVarUnits, CaseVarDescription::caseVarUnitsLen);
        WriteBool(currCaseVar.varIsLinear);
    }

    // calc par - added v4
    for (i = 0; i < currInfo.ncalcVar; i++)
    {
        const CaseVarDescription& currCalcVar = singleRun.calcDescriptions[i];
        WriteString(currCalcVar.caseVarDesc, CaseVarDescription::caseVarDescLen);
        WriteString(currCalcVar.caseVarShortDesc, CaseVarDescription::caseVarShortDescLen);
        WriteString(currCalcVar.caseVarUnits, CaseVarDescription::caseVarUnitsLen);
        WriteBool(currCalcVar.varIsLinear);
    }

   // all done
    FlushBuffer(nextRec);
}




void IO_OptSimResults::WriteNextCase(const OptCaseResults&  results,
                                           int              runStartRec,
                                           int&             nextRec)
{
    // get to start of run
    ReadAbsolute(runStartRec);

    // read current case header info
    int dataStartRec = ReadInt();
    int maxCase = ReadInt();
    SC_IntArray caseStartRecs;
    caseStartRecs.SetResizable();
    ReadIntArray(caseStartRecs);
    if (caseStartRecs.Size() == maxCase)
        BufferError("maximum number of cases in run exceeded ??");

    // save case start
    int caseStart = nextRec;
    //  set to next avail
    BufferReset(caseStart);

    // add the case to the end
    WriteSingleCase(results, nextRec);

    // update run header
    BufferReset(runStartRec);

    WriteInt(dataStartRec);
    WriteInt(maxCase);

    // add last run
    caseStartRecs += caseStart;
    WriteIntArray(caseStartRecs);
    FlushBuffer();
}

