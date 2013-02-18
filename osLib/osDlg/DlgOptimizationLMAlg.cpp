///////////////////////////////////////////////////////////////////////////////////
//
// DlgOptimizationLMAlg.cpp
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

#include "DlgOptimizationLMAlg.h"

#include <osMain/G_Optimize.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationLMAlg dialog

IMPLEMENT_DYNCREATE(DlgOptimizationLMAlg,CGenericDialog)

DlgOptimizationLMAlg::DlgOptimizationLMAlg(CWnd* pParent /*=NULL*/)
    : CGenericDialog(DlgOptimizationLMAlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgOptimizationLMAlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void DlgOptimizationLMAlg::ResetAllControls()
{
    m_ulmLambdaFactor.DoCtrlReset();
    m_ulmInitialLambda.DoCtrlReset();
    m_ulmMinLambda.DoCtrlReset();
    m_ulmRelChangeTolerance.DoCtrlReset();
    m_ulmInitialDerivSpan.DoCtrlReset();
    m_ulmMINPACKLambda.DoCtrlReset();
    m_normalizePar.DoCtrlReset();
    m_lmAdjustDerivSpan.DoCtrlReset();

    CheckRestrictions();
}

BEGIN_MESSAGE_MAP(DlgOptimizationLMAlg, CGenericDialog)
    //{{AFX_MSG_MAP(DlgOptimizationLMAlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationLMAlg message handlers

BOOL DlgOptimizationLMAlg::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_ulmLambdaFactor.Set(this, IDC_EDIT1, lmLambdaFactor,
                            SC_DecFormat(3), 1.001, 100);

    m_ulmInitialLambda.Set(this, IDC_EDIT2, lmInitialLambda,
                            SC_SciFormat(3), 1.E-10, 1.0);

    m_ulmMinLambda.Set(this, IDC_EDIT3, lmMinLambda,
                            SC_SciFormat(3), 1.E-10, 1.0);

    m_ulmRelChangeTolerance.Set(this, IDC_EDIT4, lmRelChangeTolerance,
                            SC_SciFormat(3), 1.E-12, 0.1);

    m_ulmMINPACKLambda.Set(this, IDC_EDIT6, lmLambda,
                            SC_DecFormat(3), 0.001, 1000);

    m_ulmInitialDerivSpan.Set(this, IDC_EDIT5,lmInitialDerivSpan,
                            SC_SciFormat(3), 1.E-10, 0.1);

    m_uuseLastLMForDerivativeCalc.Set(this, IDC_COMBO1, useLastLMForDerivativeCalc);

    m_normalizePar.Set(this, IDC_COMBO2, normalizeParMINPACK);

    m_lmAdjustDerivSpan.Set(this, IDC_COMBO3, lmAdjustDerivSpan);

    CheckRestrictions();

    return TRUE;

}

void DlgOptimizationLMAlg::CheckRestrictions()
{

    bool minpack = false;
    m_ulmLambdaFactor.EnableWindow(!minpack);

    m_ulmInitialLambda.EnableWindow(!minpack);

    m_ulmMinLambda.EnableWindow(!minpack);

    m_ulmRelChangeTolerance.EnableWindow(!minpack);

    m_ulmMINPACKLambda.EnableWindow(minpack);

    m_lmAdjustDerivSpan.EnableWindow(minpack);

    m_normalizePar.EnableWindow(minpack);

    bool changeDerivSpan = true;
    if (minpack)
        changeDerivSpan = OptimizationControlStatics::lmAdjustDerivSpan;
    m_uuseLastLMForDerivativeCalc.EnableWindow(changeDerivSpan);

}

