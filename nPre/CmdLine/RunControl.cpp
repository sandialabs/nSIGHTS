///////////////////////////////////////////////////////////////////////////////////
//
// RunControl.cpp
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
#include <genClass/U_Real.h>
#include <genClass/SC_Random.h>
#include <genClass/C_MessageLog.h>

#include <osClass/C_SimErr.h>
#include <osMain/G_Sample.h>
#include <osMain/G_Optimize.h>
#include <osMain/C_Optimize.h>
#include <osMain/G_Vary.h>
#include <osMain/G_OptRange.h>

#include <Var/G_CurveFiles.h>
#include <Var/G_Control.h>
#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>

#include <Var/G_OptOutput.h>
#include <Var/G_ProfileOutput.h>
#include <Var/G_RangeOutput.h>
#include <Var/G_XYOutput.h>
#include <Var/G_OutputFiles.h>

#include <Sim/C_SimCore.h>
#include <Sim/G_SimulatorRadiiOutput.h>

#include <Run/RunCommon.h>

#include "RunControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static char procID[80];


static bool RunForwardCase(RunnableC& gtfmRun,
                           int           caseIndex)
{
    try
    {
        char msgStr[80];
        if (control.IsSampleSim())
        {
            SampVar::SetTrialIndex(caseIndex);
            ConcatInt(caseIndex, "Sample", msgStr, 4, 80);
            GenAppInfoMsg(procID, msgStr);
        }
        else
        {
            if(orderedVary.IsNotEmpty())
            {
                vary.SetVaryIndexes(caseIndex);
                if (!control.IsRange())
                {
                    ConcatInt(caseIndex, "Vary", msgStr, 4, 80);
                    GenAppInfoMsg(procID, msgStr);
                }
                else
                {
                    int msgIncrement = vary.GetnVaryCombinations() / 500;
                    if ((caseIndex % msgIncrement) == 0)
                    {
                        ConcatInt(caseIndex, "Rng", msgStr, 4, 80);
                        GenAppInfoMsg(procID, msgStr);
                    }
                }
            }
        }
        xyForwardOutputFO.SetCaseIndex(caseIndex);

        SC_SetupErr errData;
        if (!gtfmRun.SetupForRun(errData))
        {
            GenAppErrorMsg("RunSetup", errData.GetErrorText());
            return false;
        }

        gtfmRun.Execute();
        gtfmRun.FinalUpdate();
    }
    catch (SimError& err)
    {
        GenAppErrorMsg("RunTime", err.errStr);
        return false;
    }

    return true;
}

#ifdef ADCONSOLEAPP

bool nsRunControl::RunOptMaster(int nSlaves)
{
    optOutputControl.NextCaseSetup(0);
    xyForwardOutputFO.SetCaseIndex(0);

    int fitNumber = 0;

    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        if (optimizeInput.inputFits[i].optimizeOnFit)
        {
            const char* fitID = optimizeInput.inputFits[i].fitObject.objRef->GetID();

            char titleStr[80];
            MakeString(titleStr, "OPT", " Fit: ", fitID, 80);
            GenAppInfoMsg(procID, titleStr);

            xyForwardOutputFO.SetFitData(fitID, fitNumber);

            int lastNrows = optCompleteOutput.GetNRows();
            bool fitOK = false;
            try
            {

                optPack.MasterMultiStageOptRun(i, fitNumber, nSlaves);

                optOutputControl.NextFitComplete(false, *optPack.finalStageOpt);

                fitOK = true; 

            }
            catch (SimError& err)
            {
                GenAppErrorMsg(fitID, err.errStr);
                BadOptFinish(lastNrows);
                optPack.lastErr = err;
            }

            fitNumber++;
        }
    }

            optPack.MPIOptStopSlaves(nSlaves);

    return optOutputControl.NextCaseComplete();
}


bool nsRunControl::RunOptSlave(int slaveRank)
{
    try
    {
        optPack.SlaveMultiStageOptRun(slaveRank);
    }
    catch (SimError& err)
    {
        GenAppErrorMsg("MPISlave", err.errStr);
        return false;
    }
    return true;
}
#endif



static void SetStartEnd(int& startIndex, int& endIndex, int maxIndex)
{
    if (startIndex < 0)
    {
        startIndex = 0;
        endIndex = maxIndex - 1;
    }

    if (endIndex >= maxIndex)
    {
        endIndex = maxIndex - 1;
    }

    if (startIndex > endIndex)
        startIndex = endIndex;
}


static bool ForwardRun(int startIndex, int endIndex, RunnableC& gtfmRun)
{
    SetStartEnd(startIndex, endIndex, GetnForwardRuns());

    for (int i = startIndex; i <= endIndex; i++)
    {
        if (!RunForwardCase(gtfmRun, i))
            return false;

        if (control.IsRange())
        {
            optRangeOutputFO.AddForwardRun();
            if (optRangeOutputFO.DownstreamRequired())
            {
                optRangeOutputFO.CalcAndUpdate();
            }
        }
    }

    return true;
}

//
// ************************************************************************** optimization
//


static bool RunOptCase(int caseIndex)
{
    char runID[40];
    if (OptimizationControlStatics::doOptPerturbations)
    {
        ConcatInt(caseIndex + 1, "Pert", runID, 3, 40);
    }
    else if (control.IsSampleSim())
    {
        ConcatInt(caseIndex + 1, "Samp", runID, 3, 40);
        SampVar::SetTrialIndex(caseIndex);
    }
    else if (orderedVary.IsNotEmpty())
    {
        vary.SetVaryIndexes(caseIndex);
        ConcatInt(caseIndex + 1, "Vary", runID, 3, 40);
    }
    else
        CopyString(runID, "Single", 40);


    optOutputControl.NextCaseSetup(caseIndex);
    if (control.IsRange())
    {
        optRangeOutputFO.FirstOptRunSetup();
    }
    xyForwardOutputFO.SetCaseIndex(caseIndex);


    int fitNumber = 0;

    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        if (optimizeInput.inputFits[i].optimizeOnFit)
        {
            const char* fitID = optimizeInput.inputFits[i].fitObject.objRef->GetID();

            char titleStr[80];
            MakeString(titleStr, runID, " Fit: ", fitID, 80);
            GenAppInfoMsg(procID, titleStr);

            xyForwardOutputFO.SetFitData(fitID, fitNumber);

            int lastNrows = optCompleteOutput.GetNRows();
            bool fitOK = false;
            try
            {

                optPack.MultiStageOptRun(i, caseIndex, fitNumber);


                optOutputControl.NextFitComplete(false, *optPack.finalStageOpt);

                GenAppInfoMsg(procID, "Fit complete");

                fitOK = true;

            }
            catch (SimError& err)
            {
                GenAppErrorMsg(fitID, err.errStr);
                BadOptFinish(lastNrows);
                optPack.lastErr = err;
            }

            if (control.IsRange())
            {
                optRangeOutputFO.AddOptRun(*optPack.finalStageOpt, fitOK);
                if (optRangeOutputFO.DownstreamRequired())
                {
                    optRangeOutputFO.CalcAndUpdate();
                }
            }

            fitNumber++;
        }
    }

    return optOutputControl.NextCaseComplete();
}


static bool OptRun(int startIndex, int endIndex)
{
    SetStartEnd(startIndex, endIndex, GetnOptRuns());
    try
    {
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (!RunOptCase(i))
                return false;
        }
    }
    catch (SimError& err)
    {
        GenAppErrorMsg("MultOptRun", err.errStr);
        return false;
    }

    return true;
}


static bool simulationOK;
RunnableC* gtfmRun;


void nsRunControl::RunSimulation(const char* procPrefix, int startIndex, int endIndex)
{
    if (!PrepareSimulation(procPrefix))
        return;

    GenAppInfoMsg(procID, "run started...");

    gasDebugFlag = false;
   #ifdef _DEBUG
    gasDebugFlag = true;
   #endif

    if (control.IsForward())
    {
        simulationOK = ForwardRun(startIndex, endIndex, *gtfmRun);
    }
    else
    {
        simulationOK = OptRun(startIndex, endIndex);
    }

    CompleteSimulation();
}

static void SimulatorUpdateProc()
{
}

static void OptimizerUpdateProc(Optimizer& opt)
{
#ifdef _DEBUG
    char iStr[40];
    char tempStr[80];
    IntToString(opt.runCount, iStr, 40);
    MakeString(tempStr, opt.opMessage," Count: ", iStr, 80);
    GenAppInfoMsg("Debug", tempStr);
#endif
}

bool nsRunControl::PrepareSimulation(const char* procPrefix)
{
    MakeString(procID, "nPreX", procPrefix, 80);

    if (IsNotNull(procPrefix))
    {
        if (rangeOutputDesc.writeFile)
        {
            rangeOutputDesc.addExisting = false;
            if (!AddFileNameSuffix(rangeOutputFile.fileName, ".nRng", procPrefix, stdFileStrLen))
            {
                GenAppErrorMsg("Init", "range output file name extension invalid");
                return false;
            }
        }
        if (optOutputDesc.writeFile)
        {
            optOutputDesc.addExisting = false;
            if (!AddFileNameSuffix(optOutputFile.fileName, ".nOpt", procPrefix, stdFileStrLen))
            {
                GenAppErrorMsg("Init", "opt output file name extension invalid");
                return false;
            }
        }
        if (xyForwardOutputDesc.writeFile)
        {
            xyForwardOutputDesc.addExisting = false;
            if (!AddFileNameSuffix(xyForwardOutputFile.fileName, ".nXYSim", procPrefix, stdFileStrLen))
            {
                GenAppErrorMsg("Init", "opt output file name extension invalid");
                return false;
            }
        }
    }

    gtfmRun = SimulatorCore::GetSimulator();
    gtfmRun->RegisterFinalFlag(xyForwardOutputFO.doingFinalUpdate);

    simulationOK = false;
    if (!gtfmRun->AllocateAll(SimulatorUpdateProc))
    {
        GenAppErrorMsg("PrepareSim", "simulator allocation failure");
    }
    else
    {
        if (control.IsForward())
        {
            simulationOK = true;
        }
        else
        {
            optCompleteOutput.Setup();
            optCompleteOutput.CalcDownStreamObjects();  // clear dependent input
            optPack.SetOptimizers();

            bool dummy;
            if (!optPack.Initialize(OptimizerUpdateProc, *gtfmRun))
            {
                // error msg displayed in optPack
            }
            else if (!optOutputControl.OutputSetup(dummy))
            {
                GenAppErrorMsg("OptInit", "output setup failure");
            }
            else
            {
                if (OptimizationControlStatics::doOptPerturbations)
                    optPack.firstStageOpt->PerturbationSetup();
                simulationOK = true;
            }
        }
    }


    return simulationOK;
}



bool nsRunControl::RunSimulationCase(int caseIndex)
{
    if (control.IsForward())
    {
        simulationOK = ForwardRun(caseIndex, caseIndex, *gtfmRun);
    }
    else
    {
        simulationOK = OptRun(caseIndex, caseIndex);
    }

    return simulationOK;
}

void nsRunControl::CompleteSimulation()
{
    if (simulationOK)
    {
        xyForwardOutputFO.SimulationComplete();
        rangeOutputControl.SimulationComplete();
        if (control.IsForward())
            profileOutputFO.SimulationComplete();
        GenAppInfoMsg(procID, "run complete OK");
    }
    else
    {
        xyForwardOutputFO.SimulationAbort();
        rangeOutputControl.SimulationAbort();
        if (control.IsForward())
            profileOutputFO.SimulationAbort();
        GenAppInfoMsg(procID, "run failure");
    }

    delete gtfmRun;
}

