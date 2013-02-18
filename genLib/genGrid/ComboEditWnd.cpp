///////////////////////////////////////////////////////////////////////////////////
//
// ComboEditWnd.cpp
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
//              Edit control for combo box
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "ComboEditWnd.h"

#include "GridCtrlCell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboEdit

CComboEdit::CComboEdit()
{
}

CComboEdit::~CComboEdit()
{
}

// Stoopid win95 accelerator key problem workaround - Matt Weagle.
BOOL CComboEdit::PreTranslateMessage(MSG* pMsg)
{
    // Make sure that the keystrokes continue to the appropriate handlers
    if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) {
        ::TranslateMessage(pMsg);
        ::DispatchMessage(pMsg);
        return TRUE;
    }

    // Catch the Alt key so we don't choke if focus is going to an owner drawn button
    if (pMsg->message == WM_SYSCHAR) {
        return TRUE;
    }

    return CEdit::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CComboEdit, CEdit)
    //{{AFX_MSG_MAP(CComboEdit)
    ON_WM_KILLFOCUS()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboEdit message handlers

void CComboEdit::OnKillFocus(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);

    CComboEditWnd* pOwner = (CComboEditWnd*)GetOwner();  // This MUST be a CComboEditWnd
    if (pOwner) {
        pOwner->EndEdit();
    }
}

void CComboEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
         nChar == VK_DOWN  || nChar == VK_UP   ||
         nChar == VK_RIGHT || nChar == VK_LEFT) &&
        (GetKeyState(VK_CONTROL) < 0 && GetDlgCtrlID() == IDC_COMBOEDIT))
    {
        CWnd* pOwner = GetOwner();
        if (pOwner) {
            pOwner->SendMessage(WM_KEYDOWN, nChar, nRepCnt+ (((DWORD)nFlags)<<16));
        }
        return;
    }

    CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CComboEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_TAB || nChar == VK_ESCAPE) {
        CWnd* pOwner = GetOwner();
        if (pOwner) {
            pOwner->SendMessage(WM_KEYUP, nChar, nRepCnt + (((DWORD)nFlags)<<16));
        }
        return;
    }
    if (nChar == VK_RETURN) {
        CWnd* pOwner = GetOwner();
        if (pOwner) {
            pOwner->GetParent()->SetFocus();    // This will destroy this window
        }
    }

    CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}


/////////////////////////////////////////////////////////////////////////////
// CComboEditWnd

CComboEditWnd::CComboEditWnd(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID,
                           int nRow, int nColumn,
                           CStringArray& Items,
                           CString sInitText,
                           UINT nFirstChar)
{
    m_nNumLines = (int) Items.GetSize();
    if (m_nNumLines > 10)
        m_nNumLines = 10;

    m_sInitText = sInitText;
    m_nRow      = nRow;
    m_nCol      = nColumn;
    m_nLastChar = 0;
    m_bExitOnArrows = false; //(nFirstChar != VK_LBUTTON);  // If mouse click brought us here,

    // Create the combobox
    DWORD dwComboStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL
                         | CBS_AUTOHSCROLL|CBS_OWNERDRAWFIXED|CBS_HASSTRINGS
                         | dwStyle;
    int nHeight = rect.Height();
    rect.bottom = rect.bottom + m_nNumLines * nHeight + ::GetSystemMetrics(SM_CYHSCROLL);
    if (!Create(dwComboStyle, rect, pParent, nID)) {
        return;
    }

    // Add the strings
    for (int i = 0; i < Items.GetSize(); i++) {
        AddString(Items[i]);
    }

    // Get the maximum width of the text strings
    int nMaxLength = 0;
    CClientDC dc(GetParent());
    CFont* pOldFont = dc.SelectObject(pParent->GetFont());

    for (int i = 0; i < Items.GetSize(); i++) {
        nMaxLength = max(nMaxLength, dc.GetTextExtent(Items[i]).cx);
    }

    nMaxLength += (::GetSystemMetrics(SM_CXVSCROLL) + dc.GetTextExtent(_T(" ")).cx*2);
    dc.SelectObject(pOldFont);

    if (nMaxLength > rect.Width()) {
        rect.right = rect.left + nMaxLength;
    }

    // Resize the edit window and the drop down window
    MoveWindow(rect);

    SetFont(pParent->GetFont());
    SetItemHeight(-1, nHeight);

    SetDroppedWidth(nMaxLength);
    SetHorizontalExtent(0); // no horz scrolling

    // Set the initial text to m_sInitText
    if (SelectString(-1, m_sInitText) == CB_ERR) {
        SetWindowText(m_sInitText);         // No text selected, so restore what was there before
    }

    // Subclass the combobox edit control if style includes CBS_DROPDOWN
    if ((dwStyle & CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST) {
        m_edit.SubclassDlgItem(IDC_COMBOEDIT, this);
        SetFocus();
        switch (nFirstChar) {
            case VK_LBUTTON:
            case VK_RETURN:         m_edit.SetSel((int)_tcslen(m_sInitText), -1);
                                return;
            case VK_BACK:       m_edit.SetSel((int)_tcslen(m_sInitText), -1); break;
            case VK_DOWN:
            case VK_UP:
            case VK_RIGHT:
            case VK_LEFT:
            case VK_NEXT:
            case VK_PRIOR:
            case VK_HOME:
            case VK_END:        m_edit.SetSel(0,-1); return;
            default:            m_edit.SetSel(0,-1);
        }
        SendMessage(WM_CHAR, nFirstChar);
    } else {
        SetFocus();
        if (nFirstChar == VK_RETURN) {
            return;
        }
        SendMessage(WM_CHAR, nFirstChar);
    }
}

CComboEditWnd::~CComboEditWnd()
{
}

void CComboEditWnd::EndEdit()
{
    CString s;
    GetWindowText(s);

    // Send Notification to parent
    GRID_DISPINFO dispInfo;
    dispInfo.hdr.hwndFrom = GetSafeHwnd();
    dispInfo.hdr.idFrom   = GetDlgCtrlID();
    dispInfo.hdr.code     = 1;

    dispInfo.m_Row = m_nRow;
    dispInfo.m_nCol = m_nCol;
    dispInfo.m_nLastChar = m_nLastChar;
    dispInfo.m_szText = s;
    dispInfo.m_nValue = GetCurSel();
    // kluge - there must be a valid cur sel
    if (dispInfo.m_nValue < 0)
        dispInfo.m_nValue = 0;

    ShowWindow(SW_HIDE);
    CWnd* pOwner = GetOwner();
    if (::IsWindow(pOwner->GetSafeHwnd())) {
        pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), LPARAM(&dispInfo) );
    }

    // Close this window (PostNcDestroy will delete this)
    PostMessage(WM_CLOSE, 0, 0);
}

void CComboEditWnd::PostNcDestroy()
{
    CExtendedComboBox::PostNcDestroy();

    delete this;
}

bool CComboEditWnd::CanClose()
{
    return true;
}

BEGIN_MESSAGE_MAP(CComboEditWnd, CExtendedComboBox)
    //{{AFX_MSG_MAP(CComboEditWnd)
    ON_WM_KILLFOCUS()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComboEditWnd message handlers

void CComboEditWnd::OnKillFocus(CWnd* pNewWnd)
{
    CComboBox::OnKillFocus(pNewWnd);

    if (GetSafeHwnd() == pNewWnd->GetSafeHwnd()) {
        return;
    }

    // Only end editing on change of focus if we're using the CBS_DROPDOWNLIST style
    if ((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST) {
        EndEdit();
    }
}

// If an arrow key (or associated) is pressed, then exit if
//  a) The Ctrl key was down, or
//  b) m_bExitOnArrows == true
void CComboEditWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
         nChar == VK_DOWN  || nChar == VK_UP   ||
         nChar == VK_RIGHT || nChar == VK_LEFT) &&
        (m_bExitOnArrows || GetKeyState(VK_CONTROL) < 0))
    {
        m_nLastChar = nChar;
        GetParent()->SetFocus();
        return;
    }
    if (nChar == VK_RETURN) {
        m_nLastChar = nChar;
    }

    CExtendedComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

// Need to keep a lookout for Tabs, Esc and Returns.
void CComboEditWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_ESCAPE) {
        SetWindowText(m_sInitText);     // restore previous text
    }
    if (nChar == VK_RETURN && m_nLastChar != VK_RETURN) {
        return;
    }

    if (nChar == VK_TAB || nChar == VK_ESCAPE || nChar == VK_RETURN) {
        m_nLastChar = nChar;
        GetParent()->SetFocus();    // This will destroy this window
        return;
    }

    CExtendedComboBox::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CComboEditWnd::OnSelchange()
{
    int nIndex = GetCurSel();
    if (nIndex < 0)
        nIndex = 0;

    CString s;
    GetLBText(nIndex, s);

    GRID_DISPINFO dispInfo;
    dispInfo.hdr.hwndFrom = GetSafeHwnd();
    dispInfo.hdr.idFrom   = GetDlgCtrlID();
    dispInfo.hdr.code     = 1;

    dispInfo.m_Row = m_nRow;
    dispInfo.m_nCol = m_nCol;
    dispInfo.m_nLastChar = 0;
    dispInfo.m_szText = s;
    dispInfo.m_nValue = GetCurSel();

    CWnd* pOwner = GetOwner();
    if (::IsWindow(pOwner->GetSafeHwnd())) {
        pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), LPARAM(&dispInfo) );
    }
}

