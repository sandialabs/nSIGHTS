///////////////////////////////////////////////////////////////////////////////////
//
// UnconfinedObsWellDlg.cpp
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


#include "stdafx.h"
#include "Resource.h"
#include "UnconfinedObsWellDlg.h"

#include <genClass/U_Str.h>

#include <Var/G_Control.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParameterFileDlg dialog


CUnconfinedObsWellDlg::CUnconfinedObsWellDlg(DataCaptureSpec& inSpec, CWnd* pParent /*=NULL*/)
  : CGenericDialog(CUnconfinedObsWellDlg::IDD, pParent),
    dataCaptureSpec(inSpec)
{
  m_radiusData  = dataCaptureSpec.radiusData;
  m_zvalueIsRatio = dataCaptureSpec.zvalueIsRatio;
  m_actualZ   = dataCaptureSpec.actualZ;
  m_normalizedZ = dataCaptureSpec.normalizedZ;
}

BOOL CUnconfinedObsWellDlg::OnInitDialog()
{

  m_uradiusData.Set(this, IDC_COMBO1, IDC_EDIT1, &m_radiusData,
                    NULL, 0.0, 1.0E+05);

  m_uzvalueIsRatio.Set(this, IDC_COMBO2, m_zvalueIsRatio,
                       "Actual", "Ratio");
  m_uzvalueIsRatio.AddCallback(this);

  m_uactualZ.Set(this, IDC_COMBO3, IDC_EDIT2, &m_actualZ,
                 NULL, 0.0, 1.0E+05);

  m_unormalizedZ.Set(this, IDC_EDIT3, m_normalizedZ, SC_DecFormat(3), 0.0, 1.0);


  CDialog::OnInitDialog();

  DoCtrlUpdate();

  InitData(true);

  return TRUE;
}

void CUnconfinedObsWellDlg::CheckRestrictions()
{
  m_uactualZ.EnableWindowEx(!m_zvalueIsRatio);
  m_unormalizedZ.EnableWindow(m_zvalueIsRatio);
}


void CUnconfinedObsWellDlg::ResetAllControls()
{
  m_uradiusData.DoCtrlReset();
  m_uactualZ.DoCtrlReset();
  m_unormalizedZ.DoCtrlReset();
}


void CUnconfinedObsWellDlg::OnOK()
{
  // kluge to make sure that ctrl with current focus gets updated
  GotoDlgCtrl(GetDlgItem(IDOK));


  dataCaptureSpec.radiusData = m_radiusData;
  dataCaptureSpec.zvalueIsRatio = m_zvalueIsRatio;
  if (m_zvalueIsRatio)
    dataCaptureSpec.normalizedZ = m_normalizedZ;
  else
    dataCaptureSpec.actualZ = m_actualZ;

  CDialog::OnOK();
}

void CUnconfinedObsWellDlg::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CParameterFileDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnconfinedObsWellDlg, CGenericDialog)
  //{{AFX_MSG_MAP(CUnconfinedObsWellDlg)
  // NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParameterFileDlg message handlers
