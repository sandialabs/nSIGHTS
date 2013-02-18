///////////////////////////////////////////////////////////////////////////////////
//
// FileDialog.h
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

#if !defined(NTCOMPATIBLEFILEDIALOG_H)
#define NTCOMPATIBLEFILEDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif

// kluge for NT compatibility under .Net

class NTCompatibleFileDialog : public CFileDialog
{
    public:
        NTCompatibleFileDialog(BOOL bOpenFileDialog,
                               LPCTSTR lpszDefExt = NULL,
                               LPCTSTR lpszFileName = NULL,
                               DWORD   dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                               LPCTSTR lpszFilter = NULL,
                               CWnd* pParentWnd = NULL);

    private:
        static DWORD GetOSStructureSize();
};
//obtained from www.codeguru.com
//written by Rommel Santor, Dec 4, 2003
//modified from CFolderDialog to make it NT compatible

static const TCHAR *FOLDER_FILTER = _T(" | ||");

class NTCompatibleFolderDialog : public NTCompatibleFileDialog
{
    DECLARE_DYNAMIC(NTCompatibleFolderDialog)

    public:

            CString     m_pPath;
            BOOL        m_bModeSelect;

            static WNDPROC m_wndProc;

    public:
            NTCompatibleFolderDialog(BOOL bSelectDlg=TRUE, LPCTSTR lpszPath=NULL, LPCTSTR lpszFileFilter=FOLDER_FILTER, DWORD dwFlags=OFN_PATHMUSTEXIST | OFN_HIDEREADONLY);

            CString     GetSelPath()      { return m_pPath; }
            CString     GetParentDir()    { return m_pPath.Left(m_pPath.ReverseFind(_T('\\'))); }
            BOOL        DidSelectFolder() { return GetFileAttributes(m_pPath) & FILE_ATTRIBUTE_DIRECTORY; }

            virtual void    OnInitDone( );
            virtual void    OnFolderChange();
            virtual void    OnFileNameChange();

            void            SetEdt1(CString inStr);
            void            GetEdt1(CString &inStr);

    protected:
    //{{AFX_MSG(CFolderDialog)
        virtual BOOL OnInitDialog();
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION
    // Microsoft Developer Studio will insert additional declarations immediately before the previous line.
//}}AFX_INSERT_LOCATION

#endif // NTCOMPATIBLEFILEDIALOG_H

