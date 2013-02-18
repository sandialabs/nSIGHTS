///////////////////////////////////////////////////////////////////////////////////
//
// FolderCtrl.cpp
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
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>

#include <genUIExt/FileDialog.h>

#include "FolderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


FolderTextCtrl::FolderTextCtrl() :
    m_pFolder(NULL)
{
}

void FolderTextCtrl::Set(CWnd* pParent, int nIDEdit, char* inFolder)
{
    m_pFolder = inFolder;

    CTextCtrl::Set(pParent, nIDEdit, inFolder, stdFileStrLen);

}


void FolderTextCtrl::UpdateText()
{
    FullTrim(m_pFolder);
    SetWindowText(m_pFolder);
}

void FolderTextCtrl::DoBrowseChange()
{
    UpdateText();
}

void FolderTextCtrl::DoCtrlReset()
{
    UpdateText();
}


/////////////////////////////////////////////////////////////////////////////
// CFolderCtrl

CFolderCtrl::CFolderCtrl()
{
}

CFolderCtrl::~CFolderCtrl()
{
}

void CFolderCtrl::Set(CWnd* pParent, int nIDText, int nIDButton, char* pFolder,
                      const char* folderDialogTitle, bool openFile)
{
    m_uFolderTextCtrl.Set(pParent, nIDText,  pFolder);
    m_uFolderTextCtrl.AddCallback(this, 1);
    m_uBrowseButton.Set(pParent, nIDButton);
    m_uBrowseButton.AddCallback(this);
    doFileOpen = openFile;

    m_pFolderName = pFolder;
    m_pFolderDialogTitle = folderDialogTitle;

}


BOOL CFolderCtrl::EnableWindowEx(BOOL bEnable)
{
    m_uFolderTextCtrl.EnableWindow(bEnable);
    BOOL bRc = m_uBrowseButton.EnableWindow(bEnable);
    return bRc;
}

void CFolderCtrl::DoCtrlUpdate()
{
    if (DoBrowseChange())
        {
            m_uFolderTextCtrl.DoBrowseChange();
            DoCallbacks();
        }
}

void CFolderCtrl::DoCtrlUpdate(int reason)
{
    DoCallbacks();
}



BOOL CFolderCtrl::DoBrowseChange()
{
    CString sFolderName;

    //CFileStatus status;
    //if (CFile::GetStatus(m_pFolderName, status))
    //{
    sFolderName = m_pFolderName;
    //}

    NTCompatibleFolderDialog dlg;
    dlg.m_pOFN->lpstrTitle = m_pFolderDialogTitle;

    if (dlg.DoModal() == IDOK) {
        CopyString(m_pFolderName, dlg.m_pPath, stdFileStrLen);
        return TRUE;
    }
    return FALSE;
}

void CFolderCtrl::DoCtrlReset()
{
    m_uFolderTextCtrl.DoCtrlReset();
}

