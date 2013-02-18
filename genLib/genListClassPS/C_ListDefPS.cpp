///////////////////////////////////////////////////////////////////////////////////
//
// C_ListDefPS.cpp
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
//      creates top-level window with IE browser in client area.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClass/U_Str.h>
#include <genApp/U_Menu.h>

#include <genListClass/C_ListObj.h>
#include <genApp/GenAppMessages.h>
#include <genApp/G_Version.h>

#include <genUIExt/FileDialog.h>

#include <genListClassPS/C_ListDefPS.h>

#include <genListClassPS/ListHtmlView.h>

#include <mshtml.h>

ListDefCPS :: ListDefCPS(bool isMod) : ListDefC("ListingPage"), isModal(isMod),
    m_pBrowser(NULL), PS_MainWindow(*this, "List")
{
    InitCommon();
}

ListDefCPS::ListDefCPS(const char* title) :
    ListDefC(title), isModal(true),
    m_pBrowser(NULL), PS_MainWindow(*this, "ModList")
{
    InitCommon();
    ChangeTitle(title);
}


ListDefCPS :: ~ListDefCPS ()
{
}

ListDefCPS :: ListDefCPS(const ListDefCPS& a) :
    PS_MainWindow(*this, "List"), ListDefC(a), isModal(false), m_pBrowser(NULL)
{
    InitCommon();
}


void ListDefCPS::InitCommon()
{
    m_bOwnerCreated = false;

    UINT nMenuID = GetMenuResourceID();
    VERIFY(LoadFrame(nMenuID));

    int xBorder, yBorder;
    CalcBorderSize(xBorder, yBorder);
    VERIFY(SetWindowPos(&CWnd::wndTop, 0, 0,
                            xBorder + 800,
                            yBorder + 600,
                            SWP_NOMOVE));

    SetWindowText("Listing...");
    ShowWindow(SW_SHOW);

    if (isModal)
    {
        CMenu* pMenu = GetSystemMenu(FALSE);
        ASSERT(pMenu != NULL);
        pMenu->ModifyMenu(SC_MINIMIZE, MF_BYCOMMAND | MF_GRAYED);

        AfxGetMainWnd()->EnableWindow(false);
    }
}

ListDefCPS& ListDefCPS::operator= (const ListDefCPS& a)
{
    if (&a != this)
    {
        ListDefC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void  ListDefCPS::LocalCopy(const ListDefCPS& a)
{
}

void ListDefCPS::DoStatusChk()
{
    ListDefC::DoStatusChk();
    // any extra checks for viewer ??
}

void ListDefCPS::ModalView()
{
    SetActiveView(m_pBrowser);

    // for modal listings it is usually neccesary to create the listings
    int i;
    if (displayDefaults)
        for (i = 0; i < defaultListObj.Size(); i++)
            defaultListObj[i]->BuildListing();

    for (i = 0; i < connectedListObj.Size(); i++)
        connectedListObj[i]->BuildListing();

    // send the listings to the viewer
    Execute();

    RunModalLoop();
}

void ListDefCPS::SendToDisplay(const SC_StringArray& dispText)
{
    if (dispText.IsEmpty())
        return;

    LPDISPATCH lpDispatch = m_pBrowser->GetDocumentX();

    if (lpDispatch) {
        IHTMLDocument2* pHTMLDocument2;
        HRESULT hr = lpDispatch->QueryInterface(IID_IHTMLDocument2, (LPVOID*) &pHTMLDocument2);
        lpDispatch->Release();
        ASSERT(SUCCEEDED(hr));

        IHTMLElement*  pBody;
        HRESULT nhr = pHTMLDocument2->get_body(&pBody);

        if (FAILED(hr))
            return;

        ASSERT(pBody != NULL);

        CString outStr;
        for (int i = 0; i < dispText.Size(); i++)
            outStr += dispText[i];

        BSTR BstrTextToBeAdded = outStr.AllocSysString();

        pBody->insertAdjacentHTML(L"BeforeEnd", BstrTextToBeAdded);

        SysFreeString(BstrTextToBeAdded);

        pBody->Release();

        pHTMLDocument2->Release();
    }
}

void ListDefCPS::PumpMessages()
{
    ASSERT(m_hWnd != NULL);

    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (!IsDialogMessage(&msg)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

void ListDefCPS::ClearDisplay()
{
    ChangeTitle(GetID());

    m_pBrowser->m_bDocumentComplete = false;

    m_pBrowser->InitPrint();

    CString& header = m_pBrowser->m_sHeader;
    header = appVersionGlobals::versionID;
    header += " Listing";
    header += " &bPage &p of &P";

    CString& footer = m_pBrowser->m_sFooter;
    footer = appVersionGlobals::versionID;
    footer += "  Released:";
    footer += appVersionGlobals::versionDate;
    footer += "  ";
    footer += appVersionGlobals::versionQA;
    footer += " &b&d";

    m_pBrowser->Navigate("about:blank");

    do {
        PumpMessages();
        Sleep(50);
    } while (!m_pBrowser->m_bDocumentComplete);
}

void ListDefCPS::ChangeTitle(const char* newName)
{
    SetMainWindowTitle(newName);
}


BEGIN_MESSAGE_MAP(ListDefCPS, CFrameWnd)
    //{{AFX_MSG_MAP(ListDefCPS)
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_CLOSE()
    ON_WM_ACTIVATE()
    ON_COMMAND(ID_LIST_TOOLBAR, OnToolbar)
    ON_UPDATE_COMMAND_UI(ID_LIST_TOOLBAR, OnUpdateToolbar)
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_PREVIEW_SAVE_AS, OnPreviewSaveAs)
END_MESSAGE_MAP()

int ListDefCPS::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
            | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
        || !m_wndToolBar.LoadToolBar(IDR_LISTING_TOOLBAR))
    {
        TRACE0("Failed to create toolbar\n");
    }

    m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);

    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP);

    if (!isModal) {
        EnableCloseInSystemMenu(false);
    }

    return 0;
}

void ListDefCPS::OnClose()
{
    if (isModal) {
        AfxGetMainWnd()->EnableWindow(true);
        EndModalLoop(0);
    } else {
        MessageBox("Please close application from the main application window");
    }
}

BOOL ListDefCPS::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs)) {
        return FALSE;
    }

    if (!m_bOwnerCreated) {
        CRect rect;
        LPCTSTR lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
                                    ::LoadCursor(NULL, IDC_ARROW),
                                    HBRUSH(COLOR_BTNFACE+1),
                                    NULL);
        m_bOwnerCreated = m_wndOwnerWindow.CreateEx(0,
                                                lpszClass,
                                                "Hidden Owner",
                                                WS_POPUP,
                                                rect,
                                                NULL,
                                                0) != FALSE;
        if (m_bOwnerCreated) {
            m_wndOwnerWindow.ShowWindow(SW_HIDE);
        }
    }

    // set the hidden window as a parent of frame window
    if (m_bOwnerCreated) {
        cs.hwndParent = m_wndOwnerWindow.GetSafeHwnd();
    }
    return TRUE;
}

void ListDefCPS::PostNcDestroy()
{
    //DON'T delete this;

    // remove me from 'Window Selector' floating window
    MainWindowDeactivated();
}


void ListDefCPS::OnPaint()
{
    CFrameWnd::OnPaint();
}

void ListDefCPS::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);
}

void ListDefCPS::EnableCloseInSystemMenu(bool bEnable)
{
    CMenu* pMenu = GetSystemMenu(FALSE);
    ASSERT(pMenu != NULL);
    if (bEnable) {
        pMenu->ModifyMenu(SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);
    } else {
        pMenu->ModifyMenu(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
    }
}

void ListDefCPS::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE) {
        TRACE("Activate ListDefCPS %d\r\n", nState);
        MainWindowActivated();
    }
    CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
}

UINT ListDefCPS::GetMenuResourceID()
{
    return IDR_LIST;
}

void ListDefCPS::CalcBorderSize(int& nWidth, int& nHeight)
{
    nWidth = 2*(::GetSystemMetrics(SM_CXFRAME)+2*::GetSystemMetrics(SM_CXEDGE));

    nHeight = ::GetSystemMetrics(SM_CYMENU);
    nHeight += ::GetSystemMetrics(SM_CYCAPTION);
    nHeight += 2*(::GetSystemMetrics(SM_CYFRAME)+::GetSystemMetrics(SM_CYEDGE));
//  nHeight += CalcButtonBarHeight();
}


void ListDefCPS::BringListWindowToTop()
{
    if (!IsWindowVisible()) {
        ShowWindow(SW_SHOW);
    }
    BringWindowToTop();
    if (IsIconic()) {
        ShowWindow(SW_RESTORE);
    }
}

void ListDefCPS::SetFrameIcon(UINT nID)
{
    HICON hIcon = HICON(::LoadImage(AfxGetInstanceHandle(),
                                MAKEINTRESOURCE(nID),
                                IMAGE_ICON, 16, 16,
                                LR_SHARED));
    ASSERT(hIcon != NULL);
    SetIcon(hIcon, FALSE);
}

BOOL ListDefCPS::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // workaround of some weird behaviour in modal mode
    // (somethings sets ActiveView to NULL)
    CView* pView = GetActiveView();
    if (pView == NULL && m_pBrowser != NULL) {
        if (m_pBrowser->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
            return TRUE;
        }
    }

    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL ListDefCPS::OnEraseBkgnd(CDC* pDC)
{
    return CFrameWnd::OnEraseBkgnd(pDC);
}

BOOL ListDefCPS::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
    CCreateContext context;
    context.m_pNewViewClass = RUNTIME_CLASS(CListingHtmlView);
    context.m_pCurrentDoc = NULL;
    context.m_pNewDocTemplate = NULL;
    context.m_pLastView = NULL;
    context.m_pCurrentFrame = this;

    m_pBrowser = static_cast<CListingHtmlView*>(CreateView(&context, AFX_IDW_PANE_FIRST));

    if (m_pBrowser == NULL)
        return FALSE;

    return TRUE;
}

void ListDefCPS::OnToolbar()
{
    ShowControlBar(&m_wndToolBar, !m_wndToolBar.IsVisible(), TRUE);
    if (isModal) { // weird exception
        RecalcLayout();
    }
}

void ListDefCPS::OnUpdateToolbar(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_wndToolBar.IsVisible());
}

void ListDefCPS::OnPreviewSaveAs()
{
    bool isErrorType = false;

    CString sFileName;
    CString sExt = !isErrorType ? "lst" : "err";
    CString sFilter = !isErrorType ?
                        "LST Files (*.lst)|*.lst|All Files (*.*)|*.*|||" :
                        "ERR Files (*.err)|*.err|All Files (*.*)|*.*|||";
    CString sTitle = !isErrorType ? "Save listing" : "Save error listing";

    NTCompatibleFileDialog dlg(FALSE, sExt, sFileName,
                    OFN_HIDEREADONLY, sFilter);

    dlg.m_ofn.lpstrTitle = sTitle;

    if (dlg.DoModal() == IDOK)
    {
        sFileName = dlg.GetPathName();
        if (FileOverwriteOK(sFileName))
            ListToFile(sFileName);
    }

}



