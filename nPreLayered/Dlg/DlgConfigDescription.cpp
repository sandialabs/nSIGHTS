///////////////////////////////////////////////////////////////////////////////////
//
// DlgConfigDescription.cpp
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


// DlgConfigDescription.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include "DlgConfigDescription.h"

#include <Var/G_Control.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDescriptionEdit::CDescriptionEdit()
{
}

CDescriptionEdit::~CDescriptionEdit()
{
}


BEGIN_MESSAGE_MAP(CDescriptionEdit, CRichEditCtrl)
  //{{AFX_MSG_MAP(CDescriptionEdit)
  ON_WM_GETDLGCODE()
  ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCDescriptionEdit message handlers

UINT CDescriptionEdit::OnGetDlgCode()
{
  return DLGC_WANTARROWS|DLGC_WANTALLKEYS|DLGC_WANTCHARS|DLGC_WANTTAB;
}


void CDescriptionEdit::OnKillfocus()
{
  GetWindowText(testDescription);
}


/////////////////////////////////////////////////////////////////////////////
// DlgConfigDescription dialog

IMPLEMENT_DYNCREATE(DlgConfigDescription, CGenericDialog)

DlgConfigDescription::DlgConfigDescription(CWnd* pParent /*=NULL*/)
  : CGenericDialog(DlgConfigDescription::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgConfigDescription)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void DlgConfigDescription::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgConfigDescription)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_RICHEDIT1, testDescription);
}


BEGIN_MESSAGE_MAP(DlgConfigDescription, CGenericDialog)
  //{{AFX_MSG_MAP(DlgConfigDescription)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgConfigDescription message handlers

BOOL DlgConfigDescription::OnInitDialog()
{
  CGenericDialog::OnInitDialog();
  m_SrcEdit.SubclassDlgItem(IDC_RICHEDIT1, this);

  SetControlInfo(IDC_RICHEDIT1, resizeBoth);
  return TRUE;
}
