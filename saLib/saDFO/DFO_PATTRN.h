///////////////////////////////////////////////////////////////////////////////////
//
// DFO_PATTRN.h
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_PATTRN_H
#define DFO_PATTRN_H

#include <genClass/DO_TableData.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/SC_SetupErr.h>
#include <genClass/SC_Random.h>
#include <objClass/DO_XYDataArray.h>

#include <saClass/DO_SAPlotTable.h>

#include <saDFO/DFO_SAObjBase.h>

class DFO_PATTRN : public DFO_SAObjBase {

    public:
                //  all tests in enum
        enum    PattrnTests {ptCC, ptCCMC, ptCIQ, ptCIQMC,
                             ptCL, ptCLMC, ptCMD, ptCMDMC,
                             ptCMN, ptCMNMC, ptCV, ptCVMC,
                             ptDR,  ptDRMC, ptFR, ptFRMC,
                             ptPCC, ptPRCC, ptRCC, ptRCCMC,
                             ptSI, ptSIMC, ptSRC, ptSRRC,
                             ptLastTest};

        enum            {inpStrLen = 255};

        static const char* testStrs[ptLastTest];

    protected:
        //  parameters - correspond to PATTRN commands
        char            obsStr[inpStrLen];  // OBSERVATIONS
        int             nreplicates;        // NUMBER OF REPLICATES
        int             randSeed;           // SEED
        int             nxintInp;           // NXINT input
        int             nyintInp;           // NYINT input
        int             nrandomPatterns;    // NUMBER OF RANDOM PATTERNS
        bool            logarithms;         // LOGARITHMS
        double          alphaCutoff;        // ALPHA CUTOFF
        int             mtest1;             // selected test for sorting

                                            // output control
        bool            verbose;            // VERBOSE
        bool            verboseMC;          // VERBOSEMC
        bool            verboseCV;          // VERBOSECV
        bool            coefficients;       // COEFFICIENTS
        bool            rankCoefficients;   // RANK COEFFICIENTS
        char            mcRepStr[inpStrLen];// for verbose MC

        SC_BoolArray    tests; // allocd for PattrnTest

        SC_RealFormat   coeffFormat;

    private:
        int             niv;        // # of IV
        int             ndv;        // # of DV

        SC_IntArray     mobs;       // selected observations
        int             nobs;       // # of selected

        int             nrep;       // numberOfReplicates if test
        int             nxint;      // number of xintervals
        int             nyint;      // number of xintervals

        // common block CBXN
        SC_DoubleMatrix xiv;        // copied IV
        SC_DoubleMatrix xsiv;       // sorted IV
        SC_DoubleMatrix xriv;       // ranked IV
        SC_DoubleMatrix xivrp;      // x coord for random patterns
        SC_DoubleMatrix xrivrp;     // ranks of x coord for random patterns

        // common block CBYN
        SC_DoubleMatrix ydv;        // copied DV
        SC_DoubleMatrix ysdv;       // sorted DV
        SC_DoubleMatrix yrdv;       // ranked DV
        SC_DoubleMatrix ydvrp;      // y coord for random patterns
        SC_DoubleMatrix yrdvrp;     // ranks of y coord for random patterns

        // common block CBGRID
        SC_DoubleMatrix egrid;      // grid stuff
        SC_DoubleArray  xgrid;
        SC_DoubleArray  ygrid;
        SC_IntArray     nxgrd;
        SC_DoubleMatrix xgrdl;
        SC_DoubleMatrix ySIgrdl;
        SC_DoubleMatrix yMDgrdl;
        SC_DoubleMatrix yIQgrdl;

        // common block CBSTAT
        SC_DoubleArray  svgsc;
        SC_DoubleMatrix stat;
        SC_DoubleMatrix statpv;

        SC_Random       ran3;       // Press RNG

        SC_BoolArray    mcReport;

        //  for output routines
        SC_IntArray     selTests;
        int             ntests;

        SC_BoolArray    ivAlphaOK;
        SC_BoolArray    dvAlphaOK;

        // output data
        DO_SAPlotTable  fullTableDO;
        DC_TableData    fullTableDC;

        DO_SAPlotTable  xgridTableDO;
        DC_TableData    xgridTableDC;

        DO_SAPlotTable  ySIgridTableDO;
        DC_TableData    ySIgridTableDC;

        DO_SAPlotTable  yMDgridTableDO;
        DC_TableData    yMDgridTableDC;

        DO_SAPlotTable  yIQgridTableDO;
        DC_TableData    yIQgridTableDC;

        DO_TableData    dvTableDO;
        DC_TableData    dvTableDC;

        DO_TableData    ivTableDO;
        DC_TableData    ivTableDC;

        DO_XYDataArray  dvCDFDO;
        DC_XYDataArray  dvCDFDC;

        DO_XYDataArray  ivCDFDO;
        DC_XYDataArray  ivCDFDC;

    public:
                            DFO_PATTRN();
                            DFO_PATTRN(const DFO_PATTRN& a);
                            ~DFO_PATTRN(){};

        DFO_PATTRN&         operator= (const DFO_PATTRN& a);

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);

    private:
        void                InitCommon();                        // common to normal and copy constructor
        void                LocalCopy(const DFO_PATTRN& a);  // common to copy constructor and = operator

        void                InputListing();

        bool                ProcessObsStr(SC_SetupErr& err);
        bool                ProcessMCRepStr(SC_SetupErr& err);
        bool                ProcessNumStr(const char*           inStr,
                                                SC_BoolArray&   selIndx,
                                                int             maxIndx,
                                                SC_SetupErr&    err);

        void                CSR();
        void                DoCSR(const DC_SATable& saTbl,
                                        SC_DoubleMatrix& v,
                                        SC_DoubleMatrix& sv,
                                        SC_DoubleMatrix& rv,
                                        int&             nv);

        void                Savage();

        bool                MonteCarloTest();
        bool                MonteCarloTest(int tIndex);
        bool                PCCSRCTest(int tIndex);

        void                CalcMC(int ivIndx, int dvIndx);

        void                SetGrid(int ivIndx, int dvIndx);

        void                CalcDV(const SC_DoubleArray& xin, // sorted
                                         SC_IntArray&    mlev); // levels

        int                 Search(const double& val,
                                   const SC_DoubleArray& edf,
                                   const SC_DoubleArray& grid);

        double              CalcProb(      PattrnTests      pTest,
                                           int              ivIndx,
                                           int              dvIndx,
                                     const double&          statV,
                                           SC_DoubleArray&  ccdf);

        void                CalcCO(const SC_DoubleArray& statRes,
                                         SC_IntArray&    maco);

        double              CalcCC(const SC_DoubleArray& x,
                                   const SC_DoubleArray& y);


        double              SITest(const SC_DoubleArray& ivVar,
                                   const SC_DoubleArray& dvVar,
                                         int             ivIndx,
                                         int             dvIndx);
        void                TestSI(int ivIndx, int dvIndx);

        double              CLTest(const SC_DoubleArray& ivVar,
                                   const SC_DoubleArray& dvVar,
                                         int             ivIndx);
        void                TestCL(int ivIndx, int dvIndx);

        void                TestCRC(int ivIndx, int dvIndx);

        double              CMDTest(const SC_DoubleArray& ivVar,
                                   const SC_DoubleArray& dvVar,
                                         const double&   medVal,
                                         int             ivIndx,
                                         bool&           testOK);

        void                TestCMD(int ivIndx, int dvIndx);

        double              CMNTest(const SC_DoubleArray& ivVar,
                                   const SC_DoubleArray& dvVar,
                                         int             ivIndx,
                                         bool&           testOK);

        void                TestCMN(int ivIndx, int dvIndx);

        double              CIQTest(const SC_DoubleArray& ivVar,
                                   const SC_DoubleArray& dvVar,
                                         const double&   ydv25,
                                         const double&   ydv75,
                                         int             ivIndx,
                                         bool&           testOK);
        void                TestCIQ(int ivIndx, int dvIndx);

        double              CVTest(const SC_DoubleArray& ivVar,
                                    const SC_DoubleArray& dvVar,
                                         int             ivIndx,
                                         bool&           testOK,
                                         int             dvIndx = -1, // only used in verbose output
                                         bool            writeCV = false);

        void                TestCV(int ivIndx, int dvIndx);

        double              DRTest(const SC_DoubleArray& ivVar,
                                   const SC_DoubleArray& dvVar,
                                         int             ivIndx);
        void                TestDR(int ivIndx, int dvIndx);

        double              FRTest(const SC_DoubleArray& ivVar,
                                   const SC_DoubleArray& dvVar,
                                         int             ivIndx);
        void                TestFR(int ivIndx, int dvIndx);

        void                TestPCSR(int dvIndx);

        void                SetIVDVOK(int dvIndx);

        void                WriteResults(int dvIndx);

        void                ImportanceTable(const SC_IntArray& maco,
                                            const SC_DoubleMatrix& rio);
        void                ImportanceOrdering(const SC_IntArray& maco,
                                            const SC_DoubleMatrix& rio);

        void                CalcSS(const SC_DoubleArray& rio,
                                   const SC_DoubleArray& ssa);

        void                WriteDetailedSummary();

        void                WriteCoeffSummary(bool rankSummary);

        void                SetCDF(const SC_DoubleArray& varVal,
                                         int             repIndx,
                                         DC_XYData&      varCDF);

        void                AddTestError(const char* test, const char* errCond, int ivIndx);

};

#endif // !DFO_PATTRN_H

