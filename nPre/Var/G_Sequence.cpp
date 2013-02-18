///////////////////////////////////////////////////////////////////////////////////
//
// G_Sequence.cpp
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
//      Sequence specification and associated file I/O, error checking and listing
//  support.
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <genClass/U_Str.h>


#ifndef ADCONSOLEAPP
#include <App/nPreUtil.h>
#endif

#include <Var/G_PreConfigFile.h>

#include <Var/G_Control.h>
#include <Var/G_Sequence.h>

namespace nsSequence  {


    SequenceArray       sequenceData;
    TestZoneCurveArray  testZoneCurves;

    bool                enterSequenceDurations = false;
    UnitReal            testStartTime(uTime, 0.0);
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
    SequenceCurveFO         sequenceCurvesFO;
    SequenceErrorListing    sequenceErrorListing;

    SequenceStaticSupport::SequenceStaticSupport()
    {
        sequenceData.SetResizable();
        partialStartSeqID[0] = '\0';
        partialEndSeqID[0] = '\0';

    }

    void SequenceStaticSupport::SetToInitial()
    {
        sequenceData.DeAlloc();
        sequenceData.Alloc(10);

        // start with a default flow sequence
        sequenceData.CreateIfNecessary(0);
        sequenceData[0]->seqType = Sequence::stFlow;
        sequenceData[0]->SetID("F_01");

        sequenceData.SetSize(1);
        testZoneCurves.DeAlloc();

        enterSequenceDurations = true;
        testStartTime = UnitReal(uTime, 0.0);
        testEndTime = UnitReal(uTime, 1E+07);

        isPartialRun        = false;
        partialStartSeqVal  = -1;
        partialEndSeqVal    = -1;
        partialStartSeqID[0] = '\0';
        partialEndSeqID[0] = '\0';

    }


    void SequenceStaticSupport::SetForFlags()
    {
        int i;
        for (i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
                sequenceData[i]->SetForFlags();

        for (i = 0; i < testZoneCurves.Size(); i++)
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
        sequenceData.Cleanup();
        testZoneCurves.Cleanup();
    }

    void SequenceStaticSupport::SetSequenceTimes(bool doRecalc)
    {
        double currStart =  testStartTime.GetMetricVal();

        bool nullLengthFound = false;

        for (int i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
            {
                Sequence& currSeq = sequenceData.GetRef(i);
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
        if ((!enterSequenceDurations) &&
            (!sequenceData.IsEmpty()) &&
            (sequenceData.IsValid(sequenceData.Size() - 1)) &&
            ((!nullLengthFound) || (sequenceData.Size() == 1)))
            {
                Sequence& currSeq = sequenceData.GetRef(sequenceData.Size() - 1);
                currSeq.seqLength.SetMetricVal(testEndTime.GetMetricVal() - currSeq.startTime);
            }

    #ifndef ADCONSOLEAPP
        if (doRecalc)
            nPreUtilities::ExecuteAndReplot(sequenceTimesFO);
    #endif

    }

    bool SequenceStaticSupport::SetSequenceIDs()
    {
        int nextFlow    = 1;
        int nextHist    = 1;
        int nextPulse   = 1;
        int nextSlug    = 1;
        bool retVal = false;

        for (int i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
            {
                int nextVal;
                char nextChar;
                Sequence& currSeq = sequenceData.GetRef(i);
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
                    CopyString(newDesig, "__", 40);
                    newDesig[0] = nextChar;
                    ConcatString(newDesig, nextIStr, 40);
                    retVal = retVal || (strcmp(newDesig, currSeq.seqID) != 0);
                    currSeq.SetID(newDesig);
                }
            }

        return retVal;
    }


    void SequenceStaticSupport::CheckAndAllocSequence(int seqIndex)
    {
        sequenceData.CreateIfNecessary(seqIndex);
        /*
        if (sequenceData.CreateIfNecessary(seqIndex))
        {
            IntToString(seqIndex, sequenceData[seqIndex]->seqID, 20);
        }
        */
    }

    void SequenceStaticSupport::CheckAndAllocTestZoneCurve(int tzcIndex)
    {
        testZoneCurves.CreateIfNecessary(tzcIndex);
    }



    bool SequenceStaticSupport::GetSequenceIDs(SC_StringArray& ids)
    {
        ids.DeAlloc();
        ids.SetStringLen(Sequence::seqIDLen);
        ids.Alloc(sequenceData.Size());
        for (int i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
                ids += sequenceData[i]->seqID;

        return !ids.IsEmpty();
    }

    bool SequenceStaticSupport::GetSequenceIndex(const char* inID,
                                                        int&         outIndex)
    {
        for (int i = 0; i < sequenceData.Size(); i++)
            if ((sequenceData.IsValid(i)) && (strcmp(sequenceData[i]->seqID, inID) == 0))
            {
                outIndex = i;
                return true;
            }

        outIndex = -1;
        return false;
    }

    int SequenceStaticSupport::GetTotalNTimeStep()
    {
        int nts = 0;
        for (int i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
                nts+= sequenceData[i]->GetNTimeStep();
        return nts;
    }

    bool  SequenceStaticSupport::UsesDynamicTS()
    {
        for (int i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
            {
                TimeStep::TimeStepType tst = sequenceData[i]->timeStep.timeStepType;
                if ((tst == TimeStep::tstDynamicP) || (tst == TimeStep::tstDynamicQ))
                    return true;
            }
        return false;
    }


    bool  SequenceStaticSupport::ContainsSlugBC()
    {
        for (int i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
            {
                Sequence& currSeq = sequenceData.GetRef(i);
                switch (currSeq.seqType) {
                    case Sequence::stFlow :  {
                        if (currSeq.flowData.wellboreStorage == FlowHistBase::wsOpen)
                            return true;
                        break;
                    }
                    case Sequence::stHistory :  {
                        if (currSeq.historyData.wellboreStorage == FlowHistBase::wsOpen)
                            return true;
                        break;
                    }
                    case Sequence::stPulse :  {
                        break;
                    }
                    case Sequence::stSlug :  {
                        return true;
                    }
                }
            }
        return false;
    }

    bool  SequenceStaticSupport::ContainsPulseBC()
    {
        for (int i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
            {
                Sequence& currSeq = sequenceData.GetRef(i);
                switch (currSeq.seqType) {
                    case Sequence::stFlow :  {
                        if (currSeq.flowData.wellboreStorage == FlowHistBase::wsIsolated)
                            return true;
                        break;
                    }
                    case Sequence::stHistory :  {
                        if (currSeq.historyData.wellboreStorage == FlowHistBase::wsIsolated)
                            return true;
                        break;
                    }
                    case Sequence::stPulse :  {
                        return true;
                    }
                    case Sequence::stSlug :  {
                        break;
                    }
                }
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

    void SequenceListing::ListTimeStep(const TimeStep& timeStep)
    {
        ClearEnumStrs();
        AddEnumStr("Static",        TimeStep::tstStatic);
        AddEnumStr("Logarithmic",   TimeStep::tstLog);
        AddEnumStr("Dynamic P",     TimeStep::tstDynamicP);
        AddEnumStr("Dynamic Q",     TimeStep::tstDynamicQ);
        AddStdEnum("  Time step type", timeStep.timeStepType);
        switch (timeStep.timeStepType) {
            case TimeStep::tstStatic :  {
                AddUnitReal("  Static time step size", timeStep.staticSize);
                break;
            }
            case TimeStep::tstLog : {
                AddUnitReal("  First log step size", timeStep.firstLogSize);
                AddStdInt("  # of log time steps", timeStep.nlogSteps);
                break;
            }
            case TimeStep::tstDynamicP: case TimeStep::tstDynamicQ : {
                AddUnitReal("  Minimum time step size", timeStep.dynamicMin);
                AddUnitReal("  Maximum time step size", timeStep.dynamicMax);
            }
        }

    }


    void SequenceListing::ListFlowHist(const FlowHistBase& flowHist)
    {
        ClearEnumStrs();
        AddEnumStr("Fixed",         FlowHistBase::vtFixed);
        AddEnumStr("Suite/Range",   FlowHistBase::vtVary);
        AddEnumStr("Curve",         FlowHistBase::vtCurve);
        AddEnumStr("Sampled",       FlowHistBase::vtSampled);
        AddEnumStr("Curve+Sampled",  FlowHistBase::vtCurveSampled);
        AddStdEnum("  Type",            flowHist.valueType);

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

        ClearEnumStrs();
        AddEnumStr("None",              FlowHistBase::wsNone);
        AddEnumStr("Isolated",          FlowHistBase::wsIsolated);
        AddEnumStr("Open",              FlowHistBase::wsOpen);
        AddStdEnum("  Wellbore storage",    flowHist.wellboreStorage);

    }


    void SequenceListing::ListPulseSlug(const PulseSlugBase& pulseSlug)
    {
        ClearEnumStrs();
        AddEnumStr("Absolute",              PulseSlugBase::ipAbsolute);
        AddEnumStr("TS Relative",           PulseSlugBase::ipTSRelative);
        AddEnumStr("Seq. Relative",         PulseSlugBase::ipSeqRelative);
        AddStdEnum("  Initial pressure type",   pulseSlug.initialPressureType);

        AddUnitReal("  Initial pressure", pulseSlug.initialPressure);

    }



    void SequenceListing::ListSequence(const Sequence& outSeq)
    {
        char titleStr[80];
        MakeString(titleStr, "Sequence: ", outSeq.seqID, 80);
        SubTitle(titleStr);

        ClearEnumStrs();
        AddEnumStr("Flow", Sequence::stFlow);
        AddEnumStr("History", Sequence::stHistory);
        AddEnumStr("Pulse", Sequence::stPulse);
        AddEnumStr("Slug", Sequence::stSlug);
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


        ClearEnumStrs();
        AddEnumStr("Static", TimeStep::tstStatic);
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
        }

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

        ClearEnumStrs();
        AddEnumStr("Pressure",          TestZoneCurve::tzPressure);
        AddEnumStr("Flow Rate",         TestZoneCurve::tzFlowRate);
        AddEnumStr("Temperature",       TestZoneCurve::tzTemperature);
        AddEnumStr("Compressibility",   TestZoneCurve::tzCompressibility);
        AddEnumStr("Delta Volume",      TestZoneCurve::tzDeltaVolume);
        AddEnumStr("Absolute Volume",   TestZoneCurve::tzAbsVolume);
        AddStdEnum("Curve type",        outTZC.tzType);

        AddStdText("Start sequence",    outTZC.startSeqID);
        AddStdText("End sequence",      outTZC.endSeqID);

        AddBoolText("Curve time base",          outTZC.timeIsSequenceBased, "Sequence", "Test");
        AddUnitIndex("Curve Y data units",      outTZC.curveYUnits);
        AddYesNo("Curve Y data is log 10",  outTZC.curveYisLog10);

        if (outTZC.timeIsSequenceBased)
        {
            AddUnitIndex("Curve time units",    outTZC.curveTimeUnits);
            AddYesNo("Curve time is log 10",    outTZC.curveTimeisLog10);
        }


    }

    void SequenceListing::CreateListing()
    {
        StdTitle("Sequences");
        int i;
        for (i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
                ListSequence(sequenceData.GetRef(i));

        if (!testZoneCurves.IsEmpty())
        {
            StdTitle("Test Zone Curves");
            for (i = 0; i < testZoneCurves.Size(); i++)
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
        int i;
        for (i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsNotValid(i))
            {
                AddError("Null sequence found");
            }
            else
            {
                if (!sequenceData.GetRef(i).SetupOK(seqErr))
                {
                    AddError(seqErr);
                }
            }

        if (isPartialRun)
        {
             if (!sequence.GetSequenceIndex(partialStartSeqID, partialStartSeqVal))
                AddError("Partial run start sequence not set or bad");
             if (!sequence.GetSequenceIndex(partialEndSeqID, partialEndSeqVal))
                AddError("Partial run end sequence not set or bad");
            if (partialEndSeqVal < partialStartSeqVal)
                AddError("Partial run end sequence before start sequence");
        }

        for (i = 0; i < testZoneCurves.Size(); i++)
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

        // check for required curves
        SC_BoolArray pressureReq(sequenceData.Size());
        SC_BoolArray flowReq(sequenceData.Size());
        SC_BoolArray tempReq(sequenceData.Size());
        for (i = 0; i < sequenceData.Size(); i++)
        {
            Sequence& currSeq = sequenceData.GetRef(i);
            flowReq[i] = (currSeq.seqType == Sequence::stFlow) &&
                ((currSeq.flowData.valueType == FlowHistBase::vtCurve) ||
                (currSeq.flowData.valueType == FlowHistBase::vtCurveSampled));
            pressureReq[i] = (currSeq.seqType == Sequence::stHistory) &&
                ((currSeq.historyData.valueType == FlowHistBase::vtCurve) ||
                           (currSeq.historyData.valueType == FlowHistBase::vtCurveSampled));
            tempReq[i] = testZoneTemperatureVaries &&
                            (
                            (currSeq.seqType == Sequence::stPulse) ||
                            ((currSeq.seqType == Sequence::stFlow) &&
                            currSeq.flowData.wellboreStorage == FlowHistBase::wsIsolated));

        }

        // required
        CheckRequiredCoverage("Pressure", pressureReq, TestZoneCurve::tzPressure);
        CheckRequiredCoverage("Flow", flowReq, TestZoneCurve::tzFlowRate);
        CheckRequiredCoverage("Temperature", tempReq, TestZoneCurve::tzTemperature);


        // double coverage for remainder
        SC_IntArray coverageFound(sequenceData.Size(), 0);
        AddCoverage(coverageFound, TestZoneCurve::tzCompressibility);
        CheckDoubleCoverage("Compressibility", coverageFound);

        // reset to 0
        coverageFound.FillToSize(0);

        // only one each vol type allowed
        AddCoverage(coverageFound, TestZoneCurve::tzDeltaVolume);
        AddCoverage(coverageFound, TestZoneCurve::tzAbsVolume);
        CheckDoubleCoverage("Volume", coverageFound);
    }

    void SequenceErrorListing::AddCoverage (SC_IntArray&               coverageFound,
                                                         TestZoneCurve::TZCurveType crvType)
    {
        for (int i = 0; i < testZoneCurves.Size(); i++)
        {
            TestZoneCurve& currRef = testZoneCurves.GetRef(i);
            if (currRef.tzType == crvType)
                for (int j = currRef.startSeq; j <= currRef.endSeq; j++)
                    coverageFound[j]++;
        }
    }


    void SequenceErrorListing::CheckRequiredCoverage(const char*                   reqID,
                                                                  SC_BoolArray&                 crvReq,
                                                                  TestZoneCurve::TZCurveType    crvType)
    {
        SC_IntArray coverageFound(sequenceData.Size(), 0);
        AddCoverage(coverageFound, crvType);

        char tempStr[80];
        for (int i = 0; i < sequenceData.Size(); i++)
        {
            if (crvReq[i] && (coverageFound[i] == 0))
            {
                CopyString(tempStr, "no ", 80);
                ConcatString(tempStr, reqID, 80);
                ConcatString(tempStr, " coverage for sequence ", 80);
                ConcatString(tempStr, sequenceData.GetRef(i).seqID, 80);
                AddError(tempStr);
            }
        }

        CheckDoubleCoverage(reqID, coverageFound);

    }

    void SequenceErrorListing::CheckDoubleCoverage (const char*            reqID,
                                                     const SC_IntArray&     coverageFound)
    {
        char tempStr[80];
        for (int i = 0; i < sequenceData.Size(); i++)
        {
            if (coverageFound[i] > 1)
            {
                CopyString(tempStr, "extra ", 80);
                ConcatString(tempStr, reqID, 80);
                ConcatString(tempStr, " coverage for sequence ", 80);
                ConcatString(tempStr, sequenceData.GetRef(i).seqID, 80);
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
    const int  SequenceStaticSupport::majorVersion = 5;   // removed non-iso pulse
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
    //      inSeq.pulseData.isIsothermal = ReadBool();

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
    void SequenceStaticSupport::WriteToFile()
    {
        WriteGroupHeader(groupHeader, majorVersion, minorVersion);

        // global objects
        WriteGlobalFunc(sequenceTimesFO);

        WriteInt(sequenceData.ValidSize());
        WriteLine();

        SetRealConvFormat(SC_SciFormat(12));

        for (int i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
                WriteSequence(sequenceData.GetRef(i));

        WriteInt(testZoneCurves.ValidSize());
        WriteLine();
        for (int i = 0; i < testZoneCurves.Size(); i++)
            if (testZoneCurves.IsValid(i))
                WriteTestZoneCurve(testZoneCurves.GetRef(i));

        // Time-Base
        WriteBool(enterSequenceDurations);
        WriteUnitReal(testStartTime);
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

        sequenceData.AllocAndCreate(ReadInt());
        for (int i = 0; i < sequenceData.Size(); i++)
            ReadSequence(sequenceData.GetRef(i));

        testZoneCurves.AllocAndCreate(ReadInt());
        for (int i = 0; i < testZoneCurves.Size(); i++)
            ReadTestZoneCurve(testZoneCurves.GetRef(i));

        // Time-Base
        enterSequenceDurations = ReadBool();
        ReadUnitReal(testStartTime);
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

        SetSequenceTimes();

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



    void  SequenceTimesFO::DoStatusChk()
    {
        FuncObjC::DoStatusChk();

        sequenceTimesDC.Clear();
        if (sequenceData.IsEmpty())
            SetObjErrMsg("no sequences");

        UnitReal currStart(uTime, testStartTime.GetMetricVal());

        for (int i = 0; i < sequenceData.Size(); i++)
            if (sequenceData.IsValid(i))
            {
                Sequence& currSeq = sequenceData.GetRef(i);
                if (currSeq.seqLength.IsNull())
                {
                    SetObjErrMsg("sequence found with bad time");
                    sequenceTimesDC.Clear();
                    return;
                }
                sequenceTimesDC.AddSequence(currSeq.seqID, currStart.GetUserVal());
                currStart.metricVal += currSeq.seqLength.GetMetricVal();
            }
        sequenceTimesDC.AddEndTime(currStart.GetUserVal());

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

