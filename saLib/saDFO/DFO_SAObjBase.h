///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SAObjBase.h
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

#ifndef DFO_SAOBJBASE_H
#define DFO_SAOBJBASE_H

#include <genClass/C_FuncObj.h>
#include <genClass/DO_Listing.h>
#include <genListClass/LSC_Listing.h>

#include <saClass/DC_SATable.h>

class DFO_SAObjBase : public FuncObjC, public  LSC_Listing {
    public:
        FuncObjRef      indepVarObjRef;
        FuncObjRef      depVarObjRef;

        enum            {titleLen = 80};
        char            title[titleLen];

    private:
        DO_Listing      progListingDO;

    public:
                            DFO_SAObjBase(const char* inID);
                            DFO_SAObjBase(const DFO_SAObjBase& a);
                            ~DFO_SAObjBase();

        DFO_SAObjBase&      operator= (const DFO_SAObjBase& a);
        void                DoStatusChk();

    protected:
        void            FullVariableList();
        void            VariablesNameList(int skipIDVar = -1);


        const DC_SATable&   GetDepVar();
        const DC_SATable&   GetIndepVar();

        static void     RankArray(SC_DoubleArray& x);
                        // returns ranks of x
                        // based on PCCSRC routine RANKER

        static void     SetRanks(const SC_DoubleArray& x,
                                       SC_DoubleArray& ranks);

        static void     StandardizeArray(SC_DoubleArray& x);
                        // standardizes array - mean 0; variance 1
                        // based on STEPWISE routine STD01

        static double   CorrelateXY(const SC_DoubleArray& x,
                                    const SC_DoubleArray& y,
                                          bool            ranked);
                        // based on PCCSRC routine CORRXY


        int             InvertSymmetricMatrix(      SC_DoubleMatrix& a,
                                              const SC_StringArray& rcIDs,
                                                    bool            listErrors);

                        // returns 0 if invert OK
                        // < 0 if warning
                        // > 0 if error

    protected:
        enum            SymFormat {sfAll, sfLTri, sfUTri};


        void            ListSymmetricMatrix(const SC_DoubleMatrix& a,
                                            const SC_StringArray& rcIDs,
                                            const SC_RealFormat&  rForm,
                                                  int             minColWidth,
                                                  SymFormat       symFormat,
                                                  int             maxCol,
                                            const char*           colType);


    private:
        void            InitCommon();
        void            LocalCopy(const DFO_SAObjBase& a);     // common to copy constructor and = operator

        void            ListOneVar(const DC_SATable&    var);

};

#endif // !DFO_SAOBJBASE_H

