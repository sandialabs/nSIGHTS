///////////////////////////////////////////////////////////////////////////////////
//
// DC_VariableArray.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>

#include <objClass/DC_VariableArray.h>

DC_Variable::DC_Variable()
{
    SetToNull(varID);
    varValue = nullReal;
}

DC_Variable :: DC_Variable(const DC_Variable& a)
{
    LocalCopy(a);
}


DC_Variable&  DC_Variable::operator=(const DC_Variable& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void DC_Variable::LocalCopy(const DC_Variable& a)
{
    CopyString(varID, a.varID, varIDLen);
    varValue = a.varValue;
}


void DC_Variable::SetID(const char* inID)
{
    CopyString(varID, inID, varIDLen);
}

bool DC_Variable::IsOK() const
{
    return IsNotNull(varID) && (!RealIsNull(varValue));
}

bool DC_Variable::IsOK(SC_SetupErr& errData) const
{
    if (IsNull(varID))
    {
        errData.SetConstantError("variable ID not set");
        return false;
    }
    if (RealIsNull(varValue))
    {
        errData.SetConstantError("variable value not set");
        return false;
    }
    return true;
}

DC_VariableArray::DC_VariableArray()
{
    SetResizable(128);
}

bool DC_VariableArray::AllOK() const
{
    if (IsEmpty())
        return false;

    for (int i = 0; i < Size(); i++)
        if (!(*this)[i].IsOK())
            return false;
    return true;
}

int DC_VariableArray::GetVarIndex(const char* varID) const
{
    for (int i = 0; i < Size(); i++)
        if (SameUCString((*this)[i].GetID(), varID))
            return i;
    return -1;
}
bool DC_VariableArray::GetValue(const char* varID, double& varVal) const
{
    int currIndex = GetVarIndex(varID);
    if (currIndex < 0)
        return false;

    varVal = (*this)[currIndex].varValue;
    return true;
}


void DC_VariableArray::SortByID()
{
}

void DC_VariableArray::SortByValue()
{
}

void DC_VariableArray::GetIDs(SC_StringArray& ids) const
{
    ids.DeAlloc();
    ids.SetStringLen(DC_Variable::varIDLen);
    ids.Alloc(Size());
    for ( int i = 0; i < Size(); i++)
        ids += (*this)[i].varID;
}

