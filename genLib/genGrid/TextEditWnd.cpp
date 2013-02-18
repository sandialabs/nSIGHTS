///////////////////////////////////////////////////////////////////////////////////
//
// TextEditWnd.cpp
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
//              Edit control for text entry
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "TextEditWnd.h"

#include "GridCtrlCell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextEditWnd

CTextEditWnd::CTextEditWnd(CWnd* pParent,
                           CRect& rect, DWORD dwStyle, UINT nID,
                           int nRow, int nCol,
                           UINT nFirstChar, const CString& sInitText) :
   m_nRow( nRow ),
   m_nCol( nCol )
{
    m_nLastChar = 0;

    DWORD dwEditStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|dwStyle;
    if (!Create(dwEditStyle, rect, pParent, nID))
        return;

    SetFont(pParent->GetFont());

    SetWindowText(sInitText);
    SetFocus();

    switch (nFirstChar){
        case VK_LBUTTON:
            SetSel(0, -1);
            return;
        case VK_RETURN:
            SetSel((int)_tcslen(sInitText), -1);
            return;
        case VK_BACK:
            SetSel((int)_tcslen(sInitText), -1);
            break;
        case VK_TAB:
        case VK_DOWN:
        case VK_UP:
        case VK_RIGHT:
        case VK_LEFT:
        case VK_NEXT:
        case VK_PRIOR:
        case VK_HOME:
        case VK_SPACE:
        case VK_END:
            SetSel(0,-1);
            return;
        default:
            SetSel(0,-1);
            break;
    }

    SendMessage(WM_CHAR, nFirstChar);
}

CTextEditWnd::~CTextEditWnd()
{
}


BEGIN_MESSAGE_MAP(CTextEditWnd, CEdit)
    //{{AFX_MSG_MAP(CTextEditWnd)
    ON_WM_KEYDOWN()
    ON_WM_CHAR()
    ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
    ON_WM_GETDLGCODE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextEditWnd message handlers

void CTextEditWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CWnd* pParent = GetParent();
    ASSERT(pParent != NULL);

    if (nChar == VK_UP || nChar == VK_DOWN) {
        m_nLastChar = nChar;
        GetParent()->SetFocus();
        return;
    } else {
        CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}

void CTextEditWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_F12) {
        m_nLastChar = nChar;
        GetParent()->SetFocus();    // This will destroy this window
        return;
    }
    if (nChar == VK_ESCAPE) {
        m_nLastChar = nChar;
        GetParent()->SetFocus();
        return;
    }

    CEdit::OnChar(nChar, nRepCnt, nFlags);
}

bool CTextEditWnd::CanClose()
{
    return true;
}

void CTextEditWnd::EndEdit()
{
    CString s;
    GetWindowText(s);

    GRID_DISPINFO dispInfo;

    dispInfo.hdr.hwndFrom = GetSafeHwnd();
    dispInfo.hdr.idFrom   = GetDlgCtrlID();
    dispInfo.hdr.code     = 1;

    dispInfo.m_Row = m_nRow;
    dispInfo.m_nCol = m_nCol;
    dispInfo.m_nLastChar = m_nLastChar;
    dispInfo.m_szText = s;
    dispInfo.m_nValue = -1;

    ShowWindow(SW_HIDE);
    CWnd* pOwner = GetOwner();
    if (pOwner != NULL) {
        pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), LPARAM(&dispInfo));
    }
    // Close this window (PostNcDestroy will delete this)
    PostMessage(WM_CLOSE, 0, 0);
}

void CTextEditWnd::OnKillfocus()
{
    EndEdit();
}

// Stupid win95 accelerator key problem workaround
BOOL CTextEditWnd::PreTranslateMessage(MSG* pMsg)
{
    // Catch the Alt key so we don't choke if focus is going to an owner drawn button
    if (pMsg->message == WM_SYSCHAR) {
        return TRUE;
    }

    // for proper F12 behaviour
    if (pMsg->wParam == VK_F12)
    {
        GetParent()->SetFocus();
    }

    return CWnd::PreTranslateMessage(pMsg);
}

// Auto delete
void CTextEditWnd::PostNcDestroy()
{
    CEdit::PostNcDestroy();

    delete this;
}

UINT CTextEditWnd::OnGetDlgCode()
{
    return DLGC_WANTALLKEYS;
}

