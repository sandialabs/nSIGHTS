///////////////////////////////////////////////////////////////////////////////////
//
// DlgSuperpositionTab.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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


// DlgSuperpositionTab.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include "DlgSuperpositionTab.h"

#include "DlgNoSuperposition.h"
#include "DlgSuperposition.h"

#include <Var/G_Parameter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSuperpositionTab dialog

IMPLEMENT_DYNCREATE(DlgSuperpositionTab,CGenericDialog)

T_SC_Array<NodeDescription> DlgSuperpositionTab::tabControl;


DlgSuperpositionTab::DlgSuperpositionTab(CWnd* pParent /*=NULL*/) :
  CGenericDialog(DlgSuperpositionTab::IDD, pParent),
  m_ctrlTab(NULL, 0, tabControl.tListData)
{
  //{{AFX_DATA_INIT(DlgSuperpositionTab)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void DlgSuperpositionTab::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgSuperpositionTab)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_TAB1, m_ctrlTab);
  if (!pDX->m_bSaveAndValidate) {

    m_ctrlTab.DestroyNode();
    m_ctrlTab.DeleteAllItems();

    DataCapturePtrArray superpositionCapture;
    dataCapture.GetSuperpositionCapture(superpositionCapture);

    if (superpositionCapture.IsEmpty())
    {
      static NodeDescription empty = { "None",  0,  RUNTIME_CLASS(DlgNoSuperposition),  NULL, &dataCapture};
      tabControl.Alloc(1);
      tabControl += empty;
      DlgNode* pDialog = new DlgNodeNoSuperposition(&m_ctrlTab, 0, &empty);
      m_ctrlTab.AddNode(pDialog);
    }
    else
    {
      static NodeDescription desc = { "XX",  0,  RUNTIME_CLASS(DlgSuperposition),  NULL, &dataCapture};
      tabControl.Alloc(superpositionCapture.Size());
      for (int i = 0; i < superpositionCapture.Size(); i++)
      {
        tabControl += desc;
        tabControl.LastIndex().m_szName = superpositionCapture[i]->dataDesig;

        DlgNode* pDialog = new DlgNodeSuperposition(&m_ctrlTab, i, &tabControl[i], superpositionCapture[i]);
        m_ctrlTab.AddNode(pDialog);
      }
    }

    VERIFY(m_ctrlTab.CreateNode(this));
  }
}


BEGIN_MESSAGE_MAP(DlgSuperpositionTab, CGenericDialog)
  //{{AFX_MSG_MAP(DlgSuperpositionTab)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgSuperpositionTab message handlers

BOOL DlgSuperpositionTab::OnInitDialog()
{
  CGenericDialog::OnInitDialog();
  SetControlInfo(IDC_TAB1, resizeBoth);

  InitData(true);

  return TRUE;
}


