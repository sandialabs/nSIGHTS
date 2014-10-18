///////////////////////////////////////////////////////////////////////////////////
//
// DlgPointsParametersYOptimization.cpp
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


// DlgPointsParametersYOptimization.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgPointsParametersYOptimization.h"

#include <UI/NPreInterfaceApp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgPointsParametersYOptimization dialog

IMPLEMENT_DYNCREATE(DlgPointsParametersYOptimization,CGenericDialog)

DlgPointsParametersYOptimization::DlgPointsParametersYOptimization(CWnd* pParent /*=NULL*/)
  : CGenericDialog(DlgPointsParametersYOptimization::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgPointsParametersYOptimization)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void DlgPointsParametersYOptimization::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgPointsParametersYOptimization)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgPointsParametersYOptimization, CGenericDialog)
  //{{AFX_MSG_MAP(DlgPointsParametersYOptimization)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgPointsParametersYOptimization message handlers

BOOL DlgPointsParametersYOptimization::OnInitDialog()
{
  CGenericDialog::OnInitDialog();
  TabbedFolderNodePointsParameterInside* pFolder =
    static_cast<TabbedFolderNodePointsParameterInside*>(GetParent()->GetParent());

  m_pParameter = pFolder->m_pParameter;
  m_pPointsCurve = m_pParameter->GetPointsCurve();

  m_tmpYMin = m_pParameter->parVal;
  m_tmpYMin.SetMetricVal(m_pPointsCurve->yoptMinLim);

  m_tmpYMax = m_pParameter->parVal;
  m_tmpYMax.SetMetricVal(m_pPointsCurve->yoptMaxLim);


  m_uyoptMinLim.Set(this, IDC_COMBO1, IDC_EDIT1, &m_tmpYMin,
                    m_pParameter->GetOverrideFormat(), m_pParameter->minLimit, m_pParameter->maxLimit);
  m_uyoptMinLim.m_uRealCtrl.AddCallback(this);  // kluge to update actual
  m_uyoptMinLim.m_uIndexCtrl.AddCallback(this);

  m_uyoptMaxLim.Set(this, IDC_COMBO2, IDC_EDIT2, &m_tmpYMax,
                    m_pParameter->GetOverrideFormat(), m_pParameter->minLimit, m_pParameter->maxLimit);
  m_uyoptMaxLim.m_uRealCtrl.AddCallback(this);

  m_uyoptIsLinear.Set(this, IDC_COMBO3,  m_pPointsCurve->yoptIsLinear, "Logarithmic", "Linear");

  m_uyoptParSigma .Set(this, IDC_EDIT3, m_pPointsCurve->yoptParSigma , SC_SciFormat(5), 1.0E-10, 1.0);

  m_uxoptIsLinear.Set(this, IDC_COMBO4,  m_pPointsCurve->xoptIsLinear, "Logarithmic", "Linear");

  m_uxoptParSigma.Set(this, IDC_EDIT4, m_pPointsCurve->xoptParSigma , SC_SciFormat(5), 1.0E-10, 1.0);

  return TRUE;
}

void DlgPointsParametersYOptimization::DoCtrlUpdate()
{
  m_uyoptMaxLim.DoUnitConvChange(m_tmpYMin.GetConvIndex());

  m_pPointsCurve->yoptMinLim = m_tmpYMin.GetMetricVal();
  m_pPointsCurve->yoptMaxLim = m_tmpYMax.GetMetricVal();

}

void DlgPointsParametersYOptimization::ResetAllControls()
{
  m_uyoptMinLim.DoCtrlReset();
  m_uyoptMaxLim.DoCtrlReset();
  m_uyoptIsLinear.DoCtrlReset();
  m_uyoptParSigma.DoCtrlReset();
  m_uxoptIsLinear.DoCtrlReset();
  m_uxoptParSigma.DoCtrlReset();
}


