///////////////////////////////////////////////////////////////////////////////////
//
// U_Lapack.cpp
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

#include "GenLibAfx.h"
#include <genClass/U_Lapack.h>
#include <genClass/SC_DoubleArray.h>
//#include <genClass/T_SC_Array.h>
//#include <genClass/T_SC_BasicPtrArray.h>

void LAPACKInvert(SC_DoubleMatrix& a, int n)
{
    //for now we do not assume that A has been allocated using fixed memory allocation, so we
    //must first create our own block of memory.
    SC_DoubleMatrix A;
    A.FixedMatrixAllocAndSetSize(n,n);

    //copy to continuous memory block
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            A.buffer[i*n+j]=a[i][j];
        }
    }


    int *ipiv = new int[n+1];
    int lwork = n*n;
    double *work = new double[lwork];
    int info;

    dgetrf_(&n, &n, (double *) A.buffer, &n, ipiv, &info);
    dgetri_(&n, (double *) A.buffer, &n, ipiv, work, &lwork, &info);

    if(info == 0) //if successfull copy back
    {
        //copy to continuous memory block
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                a[i][j]=A.buffer[i*n+j];
            }
        }
    }

    delete [] ipiv;
    delete [] work;
}



