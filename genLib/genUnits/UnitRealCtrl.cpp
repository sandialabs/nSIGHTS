///////////////////////////////////////////////////////////////////////////////////
//
// UnitRealCtrl.cpp
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
//      combined value entry/unit selection for resource based dialogs
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include "UnitRealCtrl.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

UnitRealRealCtrl::UnitRealRealCtrl() :
    m_pUnitReal(NULL),
    m_boverRideFormat(false),
    m_fminMetricVal(-1.0E+99),
    m_fmaxMetricVal(1.0E+99),
    m_userVal(0.0)
{
}

void UnitRealRealCtrl::Set(CWnd* pParent, int nIDEdit, UnitReal& inUnitReal,
                            const SC_RealFormat* overrideFormat,
                            const double& inMinMetricVal, const double& inMaxMetricVal)
{
    m_pUnitReal = &inUnitReal;

    m_boverRideFormat = (overrideFormat != NULL);
    if (m_boverRideFormat)
        m_overRideFormat = *overrideFormat;
    m_fminMetricVal = inMinMetricVal;
    m_fmaxMetricVal = inMaxMetricVal;
    ResetLimits();

    m_userVal = m_pUnitReal->GetUserVal();

    CRealCtrl::Set(pParent, nIDEdit, m_userVal);

}

void UnitRealRealCtrl::ResetLimits()
{
    m_isLimited = true;
    m_fMinLimit = m_pUnitReal->MetricToUser(m_fminMetricVal);
    m_fMaxLimit = m_pUnitReal->MetricToUser(m_fmaxMetricVal);
    if (m_boverRideFormat)
    {
        m_currFormat = m_overRideFormat;
    }
    else
    {
        m_currFormat = m_pUnitReal->GetUnitRealFormat();
    }
}


void UnitRealRealCtrl::UpdateText()
{
    m_pUnitReal->SetUserVal(m_userVal);
    CRealCtrl::UpdateText();
}

void UnitRealRealCtrl::DoUnitConvChange()
{
    ResetLimits();
    m_userVal = m_pUnitReal->GetUserVal();
    CRealCtrl::UpdateText();
}

void UnitRealRealCtrl::DoCtrlReset()
{
    m_userVal = m_pUnitReal->GetUserVal();
    CRealCtrl::UpdateText();
}


/////////////////////////////////////////////////////////////////////////////
// CUnitRealCtrl

CUnitRealCtrl::CUnitRealCtrl()
{
}

CUnitRealCtrl::~CUnitRealCtrl()
{
}

void CUnitRealCtrl::Set(CWnd* pParent, int nIDCombo, int nIDEdit,
                        UnitReal* pUnitReal, const SC_RealFormat* poverRideFormat)
{
    m_uIndexCtrl.Set(pParent, nIDCombo, pUnitReal);
    m_uIndexCtrl.AddCallback(this);
    m_uRealCtrl.Set(pParent, nIDEdit, *pUnitReal, poverRideFormat, -1.0E+99, 1.0E+99);
}

void CUnitRealCtrl::Set(CWnd* pParent, int nIDCombo, int nIDEdit,
                        UnitReal* pUnitReal, const SC_RealFormat* poverRideFormat,
                        const double& inMinMetricVal, const double& inMaxMetricVal)

{
    m_uIndexCtrl.Set(pParent, nIDCombo, pUnitReal);
    m_uIndexCtrl.AddCallback(this);
    m_uRealCtrl.Set(pParent, nIDEdit, *pUnitReal, poverRideFormat, inMinMetricVal, inMaxMetricVal);
}


BOOL CUnitRealCtrl::EnableWindowEx(BOOL bEnable)
{
    m_uRealCtrl.EnableWindow(bEnable);
    BOOL bRc = m_uIndexCtrl.EnableWindow(bEnable);
    return bRc;
}

void CUnitRealCtrl::DoCtrlUpdate()
{
    m_uRealCtrl.DoUnitConvChange();
}


void CUnitRealCtrl::DoUnitConvChange(int newConvIndex)
{
    m_uIndexCtrl.DoUnitConvChange(newConvIndex);
    m_uRealCtrl.DoUnitConvChange();
}

void CUnitRealCtrl::DoCtrlReset()
{
    m_uIndexCtrl.DoCtrlReset();
    m_uRealCtrl.DoCtrlReset();
}

