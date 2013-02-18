///////////////////////////////////////////////////////////////////////////////////
//
// T_SC_Array.h
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
// DESCRIPTION: Derived from SC_Array. Provides a template for implmenting a
//              lightweight vector class.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef T_SC_ARRAY_H
#define T_SC_ARRAY_H

#include <new>

#include <genClass/SC_Array.h>

template <class T>

class T_SC_Array: public SC_Array {
private:
    bool            fixedAlloc;
    int                 nfixedAlloc;
public :
    T*              tListData;

public :
    //  constructors/destructors
    T_SC_Array() {tListData = 0; fixedAlloc = false;} //  empty list
    T_SC_Array(int nAll);                             //  allocates to size nAll, sets size
    T_SC_Array(int nAll, const T& initVal);           //  allocates to size nAll, sets size, initializes
    T_SC_Array(T* buffer, int nbuffer);               // fixed alloc
    T_SC_Array(const T_SC_Array& a);     //
    ~T_SC_Array();

    //  array indexing operators -- range checked if not FASTARRAY defd
#ifdef _DEBUG
    T&              operator[](int indx);
    T&              operator[](int indx) const;
#else
    inline T&       operator[](int indx)        {return tListData[indx];}
    inline T&       operator[](int indx) const  {return tListData[indx];}
#endif
    T&              LastIndex() const;
    T&              LastIndex(int offset) const;   // last - offset
    void            SetLastIndex(const T& a);
    T&              NextIndex(int currIndx) const;  // gets curr+1
    // gets 0 if curr == upperbound

    //  other operators
    T_SC_Array&         operator=(const T_SC_Array& a);   //  copy operator
    T_SC_Array&     operator+=(const T& a);           //  add a to end of array and increment
    T_SC_Array&     operator+=(const T_SC_Array& a);  //  adds array to end of current

    //  initializing
    void            FillToSize(const T& fillData);   // fills to current size
    void            FillToAlloc(const T& fillData);  // fills to allocation, sets size to
    // allocation

    //  insert and delete -  should only be used for trivial types
    void            InsertItem(const T&  insData,
                               int atIndx);
    void            DeleteItem(int delIndx);  // no call on dtor for [delIndx] - memory leak if non-trivial

    // re-mapping -  should only be used for trivial types;
    void            ApplyForwardMap(const T_SC_Array<int>& forwardMap);
    void            ApplyReverseMap(const T_SC_Array<int>& reverseMap);

    // reverse order in array -  should only be used for trivial types;
    void            Reverse();


    //  swap elements i and j
    void            Swap(int i, int j);

    //  list allocation/deallocation
    bool            ChkAlloc(int nAll);
    void            AllocAndFill(int nAll, const T& fillData);  // allocs, fills, sets size to alloc
    void            DeAlloc();
    void            ReAlloc(int nnewAll);        // changes size and copies data

    void            BufferAlloc(T* buffer, int nbuffer); // sets size to nBuffer

protected:

    void            InternalCopy(const T_SC_Array& a);

};

template<class T>  T_SC_Array<T>::T_SC_Array(int nAll)
{
    fixedAlloc = false;
    nlistAlloc = 0;
    tListData = 0;
    Alloc(nAll);
    nlistData = nAll;
}

template<class T>  T_SC_Array<T>::T_SC_Array(int nAll, const T& initVal)
{
    fixedAlloc = false;
    nlistAlloc = 0;
    tListData = 0;
    AllocAndFill(nAll, initVal);
}

template<class T>  T_SC_Array<T>::T_SC_Array(T* buffer, int nbuffer)          // fixed alloc
{
    fixedAlloc = true;
    nfixedAlloc = nbuffer;
    nlistAlloc = nbuffer;
    tListData = buffer;
}

template<class T>  T_SC_Array<T>::T_SC_Array(const T_SC_Array& a)
{
    fixedAlloc = false;
    nlistAlloc = 0;
    nlistData = 0;
    tListData = 0;
    InternalCopy(a);
}


template<class T> T_SC_Array<T>::~T_SC_Array()
{
    DeAlloc();
}

//  operators
#ifdef _DEBUG

template<class T> T& T_SC_Array<T>::operator[] (int indx)
{
    if (BadIndex(indx))
        SC_ArrayError("op []");
    return tListData[indx];
}

template<class T> T& T_SC_Array<T>::operator[](int indx) const
{
    if (BadIndex(indx))
        SC_ArrayError("op [] const");
    return tListData[indx];
}

#endif


template<class T> T& T_SC_Array<T>::LastIndex() const
{
#ifdef _DEBUG
    if ((nlistAlloc == 0) || (nlistData == 0))
        SC_ArrayError("LastIndex");
#endif
    return tListData[nlistData - 1];
}

template<class T> T& T_SC_Array<T>::LastIndex(int offset) const
{
#ifdef _DEBUG
    if ((nlistAlloc == 0) || (nlistData < offset))
        SC_ArrayError("LastIndex(int)");
#endif
    return tListData[nlistData - offset - 1];
}

template<class T> T& T_SC_Array<T>::NextIndex(int currIndex) const
{
#ifdef _DEBUG
    if ((nlistAlloc == 0) || (nlistData == 0))
        SC_ArrayError("NextIndex");
#endif
    if (currIndex < (nlistData - 1))
        return tListData[currIndex + 1];
    return tListData[0];
}


template<class T> void  T_SC_Array<T>::SetLastIndex(const T& a)
{
    if ((nlistAlloc == 0) || (nlistData == 0))
        SC_ArrayError("SetLastIndex");
    tListData[nlistData - 1] = a;
}

template<class T> T_SC_Array<T>& T_SC_Array<T>::operator=(const T_SC_Array& a)  //  copy operator
{
    if ( this != &a)
        InternalCopy(a);
    return *this;
}

template<class T> T_SC_Array<T>& T_SC_Array<T>::operator+=(const T& a)  // add a to end of array and increment
{
    if (nlistData == nlistAlloc)
        if (resizable)
            ReAlloc(nlistData * 2 + 1);
        else
            {
                BadIndex(nlistData);
                SC_ArrayError("op += T");
            }

    tListData[nlistData++] = a;
    return *this;
}

template<class T> T_SC_Array<T>& T_SC_Array<T>::operator+=(const T_SC_Array& a)  // add a to end of array and increment
{
    if ((nlistData + a.Size()) > nlistAlloc)
        if (resizable)
            {
                int newSize;
                if (a.Size() < nlistData)
                    newSize = nlistData * 2 + 1;
                else
                    newSize =  nlistData + (a.Size() * 2);
                ReAlloc(newSize);
            }
        else
            {
                BadIndex(nlistData);
                SC_ArrayError("op += T_SC_Array");
            }

    for ( int i = 0; i < a.Size(); i++)
        tListData[nlistData++] = a[i];

    return *this;
}

template<class T> void T_SC_Array<T>::Swap(int i, int j)
{
    if (BadIndex(i) || BadIndex(j))
        SC_ArrayError("Swap");
    T   tmpVal = tListData[i];
    tListData[i] = tListData[j];
    tListData[j] = tmpVal;
}

template<class T> void T_SC_Array<T>::FillToSize(const T& fillData)
{
    for ( int i = 0; i < Size(); i++)
        tListData[i] = fillData;
}
template<class T> void T_SC_Array<T>::FillToAlloc(const T& fillData)
{
    for (int i = 0; i < AllocSize(); i++)
        tListData[i] = fillData;
    SetSize(AllocSize());
}

template<class T> void T_SC_Array<T>::InsertItem(const T&  insData,
                                                 int atIndx)
{
    if (IsFull())
        if (resizable)
            ReAlloc(nlistData * 2 + 1);
        else
            SC_ArrayError("InsertItem");

    if (BadIndex(atIndx))
        SC_ArrayError("InsertItem");

    for (int i = Size(); i > atIndx; i--)
        tListData[i] = tListData[i - 1];

    tListData[atIndx] = insData;

    if (atIndx < Size())
        SetSize(Size() + 1);
    else
        SetSize(atIndx + 1);
}

template<class T> void T_SC_Array<T>::ApplyForwardMap(const T_SC_Array<int>& forwardMap)
{
    // copy to tmp
    T_SC_Array<T> tmp(*this);
    for (int i = 0; i < forwardMap.Size(); i++)
        (*this)[i] = tmp[forwardMap[i]];
    SetSize(forwardMap.Size());
}



template<class T> void T_SC_Array<T>::ApplyReverseMap(const T_SC_Array<int>& reverseMap)
{
    int lastMap = -1;
    for (int i = 0; i < Size(); i++)
        {
            int nextMap = reverseMap[i];
            if (nextMap >= 0)
                {
                    if (nextMap < i)
                        (*this)[nextMap] = (*this)[i];
                    lastMap = nextMap;
                }
        }
    SetSize(lastMap + 1);
}

template<class T> void T_SC_Array<T>::Reverse()
{
    int ub = UpperBound();
    int half = Size() / 2;
    for (int i = 0; i < half; i++)
        {
            T tmp = (*this)[i];
            (*this)[i] = (*this)[ub];
            (*this)[ub] = tmp;
            ub--;
        }
}


template<class T> void T_SC_Array<T>::DeleteItem(int delIndx)
{
    if (BadIndex(delIndx))
        SC_ArrayError("DeleteItem");

    nlistData--;
    for (int i = delIndx; i < Size(); i++)
        tListData[i] = tListData[i + 1];
}


template<class T> void T_SC_Array<T>::BufferAlloc(T* buffer, int nbuffer)
{
    fixedAlloc = true;
    nfixedAlloc = nbuffer;
    nlistAlloc = nbuffer;
    nlistData = nbuffer;
    tListData = buffer;
}

//  list allocation/deallocation
template<class T> bool T_SC_Array<T>::ChkAlloc(int nAll)
{
    nlistData = 0;
    if (nAll <= nlistAlloc)
        return true;
    if (fixedAlloc)
        {
            if (nAll <= nfixedAlloc)
                {
                    nlistAlloc = nAll;
                    return true;
                }
            fixedAlloc = false;
            tListData = 0;
        }

    delete [] tListData;

    try
        {
            tListData = new T[nAll];
        }
    catch (std::bad_alloc)
        {
            tListData = 0;
            nlistAlloc = 0;
            return false;
        }

    nlistAlloc = nAll;
    return true;
}

template<class T> void T_SC_Array<T>::AllocAndFill(int nAll, const T& fillData)
{
    Alloc(nAll);
    for (int i = 0; i < nAll; i++)
        tListData[i] = fillData;
    SetSize(nAll);
}


template<class T> void T_SC_Array<T>::DeAlloc()
{
    if (!fixedAlloc)
        delete [] tListData;
    tListData    = 0;
    fixedAlloc   = false;
    SC_Array::DeAlloc();
}

template<class T> void T_SC_Array<T>::ReAlloc(int nnewAll)        // changes size and copies data
{
    if (nnewAll == 0)
        {
            nlistAlloc = 0;
            return;
        }
    if (fixedAlloc)
        {
            if (nnewAll <= nfixedAlloc)
                {
                    nlistAlloc = nnewAll;
                    return;
                }
        }

    T* tmpData;
    try
        {
            tmpData = new T[nnewAll];
        }
    catch (std::bad_alloc)
        {
            SC_ArrayError("ReAlloc");
        }

    nlistAlloc = nnewAll;
    if (nnewAll < nlistData)
        nlistData = nnewAll;

    for (int i = 0; i < nlistData; i++)
        tmpData[i] = tListData[i];

    if (fixedAlloc)
        {
            fixedAlloc = false;
            tListData = 0;
        }

    delete [] tListData;

    tListData = tmpData;
}


template<class T>  void T_SC_Array<T>::InternalCopy(const T_SC_Array& a)
{
    Alloc(a.nlistAlloc);
    nlistData = a.Size();
    resizable = a.resizable;
    sorted    = a.sorted;
    for ( int i = 0; i < nlistData; i++)
        tListData[i] = a.tListData[i];
}

#endif // T_SC_ARRAY_H

