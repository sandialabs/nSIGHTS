///////////////////////////////////////////////////////////////////////////////////
//
// DC_OptSimResults.h
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
//      class to package up all optimizer results for file I/O.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_OPTSIMRESULTS_H
#define DC_OPTSIMRESULTS_H

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_StringArray.h>

//  these are completely separated logically from nsOptSupport to avoid dragging any C_OptVar stuff into nPost

// *********************************************  first section is results
// results for a single fit component
class OptFitComponent {
    public:
        double              fitSumSquaredErrors;
        double              fitEstimatedVariance;
        int                 nfitPoints;     // convenience
        SC_DoubleMatrix     fitCovariance;  // size noptVar x noptVar
        SC_DoubleArray      fitResiduals;   // size nfitPoints
        SC_DoubleMatrix     fitJacobian;    // noptVar x nfitPoints

    public:
                            OptFitComponent();
                            ~OptFitComponent() {};
                            OptFitComponent(const OptFitComponent& a);

        OptFitComponent&    operator=(const OptFitComponent& a);

    private:
        void                LocalCopy(const OptFitComponent& a);
};
typedef T_SC_Array<OptFitComponent> OptFitComponentArray;

// results for a single run of a single fit optimization
class OptSingleFit {
    public:
        bool                    fitCompletedOK;     // true if no error on fit
        int                     nsimsRequired;      // # of simulations to complete optimization
        int                     niteration;         // # of optimizer iterations to complete optimization

        //  scalars
        double                  fitValue;           // final result for fit
        double                  derivSpan;          // final derivative span

        //  for composite fit only meaningful if covar run
        double                  sumSquaredError;    // sse -- should be same as fitval for covar runs
        double                  meanSquaredError;   // mse
        double                  estimatedVariance;
        SC_DoubleMatrix         actualCovariance;  // size noptVar x noptVar
        SC_DoubleMatrix         estimatedCovariance;  // size noptVar x noptVar


        //  opt results Size() == noptVar
        SC_DoubleArray          finalParData;       // results for fitting parameters in user units
        SC_DoubleArray          finalNormParData;   // results for fitting parameters in normalize par space

        SC_DoubleArray          startParData;       // start point of fit for fitting parameters in user units
        SC_DoubleArray          startNormParData;   // start point of fit for fitting parameters in normalize par space
        double                  fitDelta;           // sum of normalized difference from start point

        SC_DoubleArray          calcParData;        // results for calculated parameters in user units

        OptFitComponentArray    fitComponents;      // size  nfit

    public:
                                OptSingleFit();
                                OptSingleFit(const OptSingleFit& a);
                                ~OptSingleFit() {};

        OptSingleFit&           operator=(const OptSingleFit& a);
        int                     GetnFitComponent() const {return fitComponents.Size();}
        int                     GetnFitPoints() const;

    private:
        void                    LocalCopy(const OptSingleFit& a);
};

typedef T_SC_Array<OptSingleFit>      OptSingleFitArray;

class OptCaseResults {
    public:
        enum                {caseDescLen = 80};
        char                caseDesc[caseDescLen];
        int                 caseIndex;      // for cluster version & combining fragments

        SC_DoubleArray      caseParVals;    // value of varying/sampled/perturbed parameters
                                            // for perturbed it is opt start val of par
        OptSingleFitArray   caseResults;    // for each fit

    public:
                             OptCaseResults();
                             OptCaseResults(const OptCaseResults& a);
                             ~OptCaseResults() {};

        OptCaseResults&      operator=(const OptCaseResults& a);
        int                  GetnFits() const {return caseResults.Size();}

        void                 Cleanup();

    private:
        void                 LocalCopy(const OptCaseResults& a);
};

typedef T_SC_Array<OptCaseResults> OptCaseResultsArray;


// *********************************************  second section is description
//  single fit component

class OptFitComponentDescription {
    public:
        enum                {fitIDLen = 40};
        char                fitID[fitIDLen];
        int                 nfitPoints;
        bool                fitWasL1;        // for non covar only
        bool                fitWasNormed;    // for non covar only
        double              fitChisqSigma;
        SC_DoubleArray      fitResidJacobX;   // fit X value -- used for resid and jacobean

    public:
                                    OptFitComponentDescription();
                                    ~OptFitComponentDescription() {};
                                    OptFitComponentDescription(const OptFitComponentDescription& a);

        OptFitComponentDescription& operator=(const OptFitComponentDescription& a);

    private:
        void                        LocalCopy(const OptFitComponentDescription& a);
};

typedef T_SC_Array<OptFitComponentDescription> OptFitComponentDescriptionArray;


//  composite fit
class OptFitDescription{
    public:
        // description of composite fit
        enum                            {fitIDLen = 40};
        char                            fitID[fitIDLen];
        SC_DoubleArray                  fitScaling;      // for non covar only
        OptFitComponentDescriptionArray fitComponentDescription;
    public:
                                OptFitDescription();
                                OptFitDescription(const OptFitDescription& a);
                                ~OptFitDescription() {};
        OptFitDescription&      operator=(const OptFitDescription& a);
        int                     GetnFitComponents() {return fitComponentDescription.Size();}

    private:
        void                    LocalCopy(const OptFitDescription& a);

};
typedef T_SC_Array<OptFitDescription> OptFitDescriptionArray;

class OptVarDescription{
    public:
        // description of normalization for single opt var
        enum                    {varDescLen = 80};
        char                    varDesc[varDescLen];

        enum                    {shortVarDescLen = 20};
        char                    shortVarDesc[shortVarDescLen];

        enum                    {varUnitsLen = 20};
        char                    varUnits[varUnitsLen];

        double                  userMinLim;  // in user units
        double                  userMaxLim;

        double                  parSigma;
        bool                    parIsLinear;

    public:
                                OptVarDescription();
                                OptVarDescription(const OptVarDescription& a);
                                ~OptVarDescription() {};
        OptVarDescription&      operator=(const OptVarDescription& a);

    private:
        void                    LocalCopy(const OptVarDescription& a);

};
typedef T_SC_Array<OptVarDescription> OptVarDescriptionArray;

class CaseVarDescription{
    public:
        // description of case varying/sampled parameters
        enum                    {caseVarDescLen = 80};
        char                    caseVarDesc[caseVarDescLen];
        enum                    {caseVarShortDescLen = 20};
        char                    caseVarShortDesc[caseVarShortDescLen];
        enum                    {caseVarUnitsLen = 20};
        char                    caseVarUnits[caseVarUnitsLen];

        bool                    varIsLinear;

    public:
                                CaseVarDescription();
                                CaseVarDescription(const CaseVarDescription& a);
                                ~CaseVarDescription() {};
        CaseVarDescription&      operator=(const CaseVarDescription& a);

        void                    SetVars(const char* vDes, const char* vShort, const char* vUn, bool isLin);

    private:
        void                    LocalCopy(const CaseVarDescription& a);

};
typedef T_SC_Array<CaseVarDescription> CaseVarDescriptionArray;



// *********************************************  combined results & description

class OptSimRunResults {
    public:
        enum                        {runIDLen = 40};
        char                        runID[runIDLen];

        //  run specific info
        bool                        fullCovarianceCalc;
        enum                        {optDescLen = 60};
        char                        optDescription[optDescLen];

        //  values for convenience
        int                         nFits;
        int                         ncaseDesc;

        OptFitDescriptionArray      fitDescriptions;    // size n fit
        OptVarDescriptionArray      varDescriptions;    // size n opt var
        CaseVarDescriptionArray     calcDescriptions;   // size n calc var
        CaseVarDescriptionArray     caseDescriptions;   // size 0/noptvar/nvarypar/nsamplepar/ based on runType
        OptCaseResultsArray         runResults;         // size 1/nperturb/nvaryrun/nsample based on run type

    public:
                            OptSimRunResults();
                            OptSimRunResults(const OptSimRunResults& a);
                            ~OptSimRunResults() {};

        OptSimRunResults&    operator=(const OptSimRunResults& a);

        const char*         GetFitID(int fitIndex) const;
        const char*         GetSubFitID(int fitIndex, int subfitIndex) const;

        int                 GetnCases() const {return runResults.Size();}
        int                 GetnFits() const {return fitDescriptions.Size();}
        void                GetFitIDs(SC_StringArray&  fitIDs) const;

        void                Cleanup();  // de allocs all arrays

    private:
        void                LocalCopy(const OptSimRunResults& a);
};

class OptSimResultsHeader {
    public:
        enum                    RunType {rtSingle, rtPerturb, rtMultiple, rtSample};

        //  basic run type -- for all runs in file
        RunType                 runType;
        bool                    containsCovariance;
        bool                    containsResiduals;
        bool                    containsJacobian;
        int                     noptVar;
        int                     ncalcVar;

                                OptSimResultsHeader();
                                OptSimResultsHeader(const OptSimResultsHeader& a);
                                ~OptSimResultsHeader() {};

        OptSimResultsHeader&    operator=(const OptSimResultsHeader& a);

        bool                    AreCompatible(const OptSimResultsHeader& a);

        inline bool             IsSingle()      const {return runType == rtSingle;}
        inline bool             IsPerturb()     const {return runType == rtPerturb;}
        inline bool             IsMultiple()    const {return runType == rtMultiple;}
        inline bool             IsSample()      const {return runType == rtSample;}

    private:
        void                LocalCopy(const OptSimResultsHeader& a);
};



class DC_OptSimResults : public OptSimResultsHeader, public T_SC_Array<OptSimRunResults> {

    public:
                            DC_OptSimResults();
                            DC_OptSimResults(const DC_OptSimResults& a);
                            ~DC_OptSimResults();

        DC_OptSimResults&    operator=(const DC_OptSimResults& a);

        int                 GetnRuns() const {return Size();}
        int                 GetnCases() const;
        int                 GetnFits() const;
        int                 GetnFitComponents() const;

        void                GetRunIDs(SC_StringArray&  runIDs) const;

                            // for use in menus
        void                GetMenuStrings(SC_StringArray&  ids,
                                           bool             includeFitComponents,
                                           bool             includeOKonly) const;

    private:
        void                LocalCopy(const DC_OptSimResults& a);

};

#endif // !DC_OPTSIMRESULTS_H

