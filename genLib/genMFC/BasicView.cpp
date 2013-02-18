///////////////////////////////////////////////////////////////////////////////////
//
// BasicView.cpp
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

#include "GenLibAfx.h"

#include <genApp/C_MenuObj.h>

#include "BasicView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicView

IMPLEMENT_DYNCREATE(CBasicView, CView)

BEGIN_MESSAGE_MAP(CBasicView, CView)
    //{{AFX_MSG_MAP(CBasicView)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_CUT, OnEditCut)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
    //}}AFX_MSG_MAP
    // Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicView construction/destruction

CBasicView::CBasicView()
{
}

CBasicView::~CBasicView()
{
}


void CBasicView::OnDraw(CDC* pDC)
{
}

//
// Cut/Paste hotkeys didn't work properly for child windows
// of BasicView
// See MSDN Knowledge Base ID: Q145616
//
static bool IsEdit(CWnd* pWnd)
{
    ASSERT(pWnd != NULL);
    HWND hWnd = pWnd->GetSafeHwnd();
    if (hWnd == NULL) {
        return false;
    }

    TCHAR szClassName[10];
    return ::GetClassName(hWnd, szClassName, 10) &&
         ((_tcsicmp(szClassName, _T("Edit")) == 0) ||
          (_tcsicmp(szClassName, _T("RichEdit")) == 0));
}

void CBasicView::OnEditCopy()
{
    CEdit* pEdit = (CEdit*)GetFocus();
    if (IsEdit( pEdit)) {
        pEdit->Copy();
    }
}

void CBasicView::OnEditCut()
{
    CEdit* pEdit = (CEdit*)GetFocus();
    if (IsEdit(pEdit)) {
        pEdit->Cut();
    }
}

void CBasicView::OnEditPaste()
{
    CWnd* pWnd = GetFocus();
    if (IsEdit(pWnd) && ::IsClipboardFormatAvailable(CF_TEXT)) {
        static_cast<CEdit*>(pWnd)->Paste();
    } else {
        pWnd->SendMessage(WM_COMMAND, ID_EDIT_PASTE);
    }
}

void CBasicView::OnEditUndo()
{
    CEdit* pEdit = (CEdit*)GetFocus();
    if (IsEdit(pEdit)) {
        pEdit->Undo();
    }
}

