///////////////////////////////////////////////////////////////////////////////////
//
// ProjectSplitterWnd.cpp
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

#include <afxpriv.h>

#include "ProjectSplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CProjectSplitterWnd, CSplitterWnd)

BEGIN_MESSAGE_MAP(CProjectSplitterWnd, CSplitterWnd)
    //{{AFX_MSG_MAP(CProjectSplitterWnd)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectSplitterWnd construction/destruction

CProjectSplitterWnd::CProjectSplitterWnd()
{
}

CProjectSplitterWnd::~CProjectSplitterWnd()
{
}
//////////////////
// This function changes the view in a splitter window
//
void CProjectSplitterWnd::ChangeView(int row, int col, CRuntimeClass* pViewClass)
{
    ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CView)));

    CView* pView = STATIC_DOWNCAST(CView, GetPane(row, col));
    ASSERT(pView);
    CFrameWnd* pFrame = pView->GetParentFrame();
    ASSERT(pFrame);

    // just in case there is not a pane with a valid context we 
    // create one. if you want to risk it pass CreateView a NULL
    // context.
    CCreateContext cc;
    memset(&cc, 0, sizeof(cc));
    cc.m_pNewViewClass = pViewClass;
    cc.m_pCurrentDoc = pView->GetDocument();
    cc.m_pNewDocTemplate = cc.m_pCurrentDoc ?
        cc.m_pCurrentDoc->GetDocTemplate() : NULL;
    cc.m_pCurrentFrame = pFrame;

    DeleteView(row, col);
    // create the new form view
    VERIFY(CreateView(row, col, pViewClass, CSize(0,0), &cc));

    // must call because of bogus CSize(0,0) above
    RecalcLayout();

    // initialize the view
    CWnd* pWnd = GetPane(row, col);
    if (pWnd) {
        pWnd->SendMessage(WM_INITIALUPDATE);
    }
}

//////////////////
// Splitter wnd function to route commands to all splitter panes/views.
// This lets all panes, not just the active pane, handle messages.
//
BOOL CProjectSplitterWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra,
    AFX_CMDHANDLERINFO* pHandlerInfo)
{
    for (int row=0; row<GetRowCount(); row++) {
        for (int col=0; col<GetColumnCount(); col++) {
            if (GetPane(row,col)->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
                return TRUE;
        }
    }
    // Call default routing--very important!
    return CSplitterWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

