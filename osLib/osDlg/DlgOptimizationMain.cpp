///////////////////////////////////////////////////////////////////////////////////
//
// DlgOptimizationMain.cpp
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

#include "DlgOptimizationMain.h"

#include <osMain/G_Optimize.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationMain dialog

IMPLEMENT_DYNCREATE(DlgOptimizationMain,CGenericDialog)

DlgOptimizationMain::DlgOptimizationMain(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgOptimizationMain::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgOptimizationMain)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void DlgOptimizationMain::DoDataExchange(CDataExchange* pDX)
{
    CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgOptimizationMain)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgOptimizationMain, CGenericDialog)
    //{{AFX_MSG_MAP(DlgOptimizationMain)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationMain message handlers

BOOL DlgOptimizationMain::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_usingleStageOpt.Set(this, IDC_COMBO1, singleStageOpt, "Two stage", "Single stage");
    m_usingleStageOpt.AddCallback(this);

    m_ufirstStageOptType.AddMappedEntry("Simplex", otSimplex);
    m_ufirstStageOptType.AddMappedEntry("Lev-Mar", otLevMar);
    m_ufirstStageOptType.AddMappedEntry("Sim. Anneal", otSimAnneal);
    m_ufirstStageOptType.AddMappedEntry("SCEM", otSCEM);
    m_ufirstStageOptType.Set(this, IDC_COMBO2,  (int*) (&firstStageOptType));
    m_ufirstStageOptType.AddCallback(this);

    m_usecondStageOptType.AddMappedEntry("Simplex", otSimplex);
    m_usecondStageOptType.AddMappedEntry("Lev-Mar", otLevMar);
    m_usecondStageOptType.AddMappedEntry("Sim. Anneal", otSimAnneal);
    m_usecondStageOptType.AddMappedEntry("SCEM", otSCEM);
    m_usecondStageOptType.Set(this, IDC_COMBO3,  (int*) (&secondStageOptType));
    m_usecondStageOptType.AddCallback(this);


    m_ucalc95Limits.Set(this, IDC_COMBO4, calcFinalCovariance);
    m_ucalc95Limits.AddCallback(this);

    m_uuse2ndOrderCovar.Set(this, IDC_COMBO5, use2ndOrderCovar, "1st Order", "2nd Order");

    m_uuseFixedDerivSpan.Set(this, IDC_COMBO6, useFixedDerivSpan, "Calculated", "Fixed");
    m_uuseFixedDerivSpan.AddCallback(this);

    m_ufixedDerivSpan.Set(this, IDC_EDIT1, fixedDerivSpan,
                        SC_SciFormat(3), 1.0E-12, 0.10);

    m_uinitialDerivSpan.Set(this, IDC_EDIT2, initialDerivSpan,
                        SC_SciFormat(3), 1.0E-12, 0.10);

    m_ustartMultipleFromOriginalEst.Set(this, IDC_COMBO7, startMultipleFromOriginalEst, "last result", "best estimate");

    m_uupdateParameterEstimates.Set(this, IDC_COMBO8, updateParameterEstimates);

    InitData(true);

    return TRUE;
}

void DlgOptimizationMain::CheckRestrictions()
{
    m_usecondStageOptType.EnableWindow(!singleStageOpt);

    m_uuse2ndOrderCovar.EnableWindow(calcFinalCovariance);

    m_uuseFixedDerivSpan.EnableWindow(calcFinalCovariance);

    m_ufixedDerivSpan.EnableWindow(useFixedDerivSpan && calcFinalCovariance);

    m_uinitialDerivSpan.EnableWindow((!useFixedDerivSpan) && calcFinalCovariance);

    static_cast<DialogNode*>(GetParent())->CheckRestrictions();
}

void DlgOptimizationMain::ResetAllControls()
{
    m_usingleStageOpt.DoCtrlReset();
    m_ufirstStageOptType.DoCtrlReset();
    m_usecondStageOptType.DoCtrlReset();

    m_ucalc95Limits.DoCtrlReset();
    m_uuse2ndOrderCovar.DoCtrlReset();
    m_uuseFixedDerivSpan.DoCtrlReset();
    m_ufixedDerivSpan.DoCtrlReset();
    m_uinitialDerivSpan.DoCtrlReset();

    m_ustartMultipleFromOriginalEst.DoCtrlReset();
    m_uupdateParameterEstimates.DoCtrlReset();
}


