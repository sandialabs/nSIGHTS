///////////////////////////////////////////////////////////////////////////////////
//
// DFO_PCCSRC.cpp
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

#include <saDFO/DFO_PCCSRC.h>

DFO_PCCSRC :: DFO_PCCSRC() :
    DFO_SAObjBase("PCC/SRC")
{
    doPCC = false;
    doRANK = false;
    doSRC = true;
    verboseOutput = true;
    listCutoff = 0.0;
    plotCutoff = 0.0;
    topIND = -1;
    coeffFormat = SC_DecFormat(5);

    CopyString(title, "PCC/SRC - mView", 80);


    InitCommon();
}

DFO_PCCSRC::DFO_PCCSRC(const DFO_PCCSRC& a) : DFO_SAObjBase(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_PCCSRC::InitCommon()
{
    AddOutPort(depVarLabelDO);
    depVarLabelDO.Reset("Dependent variable");

    rawDepVarDO.tableData = &rawDepVarDC;
    AddOutPort(rawDepVarDO);

    pccDO.tableData = &pccDC;
    AddOutPort(pccDO);

    srcDO.tableData = &srcDC;
    AddOutPort(srcDO);

    rsqDO.tableData = &rsqDC;
    AddOutPort(rsqDO);

    DoStatusChk();
}

DFO_PCCSRC& DFO_PCCSRC::operator= (const DFO_PCCSRC& a)
{
    if (&a != this)
    {
        DFO_SAObjBase::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_PCCSRC::LocalCopy(const DFO_PCCSRC& a)
{
    doPCC   = a.doPCC;
    doRANK  = a.doRANK;
    doSRC   = a.doSRC;
    listCutoff = a.listCutoff;
    plotCutoff = a.plotCutoff;
    topIND     = a.topIND;
    coeffFormat = a.coeffFormat;
    verboseOutput = a.verboseOutput;
}

void  DFO_PCCSRC:: DoStatusChk()
{
    DFO_SAObjBase::DoStatusChk();

    if (!(doPCC || doSRC))
        SetObjErrMsg("one of PCC or SRC must be selected");
}

void DFO_PCCSRC:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    InputListing();

    // generate raw table
    MakeRawTable();

    // get copies of data
    DC_SATable indVar = GetIndepVar();
    DC_SATable depVar = GetDepVar();

    // set constants
    niv = indVar.GetNVariable();
    ndv = depVar.GetNVariable();
    nstep = depVar.GetNStep();

    // allocate output tables
    SetupOutputTable(pccDC, "PCC");
    SetupOutputTable(srcDC, "SRC");
    SetupRSquaredTable();
    if (StatusNotOK())
        return;

    // rank if necessary
    if (doRANK)
    {
        SC_DoubleMatrix& ivarTbl = indVar.saData[0];
        for (int i = 0; i < niv; i++)
            RankArray(ivarTbl[i]);

        for (int k = 0; k < depVar.GetNStep(); k++)
        {
            SC_DoubleMatrix& varTbl = depVar.saData[k];
            for (int i = 0; i < ndv; i++)
                RankArray(varTbl[i]);
        }
    }

    // build indepVar correlation table
    SC_DoubleMatrix ivCorr;
    BuildIVCorrMatrix(ivCorr, indVar.saData[0]);

    // for each DV
    SC_DoubleMatrix dvCorr;
    dvCorr.MatrixAllocAndSetSize(niv + 1, niv + 1);

    const SC_DoubleMatrix& ivarTbl = indVar.saData[0];

    // working tables - always calcd
    SC_DoubleMatrix currPCC, currSRC;
    currPCC.MatrixAllocAndSetSize(niv, nstep);
    currSRC.MatrixAllocAndSetSize(niv, nstep);
    SC_DoubleArray currRSQR(nstep);

    SC_DoubleCube pccPlot(ndv), srcPlot(ndv);
    SC_DoubleMatrix rsqPlot(ndv, nstep);
    SC_DoubleArray plotMax(ndv);
    SC_IntArray plotDV(ndv);

    SC_StringArray rcID;
    rcID.Alloc(niv + 1);
    rcID += indVar.variableNames;
    rcID += "dummy";

    int nlist = 0;
    int nplot = 0;
    int inverr = 0;
    int ninverr = 0;
    bool writeMatrices = verboseOutput && (nstep < 3) && (ndv < 3);
    for (int ii = 0; ii < ndv; ii++)
    {
        for (int k = 0; k < nstep; k++)
        {
            // fill in iv values
            for (int i = 0; i < niv; i++)
                for (int j = 0; j < niv; j++)
                    dvCorr[i][j] = ivCorr[i][j];

            // calc corrs between iv and curr dv
            const SC_DoubleArray& dvData = depVar.saData[k][ii];
            for (int i = 0; i < niv; i++)
            {
                double corrIJ = CorrelateXY(ivarTbl[i], dvData, doRANK);
                dvCorr[i][niv] = corrIJ;
                dvCorr[niv][i] = corrIJ;
            }
            // main diagonal
            dvCorr[niv][niv] = 1.0;

            rcID.SetString(depVar.GetVariableName(ii), niv);
            char stepStr[40];
            char titleStr[80];
            if (writeMatrices)
            {
                IntToString(k + 1, stepStr, 40);
                MakeString(titleStr, "Correlation Matrix: Step ", stepStr, 80);
                TextSubTitle(titleStr);
                ListSymmetricMatrix(dvCorr, rcID, coeffFormat, 10, sfAll, 10, "Corr");
            }

            inverr = InvertSymmetricMatrix(dvCorr, rcID, verboseOutput);
            if (inverr != 0)
            {
                // invert error
                if (verboseOutput)
                {
                    AddStdText("Dependent variable", depVar.GetVariableName(ii));
                    AddStdInt("Step index", k);
                    AddStdReal("Step value", depVar.stepValues[k]);
                    AddLine("**** End error diagnostics ****");
                    AddBlankLine();
                }
                ninverr++;
            }

            if (writeMatrices)
            {
                MakeString(titleStr, "Inverse  Correlation Matrix: Step ", stepStr, 80);
                TextSubTitle(titleStr);
                ListSymmetricMatrix(dvCorr, rcID, coeffFormat, 10, sfAll, 10, "Corr");
            }

            // calculate pcc and src
            for (int i = 0; i < niv; i++)
            {
                double pcc = -dvCorr[niv][i] / sqrt(dvCorr[i][i] * dvCorr[niv][niv]);
                double src = -dvCorr[niv][i] / dvCorr[niv][niv];
                currPCC[i][k] = pcc;
                currSRC[i][k] = src;
            }
            currRSQR[k] = -(1.0 / dvCorr[niv][niv] - 1.0);
        }

        if ((!verboseOutput) && (ninverr > 0))
        {
            AddBlankLine();
            AddLine("**** Inversion Errors/Warnings - use verbose for details ****");
            AddStdText("Dependent variable", depVar.GetVariableName(ii));
            AddStdInt("Number of errors/warnings", ninverr);
            AddBlankLine();
        }

        double currMax;
        if (CheckCutoff(currPCC, currSRC, listCutoff, currMax) || (ndv == 1))
        {
            // listing for current depVar
            char outStr[80];
            MakeString(outStr, "Dependent Variable: ", depVar.GetVariableName(ii), 80);
            TextBigTitle(outStr);

            if (doPCC)
            {
                const char* tblTitl = "Partial Correlation Coefficient";
                if (doRANK)
                    tblTitl = "Partial Ranked Correlation Coefficient";
                ListOutputTable(tblTitl, ii, currPCC, currRSQR);
            }

            if (doSRC)
            {
                const char* tblTitl = "Standardized Regression Coefficient";
                if (doRANK)
                    tblTitl = "Standardized Ranked Regression Coefficient";
                ListOutputTable(tblTitl, ii, currSRC, currRSQR);
            }

            nlist++;
        }

        if (CheckCutoff(currPCC, currSRC, plotCutoff, currMax))
        {
            pccPlot[nplot] = currPCC;
            srcPlot[nplot] = currSRC;
            rsqPlot[nplot] = currRSQR;
            plotMax[nplot] = currMax;
            plotDV[nplot] = ii;
            nplot++;
        }
    }

    if (listCutoff > 0.0)
    {
        if (nlist == 0)
            TextStdTitle("No listings generated -- all < PRINT CUTOFF");
        else if (nlist == ndv)
            TextStdTitle("All listings generated -- all >= PRINT CUTOFF");
        else
        {
            char nlStr[40], ndvStr[40];
            IntToString(nlist, nlStr, 40);
            IntToString(ndv, ndvStr, 40);
            char outStr[120];
            MakeString(outStr, nlStr, " variables of ", ndvStr, " >= PRINT CUTOFF", 120);
            TextStdTitle(outStr);
        }
    }

    TextStdTitle("Plotted Variables");
    if (nplot == 0)
    {
        AddLine("None", 5);
        depVarLabelDO.SetValueLabel("none");
    }
    else if (nplot == 1)
    {
        int currDV = plotDV[0];
        PlotOutputTable(pccPlot[0], pccDC, currDV);
        PlotOutputTable(srcPlot[0], srcDC, currDV);
        rsqDC.columnDesc += depVar.GetVariableName(currDV);
        for (int i = 0; i < nstep; i++)
            rsqDC.dataTable[1][i] = rsqPlot[currDV][i];
        rsqDC.dataTable.MatrixSetSize(2, nstep);

        AddLine(depVar.GetVariableName(currDV), 5);
        depVarLabelDO.SetValueLabel(depVar.GetVariableName(currDV));
    }
    else
    {
        SC_IntArray plotOrder;
        plotOrder.AllocAndIndex(nplot);
        if (topIND > -1)
        {
            plotMax.SetSize(nplot);
            plotMax.Sort(false, plotOrder);
        }

        if ((topIND > 0) && (topIND < nplot))
            nplot = topIND;

        int nextCol = 1;
        for (int i = 0; i < nplot; i++)
        {
            int indx = plotOrder[i];
            int currDV = plotDV[indx];
            PlotOutputTable(pccPlot[indx], pccDC, currDV);
            PlotOutputTable(srcPlot[indx], srcDC, currDV);
            rsqDC.columnDesc += depVar.GetVariableName(currDV);
            for (int i = 0; i < nstep; i++)
                rsqDC.dataTable[nextCol][i] = rsqPlot[currDV][i];
            nextCol++;
            AddLine(depVar.GetVariableName(currDV), 5);
        }

        rsqDC.dataTable.MatrixSetSize(nextCol, nstep);
        depVarLabelDO.SetValueLabel("multiple");
    }

    CloseListingData();
}

void DFO_PCCSRC::SetupOutputTable(DC_TableData& tbl,
                                    const char* tblID)
{
    if (!tbl.Alloc(ndv * niv + 1, nstep))
    {
        SetObjErrMsg("table allocation problem ??");
        return;
    }

    tbl.SetID(tblID);
    AddTableSteps(tbl);

}


void DFO_PCCSRC::SetupRSquaredTable()
{
    if (!rsqDC.Alloc(ndv + 1, nstep))
    {
        SetObjErrMsg("table allocation problem ??");
        return;
    }
    rsqDC.SetID("RSQUARED");
    AddTableSteps(rsqDC);
}

void DFO_PCCSRC::AddTableSteps(DC_TableData& tbl)
{
    tbl.columnDesc += "Step";
    tbl.dataTable.MatrixSetSize(1, nstep);
    for (int i = 0; i < nstep; i++)
        tbl.dataTable[0][i] = GetDepVar().stepValues[i];
}


bool DFO_PCCSRC::CheckCutoff(const SC_DoubleMatrix& pcc,
                             const SC_DoubleMatrix& src,
                             const double&         cutVal,
                                   double&         maxVal)

{
    maxVal = 0.0;
    double tVal;
    for (int i = 0; i < niv; i++)
        for (int j = 0; j < nstep; j++)
        {
            if (doPCC)
                tVal = pcc[i][j];
            else
                tVal = src[i][j];
            maxVal = DMax(fabs(tVal), maxVal);
        }

    return maxVal >= cutVal;
}


void DFO_PCCSRC::InputListing()
{
    InitListingData();
    TextBigTitle(title);

    if (verboseOutput)
        FullVariableList();

    const DC_SATable& depVar = GetDepVar();
    const DC_SATable& indepVar = GetIndepVar();

    TextStdTitle("PCCSRC Standard Listing");
    AddStdInt("Number of input independent variables", indepVar.GetNVariable());
    AddStdInt("Number of input dependent variables", depVar.GetNVariable());
    AddStdInt("Number of observations", depVar.GetNVector());
    AddStdInt("Number of step intervals", depVar.GetNStep());
    AddBlankLine();

    if (doPCC)
    {
        AddBoolText("PCC calculated with", doRANK, "Ranked Data", "Original Data");
    }
    if (doSRC)
    {
        AddBoolText("SRC calculated with", doRANK, "Ranked Data", "Original Data");
    }

    if (verboseOutput)
        VariablesNameList();
    else
        AddBlankLine();
}

void DFO_PCCSRC::MakeRawTable()
{
    const DC_SATable& depVar = GetDepVar();
    int nCol = depVar.GetNVariable() * depVar.GetNVector() + 1;
    int nRow = depVar.GetNStep();

    if (!rawDepVarDC.Alloc(nCol, nRow))
        return;

    rawDepVarDC.SetID("DepVar");

    SC_StringArray& colDesc = rawDepVarDC.columnDesc;
    colDesc += "StepVal";

    for (int i = 0; i < depVar.GetNVariable(); i++)
        for (int j = 0; j < depVar.GetNVector(); j++)
        {
            char colStr[80];
            ConcatInt(j + 1, depVar.GetVariableName(i), colStr, 3, 80);
            colDesc += colStr;
        }

    rawDepVarDC.rowDesc.SetSize(nRow);
    rawDepVarDC.rowDesc.SetIndexed("Step#", 1);

    SC_DoubleMatrix& tblData = rawDepVarDC.dataTable;
    tblData.MatrixSetSize(nCol, nRow);

    for (int k = 0; k < nRow; k++)
    {
        tblData[0][k] = depVar.stepValues[k];
        int nextCol = 1;
        for (int i = 0; i < depVar.GetNVariable(); i++)
            for (int j = 0; j < depVar.GetNVector(); j++)
                tblData[nextCol++][k] = depVar.saData[k][i][j];
    }

}

void DFO_PCCSRC::BuildIVCorrMatrix(SC_DoubleMatrix& ivCorr,
                                    const SC_DoubleMatrix& ivTbl)
{
    ivCorr.MatrixAllocAndSetSize(niv, niv);

    // main diagonal
    for (int i = 0; i < niv; i++)
        ivCorr[i][i] = 1.0;

    // off diag terms
    for (int i = 0; i < niv - 1; i++)
        for (int j = i + 1; j < niv; j++)
        {
            double corrIJ = CorrelateXY(ivTbl[i], ivTbl[j], doRANK);
            ivCorr[i][j] = corrIJ;
            ivCorr[j][i] = corrIJ;
        }
}


void DFO_PCCSRC::ListOutputTable(const  char* title,
                                        int   dvarIndex,
                                 const SC_DoubleMatrix& dvData,
                                 const SC_DoubleArray& rsqr)
{
    TextStdTitle(title);

    const SC_StringArray& varIDs = GetIndepVar().variableNames;

    int baseColWidth = IMax(10, varIDs.GetMaxStringLen() + 1);
    if (coeffFormat.format == ncf_Scientific)
        baseColWidth = IMax(baseColWidth, coeffFormat.digitsPrecision + 8); // +YE+XX
    else if (coeffFormat.format == ncf_Decimal)
        baseColWidth = IMax(baseColWidth, coeffFormat.digitsPrecision + 5);

    SC_IntArray colWidths(10);
    colWidths.FillToAlloc(baseColWidth);

    colWidths[0] = 5;
    colWidths[1] = 10;
    TableSetup(colWidths, 1);

    int ntotCol = niv + 1;
    int ntables = ntotCol / 8;
    if ((ntotCol % 8) > 0)
        ntables++;

    SC_DoubleArray stepMax(niv, 0.0);

    int stCol = 0;
    TextSubTitle("Coefficients");
    for (int i = 0; i < ntables; i++)
    {
        int endCol = stCol + 8;
        if (endCol > ntotCol)
            endCol = ntotCol;

        // titles
        SetTableCol(0, "Step");
        SetTableCol(1, "StepVal");
        int nextCol = 2;
        for (int k = stCol; k < endCol; k++)
        {
            if (k < niv)
                SetTableCol(nextCol, varIDs[k]);
            else
                SetTableCol(nextCol, "R-SQUARE");
            nextCol++;
        }
        AddNextLine();

        // data

        for (int j = 0; j < nstep; j++)
        {
            SetTableInt(0, GetDepVar().stepIndexes[j] + 1);
            SetTableReal(1, GetDepVar().stepValues[j]);

            nextCol = 2;
            for (int k = stCol; k < endCol; k++)
            {
                if (k < niv)
                {
                    double dval = dvData[k][j];
                    stepMax[k] = DMax(fabs(dval), stepMax[k]);
                    SetTableReal(nextCol++, dval, coeffFormat);
                }
                else
                    SetTableReal(nextCol, rsqr[j], coeffFormat);
            }
            AddNextLine();
        }

        if (nstep > 1)
        {
            SetTableCol(0, "MAXIMUM");
            nextCol = 2;
            for (int k = stCol; k < endCol; k++)
            {
                if (k < niv)
                    SetTableReal(nextCol++, stepMax[k], coeffFormat);
            }
            AddNextLine();
        }

        AddBlankLine();
        stCol += 8;
    }

    // now ranks - calculate

    ntotCol = niv;
    SC_DoubleArray rankedData(niv);
    SC_DoubleMatrix rankedMat;
    rankedMat.MatrixAllocAndSetSize(niv, nstep);
    for (int j = 0; j < nstep; j++)
    {
        for (int k = 0; k < ntotCol; k++)
            rankedData[k] = fabs(dvData[k][j]);
        RankArray(rankedData);

        for (int k = 0; k < ntotCol; k++)
            rankedMat[k][j] = double(ntotCol + 1) - rankedData[k];
    }

    // no rquared col
    ntables = ntotCol / 8;
    if ((ntotCol % 8) > 0)
        ntables++;

    stCol = 0;
    AddConditionalBlank();
    TextSubTitle("Ranks");

    for (int i = 0; i < ntables; i++)
    {
        int endCol = stCol + 8;
        if (endCol > ntotCol)
            endCol = ntotCol;

        // titles
        SetTableCol(0, "Step");
        SetTableCol(1, "StepVal");
        int nextCol = 2;
        for (int k = stCol; k < endCol; k++)
        {
            SetTableCol(nextCol, varIDs[k]);
            nextCol++;
        }
        AddNextLine();

        // data
        for (int j = 0; j < nstep; j++)
        {
            SetTableInt(0, GetDepVar().stepIndexes[j] + 1);
            SetTableReal(1, GetDepVar().stepValues[j]);

            nextCol = 2;
            for (int k = stCol; k < endCol; k++)
            {
                SetTableReal(nextCol++, rankedMat[k][j], SC_DecFormat(0));
            }
            AddNextLine();
        }
        AddBlankLine();
        stCol += 8;
    }
}


void DFO_PCCSRC::PlotOutputTable(const SC_DoubleMatrix& dvData,
                                       DC_TableData& outTbl,
                                       int   dvarIndex)
{
    SC_IntArray selIV(niv);
    int nsel = 0;
    for (int j = 0; j < niv; j++)
    {
        for (int k = 0; k < nstep; k++)
        {
            if (fabs(dvData[j][k]) > plotCutoff)
            {
                selIV[nsel++] = j;
                break;
            }
        }
    }

    if (nsel == 0)
    {
        AddLine("*** No IV > plot cutoff ?? - all vars selected ");
        selIV.AllocAndIndex(niv);
        nsel = niv;
    }

    SC_StringArray& colDesc = outTbl.columnDesc;
    int nextCol = colDesc.Size();

    if (ndv == 1)
    {
        for (int j = 0; j < nsel; j++)
            colDesc += GetIndepVar().GetVariableName(selIV[j]);
    }
    else
    {
        const char* dvStr = GetDepVar().GetVariableName(dvarIndex);
        char colStr[80];
        for (int j = 0; j < nsel; j++)
        {
            MakeString(colStr, dvStr, "/", GetIndepVar().GetVariableName(selIV[j]), 80);
            colDesc += colStr;
        }
    }

    for (int j = 0; j < nsel; j++)
    {
        const SC_DoubleArray& dvSel = dvData[selIV[j]];
        for (int k = 0; k < nstep; k++)
            outTbl.dataTable[nextCol + j][k] = dvSel[k];
    }

    nextCol += nsel;
    outTbl.dataTable.MatrixSetSize(nextCol, nstep);
}

