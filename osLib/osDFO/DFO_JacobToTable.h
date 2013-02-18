///////////////////////////////////////////////////////////////////////////////////
//
// DFO_JacobToTable.h
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
//      extracts data from Jacobian in DC_TableData form.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_JACOBTOTABLE_H
#define DFO_JACOBTOTABLE_H

#include <genClass/T_SC_Array.h>
#include <genClass/DO_Real.h>
#include <genClass/DO_TableData.h>

#include <genClass/C_FuncObj.h>

class DFO_JacobToTable : public FuncObjC {
    protected:
        FuncObjRef          jacobObjRef;

        bool                parJacobian;   // if true table cols are par values
        int                 fitCompIndex;  // -1 full fit, otherwise component index
        int                 parCompIndex;  // -1 all pars, otherwise parameter component index
        bool                xareValues;    // true use values of X in table col 1, otherwise index
        bool                takeAbs;       // abs value of jac

    private:
        DO_TableData        jacobTableDO;
        int                 ntableCol;
        DC_TableData        jacobTable;

    public:
                        DFO_JacobToTable();
                        DFO_JacobToTable(const DFO_JacobToTable& a);
                        ~DFO_JacobToTable();

        DFO_JacobToTable& operator= (const DFO_JacobToTable& a);

                        //  FuncObjC virtuals
        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

    private:
        void            InitCommon();                             // common to normal and copy constructor
        void            LocalCopy(const DFO_JacobToTable& a);  // common to copy constructor and = operator
};

#endif // !DFO_JACOBTOTABLE_H

