///////////////////////////////////////////////////////////////////////////////////
//
// RunCommon.cpp
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
//      Performs simulator execution according to settings and manages progress dialog.
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/C_MessageLog.h>

#include <osMain/G_Sample.h>
#include <osMain/G_Optimize.h>
#include <osMain/G_Vary.h>
#include <osMain/G_OptRange.h>

#include <Var/G_Control.h>
#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>

#include <Var/G_OptOutput.h>
#include <Var/G_XYOutput.h>
#include <Var/G_OutputFiles.h>

#include "RunCommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void PreRunError(const char* errStr)
{
    GenAppErrorMsg("PreRun", errStr);
}

bool nsRunControl::CheckInput(bool& fileError)
{
    fileError = false;
    bool retVal = false;
    bool errorDisplayed = false;

    // set fit base setting
    DFO_FitBase::useFitsForOptimization = control.IsOptimization();

    if ((!control.IsSampleSim()) && (!vary.VaryOK()))
    {
        PreRunError("suite/range setup bad");
    }
    if (!parameters.ParametersOK())
    {
        PreRunError("parameter setup bad");
    }
    else if (!sequence.SequencesOK())
    {
        PreRunError("sequence setup bad");
    }
    else if (!dataCapture.DataCaptureOK())
    {
        PreRunError("data capture setup bad");
    }
    else if (control.IsOptimization() && (!optimize.OptimizeOK()))
    {
        PreRunError("optimize setup bad");
    }
    else if (control.IsRange() && (!optimize.FitCalcOK()))
    {
        PreRunError("range fit calculations bad");
    }
    else if (control.IsSampleSim() && (!sample.Sample()))
    {
        PreRunError("sampling setup bad");
    }
    else if (control.IsRange() && (!optRangeOutputFO.PreCalcSetup(control.IsOptimization())))
    {
        PreRunError("range variable allocations bad - any fits defined?");
    }
    else if (!outputFiles.OutputFilesOK(errorDisplayed))  // should be last called
    {
        fileError = errorDisplayed;
        if (!errorDisplayed)
        {
            PreRunError("output file setup bad");
            fileError = true;
        }
    }
    else
        retVal = true;

    if (retVal)
        MessageLogC::AddOtherMessage("Run", "pre-run checks passed OK");

    return retVal;
}


int nsRunControl::GetnForwardRuns()
{
    if (control.IsSampleSim())
        return SampVar::GetnTrials();

    return vary.GetnVaryCombinations();
}

int nsRunControl::GetnOptRuns()
{
    if (OptimizationControlStatics::doOptPerturbations)
        return OptimizationControlStatics::nPerturb;

    if (control.IsSampleSim())
        return SampVar::GetnTrials();

    if (orderedVary.IsNotEmpty())
        return vary.GetnVaryCombinations();

    return 1;
}

int nsRunControl::GetnOptFits()
{
    return GetnOptRuns() * optimizeInput.GetNFits();
}

OptimizerPackage nsRunControl::optPack;

void nsRunControl::BadOptFinish(int lastNRows)
{
    optOutputControl.NextFitComplete(true, *optPack.lastOptRun);

    // add last result iff no update during opt
    if (lastNRows == optCompleteOutput.GetNRows())
    {
        optCompleteOutput.CalcAndUpdate();  // assuming best fit was last

        // try and update XY file
        if (xyForwardOutputFO.DoingOutput())
        {
            optPack.lastOptRun->OptFinalUpdate();
        }
    }
}


void nsRunControl::RunMessage(const char* msg)
{
    MessageLogC::AddOtherMessage("Run", msg);
}

