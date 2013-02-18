///////////////////////////////////////////////////////////////////////////////////
//
// FolderCtrl.h
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
// DESCRIPTION: Windows specific code for implementing basic controls with
//              callbacks on resource defined dialog forms.
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(CFOLDERCTRL)
#define CFOLDERCTRL

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FolderCtrl.h : header file
//
#include "TextCtrl.h"
#include "ButtonCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CFolderCtrl window


class FolderTextCtrl : public CTextCtrl
{
private:
    char*           m_pFolder;


public:
    FolderTextCtrl();

    void            Set(CWnd* pParent, int nIDEdit, char* inFolder);


    // CUpdateableCtrl virtual
    virtual void    DoCtrlReset();

    // Operations
public:
    virtual void    UpdateText();

    void            DoBrowseChange();

};


class CFolderCtrl : public CCtrlUpdate, public CUpdateableCtrl
{
public:
    FolderTextCtrl  m_uFolderTextCtrl;
    CButtonCtrl     m_uBrowseButton;
    char*           m_pFolderName;
    const char*     m_pFolderDialogTitle;
    bool            doFileOpen;


public:
    CFolderCtrl();
    virtual ~CFolderCtrl();

    void Set(CWnd* pParent, int nIDText, int nIDButton, char* pFolder,
             const char* folderDialogTitle, bool openFile = true);


    BOOL EnableWindowEx(BOOL bEnable = TRUE);

    // CUpdateableCtrl virtual
    virtual void    DoCtrlReset();


    //  callback to reset file text after browse
    virtual void    DoCtrlUpdate();
    virtual void    DoCtrlUpdate(int reason);
    BOOL            DoBrowseChange();


public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFolderCtrl)
    //}}AFX_VIRTUAL

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // FOLDERCTRL_H

