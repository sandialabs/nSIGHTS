///////////////////////////////////////////////////////////////////////////////////
//
// HelpDialog.cpp
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

#include "HelpDialog.h"

#include <HtmlHelp.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpDialog dialog


CHelpDialog::CHelpDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/) :
    CDialog(nIDTemplate, pParent)
{
    //{{AFX_DATA_INIT(CHelpDialog)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void CHelpDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CHelpDialog)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelpDialog, CDialog)
    //{{AFX_MSG_MAP(CHelpDialog)
    ON_WM_HELPINFO()
    ON_WM_CONTEXTMENU()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpDialog message handlers

CString CHelpDialog::GetContextFile()
{
    CString sContextFile;
    sContextFile.Format("%s::/%s", AfxGetApp()->m_pszHelpFilePath, GetHelpName());
    return sContextFile;
}

BOOL CHelpDialog::OnHelpInfo( HELPINFO* pHelpInfo )
{
    if (pHelpInfo->iContextType == HELPINFO_WINDOW) {
        return ::HtmlHelp(HWND(pHelpInfo->hItemHandle),
                                GetContextFile(),
                                HH_TP_HELP_WM_HELP,
                                GetHelpMap()) != NULL;
    }
    return TRUE;
}


void CHelpDialog::OnContextMenu( CWnd* pWnd, CPoint point )
{
    if (this == pWnd) {
        return;
    }
    ::HtmlHelp(pWnd->GetSafeHwnd(),
                    GetContextFile(),
                    HH_TP_HELP_CONTEXTMENU,
                    GetHelpMap());
}

BOOL CHelpDialog::PreTranslateMessage(MSG* pMsg)
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
    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CHelpDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    ModifyStyleEx(0, WS_EX_CONTEXTHELP);

    return TRUE;
}
