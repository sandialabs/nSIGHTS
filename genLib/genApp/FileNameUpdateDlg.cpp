///////////////////////////////////////////////////////////////////////////////////
//
// FileNameUpdateDlg.cpp
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
// DESCRIPTION:  Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genApp/G_FileNames.h>

#include "FileNameUpdateDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileNameUpdateDlg dialog

// should be in registry ???

char CFileNameUpdateDlg::currentPathName[stdFileStrLen];
char CFileNameUpdateDlg::newPathName[stdFileStrLen];
bool CFileNameUpdateDlg::executeAfter = true;
bool CFileNameUpdateDlg::firstCall = true;

CFileNameUpdateDlg::CFileNameUpdateDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CFileNameUpdateDlg::IDD, pParent)
{
    if (firstCall)
        {
            SetToNull(currentPathName);
            SetToNull(newPathName);
            firstCall = false;
        }
}


BEGIN_MESSAGE_MAP(CFileNameUpdateDlg, CDialog)
//{{AFX_MSG_MAP(CFileNameUpdateDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CFileNameUpdateDlg::OnInitDialog()
{
    FileNameArray::GetCurrentPaths(currPaths);
    if (currPaths.IsEmpty())
        m_ucurrentPaths.AddBasicEntry("<no file names registered>");
    else
        m_ucurrentPaths.AddBasicEntry(currPaths);

    currPathSel = 0;
    m_ucurrentPaths.Set(this, IDC_COMBO_CURRENTPATHS,  &currPathSel);
    m_ucurrentPaths.AddCallback(this, 1);

    if (currPaths.IsNotEmpty() && IsNull(currentPathName))
        CopyString(currentPathName, currPaths[0], stdFileStrLen);

    m_ucurrentPathName.Set(this, IDC_CURRENTUPDATEPATH, currentPathName, stdFileStrLen, false);
    m_ucurrentPathName.AddCallback(this);

    m_unewPathName.Set(this, IDC_NEWUPDATEPATH, IDC_BROWSE, newPathName,
                       "Output Files Directory", false);
    m_unewPathName.AddCallback(this);
    m_unewPathName.m_uFolderTextCtrl.AddCallback(this);

    m_executeAfter.Set(this, IDC_CHECK_EXECUTE, executeAfter);

    CDialog::OnInitDialog();

    CheckRestrictions();

    return TRUE;
}


void CFileNameUpdateDlg::DoCtrlUpdate(int reason)
{
    if (currPaths.IsNotEmpty())
        {
            CopyString(currentPathName, currPaths[currPathSel], stdFileStrLen);
            m_ucurrentPathName.DoCtrlReset();
        }

    CheckRestrictions();
}

void CFileNameUpdateDlg::DoCtrlUpdate()
{
    CheckRestrictions();
}


void CFileNameUpdateDlg::CheckRestrictions()
{
    bool pathsOK = currPaths.IsNotEmpty();
    m_ucurrentPaths.EnableWindow(pathsOK);
    m_ucurrentPathName.EnableWindow(pathsOK);
    m_executeAfter.EnableWindow(pathsOK);

    bool currPathOK = StringLength(currentPathName) > 0;
    bool newPathOK = StringLength(newPathName) > 0;

    GetDlgItem(IDOK)->EnableWindow(pathsOK && currPathOK && newPathOK);
}

void CFileNameUpdateDlg::OnOK()
{
    if (FileNameArray::UpdatePaths(currentPathName, newPathName, executeAfter))
        CDialog::OnOK();
}

void CFileNameUpdateDlg::OnCancel()
{
    GenAppClearMsgs();
    CDialog::OnCancel();
}

