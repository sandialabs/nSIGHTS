///////////////////////////////////////////////////////////////////////////////////
//
// DlgOptimizationSCEMAlg.cpp
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

#include "DlgOptimizationSCEMAlg.h"

#include <osMain/G_Optimize.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationSCEMAlg dialog

IMPLEMENT_DYNCREATE(DlgOptimizationSCEMAlg,CGenericDialog)

DlgOptimizationSCEMAlg::DlgOptimizationSCEMAlg(CWnd* pParent /*=NULL*/)
    : CGenericDialog(DlgOptimizationSCEMAlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgOptimizationSCEMAlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(DlgOptimizationSCEMAlg, CGenericDialog)
    //{{AFX_MSG_MAP(DlgOptimizationSCEMAlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationSCEMAlg message handlers

BOOL DlgOptimizationSCEMAlg::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_uscem_p.Set(this, IDC_EDIT1, scem_p, 1, 100);
    m_uscem_m.Set(this, IDC_EDIT2, scem_m, 3, 100);
    m_uscem_q.Set(this, IDC_EDIT3, scem_q, 2, 100);
    m_uscem_alpha.Set(this, IDC_EDIT4, scem_alpha, 1, 100);
    m_uscem_beta.Set(this, IDC_EDIT5, scem_beta, 1, 100);
    m_uscem_seed.Set(this, IDC_EDIT6, scem_seed, 1001, 10000000);

    CheckRestrictions();

    return TRUE;
}

void DlgOptimizationSCEMAlg::ResetAllControls()
{
    m_uscem_p.DoCtrlReset();
    m_uscem_m.DoCtrlReset();
    m_uscem_q.DoCtrlReset();
    m_uscem_alpha.DoCtrlReset();
    m_uscem_beta.DoCtrlReset();
    m_uscem_seed.DoCtrlReset();
}

void DlgOptimizationSCEMAlg::CheckRestrictions()
{
}

