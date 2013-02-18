///////////////////////////////////////////////////////////////////////////////////
//
// ProjectDoc.cpp
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
// DESCRIPTION:  fills MFC requirement for CDocument derived class. Processes file
//               new/save/open messages
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "resource.h"

#include <genApp/ProjectUtil.h>
#include <genApp/GenAppMessages.h>

#include "App/nPostRoot.h"
#include "App/nPostFile.h"

#include "ProjectDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CProjectDoc, CDocument)

BEGIN_MESSAGE_MAP(CProjectDoc, CDocument)
//{{AFX_MSG_MAP(CProjectDoc)
ON_COMMAND(ID_FILE_SAVE, OnFileSave)
ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CProjectDoc::CProjectDoc()
{
}

CProjectDoc::~CProjectDoc()
{
}

void CProjectDoc::Serialize(CArchive& ar)
{
}

BOOL CProjectDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    if (!CDocument::OnOpenDocument(lpszPathName))
        return FALSE;

    bool ok = nPostFileUtilities::OpenFile(lpszPathName);

    projectUtilities::SetModifiedFlag(false);

    return ok;
}

BOOL CProjectDoc::OnOpenTemplateDocument(LPCTSTR lpszPathName)
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    bool ok = nPostFileUtilities::OpenFile(lpszPathName);

    projectUtilities::SetModifiedFlag(false);
    UpdateAllViews(NULL);
    LPCTSTR lpUntitledName = "Template";
    SetTitle(lpUntitledName);

    return ok;
}

CProjectDoc * CProjectDoc::GetDoc()
{
    CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
    return (CProjectDoc *) pFrame->GetActiveDocument();

}

BOOL CProjectDoc::SaveModified()
{
    if (projectUtilities::IsModified()) {
        SetModifiedFlag(TRUE);
    }

    return CDocument::SaveModified();
}

BOOL CProjectDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    nPostFileUtilities::SaveFile(lpszPathName);

    projectUtilities::SetModifiedFlag(false);

    return TRUE;
}

BOOL CProjectDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    projectRoot.SetDefault();

    projectUtilities::SetModifiedFlag(false);
    MenuRootC::UpdateAllUI();

    return TRUE;
}

void CProjectDoc::OnCloseDocument()
{
    projectRoot.Destroy();

    CDocument::OnCloseDocument();
}
