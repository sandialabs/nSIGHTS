///////////////////////////////////////////////////////////////////////////////////
//
// C_WorkSequence.cpp
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
#include <math.h>

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osClass/C_SimErr.h>

#include <nsClass/nSightConst.h>

#include <Var/E_Parameter.h>

#include <Var/G_Control.h>
#include <Var/G_Sequence.h>

#include "C_SingleWell.h"
#include "C_WorkParameter.h"
#include "C_WorkSequence.h"



const double WorkingSequence::timeEpsFactor = 1.0E+08;

WorkingBC::WorkingBC() :
  useCurve(false),
  fixedVal(0.0),
  curveToUse(0),
  curveTimeUnits(uTime),
  doTimeConversion(false)
{
}

void WorkingBC::Clear()
{
  useCurve = false;
  fixedVal = 0.0;
  curveToUse = 0;
  curveTimeUnits = UnitIndex(uTime);
  doTimeConversion = false;

}

void  WorkingBC::CurveSetup(TestZoneCurve::TZCurveType  curvType,
                            int             currSeq)
{
  // see if there is curve coverage of the specified type for the specified seq
  for (int i = 0; i < testZoneCurves.Size(); i++)
  {
    TestZoneCurve& currCrv = testZoneCurves.GetRef(i);
    if ((currCrv.tzType == curvType) && (currCrv.startSeq <= currSeq) && (currCrv.endSeq >= currSeq))
    {
      useCurve = true;
      curveToUse = currCrv.curveToUse;
      curveYUnits   = currCrv.curveYUnits;
      curveYisLog10 = currCrv.curveYisLog10;

      doTimeConversion = currCrv.timeIsSequenceBased;
      if (doTimeConversion)
      {
        curveTimeUnits = currCrv.curveTimeUnits;
        doLogTime = currCrv.curveTimeisLog10;
        double metOffsetVal = 0.0;
        for (int k = currSeq - 1; k >= currCrv.startSeq; k--)
          metOffsetVal += singleSequenceData.GetRef(k).seqLength.GetMetricVal();
        // convert to user
        offsetVal = curveTimeUnits.MetricToUser(metOffsetVal);
      }
      return;
    }
  }

  // curve expected -- none found
  if (useCurve)
    GenAppInternalError("WorkingBC::CurveSetup");
}


double WorkingBC::GetCurveY(const SeqTimeVals& nextTimeVal)
{
  if (!useCurve)
    return fixedVal;

  // sanity checks
  if ((curveToUse == 0) || (!curveToUse->IsCreated()))
    GenAppInternalError("WorkingBC::GetCurveY");

  double curveTime;
  if (doTimeConversion)
  {
    curveTime = curveTimeUnits.MetricToUser(nextTimeVal.seqTime) + offsetVal;
    if (doLogTime)
    {
      if (curveTime < stdEps)
        return nullReal;
      curveTime = log10(curveTime);
    }
  }
  else
    // convert to default time units
    curveTime = curveTimeUnits.MetricToUser(nextTimeVal.testTime);

  // get curve value
  double userY = curveToUse->GetCurveY(curveTime);

  // if log then transform
  if (curveYisLog10)
  {
    if ((userY < -99.0) || (userY > 99.0))
      return nullReal;
    userY = pow(10.0, userY);
  }

  //  convert units && return
  return curveYUnits.UserToMetric(userY);
}

WorkingSequence::WorkingSequence() :
  seqIndx(0)
{};

WorkingSequence::~WorkingSequence()
{
}


bool WorkingSequence::SetData(int         sIndx,
                              DynamicTimeSteps& cSim,
                              const double&   fixedTZVol,
                              const double&     fixedTZComp,
                              const double&   fixedTZTemp,
                              SC_SetupErr&    errData)
{
  Sequence& currSeq = nsSequence::singleSequenceData.GetRef(sIndx);

  //  set basic internal vars
  seqIndx   = sIndx;
  seqType   = currSeq.seqType;
  seqLength   = currSeq.seqLength.GetMetricVal();
  seqID       = currSeq.seqID;

  startTime   = currSeq.startTime;
  currSim   = 0;
  //  time step
  timeStepType = currSeq.timeStep.timeStepType;
  switch (timeStepType)
  {
  case TimeStep::tstStatic : {
    linDeltaT = currSeq.timeStep.staticSize.GetMetricVal();
    break;
  }
  case TimeStep::tstLog : {
    logTStart = currSeq.timeStep.firstLogSize.GetMetricVal();
    logTMult = exp(log(seqLength / logTStart) / double(currSeq.timeStep.nlogSteps - 1));
    break;
  }
  case TimeStep::tstDynamicP:
  case TimeStep::tstDynamicQ : {
    currSim = &cSim;
    maxTS = currSeq.timeStep.dynamicMax.GetMetricVal();
    minTS = currSeq.timeStep.dynamicMin.GetMetricVal();
    break;
  }
  }

  tzPressBC.Clear();
  tzFlowBC.Clear();

  tzCompFTBC.Clear();
  tzCompFTBC.fixedVal = fixedTZComp;
  // only use curve if allowd by flag
  if (testZoneCompressibilityVaries)
    tzCompFTBC.CurveSetup(TestZoneCurve::tzCompressibility, sIndx);

  tzTempBC.Clear();
  tzTempBC.fixedVal = fixedTZTemp;
  isThermal = false;
  if (testZoneTemperatureVaries)
  {
    tzTempBC.CurveSetup(TestZoneCurve::tzTemperature, sIndx);
    isThermal = tzTempBC.useCurve;
  }

  tzVolBC.Clear();
  tzVolBC.fixedVal = fixedTZVol;
  volOffset = 0.0;
  if (testZoneVolumeVaries)
  {
    tzVolBC.CurveSetup(TestZoneCurve::tzDeltaVolume, sIndx);
    if (tzVolBC.useCurve)
      volOffset = fixedTZVol;
    else
      tzVolBC.CurveSetup(TestZoneCurve::tzAbsVolume, sIndx);
  }

  fixedOffset = 0.0;
  switch (seqType)
  {
  case Sequence::stFlow  : {
    switch (currSeq.flowData.valueType)
    {
    case FlowHistBase::vtFixed  : {
      tzFlowBC.fixedVal = currSeq.flowData.fixedValue.GetMetricVal();
      break;
    }
    case FlowHistBase::vtVary  : {
      tzFlowBC.fixedVal = currSeq.flowData.varyValue.GetVaryVal();
      break;
    }
    case FlowHistBase::vtSampled : {
      tzFlowBC.fixedVal = currSeq.flowData.sampleValue.GetSampleVal();
      break;
    }
    case FlowHistBase::vtCurveSampled  : {
      tzFlowBC.useCurve = true;
      fixedOffset = currSeq.flowData.sampleValue.GetSampleVal();
      break;
    }
    case FlowHistBase::vtCurve  : {
      tzFlowBC.useCurve = true;
      break;
    }
    }
    wellboreStorage = currSeq.flowData.wellboreStorage;
    if (tzFlowBC.useCurve)
      tzFlowBC.CurveSetup(TestZoneCurve::tzFlowRate, sIndx);
    break;
  }
  case Sequence::stHistory  : {
    switch (currSeq.historyData.valueType)
    {
    case FlowHistBase::vtFixed  : {
      tzPressBC.fixedVal = currSeq.historyData.fixedValue.GetMetricVal();
      break;
    }
    case FlowHistBase::vtVary  : {
      tzFlowBC.fixedVal = currSeq.historyData.varyValue.GetVaryVal();
      break;
    }
    case FlowHistBase::vtSampled : {
      tzPressBC.fixedVal = currSeq.historyData.sampleValue.GetSampleVal();
      break;
    }
    case FlowHistBase::vtCurveSampled  : {
      tzPressBC.useCurve = true;
      fixedOffset = currSeq.historyData.sampleValue.GetSampleVal();
      break;
    }
    case FlowHistBase::vtCurve  : {
      tzPressBC.useCurve = true;
      break;
    }
    }
    wellboreStorage = currSeq.historyData.wellboreStorage;
    if (tzPressBC.useCurve)
      tzPressBC.CurveSetup(TestZoneCurve::tzPressure, sIndx);
    break;
  }
  case Sequence::stPulse  : {
    /*      isThermal = testZoneTemperatureVaries && (!currSeq.pulseData.isIsothermal);
          if (isThermal)
          {
            tzTempBC.useCurve   = true;
            tzTempBC.CurveSetup(TestZoneCurve::tzTemperature, sIndx);
          }
    */
    initialPressureType = currSeq.pulseData.initialPressureType;
    initialPressure   = currSeq.pulseData.initialPressure.GetMetricVal();
    wellboreStorage = FlowHistBase::wsIsolated;
    break;
  }
  case Sequence::stSlug  : {
    initialPressureType = currSeq.slugData.initialPressureType;
    initialPressure   = currSeq.slugData.initialPressure.GetMetricVal();
    wellboreStorage = FlowHistBase::wsOpen;
    break;
  }
  case Sequence::stPacker  : {
    wellboreStorage = FlowHistBase::wsNone;
    break;
  }
  }

  return true;
}

void WorkingSequence::StartSequence(    SeqTimeVals&  initTime,
                                        const SeqBoundVals& lastSeqEndBC,  // at end of prev seq - used for TS/TZ rel init pressure
                                        SeqBoundVals& initBC)
{
  // set up time
  initTime.seqTime  = 0.0;
  initTime.testTime = startTime;
  switch (timeStepType)
  {
  case TimeStep::tstStatic : {
    initTime.deltaT = linDeltaT;
    break;
  }
  case TimeStep::tstLog : {
    initTime.deltaT = logTStart;
    break;
  }
  case TimeStep::tstDynamicP :
  case TimeStep::tstDynamicQ : {
    initTime.deltaT = minTS;
    if (initTime.deltaT < 0.01)
      initTime.deltaT = 0.01;
    lastQOK = false;
    break;
  }
  }

  // set up BC
  tsSavePressure = lastSeqEndBC.tsPressure;  // maintained for duration of pulse
  // or for flow/hist with isolated

  initBC.tsPressure   = lastSeqEndBC.tsPressure;

  switch (seqType)
  {
  case Sequence::stFlow  : {
    initBC.tzPressure   = lastSeqEndBC.tzPressure;  // maintained for duration of pulse
    initBC.wellFlow   = tzFlowBC.GetCurveY(initTime) + fixedOffset;
    break;
  }
  case Sequence::stHistory  : {
    initBC.tzPressure = tzPressBC.GetCurveY(initTime) + fixedOffset;
    break;
  }
  case Sequence::stPulse  :
  case Sequence::stSlug  : {
    switch (initialPressureType)
    {
    case PulseSlugBase::ipAbsolute : {
      initBC.tzPressure = initialPressure;
      break;
    }
    case PulseSlugBase::ipTSRelative : {
      initBC.tzPressure = lastSeqEndBC.tsPressure + initialPressure;
      break;
    }
    case PulseSlugBase::ipSeqRelative : {
      initBC.tzPressure = lastSeqEndBC.tzPressure + initialPressure;
      break;
    }
    }

    if (initBC.tzPressure < 0.0)
      throw SimError("initial well pressure/head < zero", SimError::seSemiFatal);

    break;
  }
  case Sequence::stPacker  : {
    initBC.tzPressure   = lastSeqEndBC.tzPressure;  // maintained for duration of pulse
    initBC.wellFlow   = 0.0;
    break;
  }

  }

  if (wellboreStorage == FlowHistBase::wsOpen)
    initBC.tsPressure = initBC.tzPressure;
  lastP = initBC.tzPressure;

  // get remainder
  initBC.tzTemp = tzTempBC.GetCurveY(initTime);
  initBC.tzVol = tzVolBC.GetCurveY(initTime) + volOffset;
  if (tzCompFP->IsFP())
    initBC.tzComp = tzCompFP->GetData(initBC.tzPressure);
  else
    initBC.tzComp = tzCompFTBC.GetCurveY(initTime);

  if (!control.IsGas())
    initBC.tzDens = tzDensFP->GetData(initBC.tzPressure);
}



bool WorkingSequence::SetNextTimeStep(SeqTimeVals&  timeVal)     // returns true if DT changed
{
  double currTime = timeVal.seqTime;
  double nextTime = currTime;

  switch (timeStepType)
  {
  case TimeStep::tstStatic : {
    nextTime += linDeltaT;
    break;
  }
  case TimeStep::tstLog : {
    nextTime *= logTMult;
    break;
  }
  case TimeStep::tstDynamicP :
  case TimeStep::tstDynamicQ : {
    double dynDT = timeVal.deltaT;
    if (dynUnderMin)
    {
      dynDT += dynDT;     // double it
      if (dynDT > maxTS)
        dynDT =  maxTS;
      nochangeCount = 0;
    }
    else
    {
      CheckDynamicPressureLimits();
    }

    nextTime += dynDT;
    break;
  }
  }

  if ((seqLength - nextTime) < (seqLength / timeEpsFactor))
    nextTime = seqLength;

  double nextDeltaT = nextTime - currTime;


  // TODO use time ratio for change flag -- ??? needs to be checked
  double tRatio = timeVal.deltaT / nextDeltaT;

  // update to new
  timeVal.deltaT  = nextDeltaT;
  timeVal.testTime = startTime + currTime;

  return fabs(tRatio - 1.0) > 1.0E-07;
}

void WorkingSequence::SetDynamicPressureLimits()
{
  double pChange = currSim->GetPChange();

  maxPChange = pChange * maximumPressureChange.GetMetricVal();
  if (maxPChange < 10.0)
    maxPChange = 10.0;

  minPChange = pChange * minimumPressureChange.GetMetricVal();
  if (minPChange < 0.10)
    minPChange = 0.10;

  nochangeCount = 0;
}

void WorkingSequence::CheckDynamicPressureLimits()
{
  nochangeCount++;
  if ((nochangeCount > 20) && (timeStepType == TimeStep::tstDynamicP))
    SetDynamicPressureLimits();
}


void WorkingSequence::DynamicSetup()
{
  if (!IsDynamicTS())
    return;

  currSim->SaveCurrentPressure();
  if (timeStepType == TimeStep::tstDynamicP)
    SetDynamicPressureLimits();
}


bool WorkingSequence::DynamicRestart(    SeqTimeVals&  timeVal,  // true if dynamic TS failure
    const SeqBoundVals& currBC)
{
  double currQ = currBC.formFlow;
  bool redoLastTS = false;
  switch (timeStepType)
  {
  case TimeStep::tstDynamicP : {
    double pChange = fabs(currBC.tzPressure - lastP);
    redoLastTS = (pChange > maxPChange);
    dynUnderMin = (pChange < minPChange);
    break;
  }
  case TimeStep::tstDynamicQ : {

    if (!lastQOK)
    {
      lastQOK = true;
      lastQ = currQ;
      return false;
    }

    double denom = lastQ < currQ ? lastQ : currQ;
    if (denom < stdEps)
    {
      lastQ = currQ;
      return false;
    }

    double qRatio = (currQ > lastQ ? currQ : lastQ) / denom - 1.0;

    redoLastTS = (qRatio > maximumFlowRateChange.GetMetricVal());
    dynUnderMin = (qRatio < minimumFlowRateChange.GetMetricVal());
    break;
  }
  }

  redoLastTS = redoLastTS && (timeVal.deltaT > minTS);

  if (!redoLastTS)
  {
    lastQ = currBC.formFlow;
    lastP = currBC.tzPressure;
  }
  else
  {
    timeVal.ResetTimes();
    timeVal.deltaT /= 2.0;
    if (timeVal.deltaT < minTS)
      timeVal.deltaT = minTS;
    currSim->RestoreCurrentPressure();
    CheckDynamicPressureLimits();
  }

  return redoLastTS;
}

void WorkingSequence::GetTZvals(const SeqTimeVals&    timeVal,
                                SeqBoundVals&   nextBC)
{
  double currTime   = timeVal.seqTime;
  nextBC.wellFlow   = 0.0;
  switch (seqType)
  {
  case Sequence::stFlow  : {
    nextBC.wellFlow   = tzFlowBC.GetCurveY(timeVal) + fixedOffset;
    break;
  }
  case Sequence::stHistory  : {
    nextBC.tzPressure = tzPressBC.GetCurveY(timeVal) + fixedOffset;
    if (wellboreStorage != FlowHistBase::wsIsolated)
      nextBC.tsPressure = nextBC.tzPressure;
    break;
  }
  default: {
    break;
  }
  }

  if (wellboreStorage == FlowHistBase::wsIsolated)
    nextBC.tsPressure = tsSavePressure;

  // get remainder
  nextBC.tzTemp = tzTempBC.GetCurveY(timeVal);
  nextBC.tzVol = tzVolBC.GetCurveY(timeVal) + volOffset;
  if (tzCompFP->IsFP())
    nextBC.tzComp = tzCompFP->GetData(nextBC.tzPressure);
  else
    nextBC.tzComp = tzCompFTBC.GetCurveY(timeVal);

  if (!control.IsGas())
    nextBC.tzDens = tzDensFP->GetData(nextBC.tzPressure);
}


bool WorkingSequence::IsComplete(const SeqTimeVals&  timeVal)
{
  return ((seqLength - timeVal.seqTime) < (seqLength / timeEpsFactor));
}

bool WorkingSequenceSetup(DynamicTimeSteps&     cSim,
                          WorkingSequenceArray&   wSeq,
                          WorkingParameterArray&  currPar,
                          SC_SetupErr&          errData)
{
  double wellThick = currPar[pF_t].GetData();
  if (control.Is2DRadial() && control.IsPartialPenetration())
    wellThick = currPar[pTZ_pl].GetData();

  double fixedTZVol = pi * Sqr(currPar[pTZ_r_w].GetData()) * wellThick;
  if (sequence.ContainsPulseBC())
    fixedTZVol += currPar[pTZ_dV].GetData();

  double fixedTZTemp = nsControlStatics::defaultTestZoneTemperature.GetMetricVal();

  double fixedTZComp = 0.0;
  if (sequence.ContainsPulseBC() && currPar[pTZ_C].IsFixed())
    fixedTZComp = currPar[pTZ_C].GetData();

  wSeq.AllocAndSetSize(sequence.GetNSingleSequence());
  for ( int i = 0; i < sequence.GetNSingleSequence(); i++)
  {
    wSeq[i].tzCompFP = &currPar[pTZ_C];
    wSeq[i].tzDensFP = &currPar[pFl_rho];
    if (!wSeq[i].SetData(i, cSim, fixedTZVol, fixedTZComp, fixedTZTemp, errData))
      return false;
  }

  return true;
}




