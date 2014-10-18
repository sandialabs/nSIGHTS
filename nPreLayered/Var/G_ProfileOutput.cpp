///////////////////////////////////////////////////////////////////////////////////
//
// G_ProfileOutput.cpp
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
#include <genClass/U_File.h>

#include <Var/C_Units.h>

#include <Var/G_Control.h>
#include <Var/G_DataCapture.h>
#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>

#include <osMain/G_Vary.h>
#include <Var/G_OutputFiles.h>

#include <Var/G_ProfileOutput.h>

namespace nsOutputFiles  {


IO_ProfileSimResults    profileOutputFile;        // includes file name
ProfileForwardOutputFO    profileOutputFO;          // data to output -- defaults to data capture XY Array output


//  ********************* ProfileForwardOutputFO

ProfileForwardOutputFO::ProfileForwardOutputFO() :
  FuncObjC("WriteProfileOutput")
{
  AddUseFlagInPort(seqTimesObjRef, typeid(DO_SequenceTimes), profileOutputDesc.writeFile);
  AddUseFlagInPort(staticPressureObjRef, typeid(DO_Real), profileOutputDesc.writeFile);
  profileData.SetID("Profile");
  profileData.SetXID("Time");
  profileData.SetYID("Radius");
  DoStatusChk();
}

void ProfileForwardOutputFO::CalcNCase()
{
  // forward normal only
  nCase = vary.GetnVaryCombinations();
  ncasePar = orderedVary.Size();
  if (orderedVary.IsEmpty())
    ncasePar = 1;
}


bool  ProfileForwardOutputFO::OutputSetup(bool& addOK)
{
  addOK = true;
  doingOutput = false;
  if (!(profileOutputDesc.writeFile && control.IsNormal() && (!control.IsOptimization())))
    return true;

  DoStatusChk();
  if (StatusNotOK())
    return false;

  doingOutput = true;
  CopyString(runResultData.runID, profileOutputDesc.runID, ProfileSimRunResults::runIDLen);

  CalcNCase();
  runResultData.caseValDesc.Alloc(ncasePar);
  runResultData.caseValShortDesc.Alloc(ncasePar);

  if (orderedVary.IsEmpty())
  {
    runResultData.caseValDesc += "Single run";
    runResultData.caseValShortDesc += "Single run";
  }
  else
  {
    for (int i = 0; i < orderedVary.Size(); i++)
    {
      runResultData.caseValDesc += orderedVary[i]->GetLongID();
      runResultData.caseValShortDesc += orderedVary[i]->GetShortID();
    }
  }

  if (!runResultData.ChkAlloc(nCase))
  {
    doingOutput = false;
    return false;
  }

  radMod = profileOutputDesc.radiusNodeModulus;
  tsMod = profileOutputDesc.timeStepModulus;

  int maxRadialNodes = allParam[pN_n_r]->GetIntMaxPar() / radMod + 5;

  int startSeq = profileOutputDesc.startSeq;
  if ((startSeq < 0) || (startSeq >= sequence.GetNSingleSequence()))
    startSeq = 0;

  int endSeq = profileOutputDesc.endSeq;
  if ((endSeq < 0) || (endSeq >= sequence.GetNSingleSequence()) || (endSeq < startSeq))
    endSeq = sequence.GetNSingleSequence() - 1;

  int maxTS = 0;
  profileData.xData.dataIsLog = false;
  for (int  i = startSeq; i <= endSeq; i++)
    if (singleSequenceData.IsValid(i))
    {
      if (singleSequenceData[i]->timeStep.timeStepType != TimeStep::tstStatic)
        profileData.xData.dataIsLog = true;
      maxTS += singleSequenceData[i]->GetNTimeStep();
    }

  maxTS = maxTS / tsMod + 5;

  startTime = sequenceTimesDC->sequenceStartTimes[startSeq];
  endTime = sequenceTimesDC->sequenceStartTimes[endSeq + 1];


  doExtended = control.Is2DRadial();
  if (doExtended)
  {
    int maxverticalNodes = allParam[pN_n_v2Dw]->GetIntMaxPar()
                           + allParam[pN_n_v2Du]->GetIntMaxPar()
                           + allParam[pN_n_v2Dl]->GetIntMaxPar() + 1;
    try
    {
      extendedProfileData.Alloc(maxTS);
      extendedProfileData.nodeRadii.Alloc(maxRadialNodes);
      extendedProfileData.constantVertZPos.Alloc(maxverticalNodes);
      waterTablePressure.Alloc(maxRadialNodes); // temporary
      for (int i = 0; i < maxTS; i++)
      {
        extendedProfileData[i].waterTablePos.Alloc(maxRadialNodes);
        extendedProfileData[i].nodalPressures.MatrixAlloc(maxverticalNodes, maxRadialNodes);
      }
    }
    catch (SC_Array::AllocationError& )
    {
      doingOutput = false;
      return false;
    }

  }
  else
  {
    if (!profileData.Alloc(maxTS, maxRadialNodes))
    {
      doingOutput = false;
      return false;
    }
    profileData.yData.dataIsLog = true;
  }

  runResultData.caseSeqData = *(sequenceTimesDC);
  return true;
}


void ProfileForwardOutputFO::CaseInit(const SC_DoubleArray& nodeRadius,
                                      const SC_DoubleArray& constantVertNodeZ,
                                      const double&     currThicknessToPressure)
{
  if (!doingOutput)
    return;

  doExtended = control.Is2DRadial();
  timeStepCount = 0;
  if (doExtended)
  {
    thicknessToPressureConversion = currThicknessToPressure;

    const UnitIndex distConv(uDist);
    AddNodeData(nodeRadius, extendedProfileData.nodeRadii, distConv);

    extendedProfileData.nconstantVert = constantVertNodeZ.Size();
    for (int i = 0; i < extendedProfileData.nconstantVert; i++)
      extendedProfileData.constantVertZPos[i] = distConv.MetricToUser(constantVertNodeZ[i]);
    extendedProfileData.constantVertZPos.SetSize(extendedProfileData.nconstantVert);
    extendedProfileData.nradial = extendedProfileData.nodeRadii.Size();

    extendedProfileData.SetEmpty();
  }
  else
  {
    AddNodeData(nodeRadius, profileData.yData, UnitIndex(uDist));
    profileData.xData.SetEmpty();
    profileData.gData.SetEmpty();
  }
}

void ProfileForwardOutputFO::CaseAddTimeStep(const double&      currTime,
    const SC_DoubleArray& nodePressure,
    const SC_DoubleArray&  uncThickness,
    const SC_DoubleMatrix& uncPressure)
{
  if (!doingOutput)
    return;

  UnitIndex uConv(uTime);
  double userTime = uConv.MetricToUser(currTime);

  if ((userTime < startTime) || (userTime > endTime))
    return;

  timeStepCount++;
  if (timeStepCount < tsMod)
    return;

  if (doExtended)
  {
    if (extendedProfileData.IsFull())
      return;

    extendedProfileData.ntotalVert = uncPressure.Size();
    extendedProfileData.nvariableVert = 0;
    if (control.IsUnconfined())
    {
      extendedProfileData.ntotalVert++; // extra for water table pos
      extendedProfileData.nvariableVert =  extendedProfileData.ntotalVert - extendedProfileData.nconstantVert;
    }

    extendedProfileData.nlistData++;
    ExtendedProfileTimeData& nextProf = extendedProfileData.LastIndex();

    nextProf.profileTime = userTime;

    if (control.IsUnconfined())
    {
      int nvert = extendedProfileData.ntotalVert - 1;
      AddNodeData(uncThickness, nextProf.waterTablePos, UnitIndex(uDist));
      for (int i = 0; i < nvert; i++)
        AddNodeData(uncPressure[i], nextProf.nodalPressures[i], UnitIndex(uPress));

      // process water table according to head setting
      if (NSUnitsBase::headFlag)
      {
        AddNodeData(uncThickness, nextProf.nodalPressures[nvert], UnitIndex(uDist));
      }
      else
      {
        for (int i = 0; i < uncThickness.Size(); i++)
          waterTablePressure[i] = uncThickness[i] * thicknessToPressureConversion;
        waterTablePressure.SetSize(uncThickness.Size());
        AddNodeData(waterTablePressure, nextProf.nodalPressures[nvert], UnitIndex(uPress));
      }
    }
    else
    {
      for (int i = 0; i < extendedProfileData.ntotalVert; i++)
        AddNodeData(uncPressure[i], nextProf.nodalPressures[i], UnitIndex(uPress));
    }

    nextProf.nodalPressures.SetSize(extendedProfileData.ntotalVert);

  }
  else
  {
    profileData.xData += userTime;

    int nextG = profileData.gData.Size();

    AddNodeData(nodePressure, profileData.gData[nextG], UnitIndex(uPress));

    profileData.gData.SetSize(nextG + 1);
  }


  timeStepCount = 0;
}


void ProfileForwardOutputFO::AddNodeData(const SC_DoubleArray& nodeInput,
    SC_DoubleArray& nodeOutput,
    const UnitIndex&   uConv)
{
  int next = 0;
  for (int i = 0; i < nodeInput.Size(); i++)
    if ((i % radMod) == 0)
      nodeOutput[next++] = uConv.MetricToUser(nodeInput[i]);

  // always get the last
  if ((nodeInput.UpperBound() % radMod) != 0)
    nodeOutput[next++] = uConv.MetricToUser(nodeInput.LastIndex());

  nodeOutput.SetSize(next);
}


void ProfileForwardOutputFO::SimulationComplete()
{
  if (!doingOutput)
    return;

  if (!runResultData.IsEmpty())
    profileOutputFile.AddSimRun(runResultData, profileOutputDesc.addExisting);

  Cleanup();
}


void ProfileForwardOutputFO::Cleanup()
{
  FileOutputControl::Cleanup();
  runResultData.DeAlloc();
}

void ProfileForwardOutputFO::DoStatusChk()
{
  // default set
  FuncObjC::DoStatusChk();
  sequenceTimesDC = 0;
  staticPressure  = 0.0;

  if (!profileOutputDesc.writeFile)
    return;

  if (CheckInputFO(seqTimesObjRef, "Sequence time data source"))
    sequenceTimesDC = static_cast<DO_SequenceTimes*>(GetInPortData(seqTimesObjRef))->seqTimes;

  if (CheckInputFO(staticPressureObjRef, "Static pressure"))
    staticPressure = static_cast<DO_Real*>(GetInPortData(staticPressureObjRef))->realVal;

  char pathName[stdFileStrLen];
  ExtractFilePath(pathName, profileOutputFile.fileName, stdFileStrLen);
  if (!PathExists(pathName))
  {
    char pathErr[120];
    MakeString(pathErr, "Profile file path is bad:", pathName, 120);
    SetObjErrMsg(pathErr);
    return;
  }
}

void  ProfileForwardOutputFO::CalcOutput(FOcalcType  calcType)
{
  if (!doingOutput)
    return;

  DoStatusChk();
  if (!StatusOK())
    GenAppInternalError("ProfileForwardOutputFO::CalcOutput");

  // next case results
  runResultData.SetSize(runResultData.Size()+1);

  ProfileSimCaseResults& currCase = runResultData.LastIndex();
  currCase.caseValues.Alloc(ncasePar);
  currCase.caseID[0] = '\0';

  // make description
  if (orderedVary.IsEmpty())
  {
    ConcatString(currCase.caseID, "only case", ProfileSimCaseResults::caseIDLen);
    currCase.caseValues += 0.0;
  }
  else
  {
    vary.GetCaseID(currCase.caseID, ProfileSimCaseResults::caseIDLen);
    vary.GetCaseValues(currCase.caseValues);
  }

  currCase.caseDataIsExtended = doExtended;
  if (doExtended)
    currCase.extendedCaseData = extendedProfileData;
  else
    currCase.caseData = profileData;
  currCase.staticPressure = staticPressure;
}



};  // end namespace


