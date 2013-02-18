///////////////////////////////////////////////////////////////////////////////////
//
// LFO_OptRun.h
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
//      lists summary data for an optimization run.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef LFO_OPTRUN_H
#define LFO_OPTRUN_H

#include <genClass/T_SC_Array.h>
#include <genClass/DO_Real.h>
#include <genClass/SC_Statistics.h>

#include <osClass/DO_OptSimResults.h>

#include <osLFO/LFO_OptBase.h>

struct  FitVarData {
    public:
        double  estVar;
        double  sse;
        double  stdDev;
        int     npts;
};


typedef T_SC_Array<SC_StatisticsArray> StatsMatrix;


class LFO_OptRun : public LFO_OptBase {
    public:
    protected:
        FuncObjRef      optSimResultsObjRef;

        bool            listRuns;
        SC_IntArray     runsToList;
        SC_IntArray     casesToList;

        //  covariance selection
        bool            csActual;
        bool            csEstimated;
        bool            csFitComponent;

        //  multiple opt selection - at least one must be true if runType != single
        bool            mosListAll;
        bool            mosListBest;
        bool            mosListStatistics;

        //  list categories
        bool            listRunSummary;
        bool            listFitValues;
        bool            listParameterEstimates;
        bool            listEstimatedDataVariances;
        bool            listParameterCorrelations;
        bool            listNormalizedCovarianceMatrices;
        bool            listDenormalizedCovarianceMatrices;
        bool            list95ConfidenceLimits;
        bool            listJacobian;
        bool            listModelStructure;
        bool            listCovarianceMatrixCorrelations;

    private:
        T_SC_Array<OptSimRunResults*>   selectedOpts;
        SC_IntArray                     selectedFit;
        SC_IntArray                     selectedCases;
        SC_IntArray                     currentCases;
        OptSimResultsHeader             runHeader;
        int                             singleFitCase;
        int                             ncurrCase;
        DC_FullCovar                    covarData;
        DC_Jacobian                     jacobData;
        bool                            badOptMsg;
        bool                            allCasesOK;

    public:
                        LFO_OptRun(ListDefC& assLst);
                        LFO_OptRun(const LFO_OptRun& a, ListDefC& assLst);
                        ~LFO_OptRun();

        LFO_OptRun& operator= (const LFO_OptRun& a);

                        //  FuncObjC virtuals
        virtual void    DoStatusChk();

                        // C_Listing virtuals
        void            CreateListing();

    private:
        void            InitCommon();                             // common to normal and copy constructor
        void            LocalCopy(const LFO_OptRun& a);  // common to copy constructor and = operator

        // main list routines
        void            ListRunSummary(const OptSimRunResults& currRun,
                                             int                fitIndex);

        void            ListFitValues(const OptSimRunResults& currRun,
                                            int             fitIndex);

        void            ListParameterEstimates(const OptSimRunResults& currRun,
                                            int             fitIndex);

        void            ListEstimatedDataVariances(const OptSimRunResults& currRun,
                                            int             fitIndex);

        void            ListParameterCorrelations(const OptSimRunResults& currRun,
                                                    int             fitIndex);

        void            ListCovarianceMatrices(const OptSimRunResults& currRun,
                                                     int                fitIndex,
                                                     bool           denorm);
        void            List95ConfidenceLimits(const OptSimRunResults& currRun,
                                                    int             fitIndex);

        void            ListJacobian(const OptSimRunResults& currRun,
                                           int              fitIndex);

        void            ListCovarianceMatrixCorrelations(const OptSimRunResults& currRun,
                                                       int              fitIndex);

        // misc list support

        bool            SingleFitOnly();

        void            SetChisqTable(const OptSingleFit& currFit);

        void            SetChisqStatsTable(const SC_StatisticsArray&  stats,
                                                 SC_Statistics::StatisticOp op);

        void            SetNonChisqTable(const OptSingleFit& currFit);

        void            SetNonChisqStatsTable(const SC_StatisticsArray&  stats,
                                                    SC_Statistics::StatisticOp op);

        void            GetEstVarData(const OptSingleFit& currFit,
                                            int           fitIndex,
                                            int           fitCompIndex,
                                            FitVarData&   varData);

        void            SetEstVarTable(const char*  fitID,
                                       const FitVarData& varData);

        void            EstVarTableSetup();

        void            SetVarEstStatsTable(const SC_StatisticsArray&  stats,
                                                SC_Statistics::StatisticOp op);

        void            GetCorrelationData(const OptSimRunResults& currRun,
                                                 int             fitIndex,
                                                 int             parIndex,
                                                 SC_DoubleArray& corrData);


        void            SetCovar(const OptSimRunResults& currRun,
                                 const OptSingleFit&      currFit,
                                        int          covarIndex,
                                        bool         denorm);

        void            ListCovarStats(const OptSimRunResults&  currRun,
                                             int                fitIndex,
                                             const char*        fitID,
                                             const char*        subFitID,
                                             int                covarIndex,
                                             bool               denorm);

        void            SetCovarStats(const StatsMatrix&            statsMatrix,
                                            const char*             fitID,
                                            const char*             subFitID,
                                            int                     index,
                                            SC_Statistics::StatisticOp op);

        void            ListAllCovar(const  OptSimRunResults&   currRun,
                                            int             fitIndex,
                                            const char*             fitID,
                                            const char*             subFitID,
                                            int             covarIndex,
                                            bool                denorm);

        void            ListAll95(const OptSimRunResults&   currRun,
                                             int                fitIndex,
                                       const char*              covarID,
                                             int                covarIndex);

        void            ListJac(const OptSimRunResults& currRun,
                                      int               caseIndex,
                                      int               fitIndex,
                                      const char*       runID);

        void            ListAllCovarCorr(const OptSimRunResults&    currRun,
                                             int                fitIndex,
                                            const char*             fitID,
                                            const char*             subFitID,
                                             int                covarIndex);

        void            ListOptOK(const OptSingleFit& currFit);


};

#endif // !LFO_OPTRUN_H

