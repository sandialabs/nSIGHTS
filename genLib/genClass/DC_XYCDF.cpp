///////////////////////////////////////////////////////////////////////////////////
//
// DC_XYCDF.cpp
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
// DESCRIPTION: Platform independent and application independent base classes and
//              support classes.
//              Data Classes (DC) that define basic types of data used within the
//              framework: Statistical Cumulative Distribution Functions (CDF)
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genClass/DC_XYCDF.h>


const char* DC_XYCDF::quantileStrs[nquantileType] = {"Weibull", "Hazen", "Excel", "Generic"};


DC_XYCDF :: DC_XYCDF()
{
    isValid = false;
    isLog = false;
}

DC_XYCDF::DC_XYCDF(const DC_XYCDF& a)
    : DC_XYData(a)
{
    isValid = a.isValid;
    isLog = a.isLog;
}

DC_XYCDF :: ~DC_XYCDF()
{
}

DC_XYCDF&  DC_XYCDF::operator=(const DC_XYCDF& a)
{
    if (&a != this)
        {
            DC_XYData::operator =(a);
            isValid = a.isValid;
            isLog = a.isLog;
        }
    return *this;
}

bool DC_XYCDF::SetupOK(SC_SetupErr& errData)
{
    errData.ClearErrors();
    if (!xData.IsAscending(false))
        errData.SetConstantError("X data not ascending");
    if (!yData.IsAscending(false))
        errData.SetConstantError("Y data not ascending");
    if (xData.Size() != yData.Size())
        errData.SetConstantError("X & Y data different sizes ???");

    const double minLog = -700.0;
    const double maxLog = 700.0;

    for (int i = 0; i < yData.Size(); i++)
        {
            if ((yData[i] < 0.0) || (yData[i] > 1.0)) //this assumes (i/(n+1)) quantile calculation
                errData.SetConstantError("Y values must be between 0 and 1");

            if (isLog && ((xData[i] < minLog) || (xData[i] > maxLog)))
                errData.SetConstantError("log X values must be between -700 and 700");
        }

    isValid = errData.NoErrors();

    return isValid;
}

double DC_XYCDF::GetLimitsMean(const double& minP,
                               const double& maxP) const
{
    int nOK = 0;
    double mean = 0.0;

    double maxToUse = 1.1;  // kluge for maxP == 1.0 to ensure that last point is taken
    if (maxP < 1.0)
        maxToUse = maxP;

    for (int i = 0; i < Size(); i++)
        {
            if (yData[i] >= maxToUse)
                break;

            if (yData[i] >= minP)
                {
                    mean += xData[i];
                    nOK++;
                }
        }

    if (nOK == 0)
        return nullReal;

    mean /= double(nOK);
    if (IsLog())
        mean = exp(mean);
    return mean;
}


double DC_XYCDF::GetProbability(const double& dataVal,
                                bool    nullOutSideLimits) const
{
    double interpVal = dataVal;
    if (isLog)
        {
            if (interpVal <= 0.0)
                GenAppInternalError("DC_XYCDF::GetProbability");
            interpVal = log(interpVal);
        }

    return DC_XYCDF::CDFInterp(interpVal, xData, yData, nullOutSideLimits);
}

double DC_XYCDF::GetDataVal(const double& pVal,
                            bool    nullOutSideLimits) const
{
    double resVal = DC_XYCDF::CDFInterp(pVal, yData, xData, nullOutSideLimits);
    if (isLog && (!RealIsNull(resVal)))
        resVal = exp(resVal);
    return resVal;
}


double DC_XYCDF::CDFInterp(const double&         theVal,
                           const SC_DoubleArray& aVec,
                           const SC_DoubleArray& bVec,
                           bool                  nullOutsideLimits) const
{
    if (!isValid)
        GenAppInternalError("DC_XYCDF::CDFInterp");

    if (theVal < aVec[0])
        {
            if (nullOutsideLimits)
                return nullReal;
            return bVec[0];
        }


    if (theVal > aVec.LastIndex())
        {
            if (nullOutsideLimits)
                return nullReal;
            return bVec.LastIndex();
        }

    if (theVal == aVec[0])
        return bVec[0];

    //binary splitting yields aVec[startIndx] < theVal <= aVec[stopIndx]

    int startIndx = 0;
    int stopIndx = aVec.UpperBound();

    while ((startIndx + 1) < stopIndx) {
        int i = startIndx + (stopIndx - startIndx) / 2;
        if (theVal <= aVec[i])
            stopIndx  = i;
        else
            startIndx = i;
    }

    const double& yS = bVec[startIndx];
    const double& yE = bVec[startIndx + 1];
    const double& xS = aVec[startIndx];
    const double& xE = aVec[startIndx + 1];

    return yS + (yE - yS) * (theVal - xS) / (xE - xS);

}



void DC_XYCDF::SetCDFQuantile(QuantileType  qType,
                              const double& genericVal)
{
    double quantileValue = genericVal;
    switch (qType) {
    case qtWeibull : {
        quantileValue = 0.0;
        break;
    }
    case qtHazen : {
        quantileValue = 0.5;
        break;
    }
    case qtExcel : {
        quantileValue = 1.0;
        break;
    }
    case qtGeneric : {
        break;
    }
    }

    int N = xData.Size();
    double denom = double(N + 1) - 2.0 * quantileValue;

    for (int i = 0; i < N; i++)
        {
            yData[i] = (double(i + 1) - quantileValue) / denom;
        }

}


bool DC_XYCDF::QuantileOK(QuantileType qType,
                          const double& genericVal)
{
    return (qType != qtGeneric) || ((genericVal >= 0.0) && (genericVal <= 1.0));
}

