///////////////////////////////////////////////////////////////////////////////////
//
// ProjectTreeView.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/C_TreeBase.h>
#include <genApp/C_MenuRoot.h>
#include <genHelp/H_Genlib.h>

#include "ProjectTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeView

IMPLEMENT_DYNCREATE(CProjectTreeView, CView)

BEGIN_MESSAGE_MAP(CProjectTreeView, CView)
    //{{AFX_MSG_MAP(CProjectTreeView)
    ON_WM_SIZE()
    ON_WM_HELPINFO()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeView construction/destruction

CProjectTreeView::CProjectTreeView() :
    projectRoot(NULL)
{
}

CProjectTreeView::~CProjectTreeView()
{
}

BOOL CProjectTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeView drawing

void CProjectTreeView::OnDraw(CDC* pDC)
{
}


/////////////////////////////////////////////////////////////////////////////
// CProjectTreeView printing

BOOL CProjectTreeView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CProjectTreeView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CProjectTreeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add cleanup after printing
}

void CProjectTreeView::OnInitialUpdate()
{
    CView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeView diagnostics

#ifdef _DEBUG
void CProjectTreeView::AssertValid() const
{
    CView::AssertValid();
}

void CProjectTreeView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeView message handlers

BOOL CProjectTreeView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
    VERIFY(CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext));

    projectRoot = static_cast<CCreateContextProject*>(pContext)->m_pProjectRoot;

    return TRUE;
}

void CProjectTreeView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    if (cy > 0 && cx > 0 && projectRoot != NULL && projectRoot->GetUI() != NULL) {
        projectRoot->GetUI()->Resize(0, 0, cx, cy);
    }
}


BOOL CProjectTreeView::OnHelpInfo(HELPINFO* pHelpInfo)
{
    if (pHelpInfo->iContextType == HELPINFO_WINDOW) {
        AfxGetApp()->WinHelp(IDH_OBJECT_TREE);
        return TRUE;
    }
    return CView::OnHelpInfo(pHelpInfo);
}

