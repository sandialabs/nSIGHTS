///////////////////////////////////////////////////////////////////////////////////
//
// TabbedComboBox.cpp
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

#include "TabbedComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTabbedComboBox::CTabbedComboBox()
{
    scrollAdded = false;
}

CTabbedComboBox::~CTabbedComboBox()
{
}

void CTabbedComboBox::RecalcDropWidth()
{
    m_aLength.RemoveAll();

    CClientDC dc(this);
    int nSave = dc.SaveDC();
    dc.SelectObject(GetFont());

    CString sText;
    CString sSubstring;
    CString sRight;
    int nNumEntries = GetCount();
    for (int i = 0; i < nNumEntries; i++) {
        GetLBText(i, sText);

        int nStart = 0;
        int nPos;
        int j = 0;
        while ((nPos = sText.Find('\t', nStart)) >= 0) {
            sSubstring = sText.Mid(nStart, nPos - nStart);
            int nLength = dc.GetTextExtent(sSubstring).cx;

            if (j > m_aLength.GetSize()-1) {
                int nZero = 0;
                m_aLength.Add(nZero);
            }
            m_aLength[j] = max(m_aLength[j], nLength);
            j++;
            nStart = nPos+1;
        }
        sRight = sText.Mid(nStart);
        if (!sRight.IsEmpty()) {
            int nLength = dc.GetTextExtent(sRight).cx;
            if (j > m_aLength.GetSize()-1) {
                int nZero = 0;
                m_aLength.Add(nZero);
            }
            m_aLength[j] = max(m_aLength[j], nLength);
        }
    }

    // Add margin space to the calculations
    int nSpace = dc.GetTextExtent("0").cx;
    dc.RestoreDC(nSave);

    int nWidth = 0;
    for (int i = 0; i < m_aLength.GetSize(); i++) {
        m_aLength[i] += nSpace;
        nWidth += m_aLength[i];
    }

    scrollAdded = false;
//  nWidth += ::GetSystemMetrics(SM_CXVSCROLL);


    SetDroppedWidth(nWidth);
}

BEGIN_MESSAGE_MAP(CTabbedComboBox, CComboBox)
    //{{AFX_MSG_MAP(CTabbedComboBox)
    ON_WM_CREATE()
    ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
    ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
    ON_WM_CTLCOLOR()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabbedComboBox message handlers

int CTabbedComboBox::OnCreate(LPCREATESTRUCT pCStruct)
{
    if (CComboBox::OnCreate(pCStruct) == -1) {
        return -1;
    }
    ASSERT((pCStruct->style & (CBS_HASSTRINGS|CBS_OWNERDRAWFIXED)) != 0);
    return 0;
}

// In the case of dialog created from resources PreSubclassWindow
// is the only chance to check style
void CTabbedComboBox::PreSubclassWindow()
{
    CComboBox::PreSubclassWindow();
    ASSERT((GetStyle() & (CBS_HASSTRINGS|CBS_OWNERDRAWFIXED)) != 0);
}

void CTabbedComboBox::OnDropdown()
{
    RecalcDropWidth();
}

void CTabbedComboBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
    if (int(lpDIS->itemID) < 0) {
        return;
    }

    CDC dcContext;
    if (!dcContext.Attach(lpDIS->hDC)) {
        return;
    }

    // If item has been selected, draw the highlight rectangle using the item's color.
    if ((lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & (ODA_SELECT|ODA_DRAWENTIRE)))
    {
        CBrush brush(::GetSysColor(COLOR_WINDOWTEXT));
        dcContext.FillRect(&lpDIS->rcItem, &brush);
    }

    // If item has been deselected, draw the rectangle using the window color.
    if (!(lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & ODA_SELECT))
    {
        CBrush brush(::GetSysColor(COLOR_WINDOW));
        dcContext.FillRect(&lpDIS->rcItem, &brush);
    }

    // If item has focus, draw the focus rect.
    if ((lpDIS->itemAction & ODA_FOCUS) && (lpDIS->itemState & ODS_FOCUS))
    {
        dcContext.DrawFocusRect(&lpDIS->rcItem);
    }

    // If item does not have focus, redraw (erase) the focus rect.
    if ((lpDIS->itemAction & ODA_FOCUS) && !(lpDIS->itemState & ODS_FOCUS))
    {
        dcContext.DrawFocusRect(&lpDIS->rcItem);
    }


    int nBkMode = dcContext.SetBkMode(TRANSPARENT);

    COLORREF crText;
    if (lpDIS->itemState & ODS_SELECTED)
        crText = dcContext.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    else if (lpDIS->itemState & ODS_DISABLED)
        crText = dcContext.SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
    else
        crText = dcContext.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

    CString sText;
    GetLBText(lpDIS->itemID, sText);
    CRect rcItem = lpDIS->rcItem;

    UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
    if (GetStyle() & LBS_USETABSTOPS) {
        nFormat |= DT_EXPANDTABS;
    }

    CRect rect = rcItem;
    if (lpDIS->itemState & ODS_COMBOBOXEDIT) {
        CString sRight;
        CString sSubstring;
        int nStart = 0;
        int nPos;
        int j = 0;
        while ((nPos = sText.Find('\t', nStart)) >= 0) {
            sText.SetAt(nPos, ' ');
            nStart = nPos + 1;
            j++;
        }
        dcContext.DrawText(sText, -1, &rect, nFormat);
    } else if (m_aLength.GetSize() == 0) {
        dcContext.DrawText(sText, -1, &rect, nFormat);
    } else {
        ASSERT(m_aLength.GetSize() != 0);
        CString sRight;
        CString sSubstring;
        int nStart = 0;
        int nPos;
        int j = 0;
        rect.right = rect.left;
        while ((nPos = sText.Find('\t', nStart)) >= 0) {
            rect.right += m_aLength[j];
            sSubstring = sText.Mid(nStart, nPos - nStart);
            dcContext.DrawText(sSubstring, -1, &rect, nFormat);
            nStart = nPos + 1;
            rect.left += m_aLength[j];
            j++;
        }
        sRight = sText.Mid(nStart);
        if (!sRight.IsEmpty()) {
            rect.right = rcItem.right;
            dcContext.DrawText(sRight, -1, &rect, nFormat);
        }
    }

    dcContext.SetTextColor(crText);
    dcContext.SetBkMode(nBkMode);
    dcContext.Detach();
}

void CTabbedComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
    // the menu check height???
    //lpMIS->itemHeight = ::GetSystemMetrics(SM_CYMENUCHECK);
    lpMIS->itemHeight = 2 + 12 + 2;
}

void CTabbedComboBox::OnCloseup()
{
    m_aLength.RemoveAll();
}

HBRUSH CTabbedComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (GetDroppedState() && pWnd != NULL && pWnd != this) {
        // pWnd is list box
        CRect rect;
        pWnd->GetWindowRect(rect);

        bool bMove = false;

        if ((!scrollAdded) && (pWnd->GetStyle() & WS_VSCROLL)) {
            rect.right += ::GetSystemMetrics(SM_CXVSCROLL);
            scrollAdded = true;
            bMove = true;
        }

        int cx = ::GetSystemMetrics(SM_CXSCREEN);
        if (rect.right > cx) {
            int diff = rect.right - cx;
            rect.right -= diff;
            rect.left -= diff;
            bMove = true;
        }

        if (bMove)
            pWnd->MoveWindow(rect, true);

    }
    return CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
}

