///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CompositeFit.h
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
//      combines the results from multiple single fits (and/or other composite fits).
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_COMPOSITEFIT_H
#define DFO_COMPOSITEFIT_H

#include <genClass/DC_XYData.h>
#include <genClass/DO_XYData.h>
#include <osDFO/DFO_FitBase.h>

class DFO_CompositeFit : public DFO_FitBase {
    public:

        class FitData {
            public:
                bool            useFit;
                FuncObjRef      inputFitObjRef;
                double          fitScaling;  // non Chisq only
                bool            logFit;      // non Chisq only
            public:
                                FitData();
                                FitData(const FitData& a);
                FitData&        operator= (const FitData& a);

            private:
                void            LocalCopy(const FitData& a);  // common to copy constructor and = operator

        };
        enum        {maxFitData = 12};


        FitData         fitData[maxFitData];

    private:
        bool            recursiveAlloc;

    public:
                        DFO_CompositeFit();
                        DFO_CompositeFit(const DFO_CompositeFit& a);
                        ~DFO_CompositeFit();

        DFO_CompositeFit& operator= (const DFO_CompositeFit& a);

        // DFO_FitBase virtuals
        virtual void    DoAllocCheck();
        virtual void    GetFitScaling(SC_DoubleArray& scaleData);  // default returns 1 entry of 1.0

        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

    private:
        void            InitCommon();                           // common to normal and copy constructor
        void            LocalCopy(const DFO_CompositeFit& a);     // common to copy constructor and = operator
};

#endif // !DFO_COMPOSITEFIT_H

