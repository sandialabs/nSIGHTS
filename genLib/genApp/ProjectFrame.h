///////////////////////////////////////////////////////////////////////////////////
//
// ProjectFrame.h
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

#ifndef PROJECTFRAME_H
#define PROJECTFRAME_H

#include <genUIExt/InfoBar.h>
#include <genUIExt/BitmapMenu.h>
#include <genUIExt/SizingControlBarGrip.h>

class  CProjectFrame;

class CBitmapMenuExclude : public CBitmapMenu
{
public:
    CBitmapMenuExclude(CProjectFrame& projectFrame);

    virtual bool ExcludeItem(UINT nItemID);

protected:
    CProjectFrame& projectFrame;
};


class  ListDefCPS;

class CProjectFrame : public CFrameWnd
{
    friend class CBitmapMenuExclude;

    DECLARE_DYNCREATE(CProjectFrame)
    public:
    CInfoBarStatus                 m_wndMViewMessagebar;
    CInfoBar                       m_wndInfoBar;

protected:
    CStatusBar                     m_wndStatusBar;
    CToolBar                       m_wndToolBar;

    CBitmapMenuExclude             m_menuSpawn;

    // for pulldown menu support
    UINT                           m_nPulldownMenuFrom;
    UINT                           m_nPulldownMenuTo;
    HMENU                          m_hMenuSelected;
    UINT                           m_nItemIDSelected;

    // Auto Setup of active work directory submemnu of main menu bar
    UINT                           m_nTemplateMenuPos;
    UINT                           m_nTemplateMenuStart;
    UINT                           m_nTemplateMenuEnd;

    // dynamic Windows submemnu of main menu bar
    UINT                           m_nWindowMenuPos;
    UINT                           m_nWindowMenuStart;
    UINT                           m_nWindowMenuEnd;

    CWnd*                          m_pPopWnd; // HTML Listing window -- moved from outlook window

    int                            mainToolbarID;

public:
    CProjectFrame();
    virtual ~CProjectFrame();

protected:
    virtual int                    LoadToolbars();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProjectFrame)
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
protected:
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual BOOL DestroyWindow();
    //}}AFX_VIRTUAL

    virtual void LoadTemplateFile(const char* tfName);

protected:
    //{{AFX_MSG(CProjectFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
    afx_msg void OnMenuSelected(UINT nID);
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnPageBringPlotToTop();
    afx_msg void OnWindowList();
    afx_msg void OnUpdateWindowList(CCmdUI* pCmdUI);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnMinimizeAllPlots();
    afx_msg void OnUpdateMinimizeAllPlots(CCmdUI* pCmdUI);
    afx_msg void OnViewSettings();
    afx_msg void OnFilePathUpdate();


    afx_msg void OnObjectApply();
    afx_msg void OnUpdateObjectApply(CCmdUI* pCmdUI);
    afx_msg void OnObjectCopy();
    afx_msg void OnUpdateObjectCopy(CCmdUI* pCmdUI);
    afx_msg void OnObjectCopyPage();
    afx_msg void OnUpdateObjectCopyPage(CCmdUI* pCmdUI);
    afx_msg void OnObjectDelete();
    afx_msg void OnUpdateObjectDelete(CCmdUI* pCmdUI);
    afx_msg void OnObjectDuplicate();
    afx_msg void OnUpdateObjectDuplicate(CCmdUI* pCmdUI);
    afx_msg void OnObjectPaste();
    afx_msg void OnUpdateObjectPaste(CCmdUI* pCmdUI);
    afx_msg void OnObjectListConnections();
    afx_msg void OnUpdateObjectListConnections(CCmdUI* pCmdUI);

    afx_msg void OnCreatePlotFolder();
    afx_msg void OnUpdateCreatePlotFolder(CCmdUI* pCmdUI);

    afx_msg void OnPageDuplicate();
    afx_msg void OnUpdatePageDuplicate(CCmdUI* pCmdUI);
    afx_msg void OnPageDelete();
    afx_msg void OnUpdatePageDelete(CCmdUI* pCmdUI);
    afx_msg void OnPageCopyCurrent();
    afx_msg void OnUpdatePageCopyCurrent(CCmdUI* pCmdUI);
    afx_msg void OnPageCopyAll();
    afx_msg void OnUpdatePageCopyAll(CCmdUI* pCmdUI);
    afx_msg void OnPagePaste();
    afx_msg void OnUpdatePagePaste(CCmdUI* pCmdUI);
    afx_msg void OnPageClearAll();
    afx_msg void OnUpdatePageClearAll(CCmdUI* pCmdUI);
    afx_msg void OnPageCollapseAll();
    afx_msg void OnUpdatePageCollapseAll(CCmdUI* pCmdUI);
    afx_msg void OnPageListAllConnections();
    afx_msg void OnUpdatePageListAllConnections(CCmdUI* pCmdUI);

    afx_msg void OnPageNewData();
    afx_msg void OnPageNew3dXYZ();
    afx_msg void OnPageNew2dXY();
    afx_msg void OnPageNewList();
    afx_msg void OnPageNewComposite();

    afx_msg void OnWindowMenuSelected(UINT nID);
    afx_msg void OnTemplateMenuSelected(UINT nID);
    //}}AFX_MSG

    LRESULT OnMenuSelectInternal(WPARAM wParam, LPARAM lParam);

    LRESULT OnMViewNone(WPARAM wParam, LPARAM lParam);
    LRESULT OnMViewInfo(WPARAM wParam, LPARAM lParam);
    LRESULT OnMViewWarning(WPARAM wParam, LPARAM lParam);
    LRESULT OnMViewError(WPARAM wParam, LPARAM lParam);
    LRESULT OnMViewRange(WPARAM wParam, LPARAM lParam);
    LRESULT OnMViewFrameActivate(WPARAM wParam, LPARAM lParam);
    LRESULT OnMViewFrameRename(WPARAM wParam, LPARAM lParam);
    LRESULT OnMViewPlotSettings(WPARAM wParam, LPARAM lParam);
    LRESULT OnMViewOpenPropMenu(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

    protected:
    void    DoModalView(ListDefCPS& listDef);


private:


    static  bool    ObjectsSelected();
    static  bool    PagesSelected();

};

#endif // PROJECTFRAME_H

