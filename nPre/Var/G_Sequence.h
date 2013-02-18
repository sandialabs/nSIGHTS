///////////////////////////////////////////////////////////////////////////////////
//
// G_Sequence.h
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

#ifndef G_SEQUENCE_H
#define G_SEQUENCE_H

#include <genClass/C_FuncObj.h>
#include <genClass/T_SC_AllocPtrArray.h>
#include <genClass/SC_StringArray.h>

#include <genApp/C_CfgFileObj.h>
#include <genApp/G_AppObj.h>

#include <osMain/C_OSListObj.h>

#include <genListClass/C_ErrorListObj.h>
#include <genListClass/C_ListFactory.h>

#include <nsClass/DO_SequenceTimes.h>

#include <Var/C_Sequence.h>
#include <Var/C_TestZoneCurve.h>

#include <Var/C_Units.h>


namespace nsSequence  {

    class SequenceArray: public T_SC_AllocPtrArray<Sequence> {
        public :
                        SequenceArray():T_SC_AllocPtrArray<Sequence>() {}

    };

    class TestZoneCurveArray : public T_SC_AllocPtrArray<TestZoneCurve> {
        public :
                    //  constructors
                        TestZoneCurveArray():T_SC_AllocPtrArray<TestZoneCurve>() {}

    };

    // globals
    extern SequenceArray        sequenceData;
    extern TestZoneCurveArray   testZoneCurves;

    // Time-Base
    extern bool         enterSequenceDurations;
    extern UnitReal     testStartTime;
    extern UnitReal     testEndTime;

    // Dynamic Time Step
    extern UnitReal     maximumPressureChange;
    extern UnitReal     minimumPressureChange;
    extern UnitReal     maximumFlowRateChange;
    extern UnitReal     minimumFlowRateChange;
    extern int          maxNumberOfTS;

    // Partial Run
    extern bool         isPartialRun;
    extern int          partialStartSeqVal;
    extern int          partialEndSeqVal;
    extern char         partialStartSeqID[Sequence::seqIDLen];
    extern char         partialEndSeqID[Sequence::seqIDLen];


    // sequence time output to mView objects
    class SequenceTimesFO : public FuncObjC {
        private:
            DO_SequenceTimes    sequenceTimesDO;

        public:
            DC_SequenceTimes    sequenceTimesDC;

        public:
                                SequenceTimesFO();
                                ~SequenceTimesFO(){};
            void                DoStatusChk();
            void                CalcOutput(FOcalcType  calcType);

        private:
            // don't need these -- make sure they dont't get used
                                SequenceTimesFO(const SequenceTimesFO& a) : FuncObjC(a) {};
            SequenceTimesFO&    operator= (const SequenceTimesFO& a) {return *this;}

        void                InitCommon();                        // common to normal and copy constructor

    };

    // only purpose is sequence curve obj ref updating & check 
    class SequenceCurveFO : public FuncObjC {
        public:
                        SequenceCurveFO();
                        ~SequenceCurveFO(){};

        virtual void    CalcOutput(FOcalcType  calcType) {};
        virtual void    IsBeingDeleted(FuncObjC* delObj);
        virtual void    UpdateFuncObjRefs();
        virtual bool    UsesAsInput(const FuncObjC* inputObj) const; // used in building  connection list


        private:
            // don't need these -- make sure they dont't get used
                                SequenceCurveFO(const SequenceCurveFO& a) : FuncObjC(a) {};
            SequenceCurveFO&    operator= (const SequenceCurveFO& a) {return *this;}

    };

    class SequenceListing : public OSListObj {
        public: 
                        SequenceListing();
                        ~SequenceListing() {};

            virtual void  CreateListing();

        private:
            void        ListTestZoneCurve(const TestZoneCurve& outTZC);
            void        ListSequence(const Sequence& outSeq);
            void        ListTimeStep(const TimeStep& timeStep);
            void        ListFlowHist(const FlowHistBase& flowHist);
            void        ListPulseSlug(const PulseSlugBase& pulseSlug);
    };


    class SequenceErrorListing : public ErrorListObjC {
        public: 
                                SequenceErrorListing();
                                ~SequenceErrorListing() {};

                virtual void  CreateListing();
        private:
            void        CheckRequiredCoverage(const char*                   reqID,
                                              SC_BoolArray&                 crvReq,
                                              TestZoneCurve::TZCurveType    crvType);

            void        AddCoverage (SC_IntArray&                   coverageFound,
                                      TestZoneCurve::TZCurveType    crvType);
            void        CheckDoubleCoverage (const char*            reqID,
                                             const SC_IntArray&     coverageFound);

    };

    extern SequenceErrorListing  sequenceErrorListing;



    class SequenceStaticSupport : public    ListFactory,
                                 public CfgFileObj

    {
       private:
                // object versioning support
            static const char*  groupHeader;
            static const int    majorVersion;
            static const int    minorVersion;
            static appFuncObjGlobals::AppFuncArray sequenceObj;

        public:
                        SequenceStaticSupport();     // constructor initializes all parameters and vars
                        ~SequenceStaticSupport() {};

        static void     InitAppGlobals();   // must be called once after creation and before use
        static void     SetToInitial();     // for file New
        static void     SetForFlags();      // modifies settings
        static bool     SequencesOK();

        // misc static routines for sequence data
        static int      GetNSequence() {return sequenceData.Size();}
        static int      GetTotalNTimeStep();
        static bool     UsesDynamicTS();
        static void     SequenceCleanup();
        static void     CheckAndAllocSequence(int seqIndex);
        static void     SetSequenceTimes(bool doRecalc = true);
        static bool     SetSequenceIDs();

        // misc static routines for TZC data
        static int      GetNTestZoneCurve() {return testZoneCurves.Size();}
        static void     CheckAndAllocTestZoneCurve(int tzcIndex);

        // misc static for BC requirements
        static bool     ContainsSlugBC();
        static bool     ContainsPulseBC();

        //  ID extraction/matching
        static bool     GetSequenceIDs(SC_StringArray& ids);
        static bool     GetSequenceIndex(const char* inID,
                                         int&        outIndex);


        virtual void    WriteToFile();
        virtual bool    ReadFromFile();

        virtual ListObjC*   CreateListObject() {return new SequenceListing();}
        virtual ErrorListObjC*  GetErrorListObject() {return &sequenceErrorListing;}

        private:

            void        WriteSequence(Sequence& outSeq);
            void        ReadSequence(Sequence& inSeq);

            void        WriteTestZoneCurve(const TestZoneCurve& outTZC);
            void        ReadTestZoneCurve(TestZoneCurve& inTZC);

    };


    extern  SequenceStaticSupport   sequence;           // the one and only
    extern  SequenceTimesFO         sequenceTimesFO;
    extern  SequenceCurveFO         sequenceCurvesFO;

};

using namespace nsSequence;


#endif // G_SEQUENCE_H
