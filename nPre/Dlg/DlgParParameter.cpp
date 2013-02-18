///////////////////////////////////////////////////////////////////////////////////
//
// DlgParParameter.cpp
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

// DlgParParameter.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include "DlgParParameter.h"

#include "DlgParameterGroup.h"

#include <Var/G_Parameter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgParParameter dialog

IMPLEMENT_DYNCREATE(DlgParParameter,CGenericDialog)

NodeDescription DlgParParameter::tabControl[] =
{
    { "Formation",          0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters},
    { "Fracture",           0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters },
    { "Matrix",             0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters },
    { "Skin zone",          0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters },
    { "Fluid",              0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters },
    { "Gas",                0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters },
    { "Test-zone",          0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters },
    { "Leaky layer",        0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters },
    { "Upper leaky layer",  0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters },
    { "Lower leaky layer",  0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters },
    { "Numeric",            0,  RUNTIME_CLASS(DlgParameterGroup),   NULL, &parameters },
    { NULL }
};

ParameterArray* DlgParParameter::parArrayInfo[] = {
    &formationPar,
    &fracturePar,
    &matrixPar,
    &skinPar,
    &fluidPar,
    &gasPar,
    &testZonePar,
    &leakyPar,
    &upperLeakyPar,
    &lowerLeakyPar,
    &numericPar,
    NULL,
};

DlgParParameter::DlgParParameter(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgParParameter::IDD, pParent),
    m_ctrlTab(NULL, 0, tabControl)
{
    //{{AFX_DATA_INIT(DlgParParameter)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void DlgParParameter::DoDataExchange(CDataExchange* pDX)
{
    CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgParParameter)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_TAB1, m_ctrlTab);
    if (!pDX->m_bSaveAndValidate) {

        m_ctrlTab.DestroyNode();
        m_ctrlTab.DeleteAllItems();
        for (int i = 0; parArrayInfo[i] != NULL; i++) {
            if (!parArrayInfo[i]->IsEmpty()) {
                AddParameterTab(i, parArrayInfo[i]);
            }
        }

        VERIFY(m_ctrlTab.CreateNode(this));
    }
}


BEGIN_MESSAGE_MAP(DlgParParameter, CGenericDialog)
    //{{AFX_MSG_MAP(DlgParParameter)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgParParameter message handlers

BOOL DlgParParameter::OnInitDialog()
{
    CGenericDialog::OnInitDialog();
    SetControlInfo(IDC_TAB1, resizeBoth);

    InitData(true);

    return TRUE;
}

void DlgParParameter::AddParameterTab(int nTab, ParameterArray* pParameterArray)
{
    DlgNode* pDialog = new DlgNodeParameterGroup(&m_ctrlTab, nTab,
                                                    &tabControl[nTab],
                                                    *pParameterArray );
    m_ctrlTab.AddNode(pDialog);
}

