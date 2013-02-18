///////////////////////////////////////////////////////////////////////////////////
//
// OpenGLSettingsPage.cpp
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

#include "GenLibRes.h"

#include "OpenGlSettingsPage.h"

#include <genClassPS/PS_OGLBase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// COpenGLSettingsPage dialog

COpenGLSettingsPage::COpenGLSettingsPage() : CPropertyPage(COpenGLSettingsPage::IDD)
{
    //{{AFX_DATA_INIT(COpenGLSettingsPage)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_nPFD = 0;
}


void COpenGLSettingsPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(COpenGLSettingsPage)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenGLSettingsPage, CPropertyPage)
//{{AFX_MSG_MAP(COpenGLSettingsPage)
ON_CBN_SELCHANGE(IDC_COMBO_PFD, OnSelchangeComboPfd)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLSettingsPage message handlers

BOOL COpenGLSettingsPage::OnInitDialog()
{
    // should only need to do this once
    if (availPfds.IsEmpty())
        PS_OGLBase::GetAllPfds(this, availPfds);

    m_combo.SubclassDlgItem(IDC_COMBO_PFD, this);
    CString sItem = "   Default   ";
    m_combo.AddString(sItem);
    int i;
    for (i = 0; i < availPfds.Size(); i++) {
        sItem.Format("%2d \t%2d \t%2d \t%2d \t %2d\t %2d  \t",
                     availPfds[i].pfdIndex,
                     availPfds[i].redDepth,
                     availPfds[i].greenDepth,
                     availPfds[i].blueDepth,
                     availPfds[i].colorDepth,
                     availPfds[i].zDepth);
        if (availPfds[i].isDoubleBuffered) {
            sItem += "Dbl ";
        } else {
            sItem += "Sngl";
        }
        if (availPfds[i].isStereo) {
            sItem += "\tStereo";
        } else {
            sItem += "\tMono";
        }

        m_combo.AddString(sItem);
    }


    if (PS_OGLBase::IsInitialized()) {

        // usually pfdIndex is same as position in array -- but not guaranteed
        int pfdIndex = PS_OGLBase::GetCurrentPfd().pfdIndex;
        int currIndex = 0;
        for (i = 0; i < availPfds.Size(); i++)
            if (availPfds[i].pfdIndex == pfdIndex)
                currIndex = i + 1;

        m_combo.SetCurSel(currIndex);
        m_combo.ModifyStyle(0, WS_DISABLED);

        char glVersionStr[200];
        char glVendorStr[200];
        char glRendererStr[200];
        PS_OGLBase::GetOpenGLStrings(glVersionStr, glVendorStr, glRendererStr, 200);
        GetDlgItem(IDC_EDIT_VERSION)->SetWindowText(glVersionStr);
        GetDlgItem(IDC_EDIT_VENDOR)->SetWindowText(glVendorStr);
        GetDlgItem(IDC_EDIT_RENDERER)->SetWindowText(glRendererStr);
    } else {

        m_combo.SetCurSel(m_nPFD);

        GetDlgItem(IDC_EDIT_VERSION)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_VERSION)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_VENDOR)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_VENDOR)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_RENDERER)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_RENDERER)->ShowWindow(SW_HIDE);
    }

    return TRUE;
}

void COpenGLSettingsPage::OnSelchangeComboPfd()
{
    m_nPFD = m_combo.GetCurSel();
    // if not default
    if (m_nPFD > 0)
        m_pfdIndex = availPfds[m_nPFD - 1].pfdIndex;
    else
        m_pfdIndex = 0;

    SetModified();
}

