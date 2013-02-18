///////////////////////////////////////////////////////////////////////////////////
//
// C_Sample.cpp
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
//      Main optimizer/sampler code and global variables and other support classes used by optimizer and application.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/C_Common.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Matrix.h>

#include <osMain/C_Sample.h>

DC_TableData   Sampler::sampleResults;
int            Sampler::nsampVar;
SampVarArray   Sampler::sampVarData;
bool           Sampler::sampleResultsOK = false;
bool           Sampler::sampleResultsAvailable = false;


bool        Sampler::SamplesAreAvailable()      {return sampleResultsAvailable;}
void        Sampler::ClearAvailableSamples()    {sampleResultsAvailable = false;}

bool        Sampler::SamplesAreOK()             {return  sampleResultsOK;}

bool  Sampler::SamplesCurrent()
{
    if (!sampleResultsAvailable)
        return false;

    // a bunch of  paranoi checks
    if (numberOfTrials != sampleResults.dataTable[0].Size())
        return false;

    SampVarArray currSamp;
    if (currSamp.Size() != sampleResults.dataTable.Size())
        return false;

    for (int i = 0; i < currSamp.Size(); i++)
    {
        if (currSamp[i] != sampVarData[i])
            return false;
        if (currSamp[i]->sampledValues == 0)
            return false;
    }

    return true;
}


void  Sampler::SiftXI(SC_DoubleArray&   xf,
                      SC_IntArray&      ky,
                      int               n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (xf[i] > xf[j])
            {
                int ik = ky[i];
                ky[i] = ky[j];
                ky[j] = ik;

                double ix = xf[i];
                xf[i] = xf[j];
                xf[j] = ix;
            }
}

/*
PROCEDURE SIFTXI (VAR XF : real_ptr;
                  VAR KY : int_ptr;
                      N  : int);
VAR
     ix : REAL8;
     ik : int;
BEGIN
     FOR i : int := 1 TO n - 1 DO
          FOR j : int := i + 1 TO n DO
               IF xf^[i] > xf^[j] THEN
                    BEGIN
                         ik := ky^[i];
                         ky^[i] := ky^[j];
                         ky^[j] := ik;

                         ix := xf^[i];
                         xf^[i] := xf^[j];
                         xf^[j] := ix;
                    END;
END;
 */


bool Sampler::InitSamplingLocals(SC_SetupErr& errData)
{
    //  clear prev data
    for (SampVar* o = SampVar::GetFirst(); o; o = o->GetNext())
        o->sampledValues = 0;

    nsampVar = SampVar::GetnActive();
    if (nsampVar == 0)
    {
        errData.SetConstantError("no sampled variables specified");
        return false;
    }
    SampVar::GetActiveArray(sampVarData);

    int i;
    int maxIDLen = 0;
    for (i = 0; i < nsampVar; i++)
    {
        int idLen = StringLength(sampVarData[i]->GetShortID());
        if (idLen > maxIDLen)
            maxIDLen = idLen;
    }

    if (!sampleResults.Alloc(nsampVar, numberOfTrials, maxIDLen + 5))
    {
        errData.SetConstantError("cannot allocate for samples");
        return false;
    }

    for (i = 0; i < nsampVar; i++)
    {
        SC_DoubleArray& currCol = sampleResults.dataTable[i];
        currCol.SetSize(numberOfTrials);
        for (int j = 0; j < numberOfTrials; j++)
            currCol[j] = double(j);

        sampleResults.SetTitle(i, sampVarData[i]->GetShortID());
        sampVarData[i]->sampledValues = &currCol;
    }
    sampleResults.dataTable.SetSize(nsampVar);


    return true;
}

void Sampler::DeAllocSamplingLocals()
{
}

//   ********************************************** correlation matrix stuff



/*
PROCEDURE CorrExpand;
--
-- Use the correlations in CRAW and the mapping in IdCor to expand CRAW to
-- include all nonfixed variables.  Temporarily use CORX for storage.
--

WITH
     lhs_data_globals;

VAR
     ld, m, mm, ldiag, nv2  : int;
BEGIN
     NV2 := lhsd_nvar * (lhsd_nvar + 1) DIV 2;
     FOR L : int := 1 TO NV2 DO
          CORX^[L] := 0.0;

     FOR L : int := 1 TO lhsd_nvar DO BEGIN
          LDiag := L * (L + 1) DIV 2;
          CORX^[LDiag] := 1.0;
     END;

     FOR L : int := 2 TO lhss_ncorr DO BEGIN
          LD := L * (L - 1) DIV 2;
          M := lhss_IdCor^[L];
          MM := M * (M - 1) DIV 2;
          FOR J : int := 1 TO L - 1 DO
               CORX^[MM + lhss_IdCor^[J]] := lhss_CRAW^[LD + J];
     END;

     FOR I : int := 1 TO NV2 DO BEGIN
        lhss_CRAW^[I] := CORX^[I];
        CORX^[I] := 0.0;
     END;
END;


PROCEDURE FixCRAW;
--
-- 1. User provides correlations, and nonspecified are not to be forced to
--    zero, then only worry about variable order.
-- 2. User provides correlations, and nonspecified are to be forced to zero,
--    then expand CRAW to NV by NV.
-- 3. No correlations supplied, and none are to be induced then nothing to do.
-- 4. No correlations supplied, but all are to be induced is the shortcut way
--    for the user to supply the identity matrix.  So the code provides it.
--
WITH
     lhs_data_globals;
VAR
     ii, jj, ij, iijj, nv2, nc2, idiag : int;
     InOrder : BOOLEAN;
--
-- If the user provided correlations, the variable numbers (IdCor) conformed
-- to the global numbering system that includes both fixed and nonfixed.
-- Convert to a local numbering system, ie, only count nonfixed variables.
-- The code requires that IdCor be in increasing order, so re-order if nec.
--

BEGIN
     IF lhsc_AnyCorrInfo THEN
          BEGIN
               InOrder := TRUE;
               FOR L : int := 2 TO lhss_ncorr DO
                    IF lhss_idcor^[L] <= lhss_idcor^[L - 1] THEN
                         BEGIN
                              InOrder := FALSE;
                              EXIT;
                         END;

               IF NOT InOrder THEN
                    BEGIN
                         nc2 := lhss_ncorr * (lhss_ncorr + 1) DIV 2;

                         FOR L : int := 1 TO nc2 DO
                              CORX^[L] := 0.0;

                         FOR L : int := 1 TO lhss_ncorr DO BEGIN
                              IdNdx^[L] := L;
                              CORX^[L * (L + 1) DIV 2] := 1.0;
                         END;

                         SIFTIJ(lhss_idcor, IdNdx, lhss_ncorr);

                         FOR I : int := 2 TO lhss_ncorr DO BEGIN
                              II := IdNdx^[I];
                              FOR J : int := 1 TO I - 1 DO BEGIN
                                   JJ := IdNdx^[J];
                                   IJ := I * (I - 1) DIV 2 + J;
                                   IIJJ := MAX(II, JJ) * (MAX(II, JJ) - 1) DIV 2 + MIN(II, JJ);
                                   CORX^[IJ] := lhss_CRAW^[IIJJ];
                              END;
                         END;

                         FOR I : int := 1 TO nc2 DO BEGIN
                              lhss_CRAW^[I] := CORX^[I];
                              CORX^[I] := 0.0;
                         END;
                    END;

               IF (lhsd_nvar > lhss_ncorr) AND lhsc_InduceCorr THEN
                    CorrExpand;
          END

     ELSE IF (NOT lhsc_AnyCorrInfo) AND lhsc_InduceCorr THEN
          BEGIN
               NV2 := lhsd_nvar * (lhsd_nvar + 1) DIV 2;
               FOR L : int := 1 TO NV2 DO
                    lhss_CRAW^[L] := 0.0;

               FOR L : int := 1 TO lhsd_nvar DO BEGIN
                    lhss_idcor^[L] := L;
                    IDiag := L * (L + 1) DIV 2;
                    lhss_CRAW^[IDiag] := 1.0;
               END;
          END;
END;

*/

void  Sampler::Ranker(SC_DoubleArray& rx,
                      int             n)
// Replace the N entries of vector RX with their ranks.  Ranks start at 0.0
{
    SC_IntArray ir;
    ir.AllocAndIndex(n);

    SiftXI(rx, ir, n);

    SC_DoubleArray r(n);

    r[0] = 0.0;
    int js = 0;
    int i;
    for (i = 1; i < n; i++)
    {
        r[i] = double(i);
        if (rx[i] > rx[i - 1])
        {
            js = 0;
        }
        else
        {
            js++;
            for (int j = i - js; j < i; j++)
                r[j] += 0.5;

            r[i] -= 0.5 * double(js);
        }
    }

    for (i = 0; i < n; i++)
        rx[ir[i]] = r[i];
}


/*

PROCEDURE RANKER (VAR R  : real_ptr;
                  VAR RX : real_ptr;
                  VAR IR : int_ptr;
                      N  : int);
--
-- Replace the N entries of vector RX with their ranks.  Vector IR must
-- contain the integers 1 to N on entry and will contain the indexes of
-- RX prior sorting on return.  Vector R is used for working space.  Return
-- the ranks in RX.
--

VAR
     js : int;
BEGIN
     SIFTXI(RX, IR, N);
     R^[1] := 1.0;
     JS := 0;
     FOR I : int := 2 TO N DO BEGIN
          R^[I] := REAL8(I);
          IF RX^[I] > RX^[I - 1] THEN
               JS := 0
          ELSE
               BEGIN
                    JS := JS + 1;
                    FOR J : int := I - JS TO I - 1 DO
                         R^[J] := R^[J] + 0.5;
                    R^[I] := R^[I] - 0.5 * REAL8(JS);
               END;
     END;

     FOR I : int := 1 TO N DO
          RX^[IR^[I]] := R^[I];

END;
*/


void  Sampler::MixCorr(const SC_DoubleArray& corrX,  // decomposed correlation
                             int             n)
{
/*
-- Induce user-specified correlations on n variables.
-- 1. Generate an arbitrary population matrix (lhsc_ntrial x M).
-- 2. Find its associated correlation matrix (M x M).
-- 3. Symmetrically decompose the matrix and invert the decomposition.
-- 4. Symmetrically decompose the user-specified correlation (already done on input)
-- 5. Use the product from the matrices in 3 and 4 to create a
--    transformation matrix.
-- 6. Transform the population matrix to one having the desired correlations.
-- 7. Replace the transformed matrix with its ranks within each column.
--
-- Each column of the population matrix consists of the same lhsc_ntrial Van Der
-- Waerden scores randomly mixed.
-- Avoid anamolous perfect correlations by ensuring that max off-diagonal
-- element of the associated correlation matrix is less than 0.9999.
*/

/*
WITH
     msg_utilities,
     lhs_data_globals;

LABEL
     loop_end;
VAR
     Corr2Large : BOOLEAN;
     SSQ, SUM : REAL8;
     StdArea, AreaInc : REAL8;
     kk, iwit, ic, ij, ii, n2 : int;
BEGIN
 */

    double ssq = 0.0;
    double stdArea = 0.0;
    double areaInc = 1.0 / double(numberOfTrials + 1);
    int n2 = numberOfTrials / 2;

    SC_DoubleArray  rxv(numberOfTrials), x(numberOfTrials);

    int i;
    for (i = 0; i < n2; i++)
    {
        stdArea += areaInc;
        rxv[i] = NormInv(stdArea);
        rxv[numberOfTrials - i - 1] = -rxv[i];
        ssq += Sqr(rxv[i]);
    }

    if ((n2 + n2) != numberOfTrials)
        rxv[n2] = 0.0;

    ssq += ssq;

/*
     SSQ := 0.0;
     StdArea := 0.0;
     AreaInc := 1.0 / REAL8(numberOfTrials + 1);
     N2 := numberOfTrials DIV 2;
     FOR I : int := 1 TO N2 DO BEGIN
          StdArea := StdArea + AreaInc;
          lhss_rxv^[I] := norminv(StdArea);
          lhss_rxv^[numberOfTrials - I + 1] := -lhss_rxv^[I];
          SSQ := SSQ + SQR(lhss_rxv^[I]);
     END;

     IF (N2 + N2) <> numberOfTrials THEN
          lhss_rxv^[N2 + 1] := 0.0;

     SSQ := SSQ + SSQ;
*/


// We have the scores and their sum of squares, start the mixing.

    SC_IntArray ll;
    ll.AllocAndIndex(numberOfTrials);
    bool corrTooLarge = true;

/*
     FOR I : int := 1 TO numberOfTrials DO
          LL^[I] := I;
     Corr2Large := TRUE;
*/

    // grab ref for ease of use
    SC_DoubleMatrix& sampRes = sampleResults.dataTable;

    SC_DoubleArray corrRaw(n * (n + 1) / 2);
    SC_DoubleArray lTri;

    while (corrTooLarge)
    {
        for (i = 0; i < n; i++)
        {
            sampleRandGen.RandomMix(ll);
            for (int j = 0; j < numberOfTrials; j++)
                sampRes[i][j] = rxv[ll[j]];
        }



/*
     WHILE Corr2Large DO BEGIN

          FOR I : int := 1 TO M DO BEGIN
               random_mix (LL, numberOfTrials);
               FOR J : int := 1 TO numberOfTrials DO
                    lhsd_data^[i]^[j] := lhss_rxv^[LL^[J]];
          END;
 */

// We have a population matrix in XX, find its correlation matrix.
        corrRaw[0] = 1.0;
        bool corrOK = true;
        for (i = 2; i <= n; i++)
        {
            int ii = i * (i - 1) / 2;
            corrRaw[i + ii - 1] = 1.0;

            int ij;
            for (int j = 1; j < i; j++)
            {
                double sum = 0.0;
                for (int k = 0; k < numberOfTrials; k++)
                    sum += sampRes[i - 1][k] * sampRes[j - 1][k];
                ij = j + ii - 1;
                corrRaw[ij] = sum / ssq;
            }
            if (corrRaw[ij] >= 0.9999)
            {
                corrOK = false;
                break;
            }
        }

/*
          lhss_CRAW^[1] := 1.0;
          FOR I : int := 2 TO M DO BEGIN
               II := I * (I - 1) DIV 2;
               lhss_CRAW^[I + II] := 1.0;
               FOR J: int := 1 TO I - 1 DO BEGIN
                    SUM := 0.0;
                    FOR K : int := 1 TO numberOfTrials DO
                         SUM := SUM + lhsd_data^[i]^[K] * lhsd_data^[j]^[K];
                    IJ := J + II;
                    lhss_CRAW^[IJ] := SUM / SSQ;
               END;
               IF ABS(lhss_CRAW^[IJ]) >= 0.9999 THEN
                    GOTO loop_end;
          END;
 */

// Decompose the correlation matrix and invert the factor.  Stored in Q.
// If process fails (extremely unlikely) try a new mix.
        if (corrOK)
        {
            try
            {
                CholeskyUTri(corrRaw, n, lTri);
                MatInvLTri(lTri, n);
                corrTooLarge = false;
            }
            catch (MathError)      // singular
            {

            }
        }
    }

/*
          IF NOT Cholesky (M, lhss_CRAW, Q) THEN
               CYCLE;

          IF NOT MatInv (M, Q) THEN
               CYCLE;

          Corr2Large := FALSE;

loop_end : ;
     END;
 */

// Multiply the transpose of the inverted matrix (Q) by the transpose of the
// decomposition of the user-specified matrix (CORX) to get the
// transformation matrix, and place it into CRAW.

    int k;
    for (k = 1; k <= n; k++)
    {
        int kk = k * (k - 1) / 2;
        for (int it = 1; it <= k; it++)
        {
            double sum = 0.0;
            for (int iw = it; iw <= k; iw++)
            {
                int iwit = it + iw * (iw - 1) / 2;
                sum += lTri[iwit - 1] * corrX[iw + kk - 1];
            }
            corrRaw[it + kk - 1] = sum;
        }
    }

/*
     FOR K : int := 1 TO M DO BEGIN
          KK := K * (K - 1) DIV 2;
          FOR IT : int := 1 TO K DO BEGIN
               SUM := 0.0;
               FOR IW : int := IT TO K DO BEGIN
                    IWIT := IT + IW * (IW - 1) DIV 2;
                    SUM := SUM + Q^[IWIT] * CORX^[IW + KK];
               END;
               lhss_CRAW^[IT + KK] := SUM;
          END;
     END;
 */

//  Transform the randomly mixed scores population matrix. XX * CRAW -> XX.
    rxv.FillToAlloc(0.0);

/*     FOR I : int := 1 TO numberOfTrials DO
          lhss_rxv^[I] := 0.0;
 */

    for (k = n; k > 0; k--)
    {
        int ic = k * (k - 1) / 2;
        for (int j = 1; j <= k; j++)
        {
            ic++;
            for (i = 0; i < numberOfTrials; i++)
                rxv[i] +=  sampRes[j - 1][i] * corrRaw[ic - 1];
        }
        for (i = 0; i < numberOfTrials; i++)
        {
            sampRes[k - 1][i] = rxv[i];
            rxv[i] = 0.0;
        }
    }

/*     FOR K : int := M DOWNTO 1 DO BEGIN
          IC := K * (K - 1) DIV 2;
          FOR J : int := 1 TO K DO BEGIN
               IC := IC + 1;
               FOR I : int := 1 TO numberOfTrials DO
                    lhss_rxv^[I] := lhss_rxv^[I] + lhsd_data^[j]^[I] * lhss_CRAW^[IC];
          END;
          FOR I : int := 1 TO numberOfTrials DO BEGIN
               lhsd_data^[K]^[I] := lhss_rxv^[I];
               lhss_rxv^[I] := 0.0;
          END;
     END;
*/

// Replace the entries of the XX array with their ranks
    for (int j = 0; j < n; j++)
        Ranker(sampRes[j], numberOfTrials);

/*
     FOR J : int := 1 TO M DO BEGIN
          FOR I : int := 1 TO numberOfTrials DO BEGIN
               lhss_X^[I] := lhsd_data^[j]^[I];
               LL^[I] := I;
          END;
          RANKER (lhss_rxv, lhss_X, LL, numberOfTrials);
          FOR I : int := 1 TO numberOfTrials DO
               lhsd_data^[j]^[I] := lhss_X^[I];

          update_progress_bar(j);
     END;

     clear_progress_bar;
 */

}


bool  Sampler::SetupCorrelations(SC_SetupErr& errData)
{
    // basic correlation setup
    SC_DoubleMatrix  sampCorr;
    sampCorr.MatrixAlloc(nsampVar, nsampVar);

    //  indexes of vars with correlations
    SC_IntArray corrIndexes;

    int ncorrVar = 0;
    if (specifyCorrelations)
    {
        // extract user set
        corrIndexes.Alloc(nsampVar);
        int i;
        for (i = 0; i < nsampVar; i++)
        {
            bool nonNullFound = forceCorrelationsToZero;
            for (int j = 0; j < nsampVar; j++)
            {
                double cVal = sampVarData[i]->GetCorrelation(sampVarData[j]);

                // if no correlation set to 0 if forced
                if (RealIsNull(cVal)&& forceCorrelationsToZero)
                    cVal = 0.0;

                // is i correlated to any j ?
                if ((i != j) && (!RealIsNull(cVal)))
                    nonNullFound = true;

                sampCorr[i][j] = cVal;
            }

            if (nonNullFound)
                corrIndexes += i;
        }

        ncorrVar = corrIndexes.Size();
        if ((ncorrVar < nsampVar) && (ncorrVar > 0))
        {
            // need to reduce to just correlated
            for (i = 0; i < ncorrVar; i++)
                for (int j = 0; j < ncorrVar; j++)
                    sampCorr[i][j] = sampCorr[corrIndexes[i]][corrIndexes[j]];
        }


        // set all nulls to 0.0 in correlated vars
        for (i = 0; i < ncorrVar; i++)
            for (int j = 0; j < ncorrVar; j++)
                if (RealIsNull(sampCorr[i][j]))
                    sampCorr[i][j] = 0.0;

    }
    else
        if (forceCorrelationsToZero)
        {
            corrIndexes.AllocAndIndex(nsampVar);
            for (int i = 0; i < nsampVar; i++)
            {
                for (int j = 0; j < nsampVar; j++)
                    sampCorr[i][j] = 0.0;
                sampCorr[i][i] = 1.0;
            }
            ncorrVar = nsampVar;
        }


    if (ncorrVar > 1)
    {
        SC_DoubleArray corrX, utri;
        MatrixToUTri(sampCorr, ncorrVar, utri);
        try
        {
            CholeskyUTri(utri, ncorrVar, corrX);
        }
        catch (MathError)      // singular
        {
            errData.SetConstantError("correlation decomposition failure");
            return false;
        }
        MixCorr(corrX, ncorrVar);
    }

    if ((ncorrVar < nsampVar) || (nsampVar == 1))
    {
        // random mixing on non-correlated
        SC_IntArray ll;
        ll.AllocAndIndex(numberOfTrials);

        // grab ref for ease of use
        SC_DoubleMatrix& corrRes = sampleResults.dataTable;

        for (int j = nsampVar - 1; j >= 0; j--)
        {
            int colj = -1;
            int i;
            for (i = 0; i < ncorrVar; i++)
                if (corrIndexes[i] == j)
                    colj = i;

            if (colj >= 0)
            {
                corrRes[j] = corrRes[colj];
            }
            else
            {
                sampleRandGen.RandomMix(ll);
                for (i = 0; i < numberOfTrials; i++)
                    corrRes[j][i] = double(ll[i]);
            }
        }
    }

    return true;
}


/*

FUNCTION lhss_setup_corr;
LABEL
     err_exit;

     PROCEDURE do_error(CONST err_str : STRING);
     BEGIN
          setup_err := err_str;
          lhss_setup_corr := FALSE;
          GOTO err_exit;
     END;

WITH
     lhs_data_globals;
VAR
     ColJSet : BOOLEAN;
     colj : int;
BEGIN
     lhss_setup_corr := TRUE;
--
-- Correlations (if any) were read into the CRAW array NC x NC. If non-user-
-- specified correlations are to be forced to zero (InduceCorr := TRUE), then
-- CRAW must be expanded to NV x NV or must be defined as the identity matrix
-- if none read in.   In either case, we then induce the CRAW correlations.
--

     FixCRAW;

        //  basic corr matrix
        if (specifyCorrelations || forceCorrelationsToZero)
        {
            sampCorrMatrix.MatrixAllocAndSetSize(nsampVar, nsampVar);
            SC_BoolArray corrsFound(nsampVar);
            // fill
            for (int i = 0; i < nsampVar; i++)
            {
                bool nonNullFound = forceCorrelationsToZero;

                for (int j = 0; j < nsampVar; j++)
                {
                    double cVal = nullReal;
                    if (specifyCorrelations)
                        cVal = sampVarData[i].GetCorrelation(sampVarData[j]);

                    if (RealIsNull(cVal)&& forceCorrelationsToZero)
                        cVal = 0.0;

                    if ((i != j) && (!RealIsNull(cVal))
                        nonNullFound = true;

                    sampCorrMatrix[i][j] = cVal;

                }
                sampCorrMatrix[i][i] = 1.0;  // by definition
                corrsFound[i] = nonNullFound;
                if (nonNullFound)
                    corrIndexes += i;
            }
            ncorrVar = corrIndexes.Size();

            if (ncorrVar < nsampVar)
            {
                // reset matrix to just correlated values



            }
        }
        else
            ncorrVar = 0;



     IF lhsc_inducecorr THEN
--
-- If only one nonfixed variable, set up random mixing.  Else, decompose
-- user-specified correlation matrix, store in CORX.  Put ranks that honor
-- the correlations into XX.
--
          IF lhsd_nvar > 1 THEN
               BEGIN
                    IF NOT Cholesky (lhsd_nvar, lhss_CRAW, CORX) THEN
                         do_error('Correlation structure bad - would not decompose');
                    MIX(lhsd_nvar);
               END
          ELSE
               BEGIN
                    FOR I : int  := 1 TO numberOfTrials DO
                         LL^[I] := I;

                    random_mix(LL, numberOfTrials);

                    FOR I : int  := 1 TO numberOfTrials DO
                         lhsd_data^[1]^[I] := REAL8(LL^[I]);
               END
      ELSE
--
-- Here the user wants random mixing for non-specified correlations.
--
          IF lhss_ncorr > 0 THEN
               BEGIN
--
-- Decompose user-specified correlation matrix, store in CORX.  Put ranks
-- that honor the correlations into XX.  Then, to account for uncorrelated
-- variables re-slot XX and insert random columns.
--

                    IF NOT Cholesky (lhss_ncorr, lhss_CRAW, CORX) THEN
                         do_error('Correlation structure bad - would not decompose');

                    MIX(lhss_ncorr);

                    IF lhss_ncorr < lhsd_nvar THEN
                         BEGIN
                              FOR I : int  := 1 TO numberOfTrials DO
                                   LL^[I] := I;

                              FOR J : int := lhsd_nvar DOWNTO 1 DO BEGIN
                                   ColJSet := FALSE;
                                   FOR I : int := 1 TO lhss_ncorr DO
                                        IF lhss_idcor^[I] = J THEN
                                             BEGIN
                                                  ColJSet := TRUE;
                                                  colj := i;
                                             END;

                                   IF ColJSet THEN
                                        BEGIN
                                             FOR I : int  := 1 TO numberOfTrials DO
                                                  lhsd_data^[j]^[I] := lhsd_data^[colj]^[I];
                                        END
                                   ELSE
                                        BEGIN
                                             random_mix (LL, numberOfTrials);
                                             FOR I : int  := 1 TO numberOfTrials DO
                                                  lhsd_data^[j]^[I] := REAL8(LL^[i]);
                                        END;
                              END;
                         END;
               END
          ELSE
--
-- Totally random mixing, induce no correlations.
--
               BEGIN
                    FOR I : int  := 1 TO numberOfTrials DO
                         LL^[I] := I;
                    FOR J : int := 1 TO lhsd_nvar DO BEGIN
                         random_mix (LL, numberOfTrials);
                         FOR I : int  := 1 TO numberOfTrials DO
                              lhsd_data^[j]^[I] := REAL8(LL^[I]);
                    END;
               END;

err_exit : ;

END;


*/


// main driver
bool Sampler::DoSampling(SC_SetupErr& errData)
{
    sampleResultsOK = false;
    sampleResultsAvailable = false;

    if (!InitSamplingLocals(errData))
        return false;

    sampleRandGen.SetupRandom(randSeed);  // reseed

    // correlations
    if (!SetupCorrelations(errData))
        return false;

    SC_DoubleArray samples, temp;

    bool limitsOK = true;
    for (int i = 0; i < nsampVar; i++)
    {
        // get samples
        try
        {
            sampVarData[i]->Sample(samples);
        }
        catch (MathError& err)
        {
            errData.SetInternalError("sampling", err.errStr);
            return false;
        }

        SC_DoubleArray& currSamp = sampleResults.dataTable[i];
        if (!sampVarData[i]->IsTableLookup())
        {
            // sorting or correlations for table lookup
            // if not LHS then sort
            if (!samplingIsLHS)
                samples.Sort(true);

            // apply  correlations
            temp = samples;
            for (int j = 0; j < numberOfTrials; j++)
            {
                int k = int(floor(currSamp[j] + 0.1));
                samples[j] = temp[k];
            }
        }

        // update table
        currSamp = samples;

        // check limits
        bool parLimitOK = true;
        for (int j = 0; j < numberOfTrials; j++)
            if (!sampVarData[i]->CheckLimits(j))
                parLimitOK = false;

        if (!parLimitOK)
        {
            errData.SetInternalError(sampVarData[i]->GetShortID(), "sampled value out of range");
            limitsOK = false;
        }
    }

    sampleResultsOK = limitsOK;
    sampleResultsAvailable = true;

    return limitsOK;

/*

     FOR j : int := 1 TO lhsd_nvar DO BEGIN

          WITH lhsc_var_data[j].var_data DO
               CASE dist OF
                    lhs_dt_normal        : lhss_sample_normal(b_val, a_val, FALSE);
                    lhs_dt_lognormal     : lhss_sample_normal(b_val, a_val, TRUE);
                    lhs_dt_uniform       : lhss_sample_uniform(a_val, c_val, FALSE);
                    lhs_dt_loguniform    : lhss_sample_uniform(a_val, c_val, TRUE);
                    lhs_dt_triangular    : lhss_sample_triangle(a_val, b_val, c_val, FALSE);
                    lhs_dt_logtriangular : lhss_sample_triangle(a_val, b_val, c_val, TRUE);
               END;

          update_progress_bar(j);
--
-- Order the sampled observations for variable J according to the pre-
-- established ranking in the XX array.  Put back into the XX array.
--
--
-- If doing random (Monte Carlo) sampling must sort the sample prior to
-- re-ordering it.  Although this is not necessary for totally random mixing,
-- it is done to ensure the ranks and data are consistent.
--
          IF NOT lhsc_UseLHS THEN
               lhss_sort_real(lhss_X, lhsc_NTrial);

          FOR I : int := 1 TO  lhsc_NTrial DO
               lhss_RXV^[I] := lhss_X^[I];

          FOR I : int := 1 TO  lhsc_NTrial DO BEGIN
               k := TRUNC(lhsd_data^[j]^[I] + 0.1);
               lhss_X^[I] := lhss_RXV^[K];
          END;

          FOR I : int := 1 TO  lhsc_NTrial DO
               lhsd_data^[j]^[I] := lhss_x^[I];

     END;
     clear_progress_bar;

     lhss_null_work_vars;

     FOR j : int := 1 TO lhsd_nvar DO BEGIN

          WITH lhsc_var_data[j] DO
               CASE var_type OF
                    lpd_par : WITH all_param[var_indx] DO BEGIN
                                   v_max := p_max;
                                   v_min := p_min;
                              END;
                    lpd_seqp : BEGIN
                                   v_max := 1.0E+06;
                                   v_min := 0.0;
                              END;
                    lpd_seqq : BEGIN
                                   v_max := 10.0;
                                   v_min := -10.0;
                              END;
                    lpd_dcap : BEGIN
                                   v_max := 100000.0;
                                   v_min := 0.0;
                              END;
               END;

          FOR i : int := 1 TO  lhsc_ntrial DO
               IF (lhsd_data^[j]^[i] > v_max) OR (lhsd_data^[j]^[i] < v_min) THEN
                    BEGIN
                         err_str := lhsc_var_data[j].var_id || ' sample out of range';
                         next_lhs_error(err_str);
                         RETURN;
                    END;
     END;
*/

}


