///////////////////////////////////////////////////////////////////////////////////
//
// SettingsPropSheet.cpp
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

#include <HtmlHelp/ToolTips.h>

#include <genApp/ProjectUtil.h>
#include <genApp/GenAppMessages.h>
#include <genApp/C_MenuRoot.h>

#include <genApp/WindowSelector.h>

#include <genClass/SC_RealFormat.h>

#include <genClassPS/PS_OGLBase.h>

#include "SettingsPropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsPropSheet

IMPLEMENT_DYNAMIC(CSettingsPropSheet, CHelpPropertySheet)

CSettingsPropSheet::CSettingsPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage) :
CHelpPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    AddPage(&m_plotPage);
    AddPage(&m_treeViewPage);
    AddPage(&m_openGLPage);
    AddPage(&m_openGLPropertiesPage);
    AddPage(&m_bitmapFontPage);
    AddPage(&m_calcPriorityPage);
    AddPage(&m_countrySettingsPage);
    AddPage(&m_TemplateDirectoryPage);
}

CSettingsPropSheet::CSettingsPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage) :
    CHelpPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    AddPage(&m_plotPage);
    AddPage(&m_treeViewPage);
    AddPage(&m_openGLPage);
    AddPage(&m_openGLPropertiesPage);
    AddPage(&m_bitmapFontPage);
    AddPage(&m_calcPriorityPage);
    AddPage(&m_countrySettingsPage);
    AddPage(&m_TemplateDirectoryPage);
}

CSettingsPropSheet::~CSettingsPropSheet()
{
}


BEGIN_MESSAGE_MAP(CSettingsPropSheet, CHelpPropertySheet)
//{{AFX_MSG_MAP(CSettingsPropSheet)
ON_COMMAND(ID_APPLY_NOW, OnApplyNow)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsPropSheet message handlers

void CSettingsPropSheet::OnApplyNow()
{
    Default();

    ApplyChanges(true);
}

void CSettingsPropSheet::ApplyChanges(bool bDialog)
{
    if (m_plotPage.m_nCheckSmallFont != m_nCheckSmallFont)
        {
            m_nCheckSmallFont = m_plotPage.m_nCheckSmallFont;
            projectUtilities::SetPlotSmallFont(m_nCheckSmallFont != 0);
            AfxGetMainWnd()->SendMessage(WM_GENAPP_PLOTSETTINGS, WPARAM(m_plotPage.m_nCheckSmallFont));
        }

    if (m_plotPage.m_nAlpha != m_nAlpha)
        {
            m_nAlpha = m_plotPage.m_nAlpha;
            projectUtilities::SetAlpha(m_nAlpha);
            windowSelector.SetAlpha(m_nAlpha);
        }

    if ((m_treeViewPage.m_nSingleLine != m_nSingleLine) ||
        (m_treeViewPage.m_nSingleLineType != m_nSingleLineType) ||
        (m_treeViewPage.m_nShowUnused != m_nShowUnused) ||
        (m_treeViewPage.m_nShowConnections != m_nShowConnections))
        {
            m_nSingleLine = m_treeViewPage.m_nSingleLine;
            m_nSingleLineType = m_treeViewPage.m_nSingleLineType;
            m_nShowUnused = m_treeViewPage.m_nShowUnused;
            m_nShowConnections = m_treeViewPage.m_nShowConnections;
            projectUtilities::SetTreeViewSettings(m_nSingleLine, m_nSingleLineType, m_nShowUnused, m_nShowConnections);
            MenuRootC::UpdateAllUI();
        }

    if (m_openGLPage.m_nPFD != m_nPFD)
        {
            m_nPFD = m_openGLPage.m_nPFD;
            int pfdIndex = m_openGLPage.m_pfdIndex;
            PS_OGLBase::SetPfdSelectionSettings(pfdIndex == 0, pfdIndex);
            projectUtilities::SetOpenGL_PixelFormatDescriptor(m_nPFD, pfdIndex);
        }

    if (m_openGLPropertiesPage.m_nantiAlias2DLine != m_nantiAlias2DLine)
        {
            m_nantiAlias2DLine = m_openGLPropertiesPage.m_nantiAlias2DLine;
            PS_OGLBase::Set2DAntiAliasLines(m_nantiAlias2DLine);
            projectUtilities::SetOpenGL2DAntiAlias(m_nantiAlias2DLine);
        }

    if (m_openGLPropertiesPage.m_nantiAlias3DLine != m_nantiAlias3DLine)
        {
            m_nantiAlias3DLine = m_openGLPropertiesPage.m_nantiAlias3DLine;
            PS_OGLBase::Set3DAntiAliasLines(m_nantiAlias3DLine);
            projectUtilities::SetOpenGL3DAntiAlias(m_nantiAlias3DLine);
        }

    if (m_bitmapFontPage.m_nbitmapFontPlatformType != m_nbitmapFontPlatformType)
        {
            m_nbitmapFontPlatformType = m_bitmapFontPage.m_nbitmapFontPlatformType;
            PS_OGLBase::SetBitmapFontPlatformSettings(PS_OGLBase::BitmapFontPlatform(m_nbitmapFontPlatformType));
            projectUtilities::SetBitmapFontPlatformSetting(m_nbitmapFontPlatformType);
        }

    if (m_calcPriorityPage.m_ncalcPriority != m_ncalcPriority)
        {
            m_ncalcPriority = m_calcPriorityPage.m_ncalcPriority;
            projectUtilities::SetCalculationPrioritySetting(m_ncalcPriority);
        }

    if (m_countrySettingsPage.m_europeanReals != m_europeanReals)
        {
            m_europeanReals = m_countrySettingsPage.m_europeanReals;
            SC_RealFormat::european = m_europeanReals;
            projectUtilities::SetEuropeanReals(m_europeanReals);
        }

    if (m_TemplateDirectoryPage.m_szTemplateDirectoryName != m_szTemplateDirectoryName)
        {
            m_szTemplateDirectoryName = m_TemplateDirectoryPage.m_szTemplateDirectoryName;
            projectUtilities::SetTemplateDirectory(m_szTemplateDirectoryName);
        }

    if (bDialog)
        {
            m_plotPage.SetModified(false);
            m_treeViewPage.SetModified(false);
            m_openGLPage.SetModified(false);
            m_openGLPropertiesPage.SetModified(false);
            m_bitmapFontPage.SetModified(false);
            m_calcPriorityPage.SetModified(false);
            m_countrySettingsPage.SetModified(false);
            m_TemplateDirectoryPage.SetModified(false);
        }

    // Make settings persistent
    projectUtilities::SaveSettings();
}

BOOL CSettingsPropSheet::OnInitDialog()
{
    m_plotPage.m_nCheckSmallFont = m_nCheckSmallFont;
    m_plotPage.m_nAlpha = m_nAlpha;

    m_treeViewPage.m_nSingleLine = m_nSingleLine;
    m_treeViewPage.m_nSingleLineType = m_nSingleLineType;
    m_treeViewPage.m_nShowUnused = m_nShowUnused;
    m_treeViewPage.m_nShowConnections = m_nShowConnections;

    m_openGLPage.m_nPFD = m_nPFD;
    m_openGLPage.m_pfdIndex = m_pfdIndex;

    m_openGLPropertiesPage.m_nantiAlias2DLine = m_nantiAlias2DLine;
    m_openGLPropertiesPage.m_nantiAlias3DLine = m_nantiAlias3DLine;

    m_bitmapFontPage.m_nbitmapFontPlatformType = m_nbitmapFontPlatformType;

    m_calcPriorityPage.m_ncalcPriority = m_ncalcPriority;

    m_countrySettingsPage.m_europeanReals = m_europeanReals;

    m_TemplateDirectoryPage.m_szTemplateDirectoryName = m_szTemplateDirectoryName;

    return CHelpPropertySheet::OnInitDialog();
}





DWORD CSettingsPropSheet::m_dwHelpMap[] =
    {
        IDC_CHECK_SMALL,                HIDC_CHECK_SMALL,
        IDC_CHECK_SINGLE_LINE,  HIDC_CHECK_SINGLE_LINE,
        IDC_RADIO1,                             HIDC_RADIO,
        IDC_RADIO2,                             HIDC_RADIO,
        IDC_RADIO3,                             HIDC_RADIO,
        IDC_COMBO_PFD,                  HIDC_COMBO_PFD,
        IDC_EDIT_VERSION,               HIDC_EDIT_VERSION,
        IDC_EDIT_VENDOR,                HIDC_EDIT_VENDOR,
        0,                                              0
    };

DWORD CSettingsPropSheet::GetHelpMap()
{
    return DWORD(LPVOID(m_dwHelpMap));
}

CString CSettingsPropSheet::GetHelpName()
{
    CString sContextFile = "Settings.txt";
    return sContextFile;
}

