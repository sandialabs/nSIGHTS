///////////////////////////////////////////////////////////////////////////////////
//
// SequencePulseSlugDlg.cpp
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

// SequenceSlugPulseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <Var/G_Control.h>

#include "SequencePulseSlugDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSequencePulseSlugDlg dialog


CSequencePulseSlugDlg::CSequencePulseSlugDlg(Sequence& sequence, PulseSlugBase& psData, UINT nIDTemplate, CWnd* pParent)
    : CSequenceTimeStepDlg(sequence, nIDTemplate, pParent),
    m_psData(psData)
{
    //{{AFX_DATA_INIT(CSequencePulseSlugDlg)
    //}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CSequencePulseSlugDlg, CDialog)
    //{{AFX_MSG_MAP(CSequencePulseSlugDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSequencePulseSlugDlg message handlers

BOOL CSequencePulseSlugDlg::OnInitDialog()
{
    CSequenceTimeStepDlg::OnInitDialog();

    m_initialPressureType = m_psData.initialPressureType;
    m_initialPressure     = m_psData.initialPressure;

    if (!NSUnitsBase::headFlag)
    {
        GetDlgItem(IDC_PULSESLUGSEQ_PRESSURE)->SetWindowText("Initial pressure");
    }
    else
    {
        GetDlgItem(IDC_PULSESLUGSEQ_PRESSURE)->SetWindowText("Initial head");
    }


    m_uinitialPressureType.AddMappedEntry("Absolute", PulseSlugBase::ipAbsolute);
    if (control.IsLiquid())
        m_uinitialPressureType.AddMappedEntry("Tubing String Rel.", PulseSlugBase::ipTSRelative);
    m_uinitialPressureType.AddMappedEntry("Sequence Relative", PulseSlugBase::ipSeqRelative);
    m_uinitialPressureType.Set(this, IDC_COMBO6,  &m_initialPressureType);

    m_uinitialPressure.Set(this, IDC_COMBO7, IDC_EDIT6, &m_initialPressure, NULL, -1.0E+06, 1.0E+06);

    DoCtrlUpdate();

    CDialog::OnInitDialog();

    return TRUE;
}


void CSequencePulseSlugDlg::OnOK()
{
    // kluge to make sure that ctrl with current focus gets updated
    GotoDlgCtrl(GetDlgItem(IDOK));

    m_psData.initialPressureType = PulseSlugBase::InitialPressureType(m_initialPressureType);
    m_psData.initialPressure = m_initialPressure;

    CSequenceTimeStepDlg::OnOK();
}

