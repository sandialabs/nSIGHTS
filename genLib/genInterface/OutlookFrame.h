///////////////////////////////////////////////////////////////////////////////////
//
// OutlookFrame.h
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

#ifndef OUTLOOKFRAME_H
#define OUTLOOKFRAME_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <genApp/ProjectFrame.h>
#include <genUIExt/SizingControlBarGrip.h>

class InterfaceOutlook;
class FormContainer;
class ListDefCPS;

#define WM_REPLOT WM_USER+2

class COutlookFrame : public CProjectFrame
{
    DECLARE_DYNCREATE(COutlookFrame)
    //protected: // create from serialization only
    //  COutlookFrame();

    public:
    InterfaceOutlook*       m_pInterface;
    CImageList              m_imaLarge;
    CImageList              m_imaSmall;
    CSizingControlBarGrip   m_wndControlBar;

private:
    // for thread sync of replot
    static HANDLE           replotThreadEvent;
    static HWND             replotWnd;


protected:
    CToolBar                m_wndOutlookToolBar;

    // Operations
public:
    COutlookFrame();
    virtual                 ~COutlookFrame();

    virtual void            ResetInterface(CDocument* pDocument, bool bRestoreContext);
    bool                    VerifyBarState(LPCTSTR lpszProfileName);


    // misc static ops
    static  FormContainer*  GetFormContainer();


    //  replot thread sync support
    static  void            StartReplotProcessing();
    static  void            DoReplot(const char* appID);
    static  void            EndReplotProcessing();


protected:
    //  to be implemented in derived
    virtual void            CreateInterface();

    virtual int             LoadToolbars();

    void                    DestroyInterface();
    void                    ResetRoots();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(COutlookFrame)
public:
    virtual BOOL            PreCreateWindow(CREATESTRUCT& cs);
protected:
    virtual BOOL            OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual BOOL            DestroyWindow();
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:
    //{{AFX_MSG(COutlookFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnUixLargeicon();
    afx_msg void OnUpdateUixLargeicon(CCmdUI* pCmdUI);
    afx_msg void OnUixSmallicon();
    afx_msg void OnUpdateUixSmallicon(CCmdUI* pCmdUI);
    afx_msg void OnViewControlBar();
    afx_msg void OnUpdateViewControlBar(CCmdUI* pCmdUI);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnSysCommand(UINT nID, LONG_PTR lParam);
    //}}AFX_MSG

    LRESULT OnMViewOpenPropMenu(WPARAM wParam, LPARAM lParam);

    LRESULT OnReplot(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};

#endif // OUTLOOKFRAME_H

