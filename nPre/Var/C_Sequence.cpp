///////////////////////////////////////////////////////////////////////////////////
//
// C_Sequence.cpp
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
//      Classes to define sequences for the simulator.
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <Var/G_Control.h>

#include <nsClass/nSightConst.h>

#include <Var/C_Sequence.h>

TimeStep::TimeStep() :
    timeStepType(tstLog),
    staticSize(uTime, 10.0),
    firstLogSize(uTime, 0.01),
    nlogSteps(250),
    dynamicMax(uTime, 1.0E+05),
    dynamicMin(uTime, 1.0E-02)
{
}

TimeStep::TimeStep(const TimeStep& a)
{
    LocalCopy(a);
}

TimeStep&   TimeStep::operator=(const TimeStep& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void TimeStep::LocalCopy(const TimeStep& a)
{
    timeStepType    = a.timeStepType;
    staticSize      = a.staticSize;
    firstLogSize    = a.firstLogSize;
    nlogSteps       = a.nlogSteps;
    dynamicMax      = a.dynamicMax;
    dynamicMin      = a.dynamicMin;
}

bool TimeStep::CheckError(SC_SetupErr& tsErr,
                        double  seqLength)
{
    double minTS = Pround(seqLength / double(maxTimeSteps), 5);
    double maxTS = Pround(seqLength / 10.0, 2);
    switch (timeStepType) {
        case tstStatic :  {
            tsErr.OutOfRangeErr("static TS", staticSize.GetMetricVal(), minTS, maxTS);
            break;
        }
        case tstLog : {
            tsErr.OutOfRangeErr("log first TS", firstLogSize.GetMetricVal(), 1.0E-08, maxTS);
            break;
        }
        case tstDynamicP: case tstDynamicQ : {
            tsErr.OutOfRangeErr("dynamic min TS", dynamicMin.GetMetricVal(), 0.0, maxTS);
            tsErr.OutOfRangeErr("dynamic max TS", dynamicMax.GetMetricVal(), minTS, maxTS);
            if (dynamicMax.GetMetricVal() <= dynamicMin.GetMetricVal())
                tsErr.SetConstantError("dynamic min TS > max");
        }
    }
    return tsErr.NoErrors();

}


FlowHistBase::FlowHistBase(UnitType uType, int corrGroup) :
    valueType(vtFixed),
    fixedValue(uType),
    isSampled(false),
    sampleValue(isSampled, corrGroup, seqSampVaryID, fixedValue),
    isVaried(false),
    varyValue(isVaried, seqSampVaryID, fixedValue),
    wellboreStorage(wsNone)
{
    
}

FlowHistBase::FlowHistBase(const FlowHistBase& a) :
    sampleValue(isSampled, a.sampleValue.GetCorrGroup(), seqSampVaryID, fixedValue),
    varyValue(isVaried,  seqSampVaryID, fixedValue)
{
    LocalCopy(a);
}

FlowHistBase&   FlowHistBase::operator=(const FlowHistBase& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void FlowHistBase::LocalCopy(const FlowHistBase& a)
{
    valueType       = a.valueType;
    fixedValue      = a.fixedValue;
    sampleValue     = a.sampleValue;
    varyValue       = a.varyValue;
    wellboreStorage = a.wellboreStorage;
    isSampled       = a.isSampled;
    isVaried        = a.isVaried;
}

void FlowHistBase::SetActive(bool isActive)
{
    isSampled = isActive && ((valueType == vtSampled) || (valueType == vtCurveSampled));
    isVaried = isActive && (valueType == vtVary);
}

void FlowHistBase::ClearWasActive()
{
    sampleValue.wasActive = false;
    varyValue.wasActive = false;
}

void FlowHistBase::SetForFlags()
{
    if (control.IsSampleSim())
    {
        if (valueType == vtVary)
            valueType = vtFixed;
        if ((valueType == vtFixed) && sampleValue.wasActive)
            valueType = vtSampled;
    }
    else
    {
        if (valueType == vtSampled)
            valueType = vtFixed;
        if (valueType == vtCurveSampled)
            valueType = vtCurve;
        if ((valueType == vtFixed) && varyValue.wasActive)
            valueType = vtVary;
    }
}

void FlowHistBase::SetID(const char* seqID, const char* typeID)
{
    CopyString(seqSampVaryID, seqID, 80);
    ConcatString(seqSampVaryID, ":", 80);
    ConcatString(seqSampVaryID, typeID, 80);
}

void FlowHistBase::CheckError(SC_SetupErr& tsErr)
{
    if (valueType == vtFixed)
    {
        tsErr.NullDataErr("fixed value", fixedValue.GetMetricVal());
    }
    else if (isSampled)
    {
        sampleValue.SampleSetupOK(tsErr);
    }
    else if (isVaried)
    {
        varyValue.VarySetupOK(tsErr);
    }
}

FlowSequence::FlowSequence() :
    FlowHistBase(uFlow, 1)
{
}

FlowSequence::FlowSequence(const FlowSequence& a) : FlowHistBase(a)
{
}

FlowSequence& FlowSequence::operator=(const FlowSequence& a)
{
    FlowHistBase::operator=(a);
    return *this;
}


void FlowSequence::ResetSampleID(const char* seqID)
{
    SetID(seqID, "Q");
}

void FlowSequence::CheckError(SC_SetupErr& tsErr,
                              const TimeStep& tsData)
{
    if (((valueType == vtFixed)||(valueType == vtSampled)||(valueType == vtVary)) && (tsData.timeStepType == TimeStep::tstDynamicQ))
        tsErr.SetConstantError("cannot use DynamicQ with fixed/vary/sample type");
    else
        FlowHistBase::CheckError(tsErr);
}


HistorySequence::HistorySequence() :
    FlowHistBase(uPress, 2)
{
}

HistorySequence::HistorySequence(const HistorySequence& a) : FlowHistBase(a)
{
}

HistorySequence& HistorySequence::operator=(const HistorySequence& a)
{
    FlowHistBase::operator=(a);
    return *this;
}

void HistorySequence::ResetSampleID(const char* seqID)
{
    SetID(seqID, "P");
}

void HistorySequence::CheckError(SC_SetupErr& tsErr,
                                const TimeStep& tsData)
{
    if (((valueType == vtFixed)||(valueType == vtSampled)||(valueType == vtVary)) && (tsData.timeStepType == TimeStep::tstDynamicP))
        tsErr.SetConstantError("cannot use DynamicP with fixed/vary/sample type");
    else
        FlowHistBase::CheckError(tsErr);
}

PulseSlugBase::PulseSlugBase() :
    initialPressureType(ipAbsolute),
    initialPressure(uPress)
{
}

PulseSlugBase::PulseSlugBase(const PulseSlugBase& a) :
    initialPressureType(a.initialPressureType),
    initialPressure(a.initialPressure)
{
}

PulseSlugBase& PulseSlugBase::operator=(const PulseSlugBase& a)
{
    if (&a != this)
    {
        initialPressureType = a.initialPressureType;
        initialPressure     = a.initialPressure;
    }
    return *this;
}

void PulseSlugBase::CheckError(SC_SetupErr& tsErr)
{
    if (control.IsGas())
        tsErr.SetConstantError("gas pulse sequences not verified - please report this error");

    tsErr.NullDataErr("initial pressure", initialPressure.GetMetricVal());
}


PulseSequence::PulseSequence()
{
}

PulseSequence::PulseSequence(const PulseSequence& a) 
    : PulseSlugBase(a)
{
}

PulseSequence& PulseSequence::operator=(const PulseSequence& a)
{
    if (&a != this)
    {
        PulseSlugBase::operator=(a);
    }
    return *this;
}


SlugSequence::SlugSequence()
{
}

SlugSequence::SlugSequence(const SlugSequence& a) : PulseSlugBase(a)
{
}

SlugSequence& SlugSequence::operator=(const SlugSequence& a)
{
    PulseSlugBase::operator=(a);
    return *this;
}


Sequence::Sequence() :
    seqLength(uTime),
    seqType(stFlow),
    makeDefaultID(true),
    useInAutoSetup(true),
    startTime(0.), endTime(0.0)
{
    SetID(nullStr);
    
}
Sequence::Sequence(const Sequence& a)
{
    LocalCopy(a);
}
Sequence&  Sequence::operator=(const Sequence& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void Sequence::LocalCopy(const Sequence& a)
{
    SetID(a.seqID);
    makeDefaultID = a.makeDefaultID;

    seqLength   = a.seqLength;         // duration
    timeStep    = a.timeStep;
    seqType     = a.seqType;
    historyData = a.historyData;
    flowData    = a.flowData;
    pulseData   = a.pulseData;
    slugData    = a.slugData;

    useInAutoSetup = a.useInAutoSetup;
}

void Sequence::SetForFlags()
{
    if (control.IsGas())
    {
        if (seqType == stSlug)
            seqType = stFlow;
        if (flowData.wellboreStorage == FlowHistBase::wsOpen)
            flowData.wellboreStorage = FlowHistBase::wsNone;
        if (historyData.wellboreStorage == FlowHistBase::wsOpen)
            historyData.wellboreStorage = FlowHistBase::wsNone;
        if (gasFlowIsMass && flowData.fixedValue.unitType == uFlow)
            flowData.fixedValue = UnitIndex(uMflow);
        if ((!gasFlowIsMass ) && flowData.fixedValue.unitType == uMflow)
            flowData.fixedValue = UnitIndex(uFlow);
    }
    else
    {
        if (flowData.fixedValue.unitType == uMflow)
            flowData.fixedValue = UnitIndex(uFlow);
    }

    historyData.SetForFlags();
    flowData.SetForFlags();
    SetActive();
}


void Sequence::ChangeSeqType(SequenceType   newType)
{
    seqType = newType;
    switch (seqType) {
        case stFlow :  {
            if (timeStep.timeStepType == TimeStep::tstDynamicQ)
                timeStep.timeStepType = TimeStep::tstStatic;
            break;
            }
        case stHistory :  {
            if (timeStep.timeStepType == TimeStep::tstDynamicP)
                timeStep.timeStepType = TimeStep::tstStatic;
            break;
            }
        case stPulse :  {
            break;
            }
        case stSlug :  {
            break;
            }
    }
    SetActive();
}


int Sequence::GetNTimeStep()
{
    SC_SetupErr seqErr;
    if (!timeStep.CheckError(seqErr, seqLength.GetMetricVal()))
        return 500;
    switch (timeStep.timeStepType) {
        case TimeStep::tstStatic :  {
            return int(seqLength.GetMetricVal()/timeStep.staticSize.GetMetricVal()) + 2;
        }
        case TimeStep::tstLog : {
            return timeStep.nlogSteps;
        }
        default : {
            return 1000;  // good guess
        }
    }
    return 0;
}


void Sequence::SetID(const char* sID)
{
    CopyString(seqID, sID, seqIDLen);
    historyData.ResetSampleID(seqID);
    flowData.ResetSampleID(seqID);
}

void Sequence::SetActive()
{
    historyData.SetActive(seqType == stHistory);
    flowData.SetActive(seqType == stFlow);
}

void Sequence::ClearWasActive()
{
    historyData.ClearWasActive();
    flowData.ClearWasActive();
}


bool Sequence::SetupOK(SC_SetupErr& errData)
{
    SetActive();

    SC_SetupErr seqErr;

    if(StringLength(seqID) == 0)
        seqErr.SetConstantError("sequence ID not set");

    if(RealIsNull(seqLength.GetMetricVal()) ||
       (seqLength.GetMetricVal() < 1.0E-09))
    {
        seqErr.SetConstantError("duration not set/not +ve");
    }
    else
    {
        timeStep.CheckError(seqErr, seqLength.GetMetricVal());
    }

    switch (seqType) {
        case stFlow :  {
            flowData.CheckError(seqErr, timeStep);
            break;
            }
        case stHistory :  {
            historyData.CheckError(seqErr, timeStep);
            break;
            }
        case stPulse :  {
            pulseData.CheckError(seqErr);
            break;
            }
        case stSlug :  {
            slugData.CheckError(seqErr);
            break;
            }
    }

    return errData.SetupOK(seqID, seqErr);
}

