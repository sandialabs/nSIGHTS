///////////////////////////////////////////////////////////////////////////////////
//
// SequenceTestZoneCurveDlg.cpp
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
//      nPre dialog.
//
///////////////////////////////////////////////////////////////////////////////////

// ParameterFPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "SequenceTestZoneCurveDlg.h"

#include <genClass/U_Str.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSequenceTestZoneCurveDlg dialog


CSequenceTestZoneCurveDlg::CSequenceTestZoneCurveDlg(TestZoneCurve& testZoneCurve, CWnd* pParent /*=NULL*/)
    : CDialog(CSequenceTestZoneCurveDlg::IDD, pParent),
    m_testZoneCurve(testZoneCurve)
{
    //{{AFX_DATA_INIT(CSequenceTestZoneCurveDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CSequenceTestZoneCurveDlg::OnInitDialog()
{
    m_timeIsSequenceBased  = m_testZoneCurve.timeIsSequenceBased;
    m_curveYUnits          = m_testZoneCurve.curveYUnits;
    m_curveYisLog10        = m_testZoneCurve.curveYisLog10;
    m_curveTimeUnits       = m_testZoneCurve.curveTimeUnits;
    m_curveTimeisLog10     = m_testZoneCurve.curveTimeisLog10;

    m_utimeIsSequenceBased.Set(this, IDC_COMBO1, m_timeIsSequenceBased, "Test", "Sequence");
    m_utimeIsSequenceBased.AddCallback(this);


    m_ucurveYUnits.Set(this, IDC_COMBO2, &m_curveYUnits);
    m_ucurveYisLog10.Set(this, IDC_COMBO3, m_curveYisLog10);

    m_ucurveTimeUnits.Set(this, IDC_COMBO4, &m_curveTimeUnits);
    m_ucurveTimeisLog10.Set(this, IDC_COMBO5, m_curveTimeisLog10);

    DoCtrlUpdate();

    CDialog::OnInitDialog();

    return TRUE;
}

void CSequenceTestZoneCurveDlg::DoCtrlUpdate()
{
    m_ucurveTimeUnits.EnableWindow(m_timeIsSequenceBased);
    m_ucurveTimeisLog10.EnableWindow(m_timeIsSequenceBased);
}

void CSequenceTestZoneCurveDlg::OnOK()
{
    m_testZoneCurve.timeIsSequenceBased  = m_timeIsSequenceBased;
    m_testZoneCurve.curveYUnits          = m_curveYUnits;
    m_testZoneCurve.curveYisLog10        = m_curveYisLog10;
    m_testZoneCurve.curveTimeUnits       = m_curveTimeUnits;
    m_testZoneCurve.curveTimeisLog10     = m_curveTimeisLog10;

    CDialog::OnOK();
}


void CSequenceTestZoneCurveDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSequenceTestZoneCurveDlg)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSequenceTestZoneCurveDlg, CDialog)
    //{{AFX_MSG_MAP(CSequenceTestZoneCurveDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSequenceTestZoneCurveDlg message handlers

