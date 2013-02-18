///////////////////////////////////////////////////////////////////////////////////
//
// SingleFrame.h
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
//              ProjectFrame derived class for applications with a single menu tree (like nPost)
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef SINGLEFRAME_H
#define SINGLEFRAME_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <genApp/ProjectFrame.h>
#include <genApp/C_MenuRoot.h>
#include <genMFC/ProjectSplitterWnd.h>

class CProjectView;
class FormContainer;

class CSingleFrame : public CProjectFrame
{
    DECLARE_DYNCREATE(CSingleFrame)

    protected:
    CProjectSplitterWnd     m_wndSplitter;


    MenuRootC*              frameRoot;              // must set before create
    // Operations
public:
    CSingleFrame();
    virtual                 ~CSingleFrame();

    CProjectView*           GetFormView();
    static FormContainer*   GetFormContainer();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSingleFrame)
public:
    virtual BOOL            PreCreateWindow(CREATESTRUCT& cs);
protected:
    virtual BOOL            OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual BOOL            OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual void            CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:
    //{{AFX_MSG(CSingleFrame)
    afx_msg int             OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

#endif // SINGLEFRAME_H

