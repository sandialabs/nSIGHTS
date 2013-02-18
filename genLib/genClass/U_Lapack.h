///////////////////////////////////////////////////////////////////////////////////
//
// U_Lapack.h
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
//      External definitions for linking in FORTRAN Lapack routines from a
//  Lapack library.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef U_LAPACK_H
#define U_LAPACK_H

#include <genClass/SC_DoubleArray.h>
//#include <genClass/T_SC_Array.h>
//#include <genClass/T_SC_BasicPtrArray.h>


//namespace LAPACKmatrixSolvers {

#ifdef LAPACK
extern "C" {
    //general matrix solver, Ax=b
    void dgels_(const char *trans, const int *M, const int *N, const int *nrhs,
        double *A, const int *lda, double *b, const int *ldb, double *work,
        const int * lwork, int *info);

    // LU decomposition of a general matrix, A
    void dgetrf_(int* M, int *N, double* A, int* lda, int* ipiv, int* info);

    // generate inverse using its LU decomposition
    void dgetri_(int* N, double* A, int* lda, int* ipiv, double* work, int* lwork, int* info);

    //Solve a tridiagonal matrix using Gaussian elimination with partial pivoting
    //May want to write a new Thomas algorithm if this seems too slow
    void dgtsv_(int *N, int *NRHS, double *DL, double *D, double *DU, double *b, int *ldb, int *info);


    //The general banded matrix solver to solve Ax=b
    void dgbsv_(int *N, int *KL, int *KU, int *nrhs, double *AB, int *ldab, int *ipiv, double *b, int *ldb, int *info);

    //obtain the eigenvalues for a symmetric matrix
    void dsyev_(const char *jobz, const char *uplo, int *N, double *A, int *lda, double *w, double *work, int *lwork, int *info);
}


void LAPACKInvert(SC_DoubleMatrix& a, int n);
#endif // LAPACK


//};


#endif // U_LAPACK_H

