///////////////////////////////////////////////////////////////////////////////////
//
// PlotSettingsPage.cpp
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

#include "PlotSettingsPage.h"

#include <genApp/ProjectUtil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotSettingsPage property page

IMPLEMENT_DYNCREATE(CPlotSettingsPage, CPropertyPage)

CPlotSettingsPage::CPlotSettingsPage() : CPropertyPage(CPlotSettingsPage::IDD)
{
    //{{AFX_DATA_INIT(CPlotSettingsPage)
    m_nCheckSmallFont = FALSE;
    m_nAlpha = 0;
    //}}AFX_DATA_INIT
}

CPlotSettingsPage::~CPlotSettingsPage()
{
}

void CPlotSettingsPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPlotSettingsPage)
    DDX_Check(pDX, IDC_CHECK_SMALL, m_nCheckSmallFont);
    //}}AFX_DATA_MAP
    DDX_Slider(pDX, IDC_SLIDER_ALPHA, m_nAlpha);
    DDX_Text(pDX, IDC_EDIT_ALPHA, m_nAlpha);
    DDV_MinMaxInt(pDX, m_nAlpha, 0, 255);
}


BEGIN_MESSAGE_MAP(CPlotSettingsPage, CPropertyPage)
//{{AFX_MSG_MAP(CPlotSettingsPage)
ON_BN_CLICKED(IDC_CHECK_SMALL, OnCheckSmallFont)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlotSettingsPage message handlers

void CPlotSettingsPage::OnCheckSmallFont()
{
    UpdateData();
    SetModified();
}

BEGIN_MESSAGE_MAP(CAlphaEdit, CEdit)
//{{AFX_MSG_MAP(CAlphaEdit)
ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
ON_WM_CHAR()
ON_WM_GETDLGCODE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAlphaEdit::OnKillfocus()
{
    CString sText;
    GetWindowText(sText);
    page->m_nAlpha = atol(sText);

    page->UpdateData(false);
    page->SetModified();
}

void CAlphaEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_RETURN) {
        CString sText;
        GetWindowText(sText);
        page->m_nAlpha = atol(sText);

        page->UpdateData(false);
        page->SetModified();
        return;
    }
    CEdit::OnChar(nChar, nRepCnt, nFlags);
}

UINT CAlphaEdit::OnGetDlgCode()
{
    return DLGC_WANTCHARS|DLGC_WANTARROWS|DLGC_WANTMESSAGE;
}

BEGIN_MESSAGE_MAP(CAlphaSlider, CSliderCtrl)
//{{AFX_MSG_MAP(CAlphaSlider)
ON_WM_HSCROLL_REFLECT()
ON_WM_HSCROLL()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAlphaSlider::HScroll(UINT nSBCode, UINT nPos)
{
    page->m_nAlpha = GetPos();
    page->UpdateData(false);
    page->SetModified();
}


BOOL CPlotSettingsPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    m_sliderAlpha.SubclassDlgItem(IDC_SLIDER_ALPHA, this);
    m_sliderAlpha.SetRange(0, 255, TRUE);
    m_editAlpha.SubclassDlgItem(IDC_EDIT_ALPHA, this);
    m_sliderAlpha.page = this;
    m_editAlpha.page = this;

    bool bWindows2000 = projectUtilities::IsWindows2000();
    m_sliderAlpha.EnableWindow(bWindows2000);
    m_editAlpha.EnableWindow(bWindows2000);

    return TRUE;
}

