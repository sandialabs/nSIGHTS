///////////////////////////////////////////////////////////////////////////////////
//
// SequenceFlowHistDlg.cpp
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

#include <genClass/U_Str.h>

#include <Var/G_Control.h>
#include <osMain/G_Vary.h>

#include "VaryVarUnitsDlg.h"
#include "SampVarUnitsDlg.h"
#include "SequenceFlowHistDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSequenceFlowHistDlg dialog


CSequenceFlowHistDlg::CSequenceFlowHistDlg(Sequence& sequence, bool isTSZone, FlowHistBase& fhData, bool isFlow, CWnd* pParent)
  : CSequenceTimeStepDlg(sequence, isTSZone, CSequenceFlowHistDlg::IDD, pParent),
    m_fhData(fhData), m_isFlow(isFlow)
{
  //{{AFX_DATA_INIT(CSequenceFlowHistDlg)
  //}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CSequenceFlowHistDlg, CDialog)
  //{{AFX_MSG_MAP(CSequenceFlowHistDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSequenceFlowHistDlg message handlers

BOOL CSequenceFlowHistDlg::OnInitDialog()
{
  CSequenceTimeStepDlg::OnInitDialog();


  if (m_isFlow)
  {
    SetWindowText("Flow Sequence Setup Dialog");
    GetDlgItem(IDC_FLOWHIST_TYPE)->SetWindowText("Flow type");
    GetDlgItem(IDC_FLOWHIST_VAL)->SetWindowText("Fixed flow");
    m_minFixed = -1.0E6;
    m_maxFixed = 1.0E6;
    m_minSampleOffset = -10.0;
    m_maxSampleOffset = -10.0;
  }
  else
  {
    SetWindowText("History Sequence Setup Dialog");
    if (!NSUnitsBase::headFlag)
    {
      GetDlgItem(IDC_FLOWHIST_TYPE)->SetWindowText("Pressure type");
      GetDlgItem(IDC_FLOWHIST_VAL)->SetWindowText("Fixed pressure");
    }
    else
    {
      GetDlgItem(IDC_FLOWHIST_TYPE)->SetWindowText("Head type");
      GetDlgItem(IDC_FLOWHIST_VAL)->SetWindowText("Fixed head");
    }

    m_minFixed = 0.0;
    m_maxFixed = 1.0E+06;
    m_minSampleOffset = -1.0E+06;
    m_maxSampleOffset = 1.0E+06;
  }

  m_valueType     = m_fhData.valueType;
  m_fixedValue    = m_fhData.fixedValue;
  m_wellboreStorage = m_fhData.wellboreStorage;

  m_uvalueType.AddMappedEntry("Fixed", FlowHistBase::vtFixed);
  if (!control.IsSampleSim())
  {
    if (vary.IsRange())
      m_uvalueType.AddMappedEntry("Range", FlowHistBase::vtVary);
    else
      m_uvalueType.AddMappedEntry("Suite", FlowHistBase::vtVary);
  }
  m_uvalueType.AddMappedEntry("Curve", FlowHistBase::vtCurve);
  if (control.IsSampleSim())
  {
    m_uvalueType.AddMappedEntry("Sampled", FlowHistBase::vtSampled);
    m_uvalueType.AddMappedEntry("Curve + Sample", FlowHistBase::vtCurveSampled);
  }
  m_uvalueType.Set(this, IDC_COMBO6,  &m_valueType);
  m_uvalueType.AddCallback(this, 0);

  m_ufixedValue.Set(this, IDC_COMBO7, IDC_EDIT6, &m_fixedValue, NULL, m_minFixed, m_maxFixed);

  m_usampVaryValue.Set(this, IDC_EDIT7, "SampVaryCtrl");
  m_usampVaryValue.AddCallback(this, 1);

  m_uwellboreStorage.AddMappedEntry("None", FlowHistBase::wsNone);
  m_uwellboreStorage.AddMappedEntry("Isolated", FlowHistBase::wsIsolated);
  if ( control.IsLiquid())
    m_uwellboreStorage.AddMappedEntry("Open Hole", FlowHistBase::wsOpen);
  m_uwellboreStorage.Set(this, IDC_COMBO8,  &m_wellboreStorage);

  DoCtrlUpdate();

  CDialog::OnInitDialog();

  return TRUE;
}


void CSequenceFlowHistDlg::SetSampVaryFlags()
{
  sampleOK = control.IsSampleSim() &&
             ((m_valueType == FlowHistBase::vtSampled) ||
              (m_valueType == FlowHistBase::vtCurveSampled));

  varyOK = (!control.IsSampleSim()) && (m_valueType == FlowHistBase::vtVary);

}

void CSequenceFlowHistDlg::SetSampVaryText()
{
  SetSampVaryFlags();

  m_usampVaryValue.EnableWindow(sampleOK || varyOK );
  if (sampleOK || varyOK )
  {
    char menuStr[40];
    if (sampleOK)
      m_fhData.sampleValue.GetMenuStr(menuStr, 40);
    else
      m_fhData.varyValue.GetMenuStr(menuStr, 40);

    m_usampVaryValue.ResetText(menuStr);
  }
  else
  {
    m_usampVaryValue.ResetText("n/a");
  }

}

void CSequenceFlowHistDlg::DoCtrlUpdate()
{
  CSequenceTimeStepDlg::DoCtrlUpdate();

  m_ufixedValue.EnableWindowEx(m_valueType == FlowHistBase::vtFixed);

  SetSampVaryText();
}

void CSequenceFlowHistDlg::DoCtrlUpdate(int reason)
{
  if (reason == 0)
  {
    m_fhData.ClearWasActive();
    DoCtrlUpdate();
  }
  else
  {
    m_uwellboreStorage.SetFocus();
    SetSampVaryFlags();
    if (varyOK)
    {
      VaryVarUnitsDlg dlg(m_fhData.varyValue, NULL, m_minFixed, m_maxFixed);
    }
    if (sampleOK)
    {
      double sampleMin = m_minFixed;
      double sampleMax = m_maxFixed;
      if (m_valueType == FlowHistBase::vtCurveSampled)
      {
        sampleMin = m_minSampleOffset;
        sampleMax = m_maxSampleOffset;
      }
      CSampVarUnitsDlg dlg(m_fhData.sampleValue, NULL, sampleMin, sampleMax);
      dlg.DoModal();
    }

    SetSampVaryText();
  }
}


void CSequenceFlowHistDlg::OnOK()
{
  // kluge to make sure that ctrl with current focus gets updated
  GotoDlgCtrl(GetDlgItem(IDOK));

  m_fhData.valueType     = FlowHistBase::ValueType(m_valueType);
  m_fhData.fixedValue    = m_fixedValue;
  m_fhData.wellboreStorage = FlowHistBase::WellboreStorage(m_wellboreStorage);
  m_fhData.SetActive(true);

  CSequenceTimeStepDlg::OnOK();
}
