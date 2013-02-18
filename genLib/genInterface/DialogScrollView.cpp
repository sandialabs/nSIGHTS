///////////////////////////////////////////////////////////////////////////////////
//
// DialogScrollView.cpp
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
// DESCRIPTION: Windows specific code for implementing Outlook style
//              application with dialogs and object trees.
//
///////////////////////////////////////////////////////////////////////////////////

// DialogScrollView.cpp : implementation file
//

#include "GenLibAfx.h"

#include "DialogScrollView.h"
#include <genInterface/GenericDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogScrollView

IMPLEMENT_DYNCREATE(CDialogScrollView, CScrollView)

CDialogScrollView::CDialogScrollView()
{
}

CDialogScrollView::~CDialogScrollView()
{
}


BEGIN_MESSAGE_MAP(CDialogScrollView, CScrollView)
//{{AFX_MSG_MAP(CDialogScrollView)
ON_WM_SIZE()
ON_WM_ERASEBKGND()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogScrollView drawing

void CDialogScrollView::OnInitialUpdate()
{
    CScrollView::OnInitialUpdate();
    SetScrollSizes(MM_TEXT, CSize(100,100));
    CDialog* pDlg = GetDialog();
    if (pDlg != NULL) {
        static_cast<CGenericDialog*>(pDlg)->InitData(false);
    }
}

void CDialogScrollView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CDialogScrollView diagnostics

#ifdef _DEBUG
void CDialogScrollView::AssertValid() const
{
    CScrollView::AssertValid();
}

void CDialogScrollView::Dump(CDumpContext& dc) const
{
    CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDialogScrollView message handlers

// a) Child dialog resize
// b) Scroll bars handling at resize
void CDialogScrollView::OnSize(UINT nType, int cx, int cy)
{
    if (nType == SIZE_MINIMIZED) {
        return;
    }
    CDialog* pDlg = GetDialog();
    if (pDlg != NULL) {
        pDlg->SetWindowPos(NULL, 0, 0, cx, cy,
                           SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
        CRect rect;
        GetClientRect(&rect);
        CRect totalRect;
        pDlg->GetClientRect(totalRect);
        SetScrollSizes(MM_TEXT, totalRect.Size(), rect.Size());
    }
    CScrollView::OnSize(nType, cx, cy);
}

// We change style for better look
BOOL CDialogScrollView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= WS_BORDER|WS_CAPTION;
    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    return CScrollView::PreCreateWindow(cs);
}

void CDialogScrollView::PostNcDestroy()
{
    // I don't want CView to be deleted (see CView::PostNcDestroy)
}

BOOL CDialogScrollView::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

