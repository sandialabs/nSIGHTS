///////////////////////////////////////////////////////////////////////////////////
//
// C_VaryVar.cpp
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
#include <genClass/C_Common.h>

#include <genListClass/C_ListObj.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osClass/C_VaryVar.h>


bool     VaryControlStatics::varyIsRange     = false;
int      VaryControlStatics::currVaryIndex   = 0;

VarBase* VaryVar::listHead = 0;

VaryVar::VaryVar(bool& isActiveInit, const char* varIDInit, bool defIsLinear)
        : VarBase(isActiveInit, varIDInit, listHead), rangeIsLinear(defIsLinear)
{
    InitCommon();
}

VaryVar::VaryVar(bool& isActiveInit, const char* varShortIDInit, const char* varLongIDInit, bool defIsLinear)
        : VarBase(isActiveInit, varShortIDInit, varLongIDInit, listHead), rangeIsLinear(defIsLinear)
{
    InitCommon();
}

void VaryVar::InitCommon()
{
    // properties
    rangeMinLim     = nullReal;
    rangeMaxLim     = nullReal;
    rangeNStep      = 20;
    suiteValues.Alloc(maxSuiteVal);
    varyOrder = -1;
    varyIndex = -1;
}


VaryVar::~VaryVar()
{
    RemoveVarBase(listHead);
}

VaryVar& VaryVar::operator= (const VaryVar& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


int VaryVar::GetnVary() const
{
    if (IsRange())
        return rangeNStep;

    return suiteValues.Size();
}

double VaryVar::GetVaryVal() const
{
    if ((varyIndex < 0) || (varyIndex >= GetnVary()))
        GenAppInternalError("VaryVar::GetVaryVal");

    if (IsSuite())
        return suiteValues[varyIndex];

    double normVal = double(varyIndex) / double(rangeNStep - 1);

    if (rangeIsLinear)
        return rangeMinLim + normVal * (rangeMaxLim - rangeMinLim);

    double logMin = log10(rangeMinLim);
    return pow(10.0,  logMin + normVal * (log10(rangeMaxLim) - logMin));

}

double VaryVar::GetVaryVal(int inIndex)
{
    varyIndex = inIndex;
    return GetVaryVal();
}


double  VaryVar::GetFinalVaryVal() const
{
    return GetVaryVal();
}


bool VaryVar::VarySetupOK(SC_SetupErr& errData)       // returns true if basic setup is OK
{
    // don't check if not set
    if (!isActive)
        return true;

    SC_SetupErr parErr;

    if (IsRange())
    {
        if (RealIsNull(rangeMinLim) || RealIsNull(rangeMaxLim))
            parErr.SetConstantError("range min and/or max not set");
        else if (rangeMinLim >= rangeMaxLim)
            parErr.SetConstantError("range min must < max");
        else if ((!rangeIsLinear) && (rangeMinLim < stdEps))
            parErr.SetConstantError("range min must > 0 for log stepping");
    }
    else
    {
        if (suiteValues.IsEmpty())
            parErr.SetConstantError("no suite values entered");
    }

    return errData.SetupOK(varShortID, parErr);
}

void VaryVar::GetMenuStr(char* menuStr, int maxLen)
{
    SC_SetupErr varyErr;
    if (VarySetupOK(varyErr))
    {
        IntToString(GetnVary(), menuStr, maxLen);
        ConcatString(menuStr, " vals", maxLen);
        if (IsRange())
            if (rangeIsLinear)
                ConcatString(menuStr, "/Lin", maxLen);
            else
                ConcatString(menuStr, "/Log", maxLen);
    }
    else
        CopyString(menuStr, "BAD", maxLen);
}


void VaryVar::AddToListing(ListObjC& listObj) const
{
    if (IsRange())
    {
        listObj.AddStdReal("   Minimum", rangeMinLim);
        listObj.AddStdReal("   Maximum", rangeMaxLim);
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
            listObj.AddStdReal(tempStr, suiteValues[i]);
        }
    }
}

void VaryVar::LocalCopy(const VaryVar& a)
{
    rangeMinLim     = a.rangeMinLim;
    rangeMaxLim     = a.rangeMaxLim;
    rangeIsLinear   = a.rangeIsLinear;
    rangeNStep      = a.rangeNStep;
    suiteValues     = a.suiteValues;
}


void VaryVar::GetMinMax(double& minVal,
                        double& maxVal) const
{
    if (IsRange())
    {
        minVal = rangeMinLim;
        maxVal = rangeMaxLim;
    }
    else
        suiteValues.CalcMinMax(minVal, maxVal);
}

bool VaryVar::GetDataCoord(DC_DataCoord& coord)
{
    SC_SetupErr dummy;
    if ((!IsRange()) || (!VarySetupOK(dummy)))
        return false;

    coord.Alloc(rangeNStep);
    coord.SetEmpty();

    coord.dataIsLog = !rangeIsLinear;
    coord.SetID(GetShortID());
    double minVal = rangeMinLim;
    double maxVal = rangeMaxLim;

    // set first
    coord += rangeMinLim;

    // fill middle
    if (!rangeIsLinear)
    {
        minVal = log10(minVal);
        maxVal = log10(maxVal);
    }
    double dc = (maxVal - minVal) / (double(rangeNStep - 1));
    double currVal = minVal;
    for (int i = 1; i < (rangeNStep - 1); i++)
    {
        currVal += dc;
        if (rangeIsLinear)
            coord += currVal;
        else
            coord += InvLgt(currVal);
    }

    // avoid round off on last
    coord += rangeMaxLim;

    return true;
}


double VaryVar::GetVarValue() const
{
    return GetVaryVal();
}
double VaryVar::GetMinVarValue() const
{
    double vmin, vmax;
    GetMinMax(vmin, vmax);
    return vmin;
}
double VaryVar::GetMaxVarValue() const
{
    double vmin, vmax;
    GetMinMax(vmin, vmax);
    return vmax;
}


VaryVar* VaryVar::GetNextActive() const
{
    return static_cast<VaryVar*>(VarBase::GetNextActive());
}

VaryVar* VaryVar::GetFirstActive()
{
    return static_cast<VaryVar*>(VarBase::GetFirstActive(listHead));
}


int VaryVar::GetnActive()
{
    return VarBase::GetnActive(listHead);
}

void VaryVar::SetWasActive()
{
    VarBase::SetWasActive(listHead);
}

void VaryVar::ResetAllVaryIndexes()
{
    for (VarBase* o = listHead; o; o = o->nextVB)
        static_cast<VaryVar*>(o)->varyIndex = 0;
}

void VaryVar::GetActiveArray(VaryVarArray& activeArray)
{
    // klugy approach
    VarBaseArray temp;
//  temp.GetActiveArray(listHead);
    temp.GetSortedArray(listHead);
    activeArray.CreateFrom(temp);
}


VaryVarArray::VaryVarArray()
{
    VaryVar::GetActiveArray(*this);
}


void VaryVarArray::CreateFrom(const VarBaseArray& a)
{
    AllocAndSetSize(a.Size());
    for (int i = 0; i < Size(); i++)
        (*this)[i] = static_cast<VaryVar*>(a[i]);
}

