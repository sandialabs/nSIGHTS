///////////////////////////////////////////////////////////////////////////////////
//
// OutlookFrame.cpp
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
//              ProjectFram derived class for applications with multiple
//              menu trees (like nPre)
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genInterface/InterfaceOutlook.h>
#include <genInterface/InterfaceCtrls.h>
#include <genInterface/InterfaceContext.h>
#include <genInterface/InterfacemView.h>

#include <genApp/GenAppMessages.h>
#include <genApp/C_AppMenu.h>
#include <genApp/C_MenuRoot.h>
#include <genApp/ProjectUtil.h>

#include <genClassPS/PS_FormContainer.h>

#include <genPlotClass/C_PlotDef.h>
#include <genListClassPS/C_ListDefPS.h>

#include <genInterface/OutlookFrame.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(COutlookFrame, CProjectFrame)

BEGIN_MESSAGE_MAP(COutlookFrame, CProjectFrame)
//{{AFX_MSG_MAP(COutlookFrame)
ON_WM_CREATE()
ON_COMMAND(ID_UIX_LARGEICON, OnUixLargeicon)
ON_UPDATE_COMMAND_UI(ID_UIX_LARGEICON, OnUpdateUixLargeicon)
ON_COMMAND(ID_UIX_SMALLICON, OnUixSmallicon)
ON_UPDATE_COMMAND_UI(ID_UIX_SMALLICON, OnUpdateUixSmallicon)
ON_WM_ACTIVATE()
ON_WM_SYSCOMMAND()
//}}AFX_MSG_MAP

// mView messages
ON_MESSAGE(WM_GENAPP_PROPMENU, OnMViewOpenPropMenu)


END_MESSAGE_MAP()


FormContainer* COutlookFrame::GetFormContainer()
{
    COutlookFrame* pFrame = static_cast<COutlookFrame*>(AfxGetMainWnd());
    VERIFY(pFrame);
    FolderNode* pFolderNode = static_cast<FolderNode*>(pFrame->m_pInterface->GetOutlookPane()->GetNode(0));
    InterfaceNode* pInterfaceNode = pFolderNode->GetSelNode();
    FolderNode* pFolderNode2 = dynamic_cast<FolderNode*>(pInterfaceNode);
    while (pFolderNode2 != NULL) {
        pInterfaceNode = pFolderNode2->GetSelNode();
        pFolderNode2 = dynamic_cast<FolderNode*>(pInterfaceNode);
    }

    if (dynamic_cast<MViewNode*>(pInterfaceNode) == NULL) {
        return NULL;
    }
    MViewNode* pNode = static_cast<MViewNode*>(pInterfaceNode);
    return static_cast<FormContainer*>(pNode->GetFormContainer());
}


COutlookFrame::COutlookFrame() :
    m_pInterface(NULL)
{
}

COutlookFrame::~COutlookFrame()
{
    DestroyInterface();
}


int     COutlookFrame::LoadToolbars()
{
    if (!m_wndOutlookToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
                                      | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
                                      CRect(0, 0, 0, 0),
                                      AFX_IDW_TOOLBAR + 1)
        || !m_wndOutlookToolBar.LoadToolBar(IDR_OUTBAR)
        )
        {
            TRACE0("Failed to create outlook toolbar\n");
            return -1;      // fail to create
        }
    m_wndOutlookToolBar.EnableDocking(CBRS_ALIGN_ANY);
    m_menuSpawn.AddToolBarResource(IDR_OUTBAR);

    return CProjectFrame::LoadToolbars();
}

int COutlookFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CProjectFrame::OnCreate(lpCreateStruct) == -1)
        return -1;

    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP);

    RecalcLayout();       // essential

    CRect rect;
    m_wndToolBar.GetWindowRect(&rect);
    rect.OffsetRect(1, 0);
    DockControlBar(&m_wndOutlookToolBar, AFX_IDW_DOCKBAR_TOP, &rect);


    if (!m_wndControlBar.Create(_T("Control Bar"), this, 10)) {
        TRACE0("Failed to create control bar\n");
        return false;
    }

    m_wndControlBar.SetBarStyle(m_wndControlBar.GetBarStyle()
                                | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

    m_wndControlBar.SetSCBStyle(m_wndControlBar.GetSCBStyle()
                                | SCBS_SHOWEDGES | SCBS_SIZECHILD);

    m_wndControlBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    DockControlBar(&m_wndControlBar, AFX_IDW_DOCKBAR_LEFT);

    CString sProfile(_T("BarState"));
    if (VerifyBarState(sProfile))
        {
            m_wndControlBar.LoadState(sProfile);
            LoadBarState(sProfile);
        }


    return 0;
}

BOOL COutlookFrame::DestroyWindow()
{
    CString sProfile(_T("BarState"));
    m_wndControlBar.SaveState(sProfile);
    SaveBarState(sProfile);

    // DestroyWindow will destroy all childs so let's tell it to Inteface.
    if (m_pInterface != NULL) {
        m_pInterface->ResetView();
    }

    return CProjectFrame::DestroyWindow();
}

bool COutlookFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
        {
            CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
            ASSERT(pInfo != NULL);
            int nDockedCount = (int) pInfo->m_arrBarID.GetSize();
            if (nDockedCount > 0)
                {
                    // dockbar
                    for (int j = 0; j < nDockedCount; j++)
                        {
                            UINT nID = (UINT) pInfo->m_arrBarID[j];
                            if (nID == 0)
                                continue; // row separator
                            if (nID > 0xFFFF)
                                nID &= 0xFFFF; // placeholder - get the ID
                            if (GetControlBar(nID) == NULL)
                                return false;
                        }
                }

            if (!pInfo->m_bFloating) // floating dockbars can be created later
                if (GetControlBar(pInfo->m_nBarID) == NULL)
                    return false; // invalid bar ID
        }

    return true;
}

void COutlookFrame::DestroyInterface()
{
    if (m_pInterface != NULL) {
        m_pViewActive = NULL; // OnSetFocus workaround
        m_pInterface->DestroyNode();
        delete m_pInterface;
        m_pInterface = NULL;
    }
}

BOOL COutlookFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CProjectFrame::PreCreateWindow(cs))
        return FALSE;

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// COutlookFrame message handlers

void COutlookFrame::OnUixLargeicon()
{
    if (m_pInterface->GetOutlookCtrl() != NULL) {
        m_pInterface->GetOutlookCtrl()->SetSmallIconView(false);
    }
    projectUtilities::SetOutbarIconSize(false);
    ShowControlBar(&m_wndControlBar, true, FALSE);
}

void COutlookFrame::OnUpdateUixLargeicon(CCmdUI* pCmdUI)
{
    if (m_pInterface->GetOutlookCtrl() == NULL) {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->SetCheck(!m_pInterface->GetOutlookCtrl()->IsSmallIconView());
}

void COutlookFrame::OnUixSmallicon()
{
    if (m_pInterface->GetOutlookCtrl() != NULL) {
        m_pInterface->GetOutlookCtrl()->SetSmallIconView(true);
    }
    projectUtilities::SetOutbarIconSize(true);
    ShowControlBar(&m_wndControlBar, true, FALSE);
}

void COutlookFrame::OnUpdateUixSmallicon(CCmdUI* pCmdUI)
{
    if (m_pInterface->GetOutlookCtrl() == NULL) {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->SetCheck(m_pInterface->GetOutlookCtrl()->IsSmallIconView());
}

BOOL COutlookFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
    m_wndInfoBar.Create(NULL, NULL, WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|CBRS_TOP,
                        CRect(0,0,0,0), this, AFX_IDW_STATUS_BAR+1);
    m_wndInfoBar.SetBarStyle(CBRS_ALIGN_TOP);

    return TRUE;
}

void COutlookFrame::ResetInterface(CDocument* pDocument, bool bRestoreContext)
{
    if (m_pInterface != NULL) {
        // Destroy Interface
        m_pViewActive = NULL; // OnSetFocus workaround
        m_pInterface->DestroyNode();
        delete m_pInterface;
        m_pInterface = NULL;
    }

    // Create Interface
    CreateInterface();
    m_pInterface->CreateNode(this, pDocument);

    if (!bRestoreContext) {
        m_pInterface->ClearContext();
    }
    m_pInterface->RestoreContext(false);

    m_pInterface->CheckRestrictions();

    if (projectUtilities::OutbarIconsAreSmall())
        OnUixSmallicon();
    else
        OnUixLargeicon();

    RecalcLayout();

    m_wndControlBar.SendMessage(WM_NCPAINT);
    CRect rect;
    m_wndControlBar.GetClientRect(rect);
    m_wndControlBar.SendMessage(WM_SIZE, 0, MAKELPARAM(rect.Width(), rect.Height()));

}

void COutlookFrame::CreateInterface()
{
    FolderNode* initFolder = static_cast<FolderNode*>(m_pInterface->SelectNode(0));
    initFolder->SelectNode(0);
}


LRESULT COutlookFrame::OnMViewOpenPropMenu(WPARAM wParam, LPARAM lParam)
{
    LPCSTR szText = LPCSTR(wParam);
    CString sText;
    sText.Format("%s - %s", m_pInterface->GetInfoText(), szText);
    m_wndInfoBar.SetText(sText);

    return 0;
}

void COutlookFrame::OnViewControlBar()
{
    BOOL bShow = m_wndControlBar.IsVisible();
    ShowControlBar(&m_wndControlBar, !bShow, FALSE);
}

void COutlookFrame::OnUpdateViewControlBar(CCmdUI* pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(m_wndControlBar.IsVisible());
}


void COutlookFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CProjectFrame::OnActivate(nState, pWndOther, bMinimized);
}

void COutlookFrame::OnSysCommand(UINT nID, LONG_PTR lParam)
{
    // main purpose: minimized during simulation run
    if (nID == SC_RESTORE) {
        ShowWindow(SW_RESTORE);
    }
    CFrameWnd::OnSysCommand(nID, lParam);
}



void COutlookFrame::ResetRoots()
{
    MenuRootC::CloseAllUI();

    // if this isn't called the other roots will appear blank
    // eg. after AutoField -- sequence Plot/Process is blank

    ResetInterface(Interface::GetInterface()->GetDocument(), false);
}



// thread sync

HANDLE COutlookFrame::replotThreadEvent;  // for thread sync of replot
HWND   COutlookFrame::replotWnd;


void COutlookFrame::StartReplotProcessing()
{
    replotThreadEvent = NULL;
    replotWnd = AfxGetMainWnd()->m_hWnd;
}

void COutlookFrame::DoReplot(const char* appID)
{
    if (replotThreadEvent == NULL)
        {
            long hInst = long(AfxGetInstanceHandle());
            CString sEvent;
            sEvent.Format("%s%ld",appID, hInst);
            replotThreadEvent = ::CreateEvent(NULL, FALSE, FALSE, sEvent);
        }

    ::SendNotifyMessage(replotWnd, WM_REPLOT, NULL, NULL);

    // don't resume until replot is complete
    ::WaitForSingleObject(replotThreadEvent, INFINITE);
}


void COutlookFrame::EndReplotProcessing()
{
    CloseHandle(replotThreadEvent);
    replotThreadEvent = NULL;
    replotWnd = NULL;
}


// Called from diffrent thread through PostMessage/SendNotifyMessage call
//
// !!! parametress WPARAM, LPARAM must defined, otherwise VC++ crashes
// !!! just in Release version, but not Debug !!!
// !!! And compiler in ON_MESSAGE macro DOES NOT complain !!!
//
LRESULT COutlookFrame::OnReplot(WPARAM wParam, LPARAM lParam)
{
    PlotDefC::ReplotAfterCalc();

    ::SetEvent(replotThreadEvent);

    return 0;
}

