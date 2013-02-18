///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableToXY.h
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
//      Converts two columns in a table to an XY (time series) data type
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_TABLETOXY_H
#define DFO_TABLETOXY_H

#include <genClass/C_FuncObj.h>

#include <genClass/DO_TableData.h>
#include <genClass/DC_XYData.h>
#include <genClass/DO_XYData.h>

#include <objDataClass/DSC_TableColSel.h>

class DFO_TableToXY : public FuncObjC, public DSC_TableColSel  {

    protected:
        int                         selectedYCol;  // x is from DSC_TableColSel

        enum                        {outputDescLen = 40};
        char                        outputDesc[outputDescLen];

    protected:
        //  output data
        DC_XYData                   outputXY;
    private:
        //  output object
        DO_XYData                   outputXYDO;


    public:
                        DFO_TableToXY();
                        DFO_TableToXY(const DFO_TableToXY& a);
                        ~DFO_TableToXY();

        DFO_TableToXY& operator= (const DFO_TableToXY& a);

        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

    private:
        void            InitCommon();                        // common to normal and copy constructor
        void            LocalCopy(const DFO_TableToXY& a);  // common to copy constructor and = operator
};

#endif //DFO_TABLETOXY_H

