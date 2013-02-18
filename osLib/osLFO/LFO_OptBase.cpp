///////////////////////////////////////////////////////////////////////////////////
//
// LFO_OptBase.cpp
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
//      Listing functional objects for optimizer results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osLFO/LFO_OptBase.h>


LFO_OptBase :: LFO_OptBase(const char* listID, ListDefC& assLst)
    : ListObjC(listID, assLst)
{
}

LFO_OptBase::LFO_OptBase(const LFO_OptBase& a, ListDefC& assLst) : ListObjC(a, assLst)
{
    LocalCopy(a);
}

LFO_OptBase :: ~LFO_OptBase ()
{
}


LFO_OptBase& LFO_OptBase::operator= (const LFO_OptBase& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  LFO_OptBase::LocalCopy(const LFO_OptBase& a)
{
}


void LFO_OptBase::ListUtriMatrix(const char*              matID,
                                 const SC_StringArray&  titles,
                                 const SC_DoubleMatrix& matData,
                                       int              maxCol,
                                       int              col1Width,
                                       int              colnWidth)
{
    int matSize = titles.Size();
    int ntableCols = min(matSize, maxCol) + 1;

    SC_IntArray colWidth(ntableCols, colnWidth);
    colWidth[0] = col1Width;
    SC_BoolArray leftJust(ntableCols, false);
    leftJust[0] = true;
    TableSetup(colWidth, leftJust, 1, 0);

    int nmatPages = matSize / maxCol;
    if ((matSize % maxCol) != 0)
        nmatPages++;

    int rowStart = 0;
    while (rowStart < matSize)
    {
        if (rowStart > 0)
            AddBlankLine();

        int nRow = min(rowStart + maxCol, matSize) - rowStart;

        int colStart = rowStart;
        while (colStart < matSize)
        {
            if (colStart > rowStart)
                AddBlankLine();

            int nCol = min(colStart + maxCol, matSize) - colStart;

            SetTableCol(0, matID);
            for (int j = 0; j < nCol; j++)
                SetTableCol(j + 1, titles[colStart + j]);
            AddNextLine();

            for (int k = 0; k < nRow; k++)
            {
                int firstCol = 0;
                if (rowStart == colStart)
                    firstCol = k;

                SetTableCol(0, titles[rowStart + k]);
                for (int j = firstCol; j < nCol; j++)
                    SetTableReal(j + 1, matData[colStart + j][rowStart + k]);
                AddNextLine();
            }

            colStart += maxCol;
        }
        rowStart += maxCol;
    }
}



void LFO_OptBase::ListMatrix(const char*              matID,
                             const SC_StringArray&  colTitles,
                             const SC_StringArray&  colUnits,
                             const SC_StringArray&  rowTitles,
                             const SC_DoubleMatrix& matData,
                                   int              maxCol,
                                   int              col1Width,
                                   int              colnWidth)
{
    int nmatCols = colTitles.Size();

    int ntableCols = min(nmatCols, maxCol) + 1;

    SC_IntArray colWidth(ntableCols, colnWidth);
    colWidth[0] = col1Width;

    SC_BoolArray leftJust(ntableCols, false);
    leftJust[0] = true;

    TableSetup(colWidth, leftJust, 1, 0);

    int stCol  = 0;
    while (stCol < nmatCols)
    {

        if (stCol > 0)
            AddBlankLine();

        int nCol = min(stCol + maxCol, nmatCols) - stCol;

        {
            SetTableCol(0, matID);
            for (int j = 0; j < nCol; j++)
                SetTableCol(j + 1, colTitles[stCol + j]);
            AddNextLine();
        }

        if (!colUnits.IsEmpty())
        {
            for (int j = 0; j < nCol; j++)
                SetTableCol(j + 1, colUnits[stCol + j]);
            AddNextLine();
            AddBlankLine();
        }

        for (int k = 0; k < rowTitles.Size(); k++)
        {
            // blank row titles is blank
            if (StringLength(rowTitles[k]) > 0)
            {
                SetTableCol(0, rowTitles[k]);
                for (int j = 0; j < nCol; j++)
                    SetTableReal(j + 1, matData[stCol + j][k]);
            }
            AddNextLine();
        }

        stCol += maxCol;
    }

    AddBlankLine();
}


void LFO_OptBase::ListMatrix(const char*              matID,
                             const SC_StringArray&  colTitles,
                             const SC_StringArray&  rowTitles,
                             const SC_DoubleMatrix& matData,
                                    int           maxCol,
                                    int           col1Width,
                                    int           colnWidth)
{
    ListMatrix(matID, colTitles, SC_StringArray(), rowTitles, matData, maxCol, col1Width,  colnWidth);
}



void LFO_OptBase::ListJacobian(const DC_Jacobian& jacData)
{
    const OptVarDescriptionArray& parDesc = jacData.parDesc;
    const OptFitComponentDescriptionArray&  fitComp = jacData.fitDesc.fitComponentDescription;

    int nPar = parDesc.Size();
    int nFit = fitComp.Size();

    SC_DoubleMatrix  jacVals;
    jacVals.MatrixAllocAndSetSize(nFit + 1, nPar + 1);

    SC_StringArray colTitles, rowTitles;
    colTitles.Alloc(nFit + 1);
    rowTitles.Alloc(nPar + 1);

    int i;
    for (i = 0; i < nPar; i++)
    {
        rowTitles += parDesc[i].shortVarDesc;

        double dp = parDesc[i].parSigma / jacData.derivSpan;

        jacVals[nFit][i] = 0.0;
        for (int j = 0; j < nFit; j++)
        {
            jacVals[j][i] = 0.0;

            const OptFitComponentDescription& currFit = fitComp[j];

            for ( int k = 0; k < currFit.nfitPoints; k++)
            {
                double dr = jacData.jacobData[j][i][k] / currFit.fitChisqSigma;
                jacVals[j][i] += fabs(dr * dp);
            }

            jacVals[nFit][i] += jacVals[j][i];

        }
    }


/*               FOR i : WORD := 1 TO n_ov_par_vals DO BEGIN

                    set_par_id_str(ov_par_link[i], ov_val_link[i]);
                    row_title[i] := par_id;

                    dp := ov_par_sigma[i] / ov_deriv_span;

                    jac_vals[i, ov_ec_n_data_type + 1] := 0.0;
                    FOR j : WORD := 1 TO ov_ec_n_data_type DO BEGIN
                         jac_vals[i, j] := 0.0;
                         st_indx:= ov_ec_st_indx[j] - 1;
                         FOR k : WORD := 1 TO ov_ec_n_pts[j] DO BEGIN
                              dr := ov_jacobean_dr^[i]^[k + st_indx] /
                                        ov_chisq_sigma^[k + st_indx];
                              jac_vals[i, j] := jac_vals[i, j] + ABS(dr * dp);
                         END;
                         jac_vals[i, ov_ec_n_data_type + 1] := jac_vals[i, ov_ec_n_data_type + 1] + jac_vals[i, j];
                    END;
               END;
*/

    jacVals[nFit][nPar] = 0.0;
    for (i = 0; i < nFit; i++)
    {
        colTitles += fitComp[i].fitID;

        jacVals[i][nPar] = 0.0;
        for (int j = 0; j < nPar; j++)
            jacVals[i][nPar] += jacVals[i][j];

        jacVals[nFit][nPar] += jacVals[i][nPar];
    }

    colTitles += "Total";
    rowTitles += "Total";

/*

               jac_vals[n_ov_par_vals + 1, ov_ec_n_data_type + 1] := 0.0;
               FOR j : WORD := 1 TO ov_ec_n_data_type DO BEGIN
                    jac_vals[n_ov_par_vals + 1, j] := 0.0;
                    col_title[j] := ov_ec_id[j];
                    FOR i : WORD := 1 TO n_ov_par_vals DO
                         jac_vals[n_ov_par_vals + 1, j] :=
                              jac_vals[n_ov_par_vals + 1, j] + jac_vals[i, j];

                    jac_vals[n_ov_par_vals + 1, ov_ec_n_data_type + 1] :=
                         jac_vals[n_ov_par_vals + 1, ov_ec_n_data_type + 1] +
                              jac_vals[n_ov_par_vals + 1, j];
               END;


*/
    SetDefaultFormat(SC_SciFormat(5));

    ListMatrix("Totals", colTitles, rowTitles, jacVals);

    SC_DoubleMatrix  parNormVals;
    parNormVals.MatrixAllocAndSetSize(nFit, nPar);

    for (i = 0; i < nFit; i++)
        for (int j = 0; j < nPar; j++)
            parNormVals[i][j] = jacVals[i][j] / jacVals[i][nPar] * 100;;

    // drop "totals"
    colTitles.nlistData--;
    rowTitles.nlistData--;

    SetDefaultFormat(SC_DecFormat(2));

    ListMatrix("Parameter%", colTitles, rowTitles, parNormVals);

    SC_DoubleMatrix  fitNormVals;
    fitNormVals.MatrixAllocAndSetSize(nPar, nFit);
    for (i = 0; i < nPar; i++)
        for (int j = 0; j < nFit; j++)
            fitNormVals[i][j] = jacVals[j][i] / jacVals[nFit][i] * 100;

    ListMatrix("Fit%", rowTitles, colTitles, fitNormVals);
}

void LFO_OptBase::ListCovar(const DC_FullCovar& covarData)
{
    SC_StringArray titles;
    titles.Alloc(covarData.Size());
    for (int i = 0; i < covarData.Size(); i++)
        titles += covarData[i].shortVarDesc;

    SetDefaultFormat(SC_SciFormat(3));

    if (StringLength(covarData.covarID) > 15)
    {
        AddText(covarData.covarID, 0);
        AddNextLine();
        ListUtriMatrix(NULL, titles, covarData.covarMatrix, 6, 15, 12);
    }
    else
        ListUtriMatrix(covarData.covarID, titles, covarData.covarMatrix, 6, 15, 12);
}


void LFO_OptBase::ListCovarCorr(const DC_FullCovar& covarData)
{
    DC_FullCovar listCov = covarData;

    SC_DoubleMatrix& listMat =  listCov.covarMatrix;
    SC_DoubleMatrix  tempMat =  listMat;

    for (int i = 0; i < covarData.Size(); i++)
    {
        double tempI = tempMat[i][i];

        for (int j = 0; j < covarData.Size(); j++)
        {
            double tempJ = tempMat[j][j];
            if ((tempI > 0.0) && (tempJ > 0.0))
                listMat[i][j] = listMat[i][j] / (sqrt(tempI) * sqrt(tempJ));
            else
                listMat[i][j] = 0.0;
        }
    }

    ListCovar(listCov);
}


void LFO_OptBase::SetList95()
{
    SC_IntArray colWidth(5, 16);
    SC_BoolArray leftJust(5, false);
    leftJust[0] = true;
    leftJust[1] = true;

    TableSetup(colWidth, leftJust, 2, 0);

    SetTableCol(0, "Name");
    SetTableCol(1, "Units");
    SetTableCol(2, "FitValue");
    SetTableCol(3, "LowerValue");
    SetTableCol(4, "UpperValue");
    AddNextLine();
    AddBlankLine();

    SetDefaultFormat(SC_SciFormat(5));
}

void LFO_OptBase::List95(const char*        covarTitle1,
                         const char*        covarTitle2,
                         const DC_FullCovar& covarData)
{
    char covarTitle[80];
    if (covarTitle2 == NULL)
    {
        CopyString(covarTitle, covarTitle1, 80);
    }
    else
    {
        MakeString(covarTitle, covarTitle1, "\\", covarTitle2, 80);
    }
    AddText(covarTitle, 0);
    AddNextLine();


    for (int i = 0; i < covarData.Size(); i++)
    {
        SetTableCol(0, covarData[i].shortVarDesc);
        SetTableCol(1, covarData[i].varUnits);
        SetTableReal(2, covarData[i].DeNormalize(covarData.covarFitVals[i]));

        if (covarData.CovarOK(i))
        {
            double minC, maxC;
            covarData.CalcSingleLimits(DC_Covar::cl_954, i, minC, maxC);
            SetTableReal(3, covarData[i].DeNormalize(minC));
            SetTableReal(4, covarData[i].DeNormalize(maxC));
        }
        else
        {
            SetTableCol(3, "???");
            SetTableCol(4, "???");
        }
        AddNextLine();
    }
    AddBlankLine();
}




