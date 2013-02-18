///////////////////////////////////////////////////////////////////////////////////
//
// SC_DoubleArray.h
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with || without
// modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sandia Corporation nor the
//      names of its contributors may be used to endorse || promote products
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

#ifndef SC_DOUBLEARRAY_H
#define SC_DOUBLEARRAY_H

#include <genClass/T_SC_Array.h>
#include <genClass/SC_IntArray.h>

class SC_DoubleArray: public T_SC_Array<double> {
public :

    SC_DoubleArray() : T_SC_Array<double>() {};               //default cons
    SC_DoubleArray(int nAll) : T_SC_Array<double>(nAll) {};   //allocates to size nAll
    SC_DoubleArray(int nAll, double iVal) : T_SC_Array<double>(nAll, iVal) {};   //allocates to size nAll
    SC_DoubleArray(double* buffer, int bufSize):T_SC_Array<double>(buffer, bufSize) {}

    //  type specific methods
    void            Clear();
    void            SetNull();  // fills to alloc with nullReal, sets size to alloc

    //  methods skip entries with nullReal value (see C_Common.h)
    void            CalcMinMax(double& minVal, double& maxVal)  const;  // get min max for array
    void            AddToMinMax(double& minVal, double& maxVal) const;  // modifies min max if warranted

    // added for NED
    bool            WithinLimits(const double& minVal, const double& maxVal)  const;  // check tthat all non-nulls are within limits

    //  null real checking
    bool            DataOK(int index)   const;          // returns true if not nullReal in [index]
    bool            DataOK()            const;          // returns true if at least one entry is OK
    bool            AllDataOK()         const;          // returns true if all are non null
    int             GetnDataOK()        const;          // returns count of DataOK;
    void            Cleanup();  // repacks from start with non-null entries

    //  common math
    bool            Log10();    // takes log10 of data, returns true if at least one is +ve
    void            InvLog10();    // inverse log10 of data -- underflows to 0.
    bool            Log();    // takes ln of data, returns true if at least one is +ve
    void            Exp();    // inverse ln of data -- underflows to 0.
    double          Sum() const;


    bool            IsAscending(bool strict = true) const;      // if strict is true, then x_i must be > x_i-1
                                                                // if strict is false, then x_i >= x_i-1
                                                                // and no nulls

    //  methods don't skip entries with nullReal value (C_Common.h)
    bool            Sort(bool ascending);                         // QSort - returns false if stack overflow

    void            Sort(bool         ascending,                  // carries along indexes
                         SC_IntArray& indexes);

    void            Reorder(const SC_IntArray& indexes);

    bool            SameAs(const SC_DoubleArray& a, const double& epsilon) const;
};

#ifndef LAPACK
#define LAPACK
#endif //!LAPACK

class SC_DoubleMatrix : public T_SC_Array<SC_DoubleArray> {

private:
public:
#ifdef LAPACK
    double*         buffer; // used by FixedMatrixAlloc for x64 performance problem
#endif // LAPACK

    SC_DoubleMatrix() : T_SC_Array<SC_DoubleArray>() {buffer = 0;}               //default cons
    SC_DoubleMatrix(int nAll) : T_SC_Array<SC_DoubleArray>(nAll) {buffer = 0;}   //allocates to size nAll
    SC_DoubleMatrix(int nCols, int nRows);  // does MatrixAllocAndSetSize

    //  type specific methods
    void            FixedMatrixAlloc(int nCols, int nRows);  // uses one allocation to buffer, then pointers into it
    void            FixedMatrixAllocAndSetSize(int nCols, int nRows);

    void            MatrixAlloc(int nCols, int nRows);
    void            MatrixSetSize(int nCols, int nRows);
    void            MatrixAllocAndSetSize(int nCols, int nRows);
    void            SetNull();  // fills all rows to alloc with nulls sets size to alloc

    void            MatrixFill(const double& fVal);  // fills to set sizes with fVal
    void            Multiply(const SC_DoubleMatrix& a, const SC_DoubleMatrix& b);


    //  column ops - will abort if used with FizMatrixAlloc
    void            InsertCol(int beforeCol);
    void            DeleteCol(int delCol);

    // virtuals for fixed support
    virtual void    DeAlloc();

};

#ifdef LAPACK


// the following matrix is packed in Lapack banded matrix form -- WE ASSUME SYMMETRIC
// HOWEVER, the [][] operator is overloaded so that they are used to translate between
// the banded form used in the 2D and 1D nonlinear solvers, which is essentially transposed and shifted
// This is done as a compromise to not change all of the code that sets up the matrices and hence
// changing the convention of the software.  At the same time, this saves us from having to create new
// memory and performing copies before every call to a banded Lapack solver.

class SC_LapackBandedMatrix
{

    protected:

        int kl;     //number of lower diagonals
        int ku;     //number of upper diagonals
        int ldb;    //size of non-leading edge (leading edge of RHS)
        int ldab;   //size of leading edge, the matrix should be of size ldb x ldab where ldab = 2kl+ku+1

        friend class proxy;     //because indexing using [][] is a tricky thing.

        class proxy
        {
            public:

                proxy(SC_LapackBandedMatrix &m, int ix): m_(m), indx(ix) {}
                double &operator[](int indy)
                    {
                        int bad = 0;
                           //on diagonal
                        if(indx == m_.kl)
                        {
                            int j = indy;
                            if(j<0 || j>m_.ldb)
                            {
                                bad = 1;
                            }
                            int ind = j*m_.ldab+m_.kl+m_.ku;
                            if(ind >= 0 && ind < m_.size && !bad)
                            {
                                return(m_.buffer[j*m_.ldab+m_.kl+m_.ku]);
                            }
                        }
                        //on super diagonal
                        else if(indx > m_.kl)
                        {
                            int i = indx - m_.kl - 1;
                            int j = indy;
                            if(i<0 || i>m_.ku || j<0 || j>(m_.ldb-i-1))
                            {
                                bad = 1;
                            }
                            int ind = m_.kl+m_.ku-i-1+(j+i+1)*m_.ldab;
                            if(ind >= 0 && ind < m_.size && !bad)
                            {
                                return(m_.buffer[m_.kl+m_.ku-i-1+(j+i+1)*m_.ldab]);
                            }
                        }
                        //on sub diagonal
                        else //(indx < m_.kl)
                        {
                            int i = m_.kl-1-indx;
                            int j = indy-i-1;
                            if(i<0 || i>m_.ku || j<0 || j>(m_.ldb-i-1))
                            {
                                bad = 1;
                            }
                            int ind = m_.kl+m_.ku+1+i+j*m_.ldab;
                            if(ind >= 0 && ind < m_.size && !bad)
                            {
                                return(m_.buffer[m_.kl+m_.ku+1+i+j*m_.ldab]);
                            }
                        }
                        //need a default return for bad value assignments
                        return(m_.buffer[0]); //this value is never used so it will be harmless
                    }

            protected:

                SC_LapackBandedMatrix &m_;
                int indx;
        };


    public:

        double* buffer;

        SC_LapackBandedMatrix() {buffer = 0; nrows = 0; ncols = 0; size = 0;}               //default cons
        SC_LapackBandedMatrix(int nCols, int nRows)    {buffer = 0; MatrixAlloc(nCols, nRows);}

        ~SC_LapackBandedMatrix() {if(buffer) delete buffer;}               //default cons

        //  type specific methods
        void            FixedMatrixAlloc(int nCols, int nRows) {MatrixAlloc(nCols, nRows);}
        void            FixedMatrixAllocAndSetSize(int nCols, int nRows) {MatrixAlloc(nCols, nRows);}

        void            MatrixAlloc(int nCols, int nRows)
                        {
                            if(buffer)
                            {
                                delete buffer;
                            }
                            //assume symmetry
                            ncols = nCols;
                            nrows = nRows;
                            kl=ku=(ncols-1)/2;
                            ldab=2*kl+ku+1;
                            ldb=nrows;
                            size = ldb*ldab;
                            buffer = new double[size];
                        }
        void            MatrixSetSize(int nCols, int nRows) {MatrixAlloc(nCols, nRows);}
        void            MatrixAllocAndSetSize(int nCols, int nRows) {MatrixAlloc(nCols, nRows);}
        void            MatrixFill(double val)
                        {
                            for(int i = 0; i < size; i++)
                            {
                                buffer[i] = val;
                            }
                        }
        proxy           operator[](int ix) {return proxy(*this, ix);}
        int             nrows;
        int             ncols;
        int             size;

};
#endif // LAPACK

class SC_DoubleCube  : public T_SC_Array<SC_DoubleMatrix> {

public :

    SC_DoubleCube() : T_SC_Array<SC_DoubleMatrix>() {};               //default cons
    SC_DoubleCube(int nAll) : T_SC_Array<SC_DoubleMatrix>(nAll) {};   //allocates to size nAll

    //  type specific methods
    void            CubeAlloc(int nPage, int nCols, int nRows);
    // allocates nPage SC_DoubleMatrix
    void            CubeSetSize(int nPage, int nCols, int nRows);
    void            CubeAllocAndSetSize(int nPage, int nCols, int nRows);
    void            SetNull();  // fills all rows to alloc with nulls sets size to alloc
};

#endif // SC_DOUBLEARRAY_H

