///////////////////////////////////////////////////////////////////////////////////
//
// DlgOptimizationTolerances.cpp
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

#include "DlgOptimizationTolerances.h"

#include <osMain/G_Optimize.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationTolerances dialog

IMPLEMENT_DYNCREATE(DlgOptimizationTolerances,CGenericDialog)

DlgOptimizationTolerances::DlgOptimizationTolerances(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgOptimizationTolerances::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgOptimizationTolerances)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

}

BOOL DlgOptimizationTolerances::OnInitDialog()
{
    CGenericDialog::OnInitDialog();


    m_uuseFitTolerance.Set(this, IDC_CHECK_FIT_TOLERANCE, OptimizationControlStatics::useFitTolerance);
    m_uuseFitTolerance.AddCallback(this);
    m_ufitTolerance.Set(this, IDC_EDIT_FIT_TOLERANCE,
                            OptimizationControlStatics::fitTolerance,
                            SC_SciFormat(3), 1.0E-12, 0.010);


    m_uuseParameterTolerance.Set(this, IDC_CHECK_PARAMETER_TOLERANCE, OptimizationControlStatics::useParameterTolerance);
    m_uuseParameterTolerance.AddCallback(this);
    m_uparameterTolerance.Set(this, IDC_EDIT_PARAMETER_TOLERANCE,
                            OptimizationControlStatics::parameterTolerance,
                            SC_SciFormat(3), 1.0E-12, 0.010);


    m_ucombineToleranceWithOR.Set(this, IDC_COMBO1, OptimizationControlStatics::combineToleranceWithOR, "Pass Both", "Pass One");


    m_utransitionToleranceMultiplier.Set(this, IDC_EDIT1,
                        OptimizationControlStatics::transitionToleranceMultiplier,
                            SC_DecFormat(1), 2.0, 10000.0);


    m_uderivativeTolerance.Set(this, IDC_EDIT2,
                            OptimizationControlStatics::derivativeTolerance,
                            SC_SciFormat(3), 1.0E-12, 0.010);

    m_umaxOptRun.Set(this, IDC_EDIT3,
                            OptimizationControlStatics::maxOptRun,
                            100, 100000);

    CheckRestrictions();

    return TRUE;
}

void DlgOptimizationTolerances::ResetAllControls()
{
    m_uuseFitTolerance.DoCtrlReset();
    m_uuseParameterTolerance.DoCtrlReset();
    m_ufitTolerance.DoCtrlReset();
    m_uparameterTolerance.DoCtrlReset();
    m_ucombineToleranceWithOR.DoCtrlReset();
    m_uderivativeTolerance.DoCtrlReset();
    m_utransitionToleranceMultiplier.DoCtrlReset();
    m_umaxOptRun.DoCtrlReset();
    CheckRestrictions();
}

void DlgOptimizationTolerances::CheckRestrictions()
{
    bool optTypeNL = false;
    //KSB 2/29/12 -- this boolean isn't even used below, code bloat.

    m_ufitTolerance.EnableWindow(OptimizationControlStatics::useFitTolerance);
    m_uparameterTolerance.EnableWindow(OptimizationControlStatics::useParameterTolerance);

    m_utransitionToleranceMultiplier.EnableWindow(!OptimizationControlStatics::singleStageOpt);
}

BEGIN_MESSAGE_MAP(DlgOptimizationTolerances, CGenericDialog)
    //{{AFX_MSG_MAP(DlgOptimizationTolerances)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationTolerances message handlers

