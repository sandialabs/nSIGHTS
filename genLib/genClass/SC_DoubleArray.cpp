///////////////////////////////////////////////////////////////////////////////////
//
// SC_DoubleArray.cpp
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
// DESCRIPTION: derived from T_SC_Array<double>
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <windows.h>

#include <genClass/SC_DoubleArray.h>


void SC_DoubleArray:: Clear()
{
    if (nlistAlloc == 0)
        ReAlloc(1);

    tListData[0] = 0.0;
    nlistData = 0;
}

void  SC_DoubleArray::SetNull()
{
    FillToAlloc(nullReal);
}

void  SC_DoubleArray::CalcMinMax(double& minVal, double& maxVal) const
{
    if (IsEmpty())
        {
            minVal = nullReal;
            maxVal = nullReal;
            return;
        }

    minVal = tListData[0];
    maxVal = minVal;

    AddToMinMax(minVal, maxVal);
}


void  SC_DoubleArray::AddToMinMax(double& minVal, double& maxVal) const
{
    bool minNotSet = (RealIsNull(minVal));
    for ( int i = 0; i < Size(); i++)
        {
            double dVal = tListData[i];
            if (RealIsNull(dVal))
                continue;

            if (minNotSet)
                {
                    minVal = dVal;
                    maxVal = dVal;
                    minNotSet = false;
                }
            else
                {
                    if ( dVal > maxVal)
                        maxVal = dVal;
                    else
                        if (dVal < minVal)
                            minVal = dVal;
                }
        }
}

bool  SC_DoubleArray::WithinLimits(const double& minVal, const double& maxVal) const
{
    for ( int i = 0; i < Size(); i++)
        {
            const double& dVal = tListData[i];
            if ((dVal < minVal) || (dVal > maxVal))
                return false;
        }
    return true;
}

bool SC_DoubleArray::DataOK(int index)const
{
    return (tListData[index] > nullReal);
}



bool SC_DoubleArray::DataOK()const
{
    for ( int i = 0; i < Size(); i++)
        if (tListData[i] > nullReal)
            return true;
    return false;
}

bool SC_DoubleArray::AllDataOK()const
{
    for ( int i = 0; i < Size(); i++)
        if (RealIsNull(tListData[i]))
            return false;
    return true;
}

void SC_DoubleArray::Cleanup()
{
    int nOK = 0;
    for ( int i = 0; i < Size(); i++)
        {
            const double& currVal = tListData[i];
            if (IsFinite(currVal) && (!RealIsNull(currVal)))
                tListData[nOK++] = currVal;
        }
    SetSize(nOK);
}


bool SC_DoubleArray::IsAscending(bool strict) const
{
    if (Size() < 2)
        return false;
    if (RealIsNull(tListData[0]))
        return false;

    bool opBad;
    for (int i = 1; i < Size(); i++)
        {
            if (RealIsNull(tListData[i]))
                return false;

            if (strict)
                opBad = (tListData[i] <= tListData[i - 1]);
            else
                opBad = (tListData[i] < tListData[i - 1]);

            if (opBad)
                return false;
        }
    return true;
}


int SC_DoubleArray::GetnDataOK()   const
{
    int nOK = 0;
    for ( int i = 0; i < Size(); i++)
        if (tListData[i] > nullReal)
            nOK++;
    return nOK;
}

bool  SC_DoubleArray::Log10()
{
    bool okFound = false;
    for (int i = 0; i < Size(); i++)
        {
            double& xVal = tListData[i];
            if (xVal > 1.0E-200)
                {
                    xVal = log10(xVal);
                    okFound = true;
                }
            else
                xVal = nullReal;
        }
    return okFound;
}

void  SC_DoubleArray::InvLog10()
{
    for (int i = 0; i < Size(); i++)
        {
            double& xVal = tListData[i];
            if (xVal < -200.0)
                xVal = 0.0;  // underflow to 0.0 ??
            else if (xVal < 200.0)
                xVal = InvLgt(xVal);
            else
                xVal = nullReal;
        }
}

bool SC_DoubleArray::Log()
{
    bool okFound = false;
    for (int i = 0; i < Size(); i++)
        {
            double& xVal = tListData[i];
            if (xVal > 1.0E-200)
                {
                    xVal = log(xVal);
                    okFound = true;
                }
            else
                xVal = nullReal;
        }

    return okFound;

}

void SC_DoubleArray::Exp()
{
    const double logLim = 708.0;

    for (int i = 0; i < Size(); i++)
        {
            double& xVal = tListData[i];
            if (xVal < -logLim)
                xVal = 0.0;  // underflow to 0.0 ??
            else if (xVal < logLim)
                xVal = exp(xVal);
            else
                xVal = nullReal;
        }
}

double SC_DoubleArray::Sum() const
{
    double sum = 0.0;
    for (int i = 0; i < Size(); i++)
        {
            double& xVal = tListData[i];
            if (!RealIsNull(xVal))
                sum += xVal;
        }
    return sum;
}


bool SC_DoubleArray::SameAs(const SC_DoubleArray& a, const double& epsilon) const
{
    if (a.Size() != Size())
        return false;

    for (int i = 0; i < Size(); i++)
        if (!RealsAreSame((*this)[i], a[i], epsilon))
            return false;

    return true;
}


static void QBSort(SC_DoubleArray& a, int lo, int hi)
{
    for (int j= hi; j > lo; j--)
        {
            for (int i=lo; i < j; i++)
                {
                    if (a[i] > a[i+1])
                        {
                            double t = a[i];
                            a[i] = a[i+1];
                            a[i+1] = t;
                        }
                }
        }
}

static void QSort(SC_DoubleArray& a, int lo0, int hi0)
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

    double pivot = a[(lo + hi) / 2];
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
                    double T = a[lo];
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

int stack_overflow_exception_filter(int exception_code)
{
    if (exception_code == EXCEPTION_STACK_OVERFLOW)
        {
            // Do not call _resetstkoflw here, because
            // at this point, the stack is not yet unwound.
            // Instead, signal that the handler (the __except block)
            // is to be executed.
            return EXCEPTION_EXECUTE_HANDLER;
        }
    else
        return EXCEPTION_CONTINUE_SEARCH;
}


bool SC_DoubleArray::Sort(bool ascending)
{
    __try
        {
            QSort(*this, 0, UpperBound());
        }
    __except(stack_overflow_exception_filter(GetExceptionCode()))
        {
            if (!_resetstkoflw())
                GenAppInternalError("QSort Recursion");
            return false;
        }
    if (!ascending)
        Reverse();
    return true;
}

static void QBSortI(SC_DoubleArray& a, int lo, int hi, SC_IntArray& indx)
{
    for (int j= hi; j > lo; j--)
        {
            for (int i=lo; i < j; i++)
                {
                    if (a[i] > a[i+1])
                        {
                            double t = a[i];
                            a[i] = a[i+1];
                            a[i+1] = t;
                            int it = indx[i];
                            indx[i] = indx[i + 1];
                            indx[i + 1] = it;
                        }
                }
        }
}

static void QSortI(SC_DoubleArray& a, int lo0, int hi0, SC_IntArray& indx)
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
    double pivot = a[pivIndx];
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
                    double T = a[lo];
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


void  SC_DoubleArray::Sort(bool         ascending,
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


    /*    bool swapInt = indexes.Size() == Size();
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
          double tmp = tListData[i];
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

void SC_DoubleArray::Reorder(const SC_IntArray& indexes)
{
    if (indexes.Size() != Size())
        GenAppInternalError("SC_DoubleArray::Reorder-1");

    SC_DoubleArray temp(*this);
    for(int i = 0; i < Size(); i++)
        {
            int indx = indexes[i];
            if ((indx < 0) || (indx > UpperBound()))
                GenAppInternalError("SC_DoubleArray::Reorder-2");

            (*this)[i] = temp[indexes[i]];
        }

}

SC_DoubleMatrix::SC_DoubleMatrix(int nCols, int nRows)
{
    buffer = 0;
    MatrixAllocAndSetSize(nCols, nRows);
}


void SC_DoubleMatrix:: DeAlloc()
{
    if (buffer != 0)
        {
            delete [] buffer;
            buffer = 0;
        }
    T_SC_Array<SC_DoubleArray>::DeAlloc();
}

void SC_DoubleMatrix:: FixedMatrixAlloc(int nCols, int nRows)
{
    DeAlloc();
    if ((nCols <= 0) || (nRows <= 0))
        GenAppInternalError("SC_DoubleMatrix::FixedMatrixAlloc");

    // set columns
    Alloc(nCols);

    try
        {
            buffer = new double[nCols * nRows];
        }
    catch (std::bad_alloc)
        {
            throw AllocationError("SC_DoubleMatrix::FixedMatrixAlloc");
        }

    //  each column
    for (int i = 0; i < nCols; i++)
        {
            tListData[i].BufferAlloc(&buffer[i * nRows], nRows);
            tListData[i].nlistData = 0; // default buffer is set size
        }
}

void  SC_DoubleMatrix::FixedMatrixAllocAndSetSize(int nCols, int nRows)
{
    FixedMatrixAlloc(nCols, nRows);
    MatrixSetSize(nCols, nRows);
}


void SC_DoubleMatrix:: MatrixAlloc(int nCols, int nRows)
{
    DeAlloc();
    if ((nCols == 0) && (nRows == 0))
        return;

    if (nCols < 1)
        GenAppInternalError("SC_DoubleMatrix::MatrixAlloc");

    // set columns
    Alloc(nCols);

    //  each column
    for (int i = 0; i < nCols; i++)
        tListData[i].Alloc(nRows);
}

void  SC_DoubleMatrix::MatrixAllocAndSetSize(int nCols, int nRows)
{
    MatrixAlloc(nCols, nRows);
    MatrixSetSize(nCols, nRows);
}

void SC_DoubleMatrix::MatrixSetSize(int nCols, int nRows)
{
    SetSize(nCols);
    for (int i = 0; i < nCols; i++)
        tListData[i].SetSize(nRows);
}


void  SC_DoubleMatrix::SetNull()
{
    SetSize(AllocSize());
    for ( int i = 0; i < Size(); i++)
        tListData[i].SetNull();
}


void SC_DoubleMatrix::InsertCol(int beforeCol)
{
    if (buffer != 0)
        GenAppInternalError("SC_DoubleMatrix::InsertCol");

    int nRowSize = tListData[0].Size();

    if (IsFull())
        {
            if (!resizable)
                SC_ArrayError("InsertCol");

            int oldAlloc = AllocSize();
            ReAlloc(AllocSize() * 2 + 1);
            int nRowAlloc = tListData[0].AllocSize();
            for (int i = oldAlloc; i < AllocSize(); i++)
                {
                    tListData[i].Alloc(nRowAlloc);
                    tListData[i].SetNull();
                    tListData[i].SetSize(nRowSize);
                }
        }

    for (int i = Size(); i > beforeCol; i--)
        tListData[i] = tListData[i - 1];

    if (beforeCol < Size())
        SetSize(Size() + 1);
    else
        SetSize(beforeCol + 1);

    tListData[beforeCol].SetNull();
    tListData[beforeCol].SetSize(nRowSize);
}


void SC_DoubleMatrix::DeleteCol(int delCol)
{
    if (buffer != 0)
        GenAppInternalError("SC_DoubleMatrix::DeleteCol");

    if (delCol < Size())
        {
            for (int i = delCol; i < UpperBound(); i++)
                tListData[i] = tListData[i + 1];
            tListData[UpperBound()].SetNull();
            SetSize(Size() - 1);
        }
    else
        tListData[delCol].SetNull();
}


void SC_DoubleMatrix::MatrixFill(const double& fVal)
{
    if (IsEmpty() || (*this)[0].IsEmpty())
        return;

    int nCol = Size();
    int nRow = (*this)[0].Size();

    for (int i = 0; i < nCol; i++)
        {
            SC_DoubleArray& col = (*this)[i];
            for (int j = 0; j < nRow; j++)
                col[j] = fVal;
        }
}


void SC_DoubleMatrix::Multiply(const SC_DoubleMatrix& a, const SC_DoubleMatrix& b)
{
    // square only for now
    int n = a.Size();
    if ((a[0].Size() != n) || (b.Size() != n) || (b[0].Size() != n))
        GenAppInternalError("SC_DoubleMatrix::Multiply");

    MatrixAllocAndSetSize(n, n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            {
                double temp = 0.0;
                for (int k = 0; k < n; k++)
                    temp += a[i][k] * b[k][j];
                (*this)[i][j] = temp;
            }
}


void SC_DoubleCube:: CubeAlloc(int nPage, int nCols, int nRows)
{
    DeAlloc();
    if ((nPage == 0) && (nCols == 0) && (nRows == 0))
        return;

    if ((nCols < 1) || (nPage < 1))
        GenAppInternalError("SC_DoubleMatrix::ChkAlloc");

    // set pages
    Alloc(nPage);

    //  each column
    for (int i = 0; i < nPage; i++)
        tListData[i].MatrixAlloc(nCols, nRows);
}

void SC_DoubleCube::CubeSetSize(int nPage, int nCols, int nRows)
{
    SetSize(nPage);
    for (int i = 0; i < nPage; i++)
        tListData[i].MatrixSetSize(nCols, nRows);

}

void SC_DoubleCube::CubeAllocAndSetSize(int nPage, int nCols, int nRows)
{
    CubeAlloc(nPage, nCols, nRows);
    CubeSetSize(nPage, nCols, nRows);
}

void SC_DoubleCube::SetNull()
{
    SetSize(AllocSize());
    for (int i = 0; i < Size(); i++)
        tListData[i].SetNull();
}



