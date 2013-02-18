///////////////////////////////////////////////////////////////////////////////////
//
// ProjectFrame.cpp
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
//              Basic MFC application class.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <HtmlHelp.h>

#include <genApp/C_AppMenu.h>
#include <genApp/C_MenuRoot.h>
#include <genApp/C_TreeBase.h>
#include <genApp/AppCopyPaste.h>

#include <genApp/GenAppMessages.h>
#include <genApp/WindowSelector.h>
#include <genApp/ProjectUtil.h>
#include <genApp/PullMenu.h>
#include <genApp/G_Version.h>

#include <genClassPS/PS_OGLBase.h>

#include <genListClass/LFO_ObjectListing.h>
#include <genListClassPS/C_ListDefPS.h>

#include <genUIExt/TitlePopupMenu.h>

#include "Settings/SettingsPropSheet.h"
#include "FileNameUpdateDlg.h"

#include "ProjectFrame.h"



CBitmapMenuExclude::CBitmapMenuExclude(CProjectFrame& projectFrame) :
    projectFrame(projectFrame)
{
}

// exclude menu items which are created dynamically
bool CBitmapMenuExclude::ExcludeItem(UINT nItemID)
{
    return nItemID >= projectFrame.m_nPulldownMenuFrom
        && nItemID < projectFrame.m_nPulldownMenuTo;
}

IMPLEMENT_DYNCREATE(CProjectFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CProjectFrame, CFrameWnd)
//{{AFX_MSG_MAP(CProjectFrame)
ON_WM_CREATE()
//ON_WM_MENUSELECT()
ON_WM_INITMENU()
ON_WM_INITMENUPOPUP()
ON_WM_DRAWITEM()
ON_WM_MEASUREITEM()
ON_WM_ACTIVATE()
ON_WM_ACTIVATEAPP()
ON_WM_SYSCOMMAND()
//}}AFX_MSG_MAP

ON_MESSAGE(WM_MENUSELECT, OnMenuSelectInternal)
// mView messages
ON_MESSAGE(WM_GENAPP_NONE, OnMViewNone)
ON_MESSAGE(WM_GENAPP_INFO, OnMViewInfo)
ON_MESSAGE(WM_GENAPP_WARNING, OnMViewWarning)
ON_MESSAGE(WM_GENAPP_ERROR, OnMViewError)
ON_MESSAGE(WM_GENAPP_RANGE, OnMViewRange)
ON_MESSAGE(WM_GENAPP_PROPMENU, OnMViewOpenPropMenu)
ON_MESSAGE(WM_GENAPP_PLOTSETTINGS, OnMViewPlotSettings)
ON_MESSAGE(WM_GENAPP_FRAMEACTIVATE, OnMViewFrameActivate)
ON_MESSAGE(WM_GENAPP_FRAMERENAME, OnMViewFrameRename)

END_MESSAGE_MAP()


CProjectFrame::CProjectFrame() :
m_menuSpawn(*this),
    m_pPopWnd(NULL),
    mainToolbarID(-1),
    m_nTemplateMenuPos(-1)
{
    // required to set MB sensitivity
    m_bAutoMenuEnable = FALSE;
}

CProjectFrame::~CProjectFrame()
{
}

static UINT indicators[] =
    {
        ID_SEPARATOR,           // status line indicator
        ID_INDICATOR_CAPS,
        ID_INDICATOR_NUM,
        ID_INDICATOR_SCRL,
    };

int     CProjectFrame::LoadToolbars()
{
    if (!m_wndStatusBar.Create(this)
        || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
        {
            TRACE0("Failed to create status bar\n");
            return -1;      // fail to create
        }


    m_wndMViewMessagebar.Create(NULL, NULL, WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|CBRS_TOP,
                                CRect(0,0,0,0), this, AFX_IDW_STATUS_BAR+2);
    m_wndMViewMessagebar.SetBarStyle(CBRS_ALIGN_BOTTOM);
    m_wndMViewMessagebar.SetStatus(msNone, " ");

    return 0;
}


int CProjectFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    projectUtilities::LoadSettings();

    int nPFD, pfdIndex;
    projectUtilities::GetOpenGL_PixelFormatDescriptor(nPFD, pfdIndex);
    PS_OGLBase::SetPfdSelectionSettings(nPFD == 0, pfdIndex);

    bool antiAlias2DLines;
    projectUtilities::GetOpenGL2DAntiAlias(antiAlias2DLines);
    PS_OGLBase::Set2DAntiAliasLines(antiAlias2DLines);

    bool antiAlias3DLines;
    projectUtilities::GetOpenGL3DAntiAlias(antiAlias3DLines);
    PS_OGLBase::Set3DAntiAliasLines(antiAlias3DLines);

    int currBMF;
    projectUtilities::GetBitmapFontPlatformSetting(currBMF);
    PS_OGLBase::SetBitmapFontPlatformSettings(PS_OGLBase::BitmapFontPlatform(currBMF));

    bool europeanReals;
    projectUtilities::GetEuropeanReals(europeanReals);
    SC_RealFormat::european = europeanReals;


    //      is it still OK ?
    appVersionGlobals::CheckExpired();

    ASSERT(mainToolbarID != -1);

    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
                               | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
        || !m_wndToolBar.LoadToolBar(mainToolbarID))
        {
            TRACE0("Failed to create toolbar\n");
            return -1;      // fail to create
        }
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);


    m_menuSpawn.LoadToolBarResource(mainToolbarID);


    if (LoadToolbars() == -1)
        return -1;


    AppMenuC::InitMainMenuBar(mainToolbarID);
    AppMenuC::InitWindowSelector();

    // Due to multiple frame windows,
    // when file is opened from command line we have to set
    // mainwnd for AfxGetMainWnd call here
    AfxGetApp()->m_pMainWnd = this;

    return 0;
}

BOOL CProjectFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    // Restore main window position

    CWinApp* app = AfxGetApp();
    int s, t, b, r, l;

    // only restore if there is a previously saved position
    if ( -1 != (s = app->GetProfileInt("ProjectFrame", "Status",   -1))
         && -1 != (t = app->GetProfileInt("ProjectFrame", "Top",      -1))
         && -1 != (l = app->GetProfileInt("ProjectFrame", "Left",     -1))
         && -1 != (b = app->GetProfileInt("ProjectFrame", "Bottom",   -1))
         && -1 != (r = app->GetProfileInt("ProjectFrame", "Right",    -1)))
        {
            // restore the window's status
            app->m_nCmdShow = s;

            // sometimes registry values are bad - reset to default
            int min_l = - ::GetSystemMetrics(SM_CXSCREEN);  // assume dual monitor
            if ((t < 0) || (b < t) || (r < min_l) || (l > r))
                {
                    t = 100;
                    b = 600;
                    l = 100;
                    r = 800;
                }

            // restore the window's width and height
            cs.cx = r - l;
            cs.cy = b - t;

            // the following correction is needed when the taskbar is
            // at the left or top and it is not "auto-hidden"
            RECT rcWorkArea;
            ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
            l += rcWorkArea.left;
            t += rcWorkArea.top;

            // make sure the window is not completely out of sight
            int max_x = ::GetSystemMetrics(SM_CXSCREEN) - ::GetSystemMetrics(SM_CXICON);
            int max_y = ::GetSystemMetrics(SM_CYSCREEN) - ::GetSystemMetrics(SM_CYICON);
            cs.x = min(l, max_x);
            cs.y = min(t, max_y);
        }

    return TRUE;
}

BOOL CProjectFrame::DestroyWindow()
{
    // Save main window position
    //

    CWinApp* app = AfxGetApp();
    WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
    app->WriteProfileInt("ProjectFrame", "Status", wp.showCmd);
    app->WriteProfileInt("ProjectFrame", "Top",    wp.rcNormalPosition.top);
    app->WriteProfileInt("ProjectFrame", "Left",   wp.rcNormalPosition.left);
    app->WriteProfileInt("ProjectFrame", "Bottom", wp.rcNormalPosition.bottom);
    app->WriteProfileInt("ProjectFrame", "Right",  wp.rcNormalPosition.right);

    return CFrameWnd::DestroyWindow();
}

void CProjectFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (lpDrawItemStruct->CtlType == ODT_MENU)
        {
            _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
            if (pThreadState->m_hTrackingMenu != 0) {
                CMenu* pMenu = CMenu::FromHandle(pThreadState->m_hTrackingMenu);
                if (pMenu != NULL && dynamic_cast<TitlePopupMenu*>(pMenu) != NULL) {
                    // dirty method how to find title of TitlePopupMenu (-1 doesn't work here)
                    if (lpDrawItemStruct->itemID == 1) {
                        pMenu->DrawItem(lpDrawItemStruct);
                        return;
                    }
                }
            }
            m_menuSpawn.DrawItem(lpDrawItemStruct);
        }
}

void CProjectFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    if (lpMeasureItemStruct->CtlType == ODT_MENU)
        {
            _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
            if (pThreadState->m_hTrackingMenu != 0) {
                CMenu* pMenu = CMenu::FromHandle(pThreadState->m_hTrackingMenu);
                if (pMenu != NULL && dynamic_cast<TitlePopupMenu*>(pMenu)!= NULL) {
                    // dirty method how to find title of TitlePopupMenu
                    if (lpMeasureItemStruct->itemID == 1) {
                        pMenu->MeasureItem(lpMeasureItemStruct);
                        return;
                    }
                }
            }
            m_menuSpawn.MeasureItem(lpMeasureItemStruct);
        }
}

void CProjectFrame::OnInitMenu(CMenu* pMenu)
{
    CFrameWnd::OnInitMenu(pMenu);
    _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
    m_menuSpawn.RemapMenu(pMenu);
}

void CProjectFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    if (!bSysMenu && nIndex == m_nWindowMenuPos)
        { // Window
            CMenu* pWindowMenu = GetMenu()->GetSubMenu(m_nWindowMenuPos);
            if (pWindowMenu == pPopupMenu) {
                AppMenuC::InitWindowPopupMenu(pPopupMenu, m_nWindowMenuStart);
            }
        }

    if (!bSysMenu && nIndex == m_nTemplateMenuPos)
        { // Window
            CMenu* pWindowMenu = GetMenu()->GetSubMenu(m_nTemplateMenuPos);
            if (pWindowMenu == pPopupMenu) {
                AppMenuC::InitTemplatePopupMenu(pPopupMenu, m_nTemplateMenuStart);
            }
        }

    CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

    m_menuSpawn.RemapMenu(pPopupMenu);
}

BOOL CProjectFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra,
                             AFX_CMDHANDLERINFO* pHandlerInfo)
{
    if (nCode == CN_COMMAND && nID >= m_nPulldownMenuFrom && nID < m_nPulldownMenuTo) {
        OnMenuSelected(nID);
        return TRUE;
    }

    if (nCode == CN_COMMAND && nID >= m_nWindowMenuStart && nID < m_nWindowMenuEnd) {
        OnWindowMenuSelected(nID);
        return TRUE;
    }

    if (nCode == CN_COMMAND && nID >= m_nTemplateMenuStart && nID < m_nTemplateMenuEnd) {
        OnTemplateMenuSelected(nID);
        return TRUE;
    }

    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


LRESULT CProjectFrame::OnMenuSelectInternal(WPARAM wParam, LPARAM lParam)
{
    UINT nItemID = LOWORD(wParam);
    UINT nFlags = HIWORD(wParam);
    HMENU hSysMenu = reinterpret_cast<HMENU>(lParam);
    if (nItemID >= m_nPulldownMenuFrom && nItemID < m_nPulldownMenuTo) {
        ASSERT(nFlags | MF_POPUP);
        m_hMenuSelected = hSysMenu;
        m_nItemIDSelected = nItemID;
    }
    return TRUE;
}

/*
  void CProjectFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
  {
  TRACE("CProjectFrame::OnMenuSelect\n");
  TRACE("   %d %d %x\n", nItemID, nFlags, hSysMenu);

  if (nItemID >= m_nPulldownMenuFrom && nItemID < m_nPulldownMenuTo) {
  ASSERT(nFlags | MF_POPUP);
  m_hMenuSelected = hSysMenu;
  m_nItemIDSelected = nItemID;
  }
  AppMenuC::SetMenuBarSensitive();
  }
*/

void CProjectFrame::OnMenuSelected(UINT nID)
{
    TRACE("CProjectFrame::OnMenuSelected 0x%x %d!!\n", m_hMenuSelected, m_nItemIDSelected);
    ASSERT(m_nItemIDSelected == nID);
    ASSERT(::IsMenu(m_hMenuSelected));

    PullMenuC* menu = (PullMenuC*)CMenu::FromHandle(m_hMenuSelected);
    ASSERT(menu != NULL);
    char aName[255];
    const char* pObjID = menu->GetCBName(m_nItemIDSelected, aName, 255);

    AppMenuC::NewObject(pObjID);
}

void CProjectFrame::OnWindowMenuSelected(UINT nID)
{
    AppMenuC::BringWindowToTop(nID - m_nWindowMenuStart);
}

void CProjectFrame::LoadTemplateFile(const char* tfName)
{

}

void CProjectFrame::OnTemplateMenuSelected(UINT nID)
{
    char fName[stdFileStrLen];
    AppMenuC::GetSelectedTemplateFileName(nID - m_nTemplateMenuStart, fName, stdFileStrLen);
    LoadTemplateFile(fName);
}

void CProjectFrame::WinHelp(DWORD dwData, UINT nCmd)
{
    HtmlHelp(dwData, nCmd);

    /*
      const char* szHelpFile = AfxGetApp()->m_pszHelpFilePath;

      if ( HtmlHelp(m_hWnd, szHelpFile,
      HH_HELP_CONTEXT, nCmd == HELP_CONTEXT ? dwData : 0) == NULL )
      {
      HtmlHelp(m_hWnd, AfxGetApp()->m_pszHelpFilePath, HH_HELP_FINDER, 0);
      }
    */
}

void CProjectFrame::OnPageBringPlotToTop()
{
    AppMenuC::BringPageToTop();
}

// Window Selector support
void CProjectFrame::OnWindowList()
{
    AppMenuC::SwitchWindowSelector();
}

void CProjectFrame::OnUpdateWindowList(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(AppMenuC::IsWindowSelectorEnabled());
}

void CProjectFrame::OnActivateApp(BOOL bActive, DWORD hTask)
{
    if (AppMenuC::IsWindowSelectorEnabled() && !windowSelector.IsPinned()) {
        if (bActive) {
            TRACE("Activate App\r\n");
            windowSelector.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        } else {
            TRACE("DEActivate App\r\n");
            windowSelector.SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        }
    }
    CFrameWnd::OnActivateApp(bActive, hTask);
}

void CProjectFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE) {
        if (m_pPopWnd != NULL)
            m_pPopWnd->BringWindowToTop();
        AppMenuC::RefreshWindowSelector(NULL, false);
        TRACE("Activate ProjectFrame %d\r\n", nState);
    }

    CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
}

void CProjectFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SC_MINIMIZE) {
        AppMenuC::ShowAppWindows(SW_HIDE);
        if (AppMenuC::IsWindowSelectorEnabled() && !windowSelector.IsPinned()) {
            windowSelector.ShowWindow(SW_HIDE);
        }
    } else if (nID == SC_RESTORE) {
        AppMenuC::ShowAppWindows(SW_SHOW);
        if (AppMenuC::IsWindowSelectorEnabled() && !windowSelector.IsPinned()) {
            windowSelector.ShowWindow(SW_SHOW);
        }
    }
    CFrameWnd::OnSysCommand(nID, lParam);
}

void CProjectFrame::OnMinimizeAllPlots()
{
    AppMenuC::ShowAppWindows(SW_MINIMIZE);
}

void CProjectFrame::OnUpdateMinimizeAllPlots(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(AppMenuC::IsAnyAppWindowOpen());
}

LRESULT CProjectFrame::OnMViewNone(WPARAM wParam, LPARAM lParam)
{
    m_wndMViewMessagebar.SetStatus(msNone, LPCSTR(wParam));
    return 0;
}

LRESULT CProjectFrame::OnMViewInfo(WPARAM wParam, LPARAM lParam)
{
    m_wndMViewMessagebar.SetStatus(msInformation, LPCSTR(wParam));
    return 0;
}

LRESULT CProjectFrame::OnMViewWarning(WPARAM wParam, LPARAM lParam)
{
    m_wndMViewMessagebar.SetStatus(msWarning, LPCSTR(wParam));
    return 0;
}

LRESULT CProjectFrame::OnMViewError(WPARAM wParam, LPARAM lParam)
{
    m_wndMViewMessagebar.SetStatus(msError, LPCSTR(wParam));
    return 0;
}

LRESULT CProjectFrame::OnMViewRange(WPARAM wParam, LPARAM lParam)
{
    m_wndMViewMessagebar.SetValueRange(LPCSTR(wParam), LPCSTR(lParam));
    return 0;
}

LRESULT CProjectFrame::OnMViewPlotSettings(WPARAM wParam, LPARAM lParam)
{
    AppMenuC::SetPlotFont(wParam);
    projectUtilities::SetPlotSmallFont(wParam != 0);
    return 0;
}

LRESULT CProjectFrame::OnMViewOpenPropMenu(WPARAM wParam, LPARAM lParam)
{
    LPCSTR szText = LPCSTR(wParam);
    m_wndInfoBar.SetText(szText);

    return 0;
}

LRESULT CProjectFrame::OnMViewFrameActivate(WPARAM wParam, LPARAM lParam)
{
    if (wParam == 0)
        {
            AppMenuC::RefreshWindowSelector(NULL, false);
        }
    else
        {
            PS_MainWindow* pmainWnd = (PS_MainWindow*)lParam;
            AppMenuC::RefreshWindowSelector(pmainWnd, false);
        }

    return 0;
}

LRESULT CProjectFrame::OnMViewFrameRename(WPARAM wParam, LPARAM lParam)
{
    if (wParam == 0)
        {
            ASSERT(0);
        }
    else
        {
            PS_MainWindow* pmainWnd = (PS_MainWindow*)lParam;
            AppMenuC::RefreshWindowSelector(pmainWnd, true);
        }

    return 0;

}

void CProjectFrame::OnFilePathUpdate()
{
    MenuObjC::CloseOpenMenu();
    CFileNameUpdateDlg dlg;
    dlg.DoModal();
}

void CProjectFrame::OnViewSettings()
{
    CSettingsPropSheet propSheet("Settings", this);

    propSheet.m_nCheckSmallFont = projectUtilities::IsPlotSmallFont();
    propSheet.m_nAlpha = projectUtilities::GetAlpha();

    propSheet.m_nSingleLine = FALSE;
    propSheet.m_nSingleLineType = 0;
    propSheet.m_nShowUnused = 0;
    propSheet.m_nShowConnections = 0;

    projectUtilities::GetTreeViewSettings(propSheet.m_nSingleLine, propSheet.m_nSingleLineType, propSheet.m_nShowUnused, propSheet.m_nShowConnections);

    projectUtilities::GetOpenGL_PixelFormatDescriptor(propSheet.m_nPFD, propSheet.m_pfdIndex);

    bool dummy;
    projectUtilities::GetOpenGL2DAntiAlias(dummy);
    propSheet.m_nantiAlias2DLine = dummy;
    projectUtilities::GetOpenGL3DAntiAlias(dummy);
    propSheet.m_nantiAlias3DLine = dummy;

    projectUtilities::GetBitmapFontPlatformSetting(propSheet.m_nbitmapFontPlatformType);

    projectUtilities::GetCalculationPrioritySetting(propSheet.m_ncalcPriority);

    projectUtilities::GetEuropeanReals(dummy);
    propSheet.m_europeanReals = dummy;

    CString sTemp;
    projectUtilities::GetTemplateDirectory(sTemp);
    propSheet.m_szTemplateDirectoryName = sTemp;

    if (propSheet.DoModal() == IDOK) {

        propSheet.ApplyChanges(false);

    }
}

// ****************************************************** object copy/paste support

bool CProjectFrame::ObjectsSelected()
{
    bool treeOK = (MenuRootC::currTreeObj != 0);
    if (treeOK)
        {
            TreeBase* treeUI = MenuRootC::currTreeObj->GetUI();
            if (treeUI != 0)
                treeOK = treeUI->selectedObjects.IsNotEmpty();
            else
                treeOK = false;
        }
    bool snglOK = (MenuObjC::currMenuObj != 0) && (MenuObjC::currMenuObj->IsCopyable());
    return treeOK || snglOK;
}


void CProjectFrame::OnObjectApply()
{
    AppMenuC::ApplyObject();
}
void CProjectFrame::OnUpdateObjectApply(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(MenuObjC::currMenuObj != 0);
}

void CProjectFrame::OnObjectCopy()
{
    AppMenuC::CopyObject();
}

void CProjectFrame::OnUpdateObjectCopy(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(ObjectsSelected());
}


void CProjectFrame::OnObjectCopyPage()
{
    AppMenuC::CopyPageObjects();
}

void CProjectFrame::OnUpdateObjectCopyPage(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(MenuPageC::currPageObj != 0);
}


void CProjectFrame::OnObjectDelete()
{
    AppMenuC::DeleteObject();
}

void CProjectFrame::OnUpdateObjectDelete(CCmdUI* pCmdUI)
{
    bool delOK = false;
    if (MenuRootC::currTreeObj != 0)
        {
            TreeBase* treeUI = MenuRootC::currTreeObj->GetUI();
            if (treeUI != 0)
                {
                    MenuObjArray& currObjs = treeUI->selectedObjects;
                    for (int i = 0; i < currObjs.Size(); i++)
                        if (currObjs[i]-> IsDeleteable())
                            delOK = true;
                }
        }
    else
        delOK = (MenuObjC::currMenuObj != 0) && (MenuObjC::currMenuObj->IsDeleteable());

    pCmdUI->Enable(delOK);
}

void CProjectFrame::OnObjectDuplicate()
{
    AppMenuC::DuplicateObject();
}
void CProjectFrame::OnUpdateObjectDuplicate(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(ObjectsSelected());
}


void CProjectFrame::OnObjectPaste()
{
    AppMenuC::PasteObject();
}
void CProjectFrame::OnUpdateObjectPaste(CCmdUI* pCmdUI)
{
    bool pasteOK = (MenuPageC::currPageObj != 0) && appCopyPaste::ObjectPasteOK();
    pCmdUI->Enable(pasteOK);
}


void CProjectFrame::OnObjectListConnections()
{
    if ((MenuObjC::currMenuObj == 0) || (MenuObjC::currMenuObj->objFunction == 0))
        return;

    FuncObjC* listObj = MenuObjC::currMenuObj->objFunction;
    LFO_ListOneObject snglObj(listObj);


    ListDefCPS modalListing("Object Connections");
    modalListing.AddTo(&snglObj);
    modalListing.SuppressDefaults();
    DoModalView(modalListing);
}

void CProjectFrame::OnUpdateObjectListConnections(CCmdUI* pCmdUI)
{
    bool listOK = (MenuObjC::currMenuObj != 0) && (MenuObjC::currMenuObj->objFunction != 0);
    pCmdUI->Enable(listOK);
}



void CProjectFrame::OnCreatePlotFolder()
{
    AppMenuC::NewPlotFolder();
}

void CProjectFrame::OnUpdateCreatePlotFolder(CCmdUI* pCmdUI)
{
    bool createOK = (MenuPageC::currPageObj != 0) &&
        MenuPageC::currPageObj->GetActualParent()->IsPlotPage();

    pCmdUI->Enable(createOK);

}

// ****************************************************** page copy/paste support

bool CProjectFrame::PagesSelected()
{
    bool treeOK = (MenuRootC::currTreeObj != 0);
    if (treeOK)
        {
            TreeBase* treeUI = MenuRootC::currTreeObj->GetUI();
            if (treeUI != 0)
                treeOK = treeUI->selectedPages.IsNotEmpty();
            else
                treeOK = false;
        }
    bool snglOK = (MenuPageC::currPageObj != 0) && (MenuPageC::currPageObj->IsCopyable());
    return treeOK || snglOK;
}


void CProjectFrame::OnPageDuplicate()
{
    AppMenuC::DuplicatePage();
}
void CProjectFrame::OnUpdatePageDuplicate(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(PagesSelected());
}


void CProjectFrame::OnPageClearAll()
{
    AppMenuC::ClearAllPages();
}

void CProjectFrame::OnUpdatePageClearAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(true);
}

void CProjectFrame::OnPageCollapseAll()
{
    AppMenuC::CollapseAllPages();
}

void CProjectFrame::OnUpdatePageCollapseAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(true);
}


void CProjectFrame::OnPageCopyCurrent()
{
    AppMenuC::CopyCurrentPage();
}

void CProjectFrame::OnUpdatePageCopyCurrent(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(PagesSelected());
}

void CProjectFrame::OnPageCopyAll()
{
    AppMenuC::CopyAllPages();
}

void CProjectFrame::OnUpdatePageCopyAll(CCmdUI* pCmdUI)
{
    bool copyOK = (MenuRootC::currTreeObj != 0);
    pCmdUI->Enable(copyOK);
}


void CProjectFrame::OnPagePaste()
{
    AppMenuC::PastePage();
}

void CProjectFrame::OnUpdatePagePaste(CCmdUI* pCmdUI)
{
    bool pasteOK = (MenuRootC::currTreeObj != 0) && appCopyPaste::PagePasteOK();
    pCmdUI->Enable(pasteOK);
}

void CProjectFrame::OnPageDelete()
{
    AppMenuC::DeletePage();
}

void CProjectFrame::OnUpdatePageDelete(CCmdUI* pCmdUI)
{
    bool delOK = false;
    if (MenuRootC::currTreeObj != 0)
        {
            TreeBase* treeUI = MenuRootC::currTreeObj->GetUI();
            if (treeUI != 0)
                {
                    MenuPageArray& currPages = treeUI->selectedPages;
                    for (int i = 0; i < currPages.Size(); i++)
                        if (currPages[i]->IsDeleteable())
                            delOK = true;
                }
        }
    else
        delOK = (MenuPageC::currPageObj != 0) && (MenuPageC::currPageObj->IsDeleteable());

    pCmdUI->Enable(delOK);
}

void CProjectFrame::OnPageNewData()
{
    AppMenuC::NewDataPage();
}

void CProjectFrame::OnPageNew3dXYZ()
{
    AppMenuC::NewXYZPage();
}

void CProjectFrame::OnPageNew2dXY()
{
    AppMenuC::NewXYPage();
}

void CProjectFrame::OnPageNewList()
{
    AppMenuC::NewListPage();
}

void CProjectFrame::OnPageNewComposite()
{
    AppMenuC::NewCompositePage();
}

void CProjectFrame::OnPageListAllConnections()
{
    // suppress default listings
    ListObjArray tmpList(ListDefC::defaultListObj);
    ListDefC::defaultListObj.SetEmpty();


    LFO_ListAllObjects allObj;
    ListDefCPS modalListing("Object Connections");
    modalListing.AddTo(&allObj);
    modalListing.SuppressDefaults();
    DoModalView(modalListing);

    // restore
    ListDefC::defaultListObj = tmpList;
}

void CProjectFrame::OnUpdatePageListAllConnections(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(true);
}


void CProjectFrame::DoModalView(ListDefCPS& listDef)
{
    m_pPopWnd = &listDef;
    listDef.ModalView();
    m_pPopWnd = NULL;
}

