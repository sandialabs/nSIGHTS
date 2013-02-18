///////////////////////////////////////////////////////////////////////////////////
//
// GenericDialog.h
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
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef GENERIC_DIALOG_H
#define GENERIC_DIALOG_H

#include <genCtrl/CtrlUpdate.h>
/////////////////////////////////////////////////////////////////////////////
// CGenericDialog dialog

enum ResizeInfo {
    resizeNone      = 0,
    anchoreRight    =  0x0001,
    anchoreBottom   =  0x0002,
    resizeHorizontal=  0x0004,
    resizeVertical  =  0x0008,
    resizeBoth      = (resizeHorizontal | resizeVertical)
};

class ControlInfo
{
    friend class ResizableWindow;
public:
    ControlInfo& Add(int nCtrlID, ResizeInfo resizeInfo);

public:
    CArray<ControlInfo*,ControlInfo*> m_controlInfo;

protected:
    UINT        m_nCtrlID;
    ResizeInfo  m_resizeInfo;
};

class ResizableWindow
{
public:
public:
    ResizableWindow();
    virtual ~ResizableWindow();

    void InitMinimal(int nMinX, int nMinY);
    void Reset();

    ControlInfo& SetControlInfo(UINT nCtrlId, ResizeInfo resizeInfo);

    void Sizing(CWnd* pWnd, int cx, int cy);

protected:
    int     m_nMinWidth;
    int m_nMinHeight;
    int     m_nOld_cx;
    int     m_nOld_cy;
    bool m_bInSize;
    ControlInfo m_controlInfo;
};

class InterfaceNode;

// CCtrlUpdate added -- JDA -- 19 Jun 00 -- generic callback support for dialog
//  components
//

class CGenericDialog : public CDialog, public ResizableWindow, public CCtrlUpdate
{
    DECLARE_DYNAMIC(CGenericDialog)
    public:
    CGenericDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
    BOOL CreateDlg(CWnd* pParent, InterfaceNode* pNode);

    virtual void    InitData(bool bFirstInit = true);
    virtual void    CheckRestrictions();
    virtual void    DoCtrlUpdate();      // default version calls CheckRestrictions()


    InterfaceNode* GetInterfaceNode();

protected:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGridCtrlDemoDlg)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

    InterfaceNode* m_pNode;
protected:
    // Generated message map functions
    //{{AFX_MSG(CGenericDialog)
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // GENERIC_DIALOG_H

