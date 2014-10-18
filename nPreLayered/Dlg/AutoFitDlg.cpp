///////////////////////////////////////////////////////////////////////////////////
//
// AutoFitDlg.cpp
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
#include "AutoFitDlg.h"

#include <Var/G_DataCapture.h>
#include <Auto/A_Main.h>

#include <Auto/A_Fit.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoFitDlg dialog

void AutoFitUI::Set(CWnd* pParent, int& idStart)
{
  m_udoExtract.Set(pParent, idStart++, aSpec.doExtract);
  m_udoExtract.AddCallback(this);

  for (int i = 0; i < FitAutoSpec::maxSpec; i++)
  {
    m_udoSpec[i].Set(pParent, idStart++, aSpec.doSpec[i]);
    m_udoSpec[i].SetWindowText(aSpec.specID[i]);
    m_udoSpec[i].AddCallback(this);
  }

  categoryOK = true;

  CheckRestrictions();
}

void AutoFitUI::DoCtrlUpdate()
{
  CheckRestrictions();
}

void AutoFitUI::CheckRestrictions()
{
  m_udoExtract.EnableWindow(aSpec.opOK && categoryOK);
  bool isOK = IsOK();
  for (int i = 0; i < FitAutoSpec::maxSpec; i++)
    m_udoSpec[i].EnableWindow(isOK && aSpec.IsEnabled(i));
}


CAutoFitDlg::CAutoFitDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CAutoFitDlg::IDD, pParent),
    m_ucartesianSpec(cartesianFitSpec),
    m_uflowSpec(flowFitSpec),
    m_uhistSpec(histFitSpec),
    m_upulseSpec(pulseFitSpec),
    m_uslugSpec(slugFitSpec)
{

  //{{AFX_DATA_INIT(CAutoFitDlg)
  //}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CAutoFitDlg, CDialog)
  //{{AFX_MSG_MAP(CAutoFitDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoFitDlg message handlers

BOOL CAutoFitDlg::OnInitDialog()
{
  AutoFitCheckOK();

  m_uprocessTestZonePressureData.Set(this, IDC_AUTO_MAINPRESSURE, processTestZonePressureData);
  m_uprocessTestZonePressureData.AddCallback(this);

  m_uprocessTestZoneFlowData.Set(this, IDC_AUTO_MAINFLOW, processTestZoneFlowData);
  m_uprocessTestZoneFlowData.AddCallback(this);

  m_uprocessObsWellPressureData.Set(this, IDC_AUTO_OBSPRESSURE, processObsWellPressureData);
  m_uprocessObsWellPressureData.AddCallback(this);

  SC_StringArray dummy;
  obsOK = DataCaptureStaticSupport::GetWellIDs(dummy) && (dummy.Size() > 1);

  int idStart = IDC_CHECK1;

  m_ucartesianSpec.Set(this, idStart);
  m_ucartesianSpec.m_udoExtract.AddCallback(this);

  m_uflowSpec.Set(this, idStart);
  m_uflowSpec.m_udoExtract.AddCallback(this);

  m_uhistSpec.Set(this, idStart);
  m_uhistSpec.m_udoExtract.AddCallback(this);

  m_upulseSpec.Set(this, idStart);
  m_upulseSpec.m_udoExtract.AddCallback(this);

  m_uslugSpec.Set(this, idStart);
  m_uslugSpec.m_udoExtract.AddCallback(this);

  CDialog::OnInitDialog();

  CheckRestrictions();

  return TRUE;
}


void CAutoFitDlg::DoCtrlUpdate()
{
  CheckRestrictions();
}

void CAutoFitDlg::CheckRestrictions()
{
  m_uprocessObsWellPressureData.EnableWindow(obsOK);

  m_uflowSpec.categoryOK = processTestZonePressureData ||
                           (obsOK && processObsWellPressureData);
  m_uhistSpec.categoryOK = processTestZoneFlowData ||
                           (obsOK && processObsWellPressureData);

  m_upulseSpec.categoryOK = processTestZonePressureData;
  m_uslugSpec.categoryOK = processTestZonePressureData;

  m_uflowSpec.CheckRestrictions();
  m_uhistSpec.CheckRestrictions();
  m_upulseSpec.CheckRestrictions();
  m_uslugSpec.CheckRestrictions();

  GetDlgItem(IDOK)->EnableWindow(m_ucartesianSpec.IsOK() || m_uflowSpec.IsOK() || m_uhistSpec.IsOK() ||
                                 m_upulseSpec.IsOK() || m_uslugSpec.IsOK());
}

void CAutoFitDlg::OnOK()
{
  int nDel = AutoFitWillDeletePages();

  if (nDel > 0)
  {
    CString msgString;
    msgString.Format("%d%s", nDel, " existing data pages will be deleted");
    if (AfxMessageBox(msgString, MB_OKCANCEL) == IDCANCEL)
      return;
  }

  AutoFitSetup();
  CDialog::OnOK();
}
