///////////////////////////////////////////////////////////////////////////////////
//
// SC_StringArray.h
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

#ifndef SC_STRINGARRAY_H
#define SC_STRINGARRAY_H

#include <genClass/SC_Array.h>
#include <genClass/SC_IntArray.h>

class SC_StringArray: public SC_Array {
private:
    int                 strLenAlloc;        //  length of labels - default 40
    char*               stringBuffer;       // internal storage
public :
    char**              labelListData;      // are addresses in string buffer

public :
    SC_StringArray();                   // for labels of default length
    SC_StringArray(const SC_StringArray& a);  // init and copy
    SC_StringArray(int lenAlloc);       // for labels of lenAlloc len
    // this is a kluge -- it conflicts with other SC_Array allocations
    ~SC_StringArray();

    //  access to len
    int                 GetStringLen() const {return strLenAlloc;}
    void                SetStringLen(int newLen);       // sets len if not allocd
    void                ForceMinStringLen(int newLen);  // deallocs if necessary

    // operators
    SC_StringArray&     operator= (const SC_StringArray& a);  // copy operator
    char*&              operator[](int indx);                 //  {return labelListData[indx];}
    char*               operator[](int indx) const;           //  {return labelListData[indx];}
    SC_StringArray&     operator+=(const char* a);            // add a to end of array and increment
    SC_StringArray&     operator+=(const SC_StringArray& a);  // add a to end of array and increment

    bool                operator==(const SC_StringArray& a);  // returns true if arrays are same size
    // and all strings return strcmp == 0
    // NOTE: returns  TRUE if both are empty

    char*               LastIndex() const;

    //  SC_Array virtuals
    bool                ChkAlloc(int nAll);
    void                DeAlloc();
    void                ReAlloc(int nnewAll);       // increases size and copies data

    //  extra methods for labels

    void                Clear();                    //  sets all str to empty

    void                SetString(const char inLab[],   // CopyString inLab to list[i]
                                  int        listPos);

    void                AddString(const char inLab[],   // ConcatString inLab to list[i]
                                  int        listPos);

    void                InsertItem(const char inLab[],
                                   int  insertPos);
    void                DeleteItem(int delPos);

    // mapping
    void                ApplyReverseMap(const SC_IntArray& reverseMap);

    //  sort ascending (stupid sort, will re-implement)
    void                Sort();
    void                Sort(SC_IntArray& indexes);      // carries along indexes


    // search
    bool                SearchForKey(const char key[],
                                     int& findIndx) const;    // findIndx is set to indx if true
    // or indx to insert key after if false
    int                 SearchForKey(const char key[]) const; // as above returns - 1 if not found

    bool                AddIfNotFound(const char newStr[]);   // returns false if array is full and not resizable

    int                 GetMaxStringLen() const;

    void                Clean();    // removes all special chars (no tab subs)

    // clears all elements, allocs to size
    // breaks up input strings into elements -- delimited by fullstring token
    // NOT by characters in token string
    bool                CreateFromString(const char* inputString,
                                         const char* tokenString);  // returns true if inputString <> tokenString

    bool                ExtractQuotedStrings(const char* inputString); // returns true if at least 1 quoted string found


    void                SetIndexed(const char* baseDesc, int offset);

};

typedef T_SC_Array<const char*> SC_ConstStringArray;

#endif // SC_STRINGARRAY_H

