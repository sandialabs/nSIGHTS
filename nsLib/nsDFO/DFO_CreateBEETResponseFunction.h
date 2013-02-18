///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CreateBEETResponseFunction.h
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
//      Calculates a response function based on a baseline pressure data, barometric
//  pressure data and earth tide data.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_CREATEBEETRESPONSEFUNCTION_H
#define DFO_CREATEBEETRESPONSEFUNCTION_H

#include <genClass/C_FuncObj.h>

#include <genClass/DO_XYData.h>
#include <nsClass/DO_XYResponseFunction.h>
#include <genDataClass/DSC_RealMSSpecBase.h>

#include <objClass/DC_Curve.h>

class DFO_CreateBEETResponseFunction : public FuncObjC {
    public:
        FuncObjRef                  pressureInputObjRef;
        bool                        calcBE;
        FuncObjRef                  barometricInputObjRef;
        bool                        calcET;
        FuncObjRef                  earthTideInputObjRef;


        // regression parameter
        DSC_RealMSSpecBase          maxTimeDelayMS;

        //time spacing
        enum    TimeBase            {tbSpecific, tbAvgPressure, tbAvgBE, tbAvgET};
        TimeBase                    timeBase;
        DSC_RealMSSpecBase          specTimeSpacingMS;

        //options
        int                         convertType;

        //shown in status
        int                         timeIntervalLag;  //equivalent to memory (m)
        double                      timeSpacing;      //equivalent to delta t

    private:
        //  input associated data
        const DC_XYData*            inputPressureDC;
        const DC_XYData*            inputBarometricDC;
        const DC_XYData*            inputEarthTideDC;

        int                         npts;             //equivalent to number of elements (n)

        // response functions
        DO_XYResponseFunction       outputResponseFuncDO;
        DC_XYResponseFunction       outputResponseFuncDC;

    public:
                                    DFO_CreateBEETResponseFunction();
                                    DFO_CreateBEETResponseFunction(const DFO_CreateBEETResponseFunction& a);
                                    ~DFO_CreateBEETResponseFunction();

        DFO_CreateBEETResponseFunction&         operator= (const DFO_CreateBEETResponseFunction& a);

        void                        DoStatusChk();
        void                        CalcOutput(FOcalcType  calcType);

    private:
        void                        InitCommon();                               // common to normal and copy constructor
        void                        LocalCopy(const DFO_CreateBEETResponseFunction& a);     // common to copy constructor and = operator
        void                        CalcAvgTimeSpacing(const DC_XYData* inputData);
        //void                      CalcEfficiency();
        //void                      CalcRegress();
        //void                      DoRegression(const DC_XYData& inputPressure,
        //                                             DC_XYData& outputPressure);



};
#endif //DFO_CREATEBEETRESPONSEFUNCTION_H

