///////////////////////////////////////////////////////////////////////////////////
//
// SC_IntArray.cpp
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
// DESCRIPTION: derived from T_SC_array<int>
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>

#include <genClass/SC_IntArray.h>


bool SC_IntArray::operator==(const SC_IntArray& a) const
{
    if (Size() != a.Size())
        return false;
    for (int i = 0; i < Size(); i++)
        if ((*this)[i] != a[i])
            return false;

    return true;
}

void SC_IntArray::Reset(int  maxList)
{
    if (maxList > nlistAlloc)
        ReAlloc(maxList);

    int nOK = 0;
    for ( int i = 0; i < nlistData; i++)
        if ( tListData[i] < maxList)
            {
                tListData[nOK] = tListData[i];
                nOK++;
            }
    nlistData = nOK;
}

void SC_IntArray:: Clear()
{
    if (nlistAlloc == 0)
        ReAlloc(1);

    tListData[0] = 0;
    nlistData = 0;
}

void SC_IntArray:: AllocAndIndex(int maxIndex)
{
    AllocAndSetSize(maxIndex);
    for (int i = 0; i < maxIndex; i++)
        tListData[i] = i;
}


void SC_IntArray::GetMinMax(int& minVal, int& maxVal) const
{
    if (IsEmpty())
        {
            minVal = 0;
            maxVal = 0;
            return;
        }

    minVal = (*this)[0];
    maxVal = (*this)[0];

    for (int i = 1;  i < Size(); i++)
        {
            int nextVal = (*this)[i];
            if (nextVal < minVal)
                minVal = nextVal;
            else
                if (nextVal > maxVal)
                    maxVal = nextVal;
        }
}

int SC_IntArray::GetMin() const
{
    int minV, maxV;
    GetMinMax(minV, maxV);
    return minV;
}

int SC_IntArray::GetMax() const
{
    int minV, maxV;
    GetMinMax(minV, maxV);
    return maxV;
}

int     SC_IntArray::GetCount() const
{
    int cnt = 0;
    for (int i = 0;  i < Size(); i++)
        cnt += (*this)[i];
    return cnt;
}

// search - assumes list is sorted
bool SC_IntArray::SearchForKey(int  key,
                               int& findIndx) const
{
    if ( nlistData == 0)
        {
            findIndx = -1;
            return false;
        }

    findIndx = 0;
    if ( nlistData == 1)
        {
            return (key  == tListData[0]);
        }
    else
        if ((nlistData < 8) || (!sorted))
            {
                for ( int i = 0; i < nlistData; i++)
                    if (tListData[i] == key)
                        {
                            findIndx = i;
                            return  true;
                        }
                return false;
            }

    // binary search
    int l = 0;
    int r = nlistData - 1;
    do
        {
            findIndx = (l + r) / 2;
            if (tListData[findIndx] == key)
                return true;

            if (tListData[findIndx] < key)
                l = findIndx + 1;
            else
                r = findIndx - 1;

        } while ( l < r);

    int newIndx = (l + r) / 2;
    if ( newIndx != findIndx)
        {
            findIndx = newIndx;
            return(tListData[findIndx] == key);
        }

    return false;

}

bool SC_IntArray::SearchForKey(int  key) const
{
    int dummy;
    return SearchForKey(key, dummy);
}

bool SC_IntArray::CheckLimits(int minLim,       // returns true if all are between
                              int maxLim) const
{
    for ( int i = 0; i < Size(); i++)
        if ((tListData[i] <= minLim) || (tListData[i] >= maxLim))
            return false;
    return true;
}

int SC_IntArray::PosValueCount() const
{
    int count = 0;
    for (int i = 0; i < Size(); i++)
        if (tListData[i] >= 0)
            count++;
    return count;
}

void  SC_IntArray::AddIfNotFound(int inVal)
{
    for (int i = 0; i < Size(); i++)
        if (tListData[i] == inVal)
            return;

    (*this) += inVal;
}

void SC_IntArray::MakeMap(SC_IntArray&    mapVector) const
{
    mapVector.Alloc(Size());
    for ( int i = 0; i < Size(); i++)
        if (tListData[i] >= 0)
            mapVector += i;
}

void  SC_IntArray::MakeReverseMap(int   maxReverse,
                                  const SC_IntArray& keepIndexes)
{
    AllocAndFill(maxReverse, -1);
    for (int i = 0; i < keepIndexes.Size(); i++)
        (*this)[keepIndexes[i]] = 1;
    SetReverseOK();
}

void  SC_IntArray::SetReverseOK()
{
    int nkeep = 0;
    for (int i = 0; i < Size(); i++)
        if (tListData[i] >= 0)
            tListData[i] = nkeep++;
}


bool SC_IntArray::Pack()
{
    int nOK = 0;
    for (int i  = 0; i < Size(); i++)
        {
            int currIndex = tListData[i];
            if (currIndex >= 0)
                {
                    tListData[nOK] = currIndex;
                    nOK++;
                }
        }

    SetSize(nOK);

    return (Size() > 0);
}

void SC_IntArray::IncrementIndexes(const SC_IntArray& indexes)
{
    for (int i = 0; i < indexes.Size(); i++)
        (*this)[indexes[i]]++;
}

void SC_IntArray::ClearIndexes(const SC_IntArray& indexes)
{
    for (int i = 0; i < indexes.Size(); i++)
        (*this)[indexes[i]] = 0;
}


static void QBSort(SC_IntArray& a, int lo, int hi)
{
    for (int j= hi; j > lo; j--)
        {
            for (int i=lo; i < j; i++)
                {
                    if (a[i] > a[i+1])
                        {
                            int t = a[i];
                            a[i] = a[i+1];
                            a[i+1] = t;
                        }
                }
        }
}


static void QSort(SC_IntArray& a, int lo0, int hi0)
{
    int lo = lo0;
    int hi = hi0;

    // Bubble sort if the number of elements is less than 6
    if ((hi-lo) <= 6)
        {
            QBSort(a, lo, hi);
            return;
        }

    // Pick a pivot and move it out of the way /

    int pivot = a[(lo + hi) / 2];
    a[(lo + hi) / 2] = a[hi];
    a[hi] = pivot;

    while( lo < hi )
        {
            // Search forward from a[lo] until an element is found that * is greater than the pivot or lo >= hi
            while (a[lo] <= pivot && lo < hi)
                {
                    lo++;
                }

            // Search backward from a[hi] until element is found that * is less than the pivot, or hi <= lo
            while (pivot <= a[hi] && lo < hi )
                {
                    hi--;
                }
            // Swap elements a[lo] and a[hi]
            if( lo < hi )
                {
                    int T = a[lo];
                    a[lo] = a[hi];
                    a[hi] = T;
                }
        }

    // Put the median in the "center" of the list
    a[hi0] = a[hi];
    a[hi] = pivot;
    // Recursive calls, elements a[lo0] to a[lo-1] are less than or * equal to pivot, elements a[hi+1] to a[hi0] are greater than * pivot.
    QSort(a, lo0, lo-1);
    QSort(a, hi+1, hi0);
}


void  SC_IntArray::Sort(bool ascending)
{
    QSort(*this, 0, UpperBound());
    if (!ascending)
        Reverse();
}

static void QBSortI(SC_IntArray& a, int lo, int hi, SC_IntArray& indx)
{
    for (int j= hi; j > lo; j--)
        {
            for (int i=lo; i < j; i++)
                {
                    if (a[i] > a[i+1])
                        {
                            int t = a[i];
                            a[i] = a[i+1];
                            a[i+1] = t;

                            int it = indx[i];
                            indx[i] = indx[i + 1];
                            indx[i + 1] = it;
                        }
                }
        }
}

static void QSortI(SC_IntArray& a, int lo0, int hi0, SC_IntArray& indx)
{
    int lo = lo0;
    int hi = hi0;

    // Bubble sort if the number of elements is less than 6
    if ((hi-lo) <= 6)
        {
            QBSortI(a, lo, hi, indx);
            return;
        }

    // Pick a pivot and move it out of the way /

    int pivIndx = (lo + hi) / 2;
    int pivot = a[pivIndx];
    a[pivIndx] = a[hi];
    a[hi] = pivot;

    int ipivot = indx[pivIndx];
    indx[pivIndx] = indx[hi];
    indx[hi] = ipivot;

    while( lo < hi )
        {
            // Search forward from a[lo] until an element is found that * is greater than the pivot or lo >= hi
            while (a[lo] <= pivot && lo < hi)
                {
                    lo++;
                }

            // Search backward from a[hi] until element is found that * is less than the pivot, or hi <= lo
            while (pivot <= a[hi] && lo < hi )
                {
                    hi--;
                }
            // Swap elements a[lo] and a[hi]
            if( lo < hi )
                {
                    int T = a[lo];
                    a[lo] = a[hi];
                    a[hi] = T;

                    int it = indx[lo];
                    indx[lo] = indx[hi];
                    indx[hi] = it;
                }
        }

    // Put the median in the "center" of the list
    a[hi0] = a[hi];
    a[hi] = pivot;

    indx[hi0] = indx[hi];
    indx[hi] = ipivot;

    // Recursive calls, elements a[lo0] to a[lo-1] are less than or * equal to pivot, elements a[hi+1] to a[hi0] are greater than * pivot.
    QSortI(a, lo0, lo-1, indx);
    QSortI(a, hi+1, hi0, indx);
}




void  SC_IntArray::Sort(bool         ascending,
                        SC_IntArray&  indexes)

{
    if (indexes.Size() == Size())
        {
            QSortI(*this, 0, UpperBound(), indexes);
            if (!ascending)
                indexes.Reverse();
        }
    else
        QSort(*this, 0, UpperBound());

    if (!ascending)
        Reverse();

    /*

      bool swapInt = indexes.Size() == Size();
      for ( int i = 0; i < (Size() - 1); i++)
      for ( int j = i + 1; j < Size(); j++)
      {
      bool swap;
      if (ascending)
      swap = tListData[i] > tListData[j];
      else
      swap = tListData[i] < tListData[j];

      if (swap)
      {
      int tmp = tListData[i];
      tListData[i] = tListData[j];
      tListData[j] = tmp;

      if ( swapInt)
      {
      int tInt = indexes[i];
      indexes[i] = indexes[j];
      indexes[j] = tInt;
      }

      }
      }
    */

}


void SC_IntMatrix:: MatrixAlloc(int nCols, int nRows)
{
    DeAlloc();
    if ((nCols == 0) && (nRows == 0))
        return;

    if (nCols < 1)
        GenAppInternalError("SC_IntMatrix::MatrixAlloc");

    // set columns
    Alloc(nCols);

    //  each column
    for (int i = 0; i < nCols; i++)
        tListData[i].Alloc(nRows);
}

void SC_IntMatrix:: MatrixReAlloc(int nCols, int nRows)
{
    if ((nCols == 0) && (nRows == 0))
        return;

    if (nCols < 1)
        GenAppInternalError("SC_IntMatrix::MatrixReAlloc");


    // set columns
    ReAlloc(nCols);

    //  each column
    for (int i = 0; i < nCols; i++)
        tListData[i].ReAlloc(nRows);
}

void SC_IntMatrix::MatrixAllocResizable(int nCols, int nRows)
//only number of rows is resizable
{
    DeAlloc();
    if ((nCols == 0) && (nRows == 0))
        return;

    if (nCols < 1)
        GenAppInternalError("SC_IntMatrix::MatrixAlloc");

    // set columns
    Alloc(nCols);

    //  each column
    for (int i = 0; i < nCols; i++)
        tListData[i].SetResizable(nRows);
}

void SC_IntMatrix::MatrixSetResizable(int defaultAlloc)
//set rows resizable
{
    int allocSize = nlistAlloc;
    for (int i = 0; i < allocSize; i++)
        tListData[i].SetResizable(defaultAlloc);
}

int     SC_IntMatrix::MatrixSize() const
{
    int cnt = 0;
    for (int i = 0; i < Size(); i++)
        cnt += (*this)[i].Size();
    return cnt;
}

void SC_IntMatrix::MatrixAllocAndSetSize(int nCols, int nRows)
{
    MatrixAlloc(nCols, nRows);
    MatrixSetSize(nCols, nRows);
}

void SC_IntMatrix::MatrixSetSize(int nCols, int nRows)
{
    SetSize(nCols);
    for (int i = 0; i < nCols; i++)
        tListData[i].SetSize(nRows);
}

void SC_IntMatrix::SetNull()
{
    MatrixFill(-1);
}
void SC_IntMatrix::MatrixFill(int iVal)
{
    for ( int i = 0; i < Size(); i++)
        tListData[i].FillToSize(iVal);
}


void SC_IntCube:: CubeAlloc(int nPage, int nCols, int nRows)
{
    DeAlloc();
    if ((nPage == 0) && (nCols == 0) && (nRows == 0))
        return;

    if ((nCols < 1) || (nPage < 1))
        GenAppInternalError("SC_IntMatrix::ChkAlloc");

    // set pages
    Alloc(nPage);

    //  each column
    for (int i = 0; i < nPage; i++)
        tListData[i].MatrixAlloc(nCols, nRows);
}

