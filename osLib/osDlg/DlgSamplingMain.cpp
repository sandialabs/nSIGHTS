///////////////////////////////////////////////////////////////////////////////////
//
// DlgSamplingMain.cpp
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
#include <osMain/G_Sample.h>
#include <osMain/G_Optimize.h>

#include "DlgSamplingMain.h"

#include <genInterface/InterfaceCtrls.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSamplingMain dialog

IMPLEMENT_DYNCREATE(DlgSamplingMain, CGenericDialog)

DlgSamplingMain::DlgSamplingMain(CWnd* pParent /*=NULL*/)
    : CGenericDialog(DlgSamplingMain::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgSamplingMain)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void DlgSamplingMain::DoDataExchange(CDataExchange* pDX)
{
    CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgSamplingMain)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSamplingMain, CGenericDialog)
    //{{AFX_MSG_MAP(DlgSamplingMain)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgSamplingMain message handlers

BOOL DlgSamplingMain::OnInitDialog()
{
    m_uncertProp.AddMappedEntry("Monte Carlo", SampleControlStatics::utMonteCarlo);
    m_uncertProp.AddMappedEntry("FOSM", SampleControlStatics::utFOSM);
    m_uncertProp.AddMappedEntry("PEM", SampleControlStatics::utPEM);
    m_uncertProp.Set(this, IDC_COMBO1, (int*) (&SampleControlStatics::uncertType));
    m_uncertProp.AddCallback(this);

    CGenericDialog::OnInitDialog();

    InitData(true);

    return TRUE;
}

void DlgSamplingMain::ResetAllControls()
{
    m_uncertProp.DoCtrlReset();
}

void DlgSamplingMain::DoCtrlUpdate()
{
    CheckRestrictions();
}


void DlgSamplingMain::CheckRestrictions()
{
    //m_uncertProp.EnableWindow(??);
    static_cast<DialogNode*>(GetParent())->CheckRestrictions();
}

