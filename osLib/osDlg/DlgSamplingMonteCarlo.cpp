///////////////////////////////////////////////////////////////////////////////////
//
// DlgSamplingMonteCarlo.cpp
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

#include "DlgSamplingMonteCarlo.h"

#include <genInterface/InterfaceCtrls.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSamplingMonteCarlo dialog

IMPLEMENT_DYNCREATE(DlgSamplingMonteCarlo, CGenericDialog)

DlgSamplingMonteCarlo::DlgSamplingMonteCarlo(CWnd* pParent /*=NULL*/)
    : CGenericDialog(DlgSamplingMonteCarlo::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgSamplingMonteCarlo)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void DlgSamplingMonteCarlo::DoDataExchange(CDataExchange* pDX)
{
    CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgSamplingMonteCarlo)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSamplingMonteCarlo, CGenericDialog)
    //{{AFX_MSG_MAP(DlgSamplingMonteCarlo)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgSamplingMonteCarlo message handlers

BOOL DlgSamplingMonteCarlo::OnInitDialog()
{
    m_usamplingIsLHS.Set(this, IDC_COMBO1, SampleControlStatics::samplingIsLHS, "Random", "Latin Hyper-Cube");
    m_unumberOfTrials.Set(this, IDC_EDIT1, SampleControlStatics::numberOfTrials, 3, 10000);
    m_urandSeed.Set(this, IDC_EDIT2, SampleControlStatics::randSeed, 1, INT_MAX-1);
    m_usingleRunRealization.Set(this, IDC_EDIT3, SampleControlStatics::singleRunRealization, 0, 10000);
    m_uspecifyCorrelations.Set(this, IDC_COMBO2, SampleControlStatics::specifyCorrelations);
    m_uspecifyCorrelations.AddCallback(this);
    m_uforceCorrelationsToZero.Set(this, IDC_COMBO3, SampleControlStatics::forceCorrelationsToZero);

    m_doSample.Set(this, IDC_BUTTON1);
    m_doSample.AddCallback(this, 1);

    CGenericDialog::OnInitDialog();

    InitData(true);

    return TRUE;
}

void DlgSamplingMonteCarlo::ResetAllControls()
{
    m_usamplingIsLHS.DoCtrlReset();
    m_unumberOfTrials.DoCtrlReset();
    m_urandSeed.DoCtrlReset();
    m_usingleRunRealization.DoCtrlReset();
    m_uspecifyCorrelations.DoCtrlReset();
    m_uforceCorrelationsToZero.DoCtrlReset();
}

void DlgSamplingMonteCarlo::DoSample()
{
    CWaitCursor w;
    sample.Sample();
}

void DlgSamplingMonteCarlo::DoCtrlUpdate(int reason)
{
    if (reason == 0)
    {
        CheckRestrictions();
    }
    else
    {
        DoSample();
    }
}


void DlgSamplingMonteCarlo::CheckRestrictions()
{
    static_cast<DialogNode*>(GetParent())->CheckRestrictions();
}

