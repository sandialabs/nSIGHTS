///////////////////////////////////////////////////////////////////////////////////
//
// DlgOptimizationPerturbation.cpp
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

#include "DlgOptimizationPerturbation.h"

#include <osMain/G_Optimize.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationPerturbation dialog

IMPLEMENT_DYNCREATE(DlgOptimizationPerturbation,CGenericDialog)

DlgOptimizationPerturbation::DlgOptimizationPerturbation(CWnd* pParent /*=NULL*/)
    : CGenericDialog(DlgOptimizationPerturbation::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgOptimizationPerturbation)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


BOOL DlgOptimizationPerturbation::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_udoOptPerturbations.Set(this, IDC_CHECK1, OptimizationControlStatics::doOptPerturbations);
    m_udoOptPerturbations.AddCallback(this);


    m_unPerturb.Set(this, IDC_EDIT1,
                    OptimizationControlStatics::nPerturb,
                    2, 10000);

    m_uperturbSpan.Set(this, IDC_EDIT2,
                        OptimizationControlStatics::perturbSpan,
                            SC_DecFormat(2), 0.01, 0.90);

    m_uperturbFromStart.Set(this, IDC_COMBO1,  OptimizationControlStatics::perturbFromStart, "Last Fit", "Start");


    m_uperturbMultiFitSameStart.Set(this, IDC_COMBO2,  OptimizationControlStatics::perturbMultiFitSameStart, "New Random", "Same as First");

    m_unperturbRandomSeed.Set(this, IDC_EDIT3,
                    OptimizationControlStatics::perturbRandomSeed,
                    101, 1000000000);

    CheckRestrictions();

    return TRUE;

}

void DlgOptimizationPerturbation::CheckRestrictions()
{
    m_unPerturb.EnableWindow(OptimizationControlStatics::doOptPerturbations);
    m_uperturbSpan.EnableWindow(OptimizationControlStatics::doOptPerturbations);
    m_uperturbFromStart.EnableWindow(OptimizationControlStatics::doOptPerturbations);
    m_uperturbMultiFitSameStart.EnableWindow(OptimizationControlStatics::doOptPerturbations);
    m_unperturbRandomSeed.EnableWindow(OptimizationControlStatics::doOptPerturbations);
}

void DlgOptimizationPerturbation::ResetAllControls()
{
    m_udoOptPerturbations.DoCtrlReset();
    m_unPerturb.DoCtrlReset();
    m_uperturbSpan.DoCtrlReset();
    m_uperturbFromStart.DoCtrlReset();
    m_uperturbMultiFitSameStart.DoCtrlReset();
    m_unperturbRandomSeed.DoCtrlReset();
    CheckRestrictions();
}

BEGIN_MESSAGE_MAP(DlgOptimizationPerturbation, CGenericDialog)
    //{{AFX_MSG_MAP(DlgOptimizationPerturbation)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationPerturbation message handlers

