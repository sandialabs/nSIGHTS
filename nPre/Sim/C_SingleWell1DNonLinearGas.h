///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell1DNonLinearGas.h
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

#ifndef C_SINGLEWELL1DNONLINEARGAS_H
#define C_SINGLEWELL1DNONLINEARGAS_H

#include <Sim/C_SingleWell1DNonLinear.h>

class SingleWell1DNonLinearGas : public SingleWell1DNonLinear  {

    private:

        // constants
        static const double tKelvin;
        static const double univGasConstant;

        // boundary
        double              atmosPressure;
        double              gasConstant;
        SC_DoubleArray      gasOutputPressures;

    public:
                            SingleWell1DNonLinearGas();
        virtual             ~SingleWell1DNonLinearGas();


    private:
        // SingleWell virtuals
        virtual void        DoAllocateAll();
        virtual void        SetRuntimeSize();
        virtual void        SetRunParameters();

        virtual void        UpdateTSOutput();           // override for gas
        virtual double      GetWstoreCompressibility(); // override by gas
        virtual void        MatSolve(bool& setupRequired); 

        // FB182 kluge
        virtual double      GetLastWellPressure();


        // for f(R) table calculation
        virtual double      GetFrCondVal(int nodeNum);
        virtual double      GetFrStoreVal(int nodeNum);

        // new virtuals for nonlinear
        virtual double      NLGetDTerm(      int        nodeNum,
                                       const double&    pN,
                                       const double&    pN1);

        virtual double      NLGetSTerm(      int        nodeNum,
                                       const double&    pN);


        virtual double      NLCalcDTerm(      int       nodeNum,
                                        const double&   pN,
                                        const double&   pN1);

        virtual double      NLGetWellPressure();
        virtual double      NLGetExternalPressure();

        virtual double      AnalyticPartialD(     int       nodeNum,
                                             const double&  pN);

};



#endif // C_SINGLEWELL1DNONLINEARGAS_H
