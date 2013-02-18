///////////////////////////////////////////////////////////////////////////////////
//
// DlgPointsParametersUnitsTransform.cpp
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
//      nPre dialog.
//
///////////////////////////////////////////////////////////////////////////////////

// DlgPointsParametersUnitsTransform.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgPointsParametersUnitsTransform.h"

#include <UI/NPreInterfaceApp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgPointsParametersUnitsTransform dialog

IMPLEMENT_DYNCREATE(DlgPointsParametersUnitsTransform,CGenericDialog)

DlgPointsParametersUnitsTransform::DlgPointsParametersUnitsTransform(CWnd* pParent /*=NULL*/)
    : CGenericDialog(DlgPointsParametersUnitsTransform::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgPointsParametersUnitsTransform)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void DlgPointsParametersUnitsTransform::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgPointsParametersUnitsTransform)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgPointsParametersUnitsTransform, CGenericDialog)
    //{{AFX_MSG_MAP(DlgPointsParametersUnitsTransform)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgPointsParametersUnitsTransform message handlers

BOOL DlgPointsParametersUnitsTransform::OnInitDialog()
{
    CGenericDialog::OnInitDialog();
    TabbedFolderNodePointsParameterInside* pFolder =
        static_cast<TabbedFolderNodePointsParameterInside*>(GetParent()->GetParent());


    m_pParameter = pFolder->m_pParameter;
    m_pParameterCurve = m_pParameter->GetPointsCurve();

    m_uxUnitIndex.Set(this, IDC_COMBO1, &m_pParameterCurve->xUnitIndex);
    m_uyUnitIndex.Set(this, IDC_COMBO2, &m_pParameterCurve->yUnitIndex);

    m_uxIsLog10.Set(this, IDC_COMBO3, m_pParameterCurve->xIsLog10);
    m_uyIsLog10.Set(this, IDC_COMBO4, m_pParameterCurve->yIsLog10);

    return TRUE;
}

void DlgPointsParametersUnitsTransform::ResetAllControls()
{
    m_uxUnitIndex.DoCtrlReset();
    m_uyUnitIndex.DoCtrlReset();
    m_uxIsLog10.DoCtrlReset();
    m_uyIsLog10.DoCtrlReset();
}


