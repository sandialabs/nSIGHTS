///////////////////////////////////////////////////////////////////////////////////
//
// PS_DialogShell.cpp
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
// DESCRIPTION: Platform specific but application independent code.
//              Base class for all modeless dialog windows.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClassPS/PS_DialogShell.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace genClassPS {

    DialogShell::DialogShell(const char* winType) :
        PS_MainWindow(*this, winType),
        m_bOwnerCreated(false)
    {
        SetVisible(false);  // invisible until managed
    }

    void DialogShell::CreateShell(CWnd* parent, bool resizable, const char* title, const CRect& rect)
    {
        LPCTSTR lpszClass = RegisterFormClass();
        int nID = 0; // no menu -> must be zero
        VERIFY(CWnd::CreateEx(0, lpszClass, "Dialog Shell", WS_POPUP|WS_BORDER|WS_CAPTION, rect, parent, nID));
        SetFont(GetMenuFont(), TRUE);

        if (resizable) {
            CWnd::ModifyStyle(0, WS_THICKFRAME); // add resizable flag
        }
        m_bShowGrip = resizable;
        UpdateGrip();

        CWnd::SetWindowText(title);
        UpdateWindow();

    }


    // we don't need to use recursive LayouManager algorithm
    // when we have single parent window, which shows/hides
    // all its chilren
    void DialogShell::Manage()
    {
        //  BaseWindow::Manage();
        MainWindowToTop();
        MainWindowActivated();
    }

    void DialogShell::Unmanage()
    {
        BaseWindow::Unmanage();
        MainWindowDeactivated();
    }

    BEGIN_MESSAGE_MAP(DialogShell, CWnd)
    //{{AFX_MSG_MAP(DialogShell)
    ON_WM_PAINT()
    ON_WM_NCHITTEST()
    ON_WM_SIZE()
    ON_WM_GETDLGCODE()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void DialogShell::OnPaint()
    {
        CPaintDC dc(this);

        if (m_bShowGrip) {
            dc.DrawFrameControl(&m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
        }
    }

    void DialogShell::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
    {
        if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
            {
                MainWindowActivated();
            }

        CWnd::OnActivate(nState, pWndOther, bMinimized);
    }


    NCHITRETVAL DialogShell::OnNcHitTest(CPoint point)
    {
        CPoint pt = point;
        ScreenToClient(&pt);

        if (m_bShowGrip && m_rcGripRect.PtInRect(pt)) {
            return HTBOTTOMRIGHT;
        }

        return CWnd::OnNcHitTest(point);
    }

    void DialogShell::OnSize(UINT nType, int cx, int cy)
    {
        CWnd::OnSize(nType, cx, cy);

        if (nType == SIZE_MINIMIZED) {
            return;
        }

        InvalidateRect(&m_rcGripRect);
        UpdateGrip();
        InvalidateRect(&m_rcGripRect);
    }

    CWnd* DialogShell::GetWindow()
    {
        return this;
    }

    void DialogShell::UpdateGrip()
    {
        static int xGrip = ::GetSystemMetrics(SM_CXVSCROLL);
        static int yGrip = ::GetSystemMetrics(SM_CYHSCROLL);

        GetClientRect(&m_rcGripRect);
        m_rcGripRect.left = m_rcGripRect.right - xGrip;
        m_rcGripRect.top = m_rcGripRect.bottom - yGrip;
    }

    BOOL DialogShell::PreCreateWindow(CREATESTRUCT& cs)
    {
        if (!CWnd::PreCreateWindow(cs)) {
            return FALSE;
        }

        if (!m_bOwnerCreated) {
            CRect rect;
            LPCTSTR lpszClass = RegisterFormClass(); // can be used any class
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

    void DialogShell::PumpMessages()
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

    BOOL DialogShell::PreTranslateMessage(MSG* pMsg)
    {
        if (pMsg->message == WM_KEYDOWN) {
            if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
                ::TranslateMessage(pMsg);
                ::DispatchMessage(pMsg);
                return TRUE;    // DO NOT process further
            }
        }
        return CWnd::PreTranslateMessage(pMsg);
    }


    //////////////////////////////////////////////////////
    DialogShellFormManager::DialogShellFormManager(CWnd* pWnd) :
        m_pWnd(pWnd)
    {
    }

    CWnd* DialogShellFormManager::GetParentWnd()
    {
        return m_pWnd;
    }

    OrdinalDispatcher* DialogShellFormManager::GetOrdinalDispatcher()
    {
        return this;
    }


} // namespace

