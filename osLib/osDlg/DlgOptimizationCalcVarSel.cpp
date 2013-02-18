///////////////////////////////////////////////////////////////////////////////////
//
// DlgOptimizationCalcVarSel.cpp
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

#include "OsLibAfx.h"

#include "DlgOptimizationCalcVarSel.h"

#include <osClass/DO_CalcParReal.h>
#include <genClass/U_Msg.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationCalcVarSel dialog

IMPLEMENT_DYNCREATE(DlgOptimizationCalcVarSel,CGenericDialog)

DlgOptimizationCalcVarSel::DlgOptimizationCalcVarSel(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgOptimizationCalcVarSel::IDD, pParent)
{
    for (int i = 0; i < OptimizeInput::maxOptCalcVar ; i++)
        m_calcVarObject[i] = new CComboFOCtrl(typeid(DO_CalcParReal));



    //{{AFX_DATA_INIT(DlgOptimizationCalcVarSel)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

DlgOptimizationCalcVarSel::~DlgOptimizationCalcVarSel()
{
    for (int i = 0; i < OptimizeInput::maxOptCalcVar ; i++)
        delete m_calcVarObject[i];
}

BEGIN_MESSAGE_MAP(DlgOptimizationCalcVarSel, CGenericDialog)
    //{{AFX_MSG_MAP(DlgOptimizationCalcVarSel)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationCalcVarSel message handlers

BOOL DlgOptimizationCalcVarSel::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    for (int i = 0; i < OptimizeInput::maxOptCalcVar ; i++)
    {
        m_calcVarSelected[i].Set(this, IDC_CHECK1+ i, optimizeInput.inputCalcVars[i].includeCalcVar);
        m_calcVarSelected[i].AddCallback(this, 0);
        m_calcVarObject[i]->Set(this, IDC_COMBO1 + i, optimizeInput.inputCalcVars[i].calcVarObject);
    }

    m_autoUpdate.Set(this, IDC_BUTTON1);
    m_autoUpdate.AddCallback(this, 1);

    ResetAllControls();

    return TRUE;
}

void DlgOptimizationCalcVarSel::DoCtrlUpdate(int reason)
{
    if (reason == 0)
    {
        CheckRestrictions();
    }
    else
    {
        AutoUpdate();
    }
}

void DlgOptimizationCalcVarSel::AutoUpdate()
{
    if (!m_calcVarObject[0]->FOAvail())
    {
        GenAppErrorMsg("FitSel", "no fits defined");
        return;
    }

    const FuncObjRefArray&  currCalc = m_calcVarObject[0]->GetListData();
    for (int i = 0; i < OptimizeInput::maxOptCalcVar ; i++)
    {
        if (i < currCalc.Size())
        {
            optimizeInput.inputCalcVars[i].includeCalcVar = true;
            optimizeInput.inputCalcVars[i].calcVarObject = currCalc[i];
        }
        else
        {
            optimizeInput.inputCalcVars[i].includeCalcVar = false;
            optimizeInput.inputCalcVars[i].calcVarObject = currCalc[0];
        }
    }

    ResetAllControls();
}


void DlgOptimizationCalcVarSel::CheckRestrictions()
{
    bool foOK = m_calcVarObject[0]->FOAvail();
    for (int i = 0; i < OptimizeInput::maxOptCalcVar ; i++)
    {
        m_calcVarSelected[i].EnableWindow(foOK);
        m_calcVarObject[i]->EnableWindow(foOK && optimizeInput.inputCalcVars[i].includeCalcVar);
    }
}

void DlgOptimizationCalcVarSel::ResetAllControls()
{
    for (int i = 0; i < OptimizeInput::maxOptCalcVar ; i++)
    {
        m_calcVarObject[i]->ResetSelection();
        m_calcVarSelected[i].ResetSelection();
    }
    CheckRestrictions();
}


