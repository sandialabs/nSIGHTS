///////////////////////////////////////////////////////////////////////////////////
//
// DialogGridCell.cpp
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
// DESCRIPTION: Windows specific code for implementing basic spreadsheet type
//              data entry/display.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include "DialogGridCell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CDialogGridCell::CDialogGridCell(const char* szText, CDialogGridCell::Alignment alignment) :
    m_sText(szText),
    m_alignment(alignment)
{
}

CDialogGridCell::~CDialogGridCell()
{
}

// copy constructor
CDialogGridCell::CDialogGridCell(const CDialogGridCell& rhs) :
    CGridCell(rhs)
{
    m_sText = rhs.m_sText;
    m_alignment = rhs.m_alignment;
}

// assignment operator
const CDialogGridCell& CDialogGridCell::operator=(const CDialogGridCell& rhs)
{
    if (&rhs == this) {
        return *this;
    }
    CGridCell::operator=(rhs);
    m_sText = rhs.m_sText;
    m_alignment = rhs.m_alignment;
    return *this;
}

void CDialogGridCell::DrawUnconstrained(CDC* pDC, int x, int y) const
{
    pDC->TextOut(x, y, m_sText, StringLength(m_sText));
}

void CDialogGridCell::DrawConstrained(CDC* pDC, int x, int y, int cx, int cy) const
{
    CRect rect(x, y, x + cx, y + cy);

    switch (m_alignment) {
    case alignLeft:
        pDC->DrawText(m_sText, StringLength(m_sText), &rect, DT_LEFT);
        break;
    case alignRight:
        pDC->DrawText(m_sText, -1, &rect, DT_RIGHT);
        break;
    case alignCenter:
        pDC->DrawText(m_sText, -1, &rect, DT_CENTER);
        break;
    }
}

const char* CDialogGridCell::GetText()
{
    return (const char*)m_sText;
}

void CDialogGridCell::SetText(const char* szText)
{
    m_sText = szText;
}

CSize CDialogGridCell::GetExtent(CDC* pDC) const
{
    return GetNewlineTextExtent(pDC, m_sText);
}

bool CDialogGridCell::CanEdit()
{
    return true;
}

bool CDialogGridCell::CanClose(CWnd *pWnd)
{
    ASSERT( pWnd != NULL );
    return true;
}

void CDialogGridCell::EndEdit(CWnd *pWnd)
{
    ASSERT(0);
}

void CDialogGridCell::AddMenuItems(CMenu *pMenu)
{
}

#include "GridCtrlCell.h"

CWnd* CDialogGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
    switch (AfxMessageBox("OK Dialog Test", MB_YESNOCANCEL)) {
    case IDYES:
        m_sText = "OK";
        break;
    case IDNO:
        m_sText = "Bad";
        break;
    case IDCANCEL:
        nFirstChar = 0;
        break;
    }
    pParent->SetFocus();
    static_cast<CGridCtrlCell*>(pParent)->FinishEdit(m_sText, -1, nFirstChar);
    return NULL;
}

