///////////////////////////////////////////////////////////////////////////////////
//
// DFO_Derivative.h
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
//      Calculates derivatives of input data.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_DERIVATIVE_H
#define DFO_DERIVATIVE_H

#include <genClass/C_FuncObj.h>

#include <nsClass/DO_Derivative.h>
#include <genClass/DO_Real.h>
#include <genClass/DO_XYData.h>
#include <genDataClass/DSC_RealMSSpecBase.h>

class DFO_Derivative : public FuncObjC {
    public:
        FuncObjRef          inputTPObjRef;      // input TP data
        bool                isMaster;           // spec is master
        FuncObjRef          masterObjRef;       // master spec

        //  properties
        DC_Derivative       derivativeDC;
        FuncObjRef          timeProcessObjRef;
        DSC_RealMSSpecBase  windowPctSpanMS;
        DSC_RealMSSpecBase  windowValSpanMS;

    private:
        bool*               windowPctSpanCheckRef;
        bool*               windowValSpanCheckRef;
        bool*               useDerivativeCheckRef;
        DC_TimeProcess*     timeProcessDC;          // input time process
        DO_Derivative       derivativeDO;           // output derivative

        const DC_XYData*    tpDataDC;           // input TP

        DC_XYData           outputTPDC;         // ouput TP data
        DO_XYData           outputTPDO;         // outputTP DO



    public:
                        DFO_Derivative();
                        DFO_Derivative(const DFO_Derivative& a);
                        ~DFO_Derivative();

        DFO_Derivative& operator= (const DFO_Derivative& a);

        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);
        void            SetDefault();

    private:
        void            InitCommon();                               // common to normal and copy constructor
        void            LocalCopy(const DFO_Derivative& a);     // common to copy constructor and = operator

};

#endif //DFO_DERIVATIVE_H
