///////////////////////////////////////////////////////////////////////////////////
//
// DFO_STEPWISE.h
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

#ifndef DFO_STEPWISE_H
#define DFO_STEPWISE_H

#include <genClass/DO_TableData.h>
#include <genClass/SC_RealFormat.h>

#include <saClass/DO_SAPlotTable.h>

#include <saDFO/DFO_SAObjBase.h>

class DFO_STEPWISE : public DFO_SAObjBase {
    private:
        DO_SAPlotTable  allRawVarDO;
        DC_TableData    allRawVarDC;
        DO_SAPlotTable  allProcessedVarDO;
        DC_TableData    allProcessedVarDC;

        DO_SAPlotTable  selRawVarDO;
        DC_TableData    selRawVarDC;
        DO_SAPlotTable  selProcessedVarDO;
        DC_TableData    selProcessedVarDC;

        DO_TableData    pressTableDO;
        DC_TableData    pressTableDC;

        DO_TableData    residTableDO;
        DC_TableData    residTableDC;

        DO_TableData    corrTableDO;
        DC_TableData    corrTableDC;

    protected:
        //  parameters - correspond to STEPWISE commands
        bool            optBACKWARD;
        bool            optRANK;
        bool            optPRESS;
        bool            optSTAND01;
        bool            optSTEPWISE;

        double          optSigin;  // SIGIN for BACKWARD & STEPWISE
        double          optSigout; // SIGOUT for STEPWISE

        bool            optFORCE;
        SC_IntArray     forceIndexes;
        bool            optWEIGHT;
        int             weightIndex;
        bool            optDROP;
        SC_IntArray     dropIndexes;


        // listing control - OUTPUT options
        bool            outMEAN;
        bool            outCORR;
        bool            outSSCP;
        bool            outINVERSE;
        bool            outSTEPS;
        bool            outRESIDUALS;
        bool            outOTHER;

        SC_DecFormat    decForm;
        SC_SciFormat    sciForm;

    private:
        int             niv;
        int             nvar;  // niv + nv
        int             nobs;     // nvector
        SC_DoubleArray  weights;

        // calculated in CalcCor, used in regranal
        SC_DoubleArray  varMean;    // means
        SC_DoubleArray  varStdDev;  // std dev
        SC_DoubleArray  varDiag;    // cross product
        SC_DoubleMatrix varCorr;    // correlations

        // step wise regression temporary
        static const double bigReal;
        SC_DoubleMatrix stepA;
        SC_IntArray     ivarOK;
        SC_DoubleArray  stepF;
        int             nvarOK;
        int             maxOK;
        int             nPhi;
        bool            doForce;

        // all regression temporary
        SC_DoubleArray  varMeanR;   // means
        SC_DoubleArray  varStdDevR; // std dev
        SC_DoubleArray  varDiagR;   // cross product
        SC_DoubleMatrix varCorrR;   // correlations
        SC_DoubleArray  varCorrDR;  // correlations with dependent var
        SC_DoubleMatrix varInvCorrDR;   // inverted corr matrix


    public:
                            DFO_STEPWISE();
                            DFO_STEPWISE(const DFO_STEPWISE& a);
                            ~DFO_STEPWISE(){};

        DFO_STEPWISE&       operator= (const DFO_STEPWISE& a);

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);

    private:
        void                InitCommon();                        // common to normal and copy constructor
        void                LocalCopy(const DFO_STEPWISE& a);  // common to copy constructor and = operator

        void                MakeInputTable();

        void                InputListing();

        void                CalcCorrelations();

        void                RegressVar(const int dvIndx,
                                       const int dvNum,
                                       SC_IntArray& selIvar);

        void                StepInit(const int idv);

        void                DelVar(SC_IntArray& ivar, const int idv);


        void                StepStep(int&           nvarIn,
                                     int&           nvarOut,
                                     SC_IntArray&   ivar,
                                     bool&          atEnd);

        void                StepForce();

        void                StepAdd(int iadd);

        void                StepDel();

        bool                StepPick();

        bool                CalcConfLimits(int            nivR,
                                           const SC_IntArray&     ivar,
                                           SC_DoubleArray& pstp);

        double              CalcPress(int nivR,
                                       const SC_IntArray&    ivar,
                                       const SC_DoubleArray& resi);

        double              Fsub(double f, int j, int k);
        double              Algama(double a);
        double              DcdfT(double x, double v); // CDF of T dist
        double              Tinv(double r, int k, int n);



};

#endif // !DFO_STEPWISE_H

