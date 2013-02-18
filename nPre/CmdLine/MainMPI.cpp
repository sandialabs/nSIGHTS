///////////////////////////////////////////////////////////////////////////////////
//
// MainMPI.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/C_MPISupport.h>
#include <genApp/MPIAppSupport.h>

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

#include <osDFO/DFO_SelectRTRangeGrid.h>
#include <osDFO/DFO_SelectRTRangeCube.h>

#include <App/nPreFile.h>
#include <Run/RunCommon.h>
#include <CmdLine/RunControl.h>
#include <CmdLine/Collect.h>
#include <iostream>

static bool mainIsDebug = false;

int MPIAppSupport::MPIGetNCases()
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


    return nRuns;
}

bool MPIAppSupport::MPIOptimizationRun()
{
    return !control.IsForward() && (GetnOptRuns() == 1);
}

void MPIAppSupport::MPIRunOptimizationMaster(int nSlaves)
{
    RunOptMaster(nSlaves);
}
void MPIAppSupport::MPIRunOptimizationSlave(int slaveRank)
{
    RunOptSlave(slaveRank);
}

void MPIAppSupport::MPISendCompleteToSlaves(int nSlaves)
{
    MPIOptStopSlaves(nSlaves);
}

void MPIAppSupport::MPIMasterInit(int nSlaves) 
{
    if (MPIAppSupport::MPIOptimizationRun())
    {
        if (!PrepareSimulation("_X"))
            GenAppInternalError("error on MPIMasterInit()");
    }
    else
    {
        ProcessData::spawnData.AllocAndSetSize(nSlaves);
        //initialize & start first set of processes
        for (int i = 0; i  < nSlaves; i++)
            ProcessData::spawnData[i].SetFiles(i + 1);
    }
}

void MPIAppSupport::MPIMasterCleanup(int nSlaves)
{
    if (MPIAppSupport::MPIOptimizationRun())
    {
        if (xyForwardOutputDesc.writeFile)
            xyForwardOutputFO.SimulationComplete();
        return; 
    }

    GenAppInfoMsg("nPreX", "collecting output files ...");

    // kluge - only write collection messages if in debug
    // by default all master msgs are displayed
    NodeFile::mpiDebugRun = mainIsDebug;

    if (xyForwardOutputDesc.writeFile)
        CollectXY();

    if (rangeOutputDesc.writeFile)
        CollectRange();

    if (optOutputDesc.writeFile)
        CollectOpt();

    if (!NodeFile::mpiDebugRun)
    {
        GenAppInfoMsg("nPreX", "deleting temporary files");
        for (int i = 0; i < nSlaves; i++)
            ProcessData::spawnData[i].DeleteFiles();
    }

    // remaining masters msgs OK
    NodeFile::mpiDebugRun = true;
}

void MPIAppSupport::MPISlaveInit(int slaveRank)
{
    char processID[40];
    ConcatInt(slaveRank, "_S", processID, 2, 40);
    if (!PrepareSimulation(processID))
        GenAppInternalError("error on MPISlaveInit()");
}

void MPIAppSupport::MPIRunOneCase(int caseIndex)
{
    RunSimulationCase(caseIndex);
}

void MPIAppSupport::MPISlaveCleanup()
{
    CompleteSimulation();
}
#include <iostream>
using namespace std;

void MPIAppSupport::MPIAppInit(int argc, char *argv[], bool isSlave)
{
    cout<<"MPIAppInit()"<<endl;
    // force messages for debug
    if (argc == 3)  // don't actually care what the argument is ...
    {
        NodeFile::mpiDebugRun = true;
        mainIsDebug = true;
    }

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

    GenAppInfoMsg("Opening config file", argv[1]);
    if (!nPreFileOperations::OpenFile(argv[1]))
        GenAppInternalError("error on open file");

    if (isSlave && MPIOptimizationRun())
    {
        xyForwardOutputDesc.writeFile = false;
        optOutputDesc.writeFile = false;
        rangeOutputDesc.writeFile = false;
        profileOutputDesc.writeFile = false;
    }

    bool fileError = false;
    if (!CheckInput(fileError))
        GenAppInternalError("pre-run error check failed - use full nPre to debug");
}


int main(int argc, char *argv[], char *envp[])
{
    cout<<"made it here"<<endl;
    if ((argc < 2) || (SameString(argv[1], "-h")))
        GenAppInternalError("mpiexec -np [nSlave] nPreX cfgFile [Debug]");

    MPIAppSupport::MPIMain(argc, argv, "nPreX");
}


