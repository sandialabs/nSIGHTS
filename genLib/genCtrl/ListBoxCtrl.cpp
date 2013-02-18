///////////////////////////////////////////////////////////////////////////////////
//
// ListBoxCtrl.cpp
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
#include "ListBoxCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl

CListBoxCtrl::CListBoxCtrl() :
    m_pintVal(NULL)
{
    listStrings.SetResizable();
    listStrings.SetStringLen(50);
    listStrings.Alloc(5);

    m_intValArray.SetResizable();
}

CListBoxCtrl::~CListBoxCtrl()
{
}

void CListBoxCtrl::ClearEntries()
{
    listStrings.SetEmpty();
}


void CListBoxCtrl::AddBasicEntry(const SC_StringArray& allStrings)
{
    listStrings = allStrings;
}


void CListBoxCtrl::Set(CWnd* pParent, int nIDCombo, int* pintVal)
{
    VERIFY(SubclassDlgItem(nIDCombo, pParent));

    m_pintVal = pintVal;

    multiSelection = false;

    ResetStrings();

    EnableWindow(!listStrings.IsEmpty());
    ResetSelection();
}

void CListBoxCtrl::Set(CWnd* pParent, int nIDCombo, SC_IntArray& intValArray)
{
    VERIFY(SubclassDlgItem(nIDCombo, pParent));

    m_intValArray = intValArray;

    multiSelection = true;

    ResetStrings();

    EnableWindow(!listStrings.IsEmpty());
    ResetSelection();
}

void CListBoxCtrl::GetMultipleSel(SC_IntArray& intValArray)
{
    intValArray = m_intValArray;
}

void CListBoxCtrl::ResetStrings()
{
    ResetContent();
    for (int i = 0; i < listStrings.Size(); i++)
        {
            int retVal = AddString(listStrings[i]);
            ASSERT(retVal == i);
        }
}


void CListBoxCtrl::ResetSelection()
{
    if (multiSelection)
        {
            SetRedraw(false);

            ClearSelections();

            int numSel = m_intValArray.Size();
            for (int i = 0; i < numSel; i++)
                SetSel(m_intValArray[i], true);

            SetRedraw(true);
            Invalidate();
            UpdateWindow();
        }
    else
        SetCurSel(*m_pintVal);
}

void CListBoxCtrl::ClearSelections()
{
    if (!multiSelection)
        {
            SetCurSel(-1);
        }
    else
        {
            int nItem = GetCount();
            if (nItem > 0)
                {
                    SelItemRange(FALSE, 0, nItem - 1);
                }
        }

}

void CListBoxCtrl::NewStrings(const SC_StringArray& newStrings)
{
    ClearEntries();
    AddBasicEntry(newStrings);
    ResetStrings();
    ResetSelection();
}


void CListBoxCtrl::DoCtrlReset()
{
    ResetSelection();
}


BEGIN_MESSAGE_MAP(CListBoxCtrl, CListBox)
//{{AFX_MSG_MAP(CListBoxCtrl)
ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl message handlers

void CListBoxCtrl::OnSelchange()
{
    if (multiSelection)
        {
            int numSel = GetSelCount();
            m_intValArray.AllocAndSetSize(numSel);
            GetSelItems(numSel, m_intValArray.tListData);
        }
    else
        (*m_pintVal) = GetCurSel();

    DoCallbacks();
}

