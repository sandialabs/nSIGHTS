///////////////////////////////////////////////////////////////////////////////////
//
// Main.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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


#include "StdAfx.h"

#ifdef _WIN32
#include <process.h>
#endif

#include <time.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genApp/G_Version.h>

#include <Var/G_Control.h>
#include <Var/G_Sequence.h>
#include <Var/G_Parameter.h>
#include <Var/G_DataCapture.h>
#include <Var/G_OutputFiles.h>
#include <Var/G_RangeOutput.h>
#include <Var/G_OptOutput.h>
#include <Var/G_XYOutput.h>
#include <osMain/G_Sample.h>
#include <osMain/G_Optimize.h>
#include <osMain/G_OptRange.h>

#include <nsClass/IO_XYSimResults.h>

#include <osDFO/DFO_SelectRTRangeGrid.h>
#include <osDFO/DFO_SelectRTRangeCube.h>

#include <App/nPreFile.h>
#include <Run/RunCommon.h>
#include <CmdLine/RunControl.h>


static void ReplaceExtension(char* outfName,
                             const char* baseFName,
                             const char* processID,
                             const char* extension)
{
  CopyString(outfName, baseFName, stdFileStrLen);
  if (!AddFileNameSuffix(outfName, extension, processID, stdFileStrLen))
  {
    GenAppErrorMsg(baseFName, extension);
    GenAppInternalError("spawn failure - non std extension");
  }
}


class ProcessData {
public:
  int   nPID;
  int   processResults;
  int   startIndex;
  int   endIndex;
  bool  processComplete;

  char  processID[40];
  char  xyFile[stdFileStrLen];
  char  optFile[stdFileStrLen];
  char  rngFile[stdFileStrLen];

  char  sStr[20];
  char  eStr[20];

public:
  ProcessData();

  void  SetFiles(int runIndex);

  void  UpdateFiles();

  void  DeleteFiles();

};

ProcessData::ProcessData()
{
  SetToNull(xyFile);
  SetToNull(optFile);
  SetToNull(rngFile);
  startIndex = -1;
  endIndex = -1;
  processComplete = false;
}

void ProcessData::SetFiles(int runIndex)
{
  ConcatInt(runIndex, "_S", processID, 2, 40);

  if (rangeOutputDesc.writeFile)
    ReplaceExtension(rngFile, rangeOutputFile.fileName, processID, ".nRng");

  if (optOutputDesc.writeFile)
    ReplaceExtension(optFile, optOutputFile.fileName, processID, ".nOpt");

  if (xyForwardOutputDesc.writeFile)
    ReplaceExtension(xyFile, xyForwardOutputFile.fileName, processID, ".nXYSim");
}

void ProcessData::UpdateFiles()
{
  IntToString(startIndex, sStr, 20);
  IntToString(endIndex, eStr, 20);
}

void ProcessData::DeleteFiles()
{
#ifdef _WIN32
  if (IsNotNull(rngFile))
    DeleteFile(rngFile);
  if (IsNotNull(optFile))
    DeleteFile(optFile);
  if (IsNotNull(xyFile))
    DeleteFile(xyFile);
#else
  if (IsNotNull(rngFile))
    remove(rngFile);
  if (IsNotNull(optFile))
    remove(optFile);
  if (IsNotNull(xyFile))
    remove(xyFile);
#endif
}

static T_SC_Array<ProcessData> spawnData;

static void CollectXY()
{
  GenAppInfoMsg("nPrev_M", "collecting XY");

  XYSimRunResults&  collectedXY  =   xyForwardOutputFO.runResultData;

  for (int i = 0; i < spawnData.Size(); i++)
  {
    IO_XYSimResults tempXYFile;
    DC_XYSimResults tempXYResults;
    CopyString(tempXYFile.fileName, spawnData[i].xyFile, stdFileStrLen);
    if (!tempXYFile.ReadSimResults(tempXYResults))
      GenAppInternalError("collecting XY");

    XYSimRunResults& collectRun = tempXYResults[0];
    for (int j = 0; j < collectRun.Size(); j++)
    {
      int collectCase = collectRun[j].caseIndex;
      if (collectedXY.UpperBound() < collectCase)
        collectedXY.SetSize(collectCase + 1);
      collectedXY[collectCase] = collectRun[j];
    }
  }

  xyForwardOutputFO.SimulationComplete();

  GenAppInfoMsg("nPrev_M", "XY collection complete");
}


static void CollectOpt()
{
  GenAppInfoMsg("nPrev_M", "collecting Opt");

  int maxOpt = GetnOptRuns();
  OptCaseResultsArray collectedCases;
  collectedCases.AllocAndSetSize(maxOpt);

  OptSimResultsHeader  runHeaderInfo;
  OptSimRunResults     resultsInfo;


  for (int i = 0; i < spawnData.Size(); i++)
  {
    IO_OptSimResults tempOptFile;
    DC_OptSimResults tempOptResults;

    CopyString(tempOptFile.fileName, spawnData[i].optFile, stdFileStrLen);
    if (!tempOptFile.ReadSimResults(tempOptResults))
      GenAppInternalError("collecting Opts");

    if (i == 0)
    {
      // get header
      runHeaderInfo = tempOptResults;
      resultsInfo = tempOptResults[0];
    }

    OptCaseResultsArray& collectRun = tempOptResults[0].runResults;
    for (int j = 0; j < collectRun.Size(); j++)
    {
      int collectCase = collectRun[j].caseIndex;
      collectedCases[collectCase] = collectRun[j];
    }
  }

  optOutputFile.AddSimRun(runHeaderInfo, resultsInfo, maxOpt, optOutputDesc.addExisting);

  for (int i = 0; i < maxOpt; i++)
  {
    OptCaseResults& nextCase = collectedCases[i];
    if (nextCase.caseIndex != i)
      GenAppInternalError("incomplete opt collection");
    optOutputFile.AddNextCase(nextCase);
  }

  GenAppInfoMsg("nPrev_M", "opt collection complete");
}

static void CollectRange()
{
  GenAppInfoMsg("nPrev_M", "collecting range");

  IO_RangeSimResults baseRngFile;
  DC_RangeSimResults baseRngData;

  CopyString(baseRngFile.fileName, spawnData[0].rngFile, stdFileStrLen);
  if (!baseRngFile.ReadSimResults(baseRngData))
    GenAppInternalError("collecting base range");

  RangeSimRunResults& baseRange = baseRngData[0];

  for (int i = 1; i < spawnData.Size(); i++)
  {
    IO_RangeSimResults nextRngFile;
    DC_RangeSimResults nextRngData;

    CopyString(nextRngFile.fileName, spawnData[i].rngFile, stdFileStrLen);
    if (!nextRngFile.ReadSimResults(nextRngData))
      GenAppInternalError("collecting range data");

    RangeSimRunResults& nextRange = nextRngData[0];

    for (int j = 0; j < baseRange.Size(); j++)
    {
      SC_DoubleArray& baseData = baseRange[j].gridCubeData;
      SC_DoubleArray& nextData = nextRange[j].gridCubeData;
      for (int k = 0; k < baseData.Size(); k++)
        if (RealIsNull(baseData[k]))
          baseData[k] = nextData[k];
    }
  }

  CopyString(baseRngFile.fileName, rangeOutputFile.fileName, stdFileStrLen);
  if (!baseRngFile.AddSimRun(baseRange, rangeOutputDesc.addExisting))
    GenAppInternalError("writing collected range");

  GenAppInfoMsg("nPrev_M", "range collection complete");
}

#ifdef _WIN32
static void SpawnRuns(const char* exePath, const char* mainCfgFile, int nSpawn)
{
  int nRuns = 0;
  if (control.IsForward())
  {
    nRuns = GetnForwardRuns();
  }
  else
  {
    nRuns = GetnOptRuns();
  }

  if (nRuns < nSpawn)
    GenAppInternalError("there must be more nPre cases than run processes");

  spawnData.AllocAndSetSize(nSpawn);

  int lastIndex = -1;
  double ddIndex = double(nRuns) / double(nSpawn);
  double dCurrent = -1.0;
  for (int i = 0; i < nSpawn; i++)
  {
    ProcessData& currProc = spawnData[i];
    currProc.SetFiles(i);
    currProc.startIndex = lastIndex + 1;
    dCurrent += ddIndex;
    currProc.endIndex = int(dCurrent);
    lastIndex = currProc.endIndex;
  }
  spawnData.LastIndex().endIndex = nRuns - 1;

  for (int i = 0; i < nSpawn; i++)
    spawnData[i].UpdateFiles();

  for (int i = 0; i < nSpawn; i++)
  {
    _flushall();
    ProcessData& currProc = spawnData[i];
    currProc.nPID = spawnl( _P_NOWAIT, exePath, exePath,
                            mainCfgFile,
                            currProc.processID,
                            currProc.sStr,
                            currProc.eStr, NULL);
    if (currProc.nPID < 0)
      GenAppInternalError("bad spawn exec");
  }

  bool processesRunning = true;
  while (processesRunning)
  {
    processesRunning = false;
    for (int i = 0; i < nSpawn; i++)
    {
      ProcessData& currProc = spawnData[i];
      if (!currProc.processComplete)
      {
        processesRunning = true;
        _cwait(&currProc.processResults, currProc.nPID, NULL);
        if (currProc.processResults < 0)
          GenAppInternalError("bad spawn return");
        currProc.processComplete = true;
      }
    }
  }

  _flushall();

  GenAppInfoMsg("nPrev_M", "all runs complete");

  if (xyForwardOutputDesc.writeFile)
    CollectXY();

  if (rangeOutputDesc.writeFile)
    CollectRange();

  if (optOutputDesc.writeFile)
    CollectOpt();

  GenAppInfoMsg("nPrev_M", "deleting temporary files");

  for (int i = 0; i < nSpawn; i++)
    spawnData[i].DeleteFiles();

  GenAppInfoMsg("nPrev_M", "all processes completed OK");
}
#endif

static void InitAppData()
{
  // initialize other objects
  UnitsBase::AppUnitsInitialize();
  sample.InitAppGlobals();
  optimize.InitAppGlobals();
  sequence.InitAppGlobals();
  dataCapture.InitAppGlobals();
  parameters.InitAppGlobals();
  outputFiles.InitAppGlobals();

  // kluges for RT range
  DFO_SelectRTRangeGrid::SetDefaultRangeFO(optRangeOutputFO);
  DFO_SelectRTRangeCube::SetDefaultRangeFO(optRangeOutputFO);

}


int main(int argc, char *argv[], char *envp[])
{
  GenAppInfoMsg(appVersionGlobals::versionID, appVersionGlobals::versionDate);
  if (appVersionGlobals::isBeta)
    GenAppWarningMsg(appVersionGlobals::versionID, "is not Qualified!!");

  if ((argc < 2) || (SameString(argv[1], "-h")))
    GenAppInternalError("nPreX cfgFile [[nrun] [pID start end]]");

  time_t stTime;
  if ((argc < 5) && (time(&stTime) != -1))
  {
    tm localTime = *localtime(&stTime);
    char timeLab[80];
    strftime(timeLab, 80, "%x %X", &localTime);
    GenAppInfoMsg("Start Time", timeLab);
  }


  InitAppData();

  if (!nPreFileOperations::OpenFile(argv[1]))
  {
    GenAppInternalError("error on open file");
  }

  bool fileError = false;
  if (!CheckInput(fileError))
    GenAppInternalError("pre-run error check failed - use full nPre to debug");

  switch (argc) {
  case 2: {
    RunSimulation(NULL, -1, -1);
    break;
  }
  case 3: {
    int nruns;
    if (!IsValidInt(argv[2], nruns))
      GenAppInternalError("expecting integer for # of runs");

    if ((nruns < 2) || (nruns > 32))
      GenAppInternalError("# of runs must be > 1 and <=32");
#ifdef _WIN32
    SpawnRuns(argv[0], argv[1], nruns);
#endif
    break;
  }
  case 5: {
    int start, end;
    if (!IsValidInt(argv[3], start))
      GenAppInternalError("expecting integer for start");
    if (!IsValidInt(argv[4], end))
      GenAppInternalError("expecting integer for end");

    RunSimulation(argv[2], start, end);
    break;
  }
  default : {
    GenAppInternalError("nPreX cfgFile [[nrun] [pID start end]]");
    break;
  }
  }

  time_t endTime;
  if ((argc < 5) && (time(&endTime) != -1))
  {
    tm localTime = *localtime(&endTime);
    char timeLab[80];
    strftime(timeLab, 80, "%x %X", &localTime);
    GenAppInfoMsg("End Time", timeLab);

    double deltaMin = difftime(endTime, stTime) / 60.0;
    SC_DecFormat minConv(2);
    minConv.RealToString(deltaMin, timeLab, 80);

    GenAppInfoMsg("Elapsed Minutes", timeLab);
  }
}


