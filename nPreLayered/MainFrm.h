///////////////////////////////////////////////////////////////////////////////////
//
// MainFrm.h
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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


// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__361E7868_C803_11D3_90C2_00104B4C74FD__INCLUDED_)
#define AFX_MAINFRM_H__361E7868_C803_11D3_90C2_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <genInterface/OutlookFrame.h>


class CNPreView;

class CMainFrame : public COutlookFrame
{
  DECLARE_DYNCREATE(CMainFrame)
protected: // create from serialization only
  CMainFrame();

// Operations
public:
  void ResetInterface(CDocument* pDocument, bool bRestoreContext);
  bool VerifyBarState(LPCTSTR lpszProfileName);

// from CProjectFrame
  virtual void LoadTemplateFile(const char* tfName);

protected:
  virtual void  CreateInterface();

  CNPreView* GetNPreView();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMainFrame)
public:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
  virtual BOOL DestroyWindow();
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CMainFrame();


private:
  void  ListAllErrors();
  bool  RunSetup();
  void  RunComplete();
  bool  FinalOptOK();


// Generated message map functions
protected:
  //{{AFX_MSG(CMainFrame)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSaveCommandLine();
  afx_msg void OnListCurrent();
  afx_msg void OnListError();
  afx_msg void OnListAll();
  afx_msg void OnListCalculated();
  afx_msg void OnListAllErrors();
  afx_msg void OnListMessages();
  afx_msg void OnAutoField();
  afx_msg void OnAutoSequence();
  afx_msg void OnAutoFit();
  afx_msg void OnRunVerbose();
  afx_msg void OnRunMinimal();
  afx_msg void OnRunNoThreads();
  afx_msg void OnRunOptFinal();
  afx_msg void OnRunMinimizeMain();
  afx_msg void OnUpdateRunMinimizeMain(CCmdUI* pCmdUI);
  afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
  //}}AFX_MSG

  afx_msg void OnNpreEditor(UINT nID);
  afx_msg void OnUpdateSaveCommandLine(CCmdUI* pCmdUI);
  afx_msg void OnUpdateNpreEditor(CCmdUI* pCmdUI);
  afx_msg void OnUpdateListEditor(CCmdUI* pCmdUI);
  afx_msg void OnUpdateAutoEditor(CCmdUI* pCmdUI);
  afx_msg void OnUpdateRunOptFinal(CCmdUI* pCmdUI);

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__361E7868_C803_11D3_90C2_00104B4C74FD__INCLUDED_)
