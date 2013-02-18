///////////////////////////////////////////////////////////////////////////////////
//
// nPreDoc.cpp
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
//      implementation of the CNPreDoc class (MFC required).
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nPre.h"

#include "nPreDoc.h"
#include "nPreView.h"

#include "MainFrm.h"

#include <App/nPreFile.h>
#include <App/nPreRoot.h>
#include <App/nPreUtil.h>

#include <genApp/ProjectUtil.h>
#include <genApp/C_AppMenu.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNPreDoc

IMPLEMENT_DYNCREATE(CNPreDoc, CDocument)

BEGIN_MESSAGE_MAP(CNPreDoc, CDocument)
    //{{AFX_MSG_MAP(CNPreDoc)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNPreDoc construction/destruction

CNPreDoc::CNPreDoc()
{
}

CNPreDoc::~CNPreDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNPreDoc serialization

void CNPreDoc::Serialize(CArchive& ar)
{
}

/////////////////////////////////////////////////////////////////////////////
// CNPreDoc diagnostics

#ifdef _DEBUG
void CNPreDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CNPreDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNPreDoc commands
void CNPreDoc::ResetInterface(bool bRestoreContext)
{
    CMainFrame* pMain = static_cast<CMainFrame*>(AfxGetMainWnd());
    if (pMain != NULL) {
        pMain->ResetInterface(this, bRestoreContext);
    }
}

BOOL CNPreDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    nPreFileOperations::NewFile();

    ResetInterface(false);
    projectUtilities::SetModifiedFlag(false);

    return TRUE;
}

BOOL CNPreDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    if (!CDocument::OnOpenDocument(lpszPathName))
        return FALSE;

    bool ok = nPreFileOperations::OpenFile(lpszPathName);

    MenuRootC::currTreeObj = 0;

    ResetInterface();
    projectUtilities::SetModifiedFlag(false);
    AppMenuC::SetMenuBarSensitive();

    return ok;
}

BOOL CNPreDoc::SaveModified()
{
    if (projectUtilities::IsModified()) {
        SetModifiedFlag(TRUE);
    }

    return CDocument::SaveModified();
}

BOOL CNPreDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    nPreFileOperations::SaveFile(lpszPathName, false);

    projectUtilities::SetModifiedFlag(false);

    return TRUE;
}

void CNPreDoc::OnCloseDocument()
{
    MenuRootC::DestroyAll();
    CDocument::OnCloseDocument();
}

void CNPreDoc::OnChangedViewList()
{
    // The ancestor's implementation would destroy this document
    // we don't call it
}

BOOL CNPreDoc::OnOpenTemplateDocument(LPCTSTR lpszPathName)
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    bool ok = nPreFileOperations::OpenFile(lpszPathName);

    projectUtilities::SetModifiedFlag(false);
    UpdateAllViews(NULL);
    LPCTSTR lpUntitledName = "Template";
    SetTitle(lpUntitledName);

    return ok;
}

CNPreDoc * CNPreDoc::GetDoc()
{
    CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
    return (CNPreDoc *) pFrame->GetActiveDocument();

}

