///////////////////////////////////////////////////////////////////////////////////
//
// C_SampVarUnits.cpp
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
//      variable to be sampled.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <osClass/C_SampVarUnits.h>

#include <genUnits/C_UnitListObj.h>

SampVarUnits::SampVarUnits(bool& isActiveInit, int corrGroupInit,
                           const char* varIDInit, UnitIndex& unitIndexInit, bool defIsLinear)
        : SampVar(isActiveInit, corrGroupInit, varIDInit, defIsLinear), unitIndex(unitIndexInit)
{ }

SampVarUnits::SampVarUnits(bool& isActiveInit, int corrGroupInit,
                            const char* varShortIDInit, const char* varLongIDInit,
                            UnitIndex& unitIndexInit, bool defIsLinear)
        : SampVar(isActiveInit, corrGroupInit, varShortIDInit, varLongIDInit, defIsLinear), unitIndex(unitIndexInit)
{ }

SampVarUnits& SampVarUnits::operator=(const SampVarUnits& a)
{
    if (&a != this)
    {
        SampVar::operator=(a);
        unitIndex = a.unitIndex;
    }

    return *this;
}


double SampVarUnits::GetMetricValue(const double& inVal) const
{
    if (!(IsPoisson() || IsUserCDF() || IsTableLookup()))
        return inVal;

    return unitIndex.UserToMetric(inVal);
}

double SampVarUnits::GetMean() const
{
    return GetMetricValue(SampVar::GetMean());
}
double SampVarUnits::GetMedian() const
{
    return GetMetricValue(SampVar::GetMedian());
}
double SampVarUnits::GetStdDev() const
{
    return GetMetricValue(SampVar::GetStdDev());
}
double SampVarUnits::GetLogMean() const
{
    return GetMetricValue(SampVar::GetLogMean());
}
double SampVarUnits::GetLogStdDev() const
{
    return GetMetricValue(SampVar::GetLogStdDev());
}
double SampVarUnits::GetMinVarValue() const
{
    return GetMetricValue(SampVar::GetMinVarValue());
}
double SampVarUnits::GetMaxVarValue() const
{
    return GetMetricValue(SampVar::GetMaxVarValue());
}

double SampVarUnits::GetCurrentValue(int sampleNum) const
{
    return GetMetricValue(SampVar::GetCurrentValue(sampleNum));
}

double SampVarUnits::GetFinalValue(int sampleNum) const
{
    double currValue = SampVar::GetCurrentValue(sampleNum);
    if (IsPoisson() || IsUserCDF() || IsTableLookup())
        return currValue;

    return unitIndex.MetricToUser(currValue);
}

double SampVarUnits::GetCurrentValue(const double& userValue) const
{
    return GetMetricValue(userValue);
}

void SampVarUnits::MakeUnitString(char unitStr[], int maxLen)
{
    unitIndex.MakeUnitString(unitStr, maxLen);
}

void SampVarUnits::AddToListing(UnitListObj& listObj) const
{
    listObj.AddStdText("   Distribution", GetDistString());

    switch (svDistribution) {
        case svdNormal : {
            listObj.AddUnitReal("   Mean", UnitReal(unitIndex, distMean));
            listObj.AddUnitReal("   Std deviation", UnitReal(unitIndex, distStdDev));
            break;
        }
        case svdLogNormal : {
            listObj.AddUnitReal("   Mean", UnitReal(unitIndex, distMean));
            listObj.AddStdReal("   Log std. deviation", distStdDev);
            break;
        }
        case svdUniform :
        case svdLogUniform : {
            listObj.AddUnitReal("   Minimum", UnitReal(unitIndex, distMin));
            listObj.AddUnitReal("   Maximum", UnitReal(unitIndex, distMax));
            break;
        }
        case svdTriangular :
        case svdLogTriangular : {
            listObj.AddUnitReal("   Minimum", UnitReal(unitIndex, distMin));
            listObj.AddUnitReal("   Peak", UnitReal(unitIndex, distPeak));
            listObj.AddUnitReal("   Maximum", UnitReal(unitIndex, distMax));
            break;                                          //      this should require a numerical integration
        }
        case svdExponential : {
            listObj.AddUnitReal("   Mean", UnitReal(unitIndex, distMean));
            break;
        }
        case svdPoisson: {
            listObj.AddStdReal("   Poisson mean", poissonLambda);
            break;
        }
        case svdWeibull : {
            listObj.AddStdReal("   Weibull shape (alpha)", weibullShape);
            listObj.AddStdReal("   Weibull scale (beta)", weibullScale);
            break;
        }
        case svdBeta : {
            listObj.AddUnitReal("   Minimum", UnitReal(unitIndex, distMin));
            listObj.AddUnitReal("   Maximum", UnitReal(unitIndex, distMax));
             listObj.AddStdReal("   Beta P exponent (alpha)", betaP);
             listObj.AddStdReal("   Beta Q exponent (beta)", betaQ);
            break;
        }
        case svdUserCDF : {
            listObj.AddFuncObjRef("User CDF object", userCDFInputObjRef);
            break;
        }
        case svdTableLookup : {
            listObj.AddFuncObjRef("Table object", tableLookupObjRef);
            listObj.AddStdInt("Table column index", tableLookupColumn);
            break;
        }
        default : ;
    }
}

