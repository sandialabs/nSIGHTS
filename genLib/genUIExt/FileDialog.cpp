///////////////////////////////////////////////////////////////////////////////////
//
// FileDialog.cpp
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
//      Windows specific code for MFC user-interface extension or enhancement classes.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include <AfxPriv.h>

#include <genApp/ProjectUtil.h>

#include "FileDialog.h"

//for folder dialog
#include <DLGS.H>
#include <WINUSER.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


NTCompatibleFileDialog::NTCompatibleFileDialog(BOOL bOpenFileDialog,
                               LPCTSTR lpszDefExt,
                               LPCTSTR lpszFileName,
                               DWORD   dwFlags,
                               LPCTSTR lpszFilter,
                               CWnd* pParentWnd)
   : CFileDialog(bOpenFileDialog, lpszDefExt,lpszFileName,
                dwFlags, lpszFilter, pParentWnd,
                GetOSStructureSize())
{
}

DWORD NTCompatibleFileDialog::GetOSStructureSize()
{
    DWORD osSize = sizeof (OPENFILENAME);
    if (projectUtilities::IsWindowsNT())
        osSize = OPENFILENAME_SIZE_VERSION_400;
    return osSize;
}

/////////////////////////////////////////////////////////////////////////////
// CFolderDialog
IMPLEMENT_DYNAMIC(NTCompatibleFolderDialog, NTCompatibleFileDialog)

WNDPROC NTCompatibleFolderDialog::m_wndProc = NULL;

NTCompatibleFolderDialog::NTCompatibleFolderDialog(BOOL bSelectDlg, LPCTSTR lpszPath, LPCTSTR lpszFileFilter, DWORD dwFlags)
: m_bModeSelect(bSelectDlg),
  NTCompatibleFileDialog(TRUE, NULL, NULL, dwFlags, lpszFileFilter)
{
    CString pPath;

    if (lpszPath == NULL) {
        TCHAR tmpPath[MAX_PATH + 1];
        GetCurrentDirectory(MAX_PATH, tmpPath);
        pPath = tmpPath;
    }

    else
        pPath = lpszPath;

    if (GetFileAttributes(pPath) & FILE_ATTRIBUTE_DIRECTORY)
        m_pPath = pPath;
    else
        m_pPath = pPath.Left(pPath.ReverseFind(_T('\\')));

        m_ofn.lpstrInitialDir = m_pPath;

}


BEGIN_MESSAGE_MAP(NTCompatibleFolderDialog, NTCompatibleFileDialog)
    //{{AFX_MSG_MAP(NTCompatibleFolderDialog)
        ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Function name    : WindowProcNew
// Description      : Call this function when user navigate into CFileDialog.
// Return type      : LRESULT
// Argument         : HWND hwnd
// Argument         : UINT message
// Argument         : WPARAM wParam
// Argument         : LPARAM lParam
LRESULT CALLBACK WindowProcNew(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message ==  WM_COMMAND)
    {
        if (HIWORD(wParam) == BN_CLICKED)
        {
            if (LOWORD(wParam) == IDOK)
            {
                if (NTCompatibleFileDialog* pDlg = (NTCompatibleFileDialog*)CWnd::FromHandle(hwnd))
                {
                    pDlg->EndDialog(IDOK);
                    return NULL;
                }
            }
        }
    }
    return CallWindowProc(NTCompatibleFolderDialog::m_wndProc, hwnd, message, wParam, lParam);
}

void NTCompatibleFolderDialog::OnFolderChange()
{
    TCHAR path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, path);
    m_pPath = path;

    CWnd* pParent = GetParent()->GetDlgItem(lst2);

    // get the list control
    CListCtrl *pList = (CListCtrl*)pParent->GetDlgItem(1);

    // currently selected item
    int pos = pList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

    if (pos != -1)
    {
        // create the full path...
        CString selection = pList->GetItemText(pos, 0);

        CString testStr = path;
        if (testStr.GetAt(testStr.GetLength() - 1) != _T('\\'))
            testStr = testStr + _T("\\");
        testStr = testStr + selection;

        m_pPath = testStr;
    }
    if (projectUtilities::IsWindowsNT())
        GetParent()->GetDlgItem(edt1)->SetWindowText(m_pPath);
    else
        GetParent()->GetDlgItem(cmb13)->SetWindowText(m_pPath);
}
//also by Rommel
void NTCompatibleFolderDialog::OnFileNameChange()
{
    OnFolderChange();// ROMMEL - set selection by clicking on any item
}


// Function name    : CFolderDialog::OnInitDone
// Description      : For update the wiew of CFileDialog
// Return type      : void
void NTCompatibleFolderDialog::OnInitDone()
{
    CWnd* pFD = GetParent();

    CRect rectCancel;
    pFD->GetDlgItem(IDCANCEL)->GetWindowRect(rectCancel);
    pFD->ScreenToClient(rectCancel);

    CString titleStr;
    if (!m_bModeSelect)
        titleStr = _T("Open");
    else
        titleStr = _T("Select");

    SetControlText(IDOK, titleStr);

    if (!m_bModeSelect)
    {
        if (m_ofn.Flags & OFN_ALLOWMULTISELECT)
            titleStr = _T("Open Folders");
        else
            titleStr = _T("Open Folder");
    }
    else
    {
        if (m_ofn.Flags & OFN_ALLOWMULTISELECT)
            titleStr = _T("Select Folders");
        else
            titleStr = _T("Select Folder");
    }

    pFD->SetWindowText(titleStr);

  // removed this line as it screwed the label position - ROMMEL
    //pFD->GetDlgItem(stc3)->SetWindowPos(0,0,0,rectList2.Width()+8,rectList2.Height(), SWP_NOMOVE | SWP_NOZORDER);

    titleStr = _T("Folder &name:");

    pFD->GetDlgItem(stc3)->SetWindowText(titleStr);
    if (projectUtilities::IsWindowsNT())
        pFD->GetDlgItem(edt1)->SetWindowText(m_pPath);
    else
        pFD->GetDlgItem(cmb13)->SetWindowText(m_pPath);

    //hides the file filter controls
    HideControl(cmb1);
    HideControl(stc2);

    m_wndProc = (WNDPROC)SetWindowLongPtr(pFD->m_hWnd, GWLP_WNDPROC, (LONG_PTR)WindowProcNew);
}

void NTCompatibleFolderDialog::SetEdt1(CString inStr)
{
    if (projectUtilities::IsWindowsNT())
        GetParent()->GetDlgItem(edt1)->SetWindowText(inStr);
    else
        GetParent()->GetDlgItem(cmb13)->SetWindowText(inStr);
    m_pPath = inStr;
}

void NTCompatibleFolderDialog::GetEdt1(CString &inStr)
{
    if (projectUtilities::IsWindowsNT())
        GetParent()->GetDlgItem(edt1)->GetWindowText(inStr);
    else
        GetParent()->GetDlgItem(cmb13)->GetWindowText(inStr);
}

BOOL NTCompatibleFolderDialog::OnInitDialog()
{
    NTCompatibleFileDialog::OnInitDialog();

    if (projectUtilities::IsWindowsNT())
        GetParent()->GetDlgItem(edt1)->SetWindowText(m_pPath);
    else
        GetParent()->GetDlgItem(cmb13)->SetWindowText(m_pPath);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

int NTCompatibleFolderDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (NTCompatibleFileDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    //GetParent()->GetDlgItem(edt1)->SetWindowText(m_pPath);

    return 0;
}

