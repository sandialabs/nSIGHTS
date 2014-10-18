///////////////////////////////////////////////////////////////////////////////////
//
// DlgConfigGas.cpp
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


// DlgConfigGas.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgConfigGas.h"

#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>
#include <Var/G_Control.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgConfigGas dialog

IMPLEMENT_DYNCREATE(DlgConfigGas,CGenericDialog)

DlgConfigGas::DlgConfigGas(CWnd* pParent /*=NULL*/)
  : CGenericDialog(DlgConfigGas::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgConfigGas)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}



BEGIN_MESSAGE_MAP(DlgConfigGas, CGenericDialog)
  //{{AFX_MSG_MAP(DlgConfigGas)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgConfigGas message handlers

BOOL DlgConfigGas::OnInitDialog()
{
  m_ugasUseKlinkenburg.Set(this, IDC_COMBO1, gasUseKlinkenburg);
  m_ugasUseKlinkenburg.AddCallback(this);

  m_ugasUseSimpleViscositySlope.Set(this, IDC_COMBO2, gasUseSimpleViscositySlope);
  m_ugasUseSimpleViscositySlope.AddCallback(this);

  m_ugasFlowIsMass.Set(this, IDC_COMBO3, gasFlowIsMass, "Volume @STP", "Mass Flow");
  m_ugasFlowIsMass.AddCallback(this);


  m_urTemperature.Set(this, IDC_COMBO4, IDC_EDIT_TEMPERATURE,
                      &gasStdTemperature, &SC_DecFormat(2), 0.0, 100.0);
  m_urPressure.Set(this, IDC_COMBO6, IDC_EDIT_PRESSURE,
                   &gasStdPressure, NULL, 1.0, 1000.0);

  CGenericDialog::OnInitDialog();

  InitData(true);

  return TRUE;
}


void DlgConfigGas::CheckRestrictions()
{
  m_urTemperature.EnableWindowEx(!gasFlowIsMass);
  m_urPressure.EnableWindowEx(!gasFlowIsMass);
}

void DlgConfigGas::DoCtrlUpdate()
{
  parameters.SetForFlags();
  sequence.SetForFlags();

  CheckRestrictions();
}

void DlgConfigGas::ResetAllControls()
{
  m_ugasUseKlinkenburg.DoCtrlReset();
  m_ugasUseSimpleViscositySlope.DoCtrlReset();
  m_ugasFlowIsMass.DoCtrlReset();

  m_urTemperature.DoCtrlReset();
  m_urPressure.DoCtrlReset();
}
