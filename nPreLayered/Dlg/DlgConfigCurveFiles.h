///////////////////////////////////////////////////////////////////////////////////
//
// DlgConfigCurveFiles.h
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


#if !defined(AFX_DLGCONFIGCURVEFILES_H__7AD71FB4_D783_11D3_BCC9_00104B4C74FD__INCLUDED_)
#define AFX_DLGCONFIGCURVEFILES_H__7AD71FB4_D783_11D3_BCC9_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConfigCurveFiles.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgConfigCurveFiles dialog

#include <genInterface/GenericDialog.h>
#include <Var/C_CurveFile.h>
#include <genCtrl/FileCtrl.h>


class DlgConfigCurveFiles : public CGenericDialog
{
  DECLARE_DYNCREATE(DlgConfigCurveFiles)
// Construction
public:
  DlgConfigCurveFiles(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(DlgConfigCurveFiles)
  enum { IDD = IDD_CFG_CURVEFILES };
  //}}AFX_DATA

  CFileCtrl  m_utestZoneBCCurveFile;
  CFileCtrl  m_ufrParameterCurveFile;
  CFileCtrl  m_ufPParameterCurveFile;
  CFileCtrl  m_uftParameterCurveFile;

  virtual void  DoCtrlUpdate(int reason);


// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(DlgConfigCurveFiles)
  afx_msg void OnButtonReload();
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONFIGCURVEFILES_H__7AD71FB4_D783_11D3_BCC9_00104B4C74FD__INCLUDED_)
