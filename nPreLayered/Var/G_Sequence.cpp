///////////////////////////////////////////////////////////////////////////////////
//
// G_Sequence.cpp
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


#ifndef ADCONSOLEAPP
#include <App/nPreUtil.h>
#endif

#include <Var/G_PreConfigFile.h>

#include <Var/G_Control.h>
#include <Var/G_Layer.h>

#include <Var/G_Sequence.h>

namespace nsSequence  {


SequenceArray       singleSequenceData;
SequenceArray   layerHistorySequenceData;
SequenceMatrix    layerSequenceData;
TestZoneCurveArray  testZoneCurves;

char        referenceZoneID[IntervalBase::intervalIDLen];


bool                enterSequenceDurations = false;
UnitReal            testStartTime(uTime, 0.0);
UnitReal            layerHistoryEndTime(uTime, 5E+07);
UnitReal            testEndTime(uTime, 1E+08);

UnitReal            maximumPressureChange(uRatio, 0.1);
UnitReal            minimumPressureChange(uRatio, 0.001);
UnitReal            maximumFlowRateChange(uRatio, 0.1);
UnitReal            minimumFlowRateChange(uRatio, 0.001);
int                 maxNumberOfTS = 10000;

bool                isPartialRun        = false;
int                 partialStartSeqVal  = -1;
int                 partialEndSeqVal    = -1;
char                partialStartSeqID[Sequence::seqIDLen];
char                partialEndSeqID[Sequence::seqIDLen];

SequenceStaticSupport   sequence;
SequenceTimesFO         sequenceTimesFO;
SequenceCurveFO     sequenceCurvesFO;
SequenceErrorListing  sequenceErrorListing;

void SequenceArray::SetToInitial()
{
  if (AllocSize() == 0)
    Alloc(16);

  if (IsEmpty())
  {
    // start with a default flow sequence
    CreateIfNecessary(0);
    GetRef(0).seqType = Sequence::stFlow;
    GetRef(0).SetID("F_01");
    SetSize(1);
  }
}

void SequenceArray::SetForFlags()
{
  for (int i = 0; i < Size(); i++)
    if (IsValid(i))
      GetRef(i).SetForFlags();
}

bool SequenceArray::GetSequenceIndex(const char* inID, int& outIndex) const
{
  for (int i = 0; i < Size(); i++)
    if (IsValid(i) && (SameString(GetRef(i).seqID, inID)))
    {
      outIndex = i;
      return true;
    }
  outIndex = -1;
  return false;
}


bool SequenceArray::GetSequenceIDs(const char* preStr, SC_StringArray& ids) const
{
  ids.DeAlloc();
  ids.SetStringLen(Sequence::seqIDLen * 2);
  ids.AllocAndSetSize(Size());
  for (int i = 0; i < Size(); i++)
    if (IsValid(i))
    {
      if (IsNull(preStr))
      {
        CopyString(ids[i], GetRef(i).seqID, Sequence::seqIDLen);
      }
      else
      {
        MakeString(ids[i], preStr, "|", GetRef(i).seqID, Sequence::seqIDLen * 2);
      }
    }

  return !ids.IsEmpty();
}

void SequenceArray::SetSequenceTimes(const double& startTime,
                                     const double& endTime)
{
  bool nullLengthFound = false;
  double currStart =  startTime;
  for (int i = 0; i < Size(); i++)
    if (IsValid(i))
    {
      Sequence& currSeq = GetRef(i);
      currSeq.startTime = currStart;
      if (currSeq.seqLength.IsNull() || nullLengthFound)
      {
        currSeq.endTime = nullReal;
        currStart = nullReal;
        nullLengthFound = true;
      }
      else
      {
        currStart = currStart + currSeq.seqLength.GetMetricVal();
        currSeq.endTime = currStart;
      }
    }

  // set duration of last sequence
  if ((!enterSequenceDurations) && (!IsEmpty()) &&
      (IsValid(UpperBound()) && ((!nullLengthFound) || (Size() == 1))))
  {
    Sequence& currSeq = GetRef(UpperBound());
    currSeq.seqLength.SetMetricVal(endTime - currSeq.startTime);
  }
}

bool SequenceArray::SetSequenceIDs()
{
  int nextFlow  = 1;
  int nextHist  = 1;
  int nextPulse = 1;
  int nextSlug  = 1;
  bool retVal = false;

  for (int i = 0; i < Size(); i++)
    if (IsValid(i))
    {
      int nextVal;
      char nextChar;
      Sequence& currSeq = GetRef(i);
      switch (currSeq.seqType) {
      case Sequence::stFlow :  {
        nextChar = 'F';
        nextVal = nextFlow++;
        break;
      }
      case Sequence::stHistory :  {
        nextChar = 'H';
        nextVal = nextHist++;
        break;
      }
      case Sequence::stPulse :  {
        nextChar = 'P';
        nextVal = nextPulse++;
        break;
      }
      case Sequence::stSlug :  {
        nextChar = 'S';
        nextVal = nextSlug++;
        break;
      }
      }
      if (currSeq.makeDefaultID)
      {
        char nextIStr[10];
        IntToString(nextVal, nextIStr, 2, 10);
        char newDesig[40];
        MakeString(newDesig, "__", nextIStr, 40);
        newDesig[0] = nextChar;
        retVal = retVal || (!SameString(newDesig, currSeq.seqID));
        currSeq.SetID(newDesig);
      }
    }

  return retVal;
}
int SequenceArray::GetNTimeStep() const
{
  int nts = 0;
  for (int i = 0; i < Size(); i++)
    if (IsValid(i))
      nts+= GetRef(i).GetNTimeStep();
  return nts;
}

bool SequenceArray::UsesDynamicTS() const
{
  for (int i = 0; i < Size(); i++)
    if (IsValid(i))
      if (GetRef(i).UsesDynamicTS())
        return true;
  return false;
}

bool SequenceArray::HasSlugBC() const
{
  for (int i = 0; i < Size(); i++)
    if (IsValid(i))
      if (GetRef(i).HasSlugBC())
        return true;
  return false;
}

bool SequenceArray::HasPulseBC() const
{
  for (int i = 0; i < Size(); i++)
    if (IsValid(i))
      if (GetRef(i).HasPulseBC())
        return true;
  return false;
}


void SequenceArray::CheckAndAllocSequence(int seqIndex)
{
  CreateIfNecessary(seqIndex);
}

SequenceStaticSupport::SequenceStaticSupport()
{
  /*  singleSequenceData.SetResizable(16);
    layerHistorySequenceData.SetResizable(16);
    layerSequenceData.Alloc(maxWellboreZones);
    for (int i = 0; i < maxWellboreZones; i++)
      layerSequenceData[i].SetResizable(16);
  */
  SetToNull(partialStartSeqID);
  SetToNull(partialEndSeqID);
}

void SequenceStaticSupport::SetToInitial()
{
  singleSequenceData.DeAlloc();
  singleSequenceData.SetToInitial();

  layerHistorySequenceData.DeAlloc();
  singleSequenceData.SetToInitial();

  layerSequenceData.DeAlloc();
  layerSequenceData.AllocAndSetSize(maxWellboreZones);
  for (int i = 0; i < maxWellboreZones; i++)
    layerSequenceData[i].SetToInitial();

  CopyString(referenceZoneID, "XZONE", IntervalBase::intervalIDLen);

  testZoneCurves.DeAlloc();

  enterSequenceDurations = true;
  testStartTime = UnitReal(uTime, 0.0);
  testEndTime = UnitReal(uTime, 1E+07);

  isPartialRun    = false;
  partialStartSeqVal  = -1;
  partialEndSeqVal  = -1;

  SetToNull(partialStartSeqID);
  SetToNull(partialEndSeqID);
}


void SequenceStaticSupport::SetForFlags()
{
  if (control.IsLayered())
  {
    SC_BoolArray validZones(maxWellboreZones, false);
    SC_BoolArray packerZones(maxWellboreZones, false);
    for (int i = 0; i < wellBoreZones.Size(); i++)
    {
      int seqIndx = wellBoreZones[i].seqIndex;
      if (seqIndx >= 0)
      {
        validZones[seqIndx] = true;
        if (wellBoreZones[i].zoneIsPacker)
          packerZones[seqIndx] = true;
      }
    }

    for (int i = 0; i < maxWellboreZones; i++)
    {
      SequenceArray& currSequences = layerSequenceData[i];
      if (!validZones[i])
      {
        currSequences.DeAlloc();
      }
      else
      {
        currSequences.SetToInitial();  // set up if not already
        currSequences.SetForFlags();
        if (packerZones[i])
          for (int j = 0; j < currSequences.Size(); j++)
            if (currSequences.IsValid(j))
              currSequences.GetRef(j).SetInactive();
      }
    }

    if (useCommonHistory)
    {
      layerHistorySequenceData.SetToInitial();  // set up if not already
      layerHistorySequenceData.SetForFlags();
    }
  }
  else
  {
    singleSequenceData.SetForFlags();
  }

  for (int i = 0; i < testZoneCurves.Size(); i++)
    if (testZoneCurves.IsValid(i))
    {
      testZoneCurves[i]->SetForFlags();
      if (testZoneCurves[i]->tzType == TestZoneCurve::tzFlagReset)
      {
        delete testZoneCurves[i];
        testZoneCurves[i] = 0;
      }
    }

  testZoneCurves.Cleanup();
}

bool SequenceStaticSupport::SequencesOK()
{
  return sequenceErrorListing.ErrorCheckOK();
}

void SequenceStaticSupport::SequenceCleanup()
{
  singleSequenceData.Cleanup();
  layerHistorySequenceData.Cleanup();
  for (int i = 0; i < layerSequenceData.AllocSize(); i++)
    layerSequenceData[i].Cleanup();
  testZoneCurves.Cleanup();
}

void SequenceStaticSupport::SetSequenceTimes(bool doRecalc)
{
  double currStart =  testStartTime.GetMetricVal();
  if (control.IsLayered())
  {
    if (useCommonHistory)
    {
      layerHistorySequenceData.SetSequenceTimes(currStart, layerHistoryEndTime.GetMetricVal());
      currStart = layerHistoryEndTime.GetMetricVal();
    }
    for (int i = 0; i < layerSequenceData.AllocSize(); i++)
      layerSequenceData[i].SetSequenceTimes(currStart, testEndTime.GetMetricVal());
  }
  else
    singleSequenceData.SetSequenceTimes(currStart, testEndTime.GetMetricVal());

#ifndef ADCONSOLEAPP
  if (doRecalc)
    nPreUtilities::ExecuteAndReplot(sequenceTimesFO);
#endif

}

int SequenceStaticSupport::GetNLayerSequence()
{
  if (!control.IsLayered())
    return 0;

  int nseq = 0;
  if (useCommonHistory)
    nseq += layerHistorySequenceData.Size();

  for (int i = 0; i < wellBoreZones.Size(); i++)
  {
    if (!wellBoreZones[i].zoneIsPacker)
      nseq += layerSequenceData[wellBoreZones[i].seqIndex].Size();
  }

  return nseq;
}

void SequenceStaticSupport::CheckAndAllocTestZoneCurve(int tzcIndex)
{
  testZoneCurves.CreateIfNecessary(tzcIndex);
}


int SequenceStaticSupport::GetTotalNTimeStep()
{
  int nts = 0;
  if (control.IsLayered())
  {
    // worst case
    if (useCommonHistory)
      nts += layerHistorySequenceData.GetNTimeStep();

    int refIndex;
    wellBoreZones.GetIndex(referenceZoneID, refIndex);
    if ((refIndex >= 0) && (!wellBoreZones[refIndex].zoneIsPacker))
      nts += layerSequenceData[wellBoreZones[refIndex].seqIndex].GetNTimeStep();
  }
  else
  {
    nts += singleSequenceData.GetNTimeStep();
  }
  return nts;
}

bool  SequenceStaticSupport::UsesDynamicTS()
{
  if (control.IsLayered())
  {
    // worst case
    if (useCommonHistory && layerHistorySequenceData.UsesDynamicTS())
      return true;
    for (int i = 0; i < wellBoreZones.Size(); i++)
    {
      if ((!wellBoreZones[i].zoneIsPacker) && layerSequenceData[wellBoreZones[i].seqIndex].UsesDynamicTS())
        return true;
    }
  }
  else
  {
    return singleSequenceData.UsesDynamicTS();
  }

  return false;
}


bool  SequenceStaticSupport::ContainsSlugBC()
{
  if (control.IsLayered())
  {
    if (useCommonHistory && layerHistorySequenceData.HasSlugBC())
      return true;
    for (int i = 0; i < wellBoreZones.Size(); i++)
    {
      if ((!wellBoreZones[i].zoneIsPacker) && layerSequenceData[wellBoreZones[i].seqIndex].HasSlugBC())
        return true;
    }
  }
  else
  {
    return singleSequenceData.HasSlugBC();
  }

  return false;
}

bool  SequenceStaticSupport::ContainsPulseBC()
{
  if (control.IsLayered())
  {
    if (useCommonHistory && layerHistorySequenceData.HasPulseBC())
      return true;
    for (int i = 0; i < wellBoreZones.Size(); i++)
    {
      if ((!wellBoreZones[i].zoneIsPacker) && layerSequenceData[wellBoreZones[i].seqIndex].HasPulseBC())
        return true;
    }
  }
  else
  {
    return singleSequenceData.HasPulseBC();
  }
  return false;
}


appFuncObjGlobals::AppFuncArray SequenceStaticSupport::sequenceObj("Sequence");

void SequenceStaticSupport::InitAppGlobals()
{
  sequenceObj.AddTo(&sequenceTimesFO);
  appFuncObjGlobals::appFuncData += &sequenceObj;
}


SequenceListing::SequenceListing()
  : OSListObj("nPreSequences")
{
}


void SequenceListing::ListFlowHist(const FlowHistBase& flowHist)
{
  FirstEnumStr("Fixed",     FlowHistBase::vtFixed);
  AddEnumStr("Suite/Range", FlowHistBase::vtVary);
  AddEnumStr("Curve",     FlowHistBase::vtCurve);
  AddEnumStr("Sampled",   FlowHistBase::vtSampled);
  AddEnumStr("Curve+Sampled",  FlowHistBase::vtCurveSampled);
  AddStdEnum("  Type",      flowHist.valueType);

  switch (flowHist.valueType) {
  case FlowHistBase::vtFixed : {
    AddUnitReal("  Fixed value", flowHist.fixedValue);
    break;
  }
  case FlowHistBase::vtVary : {
    AddVaryVar("  Fixed suite/range", flowHist.varyValue);
    break;
  }
  case FlowHistBase::vtCurve : {
    break;
  }
  case FlowHistBase::vtSampled : {
    AddSampVar("  Fixed sample", flowHist.sampleValue);
    break;
  }
  case FlowHistBase::vtCurveSampled : {
    AddSampVar("  Offset sample", flowHist.sampleValue);
    break;
  }
  }

  FirstEnumStr("None",        FlowHistBase::wsNone);
  AddEnumStr("Isolated",      FlowHistBase::wsIsolated);
  AddEnumStr("Open",        FlowHistBase::wsOpen);
  AddStdEnum("  Wellbore storage",  flowHist.wellboreStorage);

}


void SequenceListing::ListPulseSlug(const PulseSlugBase& pulseSlug)
{
  FirstEnumStr("Absolute",        PulseSlugBase::ipAbsolute);
  AddEnumStr("TS Relative",     PulseSlugBase::ipTSRelative);
  AddEnumStr("Seq. Relative",     PulseSlugBase::ipSeqRelative);
  AddStdEnum("  Initial pressure type", pulseSlug.initialPressureType);

  AddUnitReal("  Initial pressure", pulseSlug.initialPressure);

}



void SequenceListing::ListSequence(const char* zoneID,
                                   const Sequence& outSeq,
                                   bool  listTS)
{
  char titleStr[80];

  if (control.IsLayered())
  {
    MakeString(titleStr, "Sequence: ", zoneID, "|", outSeq.seqID, 80);
  }
  else
    MakeString(titleStr, "Sequence: ", outSeq.seqID, 80);
  SubTitle(titleStr);

  FirstEnumStr("Flow", Sequence::stFlow);
  AddEnumStr("History", Sequence::stHistory);
  AddEnumStr("Pulse", Sequence::stPulse);
  AddEnumStr("Slug", Sequence::stSlug);
  AddEnumStr("Packer", Sequence::stPacker);
  AddStdEnum("  Sequence type", outSeq.seqType);

  if (!enterSequenceDurations)
  {
    UnitReal temp(testStartTime);
    temp.SetMetricVal(outSeq.startTime);
    AddUnitReal("  Start time", temp);
    temp.SetMetricVal(outSeq.seqLength.GetMetricVal());
    AddUnitReal("  Duration", temp);
  }
  else
  {
    AddUnitReal("  Duration", outSeq.seqLength);
  }

  if (listTS)
  {
    FirstEnumStr("Static", TimeStep::tstStatic);
    AddEnumStr("Log", TimeStep::tstLog);
    AddEnumStr("Dynamic P", TimeStep::tstDynamicP);
    AddEnumStr("Dynamic Q", TimeStep::tstDynamicQ);
    AddStdEnum("  Time step type", outSeq.timeStep.timeStepType);

    switch (outSeq.timeStep.timeStepType) {
    case TimeStep::tstStatic : {
      AddUnitReal("  Static time step", outSeq.timeStep.staticSize);
      break;
    }
    case TimeStep::tstLog : {
      SC_SciFormat tStepForm(5);
      AddUnitReal("  First log step", outSeq.timeStep.firstLogSize, &tStepForm);
      AddStdInt("  # of time steps", outSeq.timeStep.nlogSteps);
      break;
    }
    case TimeStep::tstDynamicP :
    case TimeStep::tstDynamicQ : {
      AddUnitReal("  Dynamic max", outSeq.timeStep.dynamicMax);
      AddUnitReal("  Dynamic min", outSeq.timeStep.dynamicMin);
      break;
    }
    }
  }

  switch (outSeq.seqType) {
  case Sequence::stFlow : {
    ListFlowHist(outSeq.flowData);
    break;
  }
  case Sequence::stHistory : {
    ListFlowHist(outSeq.historyData);
    break;
  }
  case Sequence::stPulse : {
    ListPulseSlug(outSeq.pulseData);
    break;
  }
  case Sequence::stSlug : {
    ListPulseSlug(outSeq.slugData);
    break;
  }
  case Sequence::stPacker : {
    break;
  }
  }
}

void SequenceListing::ListSequenceArray(const char* zoneID,
                                        const SequenceArray& outSeqArray,
                                        bool  listTS)
{
  if (control.IsLayered())
  {
    char title[80];
    MakeString(title, "Well Bore Zone: ", zoneID, " Sequences", 80);
    StdTitle(title);

    if (listTS)
      AddLine("Zone is reference for time steps");
  }
  else
    StdTitle("Sequences");

  for (int i = 0; i < outSeqArray.Size(); i++)
    if (outSeqArray.IsValid(i))
      ListSequence(zoneID, outSeqArray.GetRef(i), listTS);
}


void SequenceListing::ListTestZoneCurve(const TestZoneCurve& outTZC)
{

  AddConditionalBlank();

  if (control.UseCurveFiles())
  {
    AddStdText("Curve file component to use", outTZC.curveID);
  }
  else
  {
    const char* crvStr = "Curve object to use";
    if (outTZC.curveObjRef.IsNull())
      AddStdText(crvStr, "not set");
    else
      AddStdText(crvStr, outTZC.curveObjRef.objRef->GetID());
  }

  FirstEnumStr("Pressure",      TestZoneCurve::tzPressure);
  AddEnumStr("Flow Rate",     TestZoneCurve::tzFlowRate);
  AddEnumStr("Temperature",   TestZoneCurve::tzTemperature);
  AddEnumStr("Compressibility", TestZoneCurve::tzCompressibility);
  AddEnumStr("Delta Volume",    TestZoneCurve::tzDeltaVolume);
  AddEnumStr("Absolute Volume", TestZoneCurve::tzAbsVolume);
  AddStdEnum("Curve type",    outTZC.tzType);

  if (control.IsLayered())
    AddStdText("Wellbore zone", outTZC.wellboreZoneID);

  AddStdText("Start sequence",  outTZC.startSeqID);
  AddStdText("End sequence",    outTZC.endSeqID);

  AddBoolText("Curve time base",      outTZC.timeIsSequenceBased, "Sequence", "Test");
  AddUnitIndex("Curve Y data units",    outTZC.curveYUnits);
  AddYesNo("Curve Y data is log 10",  outTZC.curveYisLog10);

  if (outTZC.timeIsSequenceBased)
  {
    AddUnitIndex("Curve time units",  outTZC.curveTimeUnits);
    AddYesNo("Curve time is log 10",  outTZC.curveTimeisLog10);
  }
}



void SequenceListing::CreateListing()
{
  if (control.IsLayered())
  {
    if (useCommonHistory)
      ListSequenceArray("All/Pre", layerHistorySequenceData, true);

    int refIndex;
    wellBoreZones.GetIndex(referenceZoneID, refIndex);

    for (int i = 0; i < wellBoreZones.Size(); i++)
      if (!wellBoreZones[i].zoneIsPacker)
        ListSequenceArray(wellBoreZones[i].intervalID, layerSequenceData[wellBoreZones[i].seqIndex], i == refIndex);
  }
  else
  {
    ListSequenceArray(NULL, singleSequenceData, true);
  }

  if (!testZoneCurves.IsEmpty())
  {
    StdTitle("Test Zone Curves");
    for (int i = 0; i < testZoneCurves.Size(); i++)
      if (testZoneCurves.IsValid(i))
        ListTestZoneCurve(testZoneCurves.GetRef(i));
  }
}


SequenceErrorListing::SequenceErrorListing()
  :ErrorListObjC("Sequence")
{
}




void SequenceErrorListing::CreateListing()
{
  StdTitle("Sequence Setup Errors");

  SC_SetupErr seqErr;

  if (control.IsLayered())
  {
    int refIndex;
    wellBoreZones.GetIndex(referenceZoneID, refIndex);
    if (refIndex < 0)
      seqErr.SetInternalError(referenceZoneID, " not a current welbore zone");
    else if (wellBoreZones[refIndex].zoneIsPacker)
      seqErr.SetInternalError(referenceZoneID, " cannot be a packer");

    if (useCommonHistory)
      CheckSeqArray("PRE", layerHistorySequenceData, true);
    for (int i = 0; i < wellBoreZones.Size(); i++)
    {
      const WellBoreZone& currZone = wellBoreZones[i];
      if (!currZone.zoneIsPacker)
      {
        CheckSeqArray(currZone.intervalID, layerSequenceData[currZone.seqIndex], i == refIndex);
      }
    }
  }
  else
  {
    CheckSeqArray("Base", singleSequenceData, true);

    if (isPartialRun)
    {
      if (!singleSequenceData.GetSequenceIndex(partialStartSeqID, partialStartSeqVal))
        AddError("Partial run start sequence not set or bad");
      if (!singleSequenceData.GetSequenceIndex(partialEndSeqID, partialEndSeqVal))
        AddError("Partial run end sequence not set or bad");
      if (partialEndSeqVal < partialStartSeqVal)
        AddError("Partial run end sequence before start sequence");
    }
  }

  for (int i = 0; i < testZoneCurves.Size(); i++)
    if (testZoneCurves.IsNotValid(i))
    {
      AddError("Null curve found");
    }
    else
    {
      if (!testZoneCurves.GetRef(i).SetupOK(seqErr))
      {
        AddError(seqErr);
      }
    }

  if (seqErr.ErrorFound())
    return;

  if (control.IsLayered())
  {
    if (useCommonHistory)
      CheckSeqArrayCoverage(-1, layerHistorySequenceData);
    for (int i = 0; i < wellBoreZones.Size(); i++)
    {
      const WellBoreZone& currZone = wellBoreZones[i];
      if (!currZone.zoneIsPacker)
      {
        CheckSeqArrayCoverage(i, layerSequenceData[currZone.seqIndex]);
      }
    }
  }
  else
    CheckSeqArrayCoverage(0, singleSequenceData);
}

void SequenceErrorListing::CheckSeqArrayCoverage(   int       zoneIndex,
    const SequenceArray&  chkArray)
{
  // check for required curves
  SC_BoolArray pressureReq(chkArray.Size());
  SC_BoolArray flowReq(chkArray.Size());
  SC_BoolArray tempReq(chkArray.Size());
  for (int i = 0; i < chkArray.Size(); i++)
    if (chkArray.IsValid(i))
    {
      const Sequence& currSeq = chkArray.GetRef(i);
      flowReq[i] = (currSeq.seqType == Sequence::stFlow) &&
                   ((currSeq.flowData.valueType == FlowHistBase::vtCurve) ||
                    (currSeq.flowData.valueType == FlowHistBase::vtCurveSampled));
      pressureReq[i] = (currSeq.seqType == Sequence::stHistory) &&
                       ((currSeq.historyData.valueType == FlowHistBase::vtCurve) ||
                        (currSeq.historyData.valueType == FlowHistBase::vtCurveSampled));
      tempReq[i] = testZoneTemperatureVaries && currSeq.HasPulseBC();
    }

  // required
  CheckRequiredCoverage("Pressure", pressureReq, TestZoneCurve::tzPressure, zoneIndex, chkArray);
  CheckRequiredCoverage("Flow", flowReq, TestZoneCurve::tzFlowRate, zoneIndex, chkArray);
  CheckRequiredCoverage("Temperature", tempReq, TestZoneCurve::tzTemperature, zoneIndex, chkArray);

  // double coverage for remainder
  SC_IntArray coverageFound(chkArray.Size(), 0);
  AddCoverage(coverageFound, TestZoneCurve::tzCompressibility, zoneIndex);
  CheckDoubleCoverage("Compressibility", coverageFound, chkArray);

  // reset to 0
  coverageFound.FillToSize(0);

  // only one each vol type allowed
  AddCoverage(coverageFound, TestZoneCurve::tzDeltaVolume, zoneIndex);
  AddCoverage(coverageFound, TestZoneCurve::tzAbsVolume, zoneIndex);
  CheckDoubleCoverage("Volume", coverageFound, chkArray);
}

void SequenceErrorListing::CheckSeqArray(const char* arrayID,
    SequenceArray&  seqData,
    bool        checkTS)
{
  SC_SetupErr seqErr;
  for (int i = 0; i < seqData.Size(); i++)
    if (seqData.IsNotValid(i))
    {
      AddError("Null sequence found");
    }
    else
    {
      if (!seqData.GetRef(i).SetupOK(seqErr, checkTS))
      {
        AddError(seqErr);
      }
    }
}


void SequenceErrorListing::AddCoverage(SC_IntArray&         coverageFound,
                                       TestZoneCurve::TZCurveType crvType,
                                       int              zoneIndex)
{
  for (int i = 0; i < testZoneCurves.Size(); i++)
  {
    TestZoneCurve& currRef = testZoneCurves.GetRef(i);
    bool crvOK = (currRef.tzType == crvType);
    if (control.IsLayered())
      crvOK = crvOK && (currRef.wellboreZone == zoneIndex);
    if (crvOK)
      for (int j = currRef.startSeq; j <= currRef.endSeq; j++)
        coverageFound[j]++;
  }
}


void SequenceErrorListing::CheckRequiredCoverage(const  char*           reqID,
    SC_BoolArray&       crvReq,
    TestZoneCurve::TZCurveType  crvType,
    int             zoneIndex,
    const SequenceArray&    chkArray)

{
  SC_IntArray coverageFound(chkArray.Size(), 0);
  AddCoverage(coverageFound, crvType, zoneIndex);

  char tempStr[80];
  for (int i = 0; i < chkArray.Size(); i++)
  {
    if (crvReq[i] && (coverageFound[i] == 0))
    {
      MakeString(tempStr, "no ", reqID, " coverage for sequence", chkArray.GetRef(i).seqID, 80);
      AddError(tempStr);
    }
  }

  CheckDoubleCoverage(reqID, coverageFound, chkArray);

}

void SequenceErrorListing::CheckDoubleCoverage (const char*       reqID,
    const SC_IntArray&    coverageFound,
    const SequenceArray&  chkArray)
{
  char tempStr[80];
  for (int i = 0; i < chkArray.Size(); i++)
  {
    if (coverageFound[i] > 1)
    {
      MakeString(tempStr, "extra ", reqID, "coverage for sequence ", chkArray.GetRef(i).seqID, 80);
      AddError(tempStr);
    }
  }
}



const char* SequenceStaticSupport::groupHeader = "Sequences";
//const int  SequenceStaticSupport::majorVersion = 0;
//const int  SequenceStaticSupport::majorVersion = 1;  // added varied par
//const int  SequenceStaticSupport::majorVersion = 2;  // global func
//const int  SequenceStaticSupport::majorVersion = 3;  // use auto
//const int  SequenceStaticSupport::majorVersion = 4;  // curve objects
//const int  SequenceStaticSupport::majorVersion = 5;   // removed non-iso pulse
const int  SequenceStaticSupport::majorVersion = 6;   // multiple layers
const int  SequenceStaticSupport::minorVersion = 0;

void SequenceStaticSupport::WriteSequence(Sequence& outSeq)
{
  WriteText(outSeq.seqID);
  WriteBool(outSeq.makeDefaultID);
  WriteInt(outSeq.seqType);
  WriteUnitReal(outSeq.seqLength);
  WriteLine();

  WriteInt(outSeq.timeStep.timeStepType);
  WriteUnitReal(outSeq.timeStep.staticSize);
  WriteUnitReal(outSeq.timeStep.firstLogSize);
  WriteInt(outSeq.timeStep.nlogSteps);
  WriteUnitReal(outSeq.timeStep.dynamicMax);
  WriteUnitReal(outSeq.timeStep.dynamicMin);
  WriteLine();

  WriteInt(outSeq.historyData.valueType);
  WriteUnitReal(outSeq.historyData.fixedValue);
  WriteSampVar(outSeq.historyData.sampleValue);
  WriteVaryVar(outSeq.historyData.varyValue);
  WriteInt(outSeq.historyData.wellboreStorage);
  WriteLine();

  WriteInt(outSeq.flowData.valueType);
  WriteUnitReal(outSeq.flowData.fixedValue);
  WriteSampVar(outSeq.flowData.sampleValue);
  WriteVaryVar(outSeq.flowData.varyValue);
  WriteInt(outSeq.flowData.wellboreStorage);
  WriteLine();

  WriteInt(outSeq.pulseData.initialPressureType);
  WriteUnitReal(outSeq.pulseData.initialPressure);
//  WriteBool(outSeq.pulseData.isIsothermal); removed v5
  WriteLine();

  WriteInt(outSeq.slugData.initialPressureType);
  WriteUnitReal(outSeq.slugData.initialPressure);
  WriteLine();

  WriteBool(outSeq.useInAutoSetup);
  WriteLine();

}


void SequenceStaticSupport::ReadSequence(Sequence& inSeq)
{
  char temp[Sequence::seqIDLen];
  ReadText(temp, Sequence::seqIDLen);
  inSeq.SetID(temp);
  inSeq.makeDefaultID = ReadBool();
  inSeq.seqType = Sequence::SequenceType(ReadInt());
  ReadUnitReal(inSeq.seqLength);

  inSeq.timeStep.timeStepType = TimeStep::TimeStepType(ReadInt());
  ReadUnitReal(inSeq.timeStep.staticSize);
  ReadUnitReal(inSeq.timeStep.firstLogSize);
  inSeq.timeStep.nlogSteps = ReadInt();
  ReadUnitReal(inSeq.timeStep.dynamicMax);
  ReadUnitReal(inSeq.timeStep.dynamicMin);

  int valType = ReadInt();
  if ((currentObjReadMajorVersion == 0) && (valType > 0))
    valType++;
  inSeq.historyData.valueType = FlowHistBase::ValueType(valType);

  ReadUnitReal(inSeq.historyData.fixedValue);
  ReadSampVar(inSeq.historyData.sampleValue);
  if (currentObjReadMajorVersion > 0)
    ReadVaryVar(inSeq.historyData.varyValue);

  inSeq.historyData.wellboreStorage = FlowHistBase::WellboreStorage(ReadInt());

  valType = ReadInt();
  if ((currentObjReadMajorVersion == 0) && (valType > 0))
    valType++;

  inSeq.flowData.valueType = FlowHistBase::ValueType(valType);
  ReadUnitReal(inSeq.flowData.fixedValue);
  ReadSampVar(inSeq.flowData.sampleValue);
  if (currentObjReadMajorVersion > 0)
    ReadVaryVar(inSeq.flowData.varyValue);

  inSeq.flowData.wellboreStorage = FlowHistBase::WellboreStorage(ReadInt());

  inSeq.pulseData.initialPressureType = PulseSlugBase::InitialPressureType(ReadInt());
  ReadUnitReal(inSeq.pulseData.initialPressure);
  if (currentObjReadMajorVersion < 5)
    ReadBool(); // removed v5
//    inSeq.pulseData.isIsothermal = ReadBool();

  inSeq.slugData.initialPressureType = PulseSlugBase::InitialPressureType(ReadInt());
  ReadUnitReal(inSeq.slugData.initialPressure);

  if (currentObjReadMajorVersion > 2)
    inSeq.useInAutoSetup = ReadBool();

  inSeq.SetActive();
}

void SequenceStaticSupport::WriteTestZoneCurve(const TestZoneCurve& outBC)
{
  WriteText(outBC.curveID);
  WriteFuncObjRef(outBC.curveObjRef); // added v4
  WriteText(outBC.wellboreZoneID);  // added v6
  WriteText(outBC.startSeqID);
  WriteText(outBC.endSeqID);

  WriteInt(outBC.tzType);
  WriteBool(outBC.timeIsSequenceBased);
  WriteUnitIndex(outBC.curveYUnits);
  WriteBool(outBC.curveYisLog10);
  WriteUnitIndex(outBC.curveTimeUnits);
  WriteBool(outBC.curveTimeisLog10);
  WriteLine();

}
void SequenceStaticSupport::ReadTestZoneCurve(TestZoneCurve& inBC)
{
  ReadText(inBC.curveID, DC_CurveWithID::curveIDlen);
  if (currentObjReadMajorVersion > 3)
    ReadFuncObjRef(inBC.curveObjRef); // added v4

  if (currentObjReadMajorVersion > 5)
    ReadText(inBC.wellboreZoneID, IntervalBase::intervalIDLen);

  ReadText(inBC.startSeqID, Sequence::seqIDLen);
  ReadText(inBC.endSeqID, Sequence::seqIDLen);

  // required to reset units appropriately
  inBC.ChangeCurveType(TestZoneCurve::TZCurveType(ReadInt()));

  inBC.timeIsSequenceBased = ReadBool();
  ReadUnitIndex(inBC.curveYUnits);
  inBC.curveYisLog10 = ReadBool();
  ReadUnitIndex(inBC.curveTimeUnits);
  inBC.curveTimeisLog10 = ReadBool();

}

void SequenceStaticSupport::WriteSequenceArray(SequenceArray& outSeq)
{
  WriteInt(outSeq.ValidSize());
  WriteLine();
  for (int i = 0; i < outSeq.Size(); i++)
    if (outSeq.IsValid(i))
      WriteSequence(outSeq.GetRef(i));
}

void SequenceStaticSupport::ReadSequenceArray(SequenceArray& inSeq)
{
  inSeq.AllocAndCreate(ReadInt());
  for (int i = 0; i < inSeq.Size(); i++)
    ReadSequence(inSeq.GetRef(i));
}

void SequenceStaticSupport::WriteToFile()
{
  WriteGroupHeader(groupHeader, majorVersion, minorVersion);

  // global objects
  WriteGlobalFunc(sequenceTimesFO);

  SetRealConvFormat(SC_SciFormat(12));

  WriteSequenceArray(singleSequenceData);

  // added v6
  WriteText(referenceZoneID);

  WriteSequenceArray(layerHistorySequenceData);

  WriteInt(layerSequenceData.AllocSize());
  for (int i = 0; i < layerSequenceData.AllocSize(); i++)
    WriteSequenceArray(layerSequenceData[i]);

  WriteInt(testZoneCurves.ValidSize());
  WriteLine();
  for (int i = 0; i < testZoneCurves.Size(); i++)
    if (testZoneCurves.IsValid(i))
      WriteTestZoneCurve(testZoneCurves.GetRef(i));

  // Time-Base
  WriteBool(enterSequenceDurations);
  WriteUnitReal(testStartTime);
  WriteUnitReal(layerHistoryEndTime);  // added v6
  WriteUnitReal(testEndTime);
  WriteLine();

  // Dynamic Time Step
  WriteUnitReal(maximumPressureChange);
  WriteUnitReal(minimumPressureChange);
  WriteUnitReal(maximumFlowRateChange);
  WriteUnitReal(minimumFlowRateChange);
  WriteInt(maxNumberOfTS);
  WriteLine();

  // Partial Run
  WriteBool(isPartialRun);
  WriteLine();
  WriteText(partialStartSeqID);
  WriteText(partialEndSeqID);
  WriteLine();

}

bool SequenceStaticSupport::ReadFromFile()
{
  ReadGroupHeader(groupHeader, majorVersion);
  if (currentObjReadMajorVersion > 1)
  {
    ReadGlobalFunc(sequenceTimesFO);
  }

  ReadSequenceArray(singleSequenceData);

  if (currentObjReadMajorVersion > 5)
  {
    // added v6
    ReadText(referenceZoneID, IntervalBase::intervalIDLen);

    ReadSequenceArray(layerHistorySequenceData);

    layerSequenceData.Alloc(ReadInt());
    for (int i = 0; i < layerSequenceData.AllocSize(); i++)
      ReadSequenceArray(layerSequenceData[i]);
  }

  testZoneCurves.AllocAndCreate(ReadInt());
  for (int i = 0; i < testZoneCurves.Size(); i++)
    ReadTestZoneCurve(testZoneCurves.GetRef(i));

  // Time-Base
  enterSequenceDurations = ReadBool();
  ReadUnitReal(testStartTime);
  if (currentObjReadMajorVersion > 5)
    ReadUnitReal(layerHistoryEndTime);
  ReadUnitReal(testEndTime);

  // Dynamic Time Step
  ReadUnitReal(maximumPressureChange);
  ReadUnitReal(minimumPressureChange);
  ReadUnitReal(maximumFlowRateChange);
  ReadUnitReal(minimumFlowRateChange);
  maxNumberOfTS = ReadInt();

  // Partial Run
  isPartialRun = ReadBool();
  ReadText(partialStartSeqID, Sequence::seqIDLen);
  ReadText(partialEndSeqID, Sequence::seqIDLen);

  SetSequenceTimes(false);

  return true;

}

SequenceCurveFO::SequenceCurveFO() :
  FuncObjC("SequenceCurveUpdate")
{
}

void SequenceCurveFO::IsBeingDeleted(FuncObjC* delObj)
{
  for (int i = 0; i < testZoneCurves.Size(); i++)
    if (testZoneCurves.IsValid(i))
    {
      FuncObjRef& curveRef = testZoneCurves.GetRef(i).curveObjRef;
      if (curveRef.objRef == delObj)
        curveRef = FuncObjRef();
    }
}

bool SequenceCurveFO::UsesAsInput(const FuncObjC* inputObj) const
{
  for (int i = 0; i < testZoneCurves.Size(); i++)
    if (testZoneCurves.IsValid(i))
    {
      if (testZoneCurves.GetRef(i).curveObjRef.objRef == inputObj)
        return true;
    }
  return false;
}


void SequenceCurveFO::UpdateFuncObjRefs()
{
  for (int i = 0; i < testZoneCurves.Size(); i++)
    if (testZoneCurves.IsValid(i))
      PerformUpdate(testZoneCurves.GetRef(i).curveObjRef);
}

SequenceTimesFO::SequenceTimesFO() :
  FuncObjC("SequenceTimes")
{
  sequenceTimesDO.seqTimes = &sequenceTimesDC;
  AddOutPort(sequenceTimesDO);
  DoStatusChk();
}


bool SequenceTimesFO::AddSeqArray(const char* arrayID,
                                  const SequenceArray& seqData,
                                  const UnitReal& stTime)
{
  char seqStr[80];
  UnitReal currStart(uTime, stTime.GetMetricVal());
  for (int i = 0; i < seqData.Size(); i++)
    if (seqData.IsValid(i))
    {
      Sequence& currSeq = seqData.GetRef(i);
      if (currSeq.seqLength.IsNull())
      {
        SetObjErrMsg("sequence found with bad time");
        sequenceTimesDC.Clear();
        return false;
      }

      if (control.IsLayered())
        MakeString(seqStr, arrayID, "|", currSeq.seqID, 80);
      else
        CopyString(seqStr, currSeq.seqID, 80);

      sequenceTimesDC.AddSequence(seqStr, currStart.GetUserVal());
      currStart.metricVal += currSeq.seqLength.GetMetricVal();
    }

  lastEndTime = currStart.GetUserVal();
  return true;
}


void  SequenceTimesFO::DoStatusChk()
{
  FuncObjC::DoStatusChk();

  sequenceTimesDC.Clear();

  lastEndTime = testStartTime.GetUserVal();

  if (control.IsLayered())
  {
    UnitReal* currStart = &testStartTime;
    if (useCommonHistory)
    {
      if (!AddSeqArray("PRE", layerHistorySequenceData, testStartTime))
        return;
      currStart = &layerHistoryEndTime;
    }

    for (int i = 0; i < wellBoreZones.Size(); i++)
    {
      const WellBoreZone& currZone = wellBoreZones[i];
      if (!currZone.zoneIsPacker)
      {
        if (!AddSeqArray(currZone.intervalID, layerSequenceData[currZone.seqIndex], *currStart))
          return;
      }
    }
  }
  else
  {
    if (!AddSeqArray(NULL, singleSequenceData, testStartTime))
      return;
  }

  sequenceTimesDC.AddEndTime(lastEndTime);

  if (!sequenceTimesDO.DataOK())
    SetObjErrMsg("Sequence times bad ???");
}

void SequenceTimesFO::CalcOutput(FOcalcType  calcType)
{
  DoStatusChk();
  if (!StatusOK())
  {
    sequenceTimesDC.Clear();
    return;
  }
}


};                                      // end NS

