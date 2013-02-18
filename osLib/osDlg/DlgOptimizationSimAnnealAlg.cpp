///////////////////////////////////////////////////////////////////////////////////
//
// DlgOptimizationSimAnnealAlg.cpp
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

#include "DlgOptimizationSimAnnealAlg.h"

#include <osMain/G_Optimize.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationSimAnnealAlg dialog

IMPLEMENT_DYNCREATE(DlgOptimizationSimAnnealAlg,CGenericDialog)

DlgOptimizationSimAnnealAlg::DlgOptimizationSimAnnealAlg(CWnd* pParent /*=NULL*/)
    : CGenericDialog(DlgOptimizationSimAnnealAlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgOptimizationSimAnnealAlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(DlgOptimizationSimAnnealAlg, CGenericDialog)
    //{{AFX_MSG_MAP(DlgOptimizationSimAnnealAlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationSimAnnealAlg message handlers

BOOL DlgOptimizationSimAnnealAlg::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_usimplexReflectionCoeff.Set(this, IDC_EDIT1, simplexReflectionCoeff,
                            SC_DecFormat(3), 0.001, 1000.0);

    m_usimplexContractionCoeff.Set(this, IDC_EDIT2, simplexContractionCoeff,
                            SC_DecFormat(3), 0.001, 1.0);

    m_usimplexExpansionCoeff.Set(this, IDC_EDIT3, simplexExpansionCoeff,
                            SC_DecFormat(3), 1.001, 1000.0);

    m_usimplexShrinkageCoeff.Set(this, IDC_EDIT4, simplexShrinkageCoeff,
                            SC_DecFormat(3), 0.050, 0.99);

    m_usaRandomSeed.Set(this, IDC_EDIT5, saRandomSeed, 1001, 1000000);


    m_ucoolingSchedule.AddMappedEntry("NR #1", csNR1);
    m_ucoolingSchedule.AddMappedEntry("NR #2", csNR2);
    m_ucoolingSchedule.AddMappedEntry("Luke #1", csLuke1);
    m_ucoolingSchedule.AddMappedEntry("Luke #2", csLuke2);
    m_ucoolingSchedule.AddMappedEntry("Luke #7", csLuke7);
    m_ucoolingSchedule.AddMappedEntry("Luke #8", csLuke8);
    m_ucoolingSchedule.AddMappedEntry("Luke #9", csLuke9);
    m_ucoolingSchedule.Set(this, IDC_COMBO1,  (int*) (&coolingSchedule));
    m_ucoolingSchedule.AddCallback(this);

    m_usa_T0.Set(this, IDC_EDIT6, sa_T0,
                      SC_DecFormat(2), 0.01, 1000000.0);

    m_usa_N.Set(this, IDC_EDIT7, sa_N,
                        10, 10000);


    m_usaL_TN.Set(this, IDC_EDIT11, saL_TN,
                      SC_DecFormat(2), 0.0, 1000000.0);

    CheckRestrictions();

    return TRUE;
}

void DlgOptimizationSimAnnealAlg::ResetAllControls()

{
    m_usimplexReflectionCoeff.DoCtrlReset();
    m_usimplexContractionCoeff.DoCtrlReset();
    m_usimplexExpansionCoeff.DoCtrlReset();
    m_usimplexShrinkageCoeff.DoCtrlReset();
    m_usaRandomSeed.DoCtrlReset();
    m_ucoolingSchedule.DoCtrlReset();
    m_usa_T0.DoCtrlReset();
    m_usa_N.DoCtrlReset();
    m_usaNR_m.DoCtrlReset();
    m_usaNR_epsilon.DoCtrlReset();
    m_usaNR_alpha.DoCtrlReset();
//  NR3 support removed Sep 05 by KV request
//  m_usaNR_beta.DoCtrlReset();
//  m_usaNR_gamma.DoCtrlReset();
    m_usaL_TN.DoCtrlReset();

}

void DlgOptimizationSimAnnealAlg::CheckRestrictions()
{
    m_usa_N.EnableWindow((coolingSchedule == csNR2) ||
                         (coolingSchedule >= csLuke1));

    m_usaNR_m.EnableWindow((coolingSchedule >= csNR1) &&
                           (coolingSchedule <= csNR2));

    m_usaNR_epsilon.EnableWindow(coolingSchedule == csNR1);
    m_usaNR_alpha.EnableWindow(coolingSchedule == csNR2);

//  NR3 support removed Sep 05 by KV request
//  m_usaNR_beta.EnableWindow(coolingSchedule == csNR3);
//  m_usaNR_gamma.EnableWindow(coolingSchedule == csNR3);

    m_usaL_TN.EnableWindow(coolingSchedule >= csLuke1);
}

