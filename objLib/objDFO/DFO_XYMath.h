///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYMath.h
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
//      Basic math (+-/*) on X or Y components of two input XY
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_XYMATH_H
#define DFO_XYMATH_H

#include <genDFO/DFO_MathBase.h>

#include <genClass/DO_XYData.h>


class DFO_XYMath : public DFO_MathBase {

    public:
        FuncObjRef                  dataInputAObjRef;  // model input data object
        FuncObjRef                  dataInputBObjRef;  // model input data object
        bool                        operateOnX;

    private:
        //  input associated data
        const DC_XYData*    inputDataA;                  // input scalar
        const DC_XYData*    inputDataB;                  // input scalar

        //  output object
        DO_XYData           outputDataDO;      // node output

        //  output data
        DC_XYData           outputData;


    public:
                        DFO_XYMath();
                        DFO_XYMath(const DFO_XYMath& a);
                        ~DFO_XYMath();

        DFO_XYMath& operator= (const DFO_XYMath& a);

        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

    private:
        void            InitCommon();                           // common to normal and copy constructor
        void            LocalCopy(const DFO_XYMath& a);     // common to copy constructor and = operator
};

#endif //DFO_XYMATH_H

