///////////////////////////////////////////////////////////////////////////////////
//
// DlgConfigMatrix.cpp
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


// DlgConfigMatrix.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgConfigMatrix.h"

#include <Var/G_Parameter.h>
#include <Var/G_Control.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgConfigMatrix dialog

IMPLEMENT_DYNCREATE(DlgConfigMatrix,CGenericDialog)

DlgConfigMatrix::DlgConfigMatrix(CWnd* pParent /*=NULL*/)
  : CGenericDialog(DlgConfigMatrix::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgConfigMatrix)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}



BOOL DlgConfigMatrix::OnInitDialog()
{
  m_udualGeometryIsPrismatic.Set(this, IDC_COMBO1, dualGeometryIsPrismatic, "Spherical", "Prismatic");
  m_udualGeometryIsPrismatic.AddCallback(this);
  m_uenterDualAlpha.Set(this, IDC_COMBO2, enterDualAlpha, "Calculated", "Entered");
  m_uenterDualAlpha.AddCallback(this);
  CGenericDialog::OnInitDialog();

  InitData(true);

  return TRUE;
}

void DlgConfigMatrix::DoCtrlUpdate()
{
  parameters.SetForFlags();
}


void DlgConfigMatrix::ResetAllControls()
{
  m_uenterDualAlpha.DoCtrlReset();
  m_udualGeometryIsPrismatic.DoCtrlReset();
}

BEGIN_MESSAGE_MAP(DlgConfigMatrix, CGenericDialog)
  //{{AFX_MSG_MAP(DlgConfigMatrix)
  // NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgConfigMatrix message handlers
