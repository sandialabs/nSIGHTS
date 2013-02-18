///////////////////////////////////////////////////////////////////////////////////
//
// HelpPropertySheet.cpp
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

#include "GenLibAfx.h"

#include "HelpPropertySheet.h"

#include <HtmlHelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CHelpPropertySheet, CPropertySheet)

CHelpPropertySheet::CHelpPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
    :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CHelpPropertySheet::CHelpPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
    :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CHelpPropertySheet::~CHelpPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CHelpPropertySheet, CPropertySheet)
    //{{AFX_MSG_MAP(CHelpPropertySheet)
    ON_WM_HELPINFO()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpPropertySheet message handlers

BOOL CHelpPropertySheet::PreTranslateMessage(MSG* pMsg)
{
    // Magical mystical MFC F1 Help msg!
    if (pMsg->message == 0x4d) {
        if (GetKeyState(VK_SHIFT) < 0) {
            // Shift key  down
            // Use this if you want Shift+F1 to create the floating arrow instead
            SendMessage(WM_SYSCOMMAND, SC_CONTEXTHELP);
            return TRUE;
        }
    }
    return CPropertySheet::PreTranslateMessage(pMsg);
}

BOOL CHelpPropertySheet::OnInitDialog()
{
    BOOL bResult = CPropertySheet::OnInitDialog();

    ModifyStyleEx(0, WS_EX_CONTEXTHELP);

    return bResult;
}

BOOL CHelpPropertySheet::OnHelpInfo(HELPINFO* pHelpInfo)
{
    if (pHelpInfo->iContextType == HELPINFO_WINDOW) {
        //AfxGetApp()->WinHelp(pHelpInfo->dwContextId, HELP_CONTEXTPOPUP);
        return ::HtmlHelp(HWND(pHelpInfo->hItemHandle),
                                GetContextFile(),
                                HH_TP_HELP_WM_HELP,
                                GetHelpMap()) != NULL;
    }
    return CPropertySheet::OnHelpInfo(pHelpInfo);
}

CString CHelpPropertySheet::GetContextFile()
{
    CString sContextFile;
    sContextFile.Format( "%s::/%s", AfxGetApp()->m_pszHelpFilePath ,GetHelpName() );
    return sContextFile;
}

