///////////////////////////////////////////////////////////////////////////////////
//
// SC_Array.cpp
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
// DESCRIPTION: Base class for all array classes
//
///////////////////////////////////////////////////////////////////////////////////

#include <typeinfo>

#include <iostream>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/SC_Array.h>

void SC_Array::DeAlloc()
{
    nlistData = 0;
    nlistAlloc = 0;
}
void SC_Array::Alloc(int nAll)
{
    if (!ChkAlloc(nAll))
        throw AllocationError(typeid(*this).name());
}

void SC_Array::SetResizable(int nAlloc)
{
    SetResizable();
    Alloc(nAlloc);
}


void SC_Array::SetSize(int newSize )
{
    if ( newSize > nlistAlloc)
        SC_ArrayError("SetSize()");
    nlistData = newSize;
}

void  SC_Array::AllocAndSetSize(int nAll)
{
    Alloc(nAll);
    nlistData = nAll;
}


void  SC_Array::SC_ArrayError(const char* errStr) const
{
    static char tempStr[80];
    CopyString(tempStr, typeid(*this).name(), 80);
    ConcatString(tempStr, " ", 80);
    ConcatString(tempStr, errStr, 80);
    GenAppInternalError(tempStr);
}

bool SC_Array::BadIndex(int indxToCheck) const
{
    if ((indxToCheck < 0) || (indxToCheck >= nlistAlloc))
        {
            using namespace std;
            cerr << "BadIndex  " << indxToCheck << " Max " << nlistAlloc << " Size " << nlistData << endl;
            return true;
        }

    return false;
}

