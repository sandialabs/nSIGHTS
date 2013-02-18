///////////////////////////////////////////////////////////////////////////////////
//
// DC_Curve.cpp
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

#include <math.h>
#include <float.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/C_Common.h>

#include <objClass/DC_Curve.h>

const double DC_Curve::maxExp = 200.0;

DC_Curve::DC_Curve() :
    curveIsCreated(false),
    inputXmin(0.0),
    inputXmax(1.0),
    normalizedTension(1.0)
{
}

DC_Curve::DC_Curve(const DC_Curve& a) :
    CurveDescription(a)
{
    LocalCopy(a);
}

DC_Curve::DC_Curve(const CurveDescription& a) :
    CurveDescription(a),
    curveIsCreated(false),
    inputXmin(0.0),
    inputXmax(1.0),
    normalizedTension(1.0)
{
}

DC_Curve::~DC_Curve()
{
    // empty
}

DC_Curve& DC_Curve::operator= (const DC_Curve& a)
{
    if (&a != this)
    {
        CurveDescription::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

DC_Curve& DC_Curve::operator= (const CurveDescription& a)
{
    if (&a != this)
        CurveDescription::operator=(a);
    return *this;
}

void  DC_Curve::LocalCopy(const DC_Curve& a)
{
    curveIsCreated    = a.curveIsCreated;
    curveData         = a.curveData;
    splineDeriv       = a.splineDeriv;
    polyCoefficients  = a.polyCoefficients;
    normalizedTension = a.normalizedTension;
    inputXmin         = a.inputXmin;
    inputXmax         = a.inputXmax;
}


double DC_Curve::CalcSpline(const double& xVal)
{
    double xPrev, xNext, yPrev, yNext, yRet;
    if (!GetSpanPoints(xVal, xPrev, xNext, yPrev, yNext, yRet))
        return yRet;

    double del_1 = xVal - xPrev;
    double del_2 = xNext - xVal;
    double del_s = xNext - xPrev;

    double  exps_1  = exp(normalizedTension * del_1);
    double  sinhd_1 = (exps_1 - 1.0 / exps_1) / 2.0;


    double  exps_2  = exp(normalizedTension * del_2);
    double  sinhd_2 = (exps_2 - 1.0 / exps_2) / 2.0;

    double  exps    = exps_1 * exps_2;
    double  sinhs  = (exps - 1.0 / exps) / 2.0;

    double dNext = splineDeriv[nextPoint];
    double dPrev = splineDeriv[lastPoint];

    double curvVal = (dNext  * sinhd_1 + dPrev * sinhd_2) / sinhs;
    curvVal += ((yNext  - dNext) * del_1 + (yPrev - dPrev) * del_2) / del_s;

    return curvVal;
}

// curve interpolation routines

double DC_Curve::CalcPoly(const double& xVal)
{
    double retVal = polyCoefficients[0];
    double tempX = xVal;
    for (int i = 1; i < polyCoefficients.Size(); i++)
    {
        retVal += polyCoefficients[i] * tempX;
        tempX *= xVal;
    }
    return retVal;
}

bool DC_Curve::GetSpanPoints(const double& xVal,
                                  double&   xPrev,
                                  double&   xNext,
                                  double&   yPrev,
                                  double&   yNext,
                                  double&   yRet)
{
    if (xVal <= inputXmin)
    {
        yRet = curveData.yData[0];
        return false;
    }
    if (xVal >= inputXmax)
    {
        yRet = curveData.yData.LastIndex();
        return false;
    }

    SC_DoubleArray& xData = curveData.xData;
    SC_DoubleArray& yData = curveData.yData;

    nextPoint = 0;
    while (xData[nextPoint] < xVal)
        nextPoint++;
    lastPoint = nextPoint - 1;

    xPrev = xData[lastPoint];
    xNext = xData[nextPoint];

    yPrev = yData[lastPoint];
    yNext = yData[nextPoint];

    return true;
}


double DC_Curve::CalcLinear(const double& xVal)
{
    double xPrev, xNext, yPrev, yNext, yRet;
    if (!GetSpanPoints(xVal, xPrev, xNext, yPrev, yNext, yRet))
        return yRet;

    return yPrev + (yNext - yPrev)/ (xNext - xPrev) * (xVal - xPrev);
}

double DC_Curve::CalcStepMid(const double& xVal)
{
    double xPrev, xNext, yPrev, yNext, yRet;
    if (!GetSpanPoints(xVal, xPrev, xNext, yPrev, yNext, yRet))
        return yRet;

    if (xVal < (xNext + xPrev) / 2.0)
        return yPrev;

    return yNext;
}

double  DC_Curve::CalcStepFull(const double& xVal)
{
    double xPrev, xNext, yPrev, yNext, yRet;
    if (!GetSpanPoints(xVal, xPrev, xNext, yPrev, yNext, yRet))
        return yRet;

    return yPrev;
}


//  various sigs to get data - dummy fils for now
double  DC_Curve::GetCurveY(const double& inputX)
{
    if (!curveIsCreated)
        GenAppInternalError("DC_Curve::GetCurveY");


    switch(curveType)
    {
        case Curve::ctCubicSpline:
                return CalcSpline(inputX);
                break;
        case Curve::ctPolynomial:
                return CalcPoly(inputX);
                break;
        case Curve::ctLinear:
                return CalcLinear(inputX);
                break;
        case Curve::ctStepMid:
                return CalcStepMid(inputX);
                break;
        case Curve::ctStepFull:
                return CalcStepFull(inputX);
                break;
        default:
                break;
    }

    return inputX;
}

void DC_Curve::GetCurveY(const DC_XYData& inputXY,   // only X used
                               DC_XYData& outputXY)
{
    GetCurveY(inputXY.xData, outputXY);
}

void DC_Curve::GetCurveY(const SC_DoubleArray& inputX,
                               DC_XYData&      outputXY)
{
    outputXY.Alloc(inputX.Size());
    for (int i = 0; i < inputX.Size(); i++)
    {
        double tempX = inputX[i];
        if (!RealIsNull(tempX))
        {
            outputXY.xData += tempX;
            outputXY.yData += GetCurveY(tempX);
        }
    }
}

void DC_Curve::GetCurveY(const SC_DoubleArray& inputX,
                                  SC_DoubleArray& outputY)
{
    outputY.Alloc(inputX.Size());
    for (int i = 0; i < inputX.Size(); i++)
    {
        double tempX = inputX[i];
        if (RealIsNull(tempX))
            outputY += tempX;
        else
            outputY += GetCurveY(tempX);
    }
}

void DC_Curve::GetCurveY(DC_XYData& inputXoutputY)
{
    int nOK = 0;
    for (int i = 0; i < inputXoutputY.Size(); i++)
    {
        double tempX = inputXoutputY.xData[i];
        if (!RealIsNull(tempX))
        {
            inputXoutputY.xData[nOK] = tempX;
            inputXoutputY.yData[nOK++] = GetCurveY(tempX);
        }
    }
    inputXoutputY.xData.SetSize(nOK);
    inputXoutputY.yData.SetSize(nOK);
}



bool DC_Curve::CreateCurve(const DC_XYData&    curveInput,
                                 SC_SetupErr& errData)
{
    curveData.DeAlloc();
    // grab a cleaned copy of the input
    if (!curveData.CreateFrom(curveInput.xData, curveInput.yData))
    {
        errData.SetInternalError("Could not copy input - all null or memory allocation ??");
        return false;
    }

    return CreateCurve(errData);
}

bool DC_Curve::CreateCurve(const SC_DoubleArray&    curveXInput,
                           const SC_DoubleArray&    curveYInput,
                                 SC_SetupErr& errData)
{   
    curveData.DeAlloc();
    if (!curveData.CreateFrom(curveXInput, curveYInput))
    {
        errData.SetInternalError("Could not copy input - all null or memory allocation ??");
        return false;
    }

    return CreateCurve(errData);
}



bool DC_Curve::CreateCurve(SC_SetupErr& errData)
{
    // clear current settings
    curveIsCreated = false;
    splineDeriv.DeAlloc();
    polyCoefficients.DeAlloc();

    if (!CurveSetupOK(curveData.Size(), errData))
        return false;

    //  set limits
    curveData.xData.CalcMinMax(inputXmin, inputXmax);
    switch(curveType)
    {
        case Curve::ctCubicSpline :
        {
            if (!curveData.xData.IsAscending())
            {
                errData.SetInternalError("spline data must be ascending in X");
                return false;
            }
            if (!GenCubicSpline())
            {
                errData.SetInternalError("tension factor too high");
                return false;
            }

            curveIsCreated = true;
            break;
        }
        case Curve::ctPolynomial: {

            int n_coeff = polyOrder + 1;
            SC_DoubleArray polyCoeff_1;
            PolyLeastSquares(curveData.xData, curveData.yData, n_coeff, polyCoeff_1);

            // adjust to 0 based array
            polyCoefficients.Alloc(n_coeff);
            for (int i = 0; i < n_coeff; i++)
                polyCoefficients += polyCoeff_1[i + 1];

            curveData.DeAlloc();
            curveIsCreated = true;
            break;
        }
        case Curve::ctLinear:
        case Curve::ctStepMid:
        case Curve::ctStepFull:
        {
            if (!curveData.xData.IsAscending())
            {
                errData.SetInternalError("linear/step data must be ascending in X");
                return false;
            }
            curveIsCreated = true;
            break;
        }
        default:
        {
            GenAppInternalError("DC_Curve::CreateCurve");
        }
    }

    return curveIsCreated;
}

//////////////////////////////////////////////////////////////////////
// Poly least squares numeric methods


void DC_Curve::PolyLeastSquares(const SC_DoubleArray& in_x_data,     // input X and Y -- 0 based
                                         SC_DoubleArray& in_y_data,
                                         int             n_poly_coeff,  //  polyorder + 1
                                         SC_DoubleArray& coeff_data)    // output coeff 1 based
{
/*
VAR
     i, j, n_sums     : WORD;
     sums   : ls_real_vector;
     temp   : REAL8;

BEGIN
 */
    int n_data = in_x_data.Size();
    int n_sums = n_poly_coeff * 2 - 1;

    SC_DoubleArray sums(n_sums + 1, 0.0);
    sums[1] = double(n_data);

    coeff_data.AllocAndFill(n_poly_coeff + 1, 0.0);

/*
     sums[1]:=REAL8(n_data);
     FOR i:= 2 TO n_sums DO
          sums[i]:= r_zero;
     FOR i:= 1 TO n_coeff DO
          coeff_data[i]:= r_zero;
*/
    int i, j;
    for (i = 1; i <= n_data; i++ )
    {
        double x    = in_x_data[i - 1];  // O offset
        double temp = x;
        for (j = 2; j <= n_sums; j++ )
        {
            sums[j] += temp;
            temp *= x;
        }
    }

/*
     FOR i:= 1 TO n_data DO
          BEGIN
               temp:=in_x_data[i];

               FOR j:= 2 TO n_sums DO
                    BEGIN
                         sums[j]:=sums[j] + temp;
                         temp := temp * in_x_data[i];
                    END;
          END;
*/
    for (i = 1; i <= n_poly_coeff; i++ )
    {
        for (j = 1; j <= n_data; j++ )
            coeff_data[i] += in_y_data[j - 1];  // 0 based

        for (j = 1; j <= n_data; j++ )
            in_y_data[j - 1] *= in_x_data[j - 1];  // 0 based
    }

/*
     FOR i:= 1 TO n_coeff DO
          BEGIN
               FOR j:= 1 TO n_data DO
                    coeff_data[i]:= coeff_data[i]+in_y_data[j];

               FOR j:= 1 TO n_data DO
                    in_y_data[j]:=in_y_data[j] * in_x_data[j];
          END;
*/

    SC_DoubleMatrix ls_matrix;
    ls_matrix.MatrixAlloc(n_poly_coeff + 1, n_poly_coeff + 1);
    for (i = 1; i <= n_poly_coeff; i++ )
        for (j = 1; j <= n_poly_coeff; j++)
            ls_matrix[i][j] = sums[i + j - 1];

/*
     FOR i:= 1 TO n_coeff DO
          FOR j:= 1 TO n_coeff DO
               ls_matrix[i,j]:=sums[i+j-1];
*/

     MatrixSolve(ls_matrix, coeff_data, n_poly_coeff);
}


void  DC_Curve::LuDecompose(SC_DoubleMatrix&    a,
                         SC_IntArray&       indx,
                             int                n)
{
/*
VAR
     aa_max, sum, temp : REAL8;
     i, j, k, i_max : WORD;
     vv   : ls_real_vector;

BEGIN
*/
    SC_DoubleArray vv(n + 1);
    indx.Alloc(n + 1);

    double aa_max;
    int i, j, k;
    for (i = 1; i <= n; i++ )
    {
        aa_max = 0.0;
        for (j = 1; j <= n; j++ )
        {
            double temp = fabs(a[i][j]);
            if ( temp > aa_max)
                aa_max = temp;
        }
        vv[i] = 1.0 / aa_max;
    }
/*
     FOR i:=1 TO n DO
          BEGIN
               aa_max := r_zero;
               FOR j:=1 TO n DO
                    BEGIN
                         temp:=ABS(a[i,j]);
                         IF temp > aa_max THEN
                              aa_max:=temp;
                    END;
               vv[i]:= r_1 / aa_max;
          END;
*/

/*
     FOR j:= 1 TO n DO
          BEGIN
*/
    int i_max = 0;
    for (j = 1; j <= n; j++ )
    {
        for (i = 1; i <= j - 1; i++ )
        {
            double sum = a[i][j];
            for (k = 1; k <= i - 1; k++ )
            {
                sum = sum - a[i][k] * a[k][j];
            }
            a[i][j] = sum;
        }
/*
        FOR i:= 1 TO j-1 DO
             BEGIN
                  sum := a[i,j];
                  FOR k:= 1 TO i-1 DO
                       sum := sum - a[i,k] * a[k,j];
                  a[i,j] := sum;
             END;
 */
        aa_max = 0.0;
        for (i = j; i <= n; i++ )
        {
            double sum = a[i][j];
            for (k = 1; k <= j - 1; k++ )
            {
                sum = sum - a[i][k] * a[k][j];
            }
            a[i][j] = sum;
            double temp = vv[i] * fabs(sum);
            if (temp > aa_max)
            {
                i_max = i;
                aa_max = temp;
            }
        }
/*
        aa_max:= r_zero;
        FOR i:= j TO n DO
             BEGIN
                  sum := a[i,j];
                  FOR k:= 1 TO j-1 DO
                       sum:= sum - a[i,k] * a[k,j];
                  a[i,j]:= sum;
                  temp:= vv[i] * ABS(sum);
                  IF temp > aa_max THEN
                       BEGIN
                            i_max := i;
                            aa_max:= temp;
                       END;
             END;
*/
        if ( j != i_max)
        {
            for (k = 1; k <= n; k++ )
            {
                double temp = a[i_max][k];
                a[i_max][k] = a[j][k];
                a[j][k]     = temp;
            }
            vv[i_max] = vv[j];
        }

/*
        IF j <> i_max THEN
        BEGIN
             FOR k:= 1 TO n DO
                  BEGIN
                       temp := a[i_max, k];
                       a[i_max,k] := a[j,k];
                       a[j,k] := temp;
                  END;
             vv[i_max]:=vv[j];
        END;
 */
        indx[j] = i_max;
        if ( j != n )
        {
            double temp = 1.0 / a[j][j];
            for (i = j + 1; i <= n; i++ )
            {
                a[i][j] =a[i][j] * temp;
            }
        }
/*
        indx[j]:=i_max;
        IF j <> n THEN
             BEGIN
                  temp := r_1 / a[j,j];
                  FOR i:= j+1 TO n DO
                       a[i,j]:=a[i,j] * temp;
             END;
 */
    }
}

void  DC_Curve::LuBacksubs(const SC_DoubleMatrix&   a,
                            const SC_IntArray&      indx,
                                  SC_DoubleArray&   b,
                                  int               n)
{
/*
VAR
     i, ii, j, ll  : WORD;
     sum  : REAL8;
BEGIN
 */
    int ii = 0;
    int i;
    for (i = 1; i <= n; i++ )
    {
        int ll = indx[i];
        double sum = b[ll];
        b[ll] = b[i];
        if (ii != 0)
        {
            for (int j = ii; j <= i - 1; j++ )
                sum = sum - a[i][j] * b[j];
        }
        else
        {
            if ( sum != 0.0 )
            {
                ii = i;
            }
        }
        b[i] = sum;
    }

/*
     ii := 0;
     FOR i:=1 TO n DO
          BEGIN
               ll := indx[i];
               sum := b[ll];
               b[ll] := b[i];
               IF ii <> 0 THEN
                    FOR j:= ii TO i-1 DO
                         sum := sum - a[i,j] * b[j]
               ELSE
                    IF sum <> r_zero THEN
                         ii := i;
               b[i]:=sum;
          END;
 */

    for (i = n; i >= 1; i-- )
    {
        double sum = b[i];
        for (int j = i + 1; j <= n; j++)
        {
            sum = sum - a[i][j] * b[j];
        }
        b[i] =sum / a[i][i];
    }

/*
     FOR i:= n DOWNTO 1 DO
          BEGIN
               sum:=b[i];
               FOR j:= i+1 TO n DO
                    sum := sum - a[i,j] * b[j];
               b[i]:=sum/a[i,i];
          END;
*/

}




void DC_Curve::MatrixSolve(SC_DoubleMatrix& ls_matrix, SC_DoubleArray& vector, int n_eqn)
{
    SC_IntArray indx(n_eqn + 1);
    LuDecompose(ls_matrix, indx, n_eqn);
    LuBacksubs(ls_matrix, indx, vector, n_eqn);
}


//////////////////////////////////////////////////////////////////////
// Spline numeric methods

bool  DC_Curve::CheckExp(const double& argVal, double& exp_argVal)
{
    if (fabs(argVal) > maxExp)
        return false;
    exp_argVal  = exp(argVal);
    return true;
}

bool DC_Curve::GenCubicSpline()
{
     SC_DoubleArray& inX = curveData.xData;
     SC_DoubleArray& inY = curveData.yData;
     int n_minus_1 = inX.Size() - 1;
     splineDeriv.AllocAndSetSize(inX.Size());

     double del_x_1 = inX[1] - inX[0];
     double slope_1 = (inY[1] - inY[0]) / del_x_1;

     normalizedTension = splineTension * double(n_minus_1) / (inX[n_minus_1] - inX[0]);

     double dels = normalizedTension * del_x_1;
     double exps;
     if (!CheckExp(dels, exps))
        return false;

     double sinhs = (exps - 1.0 / exps) / 2.0;
     double sinhin = 1.0 / (del_x_1 * sinhs);
     double diag_1 = sinhin * (dels * (exps + 1.0 / exps) / 2.0 - sinhs);
     double diagin = 1.0 / diag_1;
     if (splineNaturalSlope)
     {
          splineDeriv[0] = 0.0;
     }
     else
     {
          splineDeriv[0] = diagin * (slope_1 - fixedSplineStartSlope);
     }
     double spdiag = sinhin * (sinhs - dels);

     SC_DoubleArray temp(inX.Size());
     temp[0] = diagin * spdiag;

     int i;
     double slope_2;
     for(i = 1; i < n_minus_1; i++)
     {
        double del_x_2 = inX[i+1] - inX[i];
        slope_2 = (inY[i+1] - inY[i]) / del_x_2;
        dels = normalizedTension * del_x_2;
        if (!CheckExp(dels, exps))
            return false;

        sinhs = (exps - 1.0 / exps) / 2.0;
        sinhin= 1.0 / (del_x_2 * sinhs);

        double diag_2 = sinhin * (dels * (exps + 1.0 / exps) / 2.0 - sinhs);
        diagin = 1.0 / (diag_1 + diag_2 - spdiag * temp[i-1]);
        splineDeriv[i] = diagin *
                    (slope_2 - slope_1 - spdiag * splineDeriv[i-1]);
        spdiag = sinhin * (sinhs - dels);
        temp[i]= diagin * spdiag;
        slope_1 = slope_2;
        diag_1 = diag_2;
     }

     diagin = 1.0 / (diag_1 - spdiag * temp[n_minus_1 - 1]);
     if (splineNaturalSlope)
     {
          splineDeriv[n_minus_1]= - diagin * spdiag * splineDeriv[n_minus_1 - 1];
     }
     else
     {
          splineDeriv[n_minus_1]= diagin * (fixedSplineEndSlope - slope_2 - spdiag * splineDeriv[n_minus_1 - 1]);
     }


     for(i = n_minus_1 - 1; i >= 0; i--)
     {
          splineDeriv[i]= splineDeriv[i] - temp[i] * splineDeriv[i+1];
     }

     return true;
}

