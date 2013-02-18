///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableAddBase.h
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

#ifndef DFO_TABLEADDBASE_H
#define DFO_TABLEADDBASE_H

#include <genClass/T_SC_BasicPtrArray.h>
#include <genClass/DO_Real.h>
#include <genClass/DO_TableData.h>

#include <genClass/C_FuncObj.h>

class DFO_TableAddBase : public FuncObjC {
    public:
        enum {maxColumnID = 20};

        class TableInputData  {
            public:
                bool        doTable;
                FuncObjRef  tableObjRef;

                            TableInputData();
                            TableInputData(const TableInputData& a);
                TableInputData&  operator= (const TableInputData& a);

            private:
                void         LocalCopy(const TableInputData& a);  // common to copy constructor and = operator

        };

        enum        {maxInputTable = 24};


        TableInputData      tablesToAdd[maxInputTable];

        bool                skipXColumn;
        int                 xcolumnIndex;

    private:
        DC_TableData&           outputTableDC;
        DO_TableData&           outputTableDO;
        const std::type_info&   tableType;  // must be derived from DO_TableData

    protected:
        T_SC_BasicPtrArray<const DC_TableData>      inputTableDC;
        int                 ncols;
        int                 nrows;

    public:
                        DFO_TableAddBase(const char* objID, const std::type_info& tblType,
                                        DO_TableData& outTblDO, DC_TableData& outTblDC);
                        DFO_TableAddBase(const DFO_TableAddBase& a, DO_TableData& outTblDO, DC_TableData& outTblDC);
                        ~DFO_TableAddBase();

        DFO_TableAddBase&   operator= (const DFO_TableAddBase& a);

                        //  FuncObjC virtuals
        void            DoStatusChk();

                        // output table must be allocd and initialized to 0.0 before call
        void            CalcOutput(FOcalcType  calcType);

    private:
        void            InitCommon();                             // common to normal and copy constructor
            void            LocalCopy(const DFO_TableAddBase& a);  // common to copy constructor and = operator
    };

#endif //DFO_TABLEADDBASE_H

