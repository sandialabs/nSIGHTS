///////////////////////////////////////////////////////////////////////////////////
//
// ComboFOCtrl.cpp
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
//              Dropdown with list of defined FuncObjC for object connections
//
///////////////////////////////////////////////////////////////////////////////////

// UnitIndexCtrl.cpp : implementation file
//

#include "GenLibAfx.h"
#include "ComboFOCtrl.h"

#include <genApp/C_AppMenu.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboFOCtrl

CComboFOCtrl::CComboFOCtrl(const type_info& inType) :
    m_pFOVal(NULL),
    listType(inType)
{
}

CComboFOCtrl::~CComboFOCtrl()
{
}


void CComboFOCtrl::Set(CWnd* pParent, int nIDCombo, FuncObjRef&  pFOVal)
{
    VERIFY(SubclassDlgItem(nIDCombo, pParent));

    m_pFOVal = &pFOVal;

    ResetSelection();
}


void CComboFOCtrl::ResetSelection()
{

    SC_StringArray  comboStrings;
    listDataOK = AppMenuC::GetFuncObjList(listType, comboStrings, listData, "\t");

    ResetContent();
    for (int i = 0; i < comboStrings.Size(); i++)
        {
            int retVal = AddString(comboStrings[i]);
            ASSERT(retVal == i);
        }

    if (listDataOK)
        {
            if (m_pFOVal->objRef == 0)
                {
                    (*m_pFOVal) = listData[0];
                }

            for (int i =0; i < listData.Size(); i++)
                if (listData[i] == (*m_pFOVal))
                    SetCurSel(i);
        }

    EnableWindow(listDataOK);
}


void CComboFOCtrl::DoCtrlReset()
{
    ResetSelection();
}


BEGIN_MESSAGE_MAP(CComboFOCtrl, CTabbedComboBox)
//{{AFX_MSG_MAP(CComboFOCtrl)
ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboFOCtrl message handlers

void CComboFOCtrl::OnSelchange()
{
    int nIndex = GetCurSel();
    (*m_pFOVal) = listData[nIndex];
    DoCallbacks();
}

