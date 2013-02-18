///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableRowAccumulate.cpp
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

#include <genClass/U_Msg.h>


#include <objDFO/DFO_TableRowAccumulate.h>


DFO_TableRowAccumulate :: DFO_TableRowAccumulate() : FuncObjC("Table Row Accumulate/Delta")
{
    skipXColumn = true;
    xcolumnIndex = 0;
    tableRowOperation = troAccumulateAll;

    InitCommon();
}

DFO_TableRowAccumulate::DFO_TableRowAccumulate(const DFO_TableRowAccumulate& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}


void DFO_TableRowAccumulate::InitCommon()
{
    cumulativeTableDO.tableData = &cumulativeTableDC;
    AddOutPort(cumulativeTableDO);
    AddInPort(inputTableObjRef,   typeid(DO_TableData));

    DoStatusChk();
}

DFO_TableRowAccumulate& DFO_TableRowAccumulate::operator= (const DFO_TableRowAccumulate& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableRowAccumulate::LocalCopy(const DFO_TableRowAccumulate& a)
{
    inputTableObjRef    = a.inputTableObjRef;
    skipXColumn         = a.skipXColumn;
    xcolumnIndex        = a.xcolumnIndex;
    tableRowOperation   = a.tableRowOperation;
}

void  DFO_TableRowAccumulate:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    //  get data
    if (!CheckInputFO(inputTableObjRef, "Input table"))
        return;

    if (StatusNotOK())
        return;

    inputTableDC = static_cast<DO_TableData*>(GetInPortData(inputTableObjRef))->tableData;

    nrows = inputTableDC->GetNRows();
    ncols = inputTableDC->GetNCols();

    if ((nrows == 0) || (ncols == 0))
        SetObjErrMsg("no rows or columns in input table");

    if (skipXColumn && (xcolumnIndex >= ncols))
        SetObjErrMsg("X column index out of range");

    if (skipXColumn && (ncols == 1))
        SetObjErrMsg("no columns after X column is removed");
}


void DFO_TableRowAccumulate:: CalcOutput(FOcalcType  calcType)
{
    cumulativeTableDC.ClearAll();
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (!cumulativeTableDC.Alloc(ncols, nrows, inputTableDC->GetStringLen()))
    {
        SetObjErrMsg("Table allocation problem");
        return;
    }
    cumulativeTableDC.SetSizeToAlloc();
    cumulativeTableDC.columnDesc = inputTableDC->columnDesc;
    cumulativeTableDC.rowDesc = inputTableDC->rowDesc;

    for (int i = 0; i < ncols; i++)
    {
        SC_DoubleArray& currOutCol = cumulativeTableDC.GetColData(i);
        const SC_DoubleArray& currInCol = inputTableDC->dataTable[i];

        if (skipXColumn && (xcolumnIndex == i))
        {
            currOutCol = currInCol;
            continue;
        }

        double lastVal = 0.0;
        for (int j = 0; j < nrows; j++)
        {
            double nextVal = currInCol[j];
            if (RealIsNull(nextVal))
                nextVal = 0.0;

            switch (tableRowOperation ) {
                case troAccumulateAll: {
                    lastVal += nextVal;
                    currOutCol[j] = lastVal;
                    break;
                }
                case troAccumulatePositive: {
                    if (nextVal > 0.0)
                        lastVal += nextVal;
                    currOutCol[j] = lastVal;
                    break;
                }
                case troDelta : {
                    currOutCol[j] = nextVal - lastVal;
                    lastVal = nextVal;
                    break;
                }
                default : {
                    GenAppInternalError("DFO_TableRowAccumulate");
                }
            }
        }
    }
    cumulativeTableDC.SetID("Row accumulate");
}

