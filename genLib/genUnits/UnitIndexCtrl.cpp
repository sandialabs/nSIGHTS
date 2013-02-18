///////////////////////////////////////////////////////////////////////////////////
//
// UnitIndexCtrl.cpp
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
//      unit selection for resource based dialogs
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "UnitIndexCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitIndexCtrl

CUnitIndexCtrl::CUnitIndexCtrl() :
    m_pUnitIndex(NULL)
{
}

CUnitIndexCtrl::~CUnitIndexCtrl()
{
}

void CUnitIndexCtrl::Set(CWnd* pParent, int nIDCombo, UnitIndex* pUnitIndex)
{
    VERIFY(SubclassDlgItem(nIDCombo, pParent));

    m_pUnitIndex = pUnitIndex;
    int unitType = m_pUnitIndex->unitType;
    CString sText;
    int nMaxConvIndex = UnitsBase::GetMaxConversionIndex(unitType);
    for ( int i = 0; i < nMaxConvIndex+1; i++ ) {
        sText = UnitsBase::GetUnitString(unitType, i);
        AddString(sText);
    }
    ResetSelection();
    EnableWindow(unitType != uNone);
}

void CUnitIndexCtrl::ResetSelection()
{
    SetCurSel(m_pUnitIndex->GetConvIndex());
}

void CUnitIndexCtrl::DoUnitConvChange(int newConvIndex)
{
    m_pUnitIndex->SetConvIndex(newConvIndex);
    ResetSelection();
}

void CUnitIndexCtrl::DoCtrlReset()
{
    ResetSelection();
}


BEGIN_MESSAGE_MAP(CUnitIndexCtrl, CComboBox)
    //{{AFX_MSG_MAP(CUnitIndexCtrl)
    ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitIndexCtrl message handlers

void CUnitIndexCtrl::OnSelchange()
{
    int nIndex = GetCurSel();
    m_pUnitIndex->SetConvIndex(nIndex);
//  GetParent()->UpdateData(FALSE);
    DoCallbacks();
}

