///////////////////////////////////////////////////////////////////////////////////
//
// U_Real.cpp
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

#include <cfloat>

#include <genClass/C_Common.h>

#include <genClass/SC_RealFormat.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/C_TxtFile.h>


#include <genClass/U_Real.h>

#include <genClass/kiss_fft.h>
#include <cmath>


namespace genRealSupport {

    bool RealsAreSame(const double& rVal1, const double& rVal2, const double){
        // eps is not used, since spacing() is more appropriate
        return fabs(rVal1 - rVal2) < spacing(rVal1);
    }

    double spacing(double x){
        static const double b=2.0; // radix or base
        static const int p=53; // bits of precision in double mantissa
        double m; // mantissa (not used)
        int e; // exponent
        m = frexp(x,&e); // break x into exponent and mantissa
        return pow(b,e-p);  // spacing between representible numbers as x
    }

    static const double degPerRad = 57.2957795130823;

    double Radians(const double& degrees)
    {
        return degrees / degPerRad;
    }

    double Degrees(const double& radians)
    {
        return radians * degPerRad;
    }


    bool RoundOff (double& data,            // kluge - converts to str with dec
                   int    precision)        // notation, converts back to real
    {
        SC_DecFormat conv(precision);
        char temp[80];
        conv.RealToString(data, temp, 80);
        double tempR;
        if (!IsValidReal(temp, tempR))
            GenAppInternalError("RoundOff");
        bool retVal = (data != tempR);
        data = tempR;
        return retVal;
    }


    // --- **************************** misc extern doubles

    double  Pround(double data, int n_dec)
    {
        SC_SciFormat conv(n_dec);
        char temp[80];
        conv.RealToString(data, temp, 80);
        double tempR;
        if (!IsValidReal(temp, tempR))
            GenAppInternalError("RoundOff");
        return tempR;
    }

    double RealMod(double lho, double rho)
    {
        return lho - floor(lho / rho) * rho;
    }

    double  RealDiv (double lho, double rho)
    {
        return floor(lho / rho);
    }

    /*
      double  RealInt (double data)
      {
      int rval;
      if (data < 0.0)
      rval = int(data);  // normal conversion
      else
      rval = floor(data);  // normal conversion
      }

      VAR
      big_int : INTEGER4;
      BEGIN
      IF data < 0.0 THEN
      big_int := ROUND(data)
      ELSE
      big_int := TRUNC(data);
      real_int := LONGREAL(big_int);
      END;
    */

    double  RealSgn (double data)
    {
        double temp = Pround(data, 8);
        if (fabs(temp) <stdEps)
            return 0.0;

        if (temp > 0.0)
            return 1.0;

        return -1.0;
    }

    double Max(double value1, double value2)
    {
        if (value1 > value2)
            return value1;

        return value2;
    }

    double Min(double value1, double value2)
    {
        if (value1 < value2)
            return value1;

        return value2;
    }

    double      DMax(const double& value1, const double& value2)
    {
        if (value1 > value2)
            return value1;
        return value2;
    }

    double      DMax(const double& value1, const double& value2, const double& value3)
    {
        return DMax(DMax(value1, value2), value3);
    }

    double      DMax(const double& value1, const double& value2, const double& value3, const double& value4)
    {
        return DMax(DMax(value1, value2), DMax(value3, value4));
    }


    double  DMin(const double& value1, const double& value2)
    {
        if (value1 < value2)
            return value1;
        return value2;
    }

    double      DMin(const double& value1, const double& value2, const double& value3)
    {
        return DMin(DMin(value1, value2), value3);
    }

    double      DMin(const double& value1, const double& value2, const double& value3, const double& value4)
    {
        return DMin(DMin(value1, value2), DMin(value3, value4));
    }

    void DMinMax(const double& inVal, double& currMin, double& currMax)
    {
        if (inVal < currMin)
            currMin = inVal;
        else if (inVal > currMax)
            currMax = inVal;
    }

    int  IMax(int value1, int value2)
    {
        if (value1 > value2)
            return value1;
        return value2;
    }

    int IMax(int value1, int value2, int value3)
    {
        return IMax(IMax(value1, value2), value3);
    }

    int IMax(int value1, int value2, int value3, int value4)
    {
        return IMax(IMax(value1, value2), IMax(value3, value4));
    }


    int IMin(int value1, int value2)
    {
        if (value1 < value2)
            return value1;
        return value2;
    }

    int IMin(int value1, int value2, int value3)
    {
        return IMin(IMin(value1, value2), value3);
    }

    int IMin(int value1, int value2, int value3, int value4)
    {
        return IMin(IMin(value1, value2), IMin(value3, value4));
    }

    void IMinMax(int inVal, int & currMin, int & currMax)
    {
        if (inVal < currMin)
            currMin = inVal;
        else if (inVal > currMax)
            currMax = inVal;
    }


    double  FixInfinite(double inVal)
    {
        // Linux TODO
#ifdef _WIN32
        int res = _fpclass(inVal);
        if (res == _FPCLASS_NINF)
            return -DBL_MAX;

        if (res == _FPCLASS_PINF)
            return DBL_MAX;
#else
        // no way of determining + or -
        if (isinf(inVal))
            return DBL_MAX;
#endif

        return inVal;
    }

    double  FixIndefinite(double inVal)
    {
#ifdef _WIN32
        int res = _fpclass(inVal);
        if (res == _FPCLASS_SNAN)
            return 0.0;

        if (res == _FPCLASS_QNAN)
            return 0.0;

#else
        if (isnan(inVal))
            return 0.0;
#endif

        return inVal;
    }

    bool IsFinite(double inVal)
    {
#ifdef _WIN32
        return _finite(inVal) != 0;
#else
        return finite(inVal) != 0;
#endif
    }

    double  GammaLn(double z){
        // base-e logarithm of gamma function, using algorithm of
        // C. Lanczos "A Precision Approximation of the Gamma Function", in
        // SIAM Journal, Series B (Numerical Analysis), 1964.
        // this is an implementation of the logarithm of eqn 32 from that paper.

        // The Lanczos paper is explored in detail in the 2004 PhD dissertation
        // from UBC by Glendon Ralsph Pugh "An Analysis of the Lanczos Gamma Approximation"

        // coefficients a[1] through a[6] computed using Pugh section 6.1 method in mpmath
        static const double a[] = {-51.224241022374773699, 11.33875581348897662,
                                   -0.74773268777238839287, 0.0087828774930612977484,
                                   -1.8990302637568840508e-6, 1.9463345377557879276e-9};
        static const double r = 5.0; // confusingly called "gamma" in Lanczos paper
        static const int k = 6;
        static const double lnsqrt2pi = 0.91893853320467274178;
        double factor;

        z -= 1.0; // formulas below are for log(z!), shift by one for log(gamma(z))

        // log of constant outside summation
        factor = (z + 0.5)*log(z + r + 0.5) - (z + r + 0.5) + lnsqrt2pi;

        double series = 41.624436916439068207; // a0/2 term
        for (int j=1; j<k; j++){

            double ffnum = z;
            for (int k=1; k<j; k++){
                ffnum *= z-k;  // falling factorial
            }

            double rfdenom = z+1;
            for (int k=1; k<j; k++){
                rfdenom *= z+1+k; // rising factorial
            }

            series += a[j-1]*ffnum/rfdenom;
        }
        return factor + log(series);
    }

    double  Gamma(double z){
        return exp(GammaLn(z));
    }

    double incompleteGammaSeries(double a, double x){
        // lower-case gamma(a,x) = integral from 0 to z of t^{a-1}e^{-t} dt
        // sometimes called the "lower" incomplete gamma
        // Abramowitz & Stegun "Handbook of Mathematical Functions" (1964) equation 6.5.29
        // "Digital Library of Mathematical Functions" website (2012) equation 8.7.1

        static const int maxterms = 200;
        if (x <= 0.0){
            if (x < 0)
                GenAppInternalError("ERROR: incompleteGammaSeries argument < 0.0");
            // gamma(a,0) = 0
            return 0.0;
        }

        double term = 1.0/a;
        double sum = term;
        for (int k=1; k<maxterms; k++){
            term *= x/(a+k);
            sum += term;

            // check if new term is insignificant wrt sum
            if (fabs(term) < spacing(fabs(sum))){
                return exp(-x)*pow(x,a)*sum;
            }
        }
        GenAppInternalError("incompleteGammaSeries did not converge");
        return 0.0;
    }

    double incompleteGammaContinuedFraction(double a, double x){
        // upper-case Gamma(a,x) = integral from z to infinity of t^{a-1}e^{-t} dt
        // sometimes called the "upper" incomplete gamma
        // Abramowitz & Stegun "Handbook of Mathematical Functions" (1964) equation 6.5.31
        // "Digital Library of Mathematical Functions" website (2012) equation 8.9.1

        // continued fraction evaluatted using modified form of Lentz's method

        static const int    maxterms = 200;
        static const double tiny = spacing(0.0);

        double f,C,D,aa,b,Delta;

        b = 1.0 + x - a;
        C = 1.0/tiny;
        D = 1.0/b;
        f = D;

        for(int k=1; k<maxterms; k++){
            aa = (a-k)*k;
            b += 2.0;
            D = b + aa*D;
            if (fabs(D)< tiny){
                D = tiny;
            }
            D = 1.0/D;
            C = b + aa/C;
            if (fabs(C) < tiny){
                C = tiny;
            }
            Delta = C*D;
            f *= Delta;
            if(fabs(1.0 - Delta) < spacing(1.0)){
                return exp(-x)*pow(x,a)*f;
            }
        }
        GenAppInternalError("incompleteGammaContinuedFraction did not converge");
        return 0.0;
    }

    double GammaQ(double a, double x){
        // normalized incomplete Gamma function.
        // Q(a,x) = 1 - P(a,x) = upper-case Gamma(a,x)/Gamma(a)
        // see "Digital Library of Mathematical Functions" website (2012) section 8.2

        if ((x < 0.0) && (a <= 0.0)){
            GenAppInternalError("GammaQ not valid for x<0 or a<=0");
        }

        if ( x < (a + 1.0)){
            // region where series converges best
            return 1.0 - incompleteGammaSeries(a, x)/Gamma(a);
        }else{
            // region where continued fraction converges best
            return incompleteGammaContinuedFraction(a, x)/Gamma(a);
        }
    }

    double GammaP(double a, double x){
        // regularized incompete Gamma function.
        //  P(a,x) = 1 - Q(a,x) = lower-case gamma(a,x)/Gamma(a)
        // see "Digital Library of Mathematical Functions" website (2012) section 8.2

        if ((x < 0.0) && (a <= 0.0)){
            GenAppInternalError("GammaP not valid for x<0 or a<=0");
        }

        if ( x < (a + 1.0)){
            // series region
            return incompleteGammaSeries(a, x)/Gamma(a);
        }else{
            // continued fraction region
            return 1.0 - incompleteGammaContinuedFraction(a, x)/Gamma(a);
        }
    }

    double  Erf(double x){
        // error function defined in terms of incomplete gamma function
        // "Digital Library of Mathematical Functions" website (2012) equation 7.11.1
        // remember Gamma(0.5) = sqrt(pi)

        if (x < 0.0){
            // odd symmetry about 0
            return -GammaP(0.5, pow(x,2));
        }else{
            return GammaP(0.5, pow(x,2));
        }
    }

    double Erfc(double x){
        // complementary error function defined in terms of incomplete gamma function
        // "Digital Library of Mathematical Functions" website (2012) equation 7.11.1
        // remember Gamma(0.5) = sqrt(pi)

        if (x < 0.0){
            return 1.0 + GammaP(0.5, pow(x,2));
        }else{
            return GammaQ(0.5, pow(x,2));
        }
    }

    double  Ierfc(double y)
    {
        // Approximate the inverse complementary error function at Y
        enum {ncons = 22};

        static const double a1[] = {
            9.18725611735013E-01,  0.0,                   1.68792878000327E-02,
            0.0,                   6.60337139058300E-04,  0.0,
            3.20203849839380E-05,  0.0,                   1.72060607522481E-06,
            0.0,                   9.81965971588191E-08,  0.0,
            5.83049613537653E-09,  0.0,                   3.56019351836136E-10,
            0.0,                   2.21968915783128E-11,  0.0,
            1.40639693109741E-12,  0.0,                   9.02597345404862E-14,
            0.0};

        static const double a2[] = {
            1.54701109458613E+00, -3.31460331083896E-01,  4.33001124090060E-02,
            -1.06564004165532E-02,  2.90613542304156E-03, -8.61872838022491E-04,
            2.67933751795053E-04, -8.60838893942933E-05,  2.83232058814598E-05,
            -9.48870819734494E-06,  3.22422655069385E-06, -1.10815778472076E-06,
            3.84464770797987E-07, -1.34439275565208E-07,  4.73255976052393E-08,
            -1.67556011100019E-08,  5.96199003969093E-09, -2.13070503291886E-09,
            7.64427040920545E-10, -2.75198005584737E-10,  9.93792246090789E-11,
            -3.59877382902119E-11};

        static const double a3[] = {
            1.10642888011036E+01,  4.34299147561447E+00, -2.33781774969295E-02,
            4.23345215362947E-03,  8.68757084192089E-06, -5.98261113270881E-04,
            4.50490139240298E-04, -2.54858131942102E-04,  1.27824189261340E-04,
            -5.97873878043957E-05,  2.66474012012582E-05, -1.14381836209267E-05,
            4.75393030377615E-06, -1.91759589929610E-06,  7.50806465594834E-07,
            -2.84791180387123E-07,  1.04187791696225E-07, -3.64567243689145E-08,
            1.20129296139030E-08, -3.61030126779729E-09,  9.12356140081759E-10,
            -1.36851363400914E-10};

        static const double C1 = 2.35777520630369E-01;
        static const double C2 = 1.35777520630369E+00;

        if ((y <= 0.0) ||(y > 1.0))
            GenAppInternalError("Ierfc only valid for the range 0<=x<1");

        bool doerfc = false;
        const double* a;
        double d;
        if (y >= 0.5)
            {
                doerfc = true;
                a = a1;
                d = 2.0 - 2.0 * y;
            }
        else if (y > 0.1)
            {
                a = a2;
                d = 5.0 * y - 1.5;
            }
        else
            {
                a = a3;
                double w = sqrt(-log(y));
                d = C1 * w - C2;
            }
        double td = d + d;
        double vnp1 = 0.0;
        double vn = 0.0;
        for (int i = 1; i <= 21; i++ )
            {
                int k = 22 - i + 1;
                double temp = vn;
                vn = td * vn - vnp1 + a[k - 1];  // 0 offset adjust
                vnp1 = temp;
            }

        double retval = d * vn - vnp1 + 0.5 * a[0];
        if (doerfc)
            retval *= d;
        return retval;
    }


    double  NormInv(double x)
    /*--
      -- Given area X, 0 < X < 1, find the deviate from the standard normal using
      -- the numerical approximation to the complementary error function found in
      -- FUNCTION RIERFC.  Ensure the argument for RIERFC is strictly less than 1.
    */
    {
        if ((x < 0.0) || (x > 1.0))
            GenAppInternalError("NormInv only valid for range 0<x<1");

        if (x <= 0.49999999999)
            return -sqrt(2.0) * Ierfc(2.0 * x);

        if (x >= 0.5000000001)
            return sqrt(2.0) * Ierfc(2.0 * (1.0 - x));

        return 0.0;
    }

    double Beta(double a, double b){
        // definition of Beta function in terms of Gamma functions
        // "Digital Library of Mathematical Functions" website (2012) equation 5.12.1
        return exp(GammaLn(a+b)-GammaLn(a)-GammaLn(b));
    }

    static double BetaContinuedFraction(double a, double b, double x){
        // continued fraction representation of Beta function
        // "Digital Library of Mathematical Functions" website (2012) equation 8.17.22
        // It is used for the calculation of function Betai only.
        // Continued fraction is evaluated using modified form of Lentz's method

        static const int maxiter = 200;
        static const double bb = 1.0;

        int kk,m;
        double f,C,D,Delta,aa;

        f = stdEps;
        C = f;
        D = 0;

        for (int k=1; k<=maxiter; k++){
            kk = k-1;  // shifted index, since second term is d1
            if(k==1){
                aa = 1.0;
            }else{
                // DLMF website equation 8.17.23
                if(kk%2 == 0){
                    // k=3 <=> d2, k=5 <=> d4, etc
                    m = kk/2;
                    aa = m*(b-m)*x/((a+2*m-1)*(a+2*m));
                }else{
                    // k=2 <=> d1, k=4 <=> d3, etc
                    m = (kk-1)/2;
                    aa = -((a+m)*(a+b+m))*x/((a+2*m)*(a+2*m+1));
                }
            }
            D = bb + aa*D;
            if (fabs(D) < stdEps){
                D = stdEps;
            }
            D = 1.0/D;
            C = bb + aa/C;
            if (fabs(C) < stdEps){
                D = stdEps;
            }
            Delta = D*C;
            f *= Delta;
            if (fabs(1.0 - Delta) < spacing(1.0)){
                return Beta(a,b)*pow(x,a)*pow(1.0-x,b)*f/a;
            }
        }
        GenAppInternalError("BetaContinuedFraction did not converge");
        return 0.0;
    }

    double BetaI(double a, double b, double x){
        // Incomplete beta function
        // "Digital Library of Mathematical Functions" website (2012) equation 8.17.2

        if ((x < 0.0) || (x > 1.0)){
            GenAppInternalError("BetaI only valid for range 0<x<1");
        }

        // joining relationship given DLMF website, end of section 8.17
        if (x < (a + 1.0) / (a + b + 2.0)){
            // standard form
            return BetaContinuedFraction(a,b,x);
        }else{
            // symmetry relationship
            // DLMF website equation 8.17.4
            return 1.0 - BetaContinuedFraction(b,a,1.0-x);
        }
    }

    // --- **************************** misc double array ops

    void MedianSmooth(SC_DoubleArray& data, int kernelSize)
    {
        SC_DoubleArray temp = data;
        int offset = kernelSize / 2;
        for (int i = 0; i < data.Size(); i++)
            {
                double mean = 0.0;
                int indx = i - offset;
                for (int j = 0; j < kernelSize; j++)
                    {
                        if ((indx < 0) || (indx > temp.UpperBound()))
                            {
                                mean += temp[i];
                            }
                        else
                            mean += temp[indx];
                        indx ++;
                    }
                data[i] = mean / double (kernelSize);
            }
    }

    MathError::MathError(const char* inStr)
    {
        CopyString(errStr, inStr, maxLen);
    }


    //ksb 6/20/2012
    //this is a wrapper function for the kiss FFT routine

    void RealFFT(SC_DoubleArray& data, bool forwardFFT)
    {
        int nfft = data.Size();
        int is_inverse_fft = 1;
        if(forwardFFT)
        {
            is_inverse_fft = 0;
        }

        kiss_fft_cfg cfg = kiss_fft_alloc(nfft, is_inverse_fft, 0, 0);

        kiss_fft_cpx *in_data = new kiss_fft_cpx[nfft];
        kiss_fft_cpx *out_data = new kiss_fft_cpx[nfft];

        for(int i = 0; i < nfft; i++)
        {
            in_data[i].r = (float) data[i];
            in_data[i].i = 0.0;
        }

        kiss_fft( cfg , in_data, out_data );

        for(int i = 0; i < nfft; i++)
        {
            if(forwardFFT)
            {
                data[i] = sqrt(pow(double(out_data[i].r),2)+pow(double(out_data[i].i),2));
            }
            else
            {
                data[i] = (double) out_data[i].r;
            }
        }


        free(in_data);
        free(out_data);
        free(cfg);

    }



    double PearsonR(const SC_DoubleArray& x, const SC_DoubleArray& y){
        // Pearson's linear correlation coefficient (r) between two vectors of equal length
        // r = covariance(x,y)/(sqrt(variance(x))*sqrt(variance(y)))

        if (x.IsEmpty() || (x.Size() != y.Size())){
            GenAppInternalError("PearsonR input vectors not valid size");
        }

        int nPts = x.Size();
        if (nPts == 1){
            return 0.0;
        }

        // compute means
        double xmean = 0.0;
        double ymean = 0.0;
        for (int i=0; i<nPts; i++){
            xmean += x[i];
            ymean += y[i];
        }
        xmean /= nPts;
        ymean /= nPts;

        // compute variances & covariance
        double covxy = 0.0;
        double varx = 0.0;
        double vary = 0.0;
        double xdiff,ydiff;
        for (int i=0; i<nPts; i++){
            xdiff = x[i] - xmean;
            ydiff = y[i] - ymean;
            covxy += xdiff*ydiff;
            varx += pow(xdiff,2);
            vary += pow(ydiff,2);
        }

        double denom = sqrt(varx * vary);
        if (denom < spacing(0.0)){
            return nullReal;
        }else{
            return covxy/denom;
        }
    }


    static void selectSort(SC_DoubleArray& x, SC_DoubleArray& y){
        // selection sort for both vectors in place
        // based on values in vector x

        int n = x.Size();
        for (int i=0; i<(n-1); i++){
            // put smallest value in first spot,
            // repeating for each remaining entry
            for (int j=i+1; j<n; j++){
                if (x[i] > x[j]){
                    Swap(x[i], x[j]);
                    Swap(y[i], y[j]);
                }
            }
        }
    }

    void rank(SC_DoubleArray& x){
        // replace a sorted array of values with its ranks
        // with ties given midway values

        int n = x.Size();
        int numsame;
        float value;

        for (int i=0; i<(n-1); i++){
            if (!RealsAreSame(x[i],x[i+1])){
                // two entries are different
                x[i] = i;
                if (i == (n-2)){
                    // handle last entry too
                    x[n-1] = float(n-1);
                }
            }else{
                numsame = 2;
                for (int j=i+2; j<n; j++){
                    if(RealsAreSame(x[i],x[j])){
                        numsame += 1;
                    }else{
                        if (j == (n-1)){
                            // handle when last entry is different
                            // but follows duplicates
                            x[n-1] = float(n-1);
                        }
                        break;
                    }
                }
                // compute average value to assign to duplicates
                value = 0.0;
                for(int j=i;j<(i+numsame); j++){
                    value += float(j);
                }
                value /= numsame;
                for(int j=i;j<(i+numsame); j++){
                    x[j] = value;
                }
                // extra increment to outer counter
                i += (numsame-1);
            }
        }
    }


    double SpearmanR(const SC_DoubleArray& x, const SC_DoubleArray& y){
        // Spearman's Rank correlation coefficient (rho) between two vectors of equal length
        //  rho is simply Pearson coefficient for ranks instead of values

        if (x.IsEmpty() || (x.Size() != y.Size()))
            GenAppInternalError("SpearmanR input vectors not valid size");

        int n = x.Size();
        if (n == 1){
            return 0.0;
        }

        SC_DoubleArray srx = x;
        SC_DoubleArray sry = y;

        // sort both based on srx
        selectSort(srx, sry);
        rank(srx); // replace sorted x values with ranks

        // sort both based on sry
        selectSort(sry, srx);
        rank(sry); // replace sorted y values with ranks

        return PearsonR(srx,sry);
    }

    double WellFunction(double x){
        // exponential integral E1(x)
        // E_1(x) = integral from x to infty of exp(-t)/t dt
        // generally E_n(x), but here specifically n=1
        // "Digital Library of Mathematical Functions" website (2012) equation 6.2.1
        const double euler = 0.57721566490153287; // Euler-Masceroni constant
        const int maxit = 200;

        if (x <= 0.0){
            GenAppInternalError("WellFunction: only valid for x>0");
        }else if (x <= 1.0){
            // power series converges best for x<1
            // DLMF equation 6.6.2
            double sum = 0.0;
            double num = 1.0;
            double term;
            double fac = 1.0;
            for(int n=1; n<maxit; n++){
                fac *= double(n);
                num *= -x;
                term = num/(fac*n);
                sum += term;
                if (fabs(term) < spacing(sum)){
                    return -(sum + log(x) + euler);
                }
            }
            GenAppInternalError("WellFunction: series did not converge");
        }else{
            // approximation for large argument
            // simply related to incomplete Gamma function
            // DLMF website equation 6.11.1
            return incompleteGammaContinuedFraction(0,x);
        }
        return 0.0;
    }

    double ExpIntegral(double x){
        // exponential integral Ei(x)
        // Ei(x) = -integral from x to infty of exp(-t)/t dt
        // "Digital Library of Mathematical Functions" website (2012) equation 6.2.3

        const double euler = 0.57721566490153287; // Euler-Masceroni constant
        const int maxit = 200;
        double term = 1.0;
        double sum = 0.0;

        if (x <= 0.0){
            GenAppInternalError("ExpIntegral: only valid for x>0");
        }else if (x <= 10.0){
            // power series converges best for small x
            // DLMF equation 6.6.2
            double num = 1.0;
            double fac = 1.0;
            for(int n=1; n<maxit; n++){
                fac *= double(n);
                num *= x;
                term = num/(fac*n);
                sum += term;
                if (fabs(term) < spacing(sum)){
                    return sum + log(x) + euler;
                }
            }
            GenAppInternalError("ExpIntegral: series did not converge");
        }else{
            // for large x use asymptotic expansion
            // DLMF equation 6.12.2
            double prev;
            for (int n=1; n<maxit; n++){
                prev = term;
                term *= n/x;
                if(term < spacing(0.0)){
                    break;
                }else if(term < prev){
                    sum += term;
                }else{
                    sum -= prev;
                    break;
                }
            }
            return exp(x)*(1.0+sum)/x;
        }
        return 0.0;
    }

    // from

    double GaussElim(SC_DoubleMatrix& a,
                     SC_DoubleArray& b,
                     SC_DoubleArray& x)

    {
        int n = a.Size();
        if (n == 0)
            GenAppInternalError("GaussElim::n=0");

        if ((n != a[0].Size()) || (n != b.Size()))
            GenAppInternalError("GaussElim::size mismatch");

        x.AllocAndSetSize(n);
        SC_DoubleArray scale(n);
        SC_IntArray        index(n);

        //  assert( N == A.nCol() && N == b.nRow() && N == x.nRow() );
        //  int i, j, k;
        //  Matrix scale(N);    // Scale factor
        //  int *index;  index = new int [N+1];     // Row index list

        //  * Set scale factor, scale(i) = max( |A(i,j)| ), for each row
        for(int i = 0; i < n; i++ )
            {
                index[i] = i;          // Initialize row index list
                double scaleMax = 0.0;
                //      for( j=1; j<=N; j++ )
                for(int j = 0; j < n; j++ )
                    scaleMax = DMax(scaleMax, fabs(a[i][j]));
                scale[i] = scaleMax;
            }

        //  * Loop over rows k = 1, ..., (N-1)
        int signDet = 1;
        for(int k = 0;  k < (n - 1); k++ )
            {
                //* Select pivot row from max( |A(j,k)/s(j)| )
                double ratiomax = 0.0;
                int jPivot = k;
                for(int i = k; i < n; i++ )
                    {
                        double ratio = fabs(a[index[i]][k])/scale[index[i]];
                        if (ratio > ratiomax)
                            {
                                jPivot = i;
                                ratiomax = ratio;
                            }
                    }
                //* Perform pivoting using row index list
                int indexJ = index[k];
                if(jPivot != k)
                    {             // Pivot
                        indexJ = index[jPivot];
                        index[jPivot] = index[k];   // Swap index jPivot and k
                        index[k] = indexJ;
                        signDet *= -1;              // Flip sign of determinant
                    }
                //* Perform forward elimination
                for(int i = k + 1; i < n; i++ )
                    {
                        double coeff = a[index[i]][k]/a[indexJ][k];
                        for(int j = k + 1;  j < n; j++ )
                            a[index[i]][j] -= coeff * a[indexJ][j];
                        a[index[i]][k] = coeff;
                        b[index[i]] -= a[index[i]][k] * b[indexJ];
                    }
            }

        //* Compute determinant as product of diagonal elements
        double determ = signDet;       // Sign of determinant
        //  for( i=1; i<=N; i++ )
        for(int i = 0; i < n; i++)
            determ *= a[index[i]][i];

        //* Perform backsubstitution
        x[n - 1] = b[index[n - 1]]/a[index[n - 1]][n - 1];
        for (int i = n - 2; i >= 0; i--)
            {
                double sum = b[index[i]];
                for(int j = i + 1; j < n; j++ )
                    sum -= a[index[i]][j] * x[j];
                x[i] = sum/a[index[i]][i];
            }
        return determ;
    }


}; // end NS

