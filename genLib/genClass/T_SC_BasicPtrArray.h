///////////////////////////////////////////////////////////////////////////////////
//
// T_SC_BasicPtrArray.h
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
// DESCRIPTION: Derived from T_SC_Array to provide basic facilities for
//              operating on arrays of pointers to type. Does not handle type
//              construction/destruction.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef T_SC_BASICPTRARRAY_H
#define T_SC_BASICPTRARRAY_H

#include <genClass/T_SC_Array.h>


template <class T>

class T_SC_BasicPtrArray: public T_SC_Array<T*> {

public :
    //  constructors/destructors
    T_SC_BasicPtrArray();          // initialized as resizable
    T_SC_BasicPtrArray(int nAll);  // allocates to size nAll
                                   // fills with 0
                                   // note: unlike T_SC_Array<> resets size
                                   // to empty

    void        AddTo(T*    dVal);                  //  equiv of +=
    bool        DeleteFrom(const T* dVal);          // looks for dVal, deletes
                                                    // returns true if found
    bool        Found(const T* searchVal) const;    // returns true if found
    int         FoundAt(const T* searchVal) const;  // index or -1 if not found

    T&          GetRef(int i);
    T&          GetRef(int i) const;

    void        Clean();

protected:


};

template<class T>  T_SC_BasicPtrArray<T>::T_SC_BasicPtrArray()
{
    T_SC_Array<T*>::resizable = true;
}


template<class T>  T_SC_BasicPtrArray<T>::T_SC_BasicPtrArray(int nAll)
{
    T_SC_Array<T*>::AllocAndFill(nAll, 0);
    T_SC_Array<T*>::resizable = true;
    T_SC_Array<T*>::nlistData = 0;
}


template<class T> void T_SC_BasicPtrArray<T>::AddTo(T*    dVal)
{
    if (T_SC_Array<T*>::nlistData == T_SC_Array<T*>::nlistAlloc)
        T_SC_Array<T*>::ReAlloc(T_SC_Array<T*>::nlistData * 2 + 1);
    T_SC_Array<T*>::tListData[T_SC_Array<T*>::nlistData++] = dVal;
}



template<class T> bool T_SC_BasicPtrArray<T>::DeleteFrom(const T*   dVal)
{
    for (int i = 0; i < T_SC_Array<T*>::Size(); i++)
        if (T_SC_Array<T*>::tListData[i] == dVal)
            {
                T_SC_Array<T*>::DeleteItem(i);
                return true;
            }
    return false;
}

template<class T> bool T_SC_BasicPtrArray<T>::Found(const T*   searchVal) const
{
    for (int i = 0; i < T_SC_Array<T*>::Size(); i++)
        if (T_SC_Array<T*>::tListData[i] == searchVal)
            return true;

    return false;
}

template<class T> int T_SC_BasicPtrArray<T>::FoundAt(const T*   searchVal) const
{
    for (int i = 0; i < T_SC_Array<T*>::Size(); i++)
        if (T_SC_Array<T*>::tListData[i] == searchVal)
            return i;
    return -1;
}


template<class T> T& T_SC_BasicPtrArray<T>::GetRef(int i)
{
    return *((*this)[i]);
}

template<class T> T& T_SC_BasicPtrArray<T>::GetRef(int i) const
{
    return *((*this)[i]);
}

template<class T> void T_SC_BasicPtrArray<T>::Clean()
{
    int nOK = 0;
    for (int i = 0; i < T_SC_Array<T*>::nlistData; i++)
        if (T_SC_Array<T*>::tListData[i] != 0)
            T_SC_Array<T*>::tListData[nOK++] = T_SC_Array<T*>::tListData[i];
    T_SC_Array<T*>::SetSize(nOK);
}

#endif // T_SC_BASICPTRARRAY_H

