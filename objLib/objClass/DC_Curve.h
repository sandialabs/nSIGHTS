///////////////////////////////////////////////////////////////////////////////////
//
// DC_Curve.h
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
//      Cubic spline or polynomial fit to time series.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_CURVE_H
#define DC_CURVE_H

#include <objClass/C_Curve.h>
#include <genClass/SC_SetupErr.h>
#include <genClass/DC_XYData.h>
#include <objClass/DC_XYDataArray.h>

// data class for functional approximation types


class DC_Curve : public CurveDescription {

    friend class DC_Derivative;
    friend class DC_CurveArrayIOText;

    protected:

        static const double     maxExp;  // for spline calcs

        bool                    curveIsCreated;
        DC_XYData               curveData;          // used by all except poly
        SC_DoubleArray          splineDeriv;        // by spline
        SC_DoubleArray          polyCoefficients;   // by polynomial

        double                  normalizedTension;  // calc by spline routine

    public:
        //  limits of creation data for auto interpolation limits
        double                  inputXmin;
        double                  inputXmax;

    public:
                                DC_Curve();
                                DC_Curve(const DC_Curve& a);
                                DC_Curve(const CurveDescription& a);  // for init from base
        virtual                 ~DC_Curve();

        DC_Curve&    operator= (const DC_Curve& a);
        DC_Curve&    operator= (const CurveDescription& a);  // for copying from base

        bool                    CreateCurve(const DC_XYData&    curveInput,
                                                  SC_SetupErr& errData);      // returns true if basic setup is OK

        bool                    CreateCurve(const SC_DoubleArray&   curveXInput,
                                            const SC_DoubleArray&   curveYInput,
                                                  SC_SetupErr&      errData);      // returns true if basic setup is OK

        bool                    IsCreated() {return curveIsCreated;}

        //  various sigs to get data
        double                  GetCurveY(const double& inputX);

        void                    GetCurveY(DC_XYData& inputXoutputY);

        void                    GetCurveY(const DC_XYData& inputXY,   // only X used
                                                DC_XYData& outputXY);

        void                    GetCurveY(const SC_DoubleArray& inputX,
                                                DC_XYData&      outputXY);

        void                    GetCurveY(const SC_DoubleArray& inputX,
                                                SC_DoubleArray& outputY);

        const SC_DoubleArray&   GetPolyCoefficients() {return polyCoefficients;}

    private:
        void                    LocalCopy(const DC_Curve& a);  // implementation of copy

    private:
        bool                    CreateCurve(SC_SetupErr&    errData);      // returns true if basic setup is OK


        // these are ported from GTFM pascal routines and use 1 based arrays
        void    PolyLeastSquares(const SC_DoubleArray& in_x_data,     // input X and Y -- 0 based
                                       SC_DoubleArray& in_y_data,
                                       int              n_poly_coeff,  //  polyorder + 1
                                       SC_DoubleArray&  coeff_data);   // output coeff 1 based

        void    MatrixSolve(SC_DoubleMatrix&    a,          // 1 based
                            SC_DoubleArray&     rhs,        // 1 based
                            int                 n_eqn);     //

        void    LuDecompose(SC_DoubleMatrix&    a,
                            SC_IntArray&        indx,
                            int                 n);

        void    LuBacksubs(const SC_DoubleMatrix&   a,
                           const SC_IntArray&       indx,
                                 SC_DoubleArray&    b,
                                 int                n);

        bool    CheckExp(const double& argVal, double& exp_argVal );

        bool    GenCubicSpline();

        // calculation
        double  CalcSpline(const double& xVal);
        double  CalcPoly(const double& xVal);
        double  CalcLinear(const double& xVal);
        double  CalcStepMid(const double& xVal);
        double  CalcStepFull(const double& xVal);

        // for linear & step
        int     nextPoint, lastPoint;  // used by GenSpline, set by GetSpan

            bool    GetSpanPoints(const double& xVal,
                                  double&   xPrev,
                                  double&   xNext,
                                  double&   yPrev,
                                  double&   yNext,
                                  double&   yRet);
};


#endif // DC_CURVE_H

