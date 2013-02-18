///////////////////////////////////////////////////////////////////////////////////
//
// Collect.cpp
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

#include "StdAfx.h"

#include <genClass/C_MPISupport.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <Var/G_OutputFiles.h>
#include <Var/G_RangeOutput.h>
#include <Var/G_OptOutput.h>
#include <Var/G_XYOutput.h>

#include <CmdLine/Collect.h>
#include <Run/RunCommon.h>

T_SC_Array<nsCollect::ProcessData> nsCollect::ProcessData::spawnData;


nsCollect::ProcessData::ProcessData()
{
    SetToNull(xyFile);
    SetToNull(optFile);
    SetToNull(rngFile);
    startIndex = -1;
    endIndex = -1;
    processComplete = false;
}

void nsCollect::ProcessData::ReplaceExtension(char* outfName,
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

void nsCollect::ProcessData::SetFiles(int runIndex)
{
    ConcatInt(runIndex, "_S", processID, 2, 40);

    if (rangeOutputDesc.writeFile)
        ReplaceExtension(rngFile, rangeOutputFile.fileName, processID, ".nRng");

    if (optOutputDesc.writeFile)
        ReplaceExtension(optFile, optOutputFile.fileName, processID, ".nOpt");

    if (xyForwardOutputDesc.writeFile)
        ReplaceExtension(xyFile, xyForwardOutputFile.fileName, processID, ".nXYSim");
}

void nsCollect::ProcessData::UpdateFiles()
{
    IntToString(startIndex, sStr, 20);
    IntToString(endIndex, eStr, 20);
}

void nsCollect::ProcessData::DeleteFiles()
{
    DoDelete(rngFile);
    DoDelete(optFile);
    DoDelete(xyFile);
}

void nsCollect::ProcessData::DoDelete(char* delfName)
{
    if (IsNotNull(delfName))
    {
        GenAppInfoMsg("Deleting file", delfName);
       #ifdef _WIN32
        DeleteFile(delfName);
       #else
        remove(delfName);
       #endif
    }
}

void nsCollect::CollectXY()
{
    GenAppInfoMsg("nPreX", "collecting XY");

    XYSimRunResults&  collectedXY  =   xyForwardOutputFO.runResultData;

    for (int i = 0; i < ProcessData::spawnData.Size(); i++)
    {
        IO_XYSimResults tempXYFile;
        DC_XYSimResults tempXYResults;
        CopyString(tempXYFile.fileName, ProcessData::spawnData[i].xyFile, stdFileStrLen);
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

    GenAppInfoMsg("nPreX", "XY collection complete");
}


void nsCollect::CollectOpt()
{
    GenAppInfoMsg("nPreX", "collecting Opt");

    int maxOpt = GetnOptRuns();
    OptCaseResultsArray collectedCases;
    collectedCases.AllocAndSetSize(maxOpt);

    OptSimResultsHeader  runHeaderInfo;
    OptSimRunResults     resultsInfo;

    for (int i = 0; i < ProcessData::spawnData.Size(); i++)
    {
        IO_OptSimResults tempOptFile;
        DC_OptSimResults tempOptResults;

        CopyString(tempOptFile.fileName, ProcessData::spawnData[i].optFile, stdFileStrLen);
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

    bool optCollectOK = true;
    for (int i = 0; i < maxOpt; i++)
    {
        OptCaseResults& nextCase = collectedCases[i];
        if (nextCase.caseIndex != i)
        {
            optCollectOK = false;
            char errMsg[80];
            ConcatInt(i, "missing opt case ", errMsg, 1, 80);
            GenAppErrorMsg("Opt Collect", errMsg); 
        }
        optOutputFile.AddNextCase(nextCase);
    }

    if (!optCollectOK)
        GenAppInternalError("incomplete opt collection");

    GenAppInfoMsg("nPreX", "opt collection complete");
}

void nsCollect::CollectRange()
{
    GenAppInfoMsg("nPreX", "collecting range");

    IO_RangeSimResults baseRngFile;
    DC_RangeSimResults baseRngData;

    CopyString(baseRngFile.fileName, ProcessData::spawnData[0].rngFile, stdFileStrLen);
    if (!baseRngFile.ReadSimResults(baseRngData))
        GenAppInternalError("collecting base range");

    RangeSimRunResults& baseRange = baseRngData[0];

    for (int i = 1; i < ProcessData::spawnData.Size(); i++)
    {
        IO_RangeSimResults nextRngFile;
        DC_RangeSimResults nextRngData;

        CopyString(nextRngFile.fileName, ProcessData::spawnData[i].rngFile, stdFileStrLen);
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

    GenAppInfoMsg("nPreX", "range collection complete");
}


