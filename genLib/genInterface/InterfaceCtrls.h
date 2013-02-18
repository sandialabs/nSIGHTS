///////////////////////////////////////////////////////////////////////////////////
//
// InterfaceCtrls.h
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
//              interface for the DialogNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERFACECTRLS_H__A1005FEA_E082_11D3_BCCC_00104B4C74FD__INCLUDED_)
#define AFX_INTERFACECTRLS_H__A1005FEA_E082_11D3_BCCC_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <genInterface/Interface.h>

#include <genInterface/GenericDialog.h>
#include <genInterface/DialogScrollView.h>

/////////////////////////////////////////////////////////////////////////////
// General control stuff

class DummyNode : public InterfaceNode
{
public:
    DummyNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
        InterfaceNode(pParent, nIdent, pDescription)
    {}
};

//////////////////// Dialog Interface node /////////////////////////
class DialogNode : public CDialogScrollView, public InterfaceNode
{
public:
    DialogNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
    virtual ~DialogNode();

    virtual bool    CreateNode(CWnd* pParentWnd);
    virtual CWnd*   ShowNode(bool bShow = true, CRect* pRect = NULL);
    virtual bool    Enable(int nIdent, bool bEnable = true);
    virtual void    Show(int nIdent);

    virtual CDialog* GetDialog();

    virtual     void    CheckRestrictions();
    virtual     void    RestoreControl(InterfaceContext& context);

protected:
    CGenericDialog* m_pDlg;

    // Generated message map functions
protected:
    //{{AFX_MSG(DialogNode)
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

class DlgNode : public InterfaceNode
{
public:
    DlgNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
    virtual ~DlgNode();

    virtual bool    CreateNode(CWnd* pParentWnd);
    virtual CWnd*   ShowNode(bool bShow = true, CRect* pRect = NULL);
    virtual bool    Enable(int nIdent, bool bEnable = true);
    virtual void    Show(int nIdent);

    virtual CDialog* GetDialog();

protected:
    CGenericDialog* m_pDlg;
};

/////////////// Generic Tab Control //////////////////////////////////
class TabbedFolderNode : public CTabCtrlEx, public FolderNode
{
public:
    TabbedFolderNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual bool    CreateNode(CWnd* pParentWnd);
    virtual CWnd*   ShowNode(bool bShow = true, CRect* pRect = NULL);
    virtual bool    Enable(int nIdent, bool bEnable = true); // Enable/Disable
    virtual void    Show(int nIdent);

    virtual void    OnSelection(int nNewTab);

protected:
    void    GetInsideRect(CRect& rect);
    int         GetTabLevel();
    bool    CreateNodeEx(CWnd* pParentWnd, DWORD dwStyle);

    void    SetupFolder();

    void    SelectFirstEnabledTab();

protected:
    int m_nTabLevel;

    //{{AFX_MSG(TabbedFolderNode)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

class TabbedFolderRoot : public TabbedFolderNode
{
public:
    TabbedFolderRoot(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
    virtual bool CreateNode(CWnd* pParentWnd);
};

////////////////////// Nested Tabbed Control ////////////////////////////
class TabbedFolderNode1 : public TabbedFolderNode
{
public:
    TabbedFolderNode1(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
};

////////////// Tabbed Control inside Win32 dialog template
class TabbedFolderNodeRes : public TabbedFolderNode
{
public:
    TabbedFolderNodeRes(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
};

/////////////////// Outlook bar definition ////////////////////////////////
class OutlookPane : public COutBarCtrl, public FolderNode
{
public:
    OutlookPane(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual bool    CreateNode(CWnd* pParentWnd);

protected:
    //{{AFX_MSG(OutlookPane)
    afx_msg void OnUixLargeicon();
    afx_msg void OnUixSmallicon();
    //}}AFX_MSG
    afx_msg LRESULT OnOutbarNotify(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};

class OutlookBar : public FolderNode
{
public:
    OutlookBar(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual bool    CreateNode(CWnd* pParentWnd);
    virtual void    Show(int nIdent);
};

#endif // AFX_INTERFACECTRLS_H__A1005FEA_E082_11D3_BCCC_00104B4C74FD__INCLUDED_

