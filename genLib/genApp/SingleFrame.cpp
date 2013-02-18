///////////////////////////////////////////////////////////////////////////////////
//
// SingleFrame.cpp
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              ProjectFrame derived class for applications with a single menu tree (like nPost)
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genApp/GenAppMessages.h>
#include <genApp/C_AppMenu.h>
#include <genApp/ProjectUtil.h>
#include <genMFC/ProjectView.h>
#include <genMFC/ProjectTreeView.h>

#include <genApp/SingleFrame.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int MENUTREE_VIEW_WIDTH = 150;


IMPLEMENT_DYNCREATE(CSingleFrame, CProjectFrame)

BEGIN_MESSAGE_MAP(CSingleFrame, CProjectFrame)
//{{AFX_MSG_MAP(CSingleFrame)
ON_WM_CREATE()
//}}AFX_MSG_MAP

END_MESSAGE_MAP()


FormContainer* CSingleFrame::GetFormContainer()
{

    CSingleFrame* pFrame = static_cast<CSingleFrame*>(AfxGetMainWnd());
    VERIFY(pFrame);

    return static_cast<FormContainer*>(pFrame->GetFormView());
}


CSingleFrame::CSingleFrame() : frameRoot(0)
{
}

CSingleFrame::~CSingleFrame()
{
}


BOOL CSingleFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.cx = 580;
    cs.cy = 480;
    if (!CProjectFrame::PreCreateWindow(cs))
        return FALSE;

    return TRUE;
}


int CSingleFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CProjectFrame::OnCreate(lpCreateStruct) == -1)
        return -1;

    // set the toolbar to be dockable
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);

    // add object and page menu support
    AppMenuC::CreateObjectMenu();

    // SetupUI
    CWnd* pTreeWnd = m_wndSplitter.GetPane(0, 0);
    frameRoot->SetupUI(pTreeWnd);
    frameRoot->SelectRoot();

    CRect rect;
    pTreeWnd->GetClientRect(rect);
    pTreeWnd->SendMessage(WM_SIZE, 0, MAKELPARAM(rect.Width(), rect.Height()));


    return 0;
}

//////////////////
// Client window created: Set up splitter window and save
// pointers to the two panes
//
BOOL CSingleFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pcc)
{
    // create splitter window
    if (!m_wndSplitter.CreateStatic(this, 1, 2))
        return FALSE;

    // Create left and right pane/views.
    //
    CCreateContextProject cc = *pcc;
    cc.m_pProjectRoot = frameRoot;

    CSplitterWnd& sw = m_wndSplitter;
    if (!sw.CreateView(0, 0,
                       RUNTIME_CLASS(CProjectTreeView),
                       CSize(MENUTREE_VIEW_WIDTH, 200),
                       &cc))
        {
            TRACE("***CSingleFrame: failed to create splitter views LEFT\n");
            sw.DestroyWindow();
            return FALSE;
        }
    if (!sw.CreateView(0, 1,
                       RUNTIME_CLASS(CProjectView),
                       CSize(0,0), // will get fixed in CSingleFrame::OnCreate
                       pcc) )
        {
            TRACE("***CSingleFrame: failed to create splitter views RIGHT\n");
            sw.DestroyWindow();
            return FALSE;
        }
    // Double-check for ON_NOTIFY in message map above
    VERIFY(AFX_IDW_PANE_FIRST == sw.IdFromRowCol(0,0));

    // display application name in Outlook style info bar
    m_wndInfoBar.Create(NULL, NULL, WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|CBRS_TOP,
                        CRect(0,0,0,0), this, AFX_IDW_STATUS_BAR+3);
    m_wndInfoBar.SetBarStyle(CBRS_ALIGN_TOP);
    m_wndInfoBar.SetText(AfxGetAppName());

    return TRUE;
}

//////////////////
// Route commands to splitter window.
//
BOOL CSingleFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra,
                            AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // Pass to splitter
    if (m_wndSplitter.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
        return TRUE;

    // Call default routing--very important!
    return CProjectFrame::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


//////////////////
// Calculate the size of the total frame, given a desired client (form) size.
// Start with client rect and add all the extra stuff.
//
void CSingleFrame::CalcWindowRect(PRECT prcClient, UINT nAdjustType)
{
    CRect rect(0, 0, 32767, 32767);
    RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery,
                   &rect, &rect, FALSE);

    CSize szBorder   = m_wndSplitter.GetBorderSize();
    CSize szSplitter = m_wndSplitter.GetSplitterSize();

    prcClient->bottom += rect.Height() +        // total toolbar height  +
        GetSystemMetrics(SM_CYMENU) +           // menu +
        GetSystemMetrics(SM_CYCAPTION) +        // title bar +
        2 * GetSystemMetrics(SM_CYSIZEFRAME) +  // top/bottom window frame +
        2 * szBorder.cy;                        // splitter borders

    prcClient->right += MENUTREE_VIEW_WIDTH +   // left pane width
        2 * GetSystemMetrics(SM_CXSIZEFRAME) +  // L/R window border
        4 * szBorder.cx +                       // 2 panes==> 4 borders
        szSplitter.cx;                          // 1 splitter
}

CProjectView*  CSingleFrame::GetFormView()
{
    return static_cast<CProjectView*>(m_wndSplitter.GetPane(0, 1));
}

