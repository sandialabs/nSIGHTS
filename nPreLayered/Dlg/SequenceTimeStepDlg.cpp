///////////////////////////////////////////////////////////////////////////////////
//
// SequenceTimeStepDlg.cpp
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


// SequenceTimeStepDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <genClass/U_Real.h>

#include <Var/G_Control.h>

#include "SampVarUnitsDlg.h"
#include "SequenceTimeStepDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSequenceTimeStepDlg dialog


CSequenceTimeStepDlg::CSequenceTimeStepDlg(Sequence& sequence, bool isTSZone, UINT nIDTemplate,  CWnd* pParent /*=NULL*/)
  : CDialog(nIDTemplate, pParent),
    m_sequence(sequence), isTS(isTSZone)
{
  //{{AFX_DATA_INIT(CSequenceTimeStepDlg)
  //}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CSequenceTimeStepDlg, CDialog)
  //{{AFX_MSG_MAP(CSequenceTimeStepDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSequenceTimeStepDlg message handlers

BOOL CSequenceTimeStepDlg::OnInitDialog()
{
  m_timeStepType  = m_sequence.timeStep.timeStepType;
  m_staticSize  = m_sequence.timeStep.staticSize;
  m_firstLogSize  = m_sequence.timeStep.firstLogSize;
  m_nlogSteps   = m_sequence.timeStep.nlogSteps;
  m_dynamicMax  = m_sequence.timeStep.dynamicMax;
  m_dynamicMin  = m_sequence.timeStep.dynamicMin;

  // set controls
  m_utimeStepType.AddMappedEntry("Static", TimeStep::tstStatic);
  m_utimeStepType.AddMappedEntry("Log", TimeStep::tstLog);
  if (control.IsNotLayered())
    m_utimeStepType.AddMappedEntry("Dynamic P", TimeStep::tstDynamicP);
  if (control.IsNotLayered())
    m_utimeStepType.AddMappedEntry("Dynamic Q", TimeStep::tstDynamicQ);
  m_utimeStepType.Set(this, IDC_COMBO1,  &m_timeStepType);
  m_utimeStepType.AddCallback(this);

  SC_SciFormat tFormat(6);
  double minTS = Pround(m_sequence.seqLength.GetMetricVal() / double(TimeStep::maxTimeSteps), 5);
  double maxTS = Pround(m_sequence.seqLength.GetMetricVal() / 10.0, 2);
  m_ustaticSize.Set(this, IDC_COMBO2, IDC_EDIT1, &m_staticSize, &tFormat, minTS, maxTS);
  m_unlogSteps.Set(this, IDC_EDIT2, m_nlogSteps, 10, TimeStep::maxTimeSteps);
  m_ufirstLogSize.Set(this, IDC_COMBO3, IDC_EDIT3, &m_firstLogSize, &tFormat, 1.0E-08, maxTS);
  m_udynamicMin.Set(this, IDC_COMBO4, IDC_EDIT4, &m_dynamicMin, &tFormat, 0.0, maxTS);
  m_udynamicMax.Set(this, IDC_COMBO5, IDC_EDIT5, &m_dynamicMax, &tFormat, minTS, maxTS);

  return TRUE;
}

void CSequenceTimeStepDlg::DoCtrlUpdate()
{
  if (isTS)
  {
    m_ustaticSize.EnableWindowEx(m_timeStepType == TimeStep::tstStatic);
    m_unlogSteps.EnableWindow(m_timeStepType == TimeStep::tstLog);
    m_ufirstLogSize.EnableWindowEx(m_timeStepType == TimeStep::tstLog);
    bool isDyn = (m_timeStepType == TimeStep::tstDynamicP) || (m_timeStepType == TimeStep::tstDynamicQ);
    m_udynamicMin.EnableWindowEx(isDyn);
    m_udynamicMax.EnableWindowEx(isDyn);
  }
  else
  {
    m_utimeStepType.EnableWindow(false);
    m_ustaticSize.EnableWindowEx(false);
    m_ufirstLogSize.EnableWindowEx(false);
    m_unlogSteps.EnableWindow(false);
    m_udynamicMax.EnableWindowEx(false);
    m_udynamicMin.EnableWindowEx(false);
  }

}


void CSequenceTimeStepDlg::OnOK()
{
  if (isTS)
  {
    m_sequence.timeStep.timeStepType = TimeStep::TimeStepType(m_timeStepType);
    m_sequence.timeStep.staticSize   = m_staticSize;
    m_sequence.timeStep.firstLogSize = m_firstLogSize;
    m_sequence.timeStep.nlogSteps  = m_nlogSteps;
    m_sequence.timeStep.dynamicMax = m_dynamicMax;
    m_sequence.timeStep.dynamicMin = m_dynamicMin;
  }
  CDialog::OnOK();
}
