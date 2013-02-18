///////////////////////////////////////////////////////////////////////////////////
//
// G_Optimize.h
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
//      support for optimizer usage.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef G_OPTIMIZE_H
#define G_OPTIMIZE_H

#include <genClass/DO_Real.h>
#include <genClass/DO_TableData.h>
#include <genClass/C_GlobalFunc.h>

#include <genListClass/C_ListObj.h>
#include <genListClass/C_ErrorListObj.h>
#include <genListClass/C_ListFactory.h>

#include <osClass/C_OptVar.h>
#include <osClass/DC_OptSimResults.h>
#include <osClass/DC_FitResults.h>

#include <osMain/C_Optimize.h>

#include <genApp/C_CfgFileObj.h>
#include <genApp/G_AppObj.h>

//  namespace containing optimization variables

namespace osOptimize  {

    class FitSelection {
        public:
            bool        optimizeOnFit;
            FuncObjRef  fitObject;
        public:
                            FitSelection() {optimizeOnFit = false;}
            FitSelection&   operator= (const FitSelection& a)
                            {
                                optimizeOnFit = a.optimizeOnFit;
                                fitObject = a.fitObject;
                                return *this;
                            }
    };

    class CalcVarSelection {
        public:
            bool        includeCalcVar;
            FuncObjRef  calcVarObject;
        public:
                            CalcVarSelection() {includeCalcVar = false;}
            CalcVarSelection&   operator= (const CalcVarSelection& a)
                            {
                                includeCalcVar = a.includeCalcVar;
                                calcVarObject = a.calcVarObject;
                                return *this;
                            }
    };

    class OptimizeInput : public FuncObjC {
        public:
            enum                {maxOptFitSel = 8};
            FitSelection        inputFits[maxOptFitSel];
            int                 currSelectedFit;    // set by optimizer

            enum                {maxOptCalcVar = 8};
            CalcVarSelection    inputCalcVars[maxOptCalcVar];

            CaseVarDescriptionArray     calcVarDesc;
            SC_DoubleArray              calcVarValue;

            bool                calcVarOK;

        private:
            bool                calcWasPerformed;  // set to false before sim, checked after
            double              lastFitVal;

        public:
                                OptimizeInput();
                                ~OptimizeInput(){};

            void                DoStatusChk();
            void                CalcOutput(FOcalcType  calcType);
            virtual void        IsBeingDeleted(FuncObjC* delObj);  // set input flag

            int                 GetNFits();
            int                 GetMaxNFitComponents();

            int                 GetnCalc();


            void                ClearCalcFlag() {calcWasPerformed = false;}
            bool                GetCalcFlag() {return calcWasPerformed;}
            double              GetFitValue() {return lastFitVal;}

            FitResultsPtrArray& GetCurrentFitResults();
            int                 GetNCurrentFits();

    };

    class OptimizeRuntimeOutput : public GlobalFunc, public OptimizationControlStatics  {

        friend class OptimizerPackage;

        private:
            // output
            DO_TableData        optResultsDO;
            DC_TableData        optResultsDC;
            DO_Real             lastFitDO;
            bool                addSecondOptData;

    public:
            // will cause MT problems - fix later
            static const OptimizerPackage*  currOptPkg;         // set by optimzer for extracting tolerances

        public:
                                OptimizeRuntimeOutput();
                                ~OptimizeRuntimeOutput(){};

            void                Clear();  // deallocs table
            void                Setup();  // clears output, resets tables to current opt pars

            int                 GetNRows() {return optResultsDC.GetNRows();}
            double              GetLastFitVal() const {return lastFitDO.realVal;}

            void                DoStatusChk();
            void                CalcOutput(FOcalcType  calcType);  // adds current opt pars to table

        private:
            int                 GetNTableCols();
    };

    //  this class performs all the messy translation between the baroque DC_OptSimResults structures and
    //  the fit results,
    //  It is used by the optOutputFile routines in G_OutputFiles

    class OptimizeFileOutput : public OptimizationControlStatics {

        friend class Optimizer;

        public:
            // output
            OptSimResultsHeader  runDesc;
            OptSimRunResults     runHeader;
            OptCaseResults       nextCase;

        private:
            int                 nextFit;

        public:
                                OptimizeFileOutput() {};
                                ~OptimizeFileOutput(){};

                                // prepares runHeader for output to file
            void                SetupRunHeader(const char*                     runID,
                                               const OptSimResultsHeader&     inRunDesc,
                                               const CaseVarDescriptionArray& inCaseDesc,
                                               const Optimizer&               opt);

                                // prepares case for output
            void                SetupCaseHeader(const char*           caseDesc,
                                                const SC_DoubleArray& inCaseVals,
                                                      int             caseIndex);

                                // adds results from last fit to nextCase
            void                AddNextFit(      bool       fitAborted,
                                           const Optimizer& opt);


            // for best-fit forward run output file support
            int                 GetNFits() const;

            void                Cleanup();  // deallocs all when complete
    };


    class OptimizeListing : public ListObjC, public OptimizationControlStatics {
        public:
                    OptimizeListing();
                    ~OptimizeListing() {};

                                                    // C_Listing virtuals
            virtual void  CreateListing();
        private:

    };

    class OptimizeErrorListing : public ErrorListObjC, public OptimizationControlStatics {
        public:
                    OptimizeErrorListing();
                    ~OptimizeErrorListing() {};

            virtual void  CreateListing();

    };

    extern OptimizeErrorListing     optimizeErrorListing;


    class OptimizeStaticSupport :   public ListFactory,
                                    public CfgFileObj,
                                    public OptimizationControlStatics  // IO & listing support
    {

        private:
                // object versioning support
            static const char*  groupHeader;
            static const int    majorVersion;
            static const int    minorVersion;

            static appFuncObjGlobals::AppFuncArray optimizerObj;

        public:
                        OptimizeStaticSupport();     // constructor initializes all parameters and vars
                        ~OptimizeStaticSupport() {};

        static void     InitAppGlobals();   // must be called once after creation and before use
        static void     SetToInitial();     // for file New
        static void     SetForFlags();      // modifies settings
        static bool     FitCalcOK();        // pre-run check for range non-opt runs
        static bool     OptimizeOK();       // pre-run check
        static void     UpdateStatus();     // resets output tables -- used to ensure that all
                                            // defined OptVars show up in outputTable

        virtual void    WriteToFile();
        virtual bool    ReadFromFile();

        virtual ListObjC*       CreateListObject() {return new OptimizeListing();}
        virtual ErrorListObjC*  GetErrorListObject() {return &optimizeErrorListing;}

    private:
        static OptimizationType ReadOptType();


    };

    extern OptimizeInput            optimizeInput;
    extern OptimizeRuntimeOutput    optCompleteOutput;  // after each opt is complete
    extern OptimizeRuntimeOutput    optProgressOutput;  // during each optimization
    extern OptimizeFileOutput       optFileOutput;

    extern OptimizeStaticSupport    optimize;           // the one and only

};


//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace osOptimize;


#endif // !G_OPTIMIZE_H

