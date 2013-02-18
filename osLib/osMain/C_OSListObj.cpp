///////////////////////////////////////////////////////////////////////////////////
//
// C_OSListObj.cpp
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
//      base class for application modal listings .
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>

#include <osMain/C_OSListObj.h>



OSListObj::OSListObj(const char* inID)
    :UnitListObj(inID)
{
}


void OSListObj::AddOptVar(const char* dataDesc,
                            const OptVarUnits& optData,
                            const SC_RealFormat* overrideFormat)

{
    char tempStr[80];
    MakeString(tempStr, dataDesc, " optimization ID", 80);
    AddStdText(tempStr, optData.GetShortID());

    if (optData.IsActive())
    {
        AddOptVar(optData, overrideFormat);
    }
    else
    {
        AddStdText(dataDesc, "not active ??");
    }

}

void OSListObj::AddOptVar(const OptVarUnits& optData,
                            const SC_RealFormat* overrideFormat)

{
    AddUnitReal("   Minimum value", UnitReal(optData.unitIndex, optData.optMinLim), overrideFormat);
    AddUnitReal("   Maximum value", UnitReal(optData.unitIndex, optData.optMaxLim), overrideFormat);
    AddUnitReal("   Estimate value", UnitReal(optData.unitIndex, optData.optParEstimate), overrideFormat);
    AddBoolText("   Range type", optData.optIsLinear, "Linear", "Log");
    AddStdReal( "   Sigma", optData.optParSigma, SC_SciFormat(5));
}


void OSListObj::AddVaryVar(const char* dataDesc,
                            const VaryVarUnits& varyData,
                            const SC_RealFormat* overrideFormat)

{
    char tempStr[80];
    MakeString(tempStr, dataDesc, " suite/range ID", 80);
    AddStdText(tempStr, varyData.GetShortID());

    if (varyData.IsActive())
    {
        AddVaryVar(varyData, overrideFormat);
    }
    else
    {
        AddStdText(dataDesc, "not active ??");
    }

}

void OSListObj::AddVaryVar(const VaryVarUnits& varyData,
                             const SC_RealFormat* overrideFormat)

{
    if (varyData.IsRange())
    {
        AddUnitReal("   Minimum value", UnitReal(varyData.unitIndex, varyData.rangeMinLim), overrideFormat);
        AddUnitReal("   Maximum value", UnitReal(varyData.unitIndex, varyData.rangeMaxLim), overrideFormat);
        AddBoolText("   Range type", varyData.rangeIsLinear, "Linear", "Log");
        AddStdInt( "   # of steps", varyData.rangeNStep);
    }
    else
    {
        if (varyData.suiteValues.IsEmpty())
            AddStdText("   Values ??", "none set");
        for (int i = 0; i < varyData.suiteValues.Size(); i++)
        {
            char tempStr[40];
            CopyString(tempStr, "   Value #", 40);
            ConcatInt(i, tempStr, 40);
            AddUnitReal(tempStr, UnitReal(varyData.unitIndex, varyData.suiteValues[i]), overrideFormat);
        }
    }
}


void OSListObj::AddSampVar(const char* dataDesc,
                             const SampVarUnits& sampData,
                             const SC_RealFormat* overrideFormat)
{
    char tempStr[80];
    MakeString(tempStr, dataDesc, " sample ID", 80);
    AddStdText(tempStr, sampData.GetShortID());

    if (sampData.IsActive())
    {
        AddSampVar(sampData, overrideFormat);
    }
    else
    {
        AddStdText(dataDesc, "not active ??");
    }
}

void OSListObj::AddSampVar(const SampVarUnits& sampData,
                             const SC_RealFormat* overrideFormat)
{
    switch (sampData.svDistribution) {
        case SampleDistributionBase::svdNormal : {
            AddUnitReal("   Mean", UnitReal(sampData.unitIndex, sampData.distMean), overrideFormat);
            AddUnitReal("   Std. deviation", UnitReal(sampData.unitIndex, sampData.distStdDev), overrideFormat);
            break;
        }
        case SampleDistributionBase::svdLogNormal : {
            AddUnitReal("   Mean", UnitReal(sampData.unitIndex, sampData.distMean), overrideFormat);
            AddStdReal("   Log std. deviation", sampData.distLogStdDev, SC_SciFormat(5));
            break;
        }
        case SampleDistributionBase::svdTriangular :
        case SampleDistributionBase::svdLogTriangular: {
            AddUnitReal("   Peak value", UnitReal(sampData.unitIndex, sampData.distPeak), overrideFormat);
            // intenional fall through
        }
        case SampleDistributionBase::svdUniform :
        case SampleDistributionBase::svdLogUniform : {
            AddUnitReal("   Lower limit", UnitReal(sampData.unitIndex, sampData.distMin), overrideFormat);
            AddUnitReal("   Upper limit", UnitReal(sampData.unitIndex, sampData.distMax), overrideFormat);
            break;
        }
        default : {
            AddStdText("    PA distribution", "no listing yet");
        }
    }
}


