///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableTranspose.cpp
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
//      Switches a table's columns and rows
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <objDFO/DFO_TableTranspose.h>

DFO_TableTranspose :: DFO_TableTranspose() :
    FuncObjC("Transpose Table Columns & Rows")
{
    skipXColumn = true;
    xcolumnIndex = 0;

    InitCommon();
}

DFO_TableTranspose::DFO_TableTranspose(const DFO_TableTranspose& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableTranspose :: ~DFO_TableTranspose ()
{
}

void DFO_TableTranspose::InitCommon()
{
    outputTableDC.SetID("Transpose");
    outputTableDO.tableData = &outputTableDC;

    AddInPort(inputTableObjRef, typeid(DO_TableData));
    AddOutPort(outputTableDO);

    DoStatusChk();
}

DFO_TableTranspose& DFO_TableTranspose::operator= (const DFO_TableTranspose& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableTranspose::LocalCopy(const DFO_TableTranspose& a)
{
    inputTableObjRef = a.inputTableObjRef;
    skipXColumn         = a.skipXColumn;
    xcolumnIndex        = a.xcolumnIndex;
}


void  DFO_TableTranspose:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (CheckInputFO(inputTableObjRef, "table input data source"))
    {
        inputTableDC = static_cast<DO_TableData*>(GetInPortData(inputTableObjRef))->tableData;

        ninputRows = inputTableDC->GetNRows();
        ninputCols = inputTableDC->GetNCols();

        if ((ninputCols < 1) || (ninputRows < 1))
            SetObjErrMsg("No input table columns or rows");

        if ((ninputRows < 2) && (ninputCols < 2))
            SetObjErrMsg("table must have more than 1 entry");

        if (skipXColumn)
        {
            if (xcolumnIndex >= ninputCols)
                SetObjErrMsg("X column index out of range");

            if (ninputCols == 1)
                SetObjErrMsg("no data after X column is removed");
        }
    }

}

void DFO_TableTranspose:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;

    int nchar = inputTableDC->GetStringLen();

    int noutputRows = ninputCols;
    if (skipXColumn)
        noutputRows--;
    int noutputCols = ninputRows;

    outputTableDC.Alloc(noutputCols, noutputRows, nchar);
    outputTableDC.columnDesc = inputTableDC->rowDesc;
    SetDesc(outputTableDC.columnDesc, "Column", noutputCols);


    SC_DoubleMatrix& outTable = outputTableDC.dataTable;
    const SC_DoubleMatrix& inTable = inputTableDC->dataTable;

    int nextOutRow = 0;
    for (int i = 0; i < ninputCols; i++)
    {
        if (skipXColumn && (xcolumnIndex == i))
            continue;

        outputTableDC.rowDesc += inputTableDC->columnDesc[i];

        for (int j = 0; j < ninputRows; j++)
            outTable[j][nextOutRow] = inTable[i][j];

        nextOutRow++;
    }
    SetDesc(outputTableDC.rowDesc, "Row", noutputRows);
    outputTableDC.SetSize(noutputCols, noutputRows);
}



void DFO_TableTranspose::SetDesc(SC_StringArray& descStr, 
                                    const char*     desc,
                                    int             nreq)
{
    int ndesc = descStr.Size();
    if (ndesc < nreq)
    {
        for (int i = ndesc; i < nreq; i++)
        {
            char tempStr[80];
            CopyString(tempStr, desc, 80);
            ConcatInt(i, tempStr, 3, 80);
            descStr.SetString(tempStr, i);
        }
        descStr.SetSize(nreq);
    }
}

