///////////////////////////////////////////////////////////////////////////////////
//
// DC_TableData.cpp
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
// DESCRIPTION: Platform independent and application independent base classes and
//              support classes.
//              Data Classes (DC) that define basic types of data used within the
//              framework: Tabular Data.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>

#include <genClass/U_Str.h>

#include <genClass/U_Msg.h>

#include <genClass/DC_TableData.h>

DC_TableData :: DC_TableData()
{
    SetToNull(tableID);
}

DC_TableData::DC_TableData(const DC_TableData& a)
    : dataTable(a.dataTable), columnDesc(a.columnDesc), rowDesc(a.rowDesc)
{
    CopyString(tableID, a.tableID, tableIDLen);
}

DC_TableData&  DC_TableData::operator=(const DC_TableData& a)
{
    if (&a != this)
        {
            dataTable = a.dataTable;
            columnDesc = a.columnDesc;
            rowDesc = a.rowDesc;
            CopyString(tableID, a.tableID, tableIDLen);
        }

    return *this;
}



DC_TableData :: ~DC_TableData()
{
    DeAlloc();
}

void DC_TableData :: DeAlloc()
{
    dataTable.DeAlloc();
    columnDesc.DeAlloc();
    rowDesc.DeAlloc();
    SetToNull(tableID);
}



bool DC_TableData::Alloc(int nCol, int nRow, int nChar)
{
    try
        {
            dataTable.MatrixAlloc(nCol, nRow);

            //  set char lens for descs
            columnDesc.DeAlloc();
            columnDesc.SetStringLen(nChar);
            columnDesc.Alloc(nCol);

            rowDesc.DeAlloc();
            rowDesc.SetStringLen(nChar);
            rowDesc.Alloc(nRow);
        }
    catch (SC_Array::AllocationError)
        {
            DeAlloc();
            return false;
        }

    return true;
}

bool DC_TableData::Alloc(const DC_TableData& a)
{
    return Alloc(a.GetNCols(), a.GetNRows(), a.columnDesc.GetStringLen());
}

void DC_TableData::SetSize(int nCol, int nRow)
{
    if (rowDesc.AllocSize()>=nRow)
        rowDesc.SetSize(nRow);
    columnDesc.SetSize(nCol);
    dataTable.MatrixSetSize(nCol, nRow);
}

void DC_TableData::SetSizeToAlloc()
{
    int nrow = 0;
    if (dataTable.AllocSize() > 0)
        nrow = dataTable[0].AllocSize();
    SetSize(columnDesc.AllocSize(), nrow);
}

int      DC_TableData::GetStringLen() const
{
    int strLen = columnDesc.GetStringLen();
    if (strLen < rowDesc.GetStringLen())
        strLen = rowDesc.GetStringLen();
    return strLen;
}

bool DC_TableData::ResizableAlloc(int initialColAlloc, int initialRowAlloc,  int nChar)
{
    try
        {
            DeAlloc();

            dataTable.SetResizable(initialColAlloc);
            dataTable.SetSize(initialColAlloc);

            for ( int i = 0; i < initialColAlloc; i++)
                {
                    dataTable[i].SetResizable(initialRowAlloc);
                    dataTable[i].SetNull();
                    dataTable[i].SetEmpty();
                }

            columnDesc.SetStringLen(nChar);
            columnDesc.SetResizable(initialColAlloc);

            rowDesc.SetStringLen(nChar);
            rowDesc.SetResizable(initialRowAlloc);
        }
    catch (SC_Array::AllocationError)
        {
            DeAlloc();
            return false;
        }

    return true;
}

void DC_TableData::ClearData()
{
    for (int i = 0; i < dataTable.AllocSize(); i++)
        dataTable[i].SetEmpty();
}

void DC_TableData::ClearAll()
{
    DeAlloc();
}

int DC_TableData::GetNRows() const
{
    if (dataTable.AllocSize() > 0)
        return dataTable[0].Size();
    return 0;
}

void DC_TableData::InsertRow(int beforeRow)
{
    for (int i = 0; i < GetNCols(); i++)
        dataTable[i].InsertItem(nullReal, beforeRow);
    rowDesc.InsertItem("", beforeRow);
}

void DC_TableData::DeleteRow(int delRow)
{
    for (int i = 0; i < GetNCols(); i++)
        dataTable[i].DeleteItem(delRow);
    rowDesc.DeleteItem(delRow);
}

void DC_TableData::InsertCol(int beforeCol)
{
    dataTable.InsertCol(beforeCol);
    columnDesc.InsertItem("new", beforeCol);
}
void DC_TableData::DeleteCol(int delCol)
{
    dataTable.DeleteCol(delCol);
    columnDesc.DeleteItem(delCol);
}

void  DC_TableData::SetTitle(int titleCol, const char* titleText)
{
    CopyString(columnDesc[titleCol], titleText, columnDesc.GetStringLen());
    if (titleCol >= columnDesc.Size())
        columnDesc.SetSize(titleCol + 1);
}


void DC_TableData::SetID(const char id[])
{
    CopyString(tableID, id, tableIDLen);
}

void DC_TableData::GetID(char id[],
                         int  maxIDlen)
{
    CopyString(id, tableID, maxIDlen);
}

SC_DoubleArray&         DC_TableData::GetColData(int colIndex)
{
    return dataTable[colIndex];
}

const SC_DoubleArray&   DC_TableData::GetColData(int colIndex) const
{
    return dataTable[colIndex];
}

void DC_TableData::GetColData(int colIndex, SC_DoubleArray& colData) const
{
    colData = dataTable[colIndex];
}

void DC_TableData::GetRowData(int rowIndex, SC_DoubleArray& rowData) const
{
    int nRow = dataTable.Size();
    rowData.AllocAndSetSize(nRow);
    for (int i = 0; i < nRow; i++)
        rowData[i] = dataTable[i][rowIndex];
}

void DC_TableData::SetColumnDesc(const char* baseDesc, int offset)
{
    columnDesc.SetIndexed(baseDesc, offset);
}

void DC_TableData::SetRowDesc(const char* baseDesc, int offset)
{
    rowDesc.SetIndexed(baseDesc, offset);
}

