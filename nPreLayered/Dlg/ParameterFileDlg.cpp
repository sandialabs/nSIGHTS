///////////////////////////////////////////////////////////////////////////////////
//
// ParameterFileDlg.cpp
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


// ParameterFPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ParameterFileDlg.h"

#include <genClass/U_Str.h>
#include <objClass/DO_Curve.h>

#include <Var/G_Control.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParameterFileDlg dialog


CParameterFixedDlg::CParameterFixedDlg(FixedCurve& fixedCurve, Parameter::CurveParameterType pType, CWnd* pParent /*=NULL*/)
  : CGenericDialog(CParameterFixedDlg::IDD, pParent),
    m_fixedCurve(fixedCurve),
    m_pType(pType), m_ucurveObjRef(typeid(DO_Curve))
{
  //{{AFX_DATA_INIT(CParameterFileDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

BOOL CParameterFixedDlg::OnInitDialog()
{
  switch (m_pType) {
  case Parameter::cptRadial: {
    SetWindowText("f(radius) Curve Dialog");
    GetDlgItem(IDC_CURVEY_UNITS)->SetWindowText("Distance units in curve");
    GetDlgItem(IDC_CURVEY_ISLOG10)->SetWindowText("Distance is log10");
    break;
  }
  case Parameter::cptPressure: {
    SetWindowText("f(Pressure) Curve Dialog");
    GetDlgItem(IDC_CURVEY_UNITS)->SetWindowText("Pressure units in curve");
    GetDlgItem(IDC_CURVEY_ISLOG10)->SetWindowText("Pressure is log10");
    break;
  }
  case Parameter::cptTime: {
    SetWindowText("f(time) Curve Dialog");
    GetDlgItem(IDC_CURVEY_UNITS)->SetWindowText("Time units in curve");
    GetDlgItem(IDC_CURVEY_ISLOG10)->SetWindowText("Time is log10");
    break;
  }
  }

  if (control.UseCurveFiles())
  {
    GetDlgItem(IDC_COMBO1)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CURVE_SOURCE)->SetWindowText("Curve ID in file");
    m_fixedCurve.curveFile.GetIDs(curveIDs);
    if (!curveIDs.SearchForKey(m_fixedCurve.curveID, m_curvIDindex))
      m_curvIDindex = 0;
    m_ucurveID.AddBasicEntry(curveIDs);
    m_ucurveID.Set(this, IDC_COMBO6, &m_curvIDindex );
  }
  else
  {
    GetDlgItem(IDC_COMBO6)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CURVE_SOURCE)->SetWindowText("Curve object");
    m_curveObjRef = m_fixedCurve.curveObjRef;
    m_ucurveObjRef.Set(this, IDC_COMBO1, m_curveObjRef);
  }

  m_xUnitIndex = m_fixedCurve.xUnitIndex;
  m_yUnitIndex = m_fixedCurve.yUnitIndex;
  m_xIsLog10   = m_fixedCurve.xIsLog10;
  m_yIsLog10   = m_fixedCurve.yIsLog10;

  m_uxUnitIndex.Set(this, IDC_COMBO2, &m_xUnitIndex);
  m_uyUnitIndex.Set(this, IDC_COMBO3, &m_yUnitIndex);

  m_uxIsLog10.Set(this, IDC_COMBO4, m_xIsLog10);
  m_uyIsLog10.Set(this, IDC_COMBO5, m_yIsLog10);

  CDialog::OnInitDialog();

  return TRUE;
}


void CParameterFixedDlg::OnOK()
{
  if (control.UseCurveFiles())
  {
    if (!curveIDs.IsEmpty())
      CopyString(m_fixedCurve.curveID, curveIDs[m_curvIDindex], DC_CurveWithID::curveIDlen);
  }
  else
  {
    m_fixedCurve.curveObjRef = m_curveObjRef;
  }

  m_fixedCurve.xUnitIndex = m_xUnitIndex;
  m_fixedCurve.yUnitIndex = m_yUnitIndex;
  m_fixedCurve.xIsLog10 = m_xIsLog10;
  m_fixedCurve.yIsLog10 = m_yIsLog10;

  CDialog::OnOK();
}

void CParameterFixedDlg::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CParameterFileDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParameterFixedDlg, CGenericDialog)
  //{{AFX_MSG_MAP(CParameterFixedDlg)
  // NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParameterFileDlg message handlers
