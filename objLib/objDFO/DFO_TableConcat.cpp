///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableConcat.cpp
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
//      Concatenates tables together
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <objDFO/DFO_TableConcat.h>

DFO_TableConcat::TableInputData::TableInputData() :
    doTable(false)
{
}
DFO_TableConcat::TableInputData::TableInputData(const TableInputData& a)
{
    LocalCopy(a);
}
DFO_TableConcat::TableInputData& DFO_TableConcat::TableInputData::operator= (const TableInputData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void DFO_TableConcat::TableInputData::LocalCopy(const TableInputData& a)
{
    doTable     = a.doTable;
    tableObjRef = a.tableObjRef;
}

DFO_TableConcat :: DFO_TableConcat()
    : FuncObjC("Concatenate Tables"), inputTableDC(maxInputTable)
{
    concatRows = true;
    skipFirstColumn = false;
    InitCommon();
}

DFO_TableConcat::DFO_TableConcat(const DFO_TableConcat& a)
    : FuncObjC(a), inputTableDC(maxInputTable)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableConcat :: ~DFO_TableConcat ()
{
}

void DFO_TableConcat::InitCommon()
{
    AddOutPort(outputTableDO);

    for (int i = 0; i < maxInputTable; i++)
        AddUseFlagInPort(tablesToConcat[i].tableObjRef, typeid(DO_TableData), tablesToConcat[i].doTable);

    tablesToConcat[0].doTable = true;

    outputTableDC.SetID("Combined table");
    outputTableDO.tableData = &outputTableDC;

    DoStatusChk();
}

DFO_TableConcat& DFO_TableConcat::operator= (const DFO_TableConcat& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_TableConcat::LocalCopy(const DFO_TableConcat& a)
{
    concatRows = a.concatRows;
    skipFirstColumn = a.skipFirstColumn;
    for (int i = 0; i < maxInputTable; i++)
        tablesToConcat[i] = a.tablesToConcat[i];
}


void  DFO_TableConcat:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    // check input objects
    inputTableDC.SetEmpty();

    ncols = -1;
    nrows = -1;
    for (int i = 0; i < maxInputTable; i++)
        if (tablesToConcat[i].doTable)
        {
            if (!CheckInputFO(tablesToConcat[i].tableObjRef, "input table"))
                return;

            const DC_TableData* nextTbl = static_cast<DO_TableData*>(GetInPortData(tablesToConcat[i].tableObjRef))->tableData;

            if (ncols < 0)
            {
                if (concatRows)
                {
                    ncols = nextTbl->GetNCols();
                    nrows = 0;
                }
                else
                {
                    nrows = nextTbl->GetNRows();
                    ncols = 0;
                }
            }

            if (concatRows)
            {
                if (ncols != nextTbl->GetNCols())
                {
                    SetObjErrMsg("number of columns mismatch -- cannot concatenate");
                    return;
                }
                nrows += nextTbl->GetNRows();
            }
            else
            {
                if (nrows != nextTbl->GetNRows())
                {
                    SetObjErrMsg("number of rows mismatch -- cannot concatenate");
                    return;
                }
                int nnextCols = nextTbl->GetNCols();
                if (skipFirstColumn && (i > 0))
                    nnextCols--;

                if (nnextCols < 1)
                {
                    SetObjErrMsg("no columns after first skipped");
                    return;
                }

                ncols += nnextCols;
            }

            inputTableDC += nextTbl;
        }

    if (inputTableDC.IsEmpty())
    {
        SetObjErrMsg("no tables to concatenate ??");
        outputTableDC.ClearData();
        return;
    }

    if (StatusNotOK())
        outputTableDC.ClearData();
}


void DFO_TableConcat::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    const DC_TableData& inTable = inputTableDC.GetRef(0);
    if (!outputTableDC.Alloc(ncols, nrows, inTable.GetStringLen()))
    {
        SetObjErrMsg("could not allocate output table");
        outputTableDC.ClearData();
        return;
    }
    if (concatRows)
    {
        outputTableDC.columnDesc = inTable.columnDesc;
    }
    else
    {
        outputTableDC.rowDesc = inTable.rowDesc;
    }

    SC_DoubleMatrix& outData = outputTableDC.dataTable;

    int nextRow = 0;
    int nextCol = 0;
    for (int i = 0; i < inputTableDC.Size(); i++)
    {
        const DC_TableData& inputTable = inputTableDC.GetRef(i);
        const SC_DoubleMatrix& inData = inputTable.dataTable;
        if (concatRows)
        {
            int ninRows = inputTable.GetNRows();
            for (int j = 0; j < ninRows; j++)
            {
                outputTableDC.rowDesc.SetString(inputTable.rowDesc[j], nextRow);

                for (int k = 0; k < ncols; k++)
                    outData[k][nextRow] = inData[k][j];

                nextRow++;
            }
        }
        else
        {
            int ninCols = inputTable.GetNCols();
            int startCol = 0;
            if (skipFirstColumn && (i > 0))
                startCol = 1;

            for (int j = startCol; j < ninCols; j++)
            {
                outputTableDC.columnDesc.SetString(inputTable.columnDesc[j], nextCol);
                for (int k = 0; k < nrows; k++)
                    outData[nextCol][k] = inData[j][k];
                nextCol++;
            }
        }
    }

    if (concatRows)
        outputTableDC.SetSize(ncols, nextRow);
    else
        outputTableDC.SetSize(nextCol, nrows);
}


