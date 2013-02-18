///////////////////////////////////////////////////////////////////////////////////
//
// AutoSequenceDlg.cpp
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

#include "stdafx.h"
#include "Resource.h"
#include "AutoSequenceDlg.h"

#include <Var/G_DataCapture.h>

#include <Auto/A_Main.h>
#include <Auto/A_Sequence.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoSequenceDlg dialog

void AutoSequenceUI::Set(CWnd* pParent, int& idStart)
{
    categoryOK = true;

    m_udoExtract.Set(pParent, idStart++, aSpec.doExtract);
    m_udoExtract.AddCallback(this);

    for (int i = 0; i < SequenceAutoSpec::maxSpec; i++)
    {
        m_udoSpec[i].Set(pParent, idStart++, aSpec.doSpec[i]);
        m_udoSpec[i].SetWindowText(aSpec.specID[i]);
        m_udoSpec[i].AddCallback(this);
    }

    CheckRestrictions();
}

void AutoSequenceUI::DoCtrlUpdate()
{
    CheckRestrictions();
}

void AutoSequenceUI::CheckRestrictions()
{
    m_udoExtract.EnableWindow(aSpec.opOK && categoryOK);
    bool isOK = IsOK();
    for (int i = 0; i < SequenceAutoSpec::maxSpec; i++)
        m_udoSpec[i].EnableWindow(isOK && aSpec.IsEnabled(i));
}


CAutoSequenceDlg::CAutoSequenceDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CAutoSequenceDlg::IDD, pParent),
    m_uflowSpec(flowSpec),
    m_uhistSpec(histSpec),
    m_upulseSpec(pulseSpec),
    m_uslugSpec(slugSpec)
{
    obsOK = false;

    //{{AFX_DATA_INIT(CAutoSequenceDlg)
    //}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CAutoSequenceDlg, CDialog)
    //{{AFX_MSG_MAP(CAutoSequenceDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoSequenceDlg message handlers


BOOL CAutoSequenceDlg::OnInitDialog()
{
    AutoSequenceCheckOK();

    m_uprocessTestZonePressureData.Set(this, IDC_AUTO_MAINPRESSURE, processTestZonePressureData);
    m_uprocessTestZonePressureData.AddCallback(this);

    m_uprocessTestZoneFlowData.Set(this, IDC_AUTO_MAINFLOW, processTestZoneFlowData);
    m_uprocessTestZoneFlowData.AddCallback(this);

    m_uprocessObsWellPressureData.Set(this, IDC_AUTO_OBSPRESSURE, processObsWellPressureData);
    m_uprocessObsWellPressureData.AddCallback(this);

    m_utestDesignMode.Set(this, IDC_AUTO_TESTDESIGNMODE, testDesignMode);

    SC_StringArray dummy;
    obsOK = DataCaptureStaticSupport::GetWellIDs(dummy) && (dummy.Size() > 1);

    int idStart = IDC_CHECK1;
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


void CAutoSequenceDlg::DoCtrlUpdate()
{
    CheckRestrictions();
}

void CAutoSequenceDlg::CheckRestrictions()
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

    GetDlgItem(IDOK)->EnableWindow(m_uflowSpec.IsOK() || m_uhistSpec.IsOK() || m_upulseSpec.IsOK() || m_uslugSpec.IsOK());
}

void CAutoSequenceDlg::OnOK()
{
    int nDel = AutoSequenceWillDeletePages();

    if (nDel > 0)
    {
        CString msgString;
        msgString.Format("%d%s", nDel, " existing plots/data pages will be deleted");
        if (AfxMessageBox(msgString, MB_OKCANCEL) == IDCANCEL)
            return;
    }

    AutoSequenceSetup();
    CDialog::OnOK();
}

