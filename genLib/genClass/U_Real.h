///////////////////////////////////////////////////////////////////////////////////
//
// U_Real.h
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
// DESCRIPTION: real number utilities (matrix solvers, statistical calculations, etc)
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef U_REAL_H
#define U_REAL_H

#include <cmath>
#include <cstdlib>
#include <genClass/SC_DoubleArray.h>

namespace genRealSupport {

    static const double    e__          = 2.718281828459045235360287471352662497757247093;
    static const double    pi           = 3.141592653589793238462643383279502884197;
    static const double    stdEps       = 2.2204460492503131E-16;     //  standard epsilon value


    // were in C_Common

    extern double Radians(const double& degrees);
    extern double Degrees(const double& radians);
    extern bool   RealsAreSame(const double& rVal1, const double& rVal2, const double eps = stdEps);

    // spacing between double-precision representation of numbers at x
    // "epsilon" is basically spacing(0.0), but is inappropriate x>>0
    extern double spacing(double x);

    // these are translated wholesale from the pascal GENLIB routines in zrealfun
    inline double   Power(double x,  double y)  {return pow(x, y);}         // x^y
    inline double   Lgt(double x)               {return log10(x);}
    inline double   InvLgt(double x)            {return pow(10.0, x);}
    inline double   Sqr(double x)               {return x * x;}
    inline void     Swap(double& x,  double& y) {double t = x; x = y; y = t;}
    inline void     ISwap(int& x,  int& y)      {int t = x; x = y; y = t;}
    inline bool     IsPositive(const double& x) {return x >= stdEps;}


    extern bool     RoundOff (double& data,             // kluge - converts to str with dec
                    int     precision);       // notation, converts back to real
    extern double   Pround(double data, int n_dec);     // power of 10 round - converts to str
    // with sci notation, converts back to real

    // these routines were originally created in pascal to provide REAL8
    // equivalents of HP-Basic MOD, DIV, and SGN operators
    extern double   RealMod (double lho, double rho);
    extern double   RealDiv (double lho, double rho);

    //    extern double     RealInt (double data);   don't know why anyone would want this -- commented out
    //                                                   unitil I have a use

    extern double   RealSgn (double data);

    //added for paCalc UZ model
    inline double   Abs(double value)   {return fabs(value);}
    extern double   Max(double value1, double value2);
    extern double   Min(double value1, double value2);

    // better alternatives - no confusion with C lib
    inline double   Dabs(const double& value)   {return fabs(value);}
    extern double   DMax(const double& value1, const double& value2);
    extern double   DMax(const double& value1, const double& value2, const double& value3);
    extern double   DMax(const double& value1, const double& value2, const double& value3, const double& value4);
    extern double   DMin(const double& value1, const double& value2);
    extern double   DMin(const double& value1, const double& value2, const double& value3);
    extern double   DMin(const double& value1, const double& value2, const double& value3, const double& value4);
    extern void     DMinMax(const double& inVal, double& currMin, double& currMax);

    // not real routines; however makes sensto put it here
    inline int      Iabs(int value)     {return abs(value);}
    extern int      IMax(int value1, int value2);
    extern int      IMax(int value1, int value2, int value3);
    extern int      IMax(int value1, int value2, int value3, int value4);
    extern int      IMin(int value1, int value2);
    extern int      IMin(int value1, int value2, int value3);
    extern int      IMin(int value1, int value2, int value3, int value4);
    extern void     IMinMax(int inVal, int & currMin, int & currMax);

    // ---***************************** real number status testing
    extern  double  FixInfinite(double inVal);   // returns DBL_MAX  or -DBL_MAX
                                                 // if +ve or -ve infinity
    extern  double  FixIndefinite(double inVal); // returns 0.0 if indefinite or NAN
    extern  bool    IsFinite(double inVal);         // retrns true if inVal is a valid FP

    // --- **************************** misc extern doubles

    // Gamma and incomplete gamma-related special functions
    extern double   GammaLn(double x);
    extern double   Gamma(double x);
    extern double   GammaQ(double a, double x);
    extern double   GammaP(double a, double x);

    // error functions (related to incomplete gamma functions)
    extern double   Erf(double x);
    extern double   Erfc(double x);

    // inverse error function
    extern double   Ierfc(double y);
    // inverse normal distribution
    extern double   NormInv(double x);

    // incomplete beta function
    extern double   BetaI(double a, double b, double x);

    // --- **************************** misc double array ops

    extern void     MedianSmooth(SC_DoubleArray& data, int kernelSize);

    // ************************************* matrix routines

    // error class
    class MathError  {
    public:
        enum            {maxLen = 128};
        char            errStr[maxLen];
    public:
        MathError(const char* inStr);

    private:
        MathError() {};
    };

    // kiss_fft added to replace numerical recipe-based routines
    // RealFFT is now a wrapper for kiss_fft
    extern void RealFFT(SC_DoubleArray& data, bool forwardFFT);



    // correlation measures between two vectors
    extern double PearsonR(const SC_DoubleArray& x,
                           const SC_DoubleArray& y);

    extern double SpearmanR(const SC_DoubleArray& x,
                            const SC_DoubleArray& y);

    // exponential integral Ei(x)
    extern double ExpIntegral(double x);
    // wellfunction is exponential integral E1(x)
    extern double WellFunction(double x);


};

using namespace genRealSupport;


#endif // U_REAL_H

