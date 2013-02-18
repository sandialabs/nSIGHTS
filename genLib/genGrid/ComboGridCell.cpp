///////////////////////////////////////////////////////////////////////////////////
//
// ComboGridCell.cpp
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

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>

#include "ComboEditWnd.h"
#include "ComboGridCell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CComboGridCell::CComboGridCell(const char* szText, CComboGridCell::Alignment alignment) :
    m_sText(szText),
    m_alignment(alignment)
{
}

CComboGridCell::~CComboGridCell()
{
}

// copy constructor
CComboGridCell::CComboGridCell(const CComboGridCell& rhs) :
    CGridCell(rhs)
{
    m_sText = rhs.m_sText;
    m_alignment = rhs.m_alignment;
}

// assignment operator
const CComboGridCell& CComboGridCell::operator=(const CComboGridCell& rhs)
{
    if (&rhs == this) {
        return *this;
    }
    CGridCell::operator=(rhs);
    m_sText = rhs.m_sText;
    m_alignment = rhs.m_alignment;
    return *this;
}

void CComboGridCell::DrawUnconstrained(CDC* pDC, int x, int y) const
{
    pDC->TextOut(x, y, m_sText, StringLength(m_sText));
}

void CComboGridCell::DrawConstrained(CDC* pDC, int x, int y, int cx, int cy) const
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

const char* CComboGridCell::GetText()
{
    return (const char*)m_sText;
}

void CComboGridCell::SetText(const char* szText)
{
    m_sText = szText;
}

CSize CComboGridCell::GetExtent(CDC* pDC) const
{
    CSize sz = GetNewlineTextExtent(pDC, m_sText);
    sz.cx += 40;
    return sz;
}

bool CComboGridCell::CanEdit()
{
    return CGridCell::CanEdit();
}

bool CComboGridCell::CanClose(CWnd *pWnd)
{
    ASSERT( pWnd != NULL );
    CComboEditWnd* pEditWnd = static_cast<CComboEditWnd *>(pWnd);
    return pEditWnd->CanClose();
}

void CComboGridCell::EndEdit(CWnd *pWnd)
{
    ASSERT( pWnd != NULL );
    CComboEditWnd* pEditWnd = static_cast<CComboEditWnd *>(pWnd);
    pEditWnd->EndEdit();
}

void CComboGridCell::AddMenuItems(CMenu *pMenu)
{
}

CWnd* CComboGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
    CStringArray aText;
    CArray<bool,bool> aEnable;
    GetSelectionList(aText, aEnable);

    CComboEditWnd* pEdit = new CComboEditWnd(pParent, rect, CBS_DROPDOWNLIST, nID, nRow, nCol,
                                             aText,
                                             GetText(),
                                             nFirstChar);
    for (int i = 0; i < aEnable.GetSize(); i++) {
        pEdit->EnableItem(i, aEnable[i]);
    }
    return pEdit;
}

void CComboGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
    aText.RemoveAll();
    aEnable;
#ifdef _DEBUG
    aText.Add("Alabama");
    aText.Add("Zero");
    aText.Add("Bomb");
    aText.Add("Test !!!");
#endif
}


void CComboGridCell::SetEnumSelectionList(CStringArray&          aText,
                                          CArray<bool,bool>&     aEnable,    // all valid
                                          const SC_StringArray&  enumStrs)
{
    aText.RemoveAll();
    aEnable.RemoveAll();
    for (int i = 0; i < enumStrs.Size(); i++)
        {
            aText.Add(enumStrs[i]);
            aEnable.Add(true);
        }
}


void CComboGridCell::SetEnumText(int                    enumVal,
                                 const  SC_StringArray& enumStrs,
                                 const   SC_IntArray&   enumMap)
{
    for (int i = 0; i < enumStrs.Size(); i++)
        if (enumVal == enumMap[i])
            {
                SetText(enumStrs[i]);
                return;
            }

    SetText(nullStr);
}

void CComboGridCell::SetEnumText(int                         enumVal,
                                 const   SC_StringArray&     enumStrs)
{
    if (enumVal < 0)
        SetText(nullStr);
    else
        SetText(enumStrs[enumVal]);
}

