///////////////////////////////////////////////////////////////////////////////////
//
// DFO_PCCSRC.h
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

#ifndef DFO_PCCSRC_H
#define DFO_PCCSRC_H

#include <genClass/DO_TableData.h>
#include <genClass/DO_Label.h>

#include <saDFO/DFO_SAObjBase.h>

class DFO_PCCSRC : public DFO_SAObjBase {
    private:
        DO_Label        depVarLabelDO;

        DO_TableData    rawDepVarDO;
        DC_TableData    rawDepVarDC;

        DO_TableData    rsqDO;
        DC_TableData    rsqDC;

        DO_TableData    pccDO;
        DC_TableData    pccDC;

        DO_TableData    srcDO;
        DC_TableData    srcDC;

        int             ndv, niv, nstep;

    protected:
        //  parameters - correspond to PCCSRC commands
        bool            doPCC;
        bool            doRANK;
        bool            doSRC;
        double          listCutoff; // same as TABLE_CUTOFF - affects output listing
        double          plotCutoff; // same as TABLE_CUTOFF - affects output tables
        int             topIND;     // affects output tables
        SC_RealFormat   coeffFormat;
        bool            verboseOutput;



    public:
                            DFO_PCCSRC();
                            DFO_PCCSRC(const DFO_PCCSRC& a);
                            ~DFO_PCCSRC(){};

        DFO_PCCSRC&         operator= (const DFO_PCCSRC& a);

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);

    private:
        void                InitCommon();                        // common to normal and copy constructor
        void                LocalCopy(const DFO_PCCSRC& a);  // common to copy constructor and = operator

        void                MakeRawTable();

        void                SetupOutputTable(DC_TableData& tbl,
                                            const char* tblID);

        void                SetupRSquaredTable();

        void                AddTableSteps(DC_TableData& tbl);

        void                InputListing();

        void                ListOutputTable(const char* title,
                                                  int   dvarIndex,
                                         const SC_DoubleMatrix& dvData,
                                         const SC_DoubleArray& rsqr);

        void                PlotOutputTable(const SC_DoubleMatrix& dvData,
                                                 DC_TableData& outTbl,
                                                  int   dvarIndex);

        void                BuildIVCorrMatrix(SC_DoubleMatrix& ivCorr,
                                              const SC_DoubleMatrix& ivTbl);

        bool                CheckCutoff(const SC_DoubleMatrix& pcc,
                                        const SC_DoubleMatrix& src,
                                        const double&          cutVal,
                                              double&          maxVal);

};

#endif // !DFO_PCCSRC_H

