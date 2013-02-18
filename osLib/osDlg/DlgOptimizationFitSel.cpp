///////////////////////////////////////////////////////////////////////////////////
//
// DlgOptimizationFitSel.cpp
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

#include "DlgOptimizationFitSel.h"

#include <osClass/DO_FitResults.h>
#include <genClass/U_Msg.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationFitSel dialog

IMPLEMENT_DYNCREATE(DlgOptimizationFitSel,CGenericDialog)

DlgOptimizationFitSel::DlgOptimizationFitSel(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgOptimizationFitSel::IDD, pParent)
{
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
        m_fitObject[i] = new CComboFOCtrl(typeid(DO_FitResults));



    //{{AFX_DATA_INIT(DlgOptimizationFitSel)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

DlgOptimizationFitSel::~DlgOptimizationFitSel()
{
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
        delete m_fitObject[i];
}

BEGIN_MESSAGE_MAP(DlgOptimizationFitSel, CGenericDialog)
    //{{AFX_MSG_MAP(DlgOptimizationFitSel)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOptimizationFitSel message handlers

BOOL DlgOptimizationFitSel::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        m_fitSelected[i].Set(this, IDC_CHECK1+ i, optimizeInput.inputFits[i].optimizeOnFit);
        m_fitSelected[i].AddCallback(this, 0);
        m_fitObject[i]->Set(this, IDC_COMBO1 + i, optimizeInput.inputFits[i].fitObject);
    }

    m_autoUpdate.Set(this, IDC_BUTTON1);
    m_autoUpdate.AddCallback(this, 1);

    ResetAllControls();

    return TRUE;
}

void DlgOptimizationFitSel::DoCtrlUpdate(int reason)
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

void DlgOptimizationFitSel::AutoUpdate()
{
    if (!m_fitObject[0]->FOAvail())
    {
        GenAppErrorMsg("FitSel", "no fits defined");
        return;
    }

    const FuncObjRefArray&  currFits = m_fitObject[0]->GetListData();
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        if (i < currFits.Size())
        {
            optimizeInput.inputFits[i].optimizeOnFit = true;
            optimizeInput.inputFits[i].fitObject = currFits[i];
        }
        else
        {
            optimizeInput.inputFits[i].optimizeOnFit = false;
            optimizeInput.inputFits[i].fitObject = currFits[0];
        }
    }

    ResetAllControls();
}


void DlgOptimizationFitSel::CheckRestrictions()
{
    bool foOK = m_fitObject[0]->FOAvail();
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        m_fitSelected[i].EnableWindow(foOK);
        m_fitObject[i]->EnableWindow(foOK && optimizeInput.inputFits[i].optimizeOnFit);
    }
}

void DlgOptimizationFitSel::ResetAllControls()
{
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        m_fitObject[i]->ResetSelection();
        m_fitSelected[i].ResetSelection();
    }
    CheckRestrictions();
}


