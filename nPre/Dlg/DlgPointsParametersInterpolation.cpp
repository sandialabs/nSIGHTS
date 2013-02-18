///////////////////////////////////////////////////////////////////////////////////
//
// DlgPointsParametersInterpolation.cpp
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

// DlgPointsParametersInterpolation.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgPointsParametersInterpolation.h"

#include <UI/NPreInterfaceApp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgPointsParametersInterpolation dialog

IMPLEMENT_DYNCREATE(DlgPointsParametersInterpolation,CGenericDialog)

DlgPointsParametersInterpolation::DlgPointsParametersInterpolation(CWnd* pParent /*=NULL*/)
    : CGenericDialog(DlgPointsParametersInterpolation::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgPointsParametersInterpolation)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void DlgPointsParametersInterpolation::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgPointsParametersInterpolation)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgPointsParametersInterpolation, CGenericDialog)
    //{{AFX_MSG_MAP(DlgPointsParametersInterpolation)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgPointsParametersInterpolation message handlers

BOOL DlgPointsParametersInterpolation::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    TabbedFolderNodePointsParameterInside* pFolder =
        static_cast<TabbedFolderNodePointsParameterInside*>(GetParent()->GetParent());
    m_pParameter = pFolder->m_pParameter;
    m_pCrv = &m_pParameter->GetPointsCurve()->pointsCurve;

    m_ucurveType.AddMappedEntry("Cubic Spline", Curve::ctCubicSpline);
    m_ucurveType.AddMappedEntry("Polynomial", Curve::ctPolynomial);
    m_ucurveType.AddMappedEntry("Linear", Curve::ctLinear);
    m_ucurveType.AddMappedEntry("Step - Mid", Curve::ctStepMid);
    m_ucurveType.AddMappedEntry("Step - Full", Curve::ctStepFull);
    m_ucurveType.Set(this, IDC_COMBO1,  (int*) (&m_pCrv->curveType));
    m_ucurveType.AddCallback(this);

    m_usplineNaturalSlope.Set(this, IDC_COMBO2,  m_pCrv->splineNaturalSlope, "User Set", "Natural");
    m_usplineNaturalSlope.AddCallback(this);

    m_ufixedSplineStartSlope.Set(this, IDC_EDIT1, m_pCrv->fixedSplineStartSlope, SC_SciFormat(5), -1E+05, 1.0E+05);
    m_ufixedSplineEndSlope.Set(this, IDC_EDIT2, m_pCrv->fixedSplineEndSlope, SC_SciFormat(5), -1E+05, 1.0E+05);
    m_usplineTension.Set(this, IDC_EDIT3, m_pCrv->splineTension, SC_SciFormat(5), 0.00001, 1000.0);

    m_upolyOrder.Set(this, IDC_EDIT4, m_pCrv->polyOrder, 1, 10);


    return TRUE;
}

void DlgPointsParametersInterpolation::DoCtrlUpdate()
{
    CheckRestrictions();
}

void DlgPointsParametersInterpolation::CheckRestrictions()
{
    bool curveIsSpline = (m_pCrv->curveType == Curve::ctCubicSpline);
    m_usplineNaturalSlope.EnableWindow(curveIsSpline);
    m_ufixedSplineStartSlope.EnableWindow(curveIsSpline && (!m_pCrv->splineNaturalSlope));
    m_ufixedSplineEndSlope.EnableWindow(curveIsSpline && (!m_pCrv->splineNaturalSlope));
    m_usplineTension.EnableWindow(curveIsSpline);

    m_upolyOrder.EnableWindow(m_pCrv->curveType == Curve::ctPolynomial);
}


void DlgPointsParametersInterpolation::ResetAllControls()
{
    m_ucurveType.DoCtrlReset();
    m_usplineNaturalSlope.DoCtrlReset();
    m_ufixedSplineStartSlope.DoCtrlReset();
    m_ufixedSplineEndSlope.DoCtrlReset();
    m_usplineTension.DoCtrlReset();
    m_upolyOrder.DoCtrlReset();
    CheckRestrictions();
}


