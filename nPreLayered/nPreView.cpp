///////////////////////////////////////////////////////////////////////////////////
//
// nPreView.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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


// nPreView.cpp : implementation of the CNPreView class
//

#include "stdafx.h"
#include "Resource.h"

#include "nPreDoc.h"
#include "nPreView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNPreView

IMPLEMENT_DYNCREATE(CNPreView, CBasicView)

BEGIN_MESSAGE_MAP(CNPreView, CBasicView)
  //{{AFX_MSG_MAP(CNPreView)
  ON_WM_ERASEBKGND()
  ON_WM_SIZE()
  ON_WM_MOUSEACTIVATE()
  //}}AFX_MSG_MAP
  // Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNPreView construction/destruction

CNPreView::CNPreView() :
  m_pClient(NULL)
{
}

CNPreView::~CNPreView()
{
}

BOOL CNPreView::PreCreateWindow(CREATESTRUCT& cs)
{

  return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CNPreView drawing

void CNPreView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CNPreView printing

BOOL CNPreView::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
  return DoPreparePrinting(pInfo);
}

void CNPreView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{
}

void CNPreView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CNPreView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
}

void CNPreView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
}

/////////////////////////////////////////////////////////////////////////////
// CNPreView diagnostics

#ifdef _DEBUG
void CNPreView::AssertValid() const
{
  CView::AssertValid();
}

void CNPreView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

CNPreDoc* CNPreView::GetDocument()
{
  return (CNPreDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNPreView message handlers

BOOL CNPreView::OnEraseBkgnd(CDC* pDC)
{
  if (m_pClient == NULL) {
    COLORREF nBrushColor = ::GetSysColor(COLOR_3DFACE);
    CBrush brBackground;
    VERIFY( brBackground.CreateSolidBrush(nBrushColor) );
    CBrush* pOldBrush = (CBrush*)pDC->SelectObject(&brBackground);
    CRect rect;
    pDC->GetClipBox(&rect);
    pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
    pDC->SelectObject(pOldBrush);
  }
  return TRUE;
}

void CNPreView::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize(nType, cx, cy);
  if (nType != SIZE_MINIMIZED) {
    if (m_pClient != NULL) {
      CRect rect;
      GetClientRect(&rect);
      m_pClient->MoveWindow(rect);
    }
  }
}

void CNPreView::OnInitialUpdate()
{
  CView::OnInitialUpdate();
}

// CNPreView is just underlaying view which never is to be activated
int CNPreView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
  return MA_NOACTIVATE;
}



