///////////////////////////////////////////////////////////////////////////////////
//
// RealCtrl.cpp
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
//              Enter real numeric value
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "RealCtrl.h"

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

// #include <UI/Interface.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRealCtrl

CRealCtrl::CRealCtrl() :
    m_isLimited(false),
    m_fMinLimit(0.0),
    m_fMaxLimit(1.0),
    m_pfDataVal(NULL)
{
}

CRealCtrl::~CRealCtrl()
{
}


void CRealCtrl::Set(CWnd* pParent, int nIDEdit, double& inDataVal)
{
    VERIFY(SubclassDlgItem(nIDEdit, pParent));

    m_pfDataVal = &inDataVal;

    UpdateText();
}

void CRealCtrl::Set(CWnd* pParent, int nIDEdit, double& inDataVal,
                    const SC_RealFormat& inFormat)
{
    m_currFormat = inFormat;
    Set(pParent, nIDEdit, inDataVal);

}

void CRealCtrl::Set(CWnd* pParent, int nIDEdit, double& inDataVal,
                    const SC_RealFormat& inFormat,
                    const double& inMinLimit, const double& inMaxLimit)
{
    m_isLimited = true;
    m_fMinLimit = inMinLimit;
    m_fMaxLimit = inMaxLimit;

    Set(pParent, nIDEdit, inDataVal, inFormat);

}

void CRealCtrl::UpdateText()
{
    CString sText;
    if (RealIsNull(*m_pfDataVal))
        {
            sText = nullStr;
        }
    else
        {
            m_currFormat.RealToString(*m_pfDataVal, sText);
        }
    SetWindowText(sText);
}

void CRealCtrl::DoCtrlReset()
{
    UpdateText();
}


BEGIN_MESSAGE_MAP(CRealCtrl, CEdit)
//{{AFX_MSG_MAP(CRealCtrl)
ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)
ON_WM_CHAR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRealCtrl message handlers

void CRealCtrl::OnKillfocus()
{
    CString sText;
    GetWindowText(sText);
    double tVal;
    if (IsValidReal(sText, tVal))
        {
            if (!m_isLimited || (tVal >= m_fMinLimit && tVal <= m_fMaxLimit))
                {
                    *m_pfDataVal = tVal;
                    GenAppClearMsgs();
                }
            else
                {
                    GenAppErrorMsg("RealEntry","value out of range");
                    RangeMsg();
                    //throw new CRangeException;
                }
        }
    else
        {
            if (sText == nullStr)
                GenAppClearMsgs();
            else
                GenAppErrorMsg("RealEntry","not a valid real number");
        }

    UpdateText();
    DoCallbacks();
}

void CRealCtrl::RangeMsg()
{
    CString minText, maxText;
    m_currFormat.RealToString(m_fMinLimit, minText);
    m_currFormat.RealToString(m_fMaxLimit, maxText);
    GenAppParameterRangeMsg(minText, maxText);
}


void CRealCtrl::OnSetFocus()
{
    if (m_isLimited)
        {
            RangeMsg();
        }

    // clear if null
    if (RealIsNull(*m_pfDataVal))
        {
            SetWindowText("");
        }

    //  InterfaceNode::GetInterface()->SaveContextFocus(GetParent(), m_hWnd);
}


void CRealCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_RETURN) {
        OnKillfocus();
        return;
    }

    CEdit::OnChar(nChar, nRepCnt, nFlags);
}

