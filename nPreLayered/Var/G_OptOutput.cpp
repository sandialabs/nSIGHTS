///////////////////////////////////////////////////////////////////////////////////
//
// G_OptOutput.cpp
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
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>
#include <genClass/C_ConvFile.h>

#include <Var/G_Control.h>

#include <osMain/G_Optimize.h>
#include <osMain/G_Vary.h>

#include <Var/G_OutputFiles.h>

#include <Var/G_OptOutput.h>

namespace nsOutputFiles  {


// declarations
IO_OptSimResults            optOutputFile;
OptOutputControl            optOutputControl;

//  ********************* optimizer output

bool OptOutputControl::OutputSetup(bool& addOK)
{
  addOK = true;
  if (!optOutputDesc.writeFile)
    return true;

  maxCase = 1;
  ncasePar = 0;
  if (optimize.doOptPerturbations)
  {
    maxCase = optimize.nPerturb;
    ncasePar = OptVar::GetnActive();
  }
  else if (control.IsSampleSim())
  {
    maxCase = SampVar::GetnTrials();
    ncasePar = SampVar::GetnActive();
  }
  else if (orderedVary.IsEmpty())
  {
    ncasePar = 1;
  }
  else
  {
    ncasePar = orderedVary.Size();
    maxCase = vary.GetnVaryCombinations();
  }

  currCaseIndex = 0;
  runHeaderWritten = false;
  caseHeaderWritten = false;

  OptSimResultsHeader runDesc;
  SetRunDesc(runDesc);

  if ((!optOutputDesc.addExisting) ||(!ConvFileC::CFfileExists(optOutputDesc.fileName)))
    return true;

  addOK = optOutputFile.OKtoAdd(runDesc);

  return addOK;
}


void OptOutputControl::NextCaseSetup(int caseIndex)
{
  currCaseIndex = caseIndex;
  caseHeaderWritten = false;
}

void OptOutputControl::NextFitComplete(      bool       fitAborted,
    const Optimizer& opt)
{
  if (!optOutputDesc.writeFile)
    return;

  if (!runHeaderWritten)
  {
    FirstFitWriteOK(opt);
    runHeaderWritten = true;
  }
  if (!caseHeaderWritten)
  {
    SetupCaseHeader(opt);
    caseHeaderWritten = true;
  }

  optFileOutput.AddNextFit(fitAborted, opt);
}

bool OptOutputControl::NextCaseComplete()
{
  if (!optOutputDesc.writeFile)
    return true;

  return optOutputFile.AddNextCase(optFileOutput.nextCase);
}

void OptOutputControl::SimulationComplete()
{
  if (!optOutputDesc.writeFile)
    return;
  Cleanup();
}

void OptOutputControl::Cleanup()
{
  optFileOutput.Cleanup();
  FileOutputControl::Cleanup();
}



void OptOutputControl::SetRunDesc(OptSimResultsHeader&  runDesc)
{
  runDesc.containsCovariance = optOutputDesc.writeCovariance && optimize.HasCovariance();
  runDesc.containsResiduals  = optOutputDesc.writeResiduals;
  runDesc.containsJacobian   = optOutputDesc.writeJacobian && optimize.HasCovariance();
  runDesc.noptVar            = OptVar::GetnActive();
  runDesc.ncalcVar       = optimizeInput.GetnCalc();

  if (optimize.doOptPerturbations)
  {
    runDesc.runType = OptSimResultsHeader::rtPerturb;
  }
  else if (control.IsSampleSim())
  {
    runDesc.runType = OptSimResultsHeader::rtSample;
  }
  else if (orderedVary.IsEmpty())
  {
    runDesc.runType = OptSimResultsHeader::rtSingle;
  }
  else
  {
    runDesc.runType = OptSimResultsHeader::rtMultiple;
  }

}

bool OptOutputControl::FirstFitWriteOK(const Optimizer& opt)
{
  if (!optOutputDesc.writeFile)
    return true;

  OptSimResultsHeader runDesc;
  SetRunDesc(runDesc);

  CaseVarDescriptionArray caseDesc;
  caseDesc.Alloc(ncasePar);
  CaseVarDescription temp;
  if (optimize.doOptPerturbations)
  {
    OptVarArray currOpt;
    for (int i = 0; i < currOpt.Size(); i++)
    {
      MakeString(temp.caseVarDesc, "P_", currOpt[i]->GetLongID(), CaseVarDescription::caseVarDescLen);
      MakeString(temp.caseVarShortDesc, "P_", currOpt[i]->GetShortID(), CaseVarDescription::caseVarShortDescLen);
      temp.varIsLinear = currOpt[i]->optIsLinear;
      currOpt[i]->MakeUnitString(temp.caseVarUnits, CaseVarDescription::caseVarUnitsLen);
      caseDesc += temp;
    }
  }
  else if (control.IsSampleSim())
  {
    SampVarArray currSamp;
    for (int i = 0; i < currSamp.Size(); i++)
    {
      CopyString(temp.caseVarDesc, currSamp[i]->GetLongID(), CaseVarDescription::caseVarDescLen);
      CopyString(temp.caseVarShortDesc, currSamp[i]->GetShortID(), CaseVarDescription::caseVarShortDescLen);
      temp.varIsLinear = currSamp[i]->IsLinear();
      currSamp[i]->MakeUnitString(temp.caseVarUnits, CaseVarDescription::caseVarUnitsLen);
      caseDesc += temp;
    }
  }
  else if (orderedVary.IsEmpty())
  {
    CopyString(temp.caseVarDesc, "Only case", CaseVarDescription::caseVarDescLen);
    CopyString(temp.caseVarShortDesc, "Only case", CaseVarDescription::caseVarShortDescLen);
    caseDesc += temp;
  }
  else
  {
    SC_SciFormat conv(6);
    for (int i = 0; i < orderedVary.Size(); i++)
    {
      VaryVar& currVary = *(orderedVary[i]);

      CopyString(temp.caseVarDesc, currVary.GetLongID(), CaseVarDescription::caseVarDescLen);
      ConcatString(temp.caseVarDesc, " " , CaseVarDescription::caseVarDescLen);

      char convStr[40];
      conv.RealToString(currVary.GetFinalVaryVal(), convStr, 40);
      ConcatString(temp.caseVarDesc, convStr, CaseVarDescription::caseVarDescLen);

      CopyString(temp.caseVarShortDesc, currVary.GetShortID(), CaseVarDescription::caseVarShortDescLen);
      currVary.MakeUnitString(temp.caseVarUnits, CaseVarDescription::caseVarUnitsLen);

      temp.varIsLinear = currVary.rangeIsLinear;

      caseDesc += temp;
    }

  }

  // do the major setup work
  optFileOutput.SetupRunHeader(optOutputDesc.runID, runDesc, caseDesc, opt);

  // open the file and setup header
  return optOutputFile.AddSimRun(runDesc, optFileOutput.runHeader, maxCase, optOutputDesc.addExisting);
}

void OptOutputControl::SetupCaseHeader(const Optimizer& opt)
{
  if (!optOutputDesc.writeFile)
    return;

  SC_DoubleArray caseVals(ncasePar);
  caseVals.SetEmpty();
  char caseDesc[80];

  if (optimize.doOptPerturbations)
  {
    double dPar = 0.0;
    for ( int i = 0; i < opt.noptVars; i++)
    {
      OptVar& currOpt = (*opt.varsToOptimize[i]);
      double startVal = currOpt.DeNormalize(opt.startNormParEst[i]) * currOpt.GetFinalMultiplier();
      caseVals += startVal;
      dPar += fabs(opt.startNormParEst[i] - opt.bestNormParEst[i]);
    }
    dPar /= double(opt.noptVars);

    ConcatInt(currCaseIndex+1, "Pert#", caseDesc, 4, 80);

    SC_DecFormat parForm(3);
    char tempStr[20];
    parForm.RealToString(dPar, tempStr, 20);
    ConcatString(caseDesc, "  ", 80);
    ConcatString(caseDesc, tempStr, 80);
  }
  else if (control.IsSampleSim())
  {
    SampVarArray currSamp;
    for (int i = 0; i < currSamp.Size(); i++)
      caseVals += currSamp[i]->GetFinalSampleVal();

    ConcatInt(currCaseIndex+1, "Sample#", caseDesc, 4, 80);
  }
  else if (orderedVary.IsEmpty())
  {
    caseVals += 0.0;
    CopyString(caseDesc, "Only case", 80);
  }
  else
  {
    CopyString(caseDesc, "Case# ", 80);
    for (int i = 0; i < orderedVary.Size(); i++)
    {
      caseVals += orderedVary[i]->GetFinalVaryVal();
      char tempStr[20];
      IntToString(orderedVary[i]->varyIndex + 1, tempStr, 2, 20);
      ConcatString(caseDesc, tempStr, 80);
      ConcatString(caseDesc, " ", 80);
    }

  }
  optFileOutput.SetupCaseHeader(caseDesc, caseVals, currCaseIndex);
  caseHeaderWritten = true;
}


};  // end namespace


