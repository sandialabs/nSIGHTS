///////////////////////////////////////////////////////////////////////////////////
//
// DlgParParameter.cpp
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


// DlgParParameter.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include "DlgParParameter.h"

#include "DlgParameterGroup.h"

#include <Var/G_Control.h>
#include <Var/G_Layer.h>
#include <Var/G_Parameter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

T_SC_Array<NodeDescription>  DlgParParameter::tabControl(maxGeoLayers + 4);
T_SC_Array<ParameterPtrArray*>  DlgParParameter::parArrayInfo(maxGeoLayers + 4);

/////////////////////////////////////////////////////////////////////////////
// DlgParParameter dialog

IMPLEMENT_DYNCREATE(DlgParParameter,CGenericDialog)



DlgParParameter::DlgParParameter(CWnd* pParent /*=NULL*/) :
  CGenericDialog(DlgParParameter::IDD, pParent),
  m_ctrlTab(NULL, 0, tabControl.tListData)
{
  //{{AFX_DATA_INIT(DlgParParameter)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void DlgParParameter::ParameterSetup()
{
  parArrayInfo.SetEmpty();
  if (control.IsLayered())
  {
    parArrayInfo += &layerCommon;
    parArrayInfo += &fluidPar;
    parArrayInfo += &numericPar;
    for (int i = 0; i < layerPar.Size(); i++)
      parArrayInfo += &layerPar[i];
  }
  else
  {
    parArrayInfo += &formationPar;
    parArrayInfo += &fracturePar;
    parArrayInfo += &matrixPar;
    parArrayInfo += &skinPar;
    parArrayInfo += &fluidPar;
    parArrayInfo += &gasPar;
    parArrayInfo += &testZonePar;
    parArrayInfo += &leakyPar;
    parArrayInfo += &upperLeakyPar;
    parArrayInfo += &lowerLeakyPar;
    parArrayInfo += &numericPar;
  }

  tabControl.SetEmpty();
  for (int i = 0; i < parArrayInfo.Size(); i++)
  {
    NodeDescription& currNode = tabControl[i];
    currNode.m_szName = parArrayInfo[i]->parDesc;
    currNode.m_ImageID = 0;
    currNode.m_rtDialog = RUNTIME_CLASS(DlgParameterGroup);
    currNode.m_pNextLevel = 0;
    currNode.m_pListFactory = &parameters;
  }
  tabControl.SetSize(parArrayInfo.Size());
  tabControl += 0;

}


void DlgParParameter::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgParParameter)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_TAB1, m_ctrlTab);
  if (!pDX->m_bSaveAndValidate) {

    ParameterSetup();
    m_ctrlTab.DestroyNode();
    m_ctrlTab.DeleteAllItems();
    for (int i = 0; i < parArrayInfo.Size(); i++) {
      if (parArrayInfo[i]->IsNotEmpty()) {
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

void DlgParParameter::AddParameterTab(int nTab, ParameterPtrArray* pParameterArray)
{
  DlgNode* pDialog = new DlgNodeParameterGroup(&m_ctrlTab, nTab,
      &tabControl[nTab],
      *pParameterArray );
  m_ctrlTab.AddNode(pDialog);
}

