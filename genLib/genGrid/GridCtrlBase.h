///////////////////////////////////////////////////////////////////////////////////
//
// GridCtrlBase.h
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
//              Overall definition/control of sheet
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCTRLBASE_H__5980ED45_E130_11D3_BCCC_00104B4C74FD__INCLUDED_)
#define AFX_GRIDCTRLBASE_H__5980ED45_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GridCtrlBase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGridCtrlBase window

#include "GridCellIndices.h"
#include "GridCellRange.h"

struct GRID_DISPINFO
{
    NMHDR hdr;
    int m_Row;
    int m_nCol;
    int m_nLastChar;
    const char* m_szText;
    int m_nValue;
};

class CGridCtrlBase : public CWnd
{
    // Construction
public:
    CGridCtrlBase(int nRows = 1, int nCols = 1, int nFixedRows = 1, int nFixedCols = 1);
    virtual ~CGridCtrlBase();

    BOOL SubclassDlgItem(UINT nID, CWnd* pParent); // use in Dialog/FormView
    BOOL Create(const RECT& rect, CWnd* pParent, UINT nID, CCreateContext* pContext = NULL,
                DWORD style = WS_CHILD|WS_BORDER|WS_TABSTOP|WS_VISIBLE);

    static void RegisterClass();
    // Attributes
public:
    int GetRowCount() const;
    int GetColCount() const;

    // Size of the whole Grid incl. hidden parts(in pixels)
    long GetVirtualWidth() const;
    long GetVirtualHeight() const;

    enum MouseMode {
        Nothing,
        SelectingCells,
        SelectingRow,
        SelectingCol,
        SelectingAll,
        SizingColumn,
        SizingRow
    };

    // Operations
public:
    // row/col counts
    void    SetRowCount(int nRows);
    void    SetColCount(int nCols);

    // row/col widths/heights
    int         GetRowHeight(int nRow) const;
    int         GetColWidth(int nCol) const;
    void    SetRowHeight(int nRow, int nHeight);
    void    SetColWidth(int nCol, int nWidth);

    // fixed rows/cols
    int         GetFixedRowCount() const;
    int         GetFixedColCount() const;
    void    SetFixedRowCount(int fixedRows);
    void    SetFixedColCount(int fixedCols);

    int         GetCellBorderThickness() const;

    void    SetEditMode(bool bEnable = true);

    // List mode
    void    SetListMode(bool bEnable = true);
    bool    IsListMode() { return m_bListMode; }

    void    ExpandToFit(int nFromCol = 0, int nFromRow = 0); // expands row heights and col widths to fit dead space

    bool    GetConstrained();
    void    SetConstrained(bool bConstrain = true);

    void    GetFont(LOGFONT& logFont);
    void    SetFont(const LOGFONT& logFont);
    void    SetFont(const char* szFaceName);

    COLORREF    GetTextColor();
    void        SetTextColor(COLORREF clrText);

    void                SetActiveCell(const CGridCellIndices& cell);
    void                SetActiveCell(int nRow, int nCol);
    CGridCellIndices*   GetActiveCell();
    int                 GetActiveRow();
    void                SetActiveRow(int nRow );
    int                 GetOldActiveRow();

    const CGridCellRange&   GetSelectedRange();
    const int               GetSelectedRow();
    void                    SetSelectedRange(const CGridCellRange& range, bool bRepaint = true);
    CGridCellRange          GetVisibleNonFixedCellRange(int* pnBottom = NULL, int* pnRight= NULL);
    CGridCellRange          GetVisibleUnobstructedNonFixedCellRange();


    // Autosizing
    void AutoSize();        // sizes all rows and cols
    void AutoSizeAllCols();
    void AutoSizeAllRows();
    void AutoSizeCol(int nCol);
    void AutoSizeRow(int nRow);
    void AutoSizeByRow(int nRow);

    //  manual sizing support -- added JDA 13 Jun 00
    void SetColSize(int nCol, int nWidth);      // exclusive of border
    void SetMinColSize(int nCol, int nWidth);   // exclusive of border
    int  GetColSize(int nCol);                  // exclusive of border
    void SetBorderThickness(int newThick);

    int  GetCellWidth(int nRow, int nCol);      // exclusive of border

    void SelectRow(int nRow);

    void EnsureVisible(int nRow, int nCol);

    void InvalidateAll();

    void InvalidateCell(int nRow, int nCol);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGridCtrlBase)
    //}}AFX_VIRTUAL


protected:
    virtual void    DrawCell(int nRow, int nCol, CDC* pDC, int x, int y);
    virtual void    OnCellSelected(int nRow, int nCol);
    virtual CSize   GetCellExtent(int nRow, int nCol, CDC* pDC);
    void            PrepareDC(CDC* pDC);
    void            ResetDC(CDC* pDC);
    void            ResetScrollBars();

    // Generated message map functions
protected:
    //{{AFX_MSG(CGridCtrlBase)
    afx_msg void OnPaint();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnEnable(BOOL bEnable);
    //}}AFX_MSG
    afx_msg LRESULT OnGetFont(WPARAM hFont, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

    // Attributes
    protected:
    int GetFixedRowHeight() const;
    int GetFixedColWidth() const;

    int GetColStartX(int nCol);
    int GetRowStartY(int nRow);

    int IsTopLeftCell(const CGridCellIndices& cellIndices) const;
    int IsTopFixedCell(const CGridCellIndices& cellIndices) const;
    int IsLeftFixedCell(const CGridCellIndices& cellIndices) const;
    bool IsFixedCell(const CGridCellIndices& cellIndices) const;


    CGridCellRange GetUnobstructedCellsInRect(const CRect& rect);

    bool IsCellVisible(const CGridCellIndices& cell) const;
    bool IsSelectedCellVisible() const;
    bool IsRowVisible(int nRow) const;
    bool IsSelectedRowVisible() const;

    int m_nDefCellHeight;
    int m_nDefCellWidth;

    int m_nFixedRows;
    int m_nFixedCols;

    int m_nRows;
    int m_nCols;

    int m_nVScrollMax;
    int m_nHScrollMax;

    CUIntArray m_auRowHeights;
    CUIntArray m_auColWidths;

    CGridCellRange m_rangeSelected;

    CGridCellIndices m_cellActive;
    CGridCellIndices m_cellActiveOld;

    MouseMode m_modeMouse;
    CGridCellIndices m_cellLeftClickDown;
    CGridCellIndices m_cellLeftClickUp;
    CGridCellIndices m_cellMouseMove;
    CPoint m_ptLastMouse;

    CFont m_Font;
    CFont* m_pFontOrig;      // font before m_Font selected in
    COLORREF m_clrText;
    COLORREF m_clrTextOrig; // color before m_clrText selected in

    bool m_bConstrained;
    int m_nCellBorderThickness;

    bool m_bListMode;

    // in place editting stuff
    bool m_bEditEnabled;
    bool m_bEditMode;

    CString m_strOldEditData;     // stores old cell data
    CString m_strCurrentEditData; // current edit cell data

private:
    bool m_bDoNotCallResetScrollBars;

protected:
    void EnableEdit(bool bEnableEdit = true); // can only call from base/derived class

    // called during StopEdit to return the editted text
    virtual void GetEditText(int nRow, int nCol, const CString& text);

    // called during StartEdit to set the edittable text
    virtual CString SetEditText(int nRow, int nCol);
    virtual void StartEdit(UINT nFirstChar);
    virtual void StopEdit(bool bRedraw = true);
    virtual void HighlightActiveCell(CDC *pDC,
                                     int nStartX, int nStartY,
                                     int nEndX, int nEndY);

    int PrintPage(CDC* pDC, const CRect& rect, int nStartingRow);

    void DrawDisabledRect(CDC* pDC, const CRect& rc) const;

protected:
    // operations
    CGridCellIndices GetCellIndices(const CPoint& clickPoint);
    void InvalidateCellRect(const CGridCellRange& range);

    CRect GetCellRangeRect(const CGridCellRange& range);
    CGridCellRange GetCellRange();
    void ResetSelectedRange();

private:
    static LONG_PTR WINAPI DummyWndProc(HWND, UINT, WPARAM, LPARAM);
    virtual WNDPROC* GetSuperWndProcAddr();
};

inline int CGridCtrlBase::GetRowCount() const
{
    return m_nRows;
}
inline int CGridCtrlBase::GetColCount() const
{
    return m_nCols;
}
inline int CGridCtrlBase::GetFixedRowCount() const
{
    return m_nFixedRows;
}
inline int CGridCtrlBase::GetFixedColCount() const
{
    return m_nFixedCols;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_GRIDCTRLBASE_H__5980ED45_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

