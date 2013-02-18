///////////////////////////////////////////////////////////////////////////////////
//
// TextCtrl.cpp
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
//              Enter text string
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "TextCtrl.h"

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>


//#include <UI/Interface.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextCtrl

CTextCtrl::CTextCtrl() :
    m_blankOK(false),
    m_maxTextLen(0),
    m_pcTextVal(NULL)
{
}

CTextCtrl::~CTextCtrl()
{
}

void CTextCtrl::Set(CWnd* pParent, int nIDEdit, char* inTextVal, int maxLen, bool blankOK)
{
    VERIFY(SubclassDlgItem(nIDEdit, pParent));

    m_pcTextVal = inTextVal;
    m_maxTextLen = maxLen;
    m_blankOK    = blankOK,

        UpdateText();
}

void CTextCtrl::UpdateText()
{
    CString sText = m_pcTextVal;
    if (StringLength(m_pcTextVal) == 0)
        {
            sText = nullStr;
        }
    SetWindowText(sText);
}

void CTextCtrl::DoCtrlReset()
{
    UpdateText();
}


BEGIN_MESSAGE_MAP(CTextCtrl, CEdit)
//{{AFX_MSG_MAP(CTextCtrl)
ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)
ON_WM_CHAR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextCtrl message handlers

void CTextCtrl::OnKillfocus()
{
    CString sText;
    GetWindowText(sText);
    if ((sText.IsEmpty() || (sText == nullStr)) && m_blankOK)
        m_pcTextVal[0] = '\0';
    else
        CopyString(m_pcTextVal, sText, m_maxTextLen);

    UpdateText();
    DoCallbacks();
}

void CTextCtrl::OnSetFocus()
{
    //  InterfaceNode::GetInterface()->SaveContextFocus(GetParent(), m_hWnd);
}


void CTextCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_RETURN) {
        OnKillfocus();
        return;
    }

    CEdit::OnChar(nChar, nRepCnt, nFlags);
}

