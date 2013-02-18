///////////////////////////////////////////////////////////////////////////////////
//
// DlgConfigUnits.cpp
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
//      nPre dialog.
//
///////////////////////////////////////////////////////////////////////////////////

// DlgConfigUnits.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgConfigUnits.h"

#include <genUnits/UnitRealCtrl.h>
#include <Var/G_Control.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgConfigUnits dialog

IMPLEMENT_DYNCREATE(DlgConfigUnits,CGenericDialog)

DlgConfigUnits::DlgConfigUnits(CWnd* pParent /*=NULL*/)
   : CGenericDialog(DlgConfigUnits::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgConfigUnits)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}



BEGIN_MESSAGE_MAP(DlgConfigUnits, CGenericDialog)
    //{{AFX_MSG_MAP(DlgConfigUnits)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgConfigUnits message handlers

BOOL DlgConfigUnits::OnInitDialog()
{
    InitUnit(IDC_COMBO1, uTime);
    InitUnit(IDC_COMBO2, uDist );
    InitUnit(IDC_COMBO3, uVol  );
    InitUnit(IDC_COMBO4, uMass );

    InitUnit(IDC_COMBO5, uFlow );
    InitUnit(IDC_COMBO6, uMflow);
    InitUnit(IDC_COMBO7, uPress);

    InitUnit(IDC_COMBO8, uK    );
    InitUnit(IDC_COMBO9, uPerm );
    InitUnit(IDC_COMBO10,uTran );
    InitUnit(IDC_COMBO11,uStor );
    InitUnit(IDC_COMBO12,uComp );
    InitUnit(IDC_COMBO13,uWbor );

    InitUnit(IDC_COMBO14,uDens );
    InitUnit(IDC_COMBO15,uVisc );
    InitUnit(IDC_COMBO16,uTemp );
    InitUnit(IDC_COMBO17,uTexp );
    InitUnit(IDC_COMBO18,uGeom );

    InitUnit(IDC_COMBO19,uTinv );
    InitUnit(IDC_COMBO20,uRatio);

    CGenericDialog::OnInitDialog();

    InitData(false);

    return TRUE;
}


void DlgConfigUnits::ResetAllControls()
{
    for (int i = 0; i < maxUnitsCtrl; i++)
        m_unitCtrl[i].DoCtrlReset();
    CheckRestrictions();
}

void DlgConfigUnits::InitUnit(int nID, int unitType)
{
    SC_StringArray  unitStr;
    UnitsBase::GetUnitStrings(unitType, false, unitStr);

    int ctrlIndex = nID-IDC_COMBO1;
    m_unitCtrl[ctrlIndex].AddBasicEntry(unitStr);
    m_unitCtrl[ctrlIndex].Set(this, nID, &UnitsBase::defaultUnitIndex[unitType]);
}

void DlgConfigUnits::CheckRestrictions()
{
    bool bHeadPressure = control.IsLiquid() && NSUnitsBase::headFlag;
    GetDlgItem(IDC_STATIC_HEADPRESS)->SetWindowText(bHeadPressure? "Head" : "Pressure");

    SC_StringArray  pressStr;
    UnitsBase::GetUnitStrings(uPress, false, pressStr);
    m_unitCtrl[IDC_COMBO7 - IDC_COMBO1].NewStrings(pressStr);
}

