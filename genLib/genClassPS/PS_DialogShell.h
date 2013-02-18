///////////////////////////////////////////////////////////////////////////////////
//
// PS_DialogShell.h
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
// DESCRIPTION:Platform specific but application independent code.
//             base classs for all modeless dialog windows
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PS_DIALOGSHELL_H
#define PS_DIALOGSHELL_H

#include <genClassPS/PS_NCHitTest.h>

#include <genClassPS/PS_UIBase.h>
#include <genClassPS/PS_MainWindow.h>
#include <genClassPS/PS_FormContainer.h>

// namespace for all genClass stuff
namespace genClassPS {

    // Shell for modeless dialogs it should contain a layout manager
    class DialogShell : public CWnd, public LayoutManager, public PS_MainWindow, public FormContainer {
    public:
        DialogShell(const char* winType);

        void            CreateShell(CWnd* parent,
                                    bool resizable,
                                    const char* title,
                                    const CRect& rect);
        void            PumpMessages();

        //  BaseWindow virtuals
        virtual void    Manage();
        virtual void    Unmanage();

    protected:
        // FormContainer virtuals
        virtual CWnd*   GetWindow();

    protected:
        void            UpdateGrip();

    protected:
        bool            m_bShowGrip;
        CRect           m_rcGripRect;
        bool            m_bOwnerCreated;
        CWnd            m_wndOwnerWindow;

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CProjectView)
    public:
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        //}}AFX_VIRTUAL
    protected:
        //{{AFX_MSG(DialogShell)
        afx_msg void OnPaint();
        afx_msg NCHITRETVAL OnNcHitTest(CPoint point);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };


    // Helper class for hosting forms inside DialogShell
    class DialogShellFormManager : public LayoutManager, public OrdinalDispatcher
    {
    public:
        DialogShellFormManager(CWnd* pWnd);

    protected:
        virtual     CWnd*   GetParentWnd();
        virtual     OrdinalDispatcher* GetOrdinalDispatcher();

    protected:
        CWnd*   m_pWnd;
    };


} // end namespace

using namespace genClassPS;

#endif // PS_DIALOGSHELL_H

