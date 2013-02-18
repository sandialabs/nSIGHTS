///////////////////////////////////////////////////////////////////////////////////
//
// C_VaryVarUnits.cpp
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
//      variable that can be multiply valued (suite or range).
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <genUnits/C_UnitListObj.h>

#include <osClass/C_VaryVarUnits.h>

VaryVarUnits::VaryVarUnits(bool& isActiveInit, const char* varIDInit,
                           UnitIndex& unitIndexInit, bool defIsLinear)
        : VaryVar(isActiveInit, varIDInit, defIsLinear), unitIndex(unitIndexInit)
{ }

VaryVarUnits::VaryVarUnits(bool& isActiveInit,
                           const char* varShortIDInit, const char* varLongIDInit,
                           UnitIndex& unitIndexInit, bool defIsLinear)
        : VaryVar(isActiveInit, varShortIDInit, varLongIDInit, defIsLinear), unitIndex(unitIndexInit)
{ }

double  VaryVarUnits::GetFinalVaryVal() const
{
    return unitIndex.MetricToUser(GetVaryVal());
}


VaryVarUnits& VaryVarUnits::operator= (const VaryVarUnits& a)
{
    VaryVar::operator=(a);
    return *this;
}


void VaryVarUnits::MakeUnitString(char unitStr[], int maxLen)
{
    unitIndex.MakeUnitString(unitStr, maxLen);
}


void VaryVarUnits::AddToListing(UnitListObj& listObj) const
{

    if (IsRange())
    {
        listObj.AddUnitReal("   Minimum", UnitReal(unitIndex, rangeMinLim));
        listObj.AddUnitReal("   Maximum", UnitReal(unitIndex, rangeMaxLim));
        listObj.AddStdInt("   # of steps", rangeNStep);
        listObj.AddBoolText("   Stepping type", rangeIsLinear, "Linear", "Log");
    }
    else
    {
        if (suiteValues.IsEmpty())
        {
            listObj.AddStdText("   # of values", "nonse set");
        }
        for (int i = 0; i < suiteValues.Size(); i++)
        {
            char tempStr[80];
            CopyString(tempStr,"Value #", 80);
            ConcatInt(i, tempStr, 2, 80);
            listObj.AddUnitReal(tempStr, UnitReal(unitIndex, suiteValues[i]));
        }
    }
}

