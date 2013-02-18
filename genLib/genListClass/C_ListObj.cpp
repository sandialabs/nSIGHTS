///////////////////////////////////////////////////////////////////////////////////
//
// C_ListObj.cpp
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
//      base class for objects that list data to ListDefC functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genListClass/C_ListObj.h>

ListObjC :: ListObjC(const char* listType, ListDefC& assList)
    : FuncObjC(listType), assocList(&assList)
{
    assList.AddTo(this);
}

ListObjC :: ListObjC(const char* listType) : FuncObjC(listType)
{
    assocList = 0;
}

ListObjC :: ListObjC(const ListObjC& a, ListDefC& assList)
    : FuncObjC(a), assocList(&assList), LSC_Listing(a)
{
    assList.AddTo(this);
    LocalCopy(a);
}


ListObjC :: ~ListObjC ()
{
    // don't care if not on a list
    if (!assocList)
        return;
    assocList->DeleteFrom(this);
}


ListObjC& ListObjC::operator= (const ListObjC& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LSC_Listing::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  ListObjC::LocalCopy(const ListObjC& a)
{
}

void  ListObjC::DoStatusChk()
{
    FuncObjC::DoStatusChk();
}


void ListObjC::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
        DoListing();
}

void ListObjC::BuildListing()
{
    Execute();
}


void ListObjC::DoListing()
{
    InitListingData();
    CreateListing();
    CloseListingData();
}


