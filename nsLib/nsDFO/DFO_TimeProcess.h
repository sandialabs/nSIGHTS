///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TimeProcess.h
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

#ifndef DFO_TIMEPROCESS_H
#define DFO_TIMEPROCESS_H

#include <genClass/C_FuncObj.h>

#include <nsClass/DO_TimeProcess.h>
#include <genClass/DO_XYData.h>
#include <genDataClass/DSC_RealMSSpecBase.h>

class DFO_TimeProcess : public FuncObjC {
    public:     // for access in nPre auto setup
        //  properties
        FuncObjRef          inputTPObjRef;      // input TP data
        bool                isMaster;           // spec is master
        FuncObjRef          masterObjRef;       // master spec

        DC_TimeProcess      timeProcessDC;      // spec data
        FuncObjRef          inputTQObjRef;      // input TQ for superposition
        DSC_RealMSSpecBase  constantTMS;        // MS for horner or argawal T


    private:
        bool*               constantTCheckRef;
        bool*               inputTQCheckRef;
        const DC_XYData*    tpDataDC;           // input TP
        const DC_XYData*    tqDataDC;           // input TQ for super

        DO_TimeProcess      timeProcessDO;      // output time process spec

        DC_XYData           outputTPDC;         // ouput TP
        DO_XYData           outputTPDO;         // output TP DO
        DO_XYData           outputTQDO;         // output TQ DO

    public:
                        DFO_TimeProcess();
                        DFO_TimeProcess(const DFO_TimeProcess& a);
                        ~DFO_TimeProcess();

        DFO_TimeProcess& operator= (const DFO_TimeProcess& a);

        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);
        void            SetDefault();

    private:
        void            InitCommon();                               // common to normal and copy constructor
        void            LocalCopy(const DFO_TimeProcess& a);     // common to copy constructor and = operator

};

#endif // DFO_TIMEPROCESS_H

