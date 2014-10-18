///////////////////////////////////////////////////////////////////////////////////
//
// DlgOutputFileSetupOptimization.cpp
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


// DlgOutputFileSetupOptimization.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include <osMain/G_Optimize.h>

#include "DlgOutputFileSetupOptimization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgOutputFileSetupOptimization dialog
IMPLEMENT_DYNCREATE(DlgOutputFileSetupOptimization, DlgOutputFileSetupBase)

DlgOutputFileSetupOptimization::DlgOutputFileSetupOptimization(CWnd* pParent /*=NULL*/)
  : DlgOutputFileSetupBase(DlgOutputFileSetupOptimization::IDD, pParent,
                           optOutputDesc,
                           "Sim Opt Output Files (*.nOpt)|*.nOpt|All Files (*.*)|*.*|||",
                           "Optimization output file name", "nOpt")
{
  //{{AFX_DATA_INIT(DlgOutputFileSetupOptimization)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(DlgOutputFileSetupOptimization, DlgOutputFileSetupBase)
  //{{AFX_MSG_MAP(DlgOutputFileSetupOptimization)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOutputFileSetupOptimization message handlers


void DlgOutputFileSetupOptimization::DoCtrlUpdate()
{
  CheckRestrictions();
}


void DlgOutputFileSetupOptimization::CheckRestrictions()
{
  DlgOutputFileSetupBase::CheckRestrictions();

  m_uwriteResiduals.EnableWindow(optOutputDesc.writeFile);
  m_uwriteCovariance.EnableWindow(optOutputDesc.writeFile && optimize.HasCovariance());
  m_uwriteJacobian.EnableWindow(optOutputDesc.writeFile && optimize.HasCovariance());
}

BOOL DlgOutputFileSetupOptimization::OnInitDialog()
{
  DlgOutputFileSetupBase::OnInitDialog();

  m_uwriteResiduals.Set(this, IDC_COMBO2, optOutputDesc.writeResiduals);
  m_uwriteCovariance.Set(this, IDC_COMBO3, optOutputDesc.writeCovariance);
  m_uwriteJacobian.Set(this, IDC_COMBO4, optOutputDesc.writeJacobian);

  CheckRestrictions();
  return TRUE;
}

void DlgOutputFileSetupOptimization::ResetAllControls()
{
  DlgOutputFileSetupBase::ResetAllControls();
  CheckRestrictions();
}
