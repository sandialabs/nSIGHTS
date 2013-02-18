///////////////////////////////////////////////////////////////////////////////////
//
// DlgOptimizationSimplexAlg.cpp
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

#include "DlgOptimizationSimplexAlg.h"

#include <osMain/G_Optimize.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationSimplexAlg dialog

IMPLEMENT_DYNCREATE(DlgOptimizationSimplexAlg,CGenericDialog)

DlgOptimizationSimplexAlg::DlgOptimizationSimplexAlg(CWnd* pParent /*=NULL*/)
    : CGenericDialog(DlgOptimizationSimplexAlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgOptimizationSimplexAlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(DlgOptimizationSimplexAlg, CGenericDialog)
    //{{AFX_MSG_MAP(DlgOptimizationSimplexAlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationSimplexAlg message handlers

BOOL DlgOptimizationSimplexAlg::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_usimplexSpan.Set(this, IDC_EDIT1, simplexSpan,
                            SC_DecFormat(5), 0.0001, 0.25);


    m_usimplexReflectionCoeff.Set(this, IDC_EDIT2, simplexReflectionCoeff,
                            SC_DecFormat(3), 0.001, 1000.0);

    m_usimplexContractionCoeff.Set(this, IDC_EDIT3, simplexContractionCoeff,
                            SC_DecFormat(3), 0.001, 1.0);

    m_usimplexExpansionCoeff.Set(this, IDC_EDIT4, simplexExpansionCoeff,
                            SC_DecFormat(3), 1.001, 1000.0);

    m_usimplexShrinkageCoeff.Set(this, IDC_EDIT5, simplexShrinkageCoeff,
                            SC_DecFormat(3), 0.050, 0.99);


    CheckRestrictions();

    return TRUE;
}

void DlgOptimizationSimplexAlg::ResetAllControls()
{
    m_usimplexSpan.DoCtrlReset();
    m_usimplexReflectionCoeff.DoCtrlReset();
    m_usimplexContractionCoeff.DoCtrlReset();
    m_usimplexExpansionCoeff.DoCtrlReset();
    m_usimplexShrinkageCoeff.DoCtrlReset();

}

void DlgOptimizationSimplexAlg::CheckRestrictions()
{
    m_usimplexReflectionCoeff.EnableWindow(true);
    m_usimplexContractionCoeff.EnableWindow(true);
    m_usimplexExpansionCoeff.EnableWindow(true);
    m_usimplexShrinkageCoeff.EnableWindow(true);
}

