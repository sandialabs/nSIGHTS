///////////////////////////////////////////////////////////////////////////////////
//
// SC_BoolArray.h
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

#ifndef SC_BOOLARRAY_H
#define SC_BOOLARRAY_H

#include <genClass/T_SC_Array.h>

class SC_IntArray;

class SC_BoolArray: public T_SC_Array<bool> {

public :
    //  constructors
    SC_BoolArray():T_SC_Array<bool>() {}
    SC_BoolArray(int nAll):T_SC_Array<bool>(nAll) {}
    SC_BoolArray(int nAll, bool initVal):T_SC_Array<bool>(nAll, initVal) {}
    SC_BoolArray(bool* buffer, int bufSize):T_SC_Array<bool>(buffer, bufSize) {}

    //  type specific methods
    int             GetSetCount() const;          // # of true flags

    int             MakeReverseMap(SC_IntArray& reverseMap) const;
    int             MakeForwardMap(SC_IntArray& forwardMap) const;
    void            SetIndexes(const SC_IntArray& indexes, bool setVal = true);
    void            SetIndexArray(SC_IntArray& indexes, bool setVal = true);
    bool            OneIndexIsSet(const SC_IntArray& indexes) const;
    bool            AllIndexesAreSet(const SC_IntArray& indexes) const;

};

#endif // SC_BOOLARRAY_H

