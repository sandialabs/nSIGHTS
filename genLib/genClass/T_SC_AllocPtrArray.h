///////////////////////////////////////////////////////////////////////////////////
//
// T_SC_AllocPtrArray.h
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
//              operating on arrays of allocated pointers to type. Does not handle type
//              construction/destruction.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef T_SC_ALLOCPTRARRAY_H
#define T_SC_ALLOCPTRARRAY_H

#include <genClass/T_SC_Array.h>
#include <genClass/U_Msg.h>

template <class T>

class T_SC_AllocPtrArray: public T_SC_Array<T*> {
public :
    //  constructors/destructors
    T_SC_AllocPtrArray() : T_SC_Array<T*>() {}
    ~T_SC_AllocPtrArray();

    // access
    T&              GetRef(int index) const;

    //  list allocation/deallocation
    virtual bool    ChkAlloc(int nAll);         // returns false if unable to alloc
    virtual void    ReAlloc(int nnewAll);       // increases size and copies data
    virtual void    DeAlloc();

    // deletes all interspersed 0 entries, sets size to number of allocd entries
    // returns true if ptr movement occured
    virtual bool    Cleanup();


    // alloc and create
    void            AllocAndCreate(int nCreate);

    // basic insert/delete
    bool            InsertPtr(int index);   // inserts 0 at index
    // returns false if IsFull

    void            DeletePtr(int index);   // deletes if valid
    //
    // class specific
    // returns true if *index is constructed
    bool            IsValid(int index) const        {return T_SC_Array<T*>::tListData[index] != 0;}

    // returns true if index == 0
    bool            IsNotValid(int index) const         {return T_SC_Array<T*>::tListData[index] == 0;}

    int             ValidSize() const;

    //  if *index == 0, sets to new T()
    // returns true if new element created
    bool            CreateIfNecessary(int index);

protected:

    virtual void    MakeNew(int index);             // creates a new class at index

};



template<class T> T_SC_AllocPtrArray<T>::~T_SC_AllocPtrArray()
{
    DeAlloc();
}

template<class T> void T_SC_AllocPtrArray<T>::AllocAndCreate(int nCreate)
{
    DeAlloc();
    T_SC_Array<T*>::Alloc(nCreate);
    for (int i = 0; i < nCreate; i++)
        MakeNew(i);
    T_SC_Array<T*>::SetSize(nCreate);
}

template<class T> T& T_SC_AllocPtrArray<T>::GetRef(int index) const
{
    if (IsNotValid(index))
        GenAppInternalError("T& T_SC_AllocPtrArray<T>::GetRef");

    return *(T_SC_Array<T*>::tListData[index]);
}

//  list allocation/deallocation
template<class T> bool T_SC_AllocPtrArray<T>::ChkAlloc(int nAll)
{
    if (T_SC_Array<T*>::ChkAlloc(nAll))
        {
            for (int i = 0; i < nAll; i++)
                T_SC_Array<T*>::tListData[i] = 0;
            return true;
        }
    return false;
}


template<class T> void T_SC_AllocPtrArray<T>::DeAlloc()
{
    if (T_SC_Array<T*>::tListData != 0)
        for (int j = 0; j < T_SC_Array<T*>::nlistAlloc; j++)
            delete T_SC_Array<T*>::tListData[j];
    T_SC_Array<T*>::DeAlloc();
}

template<class T> void T_SC_AllocPtrArray<T>::ReAlloc(int nnewAll)        // changes size and copies data
{
    int oldN = T_SC_Array<T*>::nlistData;
    T_SC_Array<T*>::ReAlloc(nnewAll);
    if (oldN < T_SC_Array<T*>::nlistAlloc)
        for (int i = oldN; i < T_SC_Array<T*>::nlistAlloc; i++)
            T_SC_Array<T*>::tListData[i] = 0;
}

template<class T> void T_SC_AllocPtrArray<T>::MakeNew(int index)
{
    T_SC_Array<T*>::tListData[index] = new T();
}


template<class T> bool T_SC_AllocPtrArray<T>::CreateIfNecessary(int index)
{
    if (IsValid(index))
        return false;
    MakeNew(index);
    if (T_SC_Array<T*>::Size() <= index)
        T_SC_Array<T*>::SetSize(index + 1);
    return true;
}


template<class T> int T_SC_AllocPtrArray<T>::ValidSize() const
{
    int nValid = 0;
    for (int i = 0; i < T_SC_Array<T*>::Size(); i++)
        if (T_SC_Array<T*>::tListData[i] != 0)
            nValid++;
    return nValid;
}


template<class T> bool T_SC_AllocPtrArray<T>::InsertPtr(int index)
{
    if (T_SC_Array<T*>::IsFull())
        return false;

    for (int i = T_SC_Array<T*>::Size(); i > index; i--)
        T_SC_Array<T*>::tListData[i] = T_SC_Array<T*>::tListData[i - 1];
    T_SC_Array<T*>::tListData[index] = 0;
    T_SC_Array<T*>::nlistData++;
    return true;
}


template<class T> void T_SC_AllocPtrArray<T>::DeletePtr(int index)
{
    if (IsValid(index))
        {
            delete T_SC_Array<T*>::tListData[index];
            T_SC_Array<T*>::tListData[index] = 0;
        }

    // paranoi -- shouldn't happen
    if (index > T_SC_Array<T*>::Size())
        return;

    T_SC_Array<T*>::DeleteItem(index);
    T_SC_Array<T*>::tListData[T_SC_Array<T*>::Size()] = 0;
}



template<class T> bool T_SC_AllocPtrArray<T>::Cleanup()
{
    int nOK = 0;
    for (int i = 0; i < T_SC_Array<T*>::Size(); i++)
        if (T_SC_Array<T*>::tListData[i] != 0)
            T_SC_Array<T*>::tListData[nOK++] = T_SC_Array<T*>::tListData[i];
    for (int i = nOK; i < T_SC_Array<T*>::Size(); i++)
        T_SC_Array<T*>::tListData[i] = 0;

    bool retVal = (nOK != T_SC_Array<T*>::Size());
    T_SC_Array<T*>::SetSize(nOK);

    return retVal;
}

#endif // T_SC_ALLOCPTRARRAY_H

