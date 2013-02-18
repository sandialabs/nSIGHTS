///////////////////////////////////////////////////////////////////////////////////
//
// FileCtrl.cpp
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
//              Text field and button for browse selection
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>

#include "FileCtrl.h"
#include <genUIExt/FileDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


FileTextCtrl::FileTextCtrl() :
    m_pFile(NULL)
{
}

void FileTextCtrl::Set(CWnd* pParent, int nIDEdit, char* inFile)
{
    m_pFile = inFile;

    CTextCtrl::Set(pParent, nIDEdit, inFile, stdFileStrLen);

}


void FileTextCtrl::UpdateText()
{
    FullTrim(m_pFile);
    SetWindowText(m_pFile);
}

void FileTextCtrl::DoBrowseChange()
{
    UpdateText();
}

void FileTextCtrl::DoCtrlReset()
{
    UpdateText();
}


/////////////////////////////////////////////////////////////////////////////
// CFileCtrl

CFileCtrl::CFileCtrl()
{
}

CFileCtrl::~CFileCtrl()
{
}

void CFileCtrl::Set(CWnd* pParent, int nIDText, int nIDButton, char* pFile,
                    const char* fileFilter, const char* fileDialogTitle, const char* defExt, bool openFile)
{
    m_uFileTextCtrl.Set(pParent, nIDText,  pFile);
    m_uFileTextCtrl.AddCallback(this, 1);
    m_uBrowseButton.Set(pParent, nIDButton);
    m_uBrowseButton.AddCallback(this);
    doFileOpen = openFile;

    m_pFileName = pFile;
    m_pFileFilter = fileFilter;
    m_pFileDialogTitle = fileDialogTitle;
    m_pDefExt = defExt;
}


BOOL CFileCtrl::EnableWindowEx(BOOL bEnable)
{
    m_uFileTextCtrl.EnableWindow(bEnable);
    BOOL bRc = m_uBrowseButton.EnableWindow(bEnable);
    return bRc;
}

void CFileCtrl::DoCtrlUpdate()
{
    if (DoBrowseChange())
        {
            m_uFileTextCtrl.DoBrowseChange();
            DoCallbacks();
        }
}

void CFileCtrl::DoCtrlUpdate(int reason)
{
    DoCallbacks();
}



BOOL CFileCtrl::DoBrowseChange()
{
    CString sFileName;

    CFileStatus status;
    if (CFile::GetStatus(m_pFileName, status))
        {
            sFileName = m_pFileName;
        }

    NTCompatibleFileDialog dlg(doFileOpen, m_pDefExt, sFileName, OFN_HIDEREADONLY, m_pFileFilter);
    dlg.m_ofn.lpstrTitle = m_pFileDialogTitle;

    if (dlg.DoModal() == IDOK) {
        CopyString(m_pFileName, dlg.GetPathName(), stdFileStrLen);
        return TRUE;
    }
    return FALSE;
}

void CFileCtrl::DoCtrlReset()
{
    m_uFileTextCtrl.DoCtrlReset();
}

