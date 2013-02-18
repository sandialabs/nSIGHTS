///////////////////////////////////////////////////////////////////////////////////
//
// SC_BoolArray.cpp
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

#include <genClass/SC_IntArray.h>
#include <genClass/SC_BoolArray.h>

int SC_BoolArray::GetSetCount() const
{
    int cnt = 0;
    for (int i = 0; i < Size(); i++)
        if (tListData[i])
            cnt++;
    return cnt;
}


int SC_BoolArray::MakeReverseMap(SC_IntArray& reverseMap) const
{
    reverseMap.AllocAndFill(Size(), -1);
    int nOK = 0;
    for (int i = 0; i < Size(); i++)
        if (tListData[i])
            reverseMap[i] = nOK++;

    return nOK;
}

int SC_BoolArray::MakeForwardMap(SC_IntArray& forwardMap) const
{
    forwardMap.AllocAndSetSize(GetSetCount());
    int nOK = 0;
    for (int i = 0; i < Size(); i++)
        if (tListData[i])
            forwardMap[nOK++] = i;

    return nOK;
}

void SC_BoolArray::SetIndexes(const SC_IntArray& indexes, bool setVal)
{
    for (int i = 0; i < indexes.Size(); i++)
        (*this)[indexes[i]] = setVal;
}


void SC_BoolArray::SetIndexArray(SC_IntArray& indexes, bool setVal)
{
    int nset = GetSetCount();
    if (!setVal)
        nset = Size() - nset;
    indexes.Alloc(nset);
    for (int i = 0; i < Size(); i++)
        {
            bool currVal = (*this)[i];
            if (setVal)
                {
                    if (currVal)
                        indexes += i;
                }
            else
                {
                    if (!currVal)
                        indexes += i;
                }
        }
}

bool SC_BoolArray::OneIndexIsSet(const SC_IntArray& indexes) const
{
    for (int i = 0; i < indexes.Size(); i++)
        if ((*this)[indexes[i]])
            return true;
    return false;
}

bool SC_BoolArray::AllIndexesAreSet(const SC_IntArray& indexes) const
{
    if (indexes.IsEmpty())
        return false;

    for (int i = 0; i < indexes.Size(); i++)
        if (!(*this)[indexes[i]])
            return false;
    return true;
}

