///////////////////////////////////////////////////////////////////////////////////
//
// ComboIntCtrl.cpp
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
//              Dropdown with multiple entries mapped to integers
//
///////////////////////////////////////////////////////////////////////////////////

// UnitIndexCtrl.cpp : implementation file
//

#include "GenLibAfx.h"
#include "ComboIntCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboIntCtrl

CComboIntCtrl::CComboIntCtrl() :
    m_pintVal(NULL)
{
    comboStrings.SetStringLen(50);
    comboStrings.SetResizable(5);
    comboVals.SetResizable(5);
}

CComboIntCtrl::~CComboIntCtrl()
{
}

void CComboIntCtrl::ClearEntries()
{
    comboVals.SetEmpty();
    comboStrings.SetEmpty();
}

void CComboIntCtrl::AddBasicEntry(const char* inString)
{
    comboVals += comboStrings.Size();
    comboStrings += inString;
}


void CComboIntCtrl::AddBasicEntry(const SC_StringArray& allStrings)
{
    comboStrings = allStrings;
    comboVals.SetEmpty();
    for (int i = 0; i < comboStrings.Size(); i++)
        comboVals += i;
}

void CComboIntCtrl::AddMappedEntry(const char* inString, int inMap)
{
    comboVals += inMap;
    comboStrings += inString;
}

void CComboIntCtrl::Set(CWnd* pParent, int nIDCombo, int* pintVal)
{
    VERIFY(SubclassDlgItem(nIDCombo, pParent));

    m_pintVal = pintVal;

    ResetStrings();

    EnableWindow(!comboStrings.IsEmpty());
    ResetSelection();
}

void CComboIntCtrl::ResetStrings()
{
    ResetContent();
    for (int i = 0; i < comboVals.Size(); i++)
        {
            int retVal = AddString(comboStrings[i]);
            ASSERT(retVal == i);
        }
}


void CComboIntCtrl::ResetSelection()
{
    for (int i =0; i < comboVals.Size(); i++)
        if ((*m_pintVal) == comboVals[i])
            SetCurSel(i);
}

void CComboIntCtrl::NewStrings(const SC_StringArray& newStrings)
{
    ClearEntries();
    AddBasicEntry(newStrings);
    ResetStrings();
    ResetSelection();
}



void CComboIntCtrl::DoCtrlReset()
{
    ResetSelection();
}


BEGIN_MESSAGE_MAP(CComboIntCtrl, CComboBox)
//{{AFX_MSG_MAP(CComboIntCtrl)
ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboIntCtrl message handlers

void CComboIntCtrl::OnSelchange()
{
    int nIndex = GetCurSel();
    (*m_pintVal) = comboVals[nIndex];
    DoCallbacks();
}

