///////////////////////////////////////////////////////////////////////////////////
//
// OptVarUnitsDlg.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains certain rights in this software.
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
//
// DESCRIPTION:
//
//      nPre dialog
//
///////////////////////////////////////////////////////////////////////////////////

// ParameterOptimizationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "OptVarUnitsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptVarUnitsDlg dialog

COptVarUnitsDlg::COptVarUnitsDlg(OptVarUnits& optVarUnits, const SC_RealFormat* overrideFormat, 
                                 const double& minMetricVal, const double& maxMetricVal,
                                 CWnd* pParent /*=NULL*/)
    : CDialog(COptVarUnitsDlg::IDD, pParent),
    m_optVarUnits(optVarUnits),
    m_poverrideFormat(overrideFormat),
    m_minMetricVal(minMetricVal),
    m_maxMetricVal(maxMetricVal)
{
    //{{AFX_DATA_INIT(COptVarUnitsDlg)
    //}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(COptVarUnitsDlg, CDialog)
    //{{AFX_MSG_MAP(COptVarUnitsDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptVarUnitsDlg message handlers

BOOL COptVarUnitsDlg::OnInitDialog() 
{
    // set values
    m_optMinLim.InitVal(m_optVarUnits.unitIndex, m_optVarUnits.optMinLim);
    m_optMaxLim.InitVal(m_optVarUnits.unitIndex, m_optVarUnits.optMaxLim);
    m_optParEstimate.InitVal(m_optVarUnits.unitIndex, m_optVarUnits.optParEstimate);

    m_optParSigma = m_optVarUnits.optParSigma;
    m_optIsLinear = m_optVarUnits.optIsLinear;

    // set controls
    m_uoptMinLim.Set(this, IDC_COMBO1, IDC_EDIT1, &m_optMinLim, 
        m_poverrideFormat, m_minMetricVal, m_maxMetricVal); 
    m_uoptMinLim.m_uIndexCtrl.AddCallback(this);

    m_uoptMaxLim.Set(this, IDC_COMBO2, IDC_EDIT2, &m_optMaxLim, 
        m_poverrideFormat, m_minMetricVal, m_maxMetricVal);

    m_uoptParEstimate.Set(this, IDC_COMBO3, IDC_EDIT3, &m_optParEstimate, 
        m_poverrideFormat, m_minMetricVal, m_maxMetricVal);

    m_uoptIsLinear.Set(this, IDC_COMBO4,  m_optIsLinear, "Logarithmic", "Linear");

    m_uoptParSigma .Set(this, IDC_EDIT4, m_optParSigma , SC_SciFormat(5), 1.0E-10, 1.0);

    CDialog::OnInitDialog();

    return TRUE;
}

void COptVarUnitsDlg::DoCtrlUpdate()
{
    m_uoptMaxLim.DoUnitConvChange(m_optMinLim.GetConvIndex());
    m_uoptParEstimate.DoUnitConvChange(m_optMinLim.GetConvIndex());
}

void COptVarUnitsDlg::OnOK() 
{
    // kluge to make sure that ctrl with current focus gets updated
    GotoDlgCtrl(GetDlgItem(IDOK));

    m_optVarUnits.unitIndex.SetConvIndex(m_optMinLim.GetActualConvIndex());

    m_optVarUnits.optMinLim = m_optMinLim.GetMetricVal();
    m_optVarUnits.optMaxLim = m_optMaxLim.GetMetricVal();
    m_optVarUnits.optParEstimate = m_optParEstimate.GetMetricVal();
    m_optVarUnits.optParSigma = m_optParSigma;
    m_optVarUnits.optIsLinear = m_optIsLinear;

    CDialog::OnOK();
}

