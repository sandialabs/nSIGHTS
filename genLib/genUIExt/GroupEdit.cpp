///////////////////////////////////////////////////////////////////////////////////
//
// GroupEdit.cpp
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

#include "GroupEdit.h"
#include "OutBarCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupEdit

CGroupEdit::CGroupEdit() :
    bEscapeKey( FALSE ),
    iIndex( -1 ),
    msgSend( NM_OB_ONGROUPENDEDIT ),
    bNoDown( false )
{
}

CGroupEdit::~CGroupEdit()
{
}


BEGIN_MESSAGE_MAP(CGroupEdit, CEdit)
    //{{AFX_MSG_MAP(CGroupEdit)
    ON_WM_KILLFOCUS()
    ON_WM_CREATE()
    ON_WM_CHAR()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupEdit message handlers

void CGroupEdit::OnKillFocus(CWnd* pNewWnd)
{
    PostMessage(WM_CLOSE, 0, 0);
    if (!bEscapeKey) {
        GetWindowText(text);
        if ( text != "" ) {
            GetOwner()->SendMessage(WM_OUTBAR_NOTIFY, msgSend, (LPARAM) this);
        }
    }
}

BOOL CGroupEdit::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->wParam == VK_RETURN) {
        PostMessage(WM_CLOSE, 0, 0);
        return TRUE;
    } else if (pMsg->wParam == VK_ESCAPE) {
        PostMessage(WM_CLOSE, 0, 0);
        return bEscapeKey = TRUE;
    }

    return CEdit::PreTranslateMessage(pMsg);
}

void CGroupEdit::PostNcDestroy()
{
    CEdit::PostNcDestroy();
    delete this;
}

int CGroupEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CEdit::OnCreate(lpCreateStruct) == -1)
        return -1;

    SendMessage(WM_SETFONT,(WPARAM) GetStockObject(DEFAULT_GUI_FONT),MAKELPARAM(TRUE,0));
    return 0;
}

void CGroupEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (msgSend == NM_OB_ONGROUPENDEDIT) {
        CEdit::OnChar(nChar, nRepCnt, nFlags);
        return;
    }

    if (nChar == VK_ESCAPE || nChar == VK_RETURN) {
        if (nChar == VK_ESCAPE) {
            bEscapeKey = TRUE;
        }
        GetParent()->SetFocus();
        return;
    }
    CEdit::OnChar(nChar, nRepCnt, nFlags);

    CRect rect,
    GetClientRect(&rect);
    CRect rectParent;
    GetParent()->GetClientRect(&rectParent);
    ClientToScreen(&rect);
    GetParent()->ScreenToClient(&rect);
    CString str;
    GetWindowText(str);
    CWindowDC dc(this);
    CFont* pFont = GetParent()->GetFont();
    CFont* pFontDC = dc.SelectObject(pFont);
    CRect szRect(rect);
    szRect.bottom = szRect.top;

    if (bNoDown) {
        dc.DrawText(str, szRect, DT_CALCRECT);
        if (szRect.right >= rectParent.right - 1) {
            rect.right = rectParent.right - 1;
        } else {
            rect.right = szRect.right;
        }
        MoveWindow(&rect);
        return;
    }

    dc.DrawText(str, szRect, DT_WORDBREAK|DT_CENTER|DT_CALCRECT);
    dc.SelectObject(pFontDC);
    CSize size = szRect.Size();

    if (size.cx > rect.Width()) {
        if (size.cx + rect.left < rectParent.right) {
            rect.right = rect.left + size.cx;
        } else {
            rect.right = rectParent.right;
        }
        MoveWindow(&rect);
    } else if (size.cy > rect.Height()) {
        if (size.cy + rect.bottom < rectParent.bottom) {
            rect.bottom = rect.top + size.cy;
        } else {
            rect.bottom = rectParent.bottom;
        }
        MoveWindow(&rect);
    }
}

