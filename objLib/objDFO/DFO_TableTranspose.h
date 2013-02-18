///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableTranspose.h
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
//      Switches a table's columns and rows
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_TABLETRANSPOSE_H
#define DFO_TABLETRANSPOSE_H

#include <genClass/C_FuncObj.h>

#include <genClass/DO_TableData.h>

#include <objDataClass/DSC_TableColSel.h>

class DFO_TableTranspose : public FuncObjC {
    protected:
        FuncObjRef          inputTableObjRef;
        bool                skipXColumn;
        int                 xcolumnIndex;

    private:
        const DC_TableData* inputTableDC;

        //  output data
        DC_TableData        outputTableDC;
        DO_TableData        outputTableDO;
        int                 ninputRows;
        int                 ninputCols;

    public:
                        DFO_TableTranspose();
                        DFO_TableTranspose(const DFO_TableTranspose& a);
                        ~DFO_TableTranspose();

        DFO_TableTranspose& operator= (const DFO_TableTranspose& a);

        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

    private:
        void            InitCommon();                        // common to normal and copy constructor
        void            LocalCopy(const DFO_TableTranspose& a);  // common to copy constructor and = operator
        void            SetDesc(SC_StringArray& descStr, 
                                const char*     desc,
                                int             nreq);

};

#endif //DFO_TABLETRANSPOSE_H

