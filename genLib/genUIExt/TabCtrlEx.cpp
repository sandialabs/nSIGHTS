///////////////////////////////////////////////////////////////////////////////////
//
// TabCtrlEx.cpp
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

// TabCtrlEx.cpp : implementation file
//
#include "GenLibAfx.h"

#include "TabCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx

CTabCtrlEx::CTabCtrlEx() :
    m_iSelectedTab(0)
{
}

CTabCtrlEx::~CTabCtrlEx()
{
    m_aStatusTab.RemoveAll();
}


BEGIN_MESSAGE_MAP(CTabCtrlEx, CTabCtrl)
    //{{AFX_MSG_MAP(CTabCtrlEx)
    ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
    ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)
    ON_WM_MOUSEMOVE()
    ON_WM_TIMER()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx message handlers

void CTabCtrlEx::AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage)
{
    ASSERT_VALID(pWnd);

    TCITEM item;
    item.mask = TCIF_TEXT|TCIF_PARAM|TCIF_IMAGE;
    item.lParam = LPARAM(pWnd);
    item.pszText = lpszCaption;
    item.iImage = iImage;

    int iIndex = (int) m_aStatusTab.GetSize();
    InsertItem(iIndex, &item);

    CPoint ptTabs(4, 28);
    pWnd->SetWindowPos(NULL, ptTabs.x, ptTabs.y, 0, 0,
                        SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOZORDER);

    pWnd->ShowWindow(iIndex ? SW_HIDE : SW_SHOW);

    // The initial status is enabled
    m_aStatusTab.Add(true);
}

void CTabCtrlEx::AddTab(LPTSTR lpszCaption, int iImage)
{
    TCITEM item;
    item.mask = TCIF_TEXT|TCIF_IMAGE;
    item.lParam = LPARAM(0);
    item.pszText = lpszCaption;
    item.iImage = iImage;

    int iIndex = (int) m_aStatusTab.GetSize();
    InsertItem(iIndex, &item);
    m_aStatusTab.Add(true);
}

bool CTabCtrlEx::EnableTab(int iIndex, bool bEnable)
{
    ASSERT(iIndex < m_aStatusTab.GetSize());

    bool bPrevStatus = m_aStatusTab[iIndex];
    // If it should change the status
    if (m_aStatusTab[iIndex] != bEnable) {
        m_aStatusTab[iIndex] = bEnable;
        // Redraw the item
        CRect rect;
        GetItemRect(iIndex, &rect);
        InvalidateRect(rect);
    }
    return bPrevStatus;
}

void CTabCtrlEx::DeleteAllTabs()
{
    m_aStatusTab.RemoveAll();
    DeleteAllItems();
}

void CTabCtrlEx::DeleteTab(int iIndex)
{
    ASSERT(iIndex < m_aStatusTab.GetSize());
    m_aStatusTab.RemoveAt(iIndex);
    DeleteItem(iIndex);
}

bool CTabCtrlEx::IsTabEnabled(int iIndex)
{
    ASSERT(iIndex < m_aStatusTab.GetSize());
    return m_aStatusTab[iIndex];
}

bool CTabCtrlEx::SelectTab(int iIndex)
{
    ASSERT(iIndex < m_aStatusTab.GetSize());

    if (GetCurSel() == iIndex) {
        return true;
    }

    // Check if the tab is enabled
    if (m_aStatusTab[iIndex]) {
        TCITEM item;
        item.mask = TCIF_PARAM;
        int nSel = GetCurSel();
        if (GetItem(nSel, &item) && item.lParam != 0) {

            CWnd* pWnd = reinterpret_cast<CWnd*>(item.lParam);
            ASSERT_VALID(pWnd);
            pWnd->ShowWindow(SW_HIDE);

            SetCurSel(iIndex);
            GetItem(iIndex, &item);
            pWnd = reinterpret_cast<CWnd*>(item.lParam);
            ASSERT_VALID(pWnd);
            pWnd->ShowWindow(SW_SHOW);
        } else {
            SetCurSel(iIndex);
            OnSelection(iIndex);
        }
        return true;
    }
    return false;
}

void CTabCtrlEx::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
{
    int iNewTab = GetCurSel();

    if (!IsTabEnabled(iNewTab)) {
        SetCurSel(m_iSelectedTab);
    } else {
        TCITEM item;
        item.mask = TCIF_PARAM;

        // Hide the current tab
        if (GetItem(m_iSelectedTab, &item) && item.lParam != 0) {
            CWnd* pWnd = reinterpret_cast<CWnd*>(item.lParam);
            ASSERT_VALID(pWnd);
            pWnd->ShowWindow(SW_HIDE);

            // Show the selected tab
            GetItem(iNewTab, &item);
            pWnd = reinterpret_cast<CWnd*>(item.lParam);
            ASSERT_VALID(pWnd);
            pWnd->ShowWindow(SW_SHOW);
        } else {
            OnSelection(iNewTab);
        }
    }
    *pResult = 0;
}

void CTabCtrlEx::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult)
{
    m_iSelectedTab = GetCurSel();
    *pResult = 0;
}

void CTabCtrlEx::PreSubclassWindow()
{
    CTabCtrl::PreSubclassWindow();
    ModifyStyle(0, TCS_OWNERDRAWFIXED);
}

void CTabCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CRect rect = lpDrawItemStruct->rcItem;
    rect.top += ::GetSystemMetrics(SM_CYEDGE);
    int nTabIndex = lpDrawItemStruct->itemID;
    if (nTabIndex < 0) {
        return;
    }
    BOOL bSelected = nTabIndex == GetCurSel();
    COLORREF crSelected = ::GetSysColor(COLOR_BTNTEXT);
    COLORREF crNormal  = GetSysColor(COLOR_BTNTEXT);
    COLORREF crDisabled = GetSysColor(COLOR_GRAYTEXT);

    char szLabel[64];
    TC_ITEM item;
    item.mask = TCIF_TEXT|TCIF_IMAGE;
    item.pszText = szLabel;
    item.cchTextMax = 63;
    if (!GetItem(nTabIndex, &item)) {
        return;
    }

    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    if (pDC == NULL) {
        return;
    }

    int nSavedDC = pDC->SaveDC();

    CRect rectItem;
    POINT pt;
    GetItemRect(nTabIndex, &rectItem);
    GetCursorPos(&pt);
    ScreenToClient(&pt);

    pDC->SetBkMode(TRANSPARENT);
    pDC->FillSolidRect(rect, ::GetSysColor(COLOR_BTNFACE));

    // Draw the image
    CImageList* pImageList = GetImageList();
    if (pImageList != NULL && item.iImage >= 0) {

        rect.left += pDC->GetTextExtent(_T(" ")).cx;

        IMAGEINFO info;
        pImageList->GetImageInfo(item.iImage, &info);
        CRect rectImage(info.rcImage);
        int nYpos = rect.top;

        pImageList->Draw(pDC, item.iImage, CPoint(rect.left, nYpos), ILD_TRANSPARENT);
        rect.left += rectImage.Width();
    }

    pDC->SetTextColor(bSelected ? crSelected : crNormal);
    rect.top -= ::GetSystemMetrics(SM_CYEDGE);
    pDC->DrawText(szLabel, rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER);

    if (!IsTabEnabled(nTabIndex))
        DrawDisabledItem(*pDC, rectItem);

    pDC->RestoreDC(nSavedDC);
}

void CTabCtrlEx::DrawDisabledItem(CDC& dc, const CRect& rc) const
{
    COLORREF clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
    COLORREF clrBtnHilight = ::GetSysColor(COLOR_BTNHILIGHT);
    COLORREF clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
    // create a monochrome memory DC
    CDC monoDC;
    monoDC.CreateCompatibleDC(0);
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&monoDC, rc.Width(), rc.Height());
    CBitmap* pOldBmp = monoDC.SelectObject(&bmp);

    // build a mask
    monoDC.PatBlt(0, 0, rc.Width(), rc.Height(), WHITENESS);
    dc.SetBkColor(clrBtnFace);
    monoDC.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, rc.left, rc.top, SRCCOPY);
    dc.SetBkColor(clrBtnHilight);
    monoDC.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, rc.left, rc.top, SRCPAINT);

    // Copy the image from dc into the memory DC
    // and draw it (grayed) back dc.
    dc.FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), clrBtnFace);
    dc.SetBkColor(RGB(0, 0, 0));
    dc.SetTextColor(RGB(255, 255, 255));
    CBrush brHilight;
    brHilight.CreateSolidBrush(clrBtnHilight);
    CBrush brShadow;
    brShadow.CreateSolidBrush(clrBtnShadow);
    CBrush* pOldBrush = dc.SelectObject(&brHilight);
    dc.BitBlt(rc.left+1, rc.top+1, rc.Width(), rc.Height(), &monoDC, 0, 0, 0x00E20746L);
    dc.SelectObject(&brShadow);
    dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &monoDC, 0, 0, 0x00E20746L);

    // reset DCs
    dc.SelectObject(pOldBrush);
    monoDC.SelectObject(pOldBmp);
    monoDC.DeleteDC();

    bmp.DeleteObject();
}

inline int NextTab(int i, int nCount)
{
    return i < nCount-1 ? i+1 : 0;
}

inline int PrevTab(int i, int nCount)
{
    return i > 0 ? i-1 : nCount-1;
}

BOOL CTabCtrlEx::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN) {
        if (pMsg->wParam == VK_TAB && GetKeyState(VK_CONTROL) < 0) {
            bool bShift = GetKeyState(VK_SHIFT) < 0;
            int nCurSel = GetCurSel();
            int nCount = GetItemCount();
            if (!bShift) {
                for (int i = NextTab(nCurSel, nCount), j = 0; j < nCount ; j++, i = NextTab(i, nCount) ) {
                    if (IsTabEnabled(i)) {
                        SetCurSel(i);
                        OnSelection(i);
                        break;
                    }
                }
            } else {
                for (int i = PrevTab(nCurSel, nCount), j = 0; j < nCount ; j++, i = PrevTab(i, nCount) ) {
                    if (IsTabEnabled(i)) {
                        SetCurSel(i);
                        OnSelection(i);
                        break;
                    }
                }
            }
            return TRUE;    // DO NOT process further
        }
    }
    return CTabCtrl::PreTranslateMessage(pMsg);
}

