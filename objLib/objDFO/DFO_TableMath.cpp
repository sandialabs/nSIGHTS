///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableMath.cpp
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
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objDFO/DFO_TableMath.h>


DFO_TableMath :: DFO_TableMath() : FuncObjC("Table Math")
{
    skipXColumn = true;
    xcolumnIndex = 0;
    tableMathOperation = tmoAdd;

    InitCommon();
}

DFO_TableMath::DFO_TableMath(const DFO_TableMath& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}


void DFO_TableMath::InitCommon()
{
    resultTableDO.tableData = &resultTableDC;
    AddOutPort(resultTableDO);
    AddInPort(inputTableAObjRef,   typeid(DO_TableData));
    AddInPort(inputTableBObjRef,   typeid(DO_TableData));

    DoStatusChk();
}

DFO_TableMath& DFO_TableMath::operator= (const DFO_TableMath& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableMath::LocalCopy(const DFO_TableMath& a)
{
    inputTableAObjRef   = a.inputTableAObjRef;
    inputTableBObjRef   = a.inputTableBObjRef;
    skipXColumn         = a.skipXColumn;
    xcolumnIndex        = a.xcolumnIndex;
    tableMathOperation  = a.tableMathOperation;
}

void  DFO_TableMath:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    //  get data
    if (!CheckInputFO(inputTableAObjRef, "Input table A"))
        return;
    if (!CheckInputFO(inputTableBObjRef, "Input table B"))
        return;

    if (StatusNotOK())
        return;

    inputTableADC = static_cast<DO_TableData*>(GetInPortData(inputTableAObjRef))->tableData;
    inputTableBDC = static_cast<DO_TableData*>(GetInPortData(inputTableBObjRef))->tableData;

    nrows = inputTableADC->GetNRows();
    ncols = inputTableADC->GetNCols();

    if ((nrows == 0) || (ncols == 0))
        SetObjErrMsg("no rows or columns in input table A");

    if (skipXColumn && (xcolumnIndex >= ncols))
        SetObjErrMsg("X column index out of range");

    if (skipXColumn && (ncols == 1))
        SetObjErrMsg("no columns after X column is removed");

    int bnRows = inputTableBDC->GetNRows();
    int bnCols = inputTableBDC->GetNCols();

    bIsSingleRow = (bnRows == 1) && (nrows > 1);
    bIsSingleCol = (bnCols == 1) && (ncols > 1);
    bColOffset = 0;
    if (bIsSingleRow)
    {
        if (skipXColumn && (ncols > bnCols))
            bColOffset = 1;

        if ((bnCols + bColOffset) != ncols)
            SetObjErrMsg("table A and single row table B must have the same number of columns");
    }
    else if (bIsSingleCol)
    {
        if (bnRows != nrows)
            SetObjErrMsg("table A and single column table B must have the same number of rows");
    }
    else
    {
        if ((nrows != bnRows) || (ncols != bnCols))
            SetObjErrMsg("table A and B must have the same number of rows and columns");
    }

}


void DFO_TableMath:: CalcOutput(FOcalcType  calcType)
{
    resultTableDC.ClearAll();
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (bIsSingleRow)
        GenAppInfoMsg("Table Math", "Table B is single row");
    if (bIsSingleCol)
        GenAppInfoMsg("Table Math", "Table B is single column");

    int reqStrLen = inputTableADC->GetStringLen() + inputTableBDC->GetStringLen() + 4;
    if (!resultTableDC.Alloc(ncols, nrows, reqStrLen))
    {
        SetObjErrMsg("Table allocation problem");
        return;
    }
    resultTableDC.SetSizeToAlloc();

    resultTableDC.rowDesc = inputTableADC->rowDesc;

    const char* opStrs[] = {"+","-","*","/"};
    const char* opStr = opStrs[tableMathOperation];

    for (int i = 0; i < ncols; i++)
    {
        SC_DoubleArray& currOutCol = resultTableDC.GetColData(i);
        const SC_DoubleArray& currInColA = inputTableADC->dataTable[i];

        if (skipXColumn && (xcolumnIndex == i))
        {
            currOutCol = currInColA;
            CopyString(resultTableDC.columnDesc[i],  inputTableADC->columnDesc[i], reqStrLen);
            continue;
        }

        int bColumnSel = i;
        if (bIsSingleRow)
        {
            bColumnSel -= bColOffset;
            MakeString(resultTableDC.columnDesc[i],  inputTableADC->columnDesc[i], opStr, inputTableBDC->columnDesc[bColumnSel],"[0]", reqStrLen);
        }
        else if (bIsSingleCol)
        {
            bColumnSel = 0;
            MakeString(resultTableDC.columnDesc[i],  inputTableADC->columnDesc[i], opStr, inputTableBDC->columnDesc[bColumnSel],"[0]", reqStrLen);
        }
        else
            MakeString(resultTableDC.columnDesc[i],  inputTableADC->columnDesc[i], opStr, inputTableBDC->columnDesc[i], reqStrLen);


        const SC_DoubleArray& currInColB = inputTableBDC->dataTable[bColumnSel];

        double lastVal = 0.0;
        for (int j = 0; j < nrows; j++)
        {
            double nextAVal = currInColA[j];

            int bRowSel = j;
            if (bIsSingleRow)
                bRowSel = 0;

            double nextBVal = currInColB[bRowSel];
            double& nextRVal = currOutCol[j];
            if (RealIsNull(nextAVal) || RealIsNull(nextBVal))
            {
                nextRVal = nullReal;
                continue;
            }

            switch (tableMathOperation ) {
                case tmoAdd: {
                    nextRVal = nextAVal + nextBVal;
                    break;
                }
                case tmoSubtract: {
                    nextRVal = nextAVal - nextBVal;
                    break;
                }
                case tmoMultiply : {
                    nextRVal = nextAVal * nextBVal;
                    break;
                }
                case tmoDivide : {
                    if (fabs(nextBVal) > 1.0E-99)
                        nextRVal = nextAVal / nextBVal;
                    else
                        nextRVal = nullReal;
                    break;
                }
                default : {
                    GenAppInternalError("DFO_TableMath");
                }
            }
        }
    }
    resultTableDC.SetID("Table Math");
}

