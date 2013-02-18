///////////////////////////////////////////////////////////////////////////////////
//
// DC_Derivative.h
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
//      Derived from C_Derivative and adds derivative calculation
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_DERIVATIVE_H
#define DC_DERIVATIVE_H

#include <nsClass/C_Derivative.h>
#include <genClass/SC_SetupErr.h>
#include <genClass/DC_XYData.h>
#include <nsClass/DC_TimeProcess.h>

class DC_Derivative : public DerivativeDescription {
    protected:
        bool                    derivativeIsCreated;

    private:
        SC_SetupErr             processErr;
        SC_DoubleArray          actualT;
        SC_DoubleArray          savedT;
        SC_DoubleArray          windowT;
        SC_DoubleArray          processedT;
        SC_DoubleArray          processedY;
        DC_XYData               windowWork;

    public:
                                DC_Derivative();
                                DC_Derivative(const DC_Derivative& a);
                                DC_Derivative(const DerivativeDescription& a);  // for init from base
        virtual                 ~DC_Derivative();
        DC_Derivative&          operator= (const DC_Derivative& a);
        DC_Derivative&          operator= (const DerivativeDescription& a);  // for copying from base

                                // returns true if basic setup is OK
                                // does nothing yet ..
        bool                    CreateDerivative(SC_SetupErr& errData);  // always true for now

        bool                    IsCreated() const {return derivativeIsCreated;}

        //  two sigs to get data depending on whether there is super or not
        bool                    GetDerivatives(const DC_XYData&      inputXY,
                                                     DC_XYData&      outputXY,
                                                     SC_SetupErr&    errData);
        bool                    GetDerivatives(const DC_XYData&      inputXY,
                                               const DC_TimeProcess& timeProcess,
                                                     DC_XYData&      outputXY,
                                                     SC_SetupErr&    errData);

    private:
        void                    LocalCopy(const DC_Derivative& a);  // implementation of copy

        bool                    PrepareInput(const DC_XYData&            inputXY,
                                             const DC_TimeProcess* const timeProcess);

        bool                    DoCalc(DC_XYData&  outputXY);

        void                    Cleanup();

        bool                    WindowOK(int        ptIndx,
                                         int        stIndx,
                                         int        endIndx);

        bool                    WindowCalc(int      ptIndx,
                                           int      stIndx,
                                           int      endIndx,
                                           double&  dVal);
        void                    LogLinWindowCalc(DC_XYData&      outputXY);
        void                    FixedWindowCalc(DC_XYData&      outputXY);
        void                    SimpleCalc(DC_XYData&      outputXY);
        void                    TwoSlopeCalc(DC_XYData&      outputXY);
};


#endif // DC_DERIVATIVE_H

