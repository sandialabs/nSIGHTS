///////////////////////////////////////////////////////////////////////////////////
//
// ExtendedComboBox.cpp
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

#include "ExtendedComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT nMessage = ::RegisterWindowMessage("ComboSelEndOK");

/////////////////////////////////////////////////////////////////////////////
// CExtendedComboBox

CExtendedComboBox::CExtendedComboBox()
{
    m_listBox.SetParent(this);
}

CExtendedComboBox::~CExtendedComboBox()
{
}

void CExtendedComboBox::EnableItem(UINT nIndex, bool bEnable)
{
    ASSERT(nIndex < (DWORD)GetCount());
    DWORD_PTR uData = GetItemData(nIndex);
    SetItemData(nIndex, bEnable ? 0 : 1 );
}

// default implementation
bool CExtendedComboBox::IsItemEnabled(UINT nIndex) const
{
    ASSERT(nIndex < (DWORD)GetCount());
    DWORD_PTR uData = GetItemData(nIndex);

    return uData == 0;
}


BEGIN_MESSAGE_MAP(CExtendedComboBox, CComboBox)
//{{AFX_MSG_MAP(CExtendedComboBox)
ON_WM_CHARTOITEM()
ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendok)
ON_WM_KEYDOWN()
//}}AFX_MSG_MAP
ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColor)
ON_REGISTERED_MESSAGE(nMessage, OnRealSelEndOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtendedComboBox message handlers

void CExtendedComboBox::PreSubclassWindow()
{
    ASSERT((GetStyle()&(CBS_OWNERDRAWFIXED|CBS_HASSTRINGS))==(CBS_OWNERDRAWFIXED|CBS_HASSTRINGS));
    CComboBox::PreSubclassWindow();
}

void CExtendedComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
        (lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
        {
            BOOL fDisabled = !IsWindowEnabled () || !IsItemEnabled(lpDrawItemStruct->itemID);

            COLORREF newTextColor = fDisabled ?
                RGB(0x80, 0x80, 0x80) : GetSysColor (COLOR_WINDOWTEXT);  // light gray

            COLORREF oldTextColor = pDC->SetTextColor (newTextColor);

            COLORREF newBkColor = GetSysColor (COLOR_WINDOW);
            COLORREF oldBkColor = pDC->SetBkColor (newBkColor);

            if (newTextColor == newBkColor)
                newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

            if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0)) {
                pDC->SetTextColor (GetSysColor (COLOR_HIGHLIGHTTEXT));
                pDC->SetBkColor (GetSysColor (COLOR_HIGHLIGHT));
            }

            CString strText;
            GetLBText(lpDrawItemStruct->itemID, strText);

            const RECT &rc=lpDrawItemStruct->rcItem;

            pDC->ExtTextOut(rc.left + 2,
                            rc.top + 2,// + max(0, (cyItem - m_cyText) / 2),
                            ETO_OPAQUE, &rc,
                            strText, strText.GetLength (), NULL);

            pDC->SetTextColor (oldTextColor);
            pDC->SetBkColor (oldBkColor);
        }
    else if ((LONG)(lpDrawItemStruct->itemID)<0)    // drawing edit text
        {
            COLORREF newTextColor = GetSysColor (COLOR_WINDOWTEXT);  // light gray
            COLORREF oldTextColor = pDC->SetTextColor (newTextColor);

            COLORREF newBkColor = GetSysColor (COLOR_WINDOW);
            COLORREF oldBkColor = pDC->SetBkColor (newBkColor);

            if ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0) {
                pDC->SetTextColor (GetSysColor (COLOR_HIGHLIGHTTEXT));
                pDC->SetBkColor (GetSysColor (COLOR_HIGHLIGHT));
            }

            CString strText;
            GetWindowText(strText);
            const RECT &rc = lpDrawItemStruct->rcItem;

            pDC->ExtTextOut(rc.left + 2,
                            rc.top + 2,// + max(0, (cyItem - m_cyText) / 2),
                            ETO_OPAQUE, &rc,
                            strText, strText.GetLength (), NULL);

            pDC->SetTextColor (oldTextColor);
            pDC->SetBkColor (oldBkColor);
        }

    if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0) {
        pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
    }

}

void CExtendedComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    UNREFERENCED_PARAMETER(lpMeasureItemStruct);
}

int CExtendedComboBox::OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex)
{
    int nRc = CComboBox::OnCharToItem(nChar, pListBox, nIndex);
    if (nRc >= 0 && !IsItemEnabled(nRc)) {
        return -2;
    } else {
        return nRc;
    }
}

void CExtendedComboBox::OnSelendok()
{
    GetWindowText(m_strSavedText);
    PostMessage(nMessage);
}

LRESULT CExtendedComboBox::OnRealSelEndOK(WPARAM,LPARAM)
{
    CString currentText;
    GetWindowText(currentText);

    int nIndex = FindStringExact(-1,currentText);
    if (nIndex>=0 && !IsItemEnabled(nIndex)) {
        SetWindowText(m_strSavedText);
        GetParent()->SendMessage(WM_COMMAND,MAKELONG(GetWindowLong(m_hWnd,GWL_ID),CBN_SELCHANGE),(LPARAM)m_hWnd);
    }

    return 0;
}

LRESULT CExtendedComboBox::OnCtlColor(WPARAM,LPARAM lParam)
{
    if (m_listBox.m_hWnd == NULL && lParam != 0 && lParam != (LPARAM)m_hWnd) {
        m_listBox.SubclassWindow((HWND)lParam);
    }

    return Default();
}


void CExtendedComboBox::PostNcDestroy()
{
    //  m_listBox.UnsubclassWindow();
    m_listBox.Detach();
    CComboBox::PostNcDestroy();
}



/////////////////////////////////////////////////////////////////////////////
// CListBoxInsideComboBox

CListBoxInsideComboBox::CListBoxInsideComboBox()
{
    m_pParent = NULL;
}

CListBoxInsideComboBox::~CListBoxInsideComboBox()
{
}


BEGIN_MESSAGE_MAP(CListBoxInsideComboBox, CWnd)
//{{AFX_MSG_MAP(CListBoxInsideComboBox)
ON_WM_LBUTTONUP()
ON_WM_CREATE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CListBoxInsideComboBox message handlers

void CListBoxInsideComboBox::OnLButtonUp(UINT nFlags, CPoint point)
{
    CRect rect;
    GetClientRect(rect);

    if (rect.PtInRect(point)) {
        BOOL bOutside = FALSE;
        int nIndex = ((CListBox *)this)->ItemFromPoint(point, bOutside);
        if (!bOutside && !m_pParent->IsItemEnabled(nIndex)) {
            return;     // don't click there
        }
    }

    CWnd::OnLButtonUp(nFlags, point);
}

void CListBoxInsideComboBox::SetParent(CExtendedComboBox *ptr)
{
    m_pParent = ptr;
}

int CListBoxInsideComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    lpCreateStruct->style |= LBS_WANTKEYBOARDINPUT;

    return 0;
}

void CExtendedComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    int nIndex = GetCurSel();
    if (nChar == VK_DOWN) {
        if (nIndex+1 < GetCount()) {
            if (!IsItemEnabled(nIndex+1)) {
                for (int i = nIndex+2; i < GetCount(); i++) {
                    if (IsItemEnabled(i)) {
                        SetCurSel(i);
                        break;
                    }
                }
                return;
            }
        }
    }
    if (nChar == VK_UP) {
        if (nIndex-1 >= 0) {
            if (!IsItemEnabled(nIndex-1)) {
                for (int i = nIndex-2; i >= 0; i--) {
                    if (IsItemEnabled(i)) {
                        SetCurSel(i);
                        break;
                    }
                }
                return;
            }
        }
    }
    if (nChar == VK_HOME || nChar == VK_END
        || nChar == VK_PRIOR || nChar == VK_NEXT )
        {
            return;
        }

    CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

