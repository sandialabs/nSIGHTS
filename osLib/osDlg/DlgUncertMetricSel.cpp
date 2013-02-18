///////////////////////////////////////////////////////////////////////////////////
//
// DlgUncertMetricSel.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include "DlgUncertMetricSel.h"

#include <genClass/U_Msg.h>
#include <genClass/DO_Real.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgUncertMetricSel dialog

IMPLEMENT_DYNCREATE(DlgUncertMetricSel,CGenericDialog)

DlgUncertMetricSel::DlgUncertMetricSel(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgUncertMetricSel::IDD, pParent)
{
    for (int i = 0; i < UncertInput::maxMetricSel; i++)
        m_metricObject[i] = new CComboFOCtrl(typeid(DO_Real));

    //{{AFX_DATA_INIT(DlgUncertMetricSel)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

DlgUncertMetricSel::~DlgUncertMetricSel()
{
    for (int i = 0; i < UncertInput::maxMetricSel; i++)
        delete m_metricObject[i];
}

BEGIN_MESSAGE_MAP(DlgUncertMetricSel, CGenericDialog)
    //{{AFX_MSG_MAP(DlgUncertMetricSel)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgUncertMetricSel message handlers

BOOL DlgUncertMetricSel::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    for (int i = 0; i < UncertInput::maxMetricSel; i++)
    {
        m_metricSelected[i].Set(this, IDC_CHECK1+ i, uncertInput.inputMetrics[i].uncertOnMetric);
        m_metricSelected[i].AddCallback(this);
        m_metricObject[i]->Set(this, IDC_COMBO1 + i, uncertInput.inputMetrics[i].metricObject);
    }

    ResetAllControls();

    return TRUE;
}

void DlgUncertMetricSel::DoCtrlUpdate()
{
    CheckRestrictions();
}

void DlgUncertMetricSel::CheckRestrictions()
{
    bool foOK = m_metricObject[0]->FOAvail();
    for (int i = 0; i < UncertInput::maxMetricSel; i++)
    {
        m_metricSelected[i].EnableWindow(foOK);
        m_metricObject[i]->EnableWindow(foOK && uncertInput.inputMetrics[i].uncertOnMetric);
    }
}

void DlgUncertMetricSel::ResetAllControls()
{
    for (int i = 0; i < UncertInput::maxMetricSel; i++)
    {
        m_metricObject[i]->ResetSelection();
        m_metricSelected[i].ResetSelection();
    }
    CheckRestrictions();
}


