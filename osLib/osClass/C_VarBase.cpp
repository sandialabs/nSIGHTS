///////////////////////////////////////////////////////////////////////////////////
//
// C_VarBase.cpp
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
//      base class for different variables types.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <osClass/C_VarBase.h>


VarBase::VarBase(bool& isActiveInit, const char* varIDInit, VarBase*& listHead)
        : isActive(isActiveInit), varShortID(varIDInit), varLongID(varIDInit)
{
    InitCommon(listHead);
}

VarBase::VarBase(bool& isActiveInit, const char* varShortIDInit, const char* varLongIDInit, VarBase*& listHead)
        : isActive(isActiveInit), varShortID(varShortIDInit), varLongID(varLongIDInit)
{
    InitCommon(listHead);
}

void VarBase::InitCommon(VarBase*& listHead)
{
    //  add to linked list
    if (!listHead)
    {
        // first member
        listHead = this;
        prevVB = 0;
    }
    else
    {
        //  find the end of the list
        VarBase* endList = listHead;
        while (endList->nextVB )
            endList = endList->nextVB;

        // set next on cn current end
        endList->nextVB = this;

        //  set prev on this
        prevVB = endList;
    }

    //  new end of list
    nextVB  = 0;
    wasActive = false;
}


void VarBase::RemoveVarBase(VarBase*& listHead)
{
    //  delete from list
    if (prevVB)
        prevVB->nextVB = nextVB;
    else
        listHead = nextVB;

    if (nextVB)
        nextVB->prevVB = prevVB;
}


VarBase* VarBase::GetNextActive() const
{
    for (VarBase* o = this->nextVB; o; o = o->nextVB)
        if (o->isActive)
            return o;

    return 0;
}

VarBase* VarBase::GetFirstActive(VarBase* listHead)
{
    for (VarBase* o = listHead; o; o = o->nextVB)
       if (o->isActive)
            return o;

    return 0;
}

void VarBase::SetWasActive(VarBase* listHead)
{
   for (VarBase* o = listHead; o; o = o->nextVB)
       o->wasActive = o->isActive;
}

int VarBase::GetnActive(VarBase* listHead)
{
    int nFound = 0;
    for (VarBase* o = listHead; o; o = o->nextVB)
        if (o->isActive)
            nFound ++;

    return nFound;
}


void VarBaseArray::GetActiveArray(VarBase* listHead)
{
    AllocAndSetSize(VarBase::GetnActive(listHead));
    int nextOK = 0;
    for (VarBase* o = listHead; o; o = o->nextVB)
        if (o->isActive)
            (*this)[nextOK++] = o;
}


void VarBase::MakeUnitString(char unitStr[], int maxLen)
{
    CopyString(unitStr, "[]", maxLen);
}

void VarBaseArray::GetSortedArray(VarBase* listHead)
{
    GetActiveArray(listHead);
    int nvar = Size();
    for (int i = 0; i < (nvar - 1); i++)
        for (int j = i + 1; j < nvar; j++)
            if (strcmp((*this)[i]->GetShortID(), (*this)[j]->GetShortID()) > 0)
                (*this).Swap(i, j);
}


