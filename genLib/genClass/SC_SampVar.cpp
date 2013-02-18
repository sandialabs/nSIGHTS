///////////////////////////////////////////////////////////////////////////////////
//
// SC_SampVar.cpp
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
// DESCRIPTION: class defining distribution an containing static menu strings
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/C_Common.h>
#include <genClass/DO_XYCDF.h>
#include <genClass/DO_TableData.h>

#include <genClass/SC_SampVar.h>


SC_StringArray  SampleDistributionBase::distStrs;

void SampleDistributionBase::InitDistStrings()
{
    if (distStrs.IsNotEmpty())
        return;

    distStrs.Alloc(15);
    distStrs += "Normal";
    distStrs += "Truncated Normal";
    distStrs += "Log-Normal";
    distStrs += "Truncated Log-Normal";
    distStrs += "Uniform";
    distStrs += "Log-Uniform";
    distStrs += "Triangular";
    distStrs += "Log-Triangular";
    distStrs += "Exponential";
    distStrs += "Poisson";
    distStrs += "Weibull";
    distStrs += "Beta";
    distStrs += "Student t";
    distStrs += "User CDF";
    distStrs += "Tabulated";

}



const char*     SampleDistributionBase::GetDistString(SVDistribution dist)
{
    InitDistStrings();
    return distStrs[dist];
}

void SampleDistributionBase::GetDistStrings(SC_StringArray& outStrs)
{
    InitDistStrings();
    outStrs = distStrs;
}

const SC_StringArray& SampleDistributionBase::GetDistStrings()
{
    InitDistStrings();
    return distStrs;
}

bool SampleDistributionBase::UsesMean(SVDistribution dist)
{
    return (IsNormal(dist) || IsExponential(dist) || IsStudentT(dist));
}

bool SampleDistributionBase::UsesStdDev(SVDistribution dist)
{
    return (IsNormal(dist) || IsStudentT(dist));
}

bool  SampleDistributionBase::UsesMinMax(SVDistribution dist)
{
    return  ! (IsNotTruncNormal(dist) || IsExponential(dist) || IsPoisson(dist)
               || IsWeibull(dist) || IsUserCDF(dist) || IsTableLookup(dist)
               || IsStudentT(dist));
}

bool SampleDistributionBase::IsLinear(SVDistribution dist)
{
    return (dist == svdNormal) || (dist == svdTruncNormal)
        || (dist == svdUniform) || (dist == svdTriangular)
        || (dist == svdStudentT);
}

bool SampleDistributionBase::IsLog(SVDistribution dist)
{
    return (dist == svdLogNormal) || (dist == svdTruncLogNormal)
        || (dist == svdLogUniform) || (dist == svdLogTriangular);
}

bool SampleDistributionBase::IsNormal(SVDistribution dist)
{
    return (dist == svdNormal) || (dist == svdTruncNormal)
        || (dist == svdLogNormal) || (dist == svdTruncLogNormal);
}

bool SampleDistributionBase::IsNotTruncNormal(SVDistribution dist)
{
    return (dist == svdNormal) || (dist == svdLogNormal);
}

bool SampleDistributionBase::IsTruncNormal(SVDistribution dist)
{
    return (dist == svdTruncNormal) || (dist == svdTruncLogNormal);
}

bool SampleDistributionBase::IsUniform(SVDistribution dist)
{
    return (dist == svdUniform) || (dist == svdLogUniform);
}
bool SampleDistributionBase::IsTriangular(SVDistribution dist)
{
    return (dist == svdTriangular) || (dist == svdLogTriangular);
}

bool SampleDistributionBase::IsExponential(SVDistribution dist)
{
    return (dist == svdExponential);
}

bool SampleDistributionBase::IsPoisson(SVDistribution dist)
{
    return (dist == svdPoisson);
}

bool SampleDistributionBase::IsWeibull(SVDistribution dist)
{
    return (dist == svdWeibull);
}
bool SampleDistributionBase::IsBeta(SVDistribution dist)
{
    return (dist == svdBeta);
}
bool SampleDistributionBase::IsStudentT(SVDistribution dist)
{
    return (dist == svdStudentT);
}
bool SampleDistributionBase::IsUserCDF(SVDistribution dist)
{
    return (dist == svdUserCDF);
}
bool SampleDistributionBase::IsTableLookup(SVDistribution dist)
{
    return (dist == svdTableLookup);
}

double  SC_SampVar::stdDevMultiplier = 3.0;
const double SC_SampVar::poissonMaxLambda = 1.0E+06;      // 1.0E+06
const double SC_SampVar::minBetaExp = 0.001;              // 0.001
const double SC_SampVar::maxBetaExp = 1000.0;             // 1000.0

const double SC_SampVar::samplRMin = 4.0E-36;
const double SC_SampVar::samplRMax = 0.9999999;

SC_SampVar::SC_SampVar(bool defIsLinear)
{
    if (defIsLinear)
        svDistribution  = svdUniform;
    else
        svDistribution  = svdLogUniform;

    distMean        = 0.5;
    distStdDev      = 0.1;
    distLogStdDev   = 0.01;

    distPeak        = 0.5;
    distMin         = 0.0;
    distMax         = 1.0;

    poissonLambda   = 100.0;

    weibullShape    = 1.0;
    weibullScale    = 1.0;
    betaP           = 1.0;
    betaQ           = 1.0;

    tableLookupColumn = 0;

    userCDFDC       = 0;
    tableLookupDC   = 0;
}

SC_SampVar::SC_SampVar(const SC_SampVar& a)
{
    LocalCopy(a);
}


SC_SampVar::~SC_SampVar()
{
}

SC_SampVar& SC_SampVar::operator= (const SC_SampVar& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}



bool SC_SampVar::SampleSetupOK(SC_SetupErr& errData,
                               const char*  varID)
{
    userCDFDC = 0;

    SC_SetupErr sampErr;

    // min max check for non-normal
    if ((svDistribution == svdNormal) || (svDistribution == svdTruncNormal) || (svDistribution == svdStudentT))
        {
            if (distStdDev < stdEps)
                sampErr.SetConstantError("distribution std deviation must be > 0.0");
        }
    else if ((svDistribution == svdLogNormal) || (svDistribution == svdTruncLogNormal))
        {
            if (distLogStdDev < stdEps)
                sampErr.SetConstantError("distribution log std deviation must be > 0.0");
            else if (distMean < stdEps)
                sampErr.SetConstantError("distribution mean must be > 0.0");
        }
    else if (svDistribution == svdExponential)
        {
            if ((distMean < stdEps))
                sampErr.SetConstantError("exponential mean must be > 0.0");
        }
    else if (IsPoisson())
        {
            if ((poissonLambda < stdEps)|| (poissonLambda > poissonMaxLambda))
                sampErr.SetConstantError("Poisson mean must be > 1 and < 1000000");
        }
    else if (IsWeibull())
        {
            if (fabs(weibullShape) < stdEps)
                sampErr.SetConstantError("Weibull shape cannot be 0.0");
            if (weibullScale < stdEps)
                sampErr.SetConstantError("Weibull scale must be +ve");
        }
    else if (IsUserCDF())
        {
            if (userCDFInputObjRef.objRef == 0)
                {
                    sampErr.SetConstantError("user CDF reference not set");
                }
            else
                if (userCDFInputObjRef.objRef->StatusNotOK())
                    {
                        sampErr.SetConstantError("user CDF reference not OK");
                    }
                else
                    {
                        DO_XYCDF* tmpDO = static_cast<DO_XYCDF*>(userCDFInputObjRef.GetData(typeid(DO_XYCDF)));
                        if (tmpDO == 0)
                            {
                                sampErr.SetConstantError("selected CDF is not set ??");
                            }
                        else
                            if (!tmpDO->DataOK())
                                {
                                    sampErr.SetInternalError("selected CDF -", tmpDO->objErrMsg);
                                }
                            else
                                {
                                    userCDFDC = tmpDO->GetCDF();
                                }
                    }
        }
    else if (IsTableLookup())
        {
            if (tableLookupObjRef.objRef == 0)
                {
                    sampErr.SetConstantError("table reference not set");
                }
            else
                if (tableLookupObjRef.objRef->StatusNotOK())
                    {
                        sampErr.SetConstantError("table input reference not OK");
                    }
                else
                    {
                        DO_TableData* tmpDO = static_cast<DO_TableData*>(tableLookupObjRef.GetData(typeid(DO_TableData)));
                        if (tmpDO == 0)
                            {
                                sampErr.SetConstantError("selected table is not set ??");
                            }
                        else
                            if (!tmpDO->DataOK())
                                {
                                    sampErr.SetInternalError("selected table -", tmpDO->objErrMsg);
                                }
                            else
                                {
                                    const DC_TableData* tmpDC = tmpDO->tableData;
                                    if (tmpDC->GetNRows() < 1)
                                        sampErr.SetConstantError("no rows in selected table");
                                    else if (tmpDC->GetNCols() <= tableLookupColumn)
                                        sampErr.SetConstantError("not enough columns in selected table");
                                    else
                                        tableLookupDC = tmpDC;
                                }
                    }
        }


    if (UsesMinMax())
        {
            if (distMin >= distMax)
                sampErr.SetConstantError("distribution min must be < max");
            //  tri check
            else if ((svDistribution == svdTriangular) || (svDistribution == svdLogTriangular))
                {
                    if ((distPeak >= distMax) || (distPeak <= distMin))
                        sampErr.SetConstantError("distribution peak must be between min and max");
                    else if ((svDistribution == svdLogTriangular) && (distPeak < stdEps))
                        sampErr.SetConstantError("log distribution peak must be greater than 0.0");
                }

            // log check
            if (IsLog(svDistribution) && (distMin < stdEps))
                sampErr.SetConstantError("log distribution min must be greater than 0.0");

            if (IsBeta())
                {
                    if ((betaP < minBetaExp) || (betaP > maxBetaExp))
                        sampErr.SetConstantError("beta distribution P exponent must be 0.001 to 1000.0");
                    if ((betaQ < minBetaExp) || (betaQ > maxBetaExp))
                        sampErr.SetConstantError("beta distribution P exponent must be 0.001 to 1000.0");
                }

            if (IsTruncNormal(svDistribution))
                {
                    if ((distMin >= distMean) || (distMax <= distMean))
                        sampErr.SetConstantError("distribution mean must be between min and max");

                }

        }

    return errData.SetupOK(varID, sampErr);
}

void SC_SampVar::IsBeingDeleted(FuncObjC* delObj)
{
    if (userCDFInputObjRef.objRef == delObj)
        {
            userCDFInputObjRef = FuncObjRef();
            userCDFDC = 0;
        }
    if (tableLookupObjRef.objRef == delObj)
        {
            tableLookupObjRef = FuncObjRef();
            tableLookupDC = 0;
        }
}

bool SC_SampVar::UsesAsInput(const FuncObjC* inputObj) const
{
    return  ((IsUserCDF() && (userCDFInputObjRef.objRef == inputObj))) ||
        ((IsTableLookup() && (tableLookupObjRef.objRef == inputObj)));
}

void SC_SampVar::GetMenuStr(char* menuStr, int maxLen)
{
    SC_SetupErr sampErr;
    if (SampleSetupOK(sampErr, "dummy"))
        CopyString(menuStr, GetDistString(), maxLen);
    else
        CopyString(menuStr, "BAD", maxLen);
}


void  SC_SampVar::LocalCopy(const SC_SampVar& a)
{
    svDistribution  = a.svDistribution;
    distMin         = a.distMin;
    distMax         = a.distMax;
    distMean        = a.distMean;
    distStdDev      = a.distStdDev;
    distLogStdDev   = a.distLogStdDev;
    distPeak        = a.distPeak;

    poissonLambda   = a.poissonLambda ;
    weibullShape    = a.weibullShape;
    weibullScale    = a.weibullScale;
    betaP           = a.betaP;
    betaQ           = a.betaQ;

    userCDFInputObjRef = a.userCDFInputObjRef;

    tableLookupObjRef  = a.tableLookupObjRef;
    tableLookupColumn  = a.tableLookupColumn;
}


double SC_SampVar::GetDistMin(const double minR) const
{
    if (UsesMinMax())
        return distMin;

    if (IsUserCDF())
        {
            if (userCDFDC == 0)
                GenAppInternalError("SC_SampVar::GetMinVarValue::userCDF");
            return userCDFDC->GetDataVal(minR);
        }

    if (IsTableLookup())
        {
            const SC_DoubleArray& colVal = CheckTable("SC_SampVar::GetMinVarValue");
            double minVal = colVal[0];
            for (int i = 1; i < colVal.Size(); i++)
                {
                    if (colVal[i] < minVal)
                        minVal = colVal[i];
                }

            return minVal;
        }


    // Poisson & Weibull TODO
    if (IsPoisson())
        {
            return PoissonSampleOnce(minR);
        }

    if (IsWeibull())
        {
            return WeibullSampleOnce(minR);
        }

    // returns InvExp(0.001) or the 0.1 percentile
    if (svDistribution == svdExponential)
        return distMean / 1000;

    // based on normal - 3 std dev
    if ((svDistribution == svdNormal) || (svDistribution == svdStudentT))
        return distMean - stdDevMultiplier * distStdDev;

    // must be lognormal
    if (!IsPositive(distMean))
        {
            return nullReal;
        }

    double logMean = log10(distMean);
    logMean -= stdDevMultiplier * distStdDev;

    return InvLgt(logMean);
}

double SC_SampVar::GetDistMax(const double maxR) const
{
    if (UsesMinMax())
        return distMax;

    if (IsUserCDF())
        {
            if (userCDFDC == 0)
                GenAppInternalError("SC_SampVar::GetMaxVarValue");
            return userCDFDC->GetDataVal(maxR);
        }

    if (IsTableLookup())
        {
            const SC_DoubleArray& colVal = CheckTable("SC_SampVar::GetDistMax");
            double maxVal = colVal[0];
            for (int i = 1; i < colVal.Size(); i++)
                {
                    if (colVal[i] > maxVal)
                        maxVal = colVal[i];
                }

            return maxVal;
        }

    if (IsPoisson())
        {
            return PoissonSampleOnce(maxR);
        }

    if (IsWeibull())
        {
            return WeibullSampleOnce(maxR);
        }

    // returns InvExp(0.999) or the 99.9 percentile
    if (svDistribution == svdExponential)
        return distMean * log(1000.0);

    // based on normal +3 std dev
    if ((svDistribution == svdNormal) || (svDistribution == svdStudentT))
        return distMean + stdDevMultiplier * distStdDev;

    // must be lognormal
    if (!IsPositive(distMean))
        return nullReal;

    double logMean = log10(distMean);
    logMean += stdDevMultiplier * distStdDev;

    return InvLgt(logMean);
}

double  SC_SampVar::GetMean() const
{
    switch (svDistribution) {
    case svdNormal : case svdTruncNormal : case svdStudentT :{
        return distMean;
    }
    case svdLogNormal : case svdTruncLogNormal : {
        return exp (log(distMean) + distLogStdDev * distLogStdDev / 2.0);
    }
    case svdUniform : {
        return (distMin + distMax) / 2.0;
    }
    case svdLogUniform : {
        //ned           return exp((log(distMin) + log(distMax)) / 2.0);
        //ned           commented the previous out -- it is the equation for median not mean

        return (distMax-distMin)/(log(distMax)-log(distMin));
    }
    case svdTriangular : {
        return (distMin + distPeak + distMax) / 3.0;
    }
    case svdLogTriangular : {
        double a = log(distMin);
        double b = log(distPeak);
        double c = log(distMax);
        return exp((a + b + c) / 3.0);  //ned   this doesn't look right to me
                                        //      this should require a numerical integration
    }
    case svdExponential : {
        return distMean;
    }
    case svdPoisson: {
        return poissonLambda;
    }
    case svdWeibull : {
        return weibullScale * Gamma(1.0 + 1.0 / weibullShape);
    }
    case svdBeta : {
        return distMin + (distMax - distMin) * (betaP / (betaP + betaQ));
    }
    case svdUserCDF : {
        if (userCDFDC == 0)
            GenAppInternalError("SC_SampVar::GetMean-UserCDF");

        // kluge approach - take mean of 100 samples through dist
        double mean = 0.0;
        int nok = 0;
        for (double pval = 0.0; pval <= 1.00; pval += 0.01)
            {
                double resVal = userCDFDC->GetDataVal(pval, true);
                if (!RealIsNull(resVal))
                    {
                        mean += resVal;
                        nok++;
                    }
            }

        if (nok == 0)
            {
                GenAppErrorMsg("SC_SampVar::GetMean", "UserCDF - no results in 100 pvals ??");
                return userCDFDC->GetLimitsMean(0.0, 1.0); // this is wrong
            }
        return mean / double(nok);

        //                      return userCDFDC->GetLimitsMean(0.0, 1.0);  this is wrong
    }
    case svdTableLookup : {
        const SC_DoubleArray& colVal = CheckTable("SC_SampVar::GetMean");
        double meanVal = 0.0;
        for (int i = 0; i < colVal.Size(); i++)
            meanVal += colVal[i];
        return meanVal / double(colVal.Size());
    }
    default : ;
    }

    GenAppInternalError("SC_SampVar::GetMean");

    return 0.0;  // to keep the compiler happy
}

double  SC_SampVar::GetMedian() const
{
    const double medR = 0.5;

    switch (svDistribution) {
    case svdNormal : case svdTruncNormal : case svdStudentT : {
        return distMean;
    }
    case svdLogNormal : case svdTruncLogNormal : {
        return distMean;
    }
    case svdUniform : {
        return (distMin + distMax) / 2.0;
    }
    case svdLogUniform : {
        return exp((log(distMax)+log(distMin))/2);
    }
    case svdTriangular : {
        double a = distMin;
        double b = distPeak;
        double c = distMax;
        double med = 0.0;

        if (b >= (a + c) / 2.0)
            med = a + sqrt((c-a)*(b-a)/2.0);
        else if (b <= (a+c)/2)
            med = a + sqrt((c-a)*(c-b)/2.0);
        else if (b = a)
            med = (c-a)/sqrt(2.0);
        else if (b = c)
            med = a - (c-a)/sqrt(2.0);
        else
            med = -999; //for debugging

        return med;
    }
    case svdLogTriangular : {

        double a = log(distMin);
        double b = log(distPeak);
        double c = log(distMax);

        double c1 = c - a;
        double c2 = (b - a) / c1;

        double resX;
        if (medR <= c2)
            resX = a + sqrt(medR * c1 * (b - a));
        else
            resX = c - sqrt((1.0 - medR) * c1 * (c - b));

        resX = exp(resX);

        return resX;
    }

    case svdExponential: {

        return distMean*log(2.0);
    }

    case svdPoisson: {
        //ned   apparently there is no elegant way to calculate the median for the Poisson distribution
        //              so this is brute force.
        return PoissonSampleOnce(medR);
    }
    case svdWeibull : {
        return WeibullSampleOnce(medR);
    }
    case svdBeta : {
        int stepSize = 100;
        SC_DoubleArray betaX, betaFX;
        MakeBetaTable(stepSize, betaX, betaFX);

        int startIndx = 0;
        bool linInterp = true;
        double frac = InterpProb(medR, betaX, betaFX, startIndx, linInterp);
        return (distMax - distMin) * frac + distMin;
    }
    case svdUserCDF : {
        if (userCDFDC == 0)
            GenAppInternalError("SC_SampVar::GetMedian-UserCDF");
        return userCDFDC->GetDataVal(medR, false);
    }
    case svdTableLookup : {
        // ugly & brute force but
        SC_DoubleArray meanCol(CheckTable("SC_SampVar::GetMedian"));
        if (!meanCol.Sort(true))
            GenAppInternalError("SC_SampVar::GetMedian::Stack overflow on sort");

        int meanIndx = meanCol.Size() / 2;
        return meanCol[meanIndx];
    }

    default : ;
    }

    GenAppInternalError("SC_SampVar::GetMedian");

    return 0.0;  // to keep the compiler happy

}


double  SC_SampVar::GetStdDev() const
{
    //TODO - complete
    switch (svDistribution) {
    case svdNormal : case svdTruncNormal : case svdStudentT :{
        return distStdDev;
    }
    case svdLogNormal : case svdTruncLogNormal : {
        return sqrt(exp (2.0 * log(distMean) + 2.0 * distLogStdDev * distLogStdDev)
                    - exp (2.0 * log(distMean) + distLogStdDev * distLogStdDev));
    }
    case svdUniform : {
        return (distMax - distMin) / (2.0 * sqrt(3.0));
    }
    case svdLogUniform : {
        //??
        GenAppErrorMsg("SC_SampVar::GetStdDev", "LogUniform not currently supported");
        return 0.0;
    }
    case svdTriangular : {
        return sqrt((Sqr(distMin) + Sqr(distPeak) + Sqr(distMax)
                     - distMin * distMax - distMin * distPeak  - distMax * distPeak) / 18.0);
    }
    case svdLogTriangular : {
        //??
        GenAppErrorMsg("SC_SampVar::GetStdDev", "LogTriangular not currently supported");
        return 0.0;
    }
    case svdExponential : {
        return distMean;  //mean = 1/lambda, variance = 1/lambda^2, ergo stdev = 1/lambda = mean
    }
    case svdPoisson: {
        return sqrt(poissonLambda);  //mean = variance = poissonLambda
    }
    case svdWeibull : {
        return weibullScale * sqrt(Gamma(1.0 + 2.0 / weibullShape));
    }
    case svdBeta : {
        // not certain about first part (distMin + (distMax - distMin)
        //?? return distMin + (distMax - distMin) * sqrt(betaP * betaQ / (Sqr(betaP + betaQ) * (betaP + betaQ + 1)));
        GenAppErrorMsg("SC_SampVar::GetStdDev", "Beta not currently supported");
        return 0.0;
    }
    case svdUserCDF : {
        /*if (userCDFDC == 0)
          GenAppInternalError("SC_SampVar::GetStdDev-UserCDF");
          return userCDFDC->GetLimitsMean(0.0, 1.0);*/

        // kluge approach - take std dev of 100 samples through dist
        const double mean = GetMean();
        int nok = 0;
        double var = 0.0;
        for (double pval = 0.0; pval <= 1.00; pval += 0.01)
            {
                double resVal = userCDFDC->GetDataVal(pval, true);
                if (!RealIsNull(resVal))
                    {
                        resVal -= mean;
                        var += resVal * resVal;
                        nok++;
                    }
            }

        if (nok < 2)
            {
                GenAppErrorMsg("SC_SampVar::GetStdDev", "UserCDF - no results in 100 pvals ??");
                return 0.0;
            }

        var /= double(nok - 1);
        return sqrt(var);
    }
    case svdTableLookup : {
        const SC_DoubleArray& colVal = CheckTable("SC_SampVar::GetStdDev");
        double mean = GetMean();
        double stdDevVal = 0.0;
        for (int i = 0; i < colVal.Size(); i++)
            stdDevVal += Sqr(colVal[i] - mean);
        return sqrt(stdDevVal / double(colVal.Size()));
    }
    default : ;
    }

    GenAppInternalError("SC_SampVar::GetStdDev");

    return 0.0;  // to keep the compiler happy
}

double  SC_SampVar::GetLogMean() const
{
    //same as GetMean, only it returns the log mean if the distribution is log
    switch (svDistribution) {

    case svdLogNormal : case svdTruncLogNormal : {
        return log(distMean);
    }
    case svdLogUniform : {
        return (log(distMin) + log(distMax)) / 2.0;
    }
    case svdLogTriangular : {
        return (log(distMin) + log(distPeak) + log(distMax)) / 3.0;
    }
    default :
        return GetMean();
    }

    GenAppInternalError("SC_SampVar::GetLogMean");

    return 0.0;  // to keep the compiler happy
}

double  SC_SampVar::GetLogStdDev() const
{
    //same as GetStdDev, only it returns the log std dev if the distribution is log
    switch (svDistribution) {
    case svdLogNormal : case svdTruncLogNormal : {
        return distLogStdDev;
    }
    case svdLogUniform : {
        return (log(distMax) - log(distMin)) / (2.0 * sqrt(3.0));
    }
    case svdLogTriangular : {
        double logDistMin = log(distMin);
        double logDistPeak = log(distPeak);
        double logDistMax = log(distMax);
        return sqrt((Sqr(logDistMin) + Sqr(logDistPeak) + Sqr(logDistMax)
                     - logDistMin * logDistMax - logDistMin * logDistPeak  - logDistMax * logDistPeak) / 18.0);
    }

    default :
        return GetStdDev();
    }

    GenAppInternalError("SC_SampVar::GetLogStdDev");

    return 0.0;  // to keep the compiler happy
}

// ********************************************************** sampling stuff


void SC_SampVar::Sample(SC_Random&              randGen,
                        int             nsamples,
                        bool    stratify,
                        SC_DoubleArray&  x) const
{
    currRGen = &randGen;

    x.AllocAndSetSize(nsamples);
    switch (svDistribution)
        {
        case svdNormal :  case svdTruncNormal : case svdLogNormal :  case svdTruncLogNormal : {
            NormalSample(nsamples, stratify, x);
            break;
        }
        case svdUniform : case svdLogUniform : {
            UniformSample(nsamples, stratify, x);
            break;
        }
        case svdTriangular : case svdLogTriangular : {
            TriangleSample(nsamples, stratify, x);
            break;
        }

        case svdExponential : {
            ExponentialSample(nsamples, stratify, x);
            break;
        }

        case svdPoisson: {
            PoissonSample(nsamples, stratify, x);
            break;
        }

        case svdWeibull: {
            WeibullSample(nsamples, stratify, x);
            break;
        }
        case svdBeta: {
            BetaSample(nsamples, stratify, x);
            break;
        }
        case svdStudentT: {
            StudentTSample(nsamples, stratify, x);
            break;
        }
        case svdUserCDF: {
            UserCDFSample(nsamples, stratify, x);
            break;
        }
        case svdTableLookup : {
            TableLookupSample(nsamples, x);
            randGen.RandomMix(x);
            break;
        }
        }
}


double SC_SampVar::AreaR(const double& probInc,
                         const double& startPt,
                         const double& lowerB,
                         const double& upperB) const
{
    while (true)
        {
            double r = probInc * currRGen->Random() + startPt;
            if ((r > lowerB) && (r < upperB))
                return r;
        }

}

double SC_SampVar::SetProbInc(int          nsamples,
                              bool         stratify,
                              const double baseProbInc) const
{
    if (stratify)
        return baseProbInc / double(nsamples);
    return baseProbInc;
}

double SC_SampVar::Norm(double x)const
{
    return 0.5*(Erfc(-x/sqrt(2.0)));
}

void SC_SampVar::NormalSample(int              nsamples,
                              bool             stratify,
                              SC_DoubleArray&  x) const
{
    double pmu = distMean;
    double sig = distStdDev;
    double minZ = samplRMin;
    double maxZ = samplRMax;
    double rMin = minZ;
    double rMax = maxZ;
    bool logSamp = (svDistribution == svdLogNormal) || (svDistribution == svdTruncLogNormal);
    if (logSamp)
        {
            sig = distLogStdDev;
            pmu = log(pmu);
        }

    double probInc = SetProbInc(nsamples, stratify);
    double startPt = 0.0;

    if (IsTruncNormal(svDistribution))
        {
            minZ = distMin;
            maxZ = distMax;
            if (logSamp)
                {
                    minZ = log(minZ);
                    maxZ = log(maxZ);
                }
            minZ = (minZ-pmu)/sig;
            maxZ = (maxZ-pmu)/sig;

            rMin = Norm(minZ);
            rMax = Norm(maxZ);

            startPt = rMin;
            probInc = rMax-rMin;
            if (stratify)
                probInc /= double(nsamples);

        }

    // Will not allow a sampled area outside the range 4E-36 to 0.9999999.
    // This helps protect the function IERFC--called by FINVNOR.
    // If ABS(sampled value) is less than sample_rmin, replace with 0.

    for (int i = 0; i < nsamples; i++)
        {
            double r = AreaR(probInc, startPt, rMin, rMax);
            double resX = NormInv(r) * sig + pmu;
            if (logSamp)
                resX = exp(resX);
            if (fabs(resX) < minZ)
                resX = 0.0;
            x[i] = resX;

            if (stratify)
                startPt += probInc;
        }

}

void SC_SampVar::TriangleSample(int             nsamples,
                                bool            stratify,
                                SC_DoubleArray& x) const
{
    double a = distMin;
    double b = distPeak;
    double c = distMax;

    bool logSamp = (svDistribution == svdLogTriangular);
    if (logSamp)
        {
            a = log(a);
            b = log(b);
            c = log(c);
        }

    double probInc = SetProbInc(nsamples, stratify);
    double startPt = 0.0;


    double c1 = c - a;
    double c2 = (b - a) / c1;
    for (int i = 0; i < nsamples; i++)
        {
            double r = AreaR(probInc, startPt, 0.0, 1.0);
            double resX;
            if (r <= c2)
                {
                    resX = a + sqrt(r * c1 * (b - a));
                }
            else
                {
                    resX = c - sqrt((1.0 - r) * c1 * (c - b));
                }

            if (logSamp)
                resX = exp(resX);

            if (fabs(resX) < samplRMin)
                resX = 0.0;

            x[i] = resX;

            if (stratify)
                startPt += probInc;
        }


}

void SC_SampVar::UniformSample(int              nsamples,
                               bool             stratify,
                               SC_DoubleArray&  x) const
{
    double a = distMin;
    double b = distMax;
    bool logSamp = (svDistribution == svdLogUniform);
    if (logSamp)
        {
            a = log(a);
            b = log(b);
        }

    double probInc = SetProbInc(nsamples, stratify, b - a);
    double startPt = a;

    for (int i = 0; i < nsamples; i++)
        {
            double resX = AreaR(probInc, startPt, a, b);

            if (logSamp)
                resX = exp(resX);

            if (fabs(resX) < samplRMin)
                resX = 0.0;

            x[i] = resX;

            if (stratify)
                startPt += probInc;
        }
}


void  SC_SampVar::ExponentialSample(int              nsamples,
                                    bool             stratify,
                                    SC_DoubleArray&  x) const
{
    double probInc = SetProbInc(nsamples, stratify);
    double startPt = 0.0;

    for (int i = 0; i < nsamples; i++)
        {
            double r = AreaR(probInc, startPt, 0, 1);
            double resX = - distMean * log(1.0 - r);
            x[i] = resX;

            if (stratify)
                startPt += probInc;
        }

}


//      VB code supplied by Dennis Longsine -- 20 Jan 03

static  const double minProb = 0.0000001;
static  const double maxProb = 1.0 - minProb;

void SC_SampVar::PoissonSample(int              nsamples,
                               bool             stratify,
                               SC_DoubleArray&  x) const
{
    // Generate a discrete CDF based on parameter Lambda.  The coding assumes
    //  that Lambda <= 1e6.  Note that this max(Lambda), MinProb, and MaxTable are
    //  dependent.  If any one changes, the others may need to be changed.
    //  For example, if Lambda = 1e6, then the second loop below should terminate
    //  when n = 1,004,948 and StackNdx = 10,144.  If MinProb is replaced with a smaller
    //  value or Lambda is allowed to be larger, both the max n and StackNdx will
    //  increase.  The latter is bounded by MaxTable, and the former (although
    //  not explicitly limited) may cause an infinite loop leading to long-integer
    //  overflow.

    /*
      MinProb = 0.0000001
      MaxTable = 10150

      MaxProb = 1# - MinProb
      LogLambda = Log(Lambda)
      LogFactorial = 0#
      N = 0
      Sum = Exp(-Lambda)
    */

    double logLambda = log(poissonLambda);
    double logFactorial = 0.0;

    int n = 0;
    double sum = exp(-poissonLambda);

    /*
      Do While Sum < MinProb
      N = N + 1
      LogFactorial = LogFactorial + Log(N)
      Probn = N * LogLambda - Lambda - LogFactorial
      Probn = Exp(Probn)
      Sum = Sum + Probn
      Loop
    */

    while (sum < minProb) {
        n++;
        logFactorial += log(double(n));
        sum += exp(double(n) * logLambda - poissonLambda - logFactorial);
    }

    /* ReDim PoiTable(1 To MaxTable, 1 To 2) As Single
       StackNdx = 1
       PoiTable(StackNdx, 1) = N
       PoiTable(StackNdx, 2) = Sum
    */

    //      const int maxTable = 10150;
    const int maxTable = 10250;  // for 1E6 require
    SC_DoubleArray poiTableSum(maxTable);
    SC_IntArray     poiTableN(maxTable);
    int stackNdx = 0;  // 0 array start

    poiTableSum[0] = sum;
    poiTableN[0] = n;

    /*
      Do While Sum <= MaxProb
      N = N + 1
      LogFactorial = LogFactorial + Log(N)
      Probn = N * LogLambda - Lambda - LogFactorial
      Probn = Exp(Probn)
      Sum = Sum + Probn
      StackNdx = StackNdx + 1
      PoiTable(StackNdx, 1) = N
      PoiTable(StackNdx, 2) = Sum
      Loop
      PoiTable(StackNdx, 2) = 1#
    */

    while (sum < maxProb) {
        n++;
        logFactorial += log(double(n));
        sum += exp(double(n) * logLambda - poissonLambda - logFactorial);

        stackNdx++;
        poiTableN[stackNdx] = n;
        poiTableSum[stackNdx] = sum;
    }
    poiTableSum[stackNdx] = 1.0;
    poiTableSum.SetSize(stackNdx + 1);
    poiTableN.SetSize(stackNdx + 1);


    double probInc = SetProbInc(nsamples, stratify);
    double startPt = 0.0;
    int      startIndx = 0;
    for (int i = 0; i < nsamples; i++)
        {
            double r = AreaR(probInc, startPt, minProb, maxProb);
            x[i] = double(GetProbNdx(r, poiTableN, poiTableSum, startIndx));

            if (stratify)
                startPt += probInc;
            else
                startIndx = 0;
        }

    /*
      StartNdx = 1
      Select Case SampleOpt
      Case lhsSAMPLELHS
      LeftEdge = 0#
      Increment = 1# / CDbl(SampleSize)
      For I = 1 To SampleSize
      R = LHSCumProb(I, Increment, LeftEdge)
      SampleVals(I) = GetProbNdx(R, PoiTable(), StackNdx, StartNdx)
      LeftEdge = LeftEdge + Increment
      Next I

      Case lhsSAMPLERandom
      For I = 1 To SampleSize
      R = RestrictedMonteRnd()
      SampleVals(I) = GetProbNdx(R, PoiTable(), StackNdx, StartNdx)
      StartNdx = 1
      Next I
      End Select
    */

}

/*
  Private Function GetProbNdx(R As Single, LookUpTable() As Single, NumEntries As Integer, StartNdx As Integer) As Single

  '  Use bisection to find adjacent indexes in array LookUpTable that surround
  '  cumulative probability R.  Return the value in the table at the upper index.
  '  Note that StartNdx is also updated and returned.

  On Error GoTo VBErrGetProbNdx

  Dim StopNdx    As Integer
  Dim ReturnNdx  As Integer
  Dim I          As Integer
  Dim sMessage   As String

  If R <= LookUpTable(StartNdx, 2) Then
  ReturnNdx = StartNdx
  Else
  StopNdx = NumEntries
  If StartNdx + 1 >= StopNdx Then
  StartNdx = StopNdx - 2
  If StartNdx < 1 Then
  StartNdx = 1
  End If
  End If

  Do While StartNdx + 1 < StopNdx
  I = StartNdx + Int((StopNdx - StartNdx) / 2)
  If R <= LookUpTable(I, 2) Then
  StopNdx = I
  Else
  StartNdx = I
  End If
  Loop
  ReturnNdx = StopNdx
  End If
  StartNdx = ReturnNdx
  GetProbNdx = LookUpTable(ReturnNdx, 1)

  GetProbNdxExit:
  Exit Function

  VBErrGetProbNdx:
  Select Case Err.Number
  Case Else
  sMessage = "Error in GetProbNdx." & vbCrLf
  End Select

  sMessage = sMessage & Err.Description
  Err.Raise Err.Number, "LHS Class Method GetProbNdx", sMessage
  GoTo GetProbNdxExit

  End Function
*/

int     SC_SampVar::GetProbNdx(const double&          r,
                               const SC_IntArray&     tableN,
                               const SC_DoubleArray&  tableP,
                               int&                   startIndx) const
{

    //  Use bisection to find adjacent indexes in array LookUpTable that surround
    //  cumulative probability R.  Return the value in the table at the upper index.
    //  Note that StartNdx is also updated and returned.

    //   If R <= LookUpTable(StartNdx, 2) Then
    //      ReturnNdx = StartNdx

    if (r < tableP[startIndx])
        return tableN[startIndx];

    /*      StopNdx = NumEntries
            If StartNdx + 1 >= StopNdx Then
            StartNdx = StopNdx - 2
            If StartNdx < 1 Then
            StartNdx = 1
            End If
            End If
    */
    int stopIndx = tableN.UpperBound();
    if ((startIndx + 1) >= stopIndx)
        {
            startIndx = stopIndx - 2;
            if (startIndx < 0)
                startIndx = 0;
        }

    /*      Do While StartNdx + 1 < StopNdx
            I = StartNdx + Int((StopNdx - StartNdx) / 2)
            If R <= LookUpTable(I, 2) Then
            StopNdx = I
            Else
            StartNdx = I
            End If
            Loop
    */

    while ((startIndx + 1) < stopIndx) {
        int i = startIndx + (stopIndx - startIndx) / 2;
        if (r <= tableP[i])
            stopIndx  = i;
        else
            startIndx = i;
    }

    startIndx = stopIndx;
    return tableN[startIndx];
}


void SC_SampVar::WeibullSample(int              nsamples,
                               bool             stratify,
                               SC_DoubleArray&  x) const
{
    /*
      fWeibShape = 1# / fDistData(1)  'Invert shape and take log of scale
      fWeibScale = Log(fDistData(2))  'for efficiency below

      Select Case SampleOpt
      Case lhsSAMPLELHS
      dLeftEdge = 0#
      dIncrement = 1# / CDbl(SampleSize)
      For I = 1 To SampleSize
      fR = LHSCumProb(I, dIncrement, dLeftEdge)
      fLogWeib = fWeibShape * Log(-Log(1 - fR)) - fWeibScale
      fSampleVals(I) = Exp(fLogWeib)
      dLeftEdge = dLeftEdge + dIncrement
      Next I

      Case lhsSAMPLERandom
      For I = 1 To SampleSize
      fR = RestrictedMonteRnd()
      fLogWeib = fWeibShape * Log(-Log(1 - fR)) - fWeibScale
      fSampleVals(I) = Exp(fLogWeib)
      Next I
      End Select
    */

    double invWeibShape = 1.0 / weibullShape; // Invert shape and take log of scale
    double logWeibScale = log(weibullScale); // for efficiency below

    double probInc = SetProbInc(nsamples, stratify);
    double startPt = 0.0;
    for (int i = 0; i < nsamples; i++)
        {
            double r = AreaR(probInc, startPt, minProb, maxProb);

            double logWeib = invWeibShape * log(-log(1.0 - r)) + logWeibScale;
            x[i] = exp(logWeib);

            if (stratify)
                startPt += probInc;
        }
}

void SC_SampVar::BetaSample(int             nsamples,
                            bool            stratify,
                            SC_DoubleArray& x) const
{
    /*
      A = CDbl(fDistData(1))
      B = CDbl(fDistData(2))
      Betap = CDbl(fDistData(3))
      Betaq = CDbl(fDistData(4))
      StepSize = 100             'Borrowed from Wyss, et al

      ' Generate a discrete CDF based on parameters p and q.
      MakeBetaTable Betap, Betaq, StepSize, BetaTable()
    */

    int stepSize = 100;
    SC_DoubleArray betaX, betaFX;
    MakeBetaTable(stepSize, betaX, betaFX);

    /* NumPairs = SafeUbound(BetaTable(), 1)
       StartNdx = 1
       LinInterp = True
    */
    int startIndx = 0;
    bool linInterp = true;

    double probInc = SetProbInc(nsamples, stratify);
    double startPt = 0.0;
    for (int i = 0; i < nsamples; i++)
        {
            double r = AreaR(probInc, startPt, minProb, maxProb);

            double frac = InterpProb(r, betaX, betaFX, startIndx, linInterp);
            x[i] = (distMax - distMin) * frac + distMin;
            if (stratify)
                startPt += probInc;
            else
                startIndx = 0;
        }
}

void SC_SampVar::StudentTSample(int                             nsamples,
                                bool                stratify,
                                SC_DoubleArray& x) const
{
    /*
      student t = 1 - 0.5*BetaI(v/s, 0.5, v/(v+t^2)
      where   BetaI is the incomplete beta function (defined in UReal)
      v is the degrees of freedom (nsamples-1)
      t is the t statistic
      as nsamples increases, the student t approaches the normal distribution
      consequently:  for      nsamples<500 follow an approach similar to Beta
      nsamples>=500 use the normal sampling routine

    */

    if (nsamples >= 500)
        {       //normal sampling

            NormalSample(nsamples, stratify, x);
        }
    else
        { //Beta approach

            degF = nsamples - 1;
            double pmu = distMean;
            double sig = distStdDev;
            bool negativeT = false;

            int stepSize = 100;
            SC_DoubleArray studentt, studentFt;
            MakeStudentTable(stepSize, studentt, studentFt);

            int startIndx = 0;
            bool linInterp = true;

            double probInc = SetProbInc(nsamples, stratify);
            double startPt = 0.0;
            for (int i = 0; i < nsamples; i++)
                {
                    double r = AreaR(probInc, startPt, minProb, maxProb);

                    if (r<0.5)
                        {
                            negativeT = true;
                            r = 1.0-r;
                            startIndx = 0; //since only half a table
                        }
                    else
                        negativeT = false;

                    double frac = InterpProb(r, studentt, studentFt, startIndx, linInterp);
                    if (negativeT)
                        frac = -frac;

                    x[i] = frac * sig + pmu;
                    if (stratify)
                        startPt += probInc;
                    else
                        startIndx = 0;
                }
        }
}

/*
  Private Function InterpProb(R As Single, LookUpTable() As Single, NumEntries As Integer, StartNdx As Integer, LinInterp As Boolean) As Single

  '  Use bisection to find adjacent indexes in array LookUpTable that surround
  '  cumulative probability R.  Interpolate to R using either linear or log
  '  interpolation.  Note that StartNdx is also updated and returned.
  '  Assume
  '            NumEntries > 1
  '            Both columns of LookUpTable are montonically increasing
  '            LookUpTable(1, 2) = 0
  '            LookUpTable(NumEntries, 2) = 1
  '            LookUpTable(StartNdx, 2) <= R < 1, for all R sent in

  On Error GoTo VBErrInterpProb

  Dim StopNdx    As Integer
  Dim ReturnNdx  As Integer
  Dim I          As Integer
  Dim P1         As Single
  Dim P2         As Single
  Dim X1         As Single
  Dim X2         As Single
  Dim ProbRat    As Single
  Dim sMessage   As String

  StopNdx = NumEntries

  If R <= LookUpTable(StartNdx + 1, 2) Then
  StopNdx = StartNdx + 1
  ElseIf R > LookUpTable(StartNdx + 1, 2) Then
  Do While StartNdx + 1 < StopNdx
  I = StartNdx + Int((StopNdx - StartNdx) / 2)
  If R < LookUpTable(I, 2) Then
  StopNdx = I
  Else
  StartNdx = I
  End If
  Loop
  End If

  P1 = LookUpTable(StartNdx, 2)
  P2 = LookUpTable(StopNdx, 2)
  X1 = LookUpTable(StartNdx, 1)
  X2 = LookUpTable(StopNdx, 1)
  ProbRat = (R - P1) / (P2 - P1)
  If LinInterp Then
  InterpProb = (X2 - X1) * ProbRat + X1
  Else
  InterpProb = Exp(Log(X2 / X1) * ProbRat + Log(X1))
  End If

  InterpProbExit:
  Exit Function

  VBErrInterpProb:
  Select Case Err.Number
  Case Else
  sMessage = "Error in InterpProb." & vbCrLf
  End Select

  sMessage = sMessage & Err.Description
  Err.Raise Err.Number, "LHS Class Method InterpProb", sMessage
  GoTo InterpProbExit

  End Function
*/

double SC_SampVar::InterpProb(const double&              r,
                              const SC_DoubleArray& tableX,
                              const SC_DoubleArray& tableFX,
                              int&                    startIndx,
                              bool                    linInterp) const
{

    /*
      '  Use bisection to find adjacent indexes in array LookUpTable that surround
      '  cumulative probability R.  Interpolate to R using either linear or log
      '  interpolation.  Note that StartNdx is also updated and returned.
      '  Assume
      '            NumEntries > 1
      '            Both columns of LookUpTable are montonically increasing
      '            LookUpTable(1, 2) = 0
      '            LookUpTable(NumEntries, 2) = 1
      '            LookUpTable(StartNdx, 2) <= R < 1, for all R sent in

      StopNdx = NumEntries

      If R <= LookUpTable(StartNdx + 1, 2) Then
      StopNdx = StartNdx + 1
      ElseIf R > LookUpTable(StartNdx + 1, 2) Then
      Do While StartNdx + 1 < StopNdx
      I = StartNdx + Int((StopNdx - StartNdx) / 2)
      If R < LookUpTable(I, 2) Then
      StopNdx = I
      Else
      StartNdx = I
      End If
      Loop
      End If
    */
    int stopIndx = tableX.UpperBound();
    if (r <= tableFX[startIndx + 1])
        stopIndx = startIndx + 1;
    else
        {
            while ((startIndx + 1) < stopIndx) {
                int i = startIndx + (stopIndx - startIndx) / 2;
                if (r < tableFX[i])
                    stopIndx = i;
                else
                    startIndx = i;
            }
        }

    /*
      P1 = LookUpTable(StartNdx, 2)
      P2 = LookUpTable(StopNdx, 2)
      X1 = LookUpTable(StartNdx, 1)
      X2 = LookUpTable(StopNdx, 1)
      ProbRat = (R - P1) / (P2 - P1)
      If LinInterp Then
      InterpProb = (X2 - X1) * ProbRat + X1
      Else
      InterpProb = Exp(Log(X2 / X1) * ProbRat + Log(X1))
      End If
    */
    double p1 = tableFX[startIndx];
    double p2 = tableFX[stopIndx];
    double x1 = tableX[startIndx];
    double x2 = tableX[stopIndx];

    double probRat = (r - p1) / (p2 - p1);
    if (linInterp)
        return (x2 - x1) * probRat + x1;

    return exp(log(x2/x1) * probRat + log(x1));
}

void SC_SampVar::MakeBetaTable(int                              stepSize,
                               SC_DoubleArray&      betaX,
                               SC_DoubleArray&      betaFX) const
{
    /*
      '  The leading dimension of 500 is borrowed from Wyss, et al.

      ReDim TempBetaTable(1 To 500, 1 To 2) As Single

      I = 1
      TempBetaTable(I, 1) = 0#
      TempBetaTable(I, 2) = 0#
      X = 0#
      Q = 0#
      FofX = 0#
      Delta = 1# / (625# * CDbl(StepSize))
      DeltaX = Delta
      DeltaQ = Delta
    */

    betaX.Alloc(500);
    betaFX.Alloc(500);

    int i = 0;
    betaX[0] = 0.0;
    betaFX[0] = 0.0;

    double x = 0.0;
    double q = 0.0;
    double fofX = 0.0;

    double delta = 1.0 / (625.0 * double(stepSize));
    double deltaX = delta;
    double deltaQ = delta;

    /*
      Do While Q < 1# And X < 1#

    */
    while ((q < 1.0) && (x < 1.0)) {


        /* Select a step size DeltaQ so that the maximum step size is
           ' taken near the center of the distribution (DeltaQ < 1/stepsize)
           ' and progressively smaller steps are taken in the tails.

           If Q > 1# - 8# * Delta Then
           DeltaQ = Delta
           ElseIf Q > 1# - 38# * Delta Then
           DeltaQ = 5# * Delta
           ElseIf Q > 1# - 185# * Delta Then
           DeltaQ = 25# * Delta
           ElseIf Q > 1# - 800# * Delta Then
           DeltaQ = 125# * Delta
           ElseIf Q > 625# * Delta Then
           DeltaQ = 625# * Delta
           ElseIf Q > 125# * Delta Then
           DeltaQ = 125# * Delta
           ElseIf Q > 25# * Delta Then
           DeltaQ = 25# * Delta
           ElseIf Q > 5# * Delta Then
           DeltaQ = 5# * Delta
           Else
           DeltaQ = Delta
           End If
        */

        if (q > (1.0 - 8.0 * delta)) {
            deltaQ = delta;
        }
        else if (q > (1.0 - 38.0 * delta)) {
            deltaQ = 5.0 * delta;
        }
        else if (q > (1.0 - 185.0 * delta)) {
            deltaQ = 25.0 * delta;
        }
        else if (q > (1.0 - 800.0 * delta)) {
            deltaQ = 125.0 * delta;
        }
        else if (q > (625.0 * delta)) {
            deltaQ = 625.0 * delta;
        }
        else if (q > (125.0 * delta)) {
            deltaQ = 125.0 * delta;
        }
        else if (q > (25.0 * delta)) {
            deltaQ = 25.0 * delta;
        }
        else if (q > (5.0 * delta)) {
            deltaQ = 5.0 * delta;
        }
        else
            deltaQ = delta;

        /*
          ' Q is the maximum value of F(X) that will be accepted
          ' XNew is the X-estimate that will produce an acceptable Q

          Q = FofX + DeltaQ
          If Q > 1# Then
          Q = 1#
          End If
          XNew = X + DeltaX
          If XNew > 1 Then
          XNew = 1#
          End If
          If I = 24 Then
          I = I + 1 - 1
          End If

          BetaFct Betap, Betaq, XNew, FofX

        */
        q = fofX + deltaQ;
        if (q > 1.0)
            q = 1.0;

        double xnew = x + deltaX;
        if (xnew > 1.0)
            xnew = 1.0;

        fofX = BetaFact(xnew);

        /*
          ' If the step was too large, try again with a smaller step size.
          ' If the step size gets too small then simply accept the best we
          ' can do and truck on.

          EnlargeStep = True
          Do While FofX > Q
          EnlargeStep = False
          DeltaX = 0.6666667 * DeltaX
          If (X + DeltaX = X) Or (DeltaX < 1E-44) Then
          DeltaX = 4# * DeltaX
          Exit Do
          End If
          XNew = X + DeltaX
          BetaFct Betap, Betaq, XNew, FofX
          Loop
        */
        bool enlargeStep = true;
        while (fofX > q) {
            enlargeStep = false;
            deltaX *= 0.6666667;
            if (RealsAreSame(x + deltaX, x) || (deltaX < 1.0E-44))
                {
                    deltaX *= 4.0;
                    break;
                }
            xnew = x + deltaX;
            fofX = BetaFact(xnew);
        }

        /*
          ' If the step could have been much larger and still produced an
          ' acceptable Q, then select a larger DeltaX for the next iteration

          If (FofX < Q - 0.5 * DeltaQ) And (EnlargeStep) Then
          DeltaX = 1.5 * DeltaX
          If DeltaX > 1# - X Then
          DeltaX = 1# - X
          End If
          End If
          I = I + 1
          TempBetaTable(I, 1) = CSng(XNew)
          TempBetaTable(I, 2) = CSng(FofX)
          X = XNew
        */
        if (enlargeStep && (fofX < (q - 0.5 * deltaQ)))
            {
                deltaX *= 1.5;
                if (deltaX > (1.0 - x))
                    deltaX = 1.0 - x;
            }

        i++;
        betaX[i] = xnew;
        betaFX[i] = fofX;
        x = xnew;
    }

    /*
      ' Adjust the last value to assure that the table will not have duplicate
      ' values in the second column, then add the last value to the table.

      TempBetaTable(I, 1) = 0.05 * TempBetaTable(I - 1, 1) + 0.95 * TempBetaTable(I, 1)
      TempBetaTable(I, 2) = 0.05 * TempBetaTable(I - 1, 2) + 0.95 * TempBetaTable(I, 2)
      I = I + 1
      TempBetaTable(I, 1) = 1#
      TempBetaTable(I, 2) = 1#
    */
    betaX[i] = 0.05 * betaX[i - 1] + 0.95 * betaX[i];
    betaFX[i] = 0.05 * betaFX[i - 1] + 0.95 * betaFX[i];

    i++;
    betaX[i] = 1.0;
    betaFX[i] = 1.0;

    betaX.SetSize(i + 1);
    betaFX.SetSize(i + 1);
}

/*
  Private Sub BetaFct(Betap As Double, Betaq As Double, X As Double, FofX As Double)

  On Error GoTo VBErrBetaFct

  Dim Y(1 To 1)  As Double
  Dim OMX        As Double
  Dim N          As Integer
  Dim sMessage   As String

  If X < 0# Then
  X = 0#
  ElseIf X > 1# Then
  X = 1#
  End If
  OMX = 1# - X
  N = 1
  BetaIncomp X, OMX, Betap, Betaq, N, Y()
  FofX = Y(1)

  BetaFctExit:
  Exit Sub

  VBErrBetaFct:
  Select Case Err.Number
  Case Else
  sMessage = "Error in BetaFctExit." & vbCrLf
  End Select

  sMessage = sMessage & Err.Description
  Err.Raise Err.Number, "LHS Class Method BetaFct", sMessage
  GoTo BetaFctExit

  End Sub
*/

double SC_SampVar::BetaFact(double& x) const
{
    if (x < 0.0)
        x = 0.0;
    else if (x > 1.0)
        x = 1.0;

    // uses existing BetaI -- not Longsine version
    return BetaI(betaP, betaQ, x);
}

void SC_SampVar::MakeStudentTable(int                           stepSize,
                                  SC_DoubleArray&      studentt,
                                  SC_DoubleArray&      Ft) const
{
    /*
      '  Based on MakeBetaTable - table is only for F(t) from 0.5 to 1.0

    */

    studentt.Alloc(500);
    Ft.Alloc(500);

    int i = 0;
    studentt[0] = 0.0;
    Ft[0] = 0.5;

    double t = 0.0;
    double q = 0.5;
    double fofT = 0.5;

    double maxT = 4.0*Power(degF/(degF-2.0), 0.5);  //4*standard deviation

    double deltaT = maxT/stepSize;
    double delta = 0.5 / (500.0 * double(stepSize));;  //only doing half a table
    double deltaQ = delta;

    /*
      Do While Q < 1# And X < 1#

    */
    while ((q < 1.0) && (t < maxT)) {


        /* Select a step size DeltaQ so that the maximum step size is
           ' taken near the center of the distribution (DeltaQ < 1/stepsize)
           ' and progressively smaller steps are taken in the tails.
        */

        if (q > (1.0 - 5.0 * delta)) {
            deltaQ = delta;
        }
        else if (q > (1.0 - 50.0 * delta)) {
            deltaQ = 5.0 * delta;
        }
        else if (q > (1.0 - 250.0 * delta)) {
            deltaQ = 25.0 * delta;
        }
        else if (q > (1.0 - 1250.0 * delta)) {
            deltaQ = 125.0 * delta;
        }
        else
            deltaQ = 500 * delta;  //most of the table is at 0.005 increments


        /*
          ' Q is the maximum value of F(X) that will be accepted
          ' XNew is the X-estimate that will produce an acceptable Q
        */
        q = fofT + deltaQ;
        if (q > 1.0)
            q = 1.0;

        double tnew = t + deltaT;

        fofT = StudentFact(tnew, maxT);

        /*
          ' If the step was too large, try again with a smaller step size.
          ' If the step size gets too small then simply accept the best we
          ' can do and truck on.
        */
        bool enlargeStep = true;
        while (fofT > q) {
            enlargeStep = false;
            deltaT *= 0.6666667;
            if (RealsAreSame(t + deltaT, t) || (deltaT < 1.0E-44))
                {
                    deltaT *= 4.0;
                    break;
                }
            tnew = t + deltaT;
            fofT = StudentFact(tnew, maxT);
        }

        /*
          ' If the step could have been much larger and still produced an
          ' acceptable Q, then select a larger DeltaX for the next iteration
        */
        if (enlargeStep && (fofT < (q - 0.5 * deltaQ)))
            {
                deltaT *= 1.5;
                if (deltaT > (maxT - t))
                    deltaT = maxT - t;
            }

        i++;
        studentt[i] = tnew;
        Ft[i] = fofT;
        t = tnew;
    }

    /*
      ' Adjust the last value to assure that the table will not have duplicate
      ' values in the second column, then add the last value to the table.
    */
    studentt[i] = 0.05 * studentt[i - 1] + 0.95 * studentt[i];
    Ft[i] = 0.05 * Ft[i - 1] + 0.95 * Ft[i];

    i++;
    studentt[i] = maxT;
    Ft[i] = 1.0;

    studentt.SetSize(i + 1);
    Ft.SetSize(i + 1);
}

double SC_SampVar::StudentFact(double& t, double maxT) const
{
    if (t < 0.0)
        t = 0.0;
    else if (t > maxT)
        t = maxT;

    double x = degF/(degF + Sqr(t));
    return 1.0-0.5*BetaI(degF/2.0, 0.5, x);
}

void SC_SampVar::UserCDFSample( int                             nsamples,
                                bool                stratify,
                                SC_DoubleArray& x) const
{
    if (userCDFDC == 0)
        GenAppInternalError("SC_SampVar::UserCDFSample");

    double probInc = SetProbInc(nsamples, stratify);
    double startPt = 0.0;
    for (int i = 0; i < nsamples; i++)
        {
            double r = AreaR(probInc, startPt, 0.0, 1.0);
            x[i] = userCDFDC->GetDataVal(r, false);
            if (stratify)
                startPt += probInc;
        }
}

double SC_SampVar::PoissonSampleOnce(const double& r) const
{
    const double minProb = 0.000001;
    const double maxProb = 1.0 - minProb;

    double logLambda = log(poissonLambda);
    double logFactorial = 0.0;

    int n = 0;
    double sum = exp(-poissonLambda);

    while (sum < minProb) {
        n++;
        logFactorial += log(double(n));
        sum += exp(double(n) * logLambda - poissonLambda - logFactorial);
    }

    const int maxTable = 10250;
    SC_DoubleArray poiTableSum(maxTable);
    SC_IntArray     poiTableN(maxTable);
    int stackNdx = 0;  // 0 array start

    poiTableSum[0] = sum;
    poiTableN[0] = n;

    while (sum < maxProb) {
        n++;
        logFactorial += log(double(n));
        sum += exp(double(n) * logLambda - poissonLambda - logFactorial);

        stackNdx++;
        poiTableN[stackNdx] = n;
        poiTableSum[stackNdx] = sum;
    }
    poiTableSum[stackNdx] = 1.0;
    poiTableSum.SetSize(stackNdx + 1);
    poiTableN.SetSize(stackNdx + 1);

    int      startIndx = 0;
    return double(GetProbNdx(r, poiTableN, poiTableSum, startIndx));

}

double SC_SampVar::WeibullSampleOnce(const double& r) const
{
    double invWeibShape = 1.0 / weibullShape; // Invert shape and take log of scale
    double logWeibScale = log(weibullScale);
    double logWeib = invWeibShape * log(-log(1.0 - r)) + logWeibScale;
    return exp(logWeib);
}

const SC_DoubleArray& SC_SampVar::CheckTable(const char* procName) const
{
    if ((tableLookupDC == 0) || (tableLookupDC->GetNCols() <= tableLookupColumn))
        GenAppInternalError(procName);

    if (tableLookupDC->dataTable[tableLookupColumn].IsEmpty())
        GenAppInternalError(procName);

    return tableLookupDC->dataTable[tableLookupColumn];
}

void SC_SampVar::TableLookupSample( int                                 nsamples,
                                    SC_DoubleArray& x) const
{
    const SC_DoubleArray& colVal = CheckTable("TableLookupSample");

    int sampIndx = 0;
    for (int i = 0; i < nsamples; i++)
        {
            if (sampIndx > colVal.UpperBound())
                sampIndx = 0;
            x[i] = colVal[sampIndx++];
        }
}

