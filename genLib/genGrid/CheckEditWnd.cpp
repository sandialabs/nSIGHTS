///////////////////////////////////////////////////////////////////////////////////
//
// CheckEditWnd.cpp
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
//              Edit control for checkbox
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "CheckEditWnd.h"

#include "GridCtrlCell.h"
#include "CheckGridCell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckEditWnd

CCheckEditWnd::CCheckEditWnd(CWnd* pParent,
                             CRect& rect, DWORD dwStyle, UINT nID,
                             int nRow, int nCol,
                             UINT nFirstChar,
                             CCheckGridCell* pCell) :
    m_nRow(nRow),
    m_nCol(nCol),
    m_pCell(pCell)
{
    m_nLastChar = 0;

    DWORD dwEditStyle = WS_BORDER|WS_CHILD|dwStyle;
    if (!CreateEx(WS_EX_TRANSPARENT, NULL, NULL, dwEditStyle, rect, pParent, nID)) {
        return;
    }

    SetFont(pParent->GetFont());

    SetFocus();

    switch (nFirstChar){
    case VK_LBUTTON:
        nFirstChar = VK_SPACE; // send to control
        break;
    case VK_RETURN:
        return;
    case VK_BACK:
        break;
    case VK_TAB:
    case VK_DOWN:
    case VK_UP:
    case VK_RIGHT:
    case VK_LEFT:
    case VK_NEXT:
    case VK_PRIOR:
    case VK_HOME:
    case VK_END:
        return;
    case VK_SPACE: // send to control
        break;
    default:
        break;
    }

    SendMessage(WM_CHAR, nFirstChar);
}

CCheckEditWnd::~CCheckEditWnd()
{
}


BEGIN_MESSAGE_MAP(CCheckEditWnd, CWnd)
//{{AFX_MSG_MAP(CCheckEditWnd)
ON_WM_KEYDOWN()
ON_WM_CHAR()
ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
ON_WM_KILLFOCUS()
ON_WM_GETDLGCODE()
ON_WM_LBUTTONDOWN()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckEditWnd message handlers

void CCheckEditWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CWnd* pParent = GetParent();
    ASSERT(pParent != NULL);

    if (nChar == VK_UP || nChar == VK_DOWN) {
        m_nLastChar = nChar;
        GetParent()->SetFocus();
        return;
    } else {
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}

void CCheckEditWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_TAB || nChar == VK_RETURN) {
        m_nLastChar = nChar;
        GetParent()->SetFocus();    // This will destroy this window
        return;
    }
    if (nChar == VK_SPACE) {
        SwitchValue();
        return;
    }
    if (nChar == VK_ESCAPE) {
        m_nLastChar = nChar;
        GetParent()->SetFocus();
        return;
    }

    CWnd::OnChar(nChar, nRepCnt, nFlags);
}

bool CCheckEditWnd::CanClose()
{
    return true;
}

void CCheckEditWnd::EndEdit()
{
    GRID_DISPINFO dispInfo;

    dispInfo.hdr.hwndFrom = GetSafeHwnd();
    dispInfo.hdr.idFrom   = GetDlgCtrlID();
    dispInfo.hdr.code     = 1;

    dispInfo.m_Row = m_nRow;
    dispInfo.m_nCol = m_nCol;
    dispInfo.m_nLastChar = m_nLastChar;
    dispInfo.m_nValue = -1;

    ShowWindow(SW_HIDE);
    CWnd* pOwner = GetOwner();
    if (pOwner != NULL) {
        pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), LPARAM(&dispInfo));
    }
    // Close this window (PostNcDestroy will delete this)
    PostMessage(WM_CLOSE, 0, 0);
}

void CCheckEditWnd::OnKillfocus()
{
    EndEdit();
}

void CCheckEditWnd::OnKillFocus(CWnd* pNewWnd)
{
    CWnd::OnKillFocus(pNewWnd);
    EndEdit();
}

// Stupid win95 accelerator key problem workaround
BOOL CCheckEditWnd::PreTranslateMessage(MSG* pMsg)
{
    // Catch the Alt key so we don't choke if focus is going to an owner drawn button
    if (pMsg->message == WM_SYSCHAR) {
        return TRUE;
    }

    return CWnd::PreTranslateMessage(pMsg);
}

// Auto delete
void CCheckEditWnd::PostNcDestroy()
{
    CWnd::PostNcDestroy();

    delete this;
}

UINT CCheckEditWnd::OnGetDlgCode()
{
    return DLGC_WANTALLKEYS;
}

void CCheckEditWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    SwitchValue();
    CWnd::OnLButtonDown(nFlags, point);
}

void CCheckEditWnd::SwitchValue()
{
    m_pCell->SetValue(!m_pCell->GetValue());
    static_cast<CGridCtrlBase*>(GetParent())->InvalidateCell(m_nRow, m_nCol);
    GetParent()->UpdateWindow();
}

