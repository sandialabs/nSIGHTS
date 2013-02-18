///////////////////////////////////////////////////////////////////////////////////
//
// C_ListDefPS.h
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

#ifndef LISTDEFCPS_H
#define LISTDEFCPS_H

#include <genListClass/C_ListDef.h>
#include <genClassPS/PS_MainWindow.h>

class CListingHtmlView;

class ListDefCPS : public CFrameWnd, public PS_MainWindow, public ListDefC {

    public:

        CToolBar            m_wndToolBar;
        CListingHtmlView*   m_pBrowser;

    private:
        bool                m_bOwnerCreated;
        CWnd                m_wndOwnerWindow;
        const bool          isModal;

    public:
        //  Support for two useages:
        //  1) normal like plots --
        //      a) is created with new in newmenupage
        //      b) has objects added using normal mechanism ListObjC(ListDefC&)
        //      c) display will be updated automatically through CalcOutput()
        //      d) remains until MenuPageC with assocList is deleted
        //  2) as Modal listing for use in nPre
        //      a) created on stack
        //      b) listing objects added (ListObjC() with AddToList(ListDefC&)
        //      c) Modal viewing -- keeps app focus returns on Close
        //      d) destroyed when it goes out of scope

                        ListDefCPS(bool isMod);
                        ListDefCPS(const char* title);  // modal with title
                        ListDefCPS(const ListDefCPS& a);  // this will only be used for non modal

                        ~ListDefCPS();
        ListDefCPS&      operator= (const ListDefCPS& a);


                        // waits until window is closed
        void            ModalView();

        //  FuncObj
        virtual void    DoStatusChk();


        // copied from PC_PlatformPS
        void            BringListWindowToTop();

        //  ListDefC virtuals
        virtual void        ClearDisplay();
        virtual void        SendToDisplay(const SC_StringArray& dispText);
        virtual void        ChangeTitle(const char* newName);

    private:
        void            InitCommon();  // common to normal and copy constructor
        void            LocalCopy(const ListDefCPS& a);


//  from PC_Platform
        UINT            GetMenuResourceID();

        void            OnViewFullScreen();


    protected:
        void            EnableCloseInSystemMenu(bool bEnable);
        void            SetFrameIcon(UINT nID);

        void            PumpMessages();

// Overrides
// ClassWizard generated virtual function overrides
//{{AFX_VIRTUAL(ListDefCPS)
    public:
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
        virtual void PostNcDestroy();
        virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
        virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
//}}AFX_VIRTUAL

// Generated message map functions
    protected:
//{{AFX_MSG(ListDefCPS)
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* pMMI);
    afx_msg void OnToolbar();
    afx_msg void OnUpdateToolbar(CCmdUI* pCmdUI);
    //}}AFX_MSG
    afx_msg void OnPreviewSaveAs();
    DECLARE_MESSAGE_MAP()

    private:
        void            CalcBorderSize(int& nWidth, int& nHeight);



};



#endif // !LISTDEFCPS_H

