///////////////////////////////////////////////////////////////////////////////////
//
// G_Sample.h
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
//      support for sampler usage.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef G_SAMPLE_H
#define G_SAMPLE_H

#include <genClass/DO_Real.h>
#include <genClass/DO_TableData.h>
#include <genClass/C_GlobalFunc.h>


#include <genListClass/C_ListObj.h>
#include <genListClass/C_ErrorListObj.h>
#include <genListClass/C_ListFactory.h>

#include <osClass/C_SampVar.h>
#include <osMain/C_Uncertain.h>

#include <genApp/C_CfgFileObj.h>
#include <genApp/G_AppObj.h>

namespace osSample  {

    class MetricSelection {
        public:
            bool        uncertOnMetric;
            FuncObjRef  metricObject;
            bool        useMetric;
        public:
            MetricSelection() {uncertOnMetric = false;}
            MetricSelection&   operator= (const MetricSelection& a)
            {
                uncertOnMetric = a.uncertOnMetric;
                metricObject = a.metricObject;
                return *this;
            }
    };

    class UncertInput : public GlobalFunc {

        public:
            enum                {maxMetricSel = 8};
            MetricSelection     inputMetrics[maxMetricSel];
            //int                 currSelectedMetric;

        private:
            bool                calcWasPerformed;  // set to false before sim, checked after
            SC_DoubleArray      lastMetricVal;
            //double                lastMetricVal;

        public:
                                UncertInput();
                                ~UncertInput(){};

            void                DoStatusChk();
            void                CalcOutput(FOcalcType  calcType);  // creates sample table
            virtual void        IsBeingDeleted(FuncObjC* delObj);  // set input flag
            int                 GetNMetrics();

            void                ClearCalcFlag() {calcWasPerformed = false;}
            bool                GetCalcFlag() {return calcWasPerformed;}
            SC_DoubleArray&     GetMetricValue() {return lastMetricVal;}
            //double                GetMetricValue() {return lastMetricVal;}
    };

    class SampleResults : public GlobalFunc {

        private:
            // output
            DO_TableData        sampResultsDO;
            DC_TableData        sampResultsDC;

        public:
                                SampleResults();
                                ~SampleResults(){};

            void                DoStatusChk();
            void                CalcOutput(FOcalcType  calcType);  // creates sample table
    };

    class UncertResults : public GlobalFunc {

        friend class UncertaintyPropagationPackage;

        private:
            // input - mean and std dev for each uncertain variable
            int                 nVar;
            DO_TableData        uncertInputDO;
            DC_TableData        uncertInputDC;
            // output - mean and std dev for each metric (results)
            int                 nMet;
            DO_TableData        uncertResultsDO;
            DC_TableData        uncertResultsDC;
            //output - variable components of mean and std dev
            DO_TableData        uncertComponentResultsDO;
            DC_TableData        uncertComponentResultsDC;

            public:
            // will cause MT problems - fix later
            static const UncertaintyPropagationPackage* currUncertPkg;


        public:
                                UncertResults();
                                ~UncertResults(){};

            void                DoStatusChk();
            void                CalcOutput(FOcalcType  calcType);  // creates sample table
            void                GetInput(DC_TableData& output) {output = uncertInputDC;}
            void                GetResults(DC_TableData& output) {output = uncertResultsDC;}
            void                GetComponents(DC_TableData& output) {output = uncertComponentResultsDC;}
    };

    class SampleListing : public ListObjC, public SampleControlStatics {
        public:
                    SampleListing();
                    ~SampleListing() {};

            virtual void  CreateListing();
        private:

    };

    class SampleErrorListing : public ErrorListObjC, public SampleControlStatics {
        public:
                    SampleErrorListing();
                    ~SampleErrorListing() {};

            virtual void  CreateListing();

    };

    extern SampleErrorListing sampleErrorListing;

    class SampleStaticSupport : public ListFactory,
                                public CfgFileObj,
                                public SampleControlStatics  // IO & listing support
    {

       private:
                // object versioning support
            static const char*  groupHeader;
            static const int    majorVersion;
            static const int    minorVersion;

            static appFuncObjGlobals::AppFuncArray sampleObj;

        public:
                        SampleStaticSupport();     // constructor initializes all parameters and vars
                        ~SampleStaticSupport() {};

        static void     InitAppGlobals();   // must be called once after creation and before use
        static void     SetToInitial();     // for file New
        static void     SetForFlags();      // modifies settings
        static bool     SampleOK();         // pre-run check
        static bool     Sample();           // does sampling
        static bool     Uncertain();        // does uncertainty checks and table output
        static void     CopyUncertainTables(DC_TableData&  outputVarInp,
                                            DC_TableData& outputRes,
                                            DC_TableData&  outputComp);  //copies uncertain results tables for output control

        virtual void    WriteToFile();

                void    PreReadSetup();    // must be called before any sampled vars are read
        virtual bool    ReadFromFile();    // must NOT be called untill all sampled vars are read

        virtual ListObjC*       CreateListObject() {return new SampleListing();}
        virtual ErrorListObjC*  GetErrorListObject() {return &sampleErrorListing;}

    };

    extern SampleResults            sampResultsFO;
    extern SampleStaticSupport      sample;           // the one and only
    extern UncertInput              uncertInput;
    extern UncertResults            uncertResultsFO;


};


//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace osSample;


#endif // !G_SAMPLE_H

