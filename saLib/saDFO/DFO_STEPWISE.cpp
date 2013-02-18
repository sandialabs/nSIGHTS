///////////////////////////////////////////////////////////////////////////////////
//
// DFO_STEPWISE.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <saDFO/DFO_STEPWISE.h>

const double DFO_STEPWISE::bigReal = 1.0E+35;


DFO_STEPWISE :: DFO_STEPWISE() :
    DFO_SAObjBase("STEPWISE")
{
    optBACKWARD = false;
    optRANK     = false;
    optPRESS    = false;
    optSTAND01  = false;
    optSTEPWISE = true;
    optSigin    = 0.05;
    optSigout   = 0.05;

    optFORCE    = false;
    optWEIGHT   = false;
    weightIndex = 0;
    optDROP     = false;

    outMEAN     = true;
    outCORR     = true;
    outSSCP     = true;
    outINVERSE  = true;
    outSTEPS    = true;
    outRESIDUALS= true;
    outOTHER    = true;

    decForm     = SC_DecFormat(5);
    sciForm     = SC_SciFormat(5);

    CopyString(title, "STEPWISE - mView", 80);


    InitCommon();
}

DFO_STEPWISE::DFO_STEPWISE(const DFO_STEPWISE& a) : DFO_SAObjBase(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_STEPWISE::InitCommon()
{
    allRawVarDO.tableData = &allRawVarDC;
    AddOutPort(allRawVarDO);
    allProcessedVarDO.tableData = &allProcessedVarDC;
    AddOutPort(allProcessedVarDO);

    selRawVarDO.tableData = &selRawVarDC;
    AddOutPort(selRawVarDO);
    selProcessedVarDO.tableData = &selProcessedVarDC;
    AddOutPort(selProcessedVarDO);

    pressTableDO.tableData = &pressTableDC;
    AddOutPort(pressTableDO);

    residTableDO.tableData = &residTableDC;
    AddOutPort(residTableDO);

    corrTableDO.tableData = &corrTableDC;
    AddOutPort(corrTableDO);

    DoStatusChk();
}

DFO_STEPWISE& DFO_STEPWISE::operator= (const DFO_STEPWISE& a)
{
    if (&a != this)
    {
        DFO_SAObjBase::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_STEPWISE::LocalCopy(const DFO_STEPWISE& a)
{
    optBACKWARD = a.optBACKWARD;
    optRANK     = a.optRANK;
    optPRESS    = a.optPRESS;
    optSTAND01  = a.optSTAND01;
    optSTEPWISE = a.optSTEPWISE;
    optSigin    = a.optSigin;
    optSigout   = a.optSigout;

    optFORCE    = a.optFORCE;
    forceIndexes = a.forceIndexes;
    optWEIGHT   = a.optWEIGHT;
    weightIndex = a.weightIndex;
    optDROP     = a.optDROP;
    dropIndexes = a.dropIndexes;

    outMEAN     = a.outMEAN;
    outCORR     = a.outCORR;
    outSSCP     = a.outSSCP;
    outINVERSE  = a.outINVERSE;
    outSTEPS    = a.outSTEPS;
    outRESIDUALS= a.outRESIDUALS;
    outOTHER    = a.outOTHER;

    decForm     = a.decForm;
    sciForm     = a.sciForm;
}

void  DFO_STEPWISE:: DoStatusChk()
{
    DFO_SAObjBase::DoStatusChk();
    if (StatusNotOK())
        return;

    if (optBACKWARD && optSTEPWISE)
        SetObjErrMsg("cannot select both BACKWARD and STEPWISE");

    if (optSTEPWISE && (optSigout < optSigin))
        SetObjErrMsg("SIGOUT must be >= SIGIN for STEPWISE");

    if (optRANK && optSTAND01)
        SetObjErrMsg("cannot select both RANK and STAND01");

    if (GetDepVar().GetNStep() > 1)
        SetObjErrMsg("dependent variables can have only 1 step");


    if (GetDepVar().GetNVector() < 2)
        SetObjErrMsg("must be more than 2 observations");

    niv = GetIndepVar().GetNVariable();
    nvar = niv + GetDepVar().GetNVariable();

    if (optFORCE)
    {
        if (forceIndexes.IsEmpty())
            SetObjErrMsg("no FORCEd variables selected");
        for (int i = 0; i < forceIndexes.Size(); i++)
            if ((forceIndexes[i] < 0) || (forceIndexes[i] >= nvar))
                SetObjErrMsg("FORCEd variables selection bad ??");
    }

    if (optWEIGHT)
    {
        if ((weightIndex < 0) || (weightIndex >= niv))
            SetObjErrMsg("WEIGHT variable selection bad ??");

        if (optFORCE && forceIndexes.SearchForKey(weightIndex))
            SetObjErrMsg("WEIGHT variable cannot be forced ??");

        if (niv == 1)
            SetObjErrMsg("no variables remaining after WEIGHT var is removed");
    }

    if (optDROP)
    {
        if (dropIndexes.IsEmpty())
            SetObjErrMsg("no DROPped observations");
        nobs = GetDepVar().GetNVector();
        for (int i = 0; i < dropIndexes.Size(); i++)
            if ((dropIndexes[i] < 0) || (dropIndexes[i] >= nobs))
                SetObjErrMsg("DROPped observation selection bad ??");
    }

}

void DFO_STEPWISE::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    InitListingData();

    // generate input table
    MakeInputTable();

    InputListing();

    weights.AllocAndFill(nobs, 1.0);  // all weights set to 1.0
    if (optWEIGHT)
        weights = GetIndepVar().saData[0][weightIndex];


    CalcCorrelations();

    int ndv = GetDepVar().GetNVariable();

    // set up for PRESS table
    if (optPRESS)
    {
        pressTableDC.Alloc(ndv + 1, 100);
        pressTableDC.SetID("PRESS table");
        pressTableDC.columnDesc.SetString("Step", 0);
    }
    else
    {
        pressTableDC.ClearAll();
        pressTableDC.SetID("No PRESS results");
    }

    // and resid table
    if (outRESIDUALS)
    {
        if (optRANK)
            residTableDC.Alloc(7, nobs);
        else
            residTableDC.Alloc(8, nobs);
    }
    else
    {
        residTableDC.ClearAll();
        residTableDC.SetID("No Residuals");
    }

    SC_BoolArray ivSel(niv, false);

    SC_IntArray ivar;
    for (int i = 0; i < ndv; i++)
    {
        RegressVar(i + niv, i, ivar);
        ivSel.SetIndexes(ivar);
        if (optPRESS)
            pressTableDC.columnDesc.SetString(GetDepVar().GetVariableName(i), i + 1);
    }
    CloseListingData();

    // set DO_SAPlotTable constants
    allRawVarDO.nDV = ndv;
    allRawVarDO.nIV = niv;

    allProcessedVarDO.nDV = ndv;
    allProcessedVarDO.nIV = niv;

    // set-up selected output tables
    ivar.Alloc(nvar);

    // add selected ivar
    for (int i = 0; i < niv; i++)
        if (ivSel[i])
            ivar += i;
    int nivSel = ivar.Size();

    // and all dv next
    for (int i = 0; i < ndv; i++)
        ivar += i + niv;

    int nselCol = ivar.Size();
    if (!(selRawVarDC.Alloc(nselCol, nobs) && selProcessedVarDC.Alloc(nselCol, nobs)))
    {
        SetObjErrMsg("sel table allocations ??");
        return;
    }

    selRawVarDC.rowDesc = allRawVarDC.rowDesc;
    selProcessedVarDC.rowDesc = allProcessedVarDC.rowDesc;

    for (int i = 0; i < nselCol; i++)
    {
        int selIndx = ivar[i];
        const char* colString = allRawVarDC.columnDesc[selIndx];
        selRawVarDC.columnDesc.SetString(colString, i);
        selProcessedVarDC.columnDesc.SetString(colString, i);

        selRawVarDC.dataTable[i] = allRawVarDC.dataTable[selIndx];
        selProcessedVarDC.dataTable[i] = allProcessedVarDC.dataTable[selIndx];
    }

    selRawVarDC.dataTable.MatrixSetSize(nselCol, nobs);
    selProcessedVarDC.dataTable.MatrixSetSize(nselCol, nobs);

    selRawVarDC.SetID("Selected Raw Variables");
    selProcessedVarDC.SetID("Selected Processed Variables");

    // set DO_SAPlotTable constants
    selRawVarDO.nDV = ndv;
    selRawVarDO.nIV = nivSel;

    selProcessedVarDO.nDV = ndv;
    selProcessedVarDO.nIV = nivSel;

    // and Press table
    if (optPRESS)
    {
        int npr = pressTableDC.dataTable[0].Size();
        pressTableDC.rowDesc.SetSize(npr);
        pressTableDC.rowDesc.SetIndexed("Step", 1);
        pressTableDC.dataTable.MatrixSetSize(ndv + 1, npr);
    }

}

void DFO_STEPWISE::InputListing()
{
    InitListingData();

    TextBigTitle(title);
    FullVariableList();

    const DC_SATable& depVar = GetDepVar();
    const DC_SATable& indepVar = GetIndepVar();

    TextStdTitle("STEPWISE Standard Listing");
    AddStdInt("Number of input independent variables", indepVar.GetNVariable());
    AddStdInt("Number of input dependent variables", depVar.GetNVariable());
    AddStdInt("Number of observations", depVar.GetNVector());
    AddBlankLine();

    if (optDROP)
    {
        AddStdInt("Number of DROPPed observations", dropIndexes.Size());

        const int ndropCol = 10;
        TableSetup(8, 0, 0, ndropCol);
        int colNum = 0;
        for (int i = 0; i < dropIndexes.Size(); i++)
        {
            SetTableInt(colNum++, dropIndexes[i] + 1);
            if (colNum == ndropCol)
            {
                AddNextLine();
                colNum = 0;
            }
        }
        if (colNum > 0)
            AddNextLine();

        AddStdInt("Number of observations to process", allRawVarDC.GetNRows());
        AddBlankLine();
    }

    if (optRANK)
    {
        AddLine("Do regression analysis on RANKED data");
    }
    else if (optSTAND01)
    {
        AddLine("Do regression analysis on STAND01 data");
    }
    else
    {
        AddLine("Do regression analysis on RAW data");
    }

    if (optSTEPWISE)
    {
        AddLine("Using STEPWISE procedure for ""best"" subset selection");
        AddStdReal("Sigin significance level", optSigin, decForm);
        AddStdReal("Sigout significance level", optSigout, decForm);
    }
    if (optBACKWARD)
    {
        AddLine("Using BACKWARD ELIMINATION procedure for ""best"" subset selection");
        AddStdReal("Significance level", optSigin, decForm);
    }
    if (optPRESS)
    {
        AddLine("Using PRedicted Errors Sum of Square (PRESS) to protect agains overfit");
    }

    int skipVar = -1;
    if (optWEIGHT)
        skipVar = weightIndex;
    VariablesNameList(skipVar);

    if (optFORCE)
    {
        AddLine("FORCE the following variables");
        for (int i = 0; i < forceIndexes.Size(); i++)
            AddLine(allRawVarDC.columnDesc[forceIndexes[i]], 5);
        AddBlankLine();
    }

    if (optWEIGHT)
    {
        AddStdText("WEIGHTing variable", indepVar.GetVariableName(weightIndex));
        AddLine("Note: WEIGHT variable removed from input independent vars");
        AddBlankLine();
    }

}


void DFO_STEPWISE::MakeInputTable()
{
    const DC_SATable& depVar = GetDepVar();
    const DC_SATable& indVar = GetIndepVar();

    niv  = indVar.GetNVariable();
    int skipID = -1;
    if (optWEIGHT)
    {
        niv--;
        skipID = weightIndex;
    }

    nvar = depVar.GetNVariable() + niv;
    nobs  = depVar.GetNVector();

    if (!allRawVarDC.Alloc(nvar, nobs))
        return;

    allRawVarDC.SetID("All Raw Variables");
    SC_StringArray& colDesc = allRawVarDC.columnDesc;
    {
        SC_DoubleMatrix& dataTable   = allRawVarDC.dataTable;
        int colNum = 0;
        for (int i = 0; i < indVar.GetNVariable(); i++)
        {
            if (i != skipID)
            {
                colDesc += indVar.GetVariableName(i);
                dataTable[colNum++] = indVar.saData[0][i];
            }
        }
        for (int i = 0; i < depVar.GetNVariable(); i++)
        {
            colDesc += depVar.GetVariableName(i);
            dataTable[colNum++] = depVar.saData[0][i];
        }

        // take out drops
        if (optDROP)
        {
            SC_BoolArray okRows(nobs, true);
            okRows.SetIndexes(dropIndexes, false);
            int nOK = 0;
            for (int i = 0; i < nobs; i++)
                if (okRows[i])
                {
                    if (nOK < i)
                    {
                        for (int j = 0; j < nvar; j++)
                            dataTable[j][nOK] = dataTable[j][i];
                    }
                    nOK++;
                }
            nobs = nOK;
        }

        dataTable.MatrixSetSize(nvar, nobs);

    }

    // and process
    allProcessedVarDC = allRawVarDC;
    allProcessedVarDC.SetID("All Processed Variables");
    {
        SC_DoubleMatrix& dataTable   = allProcessedVarDC.dataTable;
        for (int i = 0; i <  nvar; i++)
        {
            if (optRANK)
                RankArray(dataTable[i]);
            else
                if (optSTAND01)
                    StandardizeArray(dataTable[i]);
        }
    }

}


void DFO_STEPWISE::CalcCorrelations()
{
    // based on Sandia STEPWISE SUBROUTINE CALCCOR

    const SC_DoubleMatrix& varData = allProcessedVarDC.dataTable;

    // allocate
    varMean.AllocAndSetSize(nvar);  // means
    varStdDev.AllocAndSetSize(nvar);    // std dev

    varDiag.AllocAndSetSize(nvar);  // cross product
    varCorr.MatrixAllocAndSetSize(nvar, nvar);  // correlations

    // just allocate temps - actuall onlyt need niv
    varMeanR.Alloc(nvar);   // means
    varStdDevR.Alloc(nvar); // std dev
    varDiagR.Alloc(nvar);   // cross product
    varCorrDR.Alloc(nvar);
    varCorrR.MatrixAlloc(nvar, nvar);   // correlations
    varInvCorrDR.MatrixAlloc(nvar, nvar);

    // means
    for (int i = 0; i < nvar; i++)
    {
        double sum = 0.0;
        for (int j = 0; j < nobs; j++)
            sum += weights[j] * varData[i][j];
        varMean[i] = sum / double(nobs);
    }

    SC_DoubleMatrix sscp(nvar, nvar);   // sum of squares and cross product

    // sum of squares and cross products
    for (int i = 0; i < nvar; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            double ss = 0.0;
            for (int k = 0; k < nobs; k++)
            {
                double b1 = varData[i][k] - varMean[i];
                double b2 = varData[j][k] - varMean[j];
                ss += weights[k] * b1 * b2;
            }
            sscp[i][j] = ss;
            sscp[j][i] = ss;
        }
    }

    // outMEAN listing
    const SC_StringArray& varIDs = allProcessedVarDC.columnDesc;

    if (outMEAN)
    {
        TextStdTitle("Variable Statistics");
        const int cWid[] = {20, 15, 15, 15, 15, 15, -1};
        const bool lJust[] = {true, false, false, false, false, false};
        TableSetup(cWid, lJust);

        const char* colText[] = {"Variable", "Mean", "Variance", "Std. Dev", "Std. Err", "CV"};
        SetTableCol(colText);
        AddNextLine();
    }

    // variance, std dev, std err & coeff of var
    double sqrobs = sqrt(double(nobs));
    for (int i = 0; i < nvar; i++)
    {
        varDiag[i] = sscp[i][i];
        double var = varDiag[i] / double(nobs - 1);
        varStdDev[i] = sqrt(var);
        if (outMEAN)
        {
            double sterr = varStdDev[i] / sqrobs;
            double cv = 0.0;
            if (varMean[i] != 0.0)
                cv = 100.0 * varStdDev[i] / varMean[i];
            SetTableCol(0, varIDs[i]);
            SetTableReal(1, varMean[i], sciForm);
            SetTableReal(2, var, sciForm);
            SetTableReal(3, varStdDev[i], sciForm);
            SetTableReal(4, sterr, sciForm);
            SetTableReal(5, cv, sciForm);
            AddNextLine();
        }
    }

    if (outMEAN)
        AddBlankLine();

    // correlation matrix
    for (int i = 0; i < nvar; i++)
    {
        for (int j = 0; j < i; j++)
        {
            double b = varDiag[i] * varDiag[j];
            double corr = 0.0;
            if (b != 0.0)
                corr = sscp[i][j] / sqrt(b);
            varCorr[i][j] = corr;
            varCorr[j][i] = corr;
        }

        double b = varDiag[i];
        double corr = 0.0;
        if (b != 0.0)
            corr = sscp[i][i] / b;
        varCorr[i][i] = corr;
    }

    // add to output table
    corrTableDC.Alloc(nvar, nvar);
    corrTableDC.SetID("Corr. Matrix");
    corrTableDC.columnDesc = varIDs;
    corrTableDC.rowDesc = varIDs;
    corrTableDC.dataTable = varCorr;

    if (!outCORR)
        return;

    TextStdTitle("Full Correlation Matrix");
    ListSymmetricMatrix(varCorr, varIDs, decForm, 10, sfAll, 8, "VarID");

}

void DFO_STEPWISE::RegressVar(const int dvIndx,
                              const int dvNum,
                              SC_IntArray& ivar)
{
    // based on Sandia STEPWISE SUBROUTINE REGRVAR
    const char* dvStr = allProcessedVarDC.columnDesc[dvIndx];
    const SC_DoubleArray& yraw = allRawVarDC.dataTable[dvIndx];
    const SC_DoubleArray& yrank = allProcessedVarDC.dataTable[dvIndx];

    char outStr[256];
    MakeString(outStr, "Regression analysis for dependent variable -- ", dvStr , "--", 256);
    TextStdTitle(outStr);


    // calc rank stuff for later
    SC_DoubleArray rank, ysort;
    double sy, ssy;
    if (optRANK)
    {
        ysort = yraw;
        ysort.Sort(true);
        SetRanks(ysort, rank);

        sy = 0.0;
        ssy = 0.0;
        for (int i = 0; i < nobs; i++)
        {
            double yv = yraw[i];
            sy += yv * weights[i];
            ssy += yv * yv * weights[i];
        }
    }

    bool calcStep = true;
    bool printStep = outSTEPS;
    bool doResiduals = outRESIDUALS || optPRESS || optRANK;
    bool atEnd = false;

    int nsteps = 0;
    int nrsq = 0;

    // temporary arrays
    int iaSize = niv * 2 + 1;
    SC_DoubleArray rsqv(iaSize, 0.0);
    SC_IntArray ixrsq;
    ixrsq.SetResizable(iaSize);

    if (optSTEPWISE)
    {
        StepInit(dvIndx);
    }

    // used by optBACKWARD
    double probd = 0.0;
    int ivdel = -999;
    int nivR = niv;  // nivR = NIV in regrvar, niv = NIV0
    int nvarR = niv;

    ivar.AllocAndIndex(niv);
    if (optSTEPWISE)
    {
        ivar.AllocAndFill(niv, -1);
        nvarR = 0;
        ivar.SetEmpty();
    }
    else
    {
        ivar.AllocAndIndex(niv);
        nivR = niv;
    }

    // defined prior to use because they may be calcd on one iter and printed next
    int nrdf;
    double regss, regms, ressq, resms, regF, fprob, xg6;
    double rsq, b0;

    // fortran parameters for dv
    const double& ymean = varMean[dvIndx];
    const double& tssq = varDiag[dvIndx];

    SC_DoubleArray beta(niv), betas(niv), press(niv), time(nobs), yhat(nobs),
                    resi(nobs), yharaw(nobs), resraw(nobs);

    // main regression loop
    while (calcStep) {

        int nvarStep = nvarR;

        if (optSTEPWISE)
        {
            // perform step of stepwise regression

            StepStep(nvarStep, nvarR, ivar, atEnd);
            if (atEnd && (nvarR == 0))
            {
                AddLine("++ ERROR - no selected ind vars qualify for entry");
                return;
            }
        }

        if (optBACKWARD)
        {
            // delete previously selected variable in backward elimination process
            // note - act variable is ivdel - 1 because of C vs FORTRAN array
            if (ivdel >= 0)
            {
                // dDetermine if previously selected variable should be deleted

                if (ivdel == 0)
                    atEnd = true;
                if (probd < optSigin)
                    atEnd = true;
                if (nvarR <= 1)
                    atEnd = true;

                if (!atEnd)
                {
                    nivR--;
                    nvarR = nivR;
                    ivar.DeleteItem(ivdel - 1);
                }
            }
        }

        if (atEnd)
        {

            // step already printed ?
            if (printStep)
                break;

            // just print results from last step
            calcStep = false;
            printStep = true;
        }

        if (calcStep)
        {
            // increment step number and adjust arrays for new selected variables
            // and calculate inverse of correlation matrix
            nsteps++;
            if (printStep && (nsteps > 1))
            {
                AddConditionalBlank();
                char istr[40];
                IntToString(nsteps, istr, 40);
                MakeString(outStr, dvStr, " regression step # ", istr, 256);
                TextSubTitle(outStr);
            }

            // set up temporary means, etc for new selected variables
            DelVar(ivar, dvIndx);
            nivR = ivar.Size();
            nvarR = nivR;

            // invert subset corr matrix
            varInvCorrDR = varCorrR;
            varInvCorrDR.MatrixSetSize(nvarR, nvarR);
            SC_StringArray dummy;
            if (InvertSymmetricMatrix(varInvCorrDR, dummy, false) != 0)
            {
                AddLine("++ ERROR - inverting correlation matrix");
                return;
            }
        }

        if (printStep && outINVERSE)
        {
            SC_StringArray varIDs;
            varIDs.AllocAndSetSize(nvarR);
            for (int i = 0; i < nvarR; i++)
                varIDs.SetString(allProcessedVarDC.columnDesc[ivar[i]], i);

            MakeString(outStr, "Inverse correlation matrix for ", dvStr, 255);
            TextStdTitle(outStr);
            ListSymmetricMatrix(varInvCorrDR, varIDs, sciForm, 13, sfUTri, 8, "VarID");

        }

        if (calcStep)
        {
            nrdf = nobs - nivR - 1;

            // calculate beta star and beta arrays
            rsq = 0.0;
            b0 = ymean;
            for (int i = 0; i < nivR; i++)
            {
                double a = 0.0;
                for (int j = 0; j < nivR; j++)
                    a += varInvCorrDR[i][j] * varCorrDR[j];

                betas[i] = a;
                beta[i] = a * sqrt(tssq / varDiagR[i]);

                // calculate b0 (the intercept)
                b0 -= varMeanR[i] * beta[i];
                // and rsquared
                rsq += a * varCorrDR[i];
            }

            // calculate regression ssq
            regss = rsq * tssq;
            regms = regss / double(nivR);

            // calculate the analysis of regression table for dependent variable
            ressq = tssq - regss;

            // patch -- negative residual sum of squares

            if ((ressq <= 0.0) || (nrdf <= 0))
            {
                atEnd = true;
                ressq = 0.0;
                resms = 0.0;
                regF  = 9999.0;
                if (nrdf == 1)
                    fprob = bigReal;
                else
                    fprob = 0.0;
                xg6 = 0.0;
            }
            else
            {
                resms = ressq / double(nrdf);
                regF = regms / resms;
                fprob = Fsub (regF, nivR, nrdf);

                if (outOTHER)
                {
                    xg6 = 0.0;
                    for (int i = 0; i < nivR; i++)
                        for (int j = 0; j < nivR; j++)
                        {
                            double corr12 = varInvCorrDR[i][j];
                            if (i != j)
                                xg6 += varMeanR[i] * varMeanR[j] * corr12
                                        / sqrt(varDiagR[i] * varDiagR[j]);
                             else
                                xg6 += Sqr(varMeanR[i]) *corr12 / varDiagR[i];
                        }
                    xg6 = sqrt(resms * ((1.0/double(nobs)) + xg6));
                }
            }


            if (optSTEPWISE)
            {
                // this is modified somewhat to account for 0 based C arrays
                // ixrsq index is  +/- (varIndex + 1)
                for (int i = 0; i < nrsq; i++)
                    if (ixrsq[i] > 0)
                    {
                        if (!ivar.SearchForKey(ixrsq[i] - 1))
                            ixrsq[i] *= -1;
                    }

                for (int i = 0; i < nvarR; i++)
                    ixrsq.AddIfNotFound(ivar[i] + 1);

                for (int i = nvarStep; i < nvarR; i++)
                {
                    int rindex;
                    if (ixrsq.SearchForKey(ivar[i] + 1, rindex))
                    {
                        // ixrsq is resizable - make sure rsqv keeps up
                        if (ixrsq.AllocSize() != rsqv.AllocSize())
                            rsqv.ReAlloc(ixrsq.AllocSize());
                        if (rsqv.Size() <= rindex)
                            rsqv.SetSize(rindex + 1);
                        rsqv[rindex] = rsq;
                    }
                }
            }
        }

        if (printStep)
        {
            MakeString(outStr, "ANOVA table for dependent variable -- ", dvStr, 256);
            TextStdTitle(outStr);

            const int cWid[] = {20, 6, 15, 15, 15, 15, -1};
            const bool lJust[] = {true, false, false, false, false, false};
            TableSetup(cWid, lJust);

            const char* colText[] = {"Source", "DofF", "SS", "MS", "F", "SIGNIF"};
            SetTableCol(colText);
            AddNextLine();

            SetTableCol(0, "REGRESSION");
            SetTableInt(1, nivR);
            SetTableReal(2, regss, sciForm);
            SetTableReal(3, regms, sciForm);
            SetTableReal(4, regF, sciForm);
            SetTableReal(5, fprob, sciForm);
            AddNextLine();

            SetTableCol(0, "RESIDUAL");
            SetTableInt(1, nrdf);
            SetTableReal(2, ressq, sciForm);
            SetTableReal(3, resms, sciForm);
            AddNextLine();

            SetTableCol(0, "TOTAL");
            SetTableInt(1, nobs - 1);
            SetTableReal(2, tssq, sciForm);
            AddNextLine();

            AddBlankLine();
            AddStdReal("R-Square", rsq, decForm);
            AddStdReal("Intercept", b0, sciForm);

            if (outOTHER)
                AddStdReal("Standard error", xg6, sciForm);
            AddBlankLine();
        }

        //  calculate beta, beta star, ss, t*s, and r-square deletes and
        // significance for each independent variable in this analysis

        if (printStep || calcStep)
        {
            if (printStep)
            {
                const bool lJust[] = {true, false, false, false, false, false, false, false};
                const int cWid[] = {15, 15, 15, 15, 15, 15, 15, 15, -1};
                TableSetup(cWid, lJust);
                if (optSTEPWISE)
                {
                    const char* colText1[] = {"Variable", "R-Square", "Regression", "Standardized",
                                      "Partial", "T-Test", "R-Square", "Alpha", NULL};
                    SetTableCol(colText1);
                    AddNextLine();

                    const char* colText2[] = {" ", "When Incl.", "Coefficient", "Regr. Coeff.",
                                      "SSQ", "Values", "Deletes", "Hats", NULL};
                    SetTableCol(colText2);
                    AddNextLine();
                }
                else
                {
                    const char* colText1[] = {"Variable", "Regression", "Standardized",
                                      "Partial", "T-Test", "R-Square", "Alpha", NULL};
                    SetTableCol(colText1);
                    AddNextLine();

                    const char* colText2[] = {" ", "Coefficient", "Regr. Coeff.",
                                      "SSQ", "Values", "Deletes", "Hats", NULL};
                    SetTableCol(colText2);
                    AddNextLine();
                }

            }

            double rdmax = 0.0;
            ivdel = 0;

            for (int i = 0; i < nivR; i++)
            {
                double b = betas[i];
                double ccc = varInvCorrDR[i][i];
                double beta2 = b * b / ccc;
                double ss = beta2 * tssq;
                double ti = 9999.0;
                if (resms != 0.0)
                    ti = b * sqrt(tssq/(resms * ccc));
                double rsqd = rsq - beta2;
                double t = fabs(ti);
                double prob;
                if (nrdf == 0)
                    prob = 0.0;
                else if (t <= 1.0E-10)
                    prob = 1.0;
                else
                {
                    double tprob = DcdfT(t, double(nrdf));
                    prob = 2.0 * (1.0 - tprob);
                }
                if (optBACKWARD)
                {
                    bool isForced = optFORCE && forceIndexes.SearchForKey(ivar[i]);
                    if (!isForced)
                    {
                        if (rsqd > rdmax)
                        {
                            rdmax = rsqd;
                            probd = prob;
                            ivdel = i + 1;  // for C/FORTAN  array diff
                        }
                    }
                }
                if (printStep)
                {
                    int iv2;
                    if (!ixrsq.SearchForKey(ivar[i] + 1, iv2))
                        iv2 = -1;
                    int nextCol;
                    if (optSTEPWISE)
                    {
                        int iv;
                        if ((i == 0) || (!ixrsq.SearchForKey(ivar[i-1] + 1, iv)))
                            iv = -1;
                        for (int j = iv + 1; j <= iv2-1; j++)
                            if (ixrsq[j] <= 0)
                            {
                                int vindx = -ixrsq[j] - 1;
                                SetTableCol(0, allProcessedVarDC.columnDesc[ivar[vindx]]);
                                SetTableReal(1, rsqv[j], decForm);
                                for (int k = 2; k < tableColWidth.Size(); k++)
                                    SetTableCol(k, "********");
                                AddNextLine();
                            }

                        if (iv2 >= 0)
                            SetTableReal(1, rsqv[iv2], decForm);
                        else
                            SetTableCol(1, "????");
                        nextCol = 2;
                    }
                    else
                    {
                        nextCol = 1;
                    }

                    SetTableCol(0, allProcessedVarDC.columnDesc[ivar[i]]);
                    SetTableReal(nextCol++, beta[i], sciForm);
                    SetTableReal(nextCol++, b, sciForm);
                    SetTableReal(nextCol++, ss, sciForm);
                    SetTableReal(nextCol++, ti, sciForm);
                    SetTableReal(nextCol++, rsqd, sciForm);
                    SetTableReal(nextCol++, prob, sciForm);
                    AddNextLine();

                }
            }
            if (printStep)
                AddBlankLine();
        }

        if (doResiduals && calcStep)
        {
//         --Calculate time, yhats, residuals

            for (int i = 0; i < nobs; i++)
            {
                time[i] = double(i + 1);
                double yh = b0;
                for (int j = 0; j < nivR; j++)
                    yh += allProcessedVarDC.dataTable[ivar[j]][i] * beta[j];
                yhat[i] = yh;
                resi[i] = yrank[i] - yh;
            }
        }

        if (optRANK)
        {
            if (calcStep)
            {
            // convert rank predictions to raw predictions
            // as outlined in Iman and Conover *technometrics* - 1979
                for (int i = 0; i < nobs; i++)
                {
                    if (yhat[i] <= rank[0])
                        yharaw[i] = ysort[0];
                    else if (yhat[i] >= rank[nobs - 1])
                        yharaw[i] = ysort[nobs - 1];
                    else
                    {
                        int ik = int(yhat[i] + 0.000005);
                        ik--; // for C arrays
                        while (rank[ik] > yhat[i])
                            ik --;
                        int ik1 = ik + 1;
                        while (rank[ik1] <= rank[ik])
                            ik1++;
                        yharaw[i] = ysort[ik] + (ysort[ik1] - ysort[ik]) *
                                    (yhat[i] - rank[ik]) / (rank[ik1] - rank[ik]);
                    }
                    resraw[i] = yraw[i] - yharaw[i];
                }
            }

            if (outSTEPS && outOTHER)
            {
                double syhat = 0.0;
                double ssyhat = 0.0;
                double resss = 0.0;
                for (int i = 0; i < nobs; i++)
                {
                    double term = weights[i] * yharaw[i];
                    syhat += term;
                    ssyhat += term * yharaw[i];
                    resss += weights[i] * resraw[i] * resraw[i];
                }
                double rgrss = ssyhat - (2.0 * syhat - sy) * sy /double(nobs);
                double rpluse = rgrss + resss;
                double ssty = ssy - sy * sy /double(nobs);
                if (rpluse != 0.0)
                {
                    AddConditionalBlank();
                    AddStdReal("Rank fit gives a raw data norm. R-SQR =", rgrss / rpluse, sciForm);
                }

                if ((rpluse + ssty) != 0.0)
                {
                    double cofint = fabs(2.0 * (rpluse / (rpluse + ssty)) - 1.0);
                    AddStdReal("Coefficient of interpolation", cofint, sciForm);
                }
            }
        }

        if (optPRESS && calcStep)
        {
            double pressX = CalcPress(nivR, ivar, resi);
            AddConditionalBlank();
            AddStdReal("PRESS = ", pressX, sciForm);
            AddBlankLine();

            // add to output table
            SC_DoubleMatrix& prTable = pressTableDC.dataTable;
            if (nsteps > prTable[0].Size())
            {
                // add new step

                // realloc required ?
                if (prTable[0].IsFull())
                {
                    int nextAlloc = nsteps * 2 + 1;
                    for (int i = 0; i < prTable.AllocSize(); i++)
                        prTable[i].ReAlloc(nextAlloc);
                }

                // update
                prTable[0] += double(nsteps);

                // set pre DV to -1
                for (int i = 0; i < dvNum; i++)
                    prTable[i + 1] += -1.0;
            }

            // add pressX
            prTable[dvNum + 1] += pressX;
        }

        if (!(optSTEPWISE  || optBACKWARD))
            atEnd = true;
    }

    // update press table
    if (optPRESS)
    {
        SC_DoubleMatrix& prTable = pressTableDC.dataTable;
        while (prTable[dvNum + 1].Size() < prTable[0].Size())
            prTable[dvNum + 1] += -1.0;
    }

    // list residuals
    if (!outRESIDUALS)
        return;

    const SC_DecFormat dec0(0);

    if (optRANK)
    {
        const int nrankCol = 7;

        const bool lJust[nrankCol] = {false, false, false, false, false, false, false};
        const int cWid[nrankCol + 1] = {8, 8, 15, 15, 15, 15, 15, -1};
        TableSetup(cWid, lJust);

        const char* colText1[nrankCol + 1] = {"Time", "Rank", "Predicted", "Rank",
                                "Raw", "Raw", "Raw",  NULL};
        SetTableCol(colText1);
        AddNextLine();

        const char* colText2[nrankCol + 1] = {" ", "of Y", "Rank of Y", "Residual",
                                 "Y", "YHAT", "Residual",  NULL};
        SetTableCol(colText2);
        AddNextLine();

        //set up table
        for (int i = 0; i < nrankCol; i++)
        {
            char colStr[80];
            MakeString(colStr, colText1[i], " ", colText2[i], 80);
            residTableDC.columnDesc.SetString(colStr, i);
        }

        SC_DoubleMatrix& rData = residTableDC.dataTable;
        for (int i = 0; i < nobs; i++)
        {
            SetTableReal(0, time[i], dec0);
            SetTableReal(1, yrank[i], dec0);
            SetTableReal(2, yhat[i], sciForm);
            SetTableReal(3, resi[i], sciForm);
            SetTableReal(4, yraw[i], sciForm);
            SetTableReal(5, yharaw[i], sciForm);
            SetTableReal(6, resraw[i], sciForm);
            AddNextLine();

            rData[0][i] = time[i];
            rData[1][i] = yrank[i];
            rData[2][i] = yhat[i];
            rData[3][i] = resi[i];
            rData[4][i] = yraw[i];
            rData[5][i] = yharaw[i];
            rData[6][i] = resraw[i];
        }
        AddBlankLine();
        rData.MatrixSetSize(nrankCol, nobs);

        if (outOTHER)
        {
            double resss = 0.0;
            for (int i = 0; i < nobs; i++)
                resss += weights[i] * resraw[i] * resraw[i];
            AddStdReal("Residual sum of squares on raw data", resss, sciForm);
            AddBlankLine();
        }
    }
    else
    {
        double st = 0.0;
        SC_DoubleArray pstp(nobs);
        bool conLimitsOK = (nrdf > 0);
        if (conLimitsOK)
        {
            conLimitsOK = CalcConfLimits(nivR, ivar, pstp);
            if (conLimitsOK)
                // ??? constant is 0.975 - conf limits are 95% ???
                st = sqrt(resms) * Tinv(0.975, 1, nrdf);
        }

        const int nrawCol = 8;

        const bool lJust[nrawCol] = {false, false, false, false, false, false, false, false};
        const int cWid[nrawCol + 1] = {8, 15, 15, 15, 15, 15, 15, 15, -1};
        TableSetup(cWid, lJust);

        const char* colText1[nrawCol + 1] = {"Time", "Observed Y", "Predicted Y", "Residual",
                                "95% LOW", "95% HIGH", "95% LOW",  "95% HIGH", NULL};
        SetTableCol(colText1);
        AddNextLine();

        const char* colText2[nrawCol + 1] = {" ", " ", " ", " ",
                                 "MEAN", "MEAN", "INDIV",  "INDIV", NULL};
        SetTableCol(colText2);
        AddNextLine();
        //set up table
        for (int i = 0; i < nrawCol; i++)
        {
            char colStr[80];
            MakeString(colStr, colText1[i], " ", colText2[i], 80);
            residTableDC.columnDesc.SetString(colStr, i);
        }

        SC_DoubleMatrix& rData = residTableDC.dataTable;

        const SC_DecFormat dec0(0);
        for (int i = 0; i < nobs; i++)
        {
            double pstl95 = 0.0;
            double pstp95 = 0.0;
            if (conLimitsOK)
            {
                pstl95 = st * sqrt(pstp[i]);
                pstp95 = st * sqrt(1.0 + pstp[i]);
            }
            double yh = yhat[i];

            SetTableReal(0, time[i], dec0);
            SetTableReal(1, yrank[i], sciForm);
            SetTableReal(2, yh, sciForm);
            SetTableReal(3, resi[i], sciForm);
            SetTableReal(4, yh - pstl95, sciForm);
            SetTableReal(5, yh + pstl95, sciForm);
            SetTableReal(6, yh - pstp95, sciForm);
            SetTableReal(7, yh + pstp95, sciForm);
            AddNextLine();

            rData[0][i] = time[i];
            rData[1][i] = yrank[i];
            rData[2][i] = yh;
            rData[3][i] = resi[i];
            rData[4][i] = yh - pstl95;
            rData[5][i] = yh + pstl95;
            rData[6][i] = yh - pstp95;
            rData[7][i] = yh + pstp95;

        }
        AddBlankLine();
        rData.MatrixSetSize(nrawCol, nobs);

    }

    char titlStr[80];
    MakeString(titlStr, dvStr , " Residuals", 80);
    residTableDC.SetID(titlStr);

    residTableDC.rowDesc.SetIndexed("Obs#", 1);


}

double DFO_STEPWISE::CalcPress(int   nivR,
                               const SC_IntArray&    ivar,
                               const SC_DoubleArray& resi)
{
    SC_DoubleArray ydum(nivR);
    double sqrobs = sqrt(double(nobs - 1));
    double press = 0.0;
    for (int i = 0; i < nobs; i++)
    {
        for (int j = 0; j < nivR; j++)
            ydum[j] = (allProcessedVarDC.dataTable[ivar[j]][i] - varMeanR[j]) /
                        (varStdDevR[j] * sqrobs);
        double qi = 0.0;
        for (int j = 0; j < nivR; j++)
        {
            double sumq = 0.0;
            for (int k = 0; k < nivR; k++)
                sumq += ydum[k] * varInvCorrDR[j][k];
            qi += sumq * ydum[j];
        }
        qi += 1.0 / double(nobs);
        press += weights[i] * Sqr(resi[i] / (1.0 - qi));
    }

    return press;
}

bool DFO_STEPWISE::CalcConfLimits(int             nivR,
                                 const SC_IntArray& ivar,
                                  SC_DoubleArray& pstp)
{
    int nivR1 = nivR + 1;

    SC_DoubleMatrix tmpInv;
    tmpInv.MatrixAllocAndSetSize(nivR1, nivR1);

    double rnobs = double(nobs);
    tmpInv[0][0] = rnobs;
    for (int i = 0; i < nivR; i++)
    {
        tmpInv[i + 1][0] = varMeanR[i] * rnobs;
        tmpInv[0][i + 1] = tmpInv[i + 1][0];
        for (int j = 0; j < i ; j++)
        {
            double term = varCorrR[i][j] * sqrt(varDiagR[i] * varDiag[j])
                                    + varMeanR[i] * varMeanR[j] * rnobs;
            tmpInv[i + 1][j + 1] = term;
            tmpInv[j + 1][i + 1] = term;
        }
        tmpInv[i + 1][ i + 1] = varCorrR[i][i] * varDiagR[i]
                                + varMeanR[i] * varMeanR[i] * rnobs;
    }

    SC_StringArray dummy;
    if (InvertSymmetricMatrix(tmpInv, dummy, false) != 0)
    {
        AddLine("++ ERROR - inverting correlation matrix");
    }

    for (int i = 0; i < nobs; i++)
    {
        double p = 0.0;
        for (int j = 0; j < nivR1; j++)
        {
            double psum = 0.0;
            for (int k = 0; k < nivR1; k++)
            {
                double x2 = 1.0;
                if (k > 0)
                    x2 = allProcessedVarDC.dataTable[ivar[k - 1]][i];
                psum += tmpInv[j][k] * x2;
            }

            double x1 = 1.0;
            if (j > 0)
                x1 = allProcessedVarDC.dataTable[ivar[j - 1]][i];

            p += psum * x1;
        }
        pstp[i] = p;
    }
    return true;

}



void DFO_STEPWISE::DelVar(SC_IntArray& ivar, const int idv)
{
    int nsel = ivar.Size();

    const SC_DoubleArray& corrDV = varCorr[idv];

    for (int i = 0; i < nsel; i++)
    {
        int seli = ivar[i];
        varMeanR[i] = varMean[seli];
        varStdDevR[i] = varStdDev[seli];
        varDiagR[i] = varDiag[seli];
        varCorrDR[i] = corrDV[seli];
        for (int j = 0; j < nsel; j++)
            varCorrR[i][j] = varCorr[seli][ivar[j]];
    }

    varCorrR.MatrixSetSize(nsel, nsel);
    varMeanR.SetSize(nsel);
    varStdDevR.SetSize(nsel);
    varDiagR.SetSize(nsel);
    varCorrDR.SetSize(nsel);
}



void DFO_STEPWISE::StepInit(const int idv)
{
    // allocate
    int iadim = niv + 1 + niv;
    stepA.MatrixAllocAndSetSize(iadim, iadim);
    stepA.MatrixFill(0.0);
    stepF.Alloc(niv);

    // copy corr to A - ivs are always first
    for (int i = 0; i < niv; i++)
        for (int j = 0; j < niv; j++)
            stepA[i][j] = varCorr[i][j];

    // add depvar
    const SC_DoubleArray& corrDV = varCorr[idv];
    for (int i = 0; i < niv; i++)
    {
        stepA[i][niv] = corrDV[i];
        stepA[niv][i] = corrDV[i];
    }
    stepA[niv][niv] = 1.0;

    // fill out - ??
    for (int i = 0; i < niv; i++)
    {
        int niv1i = niv + i + 1;
        stepA[i][niv1i] = 1.0;
        stepA[niv1i][i] = -1.0;
    }

    // set all to possible
    nvarOK = 0;
    ivarOK.AllocAndFill(niv, -1);

    nPhi = nobs -1;
    maxOK = IMin(niv, nPhi);

    doForce = optFORCE;
}


void DFO_STEPWISE::StepStep(int&                nvarIn,
                            int&            nvarOut,
                            SC_IntArray&    ivar,
                            bool&           atEnd)
{
    SC_DoubleArray f1(niv);

    atEnd = false;
    if (nvarOK < maxOK)
    {
        // add forced vars
        bool forceAdded = false;
        if (doForce)
        {
            StepForce();
            doForce = false;
            forceAdded = (nvarOK > 0);
        }

        if (!forceAdded)
            if (StepPick())
            {
                StepDel();
            }
            else
                atEnd = true;
    }
    else
        atEnd = true;

    nvarOut = 0;
    for (int i = 0; i < nvarIn; i++)
    {
        int iv = ivar[i];
        if (ivarOK[iv] >= 0)
        {
            ivar[nvarOut] = iv;
            nvarOut++;
        }
    }
    ivar.SetSize(nvarOut);

    nvarIn = nvarOut;
    // add new variables ??
    for (int i = 0; i < niv; i++)
        if (ivarOK[i] >= 0)
            ivar.AddIfNotFound(i);
    nvarOut = ivar.Size();

}


void DFO_STEPWISE::StepForce()
{
    for (int i = 0; i < forceIndexes.Size(); i++)
    {
        if (nvarOK >= maxOK)
            return;
        int ifrc = forceIndexes[i];
        if (ivarOK[ifrc] < 0)
        {
            nPhi--;
            nvarOK++;
            ivarOK[ifrc] = 1;
            StepAdd(ifrc);
        }
    }
}

void DFO_STEPWISE::StepAdd(int iadd)
{
    int ip1 = niv + niv + 1;
    double d = stepA[iadd][iadd];
    for (int i = 0; i < ip1; i++)
        for (int j = 0; j < ip1; j++)
            if ((i != iadd) && (j != iadd))
                stepA[i][j] -= stepA[i][iadd] * stepA[iadd][j] / d;


    for (int i = 0; i < ip1; i++)
        if (i != iadd)
        {
            stepA[i][iadd] = 0.0;
            stepA[iadd][i] /= d;
        }
}

bool DFO_STEPWISE::StepPick()
{
    int imax = -1;

    double vmax;
    for (int i = 0; i < niv; i++)
    {
        double diag = stepA[i][i];
        if ((ivarOK[i] < 0) && (diag  != 0.0))
        {
            double v = stepA[i][niv] * stepA[niv][i] / diag;
            if ((imax < 0) || (vmax < v))
            {
                vmax = v;
                imax = i;
            }
        }
    }

    nPhi--;

    double f = bigReal;
    if (vmax != stepA[niv][niv])
        f = double(nPhi) * vmax / (stepA[niv][niv] - vmax);
    double fp = Fsub(f, 1, nPhi);

    // no var ??
    if (fp > optSigin)
        return false;

    nvarOK++;
    ivarOK[imax] = 0;
    StepAdd(imax);
    return true;

}


void DFO_STEPWISE::StepDel()
{
    bool anydel = true;
    int niv1 = niv + 1;
    while (anydel)
    {
        for (int i = 0; i < niv; i++)
            if (ivarOK[i] == 0)
            {
                double d = stepA[niv][niv] * stepA[niv1 + i][niv1 + i];
                if (d != 0.0)
                    stepF[i] = Sqr(stepA[i][niv])* double(nPhi) / d;
                else
                    stepF[i] = bigReal;
            }
            else
                if (ivarOK[i] > 0)
                    stepF[i] = 1001.0;

        anydel = false;

        for (int idel = 0; idel < niv; idel++)
            if (ivarOK[idel] >= 0)
            {
                double fp = Fsub(stepF[idel], 1, nPhi);
                if (fp > optSigout)
                {
                    anydel = true;
                    ivarOK[idel] = -1;
                    nvarOK--;
                    nPhi++;
                    int ip1 = niv + niv + 1;
                    double d = stepA[niv1 + idel][niv1 + idel];
                    for (int i = 0; i < ip1; i++)
                        for (int j = 0; j < ip1; j++)
                            if ((i != idel) && (j != (niv1 + idel)))
                                stepA[i][j] -= stepA[i][niv1 + idel] * stepA[idel][j] / d;

                    for (int j = 0; j < ip1; j++)
                        stepA[idel][j] /= d;

                    for (int j = 0; j < ip1; j++)
                        if (j != idel)
                        {
                            stepA[j][niv1 + idel] = 0.0;
                            stepA[niv1 + idel][j] = 0.0;
                        }
                }
            }
    }

}

double  DFO_STEPWISE::Fsub(double f, int j, int k)
{
    double fsub;
    if (f <= 1000.0)
    {
        double xa = 0.5 * double(j);
        double xb = 0.5 * double(k);
        double temp = xb + xa * f;
        double xx = xa * f / temp;
        if ((f > 0.0) && (xx > 0.0))
        {
            double xc = xb / temp;
            double ab = xa + xb;
            double con = 0.0;
            double sgn = 1.0;
            if (f < 1.0)
            {
                temp = xa;
                xa = xb;
                xb = temp;
                temp = xc;
                xc = xx;
                xx = temp;
                con = 1.0;
                sgn = -1.0;
            }
            double top = ab;
            double bot = xb + 1.0;
            double xsum = 1.0;
            double term = 1.0;
            do
            {
                temp = xsum;
                term = term * xc * top / bot;
                xsum = xsum + term;
                top = top + 1.0;
                bot = bot + 1.0;
            } while (xsum > temp);

            fsub = con + sgn * exp(xa * log(xx) + xb * log(xc)
                    + Algama(ab) - Algama(xa) - Algama(xb)) * xsum / xb;
        }
        else
            fsub = 1.0;
    }
    else
        if (k == 1)
            fsub = bigReal;
         else
            fsub = 0.0;

    return fsub;
}


double DFO_STEPWISE::Algama(double a)
{
    double w = a;
    double temp = 0.0;
    if (w <= 13.0)
    {
        int n = int(14.0 - w);
        temp = 1.0;
        for (int i = 0; i < n; i++)
        {
            temp *= w;
            w += 1.0;
        }
         temp = log(temp);
    }
    double w2 = w * w;

    double algama = (8.33333333e-02 - (2.77777777e-03 - 7.93650793e-04/w2)/w2)/w +
                    0.918938533 - w + (w-0.5)*log(w) - temp;
    return algama;
}

double DFO_STEPWISE::DcdfT(double x, double v)
{
/*   --DCDFT calculates CDF of t distribution with V degrees of freedom,
C   --at X.
C   --From Hubert J Chen J Statis. Comput. Simul. 1978 vol 7 pp 167-180. */

    int n = int(v);
    double theta = atan(x / sqrt (v));
    double ss, cs, css, sum;

    if (n != 1)
    {
         ss = sin(theta);
         cs = cos(theta);
         css = cs * cs;
    }
    if ((n % 2) == 0)
    {
        double term = 1.0;
        sum = term;
        for (int i = 2; i <= (n - 2); i += 2)
        {
            term *= css * (double(i - 1) / double(i));
            sum += term;
        }
        sum *= ss;
    }
    else
    {
        double sums = 0.0;
        if (n != 1)
        {
            double term = cs;
            sums = term;
            for (int i = 3; i <= (n - 2); i += 2)
            {
                term *= css * (double(i - 1) / double(i));
                sums += term;
            }
            sums *= ss;
        }
        sum = (2.0 / pi) * (theta + sums);
    }
    return (sum + 1.0) / 2.0;
}


double DFO_STEPWISE::Tinv(double r, int k, int n)
{
/*   --TINV calculates the inverse CDF of the t distribution the N degrees of
C   --freedom at the Kth root of R (R is between 0 and 1).
C   --TINV can generate random variates from t distribution with K=1 and
C   --evaluate percentage points with K>=1.
C   --From Hubert J Chen J Statis. Comput. Simul. 1978 vol 7 pp 167-180. */


    const int nw = 19;
    static const double w[nw] = {   3183500.,   2250.,  230.,   75.,    41.,
                                28.,        21.,    17.,    15.,    13.,
                                12.,        11.,    10.5,   10.,    9.5,
                                9.,         8.5,    8.3,    8.1};

    double p = pow(r, 1.0/double(k));
    if ((p < 1.0E-06) || (p > 0.999999))
    {
        AddLine("++ ERROR - Tinv extreme probability");
        return 0.0;
    }

    double df = double(n);
    double x2 = 8.0;
    if (n <= nw)
        x2 = w[n - 1];
    double x1 = -x2;
    while (true)
    {
        double xnew = (x1 + x2) / 2.0;
        double r1 = DcdfT(xnew, df);

        if (fabs(r1 - p) <= 5.0E-06)
            return xnew;

        if (r1 <= p)
            x1 = xnew;
        else
            x2 = xnew;
    }
}

