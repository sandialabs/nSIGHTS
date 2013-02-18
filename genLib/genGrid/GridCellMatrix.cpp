///////////////////////////////////////////////////////////////////////////////////
//
// GridCellMatrix.cpp
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
// DESCRIPTION: Windows specific code for implementing basic spreadsheet type
//              data entry/display.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "GridCellMatrix.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CGridCellArray::CGridCellArray(int nItems)
{
    SetSize(nItems);
}

void CGridCellArray::DeleteAll()
{
    for (int i = 0; i < GetSize(); i++ ) {
        CGridCell* pCell = GetAt(i);
        if (pCell != NULL) {
            delete pCell;
        }
    }
    RemoveAll();
}

CGridCellMatrix::CGridCellMatrix(int nRows, int nCols) :
    m_nRows(nRows),
    m_nCols(nCols)
{
    m_aCellColumn.SetSize(m_nCols);

    // initialize each nColumn to a cell array of nRow length
    for (int i = 0; i < m_nCols; i++) {
        m_aCellColumn[i] = new CGridCellArray(m_nRows);
        ASSERT(m_aCellColumn[i]);
    }
}

CGridCellMatrix::~CGridCellMatrix()
{
    DeleteAll();
}

// delete all nColumns
void CGridCellMatrix::DeleteAll()
{
    for (int i = 0; i < m_nCols; i++) {
        m_aCellColumn[i]->DeleteAll();
        delete m_aCellColumn[i];
    }
}

void CGridCellMatrix::AddRow()
{
    for (int i = 0; i < m_nCols; i++) {
        m_aCellColumn[i]->Add(NULL);
    }
    m_nRows++;
}

void CGridCellMatrix::AddColumn()
{
    CGridCellArray* pNewColumn = new CGridCellArray( m_nRows );
    ASSERT(pNewColumn);
    m_aCellColumn.Add( pNewColumn );
    m_nCols++;
}

void CGridCellMatrix::DeleteRow(int nRow)
{
    ASSERT( (nRow >= 0) && (nRow < m_nRows) );
    for (int i = 0; i < m_nCols; i++) {
        CGridCell* pCell = m_aCellColumn[i]->GetAt(nRow);
        if (pCell != NULL) {
            delete pCell;
        }
        m_aCellColumn[i]->RemoveAt(nRow);
    }
    m_nRows--;
}

void CGridCellMatrix::DeleteColumn(int nCol)
{
    ASSERT(nCol >= 0 && nCol < m_nCols);
    m_aCellColumn[nCol]->DeleteAll();
    delete m_aCellColumn[nCol];
    m_aCellColumn.RemoveAt(nCol);
    m_nCols--;
}

void CGridCellMatrix::InsertRowAt(int nRow)
{
    ASSERT(nRow >= 0 && nRow <= m_nRows);
    for (int i = 0; i < m_nCols; i++) {
        m_aCellColumn[i]->InsertAt(nRow, (CGridCell*)NULL);
    }
    m_nRows++;
}

void CGridCellMatrix::InsertColumnAt(int nCol)
{
    ASSERT( (nCol >= 0) && (nCol < m_nCols) );
    CGridCellArray* pNewColumn = new CGridCellArray(m_nRows);
    ASSERT(pNewColumn);
    m_aCellColumn.InsertAt(nCol, pNewColumn);
    m_nCols++;
}

void CGridCellMatrix::SetCell(int nRow, int nCol, CGridCell* pCell)
{
    CGridCell* pCellOld = m_aCellColumn[nCol]->GetAt(nRow);
    if (pCellOld != NULL) {
        delete pCellOld;
    }
    m_aCellColumn[nCol]->RemoveAt(nRow);
    m_aCellColumn[nCol]->InsertAt(nRow, pCell);
}

CGridCell* CGridCellMatrix::GetCell(int nRow, int nCol)
{
    return m_aCellColumn[nCol]->GetAt(nRow);
}

void CGridCellMatrix::SetRowCount(int nRows)
{
    ASSERT(nRows > 0);
    if (nRows == GetRowCount()) {
        return;
    } else if (nRows < GetRowCount()) {
        // delete nRows from the end
        int nIterations = GetRowCount() - nRows;
        for (int i = 0; i < nIterations; i++) {
            DeleteRow(GetRowCount() - 1);
        }
    } else { //(nRows > GetRowCount())
        // add nRows to the end
        int nIterations = nRows - GetRowCount();
        for (int i = 0; i < nIterations; i++) {
            AddRow();
        }
    }
}

void CGridCellMatrix::SetColCount(int nCols)
{
    ASSERT(nCols > 0);
    if (nCols == GetColCount()) {
        return;
    } else if (nCols < GetColCount()) {
        // delete nCols from the end
        int nIterations = GetColCount() - nCols;
        for (int i = 0; i < nIterations; i++) {
            DeleteColumn(GetColCount() - 1);
        }
    } else { // (nCols > GetColCount())
        // add nCols to the end
        int nIterations = nCols - GetColCount();
        for (int i = 0; i < nIterations; i++) {
            AddColumn();
        }
    }
}

