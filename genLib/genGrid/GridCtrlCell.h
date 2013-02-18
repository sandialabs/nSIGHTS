///////////////////////////////////////////////////////////////////////////////////
//
// GridCtrlCell.h
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

#if !defined(AFX_GRIDCTRLCELL_H__5980ED4C_E130_11D3_BCCC_00104B4C74FD__INCLUDED_)
#define AFX_GRIDCTRLCELL_H__5980ED4C_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCtrlBase.h"
#include "GridCell.h"
#include "GridCellMatrix.h"

#define IDC_EDITWND 0x1234

class CGridCtrlCell : public CGridCtrlBase
{
public:
    CGridCtrlCell(int nRows = 2, int nCols = 2, int nFixedRows = 1, int nFixedCols = 1);
    virtual ~CGridCtrlCell();

    void    SetRowCount(int nRows);
    void    SetColCount(int nCols);

    void    DeleteAll();
    void    DeleteRow(int nRow);
    int     InsertRow(int nIndex);
    int     AddRow();
    int     AddFirstRow();
    void    EmptyDataCells(); // deletes data in non-control cells

    CGridCell* GetCell(int nRow, int nCol);
    void    ResetCell(int nRow, int nCol);  // updates text, invalidates
    void    ResetRow(int nRow);                 // updates text, invalidates

    void    SetCell(int nRow, int nCol, CGridCell* pCell, bool bRepaint = true);
    void    SetText(int nRow, int nCol, const char* szText, bool bRepaint = true);
    void    SetRightText(int nRow, int nCol, const char* szText, bool bRepaint = true);
    void    SetCenteredText(int nRow, int nCol, const char* szText, bool bRepaint = true);

    virtual void FinishEdit(const char* szText, int nValue, int nLastChar);

protected:
    virtual void    DrawCell(int nRow, int nCol, CDC* pDC, int x, int y);
    virtual void    OnCellSelected(int nRow, int nCol);
    virtual CSize   GetCellExtent(int nRow, int nCol, CDC* pDC);

    // Edit functionality
    virtual void    GetEditText(int nRow, int nCol, const CString& text);
    virtual CString SetEditText(int nRow, int nCol);

    virtual void    StartEdit(UINT nFirstChar);

    bool IsDataInClipboard();

protected:
    CGridCellMatrix m_matrixCell;

    // Generated message map functions
protected:
    //{{AFX_MSG(CGridCtrlCell)
    //}}AFX_MSG
    afx_msg void OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
};

#endif // AFX_GRIDCTRLCELL_H__5980ED4C_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

