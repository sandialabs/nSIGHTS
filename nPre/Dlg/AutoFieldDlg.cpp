///////////////////////////////////////////////////////////////////////////////////
//
// AutoFieldDlg.cpp
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
#include "AutoFieldDlg.h"

#include <Var/G_DataCapture.h>

#include <Auto/A_Main.h>
#include <Auto/A_Field.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoFieldDlg dialog

CAutoFieldDlg::CAutoFieldDlg(CWnd* pParent /*=NULL*/) : CDialog(CAutoFieldDlg::IDD, pParent)
{

    //{{AFX_DATA_INIT(CAutoFieldDlg)
    //}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CAutoFieldDlg, CDialog)
    //{{AFX_MSG_MAP(CAutoFieldDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoFieldDlg message handlers

static bool obsOK = false;

BOOL CAutoFieldDlg::OnInitDialog()
{
    m_uinputDataFormat.AddMappedEntry("XY Files", idfXY);
    m_uinputDataFormat.AddMappedEntry("Table", idfTable);
    m_uinputDataFormat.AddMappedEntry("Mini-Troll", idfMiniTroll);
    m_uinputDataFormat.Set(this, IDC_COMBO1,  (int*) &inputDataFormat);
    m_uinputDataFormat.AddCallback(this);

    m_uprocessTestZonePressureData.Set(this, IDC_CHECK1, processTestZonePressureData);
    m_uprocessTestZonePressureData.AddCallback(this);

    m_uprocessTestZoneFlowData.Set(this, IDC_CHECK2, processTestZoneFlowData);
    m_uprocessTestZoneFlowData.AddCallback(this);

    m_uprocessObsWellPressureData.Set(this, IDC_CHECK3, processObsWellPressureData);
    m_uprocessObsWellPressureData.AddCallback(this);

    m_ubothInSameTable.Set(this, IDC_CHECK4, bothInSameFile);
    m_ucreatePressurePlot.Set(this, IDC_CHECK5, createPressurePlot);
    m_ucreateFlowPlot.Set(this, IDC_CHECK6, createFlowPlot);

    m_uprocessDups.Set(this, IDC_CHECK7, processDups);
    m_uprocessReduce.Set(this, IDC_CHECK8, processReduce);
    m_uprocessST.Set(this, IDC_CHECK9, processST);

    SC_StringArray dummy;
    obsOK = DataCaptureStaticSupport::GetWellIDs(dummy) && (dummy.Size() > 1);

    CDialog::OnInitDialog();

    CheckRestrictions();

    return TRUE;
}


void CAutoFieldDlg::DoCtrlUpdate()
{
    CheckRestrictions();
}

void CAutoFieldDlg::CheckRestrictions()
{

    GetDlgItem(IDOK)->EnableWindow(processTestZonePressureData || processTestZoneFlowData);

    m_uinputDataFormat.EnableWindow(processTestZonePressureData || processTestZoneFlowData);

    m_uprocessObsWellPressureData.EnableWindow(processTestZonePressureData && obsOK);

    m_ubothInSameTable.EnableWindow(inputDataFormat != idfXY && processTestZonePressureData && processTestZoneFlowData);
    m_ucreatePressurePlot.EnableWindow(processTestZonePressureData);
    m_ucreateFlowPlot.EnableWindow(processTestZoneFlowData);
}

void CAutoFieldDlg::OnOK()
{
    int nDel = AutoFieldWillDeletePages();
    if (nDel > 0)
    {
        CString msgString;
        msgString.Format("%d%s", nDel, " existing plots/data pages will be deleted");
        if (AfxMessageBox(msgString, MB_OKCANCEL) == IDCANCEL)
            return;
    }

    AutoFieldSetup();
    CDialog::OnOK();
}

