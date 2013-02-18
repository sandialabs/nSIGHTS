///////////////////////////////////////////////////////////////////////////////////
//
// PS_PSOutputSetup.cpp
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
//      Platform specific implementation of genPlotClass codes
//  adds Windows and OpenGL specific code.  Includes
//  operating system independent implementation of
//  2D and 3D OpenGL functionality (platform specific
//  in this case refers to the OpenGL renderer).
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClass/U_Str.h>

#include <genPlotClassPS/PS_PSOutputSetup.h>
#include <genUIExt/FileDialog.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PS_PSOutputSetup dialog


PS_PSOutputSetup::PS_PSOutputSetup(CWnd* pParent /*=NULL*/)
    : CHelpDialog(PS_PSOutputSetup::IDD, pParent)
{
    //{{AFX_DATA_INIT(PS_PSOutputSetup)
    m_fBottom = 0.0f;
    m_szFullPathName = _T("");
    m_fGammaCorrect = 0.0f;
    m_fHeight = 0.0f;
    m_fLineWidthMult = 0.0f;
    m_fLeftMargin = 0.0f;
    m_fRight = 0.0f;
    m_fTextMult = 0.0f;
    m_fTopMargin = 0.0f;
    m_fWidth = 0.0f;
    m_fZBufferMult = 0.0f;
    m_iArialAsHelvetica = BST_CHECKED;
    m_inextPSIncrement = 0;
    //}}AFX_DATA_INIT
}


void PS_PSOutputSetup::DoDataExchange(CDataExchange* pDX)
{
    CHelpDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(PS_PSOutputSetup)
    DDX_Text(pDX, IDC_BOTTOM, m_fBottom);
    DDX_Text(pDX, IDC_FULLPATHNAME, m_szFullPathName);
    DDX_Text(pDX, IDC_GAMMA_CORRECT, m_fGammaCorrect);
    DDX_Text(pDX, IDC_HEIGHT, m_fHeight);
    DDX_Text(pDX, IDC_LINE_WIDTH_MULT, m_fLineWidthMult);
    DDX_Text(pDX, IDC_LEFT_MARGIN, m_fLeftMargin);
    DDX_Text(pDX, IDC_RIGHT, m_fRight);
    DDX_Text(pDX, IDC_TEXT_MULT, m_fTextMult);
    DDX_Text(pDX, IDC_TOP_MARGIN, m_fTopMargin);
    DDX_Text(pDX, IDC_WIDTH, m_fWidth);
    DDX_Text(pDX, IDC_Z_BUFFER_MULT, m_fZBufferMult);
    DDX_Text(pDX, IDC_AUTO_INCREMENT, m_inextPSIncrement);
    DDX_Radio(pDX, IDC_PS, m_iFileFormat);
    DDX_Radio(pDX, IDC_LANDSCAPE, m_iOrientation);
    DDX_Check(pDX, IDC_ARIAL_AS_HELVETCA, m_iArialAsHelvetica);
    //}}AFX_DATA_MAP
}

void PS_PSOutputSetup::SetValues(PC_PSOutData& values)
{
    m_fBottom = values.bottomMargin;
    m_szFullPathName = values.outputFile;
    m_fGammaCorrect = values.gammaCorrection;
    m_fHeight = values.pageHeight;
    m_fLineWidthMult = values.lineWidthMultiplier;
    m_fLeftMargin = values.leftMargin;
    m_fRight = values.rightMargin;
    m_fTextMult = values.textLengthMultiplier;
    m_fTopMargin = values.topMargin;
    m_fWidth = values.pageWidth;
    m_fZBufferMult = values.zBufferMultiplier;
    m_iFileFormat = values.outputFormat;
    m_iOrientation = values.outputIsPortrait;
    if (values.arialIsHelvetica)
        m_iArialAsHelvetica = BST_CHECKED;
    else
        m_iArialAsHelvetica = BST_UNCHECKED;
    m_inextPSIncrement = values.nextPSIncrement;

}

void PS_PSOutputSetup::GetValues(PC_PSOutData& values)
{
    values.bottomMargin = m_fBottom;
    CopyString(values.outputFile, m_szFullPathName, stdFileStrLen);
    values.gammaCorrection = m_fGammaCorrect;
    values.pageHeight = m_fHeight;
    values.lineWidthMultiplier = m_fLineWidthMult;
    values.leftMargin = m_fLeftMargin;
    values.rightMargin = m_fRight;
    values.textLengthMultiplier = m_fTextMult;
    values.topMargin = m_fTopMargin;
    values.pageWidth = m_fWidth;
    values.zBufferMultiplier = m_fZBufferMult;
    values.outputFormat = PC_PSOutData::OutputFormat(m_iFileFormat);
    values.outputIsPortrait = m_iOrientation ? true : false;
    values.arialIsHelvetica = (m_iArialAsHelvetica == BST_CHECKED);
    values.nextPSIncrement = m_inextPSIncrement;
}

BEGIN_MESSAGE_MAP(PS_PSOutputSetup, CHelpDialog)
    //{{AFX_MSG_MAP(PS_PSOutputSetup)
        ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
        ON_BN_CLICKED(IDC_ACCEPT, OnAccept)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// PS_PSOutputSetup message handlers

void PS_PSOutputSetup::OnBrowse()
{
    UpdateData(true);

    NTCompatibleFileDialog dlg(false, NULL, m_szFullPathName, 0);

    if (m_iFileFormat == PC_PSOutData::of_PS)
        dlg.m_ofn.lpstrFilter = "Generic Postscript (*.ps)\0*.ps\0\0";
    else
        dlg.m_ofn.lpstrFilter = "Encapsulated Postscript (*.eps)\0*.eps\0\0";

    INT_PTR nResults = dlg.DoModal();

    if (nResults == IDOK)
    {
        m_szFullPathName = dlg.GetPathName();
        UpdateData(false);
    }
}

void PS_PSOutputSetup::OnAccept()
{
    UpdateData(true);
    EndDialog(IDYES);
}


#include <HTMLHelp/ToolTips.h>

DWORD PS_PSOutputSetup::m_dwHelpMap[] =
{
    IDC_PS,             HIDC_PSSETUP_PS,
    IDC_EPS,            HIDC_PSSETUP_EPS,
    IDC_EPS_WIN,        HIDC_PSSETUP_EPS_WIN,
    IDC_LANDSCAPE,      HIDC_PSSETUP_LANDSCAPE,
    IDC_PORTRAIT,       HIDC_PSSETUP_PORTRAIT,
    IDC_WIDTH,          HIDC_PSSETUP_WIDTH,
    IDC_LEFT_MARGIN,    HIDC_PSSETUP_LEFT_MARGIN,
    IDC_RIGHT,          HIDC_PSSETUP_RIGHT,
    IDC_HEIGHT,         HIDC_PSSETUP_HEIGHT,
    IDC_TOP_MARGIN,     HIDC_PSSETUP_TOP_MARGIN,
    IDC_BOTTOM,         HIDC_PSSETUP_BOTTOM,
    IDC_FULLPATHNAME,   HIDC_PSSETUP_FULLPATHNAME,
    IDC_BROWSE,         HIDC_PSSETUP_BROWSE,
    IDC_LINE_WIDTH_MULT,HIDC_PSSETUP_LINE_WIDTH_MULT,
    IDC_GAMMA_CORRECT,  HIDC_PSSETUP_GAMMA_CORRECT,
    IDC_Z_BUFFER_MULT,  HIDC_PSSETUP_Z_BUFFER_MULT,
    IDC_TEXT_MULT,      HIDC_PSSETUP_TEXT_MULT,
    0,                  0
};

DWORD PS_PSOutputSetup::GetHelpMap()
{
    return DWORD(LPVOID(m_dwHelpMap));
}

CString PS_PSOutputSetup::GetHelpName()
{
    CString sContextFile = "PSOutputSetup.txt";
    return sContextFile;
}

