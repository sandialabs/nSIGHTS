///////////////////////////////////////////////////////////////////////////////////
//
// SplitterWndEx.cpp
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
//      Windows specific code for MFC user-interface extension or enhancement classes.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "SplitterWndEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define CX_BORDER   1
#define CY_BORDER   1

#include <afxpriv.h>

static BOOL bNotWin4;

BEGIN_MESSAGE_MAP(CSplitterWndEx, CSplitterWnd)
    //{{AFX_MSG_MAP(CSplitterWndEx)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSplitterWndEx::CSplitterWndEx()
{
    m_cxBorder = 0;
    m_cyBorder = 0;
    m_cxSplitter = m_cySplitter = 2 + 2;
    m_cxSplitterGap = m_cySplitterGap = 2 + 2;
    m_upBorder = 8;
    bWhiteLine = true;

    DWORD dwVersion = ::GetVersion();
    BOOL bWin4 = (BYTE)dwVersion >= 4;
    bNotWin4 = 1 - bWin4;   // for convenience
}

CSplitterWndEx::~CSplitterWndEx()
{
}

void CSplitterWndEx::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
    if (pDC == NULL) {
        RedrawWindow(rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
        return;
    }
    ASSERT_VALID(pDC);
    CRect rect = rectArg;
    switch (nType) {
    case splitBorder:
        return;
    case splitBox:
        pDC->Draw3dRect(rect, GetSysColor(COLOR_3DFACE), GetSysColor(COLOR_WINDOWFRAME));
        rect.InflateRect(-CX_BORDER, -CY_BORDER);
        pDC->Draw3dRect(rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
        rect.InflateRect(-CX_BORDER, -CY_BORDER);
        break;
    }

    COLORREF clr = GetSysColor(COLOR_3DFACE);
    pDC->FillSolidRect(rect, clr);
}

void CSplitterWndEx::OnInvertTracker(const CRect & rect)
{
    ASSERT_VALID(this);
    ASSERT(!rect.IsRectEmpty());
    ASSERT((GetStyle() & WS_CLIPCHILDREN) == 0);
    CDC* pDC = GetDC();
    CBrush* pBrush = CBrush::FromHandle((HBRUSH) GetStockObject(WHITE_BRUSH));
    HBRUSH hOldBrush = NULL;
    if (pBrush != NULL) hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);
    pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
    if (hOldBrush != NULL) SelectObject(pDC->m_hDC, hOldBrush);
    ReleaseDC(pDC);
}

// repositions client area of specified window
// assumes everything has WS_BORDER or is inset like it does
//  (includes scroll bars)
static void AFXAPI DeferClientPos(AFX_SIZEPARENTPARAMS* lpLayout,
    CWnd* pWnd, int x, int y, int cx, int cy, BOOL bScrollBar)
{
    ASSERT(pWnd != NULL);
    ASSERT(pWnd->m_hWnd != NULL);

    if (bScrollBar)
    {
        // if there is enough room, draw scroll bar without border
        // if there is not enough room, set the WS_BORDER bit so that
        //   we will at least get a proper border drawn
        BOOL bNeedBorder = (cx <= CX_BORDER || cy <= CY_BORDER);
        pWnd->ModifyStyle(bNeedBorder ? 0 : WS_BORDER,
            bNeedBorder ? WS_BORDER : 0);
    }
    CRect rect(x, y, x+cx, y+cy);

    // adjust for border size (even if zero client size)

    // adjust for 3d border (splitter windows have implied border)
    if ((pWnd->GetExStyle() & WS_EX_CLIENTEDGE) ||
          pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
        rect.InflateRect(CX_BORDER*2, CY_BORDER*2);

    // first check if the new rectangle is the same as the current
    CRect rectOld;
    pWnd->GetWindowRect(rectOld);
    pWnd->GetParent()->ScreenToClient(&rectOld);
    if (rect != rectOld)
        AfxRepositionWindow(lpLayout, pWnd->m_hWnd, rect);
}


static void AFXAPI LayoutRowCol(CSplitterWnd::CRowColInfo* pInfoArray,
    int nMax, int nSize, int nSizeSplitter)
{
    ASSERT(pInfoArray != NULL);
    ASSERT(nMax > 0);
    ASSERT(nSizeSplitter > 0);

    CSplitterWnd::CRowColInfo* pInfo;
    int i;

    if (nSize < 0)
        nSize = 0;  // if really too small, layout as zero size

    // start with ideal sizes
    for (i = 0, pInfo = pInfoArray; i < nMax-1; i++, pInfo++)
    {
        if (pInfo->nIdealSize < pInfo->nMinSize)
            pInfo->nIdealSize = 0;      // too small to see
        pInfo->nCurSize = pInfo->nIdealSize;
    }
    pInfo->nCurSize = INT_MAX;  // last row/column takes the rest

    for (i = 0, pInfo = pInfoArray; i < nMax; i++, pInfo++)
    {
        ASSERT(nSize >= 0);
        if (nSize == 0)
        {
            // no more room (set pane to be invisible)
            pInfo->nCurSize = 0;
            continue;       // don't worry about splitters
        }
        else if (nSize < pInfo->nMinSize && i != 0)
        {
            // additional panes below the recommended minimum size
            //   aren't shown and the size goes to the previous pane
            pInfo->nCurSize = 0;

            // previous pane already has room for splitter + border
            //   add remaining size and remove the extra border
            ASSERT(CX_BORDER*2 == CY_BORDER*2);
            (pInfo-1)->nCurSize += nSize + CX_BORDER*2;
            nSize = 0;
        }
        else
        {
            // otherwise we can add the second pane
            ASSERT(nSize > 0);
            if (pInfo->nCurSize == 0)
            {
                // too small to see
                if (i != 0)
                    pInfo->nCurSize = 0;
            }
            else if (nSize < pInfo->nCurSize)
            {
                // this row/col won't fit completely - make as small as possible
                pInfo->nCurSize = nSize;
                nSize = 0;
            }
            else
            {
                // can fit everything
                nSize -= pInfo->nCurSize;
            }
        }

        // see if we should add a splitter
        ASSERT(nSize >= 0);
        if (i != nMax - 1)
        {
            // should have a splitter
            if (nSize > nSizeSplitter)
            {
                nSize -= nSizeSplitter; // leave room for splitter + border
                ASSERT(nSize > 0);
            }
            else
            {
                // not enough room - add left over less splitter size
                ASSERT(CX_BORDER*2 == CY_BORDER*2);
                pInfo->nCurSize += nSize;
                if (pInfo->nCurSize > (nSizeSplitter - CX_BORDER*2))
                    pInfo->nCurSize -= (nSizeSplitter - CY_BORDER*2);
                nSize = 0;
            }
        }
    }
    ASSERT(nSize == 0); // all space should be allocated
}

void CSplitterWndEx::RecalcLayout()
{
    ASSERT_VALID(this);
    ASSERT(m_nRows > 0 && m_nCols > 0); // must have at least one pane

    CRect rectClient;
    GetClientRect(rectClient);
    rectClient.InflateRect(-m_cxBorder, -m_cyBorder);
    rectClient.top += m_upBorder;

    CRect rectInside;
    GetInsideRect(rectInside);

    // layout columns (restrict to possible sizes)
    LayoutRowCol(m_pColInfo, m_nCols, rectInside.Width(), m_cxSplitterGap);
    LayoutRowCol(m_pRowInfo, m_nRows, rectInside.Height(), m_cySplitterGap);

    // adjust the panes (and optionally scroll bars)

    // give the hint for the maximum number of HWNDs
    AFX_SIZEPARENTPARAMS layout;
    layout.hDWP = ::BeginDeferWindowPos((m_nCols + 1) * (m_nRows + 1) + 1);

    // size of scrollbars
    int cx = (rectClient.right - rectInside.right - bNotWin4);// - afxData.bNotWin4;
    int cy = (rectClient.bottom - rectInside.bottom - bNotWin4);// - afxData.bNotWin4;

    // reposition size box
    if (m_bHasHScroll && m_bHasVScroll) {
        CWnd* pScrollBar = GetDlgItem(AFX_IDW_SIZE_BOX);
        ASSERT(pScrollBar != NULL);

        // fix style if necessary
        BOOL bSizingParent = (GetSizingParent() != NULL);
        // modifyStyle returns TRUE if style changes
        if (pScrollBar->ModifyStyle(SBS_SIZEGRIP|SBS_SIZEBOX,
                bSizingParent ? SBS_SIZEGRIP : SBS_SIZEBOX))
            pScrollBar->Invalidate();
        pScrollBar->EnableWindow(bSizingParent);

        // reposition the size box
        DeferClientPos(&layout, pScrollBar,
            rectInside.right + bNotWin4,
            rectInside.bottom + bNotWin4, cx, cy, TRUE);
    }

    // reposition scroll bars
    if (m_bHasHScroll) {
        int cxSplitterBox = m_cxSplitter + bNotWin4;// split box bigger
        int x = rectClient.left;
        int y = rectInside.bottom + bNotWin4;
        for (int col = 0; col < m_nCols; col++) {
            CWnd* pScrollBar = GetDlgItem(AFX_IDW_HSCROLL_FIRST + col);
            ASSERT(pScrollBar != NULL);
            int cx = m_pColInfo[col].nCurSize;
            if (col == 0 && m_nCols < m_nMaxCols)
                x += cxSplitterBox, cx -= cxSplitterBox;
            DeferClientPos(&layout, pScrollBar, x, y, cx, cy, TRUE);
            x += cx + m_cxSplitterGap;
        }
    }

    if (m_bHasVScroll) {
        int cySplitterBox = m_cySplitter + bNotWin4;// split box bigger
        int x = rectInside.right + bNotWin4;
        int y = rectClient.top;
        for (int row = 0; row < m_nRows; row++)
        {
            CWnd* pScrollBar = GetDlgItem(AFX_IDW_VSCROLL_FIRST + row);
            ASSERT(pScrollBar != NULL);
            int cy = m_pRowInfo[row].nCurSize;
            if (row == 0 && m_nRows < m_nMaxRows)
                y += cySplitterBox, cy -= cySplitterBox;
            DeferClientPos(&layout, pScrollBar, x, y, cx, cy, TRUE);
            y += cy + m_cySplitterGap;
        }
    }

    //BLOCK: Reposition all the panes
    {
        int x = rectClient.left;
        for (int col = 0; col < m_nCols; col++) {
            int cx = m_pColInfo[col].nCurSize;
            int y = rectClient.top;
            for (int row = 0; row < m_nRows; row++) {
                int cy = m_pRowInfo[row].nCurSize;
                CWnd* pWnd = GetPane(row, col);
                DeferClientPos(&layout, pWnd, x, y, cx, cy, FALSE);
                y += cy + m_cySplitterGap;
            }
            x += cx + m_cxSplitterGap;
        }
    }

    // move and resize all the windows at once!
    if (layout.hDWP == NULL || !::EndDeferWindowPos(layout.hDWP))
        TRACE0("Warning: DeferWindowPos failed - low system resources.\n");

    // invalidate all the splitter bars (with NULL pDC)
    DrawAllSplitBars(NULL, rectInside.right, rectInside.bottom);
}

void CSplitterWndEx::GetInsideRect(CRect& rect) const
{
    CSplitterWnd::GetInsideRect(rect);
    rect.top += m_upBorder;
/*  ASSERT_VALID(this);

    GetClientRect(rect);
    ASSERT(rect.left == 0 && rect.top == 0);

    // subtract space for 3d borders
    rect.InflateRect(-m_cxBorder, -m_cyBorder);
    rect.top += m_upBorder;

    // subtract scrollbar clearance
    if (m_bHasVScroll)
        rect.right -= GetSystemMetrics(SM_CXVSCROLL);
    if (m_bHasHScroll)
        rect.bottom -= GetSystemMetrics(SM_CYHSCROLL);*/
}

BOOL CSplitterWndEx::OnEraseBkgnd(CDC* pDC)
{
    CRect rectClient;
    GetClientRect(rectClient);
    rectClient.InflateRect(-m_cxBorder, -m_cyBorder);
    rectClient.bottom = rectClient.top + m_upBorder;
    pDC->FillSolidRect(rectClient, GetSysColor(COLOR_3DFACE));
    return TRUE;
}

void CSplitterWndEx::OnPaint()
{
    CSplitterWnd::OnPaint();
    if (bWhiteLine) {
        CClientDC dc(this);
        CRect rectClient;
        GetClientRect(rectClient);
        CPen pn(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
        CPen* op = dc.SelectObject(&pn);
        dc.MoveTo(rectClient.left, rectClient.top);
        dc.LineTo(rectClient.right, rectClient.top);
        CPen pn1(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
        dc.SelectObject(&pn1);
        dc.MoveTo(rectClient.left, rectClient.top+1);
        dc.LineTo(rectClient.right, rectClient.top+1);
        dc.SelectObject(op);
    }
}

void CSplitterWndEx::OnDestroy()
{
    CSplitterWnd::OnDestroy();
    m_nRows = 0;
    m_nCols = 0;
    delete[] m_pRowInfo;
    m_pRowInfo = NULL;
    delete[] m_pColInfo;
    m_pColInfo = NULL;
}


//////////////////
// Splitter wnd function to route commands to all splitter panes/views.
// This lets all panes, not just the active pane, handle messages.
//
BOOL CSplitterWndEx::OnCmdMsg(UINT nID, int nCode, void* pExtra,
    AFX_CMDHANDLERINFO* pHandlerInfo)
{
    for (int row=0; row<GetRowCount(); row++) {
        for (int col=0; col<GetColumnCount(); col++) {
            if (GetPane(row,col)->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
                return TRUE;
        }
    }
    // Call default routing--very important!
    return CSplitterWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

