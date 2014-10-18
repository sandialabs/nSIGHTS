///////////////////////////////////////////////////////////////////////////////////
//
// SampVarUnitsDlg.cpp
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


// ParameterSampledDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "SampVarUnitsDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampVarUnitsDlg dialog


CSampVarUnitsDlg::CSampVarUnitsDlg(SampVarUnits& sampVarUnits, const SC_RealFormat* overrideFormat,
                                   const double& minMetricVal, const double& maxMetricVal,
                                   CWnd* pParent /*=NULL*/)
  : CDialog(CSampVarUnitsDlg::IDD, pParent),
    m_sampVarUnits(sampVarUnits),
    m_poverrideFormat(overrideFormat),
    m_minMetricVal(minMetricVal),
    m_maxMetricVal(maxMetricVal)
{
  //{{AFX_DATA_INIT(CSampVarUnitsDlg)
  //}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CSampVarUnitsDlg, CDialog)
  //{{AFX_MSG_MAP(CSampVarUnitsDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampVarUnitsDlg message handlers

BOOL CSampVarUnitsDlg::OnInitDialog()
{
  // initialize members
  m_svDistribution  = m_sampVarUnits.svDistribution;

  m_distMean.InitVal(m_sampVarUnits.unitIndex, m_sampVarUnits.distMean);
  m_distStdDev.InitVal(m_sampVarUnits.unitIndex, m_sampVarUnits.distStdDev);
  m_distPeak.InitVal(m_sampVarUnits.unitIndex, m_sampVarUnits.distPeak);
  m_distMin.InitVal(m_sampVarUnits.unitIndex, m_sampVarUnits.distMin);
  m_distMax.InitVal(m_sampVarUnits.unitIndex, m_sampVarUnits.distMax);
  m_distLogStdDev = m_sampVarUnits.distLogStdDev ;

  //  set up ctrls
  SC_StringArray sampStrs;
  SampleDistributionBase::GetDistStrings(sampStrs);

  m_usvDistribution.AddMappedEntry(sampStrs[int(SampleDistributionBase::svdNormal)], SampleDistributionBase::svdNormal);
  m_usvDistribution.AddMappedEntry(sampStrs[int(SampleDistributionBase::svdLogNormal)], SampleDistributionBase::svdLogNormal);
  m_usvDistribution.AddMappedEntry(sampStrs[int(SampleDistributionBase::svdUniform)], SampleDistributionBase::svdUniform);
  m_usvDistribution.AddMappedEntry(sampStrs[int(SampleDistributionBase::svdLogUniform)], SampleDistributionBase::svdLogUniform);
  m_usvDistribution.AddMappedEntry(sampStrs[int(SampleDistributionBase::svdTriangular)], SampleDistributionBase::svdTriangular);
  m_usvDistribution.AddMappedEntry(sampStrs[int(SampleDistributionBase::svdLogTriangular)], SampleDistributionBase::svdLogTriangular);

  m_usvDistribution.Set(this, IDC_COMBO1,  (int*) (&m_svDistribution));
  m_usvDistribution.AddCallback(this, 0);

  m_udistMean.Set(this, IDC_COMBO2, IDC_EDIT1, &m_distMean,
                  m_poverrideFormat, m_minMetricVal, m_maxMetricVal);
  m_udistMean.m_uIndexCtrl.AddCallback(this, 1);

  m_udistStdDev.Set(this, IDC_COMBO3, IDC_EDIT2, &m_distStdDev,
                    m_poverrideFormat, m_minMetricVal, m_maxMetricVal);

  m_udistLogStdDev.Set(this, IDC_EDIT3, m_distLogStdDev, SC_DecFormat(3), 0.001, 10.0);

  m_udistPeak.Set(this, IDC_COMBO4, IDC_EDIT4, &m_distPeak,
                  m_poverrideFormat, m_minMetricVal, m_maxMetricVal);
  m_udistPeak.m_uIndexCtrl.AddCallback(this, 2);


  m_udistMin.Set(this, IDC_COMBO5, IDC_EDIT5, &m_distMin,
                 m_poverrideFormat, m_minMetricVal, m_maxMetricVal);
  m_udistMin.m_uIndexCtrl.AddCallback(this, 3);

  m_udistMax.Set(this, IDC_COMBO6, IDC_EDIT6, &m_distMax,
                 m_poverrideFormat, m_minMetricVal, m_maxMetricVal);
  m_udistMin.m_uIndexCtrl.AddCallback(this, 4);

  CDialog::OnInitDialog();

  CheckRestrictions();

  return TRUE;
}

void CSampVarUnitsDlg::OnOK()
{
  // kluge to make sure that ctrl with current focus gets updated
  GotoDlgCtrl(GetDlgItem(IDOK));

  m_sampVarUnits.svDistribution = m_svDistribution;

  m_sampVarUnits.distMean = m_distMean.GetMetricVal();
  m_sampVarUnits.distStdDev = m_distStdDev.GetMetricVal();
  m_sampVarUnits.distLogStdDev = m_distLogStdDev;
  m_sampVarUnits.distPeak = m_distPeak.GetMetricVal();
  m_sampVarUnits.distMin  = m_distMin.GetMetricVal();
  m_sampVarUnits.distMax  = m_distMax.GetMetricVal();

  CDialog::OnOK();
}

void CSampVarUnitsDlg::CheckRestrictions()
{
  bool isNormal = SampleDistributionBase::IsNormal(m_svDistribution);

  m_udistMean.EnableWindowEx(isNormal);
  m_udistStdDev.EnableWindowEx(isNormal && SampleDistributionBase::IsLinear(m_svDistribution));
  m_udistLogStdDev.EnableWindow(isNormal && SampleDistributionBase::IsLog(m_svDistribution));

  m_udistPeak.EnableWindowEx(SampleDistributionBase::IsTriangular(m_svDistribution));

  m_udistMin.EnableWindowEx(SampleDistributionBase::UsesMinMax(m_svDistribution));
  m_udistMax.EnableWindowEx(SampleDistributionBase::UsesMinMax(m_svDistribution));
}



void CSampVarUnitsDlg::DoCtrlUpdate(int reason)
{
  int newConvIndex = -1;
  switch (reason) {
  case 0 : {
    CheckRestrictions();
    return;
  }
  case 1 : {
    newConvIndex = m_distMean.GetConvIndex();
    break;
  }
  case 2 : {
    newConvIndex = m_distPeak.GetConvIndex();
    break;
  }
  case 3: {
    newConvIndex = m_distMin.GetConvIndex();
    break;
  }
  case 4: {
    newConvIndex = m_distMax.GetConvIndex();
    break;
  }


  default:
    break;
  }

  m_udistMean.DoUnitConvChange(newConvIndex);
  m_udistStdDev.DoUnitConvChange(newConvIndex);
  m_udistPeak.DoUnitConvChange(newConvIndex);
  m_udistMin.DoUnitConvChange(newConvIndex);
  m_udistMax.DoUnitConvChange(newConvIndex);
}





