///////////////////////////////////////////////////////////////////////////////////
//
// VaryVarUnitsRangeDlg.cpp
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
#include "VaryVarUnitsRangeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVaryVarUnitsRangeDlg dialog

CVaryVarUnitsRangeDlg::CVaryVarUnitsRangeDlg(VaryVarUnits& varyVarUnits, const SC_RealFormat* overrideFormat,
    const double& minMetricVal, const double& maxMetricVal,
    CWnd* pParent /*=NULL*/)
  : CDialog(CVaryVarUnitsRangeDlg::IDD, pParent),
    m_varyVarUnits(varyVarUnits),
    m_poverrideFormat(overrideFormat),
    m_minMetricVal(minMetricVal),
    m_maxMetricVal(maxMetricVal)
{
  //{{AFX_DATA_INIT(CVaryVarUnitsRangeDlg)
  //}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CVaryVarUnitsRangeDlg, CDialog)
  //{{AFX_MSG_MAP(CVaryVarUnitsRangeDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVaryVarUnitsRangeDlg message handlers

BOOL CVaryVarUnitsRangeDlg::OnInitDialog()
{
  // set values
  m_rangeMinLim = m_varyVarUnits.unitIndex;
  m_rangeMaxLim = m_varyVarUnits.unitIndex;

  m_rangeMinLim.SetMetricVal(m_varyVarUnits.rangeMinLim);
  m_rangeMaxLim.SetMetricVal(m_varyVarUnits.rangeMaxLim);
  m_rangeIsLinear = m_varyVarUnits.rangeIsLinear;
  m_rangeNStep    = m_varyVarUnits.rangeNStep;

  // set controls
  m_urangeMinLim.Set(this, IDC_COMBO1, IDC_EDIT1, &m_rangeMinLim,
                     m_poverrideFormat, m_minMetricVal, m_maxMetricVal);
  m_urangeMinLim.m_uIndexCtrl.AddCallback(this);

  m_urangeMaxLim.Set(this, IDC_COMBO2, IDC_EDIT2, &m_rangeMaxLim,
                     m_poverrideFormat, m_minMetricVal, m_maxMetricVal);

  m_urangeIsLinear.Set(this, IDC_COMBO3,  m_rangeIsLinear, "Logarithmic", "Linear");

  m_urangeNStep.Set(this, IDC_EDIT3, m_rangeNStep, 2, 1000);

  CDialog::OnInitDialog();

  return TRUE;
}

void CVaryVarUnitsRangeDlg::DoCtrlUpdate()
{
  m_urangeMaxLim.DoUnitConvChange(m_rangeMinLim.GetConvIndex());
}

void CVaryVarUnitsRangeDlg::OnOK()
{
  // kluge to make sure that ctrl with current focus gets updated
  GotoDlgCtrl(GetDlgItem(IDOK));

  m_varyVarUnits.unitIndex.SetConvIndex(m_rangeMinLim.GetActualConvIndex());

  m_varyVarUnits.rangeMinLim = m_rangeMinLim.GetMetricVal();
  m_varyVarUnits.rangeMaxLim = m_rangeMaxLim.GetMetricVal();
  m_varyVarUnits.rangeIsLinear = m_rangeIsLinear;
  m_varyVarUnits.rangeNStep = m_rangeNStep;

  CDialog::OnOK();
}
