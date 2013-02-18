///////////////////////////////////////////////////////////////////////////////////
//
// IntCtrl.cpp
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
// DESCRIPTION: Windows specific code for implementing basic controls with
//              callbacks on resource defined dialog forms.
//              Enter integer value
//
///////////////////////////////////////////////////////////////////////////////////

// RealCtrl.cpp : implementation file
//

#include "GenLibAfx.h"
#include "IntCtrl.h"


#include <genClass/C_Common.h>
#include <genClass/U_Str.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIntCtrl

CIntCtrl::CIntCtrl() :
    m_iMinLimit(0),
    m_iMaxLimit(10000),
    m_piDataVal(NULL)
{
}

CIntCtrl::~CIntCtrl()
{
}


void CIntCtrl::Set(CWnd* pParent, int nIDEdit, int& inDataVal, int inMinLimit, int inMaxLimit)
{
    VERIFY(SubclassDlgItem(nIDEdit, pParent));

    m_iMinLimit = inMinLimit;
    m_iMaxLimit = inMaxLimit;
    m_piDataVal = &inDataVal;

    UpdateText();
}


void CIntCtrl::UpdateText()
{
    char tempStr[80];
    IntToString(*m_piDataVal, tempStr, 80);
    SetWindowText(tempStr);
}

void CIntCtrl::DoCtrlReset()
{
    UpdateText();
}


BEGIN_MESSAGE_MAP(CIntCtrl, CEdit)
//{{AFX_MSG_MAP(CIntCtrl)
ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
ON_WM_CHAR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntCtrl message handlers

void CIntCtrl::OnKillfocus()
{
    CString sText;
    GetWindowText(sText);
    double tVal;
    if (IsValidReal(sText, tVal) && tVal >= double(m_iMinLimit) && tVal <= double(m_iMaxLimit))
        {
            *m_piDataVal = int(tVal);
        }
    UpdateText();
    DoCallbacks();
}

void CIntCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_RETURN) {
        OnKillfocus();
        return;
    }
    CEdit::OnChar(nChar, nRepCnt, nFlags);
}

