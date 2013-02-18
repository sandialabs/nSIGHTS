///////////////////////////////////////////////////////////////////////////////////
//
// U_Matrix.cpp
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

#include <cfloat>

#include <genClass/C_Common.h>

#include <genClass/SC_RealFormat.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/C_TxtFile.h>

#include <genClass/U_Real.h>
#include <genClass/U_Matrix.h>


namespace matrixSolvers {

////////////////////////////////////////////////////////////////////

void MatrixToUTri(const SC_DoubleMatrix&   mat,  // converts n x n mat into packed vector utri
                                       int                n,
                                       SC_DoubleArray&    utri)
{
    utri.AllocAndSetSize(n * (n + 1) / 2);
    int next = 0;
    for (int i = 0; i < n; i ++)
        for (int j = 0; j <= i; j ++)
            utri[next++] = mat[i][j];
}

void CholeskyUTri(const SC_DoubleArray&    utri,  // solves packed vector utri
                        int                n,     // solution returned in ltri
                        SC_DoubleArray&    ltri)  // throws MathError if singular
{
/*
VAR
     m2, j, j2, jj, ij, jmin1, ji, jplus, ii, ik, jk : int;
 */

    int n2 = n * (n + 1) / 2;
    ltri.AllocAndFill(n2, 0.0);         // initialize

/*
     M2 := M * (M + 1) DIV 2;
     FOR I : int := 1 TO M2 DO
          ltri^[I] := 0.0;
*/
    int j = 1;
    int i;
    for (i = 1; i <= n; i++)
    {
        int ij = j + i * (i - 1) / 2;
        ltri[ij - 1] = utri[ij - 1];
    }


/*     j := 1;
     FOR i : int := 1 TO m DO BEGIN
          ij := j + i * (i - 1) DIV 2;
          ltri^[ij] := matin^[ij];
     END;
*/

    //  loop forever
    while (true)
    {
        j++;
        int j2 = j * (j - 1) / 2;
        int jj = j + j2 - 1;            // array bound
        int jmin1 = j - 1;
        for (i = 1; i <= jmin1; i++)
        {
            int ji = i + j2 - 1;        // array bound
            ltri[jj] += ltri[ji] * ltri[ji];
        }

/*          J := J + 1;
          J2 := J * (J - 1) DIV 2;
          JJ := J + J2;
          JMIN1 := J - 1;
          FOR I : int := 1 TO JMIN1 DO BEGIN
               JI := I + J2;
               ltri^[JJ] := ltri^[JJ] + ltri^[JI] * ltri^[JI];
          END;
*/

        if ((1.0 - ltri[jj]) <= 0.0)
            throw MathError("CholeskiUTri - singular");

        ltri[jj] = sqrt(1.0 - ltri[jj]);
        if (j >= n)
            return;

/*
          IF (1.0 - ltri^[JJ]) <= 0.0 THEN
               RETURN(FALSE);
          ltri^[JJ] := SQRT(1.0 - ltri^[JJ]);
          IF J >= M  THEN
               RETURN;
 */
        int jplus = j + 1;
        for (i = jplus; i <= n; i++)
        {
            int ii = i * (i - 1) / 2;
            int ij = j + ii - 1;        // array bound

            for (int k = 1; k <= jmin1; k++)
            {
                int ik = k + ii;
                int jk = k + j2;
                ltri[ij] += ltri[ik - 1] * ltri[jk - 1];
            }
            ltri[ij] = (utri[ij] - ltri[ij]) / ltri[jj];
        }

/*
          JPLUS := J + 1;
          FOR i : int := JPLUS TO M DO BEGIN
               II := I * (I - 1) DIV 2;
               IJ := J + II;
               FOR k : int := 1 TO JMIN1 DO BEGIN
                    IK := K + II;
                    JK := K + J2;
                    ltri^[IJ] := ltri^[IJ] + ltri^[IK] * ltri^[JK];
               END;
               ltri^[IJ] := (matin^[IJ] - ltri^[IJ]) / ltri^[JJ];
          END;
*/
    }
}

void MatInvLTri(SC_DoubleArray&    ltri,
                int                n)
{

//
// Subroutine to invert a M x M lower triangular matrix, ltri.
//
    int i;
    for (i = 1; i <= n; i++)
    {
        int ii = i * (i + 1) / 2 - 1;   // array bound
        if (ltri[ii] == 0.0)
            throw MathError("MatInvLTri - singular");
        ltri[ii] = 1.0 / ltri[ii];
    }

/*
     FOR I : int := 1 TO M DO BEGIN
          II := I * (I + 1) DIV 2;
          IF mat^[II] = 0.0 THEN
               RETURN(FALSE);
          mat^[II] := 1.0 / mat^[II];
     END;
 */
    int k = n;
    do
    {
        int kk = k * (k - 1) / 2;
        int j = k - 1;
        do
        {
            double sum = 0.0;
            int jplus = j + 1;
            for (int i = jplus; i <= k; i++)
            {
                int ki = i + kk;
                int ij = j + i * (i - 1) / 2;
                sum += ltri[ki - 1] * ltri[ij - 1];
            }
            int kj = j + kk;
            int jj = j * (j + 1) / 2;
            ltri[kj - 1] = - sum * ltri[jj - 1];
            j--;
        } while ( j > 0);
        k--;

    } while (k > 1);

/*
     K := M;
     REPEAT
          KK := K * (K - 1) DIV 2;
          J := K - 1;
          REPEAT
               SUM := 0.0;
               JPLUS := J + 1;
               FOR I : int := JPLUS TO K DO BEGIN
                    KI := I + KK;
                    IJ := J + I * (I - 1) DIV 2;
                    SUM := SUM + mat^[KI] * mat^[IJ];
               END;
               KJ := J + KK;
               JJ := J * (J + 1) DIV 2;
               mat^[KJ] := -SUM * mat^[JJ];
               J := J - 1;
          UNTIL J = 0;
          K := K - 1;
     UNTIL K = 1;
END;

*/

}

///////////////////////////////////////////////////////////////////////////

}; // end NS

