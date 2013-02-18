///////////////////////////////////////////////////////////////////////////////////
//
// GridCtrlCell.cpp
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

#include "GridCtrlCell.h"
#include "TextGridCell.h"
#include "ComboGridCell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGridCtrlCell::CGridCtrlCell(int nRows, int nCols, int nFixedRows, int nFixedCols) :
    CGridCtrlBase(nRows, nCols, nFixedRows, nFixedCols),
    m_matrixCell(nRows, nCols)
{
    // note - did not initialize cells, they are already zeroed
}

CGridCtrlCell::~CGridCtrlCell()
{
}

CGridCell* CGridCtrlCell::GetCell(int nRow, int nCol)
{
    return m_matrixCell.GetCell(nRow, nCol);
}

void CGridCtrlCell::ResetCell(int nRow, int nCol)
{
    m_matrixCell.GetCell(nRow, nCol)->UpdateText();
    InvalidateCell(nRow, nCol);
}

void CGridCtrlCell::ResetRow(int nRow)
{
    for (int i = 0; i < GetColCount(); i++)
        ResetCell(nRow, i);
}

void CGridCtrlCell::SetRowCount(int nRows)
{
    if (nRows == GetRowCount())
        return;

    int nAddedRows = nRows - GetRowCount();

    CGridCtrlBase::SetRowCount(nRows);

    m_matrixCell.SetRowCount(nRows);
    int nStartRow = nRows - nAddedRows;
    if (nAddedRows > 0) {
        // initialize all added nRows to NULL
        for (int i = 0; i < GetColCount(); i++) {
            for (int j = nStartRow; j < GetRowCount(); j++) {
                SetCell(j, i, 0, FALSE);
            }
        }
    }
}

void CGridCtrlCell::DeleteAll()
{
    for ( int i = m_nRows - 1; i > 1; i-- ) {
        m_matrixCell.DeleteRow(i);
    }
    m_nRows = 1;
}

void CGridCtrlCell::DeleteRow(int nRow)
{
    m_matrixCell.DeleteRow( nRow );
    m_nRows--;
    Invalidate();
}

int CGridCtrlCell::AddRow()
{
    int nRowIndex = m_nRows;
    m_nRows++;
    m_auRowHeights.SetSize( m_nRows );
    m_auRowHeights[ nRowIndex ] = m_nDefCellHeight;
    m_matrixCell.AddRow();

    // initialize all added nCols to NULL
    for ( int i = 0; i < GetColCount(); i++ ) {
        SetCell(nRowIndex, i, 0, FALSE);
    }
    return nRowIndex;
}

int CGridCtrlCell::InsertRow(int nIndex)
{
    int nRowIndex = nIndex;
    m_nRows++;
    m_auRowHeights.InsertAt( nIndex, m_nDefCellHeight );
    m_matrixCell.InsertRowAt( nRowIndex );

    // initialize all added nCols to NULL
    for ( int i = 0; i < GetColCount(); i++ ) {
        SetCell(nRowIndex, i, 0, FALSE);
    }

    return nRowIndex;
}

int CGridCtrlCell::AddFirstRow()
{
    int nRowIndex = 1;
    m_nRows++;
    m_auRowHeights.InsertAt( nRowIndex, m_nDefCellHeight );
    m_matrixCell.InsertRowAt( nRowIndex );

    // initialize all added nCols to NULL
    for ( int i = 0; i < GetColCount(); i++ ) {
        SetCell( nRowIndex, i, 0, FALSE );
    }

    return nRowIndex;
}


void CGridCtrlCell::SetColCount( int nCols )
{
    if (nCols == GetColCount()) {
        return;
    }
    int nAddedCols = nCols - GetColCount();
    CGridCtrlBase::SetColCount(nCols);
    m_matrixCell.SetColCount(nCols);
    if (nAddedCols > 0) {
        // initialized all added nCols to NULL
        int startCol = nCols - nAddedCols;
        for (int i = startCol; i < GetColCount(); i++) {
            for (int j= 0; j < GetRowCount(); j++) {
                SetCell(j, i, 0, FALSE);
            }
        }
    }
}

void CGridCtrlCell::SetCell(int nRow, int nCol, CGridCell* pCell, bool bRepaint)
{
    m_matrixCell.SetCell(nRow, nCol, pCell);
    if (bRepaint && GetSafeHwnd()) {
        Invalidate();
    }
}

void CGridCtrlCell::SetText(int nRow, int nCol, const char* szText, bool bRepaint)
{
    SetCell(nRow, nCol, new CTextGridCell(szText), bRepaint);
}

void CGridCtrlCell::SetRightText(int nRow, int nCol, const char* szText, bool bRepaint)
{
    SetCell(nRow, nCol, new CTextGridCell(szText, CTextGridCell::alignRight), bRepaint);
}

void CGridCtrlCell::SetCenteredText(int nRow, int nCol, const char* szText, bool bRepaint)
{
    SetCell(nRow, nCol, new CTextGridCell(szText, CTextGridCell::alignCenter), bRepaint);
}

void CGridCtrlCell::DrawCell(int nRow, int nCol, CDC* pDC, int x, int y)
{
    const CGridCell* pCell = GetCell(nRow, nCol);
    if (pCell != NULL) {
        if (m_bConstrained) {
            pCell->DrawConstrained(pDC, x, y,
                                   m_auColWidths[nCol] - 2 *m_nCellBorderThickness,
                                   m_auRowHeights[nRow] - 2 * m_nCellBorderThickness);
        } else {
            pCell->DrawUnconstrained(pDC, x, y);
        }
    }
}

void CGridCtrlCell::OnCellSelected(int nRow, int nCol)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    if (pCell != NULL) {
        pCell->OnCellSelected(this, nRow, nCol);
    }
}

CSize CGridCtrlCell::GetCellExtent(int nRow, int nCol, CDC* pDC)
{
    const CGridCell* pCell = m_matrixCell.GetCell(nRow, nCol);
    if (pCell == NULL) {
        return CSize(0, 0);
    }
    return pCell->GetExtent(pDC);
}

CString CGridCtrlCell::SetEditText(int nRow, int nCol)
{
    CString retString;
    CGridCell* pCell = m_matrixCell.GetCell(nRow, nCol);
    if (pCell != NULL) {
        retString = pCell->GetText();
    }
    return retString;
}


void CGridCtrlCell::GetEditText(int nRow, int nCol, const CString& szText)
{
    CGridCell* pCell = m_matrixCell.GetCell(nRow, nCol);
    if (pCell != NULL) {
        pCell->SetText( szText );
    }
}

void CGridCtrlCell::EmptyDataCells()
{
    for ( int i = m_nFixedCols; i< m_nCols; i++ ) {
        for ( int j = m_nFixedRows; j < m_nRows; j++) {
            m_matrixCell.SetCell(j, i, 0);
        }
    }
}

void CGridCtrlCell::StartEdit(UINT nFirstChar)
{
    CGridCtrlBase::StartEdit(nFirstChar);
    CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
    CRect rectCell(GetColStartX(m_cellActive.m_nCol),
                   GetRowStartY(m_cellActive.m_nRow),
                   GetColStartX(m_cellActive.m_nCol+1),
                   GetRowStartY(m_cellActive.m_nRow+1));

    if (pCell != NULL && pCell->CanEdit()) {
        pCell->CreateEditWnd(rectCell, this,
                             IDC_EDITWND,
                             m_cellActive.m_nRow, m_cellActive.m_nCol,
                             nFirstChar);
    } else {
        m_bEditMode = false;
    }
}

void CGridCtrlCell::FinishEdit(const char* szText, int nValue, int nLastChar)
{
    // Change values in Grid
    CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
    pCell->SetText(szText);
    InvalidateCellRect(CGridCellRange(m_cellActive, m_cellActive));

    StopEdit(false);

    // if the right key continue editing on next cell
    switch (nLastChar) {
    case VK_TAB:
    case VK_DOWN:
    case VK_UP:
    case VK_RIGHT:
    case VK_LEFT:
    case VK_NEXT:
    case VK_PRIOR:
    case VK_HOME:
    case VK_END:
        OnKeyDown(nLastChar, 0, 0);
        UpdateWindow();
        StartEdit(nLastChar);
        break;
    }
}

BEGIN_MESSAGE_MAP(CGridCtrlCell, CGridCtrlBase)
//{{AFX_MSG_MAP(CGridCtrlCell)
//}}AFX_MSG_MAP
ON_NOTIFY(1, IDC_EDITWND, OnEndInPlaceEdit)
END_MESSAGE_MAP()

void CGridCtrlCell::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    GRID_DISPINFO* pDispInfo = (GRID_DISPINFO*)pNMHDR;
    if (!::IsWindow(GetSafeHwnd())) {
        return;
    }
    if (pDispInfo->m_nLastChar == VK_ESCAPE) {
        *pResult = 0;
        return;
    }
    FinishEdit(pDispInfo->m_szText, pDispInfo->m_nValue, pDispInfo->m_nLastChar);
    *pResult = 0;
}


bool CGridCtrlCell::IsDataInClipboard()
{
    return ::IsClipboardFormatAvailable(CF_TEXT);
}

