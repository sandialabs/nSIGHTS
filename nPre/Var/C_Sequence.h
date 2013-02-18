///////////////////////////////////////////////////////////////////////////////////
//
// C_Sequence.h
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
//      Classes to define sequences for simulator
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_SEQUENCE_H
#define C_SEQUENCE_H

#include <genClass/T_SC_Array.h>
#include <genApp/C_CfgFileObj.h>

#include <Var/C_Units.h>
#include <osClass/C_SampVarUnits.h>
#include <osClass/C_VaryVarUnits.h>


class  TimeStep {
    public:
        enum TimeStepType   {tstStatic, tstLog, tstDynamicP, tstDynamicQ};
        enum                {maxTimeSteps = 15000};

        TimeStepType        timeStepType;
        UnitReal            staticSize;
        UnitReal            firstLogSize;
        int                 nlogSteps;
        UnitReal            dynamicMax;
        UnitReal            dynamicMin;

    public:
                    TimeStep();
                    TimeStep(const TimeStep& a);
        TimeStep&   operator=(const TimeStep& a);
        bool        CheckError(SC_SetupErr& tsErr,
                                double  seqLength);


    private:
        void        LocalCopy(const TimeStep& a);
};

class FlowHistBase {
    public:
        enum WellboreStorage    {wsNone, wsIsolated, wsOpen};
        enum ValueType          {vtFixed, vtVary, vtCurve, vtSampled, vtCurveSampled};
        ValueType               valueType;
        UnitReal                fixedValue;
        bool                    isSampled;          //  isActive flag used by SampVar class
        SampVarUnits            sampleValue;
        bool                    isVaried;           //  isActive flag used by VaryVar class
        VaryVarUnits            varyValue;
        WellboreStorage         wellboreStorage;
    private:
        char                    seqSampVaryID[80];

    public:
                            FlowHistBase(UnitType uType, int corrGroup);
                            FlowHistBase(const FlowHistBase& a);
        FlowHistBase&       operator=(const FlowHistBase& a);
        void                SetActive(bool isActive);
        void                SetForFlags();
        void                CheckError(SC_SetupErr& tsErr);
        void                ClearWasActive();

    protected:
        void                SetID(const char* seqID, const char* typeID);

    private:
        void                LocalCopy(const FlowHistBase& a);

};


class FlowSequence : public FlowHistBase {
    public:
                        FlowSequence();
                        FlowSequence(const FlowSequence& a);
    FlowSequence&       operator=(const FlowSequence& a);

    void                CheckError(SC_SetupErr& tsErr,
                                const TimeStep& tsData);
    void                ResetSampleID(const char* seqID);
};

class HistorySequence : public FlowHistBase {
    public:
                        HistorySequence();
                        HistorySequence(const HistorySequence& a);
    HistorySequence&    operator=(const HistorySequence& a);

    void                CheckError(SC_SetupErr& tsErr,
                                const TimeStep& tsData);
    void                ResetSampleID(const char* seqID);
};

class PulseSlugBase {
    public:
        enum InitialPressureType    {ipAbsolute, ipTSRelative, ipSeqRelative};
        InitialPressureType         initialPressureType;
        UnitReal                    initialPressure;
    public:
                            PulseSlugBase();
                            PulseSlugBase(const PulseSlugBase& a);
        PulseSlugBase&      operator=(const PulseSlugBase& a);

        void                CheckError(SC_SetupErr& tsErr);

};

class PulseSequence : public PulseSlugBase {
    public:
                            PulseSequence();
                            PulseSequence(const PulseSequence& a);
        PulseSequence&      operator=(const PulseSequence& a);
};

class SlugSequence : public PulseSlugBase {
    public:
                            SlugSequence();
                            SlugSequence(const SlugSequence& a);
        SlugSequence&       operator=(const SlugSequence& a);

};

//  all classes combined

class Sequence {
    public:
        enum SequenceType {stFlow, stHistory, stPulse, stSlug};
        enum            {seqIDLen = 40};
        char            seqID[seqIDLen];
        bool            makeDefaultID;
        SequenceType    seqType;
        UnitReal        seqLength;         // duration
        TimeStep        timeStep;
        HistorySequence historyData;
        FlowSequence    flowData;
        PulseSequence   pulseData;
        SlugSequence    slugData;

        //  these are not entered but are set/used by the G_Sequence routines
        //  to support start time entry
        double          startTime;
        double          endTime;

        bool            useInAutoSetup; // added for better Auto support

    public:
                        Sequence();
                        Sequence(const Sequence& a);
        Sequence&       operator=(const Sequence& a);

        void            SetActive();
        void            ClearWasActive();

        void            SetForFlags();

        int             GetNTimeStep(); // approx - used by capture data alloc routines

        // checks/changes time step to be OK for new type
        void            ChangeSeqType(SequenceType  newType);

            // checking
        bool            SetupOK(SC_SetupErr& errData);      // returns true if basic setup is OK

        void            SetID(const char* sID);


    private:
        void            LocalCopy(const Sequence& a);

};



#endif // C_SEQUENCE_H
