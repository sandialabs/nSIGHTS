///////////////////////////////////////////////////////////////////////////////////
//
// DlgOutputFileSetupRange.h
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


#if !defined(AFX_DLGOUTPUTFILESETUPRANGE_H__E5CE4FC2_88D2_11D4_94B7_D0539FADE170__INCLUDED_)
#define AFX_DLGOUTPUTFILESETUPRANGE_H__E5CE4FC2_88D2_11D4_94B7_D0539FADE170__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Dlg/DlgOutputFileSetupBase.h>

// DlgOutputFileSetupRange dialog

class DlgOutputFileSetupRange : public DlgOutputFileSetupBase
{
  DECLARE_DYNCREATE(DlgOutputFileSetupRange)

public:
  DlgOutputFileSetupRange(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(DlgOutputFileSetupRange)
  enum { IDD = IDD_OUTFILE_RANGE};
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(DlgOutputFileSetupRange)
  //}}AFX_VIRTUAL

  virtual void  ResetAllControls();


protected:
  // Generated message map functions
  //{{AFX_MSG(DlgOutputFileSetupRange)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOUTPUTFILESETUPRANGE_H__E5CE4FC2_88D2_11D4_94B7_D0539FADE170__INCLUDED_)
