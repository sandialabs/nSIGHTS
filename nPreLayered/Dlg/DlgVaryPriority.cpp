///////////////////////////////////////////////////////////////////////////////////
//
// DlgVaryPriority.cpp
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


#include "stdafx.h"
#include "Resource.h"

#include <osMain/G_Vary.h>

#include "DlgVaryPriority.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgVaryPriority dialog

IMPLEMENT_DYNCREATE(DlgVaryPriority, CGenericDialog)

DlgVaryPriority::DlgVaryPriority(CWnd* pParent /*=NULL*/) :
  CGenericDialog(DlgVaryPriority::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgVaryPriority)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void DlgVaryPriority::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgVaryPriority)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgVaryPriority, CGenericDialog)
  //{{AFX_MSG_MAP(DlgVaryPriority)
  // NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgVaryPriority message handlers

BOOL DlgVaryPriority::OnInitDialog()
{
  CGenericDialog::OnInitDialog();

  for (int i = 0; i < 3; i++)
  {
    varyPriority[i] = i;
    m_varyPriority[i].AddMappedEntry("Dummy", 0);
    m_varyPriority[i].Set(this, IDC_COMBO1 + i,  &varyPriority[i]);
    m_varyPriority[i].AddCallback(this, i);
  }

  InitData(true);

  ResetAllControls();
  return TRUE;

}

void DlgVaryPriority::Swap(int i, int j)
{
  VaryVar* temp =  orderedVary[i];
  orderedVary[i] = orderedVary[j];
  orderedVary[j] = temp;
}

void DlgVaryPriority::DoCtrlUpdate(int reason)
{
  if (orderedVary.Size() < 2)
    return;

  //  check for no change
  if (varyPriority[reason] == reason)
    return;


  // brute force approach - I am tired and didn't want to figure out the elegant way ...

  if (orderedVary.Size() == 2)
  {
    Swap(0, 1);
  }

  if (orderedVary.Size() == 3)
  {
    switch (reason) {
    case 0: {
      if (varyPriority[0] == 1)
        Swap(0, 1);
      else
        Swap(0, 2);
      break;
    }
    case 1: {
      if (varyPriority[1] == 0)
        Swap(0, 1);
      else
        Swap(1, 2);
      break;
    }
    case 2: {
      if (varyPriority[2] == 0)
        Swap(0, 2);
      else
        Swap(1, 2);
      break;
    }
    }
  }

  for (int i = 0; i < orderedVary.Size(); i++)
    orderedVary[i]->varyOrder = i;

  ResetAllControls();
}


void DlgVaryPriority::ResetAllControls()
{
  vary.SetForFlags();

  SC_StringArray newVaryPars;
  newVaryPars.SetStringLen(60);
  newVaryPars.Alloc(orderedVary.Size());
  for (int i = 0; i < orderedVary.Size(); i++)
    newVaryPars += orderedVary[i]->GetLongID();

  for (int j = 0; j < 3; j++)
  {
    m_varyPriority[j].ClearEntries();
    if (j < orderedVary.Size())
      m_varyPriority[j].AddBasicEntry(newVaryPars);
    m_varyPriority[j].ResetStrings();
    varyPriority[j] = j;
    m_varyPriority[j].ResetSelection();
  }

  CheckRestrictions();
}


void DlgVaryPriority::CheckRestrictions()
{
  for (int i = 0; i < 3; i++)
    m_varyPriority[i].EnableWindow(orderedVary.Size() > i);
}
