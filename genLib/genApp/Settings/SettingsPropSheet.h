///////////////////////////////////////////////////////////////////////////////////
//
// SettingsPropSheet.h
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

#if !defined(AFX_SETTINGSPROPSHEET_H__B6262584_A506_11D4_BD0F_00104B4C74FD__INCLUDED_)
#define AFX_SETTINGSPROPSHEET_H__B6262584_A506_11D4_BD0F_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SettingsPropSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingsPropSheet

#include "PlotSettingsPage.h"
#include "TreeViewSettingsPage.h"
#include "OpenGLSettingsPage.h"
#include "OpenGLSettingsPage.h"
#include "OpenGLPropertiesPage.h"
#include "BitmapFontSettingsPage.h"
#include "CalcPrioritySettingsPage.h"
#include "CountrySettingsPage.h"
#include "TemplateDirectorySettingsPage.h"

#include <genMFC/HelpPropertySheet.h>

class CSettingsPropSheet : public CHelpPropertySheet
{
    DECLARE_DYNAMIC(CSettingsPropSheet)

    // Construction
    public:
    CSettingsPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CSettingsPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

    // Attributes
public:
    CPlotSettingsPage               m_plotPage;
    CTreeViewSettingsPage           m_treeViewPage;
    COpenGLSettingsPage             m_openGLPage;
    COpenGLPropertiesPage           m_openGLPropertiesPage;
    CBitmapFontSettingsPage         m_bitmapFontPage;
    CCalcPrioritySettingsPage       m_calcPriorityPage;
    CCountrySettingsPage            m_countrySettingsPage;
    CTemplateDirectorySettingsPage  m_TemplateDirectoryPage;

    int                             m_nCheckSmallFont;
    BYTE                            m_nAlpha;

    BOOL                            m_nSingleLine;
    int                             m_nSingleLineType;
    BOOL                            m_nShowUnused;
    BOOL                            m_nShowConnections;

    int                             m_nPFD;
    int                             m_pfdIndex;

    int                             m_nbitmapFontPlatformType;

    int                             m_ncalcPriority;
    BOOL                            m_nantiAlias2DLine;
    BOOL                            m_nantiAlias3DLine;

    BOOL                            m_europeanReals;
    CString                         m_szTemplateDirectoryName;


    // Operations
public:

    void    ApplyChanges(bool bDialog = true);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSettingsPropSheet)
public:
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CSettingsPropSheet();

    // Generated message map functions
protected:
    //{{AFX_MSG(CSettingsPropSheet)
    afx_msg void OnApplyNow();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Help support
    protected:
    virtual DWORD   GetHelpMap();
    virtual CString GetHelpName();
private:
    static DWORD    m_dwHelpMap[];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSPROPSHEET_H__B6262584_A506_11D4_BD0F_00104B4C74FD__INCLUDED_)

