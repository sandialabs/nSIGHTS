///////////////////////////////////////////////////////////////////////////////////
//
// PS_TGAOutputSetup.cpp
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

#include <genUIExt/FileDialog.h>

#include <genPlotClassPS/PS_TGAOutputSetup.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PS_TGAOutputSetup dialog

PS_TGAOutputSetup::PS_TGAOutputSetup(CWnd* pParent /*=NULL*/)
    : CHelpDialog(PS_TGAOutputSetup::IDD, pParent)
{
    //{{AFX_DATA_INIT(PS_TGAOutputSetup)
    m_iDumpCount = 0;
    m_szFullPathName = _T("");
    m_iNextIncrement = 0;
    m_iQualityJPEG = 100;
    //}}AFX_DATA_INIT
}

void PS_TGAOutputSetup::DoDataExchange(CDataExchange* pDX)
{
    CHelpDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(PS_TGAOutputSetup)
    DDX_Radio(pDX, IDC_TGA, m_iFormatGroup);
    DDX_Radio(pDX, IDC_AUTO, m_iMethodGroup);
    DDX_Text(pDX, IDC_DUMP_COUNT, m_iDumpCount);
    DDX_Text(pDX, IDC_FULLPATHNAME, m_szFullPathName);
    DDX_Text(pDX, IDC_NEXT_INCREMENT, m_iNextIncrement);
    DDX_Text(pDX, IDC_JPEG_QUALITY, m_iQualityJPEG);
    DDV_MinMaxInt(pDX, m_iQualityJPEG, 1, 100);
    //}}AFX_DATA_MAP
}

void PS_TGAOutputSetup::SetValues(PC_DumpTGA& values)
{
    m_szFullPathName = values.tgaRootFile;
    m_iFormatGroup = values.dumpFormat;
    m_iMethodGroup = values.dumpMethod;
    m_iNextIncrement = values.nextIncrement;
    m_iDumpCount = values.dumpFrameCount;
    m_iQualityJPEG = values.qualityJPEG;
}

void PS_TGAOutputSetup::GetValues(PC_DumpTGA& values)
{
    CopyString(values.tgaRootFile, m_szFullPathName, stdFileStrLen);
    values.dumpFormat = PC_DumpTGA::DumpFormat(m_iFormatGroup);
    values.dumpMethod = PC_DumpTGA::TGADumpMethod(m_iMethodGroup);
    values.nextIncrement = m_iNextIncrement;
    values.dumpFrameCount = m_iDumpCount;
    values.qualityJPEG = m_iQualityJPEG;
}

BEGIN_MESSAGE_MAP(PS_TGAOutputSetup, CHelpDialog)
    //{{AFX_MSG_MAP(PS_TGAOutputSetup)
    ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
    ON_BN_CLICKED(IDC_TGA,  OnTGAFormat)
    ON_BN_CLICKED(IDC_JPEG, OnJPEGFormat)
    ON_BN_CLICKED(IDC_BMP, OnBMPFormat)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PS_TGAOutputSetup message handlers

void PS_TGAOutputSetup::OnBrowse()
{
    UpdateData(true);

    NTCompatibleFileDialog dlg(false, NULL, m_szFullPathName);

    if (m_iFormatGroup == PC_DumpTGA::dumpTGA)
        dlg.m_ofn.lpstrFilter = "Targa Bitmap (*.TGA)\0*.TGA\0\0";
    else if (m_iFormatGroup == PC_DumpTGA::dumpJPEG)
        dlg.m_ofn.lpstrFilter = "Jpeg Bitmap (*.JPG)\0*.JPG\0\0";
    else
        dlg.m_ofn.lpstrFilter = "Windows Bitmap (*.BMP)\0*.BMP\0\0";

    INT_PTR nResults = dlg.DoModal();
    if (nResults == IDOK)
    {
        m_szFullPathName = dlg.GetPathName();
        UpdateData(false);
    }
}

BOOL PS_TGAOutputSetup::OnInitDialog()
{
    CHelpDialog::OnInitDialog();

    SetupDialog();

    return TRUE;
}

void PS_TGAOutputSetup::SetupDialog()
{
    GetDlgItem(IDC_JPEG_QUALITY)->EnableWindow(m_iFormatGroup == PC_DumpTGA::dumpJPEG);
}

void PS_TGAOutputSetup::OnTGAFormat()
{
    UpdateData(true);
    SetupDialog();
}

void PS_TGAOutputSetup::OnJPEGFormat()
{
    UpdateData(true);
    SetupDialog();
}

void PS_TGAOutputSetup::OnBMPFormat()
{
    UpdateData(true);
    SetupDialog();
}

#include <HTMLHelp/ToolTips.h>

DWORD PS_TGAOutputSetup::m_dwHelpMap[] =
{
    IDC_TGA,            HIDC_TGASETUP_TGA,
    IDC_JPEG,           HIDC_TGASETUP_JPEG,
    IDC_BMP,            HIDC_TGASETUP_TGA,  // kluge - not used anyway ?
    IDC_AUTO,           HIDC_TGASETUP_AUTO,
    IDC_SEMI_AUTO,      HIDC_TGASETUP_SEMI_AUTO,
    IDC_MANUAL,         HIDC_TGASETUP_MANUAL,
    IDC_FULLPATHNAME,   HIDC_TGASETUP_FULLPATHNAME,
    IDC_BROWSE,         HIDC_TGASETUP_BROWSE_PATH,
    IDC_NEXT_INCREMENT, HIDC_TGASETUP_NEXT_INCREMENT,
    IDC_DUMP_COUNT,     HIDC_TGASETUP_DUMP_COUNT,
    IDC_JPEG_QUALITY,   HIDC_TGASETUP_JPEG_QUALITY,
    0,                  0
};

DWORD PS_TGAOutputSetup::GetHelpMap()
{
    return DWORD(LPVOID(m_dwHelpMap));
}

CString PS_TGAOutputSetup::GetHelpName()
{
    CString sContextFile = "TGAOutputSetup.txt";
    return sContextFile;
}

