///////////////////////////////////////////////////////////////////////////////////
//
// DlgSeqDynamicStep.cpp
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


// DlgSeqDynamicStep.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include "DlgSeqDynamicStep.h"

#include <Var/G_Sequence.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgSeqDynamicStep dialog

IMPLEMENT_DYNCREATE(DlgSeqDynamicStep, CGenericDialog)

DlgSeqDynamicStep::DlgSeqDynamicStep(CWnd* pParent /*=NULL*/)
  : CGenericDialog(DlgSeqDynamicStep::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgSeqDynamicStep)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void DlgSeqDynamicStep::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgSeqDynamicStep)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSeqDynamicStep, CGenericDialog)
  //{{AFX_MSG_MAP(DlgSeqDynamicStep)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgSeqDynamicStep message handlers

BOOL DlgSeqDynamicStep::OnInitDialog()
{
  m_umaximumPressureChange.Set(this, IDC_COMBO1, IDC_EDIT1, &maximumPressureChange,
                               &SC_SciFormat(5), 1E-04, 5.0);
  m_uminimumPressureChange.Set(this, IDC_COMBO2, IDC_EDIT2, &minimumPressureChange,
                               &SC_SciFormat(5), 1E-04, 0.5);
  m_umaximumFlowRateChange.Set(this, IDC_COMBO3, IDC_EDIT3, &maximumFlowRateChange,
                               &SC_SciFormat(5), 1E-06, 5.0);
  m_uminimumFlowRateChange.Set(this, IDC_COMBO4, IDC_EDIT4, &minimumFlowRateChange,
                               &SC_SciFormat(5), 1E-06, 0.5);
  m_umaxNumberOfTS.Set(this, IDC_EDIT5, maxNumberOfTS, 100, 100000);

  CGenericDialog::OnInitDialog();

  InitData(true);

  return TRUE;
}

void DlgSeqDynamicStep::ResetAllControls()
{
  m_umaximumPressureChange.DoCtrlReset();
  m_uminimumPressureChange.DoCtrlReset();
  m_umaximumFlowRateChange.DoCtrlReset();
  m_uminimumFlowRateChange.DoCtrlReset();
  m_umaxNumberOfTS.DoCtrlReset();
}
