///////////////////////////////////////////////////////////////////////////////////
//
// SC_IntArray.h
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

#ifndef SC_INTARRAY_H
#define SC_INTARRAY_H

#include <genClass/T_SC_Array.h>

class SC_IntArray: public T_SC_Array<int> {

public :
    //  constructors
    SC_IntArray():T_SC_Array<int>() {}
    SC_IntArray(int nAll):T_SC_Array<int>(nAll) {}
    SC_IntArray(int nAll, int initVal):T_SC_Array<int>(nAll, initVal) {}
    SC_IntArray(int* buffer, int bufSize):T_SC_Array<int>(buffer, bufSize) {}


    //  type specific methods

    bool            operator==(const SC_IntArray& a) const;


    void            Reset(int maxList);         // ReAllocs to maxlist if necc. sets all list items
    // to between 0 and maxlist - 1
    void            Clear();

    void            AllocAndIndex(int maxIndex);          // Allocs to maxIndex, fills with 0 to maxIndex - 1

    bool            SearchForKey(int  key,
                                 int& findIndx) const;  // findIndx is set to indx if true
    // or indx to insert key after if false
    bool            SearchForKey(int  key) const;       // returns true if key is in list


    bool            CheckLimits(int minLim,             // returns true if all are between
                                int maxLim) const;      // min and max (exclusive)

    void            GetMinMax(int& minVal, int& maxVal) const;
    int             GetMin() const;
    int             GetMax() const;
    int                 GetCount() const; // sum

    int             PosValueCount() const;            // returns # of elements >= 0

    void            AddIfNotFound(int inVal);     // searches through unordered list and
    // adds inVal if not in list

    void            MakeMap(SC_IntArray& mapVector) const;  // creates map vector based on -1
    // input values

    void            MakeReverseMap(int   maxReverse,                  // makes vector of size maxReverse
                                   const SC_IntArray& keepIndexes);   // with all entries -1 except those in keepIndexes
    // which are numbered in ascending indexes

    void            SetReverseOK();  // processes zero and +ve entries into index order for reverse mapping


    void            Sort(bool ascending);                         // stupid sort for now
    // may reimplement in template later
    void            Sort(bool         ascending,                  // carries along indexes
                         SC_IntArray& indexes);

    bool            Pack();                         //  deletes entries < 0
    //  returns true if not empty

    void            IncrementIndexes(const SC_IntArray& indexes);
    void            ClearIndexes(const SC_IntArray& indexes);


};

class SC_IntMatrix : public T_SC_Array<SC_IntArray> {

public :

    SC_IntMatrix() : T_SC_Array<SC_IntArray>() {};               //default cons
    SC_IntMatrix(int nAll) : T_SC_Array<SC_IntArray>(nAll) {};   //allocates to size nAll

    //  type specific methods
    void            MatrixAlloc(int nCols, int nRows);
    void            MatrixAllocResizable(int nCols, int nRows);
    //only number of rows is resizable
    void            MatrixSetSize(int nCols, int nRows);
    void            MatrixAllocAndSetSize(int nCols, int nRows);

    void            SetNull();  // fills all entry with - 1
    void            MatrixFill(int iVal);  // fills to set sizes with iVal

    void            MatrixReAlloc(int nCols, int nRows);
    void            MatrixSetResizable(int defaultAlloc = 0);

    int             MatrixSize() const; // count of all items


};


class SC_IntCube  : public T_SC_Array<SC_IntMatrix> {

public :

    SC_IntCube() : T_SC_Array<SC_IntMatrix>() {};               //default cons
    SC_IntCube(int nAll) : T_SC_Array<SC_IntMatrix>(nAll) {};   //allocates to size nAll

    //  type specific methods
    void            CubeAlloc(int nPage, int nCols, int nRows);
    // allocates nPage SC_IntMatrix
};

#endif // SC_INTARRAY_H

