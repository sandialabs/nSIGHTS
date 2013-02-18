///////////////////////////////////////////////////////////////////////////////////
//
// GridCtrlBase.cpp
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
//              Overall definition/control of sheet.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/ProjectUtil.h>
#include <genClass/U_Str.h>

#include "GridCtrlBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridCtrlBase

BEGIN_MESSAGE_MAP(CGridCtrlBase, CWnd)
//{{AFX_MSG_MAP(CGridCtrlBase)
ON_WM_PAINT()
ON_WM_HSCROLL()
ON_WM_VSCROLL()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONDBLCLK()
ON_WM_RBUTTONDOWN()
ON_WM_SIZE()
ON_WM_KEYDOWN()
ON_WM_CHAR()
ON_WM_GETDLGCODE()
ON_WM_ERASEBKGND()
ON_WM_ENABLE()
//}}AFX_MSG_MAP
ON_MESSAGE(WM_GETFONT, OnGetFont)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGridCtrlBase message handlers

static const int widthFactor = 12; // default number of characters in cell
static const int slop = 3; // pixels around sizing region

CGridCtrlBase::CGridCtrlBase(int nRows, int nCols, int nFixedRows, int nFixedCols) :
    m_nRows(nRows), m_nCols(nCols),
    m_nFixedRows(nFixedRows), m_nFixedCols(nFixedCols),
    m_modeMouse(Nothing),
    m_bConstrained(true),
    m_clrText(0),
    m_nCellBorderThickness(2),
    m_bEditEnabled(false),
    m_bEditMode(false),
    m_bListMode(false),
    m_bDoNotCallResetScrollBars(false)
{
    // Default width and heght
    CWnd* pDesktop = GetDesktopWindow();
    CDC* pDC = pDesktop->GetDC();
    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    m_nDefCellHeight = tm.tmHeight + 2 * m_nCellBorderThickness;
    m_nDefCellWidth = tm.tmAveCharWidth * widthFactor + 2 * m_nCellBorderThickness;
    pDesktop->ReleaseDC(pDC);

    // default font
    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfHeight = tm.tmHeight;
    lf.lfItalic = FALSE; // regular
    lf.lfWeight = FW_NORMAL;
    CopyString(lf.lfFaceName, "MS Sans Serif", 32 ); // kluge on length
    m_Font.CreateFontIndirect( &lf );


    // initialize nRow heights
    m_auRowHeights.SetSize(nRows);
    for (int i = 0; i < nRows; i++) {
        m_auRowHeights[i] = m_nDefCellHeight;
    }

    // initialize nCol widths
    m_auColWidths.SetSize(nCols);
    for (int i = 0; i < nCols; i++) {
        m_auColWidths[i] = m_nDefCellWidth;
    }

    // set selection range
    m_rangeSelected.Set(m_nFixedRows, m_nFixedCols, m_nFixedRows, m_nFixedCols);

    // did not initialize cells, they are already zeroed
}

CGridCtrlBase::~CGridCtrlBase()
{
    DestroyWindow();
}

BOOL CGridCtrlBase::Create(const RECT& rect, CWnd* pParent, UINT nID, CCreateContext* pContext, DWORD dwStyle)
{
    RegisterClass();
    if ( CWnd::Create("GRID", 0, dwStyle, rect, pParent, nID, pContext) ) {
        ResetScrollBars();
        return TRUE;
    } else {
        ASSERT(0);
    }
    return FALSE;
}

BOOL CGridCtrlBase::SubclassDlgItem(UINT nID, CWnd* pParent)
{
    RegisterClass();
    if (!CWnd::SubclassDlgItem(nID, pParent))  {
        return FALSE;
    }
    ResetScrollBars();
    return TRUE;
}

WNDPROC* CGridCtrlBase::GetSuperWndProcAddr()
{
    static WNDPROC NEAR pfnSuper = NULL;
    return &pfnSuper;
}

LONG_PTR WINAPI CGridCtrlBase::DummyWndProc(HWND h, UINT u, WPARAM w, LPARAM l)
{
    return ::DefWindowProc(h, u, w, l);
}

void CGridCtrlBase::RegisterClass()
{
    static bool bRegistered = false;
    if (bRegistered)
        return;

    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));

    wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = DummyWndProc;
    wc.hInstance = AfxGetInstanceHandle();
    wc.hCursor = 0;
    wc.lpszClassName = "GRID";
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);

    VERIFY(::RegisterClass(&wc));
    bRegistered = true;
}

int CGridCtrlBase::GetCellBorderThickness() const
{
    return m_nCellBorderThickness;
}

void CGridCtrlBase::SetActiveCell(const CGridCellIndices& cell)
{
    if (m_rangeSelected.IsValid()) {
        InvalidateCellRect(m_rangeSelected);
    }
    m_rangeSelected.Set(cell, cell);
    InvalidateCellRect(m_rangeSelected);

    m_cellActiveOld = m_cellActive;
    m_cellActive = cell;
}

void CGridCtrlBase::SetActiveCell(int nRow, int nCol)
{
    CGridCellIndices cell(nRow, nCol);
    SetActiveCell(cell);
}

CGridCellIndices* CGridCtrlBase::GetActiveCell()
{
    return &m_cellActive;
}

int CGridCtrlBase::GetActiveRow()
{
    return m_cellActive.m_nRow;
}

void  CGridCtrlBase::SetActiveRow(int nRow)
{
    m_cellActiveOld = m_cellActive;
    m_cellActive.m_nRow = nRow;
}

int CGridCtrlBase::GetOldActiveRow()
{
    return m_cellActiveOld.m_nRow;
}

long CGridCtrlBase::GetVirtualWidth() const
{
    long lVirtualWidth = 0;
    int iColCount = GetColCount();
    for (int i = 0; i < iColCount; i++) {
        lVirtualWidth += m_auColWidths[i];
    }
    return lVirtualWidth;
}

long CGridCtrlBase::GetVirtualHeight() const
{
    long lVirtualHeight = 0;
    int iRowCount = GetRowCount();
    for (int i = 0; i < iRowCount; i++) {
        lVirtualHeight += m_auRowHeights[i];
    }
    return lVirtualHeight;
}

void CGridCtrlBase::SetRowCount(int nRows)
{
    if (nRows == GetRowCount()) {
        return;
    }
    int nAddedRows = nRows - GetRowCount();
    m_nRows = nRows;
    m_auRowHeights.SetSize(nRows);
    int nStartRow = nRows - nAddedRows;
    if ( nAddedRows < 0 ) {
        ResetSelectedRange();
    }
    if ( nAddedRows > 0 ) {
        // initialize nRow heights
        for (int j = nStartRow; j < GetRowCount(); j++) {
            m_auRowHeights[j] = m_nDefCellHeight;
        }
    }

    if (GetSafeHwnd()) {
        ResetScrollBars();
        Invalidate();
    }
}

void CGridCtrlBase::SetColCount(int nCols)
{
    if ( nCols == GetColCount()) {
        return;
    }
    int nAddedCols = nCols - GetColCount();
    m_nCols = nCols;
    m_auColWidths.SetSize(nCols);
    if (nAddedCols > 0) {
        // initialized all added nCols to NULL
        int nStartCol = nCols - nAddedCols;
        for (int i = nStartCol; i < GetColCount(); i++) {
            m_auColWidths[i] = m_nDefCellWidth;
        }
    } else {
        // check for selected cell ranges
        ResetSelectedRange();
    }
    if (GetSafeHwnd()) {
        ResetScrollBars();
        Invalidate();
    }
}

void CGridCtrlBase::SetFixedRowCount(int nFixedRows)
{
    ASSERT((nFixedRows >= 0) && (nFixedRows <= GetRowCount()));
    m_nFixedRows = nFixedRows;
    ResetScrollBars();
}

void CGridCtrlBase::SetFixedColCount(int nFixedCols)
{
    ASSERT((nFixedCols >= 0) && (nFixedCols < GetColCount()));
    m_nFixedCols = nFixedCols;
    ResetScrollBars();
}

int CGridCtrlBase::GetRowHeight(int nRow) const
{
    return m_auRowHeights[nRow];
}

int CGridCtrlBase::GetColWidth(int nCol) const
{
    return m_auColWidths[nCol];
}

int CGridCtrlBase::GetFixedRowHeight() const
{
    int nHeight = 0;
    for (int i = 0; i < m_nFixedRows; i++) {
        nHeight += GetRowHeight(i);
    }
    return nHeight;
}

int CGridCtrlBase::GetFixedColWidth() const
{
    int nWidth = 0;
    for (int i = 0; i < m_nFixedCols; i++) {
        nWidth += GetColWidth(i);
    }
    return nWidth;
}

int CGridCtrlBase::GetColStartX(int nCol)
{
    int xpos = 0;
    if (nCol < m_nFixedCols) { // is a fixed nColumn
        for (int i = 0; i < nCol; i++) {
            xpos += GetColWidth(i);
        }
    } else { // is a scrollable data nColumn
        xpos = GetFixedColWidth();
        for (int i = m_nFixedCols + GetScrollPos(SB_HORZ); i < nCol; i++) {
            xpos += GetColWidth(i);
        }
    }
    return xpos;
}

int CGridCtrlBase::GetRowStartY(int nRow)
{
    int ypos = 0;
    if (nRow < m_nFixedRows) {  // is a fixed nRow
        for (int i = 0; i < nRow; i++) {
            ypos += GetRowHeight(i);
        }
    } else { // is a scrollable data nRow
        ypos = GetFixedRowHeight();
        for (int i = m_nFixedRows + GetScrollPos(SB_VERT); i < nRow; i++) {
            ypos += GetRowHeight(i);
        }
    }
    return ypos;
}

bool CGridCtrlBase::GetConstrained()
{
    return m_bConstrained;
}

void CGridCtrlBase::SetConstrained(bool bConstrain)
{
    m_bConstrained = bConstrain;
}

const CGridCellRange& CGridCtrlBase::GetSelectedRange()
{
    return m_rangeSelected;
}

const int CGridCtrlBase::GetSelectedRow()
{
    return m_rangeSelected.GetTopLeft().m_nRow;
}


void CGridCtrlBase::SetSelectedRange(const CGridCellRange& range, bool bRepaint)
{
    m_rangeSelected = range;
    if (bRepaint) {
        Invalidate();
    }
}

// used by ResetScrollBars()
CGridCellRange CGridCtrlBase::GetUnobstructedCellsInRect(const CRect& rectIn)
{
    CRect rect = rectIn;
    // normalize rect
    rect.OffsetRect(-rect.TopLeft());

    // calc bottom
    int nBottom = 0;
    int i;
    for (i = 0; i < GetRowCount(); i++) {
        nBottom += GetRowHeight(i);
        if (nBottom >= rect.bottom)
            break;
    }
    int nMaxVisibleRow = min(i - 1, GetRowCount() - 1);
    if (nBottom > rect.bottom) {
        nMaxVisibleRow--;
    }

    // calc right
    int nRight = 0;
    for (i = 0; i < GetColCount(); i++) {
        nRight += GetColWidth(i);
        if (nRight >= rect.right)
            break;
    }
    int nMaxVisibleCol = min(i - 1, GetColCount() - 1);
    if (nRight > rect.right) {
        nMaxVisibleCol--;
    }
    return CGridCellRange(0, 0, nMaxVisibleRow, nMaxVisibleCol);
}


void CGridCtrlBase::ResetScrollBars()
{
    if (m_bDoNotCallResetScrollBars) {
        return;
    }

    CRect rect;
    GetWindowRect(&rect);

    // fixes for borders

    HWND hWnd = GetSafeHwnd();
    if (hWnd) {
        LONG style = GetWindowLong(hWnd, GWL_STYLE);
        int cxoffset = 0;
        int cyoffset = 0;
        if (style & WS_THICKFRAME) {
            cxoffset = -::GetSystemMetrics(SM_CXFRAME);
            cyoffset = -::GetSystemMetrics(SM_CYFRAME);
        } else if (style & WS_BORDER) {
            cxoffset = -::GetSystemMetrics(SM_CXBORDER);
            cyoffset = -::GetSystemMetrics(SM_CYBORDER);
        } else {
            TRACE("Grid does not have acceptable border\n");
            //ASSERT(0);
        }
        rect.InflateRect(cxoffset, cyoffset);
    }

    CGridCellRange rangeVisible = GetUnobstructedCellsInRect(rect);

    if (rangeVisible.GetMaxRow() == GetRowCount()-1 && rangeVisible.GetMaxCol() == GetColCount()-1){
        // all cells visible
        m_nVScrollMax = 0;
        m_nHScrollMax = 0;
    } else if (rangeVisible.GetMaxRow() < GetRowCount()-1 && rangeVisible.GetMaxCol() == GetColCount()-1) {
        // nRows obstructed
        m_nVScrollMax = GetRowCount() - m_nFixedRows - 1;
        // check to see it vertical scroll bar obstructs nColumns
        int cx = -::GetSystemMetrics(SM_CXVSCROLL);
        rect.left -= cx;
        rangeVisible = GetUnobstructedCellsInRect(rect);
        if (rangeVisible.GetMaxCol() < GetColCount()-1) {
            m_nHScrollMax = GetColCount() - m_nFixedCols - 1;
        } else {
            m_nHScrollMax = 0;
        }
    } else if ((rangeVisible.GetMaxRow() == GetRowCount()-1) && rangeVisible.GetMaxCol() < GetColCount() - 1) {
        // nColumns obstructed
        m_nHScrollMax = GetColCount() - m_nFixedCols - 1;
        // check to see if horizontal scroll bar obstructs nRows
        int cy = -::GetSystemMetrics(SM_CYHSCROLL);
        rect.top -= cy;
        rangeVisible = GetUnobstructedCellsInRect(rect);
        if (rangeVisible.GetMaxRow() < GetRowCount() - 1) {
            m_nVScrollMax = GetRowCount() - m_nFixedRows - 1;
        } else {
            m_nVScrollMax = 0;
        }
    } else {
        // both nRows and nCols are obstructed
        m_nVScrollMax = GetRowCount() - m_nFixedRows - 1;
        m_nHScrollMax = GetColCount() - m_nFixedCols - 1;
    }
    m_bDoNotCallResetScrollBars = true;
    try {
        // This call
        SetScrollRange(SB_VERT, 0, m_nVScrollMax, FALSE);
        SetScrollRange(SB_HORZ, 0, m_nHScrollMax, FALSE);
    } catch(...) {
        m_bDoNotCallResetScrollBars = false;
        throw;
    }
    m_bDoNotCallResetScrollBars = false;
}

// this function partially visible cells as well
CGridCellRange CGridCtrlBase::GetVisibleNonFixedCellRange(int* pnBottom, int* pnRight)
{
    int nHorzScrollPos = GetScrollPos(SB_HORZ);
    int nVertScrollPos = GetScrollPos(SB_VERT);
    CRect rect;
    GetClientRect(&rect);

    // calc bottom
    int nBottom = 0;
    int nFixedRowHeight = nBottom = GetFixedRowHeight();
    int i;
    for (i = m_nFixedRows + nVertScrollPos; i < GetRowCount(); i++) {
        nBottom += GetRowHeight(i);
        if (nBottom >= rect.bottom)
            break;
    }
    int nMaxVisibleRow = min(i, GetRowCount() - 1);

    // calc right
    int nRight = GetFixedColWidth();
    for (i = m_nFixedCols + nHorzScrollPos; i < GetColCount(); i++) {
        nRight += GetColWidth(i);
        if (nRight >= rect.right)
            break;
    }
    int nMaxVisibleCol = min(i, GetColCount() - 1);
    // set return values if asked
    if (pnBottom != NULL)
        *pnBottom = nBottom;
    if (pnRight != NULL)
        *pnRight = nRight;
    return CGridCellRange(nVertScrollPos + m_nFixedRows,
                          nHorzScrollPos + m_nFixedCols,
                          nMaxVisibleRow,
                          nMaxVisibleCol);
}

// this function partially visible cells as well
CGridCellRange CGridCtrlBase::GetVisibleUnobstructedNonFixedCellRange()
{
    int nHorzScrollPos = GetScrollPos(SB_HORZ);
    int nVertScrollPos = GetScrollPos(SB_VERT);
    CRect rect;
    GetClientRect(&rect);

    // calc bottom
    int nBottom = 0;
    int nFixedRowHeight = nBottom = GetFixedRowHeight();
    int i;
    for (i = m_nFixedRows + nVertScrollPos; i < GetRowCount(); i++) {
        if (nBottom+GetRowHeight(i) >= rect.bottom)
            break;
        nBottom += GetRowHeight(i);
    }
    int nMaxVisibleRow = min(i, GetRowCount() - 1);

    // calc right
    int nRight = GetFixedColWidth();
    for (i = m_nFixedCols + nHorzScrollPos; i < GetColCount(); i++) {
        if (nRight+GetColWidth(i) >= rect.right)
            break;
        nRight += GetColWidth(i);
    }
    int nMaxVisibleCol = min(i, GetColCount() - 1);
    return CGridCellRange(nVertScrollPos + m_nFixedRows,
                          nHorzScrollPos + m_nFixedCols,
                          nMaxVisibleRow,
                          nMaxVisibleCol);
}


void CGridCtrlBase::PrepareDC(CDC* pDC)
{
    m_pFontOrig = pDC->SelectObject(&m_Font);
    m_clrTextOrig = pDC->SetTextColor(m_clrText);
}

void CGridCtrlBase::ResetDC(CDC* pDC)
{
    pDC->SelectObject(m_pFontOrig);
    pDC->SetTextColor(m_clrTextOrig);
}

void CGridCtrlBase::HighlightActiveCell(CDC* pDC, int nStartX, int nStartY, int nEndX, int nEndY)
{
    // Prepare DC
    CPen blackPen(PS_SOLID, 1, RGB(0,0,0));
    CPen* pOldPen = (CPen*)pDC->SelectObject(&blackPen);
    CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(WHITE_BRUSH);
    ASSERT(pOldPen != NULL);
    ASSERT(pOldBrush != NULL);

    // Draw DC
    //VERIFY(pDC->RoundRect(nStartX+1, nStartY+1, nEndX, nEndY, 5, 5));
    VERIFY(pDC->Rectangle(nStartX+1, nStartY+1, nEndX, nEndY));

    // Restore DC
    VERIFY(pDC->SelectObject(pOldBrush) != NULL);
    VERIFY(pDC->SelectObject(pOldPen) != NULL);
}

void CGridCtrlBase::DrawDisabledRect(CDC* pDC, const CRect& rc) const
{
    COLORREF clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
    COLORREF clrBtnHilight = ::GetSysColor(COLOR_BTNHILIGHT);
    COLORREF clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
    // create a monochrome memory DC
    CDC monoDC;
    monoDC.CreateCompatibleDC(NULL);
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&monoDC, rc.Width(), rc.Height());
    CBitmap* pOldBmp = monoDC.SelectObject(&bmp);

    // build a mask
    monoDC.PatBlt(0, 0, rc.Width(), rc.Height(), WHITENESS);
    pDC->SetBkColor(clrBtnFace);
    monoDC.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, rc.left, rc.top, SRCCOPY);
    pDC->SetBkColor(clrBtnHilight);
    monoDC.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, rc.left, rc.top, SRCPAINT);

    // Copy the image from dc into the memory DC
    // and draw it (grayed) back dc.
    pDC->FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), clrBtnFace);
    pDC->SetBkColor(RGB(0, 0, 0));
    pDC->SetTextColor(RGB(255, 255, 255));
    CBrush brHilight;
    brHilight.CreateSolidBrush(clrBtnHilight);
    CBrush brShadow;
    brShadow.CreateSolidBrush(clrBtnShadow);
    CBrush* pOldBrush = pDC->SelectObject(&brHilight);

    pDC->BitBlt(rc.left+1, rc.top+1, rc.Width()-1, rc.Height()-1, &monoDC, 0, 0, 0x00E20746L);
    pDC->SelectObject(&brShadow);
    pDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &monoDC, 0, 0, 0x00E20746L);

    // reset DCs
    pDC->SelectObject(pOldBrush);
    monoDC.SelectObject(pOldBmp);

    bmp.DeleteObject();
    monoDC.DeleteDC();
}

void CGridCtrlBase::OnPaint()
{
    CPaintDC dcPaint(this); // device context for painting

    CDC* pDC = &dcPaint;

    CRect rc;
    GetClientRect(&rc);

    // allocation memory device context
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
    CBitmap* pOldBitmap = dc.SelectObject(&bitmap);

    PrepareDC(&dc);

    dc.FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));

    int nHorzScrollPos = GetScrollPos(SB_HORZ);
    int nVertScrollPos = GetScrollPos(SB_VERT);


    if (m_bListMode) {
        m_rangeSelected.Set(GetActiveRow(), m_nFixedCols,
                            GetActiveRow(), m_nCols - 1 );
    }

    // Create range of visible cells
    int nBottom;
    int nRight;
    CGridCellRange rangeVisible = GetVisibleNonFixedCellRange(&nBottom, &nRight);
    CRect rect = GetCellRangeRect(rangeVisible);
    CBrush brushBackground;
    brushBackground.CreateStockObject(WHITE_BRUSH);
    dc.FillRect(&rect, &brushBackground);

    // brush fill hilighted cells
    CGridCellRange rangeVisibleSelected = m_rangeSelected.Intersect(rangeVisible);
    if (rangeVisibleSelected.IsValid()) {
        CRect rectVsr = GetCellRangeRect(rangeVisibleSelected); // visible selected rect
        CBrush cyanBrush(RGB(0,255,255));
        //dc.FillRect(&rectVsr, &cyanBrush);
    }

    {
        // draw top and left lines
        dc.MoveTo(0, 0);
        dc.LineTo(nRight, 0);
        dc.MoveTo(0, 0);
        dc.LineTo(0, nBottom);

        // draw vertical lines fixed
        int x = 0;
        for (int i = 0; i < m_nFixedCols; i++) {
            x += GetColWidth(i);
            dc.MoveTo(x, 0);
            dc.LineTo(x, nBottom);
        }
        // draw horizontal lines fixed
        int y = 0;
        for (int i = 0; i < m_nFixedRows; i++) {
            y += GetRowHeight(i);
            dc.MoveTo(0, y);
            dc.LineTo(nRight, y);
        }

        int nFixedRowHeight = GetFixedRowHeight();
        int nFixedColWidth = GetFixedColWidth();
        CPen pen;
        pen.CreatePen(PS_SOLID, 0, RGB(192,192,192)); // grey
        CPen* pPen = dc.SelectObject(&pen);

        // draw vertical lines  variable
        for (int i = m_nFixedCols + nHorzScrollPos; i <= rangeVisible.GetMaxCol(); i++) {
            x += GetColWidth(i);
            dc.MoveTo(x, 0);
            dc.SelectObject(pPen);
            dc.LineTo(x, nFixedRowHeight);
            dc.SelectObject(&pen);
            dc.LineTo(x, nBottom);
        }
        // draw horizontal lines  variable
        for (int i = m_nFixedRows + nVertScrollPos; i <= rangeVisible.GetMaxRow(); i++) {
            y += GetRowHeight(i);
            dc.MoveTo(0, y);
            dc.SelectObject(pPen);
            dc.LineTo(nFixedColWidth, y);
            dc.SelectObject(&pen);
            dc.LineTo(nRight, y);
        }
        dc.SelectObject(pPen);
    }

    // draw 3D button highlights
    CPen* pOldPen = (CPen*)dc.SelectStockObject(WHITE_PEN);

    // draw upper-left corner
    int x = 1;
    for (int i= 0; i < m_nFixedCols; i++) {
        int y = 1;
        for (int j = 0; j < m_nFixedRows; j++) {
            dc.MoveTo(x,y);
            dc.LineTo(x + GetColWidth(i) - 2, y);
            dc.MoveTo(x,y);
            dc.LineTo(x, y + GetRowHeight(j) - 2);
            y += GetRowHeight(j);
        }
        x += GetColWidth(i);
    }


    // vertical buttons
    int nFixedRowHeight = GetFixedRowHeight();
    x = 1;
    for (int i = 0; i < m_nFixedCols; i++) {
        int y = nFixedRowHeight + 1;
        for (int j = m_nFixedRows + nVertScrollPos; j <= rangeVisible.GetMaxRow(); j++) {
            dc.MoveTo(x,y);
            dc.LineTo(x + GetColWidth(i) - 2, y);
            dc.MoveTo(x,y);
            dc.LineTo(x, y + GetRowHeight(j) - 2);
            y += GetRowHeight(j);
        }
        x += GetColWidth(i);
    }

    // highlight horizontal buttons
    int nFixedColWidth = GetFixedColWidth();
    int y = 1;
    for (int i = 0; i < m_nFixedRows; i++) {
        x = nFixedColWidth + 1;
        for (int j = m_nFixedCols + nHorzScrollPos; j <= rangeVisible.GetMaxCol(); j++) {
            dc.MoveTo(x,y);
            dc.LineTo(x + GetColWidth(j) - 2, y);
            dc.MoveTo(x,y);
            dc.LineTo(x, y + GetRowHeight(i) - 2);
            x += GetColWidth(j);
        }
        y += GetRowHeight(i);
    }

    if ( !m_bListMode && m_cellActive.IsValid() ) {
        HighlightActiveCell(&dc,
                            GetColStartX(m_cellActive.m_nCol),
                            GetRowStartY(m_cellActive.m_nRow),
                            GetColStartX(m_cellActive.m_nCol+1),
                            GetRowStartY(m_cellActive.m_nRow+1) );
    }

    dc.SelectObject(pOldPen);

    // draw individual cells
    dc.SetBkMode(TRANSPARENT);

    // draw top-left cells (0,0)
    x = m_nCellBorderThickness;
    for (int i = 0; i < m_nFixedCols; i++) {
        y = m_nCellBorderThickness;
        for (int j = 0; j < m_nFixedRows; j++) {
            DrawCell(j, i, &dc, x, y);
            y += GetRowHeight(j);
        }
        x += GetColWidth(i);
    }

    // draw fixed nColumn cells  1..n
    x = m_nCellBorderThickness;
    for (int i = 0; i < m_nFixedCols; i++) {
        y = GetFixedRowHeight() + m_nCellBorderThickness;
        for (int j = nVertScrollPos + m_nFixedRows; j <= rangeVisible.GetMaxRow(); j++) {
            DrawCell(j,i, &dc, x, y);
            y += GetRowHeight(j);
        }
        x += GetColWidth(i);
    }

    // draw fixed nRow cells  1..n
    //x = GetColWidth(0) + m_nCellBorderThickness;
    x = GetFixedColWidth() + m_nCellBorderThickness;
    for (int i = nHorzScrollPos + m_nFixedCols; i <= rangeVisible.GetMaxCol(); i++) {
        y = m_nCellBorderThickness;
        for (int j = 0; j < m_nFixedRows; j++) {
            DrawCell(j, i, &dc, x, y);
            y += GetRowHeight(j);
        }
        x += GetColWidth(i);
    }

    // draw rest of non-fixed cells
    x = nFixedColWidth + m_nCellBorderThickness;
    for (int i = nHorzScrollPos + m_nFixedCols; i <= rangeVisible.GetMaxCol(); i++) {
        y = nFixedRowHeight + m_nCellBorderThickness;
        for (int j = nVertScrollPos + m_nFixedRows; j <= rangeVisible.GetMaxRow(); j++) {
            DrawCell(j, i, &dc, x, y);
            y += GetRowHeight(j);
        }
        x += GetColWidth(i);
    }


    if (!IsWindowEnabled())
        DrawDisabledRect(&dc, rc);

    pDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dc, 0, 0, SRCCOPY);

    dc.SelectObject(pOldBitmap);
    ResetDC(&dc);
}

void CGridCtrlBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
    if (GetFocus()->GetSafeHwnd() != GetSafeHwnd()) {
        SetFocus();        // Auto-destroy any InPlaceEdit's
    }

    int nScrollPos = GetScrollPos(SB_HORZ);
    int nScrollMin, nScrollMax;
    GetScrollRange(SB_HORZ, &nScrollMin, &nScrollMax);
    switch (nSBCode) {
    case SB_LINERIGHT:
        if (nScrollPos != m_nHScrollMax) {
            SetScrollPos(SB_HORZ, nScrollPos + 1);
            int xScroll = -GetColWidth(nScrollPos + m_nFixedCols);
            CRect rect;
            GetClientRect(&rect);
            rect.left = GetFixedColWidth() - xScroll;
            ScrollWindow(xScroll, 0, &rect);
            rect.left = rect.right + xScroll;
            InvalidateRect(&rect);
        }
        break;
    case SB_PAGERIGHT:
        if (nScrollPos != m_nHScrollMax) {
            CRect rect;
            GetClientRect(&rect);
            CGridCellRange rangeVisible = GetVisibleNonFixedCellRange();
            int offset = rangeVisible.GetMaxCol() - rangeVisible.GetMinCol();
            SetScrollPos(SB_HORZ, min(m_nHScrollMax, nScrollPos + offset));
            rect.left = GetFixedColWidth();
            InvalidateRect(&rect);
        }
        break;
    case SB_LINELEFT:
        if (nScrollPos != 0) {
            SetScrollPos(SB_HORZ, nScrollPos - 1);
            int xScroll = GetColWidth(nScrollPos - 1 + m_nFixedCols);
            CRect rect;
            GetClientRect(&rect);
            rect.left = GetFixedColWidth()+1;
            ScrollWindow(xScroll, 0, &rect);
            UpdateWindow();
        }
        break;
    case SB_PAGELEFT:
        if (nScrollPos != 0) {
            CRect rect;
            GetClientRect(&rect);
            int nScrollableWidth = rect.right - GetFixedColWidth();
            CGridCellRange rangeVisible = GetVisibleNonFixedCellRange();
            int i, j;
            for (i = nScrollPos, j = rangeVisible.GetMinCol() - 1; i >= 0 && j >= 0; i--, j--) {
                nScrollableWidth -= GetColWidth(j);
                if (nScrollableWidth <= 0)
                    break;
            }
            SetScrollPos(SB_HORZ, max(0, i));
            rect.left = GetFixedColWidth();
            InvalidateRect(&rect);
        }
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        {
            CRect rect;
            GetClientRect(&rect);
            SetScrollPos(SB_HORZ, nPos);
            rect.left = GetFixedColWidth();
            InvalidateRect(&rect);
        }
        break;
    case SB_TOP:
        if (nScrollPos != 0) {
            SetScrollPos( SB_HORZ, 0 );
            Invalidate();
        }
        break;
    case SB_BOTTOM:
        if (nScrollPos != nScrollMax) {
            SetScrollPos( SB_HORZ, nScrollMax );
            Invalidate();
        }
    default:
        {
            TRACE("Horizontal scroll case not handled\n");
        }
    }
}

void CGridCtrlBase::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
    if (GetFocus()->GetSafeHwnd() != GetSafeHwnd()) {
        SetFocus();        // Auto-destroy any InPlaceEdit's
    }

    int nScrollPos = GetScrollPos(SB_VERT);
    int nScrollMin, nScrollMax;
    GetScrollRange(SB_VERT, &nScrollMin, &nScrollMax);
    switch (nSBCode) {
    case SB_LINEDOWN:
        if (nScrollPos != m_nVScrollMax) {
            SetScrollPos(SB_VERT, nScrollPos + 1);
            int yScroll = -GetRowHeight(nScrollPos + m_nFixedRows);
            CRect rect;
            GetClientRect(&rect);

            rect.top = GetFixedRowHeight() + GetRowHeight(nScrollPos + m_nFixedRows);
            ScrollWindow(0, yScroll, &rect);
            rect.top = rect.bottom + yScroll;
            InvalidateRect(&rect);
        }
        break;
    case SB_PAGEDOWN:
        if (nScrollPos != m_nVScrollMax) {
            CRect rect;
            GetClientRect(&rect);
            CGridCellRange rangeVisible = GetVisibleNonFixedCellRange();
            int offset = rangeVisible.GetMaxRow() - rangeVisible.GetMinRow();
            if (offset == 0) {
                offset++;
            }
            nScrollPos += offset;
            if (nScrollPos > m_nVScrollMax) {
                nScrollPos = m_nVScrollMax;
            }
            SetScrollPos(SB_VERT, nScrollPos);
            rect.top = GetFixedRowHeight();
            InvalidateRect(&rect);
        }
        break;
    case SB_LINEUP:
        if (nScrollPos != 0) {
            SetScrollPos(SB_VERT, nScrollPos - 1);
            int yScroll = GetRowHeight(nScrollPos - 1 + m_nFixedRows);
            CRect rect;
            GetClientRect(&rect);
            rect.top = GetFixedRowHeight()+1;
            ScrollWindow(0, yScroll, &rect);
            UpdateWindow();
        }
        break;
    case SB_PAGEUP:
        if (nScrollPos != 0) {
            CRect rect;
            GetClientRect(&rect);
            int nScrollableHeight = rect.bottom - GetFixedRowHeight();
            CGridCellRange rangeVisible = GetVisibleNonFixedCellRange();
            int i, j;
            for (i = nScrollPos, j = rangeVisible.GetMinRow() - 1; i >= 0 && j >= 0; i--, j--) {
                nScrollableHeight -= GetRowHeight(j);
                if (nScrollableHeight <= 0)
                    break;
            }
            SetScrollPos(SB_VERT, max(0, i));
            rect.top = GetFixedRowHeight();
            InvalidateRect(&rect);
        }
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        {
            CRect rect;
            GetClientRect(&rect);
            SetScrollPos(SB_VERT, nPos);
            rect.top = GetFixedRowHeight();
            InvalidateRect(&rect);
        }
        break;
    case SB_TOP:
        if (nScrollPos != 0) {
            SetScrollPos(SB_VERT, 0);
            Invalidate();
        }
        break;
    case SB_BOTTOM:
        if (nScrollPos != nScrollMax) {
            SetScrollPos(SB_VERT, nScrollMax);
            Invalidate();
        }
    default:
        {
            TRACE("Vertical scroll case not handled\n");
        }
    }
}

void CGridCtrlBase::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    ResetScrollBars();
}

CGridCellIndices CGridCtrlBase::GetCellIndices(const CPoint& clickPoint)
{
    CGridCellIndices cellIndices; // return value

    // calculate nColumn index
    int nFixedColWidth = GetFixedColWidth();
    if (clickPoint.x < 0) { // not in window
        cellIndices.m_nCol = -1;
    } else if (clickPoint.x < nFixedColWidth) { // in fixed nCol
        int xpos = 0;
        int nCol;
        for (nCol = 0; nCol < m_nFixedCols; nCol++) {
            xpos += GetColWidth(nCol);
            if (xpos > clickPoint.x)
                break;
        }
        cellIndices.m_nCol = nCol;
    } else {
        int xpos = nFixedColWidth;
        int nHorzScrollPos = GetScrollPos(SB_HORZ);
        int nCol;
        for (nCol = m_nFixedCols + nHorzScrollPos; nCol < GetColCount(); nCol++) {
            xpos += GetColWidth(nCol);
            if (xpos > clickPoint.x)
                break;
        }
        if (nCol >= GetColCount()) {
            cellIndices.m_nCol = -1;
        } else {
            cellIndices.m_nCol = nCol;
        }
    }

    // calculate nRow index
    int nFixedRowHeight = GetFixedRowHeight();
    if (clickPoint.y < 0) { // not in window
        cellIndices.m_nRow = -1;
    } else if (clickPoint.y < nFixedRowHeight) { // in fixed nCol
        int ypos = 0;
        int nRow;
        for (nRow = 0; nRow < m_nFixedRows; nRow++) {
            ypos += GetRowHeight(nRow);
            if (ypos > clickPoint.y)
                break;
        }
        cellIndices.m_nRow = nRow;
    } else {
        int ypos = nFixedRowHeight;
        int nVertScrollPos = GetScrollPos(SB_VERT);
        int nRow;
        for (nRow = m_nFixedRows + nVertScrollPos; nRow < GetRowCount(); nRow++) {
            ypos += GetRowHeight(nRow);
            if (ypos > clickPoint.y)
                break;
        }
        if (nRow >= GetRowCount()) {
            cellIndices.m_nRow = -1;
        } else {
            cellIndices.m_nRow = nRow;
        }
    }
    return cellIndices;
}

void CGridCtrlBase::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
    CGridCellIndices cellCurrent = GetCellIndices(point);
    if (m_modeMouse == Nothing) {
        if (cellCurrent.m_nRow < m_nFixedRows && cellCurrent.IsValid()) {
            int nStartX = GetColStartX(cellCurrent.m_nCol);
            int nEndX = nStartX + GetColWidth(cellCurrent.m_nCol);
            if ((point.x - nStartX <= slop && cellCurrent.m_nCol != 0) || nEndX - point.x <= slop) {
                HCURSOR cursor = ::LoadCursor(0,IDC_SIZEWE);
                SetCursor(cursor);
            } else {
                SetCursor(::LoadCursor(0, IDC_ARROW));
            }
        } else if (cellCurrent.m_nCol < m_nFixedCols && cellCurrent.IsValid()) {
            int nStartY = GetRowStartY(cellCurrent.m_nRow);
            int nEndY = nStartY + GetRowHeight(cellCurrent.m_nRow);
            if ((point.y - nStartY <= slop && cellCurrent.m_nRow != 0) || nEndY - point.y <= slop) {
                SetCursor(::LoadCursor(0, IDC_SIZENS));
            } else {
                SetCursor(::LoadCursor(0, IDC_ARROW));
            }
        } else {
            SetCursor(::LoadCursor(0, IDC_ARROW));
        }
        m_ptLastMouse = point;
        return;
    }
    if (!m_cellLeftClickDown.IsValid()) {
        m_ptLastMouse = point;
        return;
    }
    switch(m_modeMouse) {
    case SelectingAll:
        break;
    case SelectingCol:
        {
            int nCol = cellCurrent.m_nCol;
            if (nCol == m_cellMouseMove.m_nCol)
                break;
            if (nCol < m_nFixedCols)
                break;
            InvalidateCellRect(m_rangeSelected);
            if (nCol > m_cellLeftClickDown.m_nCol) {
                m_rangeSelected.SetMinCol(m_cellLeftClickDown.m_nCol);
                m_rangeSelected.SetMaxCol(nCol);
            } else {
                m_rangeSelected.SetMinCol(nCol);
                m_rangeSelected.SetMaxCol(m_cellLeftClickDown.m_nCol);
            }
            InvalidateCellRect(m_rangeSelected);
        }
        break;
    case SelectingRow:
        {
            int nRow = cellCurrent.m_nRow;
            if (nRow == m_cellMouseMove.m_nRow)
                break;
            if (nRow < m_nFixedRows)
                break;
            InvalidateCellRect(m_rangeSelected);
            if (nRow > m_cellLeftClickDown.m_nRow) {
                m_rangeSelected.SetMinRow(m_cellLeftClickDown.m_nRow);
                m_rangeSelected.SetMaxRow(nRow);
            } else {
                m_rangeSelected.SetMinRow(nRow);
                m_rangeSelected.SetMaxRow(m_cellLeftClickDown.m_nRow);
            }
            InvalidateCellRect(m_rangeSelected);
        }
        break;
    case SelectingCells:
        {
            int nRow = cellCurrent.m_nRow;
            int nCol = cellCurrent.m_nCol;
            if (nRow < m_nFixedRows || nCol < m_nFixedCols)
                break;
            if (cellCurrent == m_cellMouseMove)
                return;
            InvalidateCellRect(m_rangeSelected);
            if (cellCurrent.IsDownRightOf(m_cellLeftClickDown)) {
                m_rangeSelected.SetMinRow(m_cellLeftClickDown.m_nRow);
                m_rangeSelected.SetMaxRow(nRow);
                m_rangeSelected.SetMinCol(m_cellLeftClickDown.m_nCol);
                m_rangeSelected.SetMaxCol(nCol);
            } else if (cellCurrent.IsDownLeftOf(m_cellLeftClickDown)) {
                m_rangeSelected.SetMinRow(m_cellLeftClickDown.m_nRow);
                m_rangeSelected.SetMaxRow(nRow);
                m_rangeSelected.SetMinCol(nCol);
                m_rangeSelected.SetMaxCol(m_cellLeftClickDown.m_nCol);
            } else if (cellCurrent.IsUpRightOf(m_cellLeftClickDown)) {
                m_rangeSelected.SetMinRow(nRow);
                m_rangeSelected.SetMaxRow(m_cellLeftClickDown.m_nRow);
                m_rangeSelected.SetMinCol(m_cellLeftClickDown.m_nCol);
                m_rangeSelected.SetMaxCol(nCol);
            } else if (cellCurrent.IsUpLeftOf(m_cellLeftClickDown)) {
                m_rangeSelected.SetMinRow(nRow);
                m_rangeSelected.SetMaxRow(m_cellLeftClickDown.m_nRow);
                m_rangeSelected.SetMinCol(nCol);
                m_rangeSelected.SetMaxCol(m_cellLeftClickDown.m_nCol);
            }
            InvalidateCellRect(m_rangeSelected);
        }
        break;
    case SizingColumn:
        {
            CRect rect;
            GetClientRect(&rect);
            CDC* pDC = GetDC();
            CRect oldInvertedRect(m_ptLastMouse.x, rect.top, m_ptLastMouse.x + 2, rect.bottom);
            pDC->InvertRect(&oldInvertedRect);
            CRect newInvertedRect(point.x, rect.top, point.x + 2, rect.bottom);
            pDC->InvertRect(&newInvertedRect);
            ReleaseDC(pDC);
        }
        break;
    case SizingRow:
        {
            CRect rect;
            GetClientRect(&rect);
            CDC* pDC = GetDC();
            CRect oldInvertedRect(rect.left, m_ptLastMouse.y, rect.right, m_ptLastMouse.y + 2);
            pDC->InvertRect(&oldInvertedRect);
            CRect newInvertedRect(rect.left, point.y, rect.right, point.y + 2);
            pDC->InvertRect(&newInvertedRect);
            ReleaseDC(pDC);
        }
        break;
    }
    m_cellMouseMove = cellCurrent;
    m_ptLastMouse = point;
}

void CGridCtrlBase::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetFocus(); // Auto-destroy any InPlaceEdit's

    if (m_bEditMode) {
        CWnd::OnLButtonDown(nFlags, point);
        return;
    }

    SetCapture();
    StopEdit(/* bRedraw = */ false);
    m_cellLeftClickDown = GetCellIndices(point);
    if (GetCursor() == LoadCursor(0, IDC_SIZEWE)) { // sizing nColumn
        m_modeMouse = SizingColumn;
        int nStartX = GetColStartX(m_cellLeftClickDown.m_nCol);
        CRect rect;
        GetClientRect(&rect);
        CRect invertedRect(point.x, rect.top, point.x + 2, rect.bottom);
        CDC* pDC = GetDC();
        pDC->InvertRect(&invertedRect);
        ReleaseDC(pDC);
        if (point.x - nStartX <= slop) { // clicked right of border
            rect.left = GetColStartX(--m_cellLeftClickDown.m_nCol);
        } else {
            rect.left = nStartX;
        }
        ClientToScreen(&rect);
        ::ClipCursor(&rect);
    } else if (GetCursor() == ::LoadCursor(0, IDC_SIZENS)) { // sizing nRow
        m_modeMouse = SizingRow;
        int nStartY = GetRowStartY(m_cellLeftClickDown.m_nRow);
        CRect rect;
        GetClientRect(&rect);
        CRect invertedRect(rect.left, point.y, rect.right, point.y + 2);
        CDC* pDC = GetDC();
        pDC->InvertRect(&invertedRect);
        ReleaseDC(pDC);
        if (point.y - nStartY <= slop) { // clicked below border
            rect.top = GetRowStartY(--m_cellLeftClickDown.m_nRow);
        } else {
            rect.top = nStartY;
        }
        ClientToScreen(&rect);
        ClipCursor(&rect);
    } else { // not sizing
        if (!m_cellLeftClickDown.IsValid())
            return;

        if (m_rangeSelected.IsValid()) {
            InvalidateCellRect(m_rangeSelected);
        }
        if (IsTopLeftCell(m_cellLeftClickDown) && m_nFixedRows && m_nFixedCols) {
            m_rangeSelected.Set(m_nFixedRows, m_nFixedCols, GetRowCount() - 1, GetColCount() - 1);
            m_modeMouse = SelectingAll;
        } else if (IsTopFixedCell(m_cellLeftClickDown)) {
            m_rangeSelected.Set(m_nFixedRows, m_cellLeftClickDown.m_nCol, GetRowCount() - 1, m_cellLeftClickDown.m_nCol);
            m_modeMouse = SelectingCol;

            InvalidateCellRect(CGridCellRange(m_cellActive, m_cellActive));
            m_cellActive.m_nRow = m_nFixedRows;
            m_cellActive.m_nCol = m_cellLeftClickDown.m_nCol;
        } else if (IsLeftFixedCell(m_cellLeftClickDown)) {
            m_rangeSelected.Set(m_cellLeftClickDown.m_nRow, m_nFixedCols, m_cellLeftClickDown.m_nRow, GetColCount() - 1);
            m_modeMouse = SelectingRow;
        } else {
            m_modeMouse = SelectingCells;

            m_cellActive = m_cellLeftClickDown;

            //InvalidateCellRect(m_rangeSelected);
            if (m_bListMode) {
                m_rangeSelected.Set(m_cellLeftClickDown.m_nRow, m_nFixedCols,
                                    m_cellLeftClickDown.m_nRow, m_nCols - 1);
                m_modeMouse = SelectingRow;
            } else {
                m_rangeSelected = CGridCellRange(m_cellLeftClickDown, m_cellLeftClickDown);
            }
            //InvalidateCellRect(m_rangeSelected);

        }
        InvalidateCellRect(m_rangeSelected);
    }
    m_ptLastMouse = point;
}

void CGridCtrlBase::OnLButtonUp(UINT nFlags, CPoint point)
{
    CWnd::OnLButtonUp(nFlags, point);

    ReleaseCapture();
    ::ClipCursor(0);
    if (m_modeMouse == SizingColumn) {
        CRect rect;
        GetClientRect(&rect);
        CRect invertedRect(m_ptLastMouse.x, rect.top, m_ptLastMouse.x + 2, rect.bottom);
        CDC* pDC = GetDC();
        pDC->InvertRect(&invertedRect);
        ReleaseDC(pDC);
        int nStartX = GetColStartX(m_cellLeftClickDown.m_nCol);
        SetColWidth(m_cellLeftClickDown.m_nCol, point.x - nStartX);
        ResetScrollBars();
        Invalidate();
    } else if (m_modeMouse == SizingRow) {
        CRect rect;
        GetClientRect(&rect);
        CRect invertedRect(rect.left, m_ptLastMouse.y, rect.right, m_ptLastMouse.y + 2);
        CDC* pDC = GetDC();
        pDC->InvertRect(&invertedRect);
        ReleaseDC(pDC);
        int nStartY = GetRowStartY(m_cellLeftClickDown.m_nRow);
        SetRowHeight(m_cellLeftClickDown.m_nRow, point.y - nStartY);
        ResetScrollBars();
        Invalidate();
    } else if (m_modeMouse = SelectingCells) {
        m_cellLeftClickUp = GetCellIndices(point);
        if (m_cellLeftClickDown.IsValid() && m_cellLeftClickDown == m_cellLeftClickUp) {
            OnCellSelected(m_cellLeftClickDown.m_nRow, m_cellLeftClickDown.m_nCol);
        }
    }
    m_modeMouse = Nothing;
    if (!m_cellLeftClickDown.IsValid()) {
        return;
    }
    m_cellLeftClickUp = GetCellIndices(point);

    GetParent()->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), BN_CLICKED), (LPARAM) m_hWnd);
}

void CGridCtrlBase::OnRButtonDown(UINT nFlags, CPoint point)
{
    GetParent()->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), BN_CLICKED), (LPARAM) m_hWnd);
}

void CGridCtrlBase::OnLButtonDblClk(UINT /*nFlags*/, CPoint point)
{
    CGridCellIndices cell = GetCellIndices(point);
    if (!cell.IsValid()) {
        return;
    }
    if (!IsFixedCell(cell)) {
        StartEdit(VK_LBUTTON);
        return;
    }
    if (cell.m_nRow < m_nFixedRows) { // header
        if (GetCursor() != ::LoadCursor(0, IDC_SIZEWE)) { // sizing column
            CPoint ptTest = point;
            ptTest.x -= 2*slop;
            CGridCellIndices cell = GetCellIndices(ptTest);

            if (cell.m_nCol >= 0)
                {
                    AutoSizeCol(cell.m_nCol);
                    SetColSize(cell.m_nCol, GetColSize(cell.m_nCol) + 10);
                    ResetScrollBars();
                    Invalidate();
                }
            return;
        }
    }
}

CRect CGridCtrlBase::GetCellRangeRect(const CGridCellRange& range)
{
    int left = GetColStartX(range.GetMinCol());
    int top = GetRowStartY(range.GetMinRow());
    int right = GetColStartX(range.GetMaxCol()) + GetColWidth(range.GetMaxCol());
    int bottom = GetRowStartY(range.GetMaxRow()) + GetRowHeight(range.GetMaxRow());
    return CRect(left, top, right, bottom);
}

void CGridCtrlBase::InvalidateCellRect(const CGridCellRange& range)
{
    if (!range.IsValid()) {
        return;
    }
    CGridCellRange rangeVisible = GetVisibleNonFixedCellRange().Intersect(range);
    CRect rectVisible = GetCellRangeRect(rangeVisible);
    InvalidateRect(&rectVisible);
}

void CGridCtrlBase::InvalidateCell(int nRow, int nCol)
{
    int left    = GetColStartX(nCol);
    int top         = GetRowStartY(nRow);
    int right   = left + GetColWidth(nCol);
    int bottom  = top + GetRowHeight(nRow);
    InvalidateRect(&CRect(left, top, right, bottom));
}

int CGridCtrlBase::IsTopLeftCell(const CGridCellIndices& cellIndices) const
{
    return cellIndices.m_nRow == 0 && cellIndices.m_nCol == 0;
}

int CGridCtrlBase::IsTopFixedCell(const CGridCellIndices& cellIndices) const
{
    return cellIndices.IsValid()  && cellIndices.m_nRow < m_nFixedRows;
}

int CGridCtrlBase::IsLeftFixedCell(const CGridCellIndices& cellIndices) const
{
    return cellIndices.IsValid() && cellIndices.m_nCol < m_nFixedCols;
}

bool CGridCtrlBase::IsFixedCell(const CGridCellIndices& cellIndices) const
{
    return cellIndices.IsValid()
        && (cellIndices.m_nRow < m_nFixedRows || cellIndices.m_nCol < m_nFixedCols);
}

void CGridCtrlBase::AutoSizeByRow( int nRow )
{
    CDC* pDC = GetDC();
    PrepareDC(pDC);

    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);

    int nHeight = 0;
    for ( int i = 0; i < GetColCount(); i++ ) {
        int nWidth = 0;
        CSize size = GetCellExtent( nRow, i, pDC );
        if ( size.cx > nWidth ) {
            nWidth = size.cx;
        }
        if ( (nWidth + 2 * m_nCellBorderThickness) > m_nDefCellWidth ) {
            m_auColWidths[i] = nWidth + 2 * m_nCellBorderThickness;
        }
        if ( size.cy > nHeight ) {
            nHeight = size.cy;
        }
    }

    m_nDefCellHeight =  nHeight + 2 * m_nCellBorderThickness;
    for ( int j = 0; j < GetRowCount(); j++ ) {
        m_auRowHeights[j] = m_nDefCellHeight;
    }

    ResetDC(pDC);
    ReleaseDC(pDC);
    ResetScrollBars();
}

void CGridCtrlBase::AutoSizeCol(int nCol)
{
    CDC* pDC = GetDC();
    PrepareDC(pDC);
    int nWidth = 0;
    CSize size;
    for (int i = 0; i < GetRowCount(); i++) {
        size = GetCellExtent(i, nCol, pDC);
        if (size.cx > nWidth) {
            nWidth = size.cx;
        }
    }
    m_auColWidths[nCol] = nWidth + 2 * m_nCellBorderThickness;
    ResetDC(pDC);
    ReleaseDC(pDC);
    ResetScrollBars();
}

void CGridCtrlBase::SetColSize(int nCol, int nWidth)
{
    m_auColWidths[nCol] = nWidth + 2 * m_nCellBorderThickness;
}

void CGridCtrlBase::SetMinColSize(int nCol, int nWidth)
{
    if (GetColSize(nCol) < nWidth)
        {
            SetColSize(nCol, nWidth);
        }
}

int  CGridCtrlBase::GetColSize(int nCol)
{
    return m_auColWidths[nCol] - 2 * m_nCellBorderThickness;
}

void CGridCtrlBase::SetBorderThickness(int newThick)
{
    m_nCellBorderThickness = newThick;
}

void CGridCtrlBase::AutoSizeRow(int nRow)
{
    CDC* pDC = GetDC();
    PrepareDC(pDC);
    int nHeight = 0;
    CSize size;
    for (int i = 0; i < GetColCount(); i++) {
        size = GetCellExtent(nRow, i, pDC);
        if (size.cy > nHeight) {
            nHeight = size.cy;
        }
    }
    m_auRowHeights[nRow] = nHeight + 2 * m_nCellBorderThickness;
    ResetDC(pDC);
    ReleaseDC(pDC);
    ResetScrollBars();
}

void CGridCtrlBase::AutoSizeAllCols()
{
    for (int nCol = 0; nCol < GetColCount(); nCol++) {
        AutoSizeCol(nCol);
    }
}

void CGridCtrlBase::AutoSizeAllRows()
{
    for (int nRow = 0; nRow < GetRowCount(); nRow++) {
        AutoSizeRow(nRow);
    }
}

// sizes all nRows and nColumns
// faster than calling both AutoSizeAllCols() and AutoSizeAllRows()
void CGridCtrlBase::AutoSize()
{
    CDC* pDC = GetDC();
    PrepareDC(pDC);
    int nColCount = GetColCount();
    int nRowCount = GetRowCount();

    // initialize nCol nWidths to zero
    for (int i = 0; i < nColCount; i++) {
        m_auColWidths[i] = 0;
    }

    // initialize nRow heights to zero
    for (int j = 0; j < nRowCount; j++) {
        m_auRowHeights[j] = 0;
    }

    CSize size;
    for (int i = 0; i < nColCount; i++) {
        for (int j = 0; j < nRowCount; j++) {
            size = GetCellExtent(j, i, pDC);
            if (size.cx > (int) m_auColWidths[i]) {
                m_auColWidths[i] = size.cx;
            }
            if (size.cy > (int) m_auRowHeights[j]) {
                m_auRowHeights[j] = size.cy;
            }
        }
    }

    for (int i = 0; i < nColCount; i++) {
        m_auColWidths[i] += 2 * m_nCellBorderThickness;
    }

    for (int j = 0; j< nRowCount; j++) {
        m_auRowHeights[j] += 2 * m_nCellBorderThickness;
    }
    ResetDC(pDC);
    ReleaseDC(pDC);
    ResetScrollBars();
    if (GetSafeHwnd()) {
        Invalidate();
    }
}

// fake draw
void CGridCtrlBase::DrawCell(int nRow, int nCol, CDC* pDC, int x, int y)
{
    CString sText;
    sText.Format( "%d-%d", nRow, nCol);
    if (m_bConstrained) {
        CRect rect(x, y, x + m_auColWidths[nCol] - 2 * m_nCellBorderThickness,
                   y + m_auRowHeights[nRow] - 2 * m_nCellBorderThickness);
        pDC->DrawText(sText, -1, &rect, DT_CENTER);
    } else {
        pDC->TextOut(x, y, sText, sText.GetLength());
    }
}

void CGridCtrlBase::OnCellSelected(int nRow, int nCol)
{
    // default implemetation does nothing
}

CGridCellRange CGridCtrlBase::GetCellRange()
{
    return CGridCellRange(0, 0, GetRowCount() - 1, GetColCount() - 1);
}

void CGridCtrlBase::ResetSelectedRange()
{
    m_rangeSelected = m_rangeSelected.Intersect(GetCellRange());
    if (!m_rangeSelected.IsValid()) {
        m_rangeSelected = CGridCellRange(m_nFixedRows, m_nFixedCols, m_nFixedRows, m_nFixedCols);
    }
}

void CGridCtrlBase::GetFont(LOGFONT& logFont)
{
    m_Font.GetObject(sizeof(LOGFONT), &logFont);
}

void CGridCtrlBase::SetFont(const LOGFONT& logFont)
{
    m_Font.DeleteObject();
    m_Font.CreateFontIndirect(&logFont);
}

void CGridCtrlBase::SetFont(const char* facename)
{
    LOGFONT lf;
    GetFont(lf);
    CopyString(lf.lfFaceName, facename, 32); // kluge on length
    SetFont(lf);
}

COLORREF CGridCtrlBase::GetTextColor()
{
    return m_clrText;
}

void CGridCtrlBase::SetTextColor(COLORREF textColor)
{
    m_clrText = textColor;
}

void CGridCtrlBase::ExpandToFit(int nFromCol, int nFromRow)
{
    CRect rect;
    GetClientRect(&rect);
    long nVirtualWidth = GetVirtualWidth();
    if (nVirtualWidth < rect.Width()) {
        int nDifference = rect.Width() - int(nVirtualWidth) -1  ; // -1 for padding
        int nAdditionalColWidth = nDifference / (GetColCount() - nFromCol);
        for (int i = nFromCol; i < GetColCount(); i++) {
            m_auColWidths[i] += nAdditionalColWidth;
        }
        int nLeftOver = nDifference % GetColCount();
        for (int i = nFromCol; i < nLeftOver; i++) {
            m_auColWidths[i] += 1;
        }
    }
    if (nFromRow != -1) {
        long nVirtualHeight = GetVirtualHeight();
        if (nVirtualHeight < rect.Height()) {
            int nDifference = rect.Height() - int(nVirtualHeight);
            int nAdditionalRowHeight = nDifference / GetRowCount();
            for (int i = 0; i < GetRowCount(); i++) {
                m_auRowHeights[i] += nAdditionalRowHeight;
            }
            int nLeftOver = nDifference % GetRowCount();
            for (int i = 0; i < nLeftOver; i++) {
                m_auRowHeights[i] += 1;
            }
        }
    }
    if (IsWindowVisible()) {
        Invalidate();
    }
}

UINT CGridCtrlBase::OnGetDlgCode()
{
    return DLGC_WANTARROWS|DLGC_WANTCHARS;
}

void CGridCtrlBase::SelectRow(int nRow)
{
    ASSERT( nRow < m_nRows );
    m_rangeSelected.Set(nRow, m_nFixedCols,
                        nRow, m_nCols - 1);
    CGridCellIndices topLeft(m_rangeSelected.GetTopLeft());
    CGridCellIndices cellNext(topLeft);
    if ( GetSafeHwnd() ) {
        if ( !IsCellVisible( cellNext ) ) {
            SetScrollPos( SB_VERT, cellNext.m_nRow - m_nFixedRows - 1 );
        }
        Invalidate();
    }
}

void CGridCtrlBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    int nScrollMin, nScrollMax;
    CGridCellIndices topLeft(m_rangeSelected.GetTopLeft());
    CGridCellIndices cellNext(m_cellActive);

    m_cellActiveOld = m_cellActive;
    switch ( nChar ) {
    case VK_DOWN:
        if (cellNext.m_nRow < GetRowCount() - 1) {
            cellNext.m_nRow += 1;
        }
        StopEdit(false);
        break;
    case VK_UP:
        if (cellNext.m_nRow > m_nFixedRows) {
            cellNext.m_nRow -= 1;
        }
        StopEdit(false);
        break;
    case VK_RIGHT:
        if (cellNext.m_nCol < GetColCount() - 1) {
            cellNext.m_nCol += 1;
        }
        StopEdit(false);
        break;
    case VK_LEFT:
        if (cellNext.m_nCol > m_nFixedCols) {
            cellNext.m_nCol -= 1;
        }
        StopEdit(false);
        break;
    case VK_TAB:
        if (::GetKeyState(VK_SHIFT) < 0) {
            if (cellNext.m_nCol > m_nFixedCols) {
                cellNext.m_nCol -= 1;
            } else {
                cellNext.m_nCol = GetColCount() - 1;
                if (cellNext.m_nRow > m_nFixedRows) {
                    cellNext.m_nRow -= 1;
                }
            }
        } else {
            if (cellNext.m_nCol < GetColCount() - 1) {
                cellNext.m_nCol += 1;
            } else {
                cellNext.m_nCol = m_nFixedCols;
                if (cellNext.m_nRow < GetRowCount() - 1) {
                    cellNext.m_nRow += 1;
                }
            }
        }
        StopEdit(false);
        break;
    case VK_NEXT: // Page Down
        {
            SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
            int nHScroll = GetScrollPos(SB_HORZ);
            int nVScroll = GetScrollPos(SB_VERT);
            m_rangeSelected.Set(m_nFixedRows + nVScroll, m_rangeSelected.GetMinCol(),
                                m_nFixedRows + nVScroll, m_rangeSelected.GetMinCol());
            StopEdit(false);
            Invalidate();
            return;
        }
        break;
    case VK_PRIOR: // Page Up
        {
            SendMessage(WM_VSCROLL, SB_PAGEUP, 0);
            int nHScroll = GetScrollPos(SB_HORZ);
            int nVScroll = GetScrollPos(SB_VERT);
            m_rangeSelected.Set(m_nFixedRows + nVScroll, m_rangeSelected.GetMinCol(),
                                m_nFixedRows + nVScroll, m_rangeSelected.GetMinCol());
            StopEdit(false);
            Invalidate();
            return;
        }
        break;
    case VK_HOME:
        {
            GetScrollRange(SB_HORZ, &nScrollMin, &nScrollMax);
            if (nScrollMax > 0) {
                PostMessage(WM_HSCROLL, SB_TOP, 0);
            }
            cellNext.m_nCol = GetFixedColCount();

            if (::GetKeyState(VK_CONTROL) < 0) {
                GetScrollRange(SB_VERT, &nScrollMin, &nScrollMax);
                if (nScrollMax > 0) {
                    PostMessage(WM_VSCROLL, SB_TOP, 0);
                }
                cellNext.m_nRow = GetFixedRowCount();
            }
            m_cellActive = cellNext;
            return;
        }
        break;
    case VK_END:
        {
            GetScrollRange(SB_HORZ, &nScrollMin, &nScrollMax);
            if (nScrollMax > 0) {
                PostMessage(WM_HSCROLL, SB_BOTTOM, 0);
            }
            cellNext.m_nCol = GetColCount() - 1;

            if (::GetKeyState(VK_CONTROL) < 0) {
                GetScrollRange(SB_VERT, &nScrollMin, &nScrollMax);
                if (nScrollMax > 0) {
                    PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
                }
                cellNext.m_nRow = GetRowCount() - 1;
            }
            m_cellActive = cellNext;
            return;
        }
        break;
    default:
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
    } // switch

    if (cellNext != topLeft) {
        m_cellActive = cellNext;

        CGridCellRange newSelectedRange(cellNext,cellNext);
        InvalidateCellRect(newSelectedRange.Union(m_rangeSelected));
        m_rangeSelected = newSelectedRange;
        if (!IsCellVisible(cellNext)) {
            switch (nChar) {
            case VK_RIGHT:
                EnsureVisible(m_cellActive.m_nRow, m_cellActive.m_nCol);
                break;
            case VK_LEFT:
                PostMessage(WM_HSCROLL, SB_LINELEFT, 0);
                break;
            case VK_DOWN:
                EnsureVisible(m_cellActive.m_nRow, m_cellActive.m_nCol);
                break;
            case VK_UP:
                PostMessage(WM_VSCROLL, SB_LINEUP, 0);
                break;
            case VK_TAB:
                EnsureVisible(m_cellActive.m_nRow, m_cellActive.m_nCol);
                break;
            }
        }
    }
}

bool CGridCtrlBase::IsSelectedRowVisible() const
{
    return IsRowVisible( m_rangeSelected.GetTopLeft().m_nRow );
}

bool CGridCtrlBase::IsRowVisible(int nRow) const
{
    int nVertScrollPos = GetScrollPos(SB_VERT);
    if ( nRow < m_nFixedRows + nVertScrollPos) {
        return false;
    }

    CRect rect;
    GetClientRect(&rect);
    int y = GetFixedRowHeight();
    for (int i = m_nFixedRows + nVertScrollPos; i <= nRow; i++) {
        y += GetRowHeight(i);
        if (y > rect.bottom)
            break;
    }
    if (y > rect.bottom) {
        return false;
    }
    return true;
}

bool CGridCtrlBase::IsSelectedCellVisible() const
{
    return IsCellVisible(m_rangeSelected.GetTopLeft());
}

bool CGridCtrlBase::IsCellVisible(const CGridCellIndices& cell) const
{
    int nHorzScrollPos = GetScrollPos(SB_HORZ);
    int nVertScrollPos = GetScrollPos(SB_VERT);
    if (cell.m_nCol < m_nFixedCols + nHorzScrollPos) {
        return false;
    } else if (cell.m_nRow < m_nFixedRows + nVertScrollPos) {
        return false;
    }
    CRect rect;
    GetClientRect(&rect);
    int x = GetFixedColWidth();
    for (int i = m_nFixedCols + nHorzScrollPos; i <= cell.m_nCol; i++) {
        x += GetColWidth(i);
        if (x > rect.right)
            break;
    }
    if (x > rect.right) {
        return false;
    }
    int y = GetFixedRowHeight();
    for (int i = m_nFixedRows + nVertScrollPos; i <= cell.m_nRow; i++) {
        y += GetRowHeight(i);
        if (y > rect.bottom)
            break;
    }
    if (y > rect.bottom) {
        return false;
    }
    return true;
}

void CGridCtrlBase::SetRowHeight(int nRow, int nHeight)
{
    m_auRowHeights[nRow] = nHeight;
}

void CGridCtrlBase::SetColWidth(int nCol, int nWidth)
{
    m_auColWidths[nCol] = nWidth;
}

CSize CGridCtrlBase::GetCellExtent(int /*nRow*/, int /*nCol*/, CDC* /*pDC*/)
{
    return CSize(m_nDefCellWidth, m_nDefCellHeight);
}

void CGridCtrlBase::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if ( GetRowCount() - GetFixedRowCount() == 0 ) {
        // no data nRows
        return;
    }
    if (m_bEditEnabled && !m_bEditMode && m_modeMouse == Nothing && m_rangeSelected.IsSingleCell() ) {
        if (nChar == VK_ESCAPE) {
            return;
        }
        StartEdit(nChar);
        if (nChar != VK_BACK) {
            m_strCurrentEditData.Empty();
        }
    }

    if (m_bEditMode && IsSelectedCellVisible()) {
        int nLength = max(0, m_strCurrentEditData.GetLength());
        switch (nChar) {
        case VK_BACK:
            {
                if (nLength > 0) {
                    m_strCurrentEditData.GetBufferSetLength(--nLength);
                    m_strCurrentEditData.ReleaseBuffer(nLength);
                }
            }
            break;
        case VK_ESCAPE:
            m_strCurrentEditData = m_strOldEditData;
        case VK_TAB:
        case VK_RETURN:
            StopEdit();
            break;
        default:
            {
                char* sz = m_strCurrentEditData.GetBufferSetLength(++nLength);
                sz[nLength - 1] = char(nChar);
                m_strCurrentEditData.ReleaseBuffer(nLength);
            }
            break;
        }
    }
    CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CGridCtrlBase::EnableEdit(bool bEnableEdit /*= true*/)
{
    // can't edit files, when you can't select single cells
    ASSERT(!m_bListMode);
    m_bEditEnabled = bEnableEdit;
}

void CGridCtrlBase::GetEditText(int /*nRow*/, int /*nCol*/, const CString& /*text*/)
{
    TRACE("You must override this in your derived class");
    ASSERT(0);
}

CString CGridCtrlBase::SetEditText(int /*nRow*/, int /*nCol*/)
{
    TRACE("You must override this in your derived class");
    ASSERT(0);
    return CString("grid");
}

void CGridCtrlBase::StartEdit(UINT nChar)
{
    ASSERT(m_rangeSelected.IsSingleCell());
    EnsureVisible(m_cellActive.m_nRow, m_cellActive.m_nCol);
    //  ASSERT(IsCellVisible(m_rangeSelected.GetTopLeft()));
    m_bEditMode = true;
}

void CGridCtrlBase::StopEdit(bool bRedraw /* = true */)
{
    projectUtilities::SetModifiedFlag(true);

    if (m_bEditMode) {

        ASSERT(m_rangeSelected.IsSingleCell());
        //      ASSERT(IsCellVisible(m_rangeSelected.GetTopLeft()));
        m_bEditMode = false;
        if (bRedraw) {
            Invalidate();
        }
    }
}

int CGridCtrlBase::PrintPage(CDC* pDC, const CRect& rect, int nStartingRow)
{
    CFont* pOldFont = pDC->SelectObject(&m_Font);
    int nCurrentY = rect.top;
    int i;
    for (i = nStartingRow;
         nCurrentY < rect.bottom && i < m_nRows;
         nCurrentY += GetRowHeight(i++))
        {
            int nCurrentX = rect.left;
            for (int j = 0; j < m_nCols; j++) {
                DrawCell(i, j, pDC, nCurrentX, nCurrentY);
                nCurrentX += GetColWidth(j);
            }
        }

    pDC->SelectObject(pOldFont);
    return i - nStartingRow;
}

// if set to true, makes single cell selection impossible
void CGridCtrlBase::SetListMode(bool bEnableListMode)
{
    // can't edit, when you can't select single cells
    ASSERT(!m_bEditEnabled);
    m_bListMode = bEnableListMode;
    m_rangeSelected.Set(m_nFixedRows, m_nFixedCols, m_nFixedRows, m_nCols - 1);
}

void CGridCtrlBase::SetEditMode(bool bEnable)
{
    m_bEditEnabled = bEnable;
}

LRESULT CGridCtrlBase::OnGetFont(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    return LRESULT(HFONT(m_Font));
}


// EnsureVisible
void CGridCtrlBase::EnsureVisible(int nRow, int nCol)
{
    if (nRow < 0 || nCol < 0) {
        return;
    }
    CGridCellRange cellsVisible = GetVisibleUnobstructedNonFixedCellRange();

    int right = nCol - cellsVisible.GetMaxCol();
    int left  = cellsVisible.GetMinCol() - nCol;
    int down  = nRow - cellsVisible.GetMaxRow();
    int up    = cellsVisible.GetMinRow() - nRow;

    while (right > 0) {
        SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
        right--;
    }
    while (left > 0) {
        SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
        left--;
    }
    while (down > 0) {
        SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
        down--;
    }
    while (up > 0) {
        SendMessage(WM_VSCROLL, SB_LINEUP, 0);
        up--;
    }

    // Move one more if we only see a small bit of the cell
    CGridCellRange range(nRow, nCol, nRow, nCol);
    CRect rectCell = GetCellRangeRect(range);
    CRect rectWindow;
    GetClientRect(rectWindow);
    if (rectCell.right > rectWindow.right) {
        SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
    }
    if (rectCell.bottom > rectWindow.bottom) {
        SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
    }
}

void CGridCtrlBase::InvalidateAll()
{
    InvalidateCellRect(CGridCellRange(CGridCellIndices(0, 0),
                                      CGridCellIndices(GetRowCount()-1, GetColCount()-1)));
}


BOOL CGridCtrlBase::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

void CGridCtrlBase::OnEnable(BOOL bEnable)
{
    Invalidate();
    Default();
}

