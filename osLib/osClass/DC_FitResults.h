///////////////////////////////////////////////////////////////////////////////////
//
// DC_FitResults.h
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
//      optimizer results (fit value, residuals) for a single fit.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_FITRESULTS_H
#define DC_FITRESULTS_H

#include <genClass/DC_XYData.h>
#include <genClass/SC_DoubleArray.h>
#include <osClass/C_OptVar.h>


class DC_FitResults : public OptimizationControlStatics  {
    public:
        enum                    {fitIDLen = 25};
        char                    fitID[fitIDLen];  // set to single fit object ID

        // allocation data
        bool                    okToAlloc;
        int                     nfitAlloc;   //Set in fit to be maximal number of residuals

        // output for all fit types
        double                  fitValue;
        SC_DoubleArray          fitResults;        // simulation results at best-fit point
        DC_XYData               fitResiduals;      // x is fieldX,  y[i] is fieldY[i] -  fitResults[i]

        // set by DFO_SingleFit in status checks
        double                  sigma;   /// std dev measurement error entered for chisq fits


        //  fit results
        int                     nfitPoints;
        bool                    fitWasL1;
        bool                    fitWasNormed;

        //  used for chisq hessian calcs -- set by fit calculation proc DFO_SingleFit
        SC_DoubleArray          fitDerivatives;    // simulation results at derivative points
//            SC_DoubleMatrix         fitDerivDiagonal;  // second diagonal term for full derivative calcs (optHessianI < 0)
        double                  sse;               // sum of squared errors

        // used for MINPACK L-M derivative of residuals - set in SingleFitBase
        SC_DoubleArray          fitResidualDerivatives;  //residual at parameter offset by derivative span

        //  set and used by optimizer
//            SC_DoubleMatrix         fitHessian;        // hessian matrix for fit
        double                  estVar;

    public:
                                DC_FitResults();
                                DC_FitResults(const DC_FitResults& a);
        virtual                 ~DC_FitResults();

        bool                    PreRunAlloc(bool allocDeriv);  // does pre-run allocation of all vars


        DC_FitResults&          operator= (const DC_FitResults& a); // {return *this;}
        void                    LocalCopy(const DC_FitResults& a);


};

typedef T_SC_Array<DC_FitResults*> FitResultsPtrArray;

typedef T_SC_Array<DC_FitResults> FitResultsArray;

typedef T_SC_Array<FitResultsArray> FitResultsMatrix;


#endif // !DC_FITRESULTS_H

