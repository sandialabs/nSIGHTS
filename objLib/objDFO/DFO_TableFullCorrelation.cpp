///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableFullCorrelation.cpp
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

#include <genClass/U_Real.h>

#include <objDFO/DFO_TableFullCorrelation.h>

DFO_TableFullCorrelation :: DFO_TableFullCorrelation() :
    FuncObjC("Correlate All Table Columns")
{
    logData = false;
    correlationType = ctPearson;
    InitCommon();
}

DFO_TableFullCorrelation::DFO_TableFullCorrelation(const DFO_TableFullCorrelation& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableFullCorrelation :: ~DFO_TableFullCorrelation ()
{
}

void DFO_TableFullCorrelation::InitCommon()
{
    outputTableDC.SetID("Correlations");
    outputTableDO.tableData = &outputTableDC;

    AddInPort(inputTableObjRef, typeid(DO_TableData));
    AddOutPort(outputTableDO);

    DoStatusChk();
}

DFO_TableFullCorrelation& DFO_TableFullCorrelation::operator= (const DFO_TableFullCorrelation& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableFullCorrelation::LocalCopy(const DFO_TableFullCorrelation& a)
{
    inputTableObjRef = a.inputTableObjRef;
    logData = a.logData;
    correlationType = a.correlationType;
}


void  DFO_TableFullCorrelation:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (CheckInputFO(inputTableObjRef, "table input data source"))
    {
        inputTableDC = static_cast<DO_TableData*>(GetInPortData(inputTableObjRef))->tableData;
        if (inputTableDC->GetNCols() < 2)
            SetObjErrMsg("table must have more than 1 column");
        if (inputTableDC->GetNRows() < 2)
            SetObjErrMsg("table must have more than 1 row");
    }

    if (correlationType == ctOther)
        SetObjErrMsg("other type not supported");
}

void DFO_TableFullCorrelation:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;

    int ncol = inputTableDC->GetNCols();
    outputTableDC.Alloc(ncol, ncol);
    int i;
    for (i = 0; i < ncol; i++)
    {
        outputTableDC.SetTitle(i, inputTableDC->GetTitle(i));
        outputTableDC.rowDesc.SetString(outputTableDC.GetTitle(i), i);
        outputTableDC.dataTable[i].SetNull();
        outputTableDC.dataTable[i][i] = 1.0;
    }
    outputTableDC.columnDesc.SetSize(ncol);
    outputTableDC.rowDesc.SetSize(ncol);
    outputTableDC.dataTable.SetSize(ncol);

    for (i = 0; i < ncol - 1; i++)
        for (int j = i + 1; j < ncol; j++)
        {
            outputTableDC.dataTable[i][j] = CalcCorrelation(i, j);
            outputTableDC.dataTable[j][i] = outputTableDC.dataTable[i][j];
        }

    if (correlationType == ctPearson)
        outputTableDC.SetID("Pearson Correlations");
    else
        outputTableDC.SetID("Spearman Correlations");

}


double DFO_TableFullCorrelation::CalcCorrelation(int xcol, int ycol)
{
    SC_DoubleArray& xIn = inputTableDC->dataTable[xcol];
    SC_DoubleArray& yIn = inputTableDC->dataTable[ycol];

    SC_DoubleArray xCorr, yCorr;

    xCorr.Alloc(xIn.Size());
    yCorr.Alloc(xIn.Size());

    int nOK = 0;
    int i;
    for (i = 0; i < xIn.Size(); i++)
    {
        double nextX = xIn[i];
        double nextY = yIn[i];

        if (RealIsNull(nextX) || RealIsNull(nextY))
            continue;

        if (logData)
        {
            if ((nextX < stdEps) || (nextY < stdEps))
                continue;
            nextX = log10(nextX);
            nextY = log10(nextY);
        }

        xCorr[nOK] = nextX;
        yCorr[nOK++] = nextY;
    }

    if (nOK < 2)
        return nullReal;

    xCorr.SetSize(nOK);
    yCorr.SetSize(nOK);

    if (correlationType == ctPearson)
        return PearsonR(xCorr, yCorr);

    return  SpearmanR(xCorr, yCorr);
}

