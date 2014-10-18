///////////////////////////////////////////////////////////////////////////////////
//
// DlgSeqMain.cpp
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


// DlgSeqMain.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <genClass/U_Str.h>

#include <nsClass/nSightConst.h>
#include <Var/G_Sequence.h>
#include <Var/G_Layer.h>
#include <Var/G_Control.h>

#include "PopupCmdDefines.h"

#include "DlgSeqArray.h"
#include "DlgSeqMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


T_SC_Array<NodeDescription>   DlgSeqMain::tabControl;
T_SC_Array<SequenceArray*>    DlgSeqMain::seqArrayInfo;
T_SC_BasicPtrArray<UnitReal>  DlgSeqMain::seqStTime;
T_SC_BasicPtrArray<UnitReal>  DlgSeqMain::seqEndTime;
SC_BoolArray          DlgSeqMain::timeStepSequences;

/////////////////////////////////////////////////////////////////////////////
// DlgSeqMain dialog

IMPLEMENT_DYNCREATE(DlgSeqMain, CGenericDialog)

DlgSeqMain::DlgSeqMain(CWnd* pParent /*=NULL*/) :
  CGenericDialog(DlgSeqMain::IDD, pParent),
  m_ctrlTab(NULL, 0, tabControl.tListData)

{
  //{{AFX_DATA_INIT(DlgSeqMain)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

DlgSeqMain::~DlgSeqMain()
{
}


void DlgSeqMain::SequenceSetup()
{
  tabIDs.SetResizable(32);
  seqArrayInfo.SetResizable(32);
  tabControl.SetResizable(32);
  seqStTime.SetResizable(32);
  seqEndTime.SetResizable(32);
  timeStepSequences.SetResizable(32);

  if (control.IsLayered())
  {
    UnitReal* zoneSt = &testStartTime;
    if (useCommonHistory)
    {
      seqArrayInfo += &layerHistorySequenceData;
      tabIDs += "*PRE*";
      seqStTime += &testStartTime;
      seqEndTime += &layerHistoryEndTime;
      zoneSt = &layerHistoryEndTime;
      timeStepSequences += true;
    };

    for (int i = 0; i < wellBoreZones.Size(); i++)
    {
      const WellBoreZone& currZone = wellBoreZones[i];
      if ((currZone.seqIndex >= 0) && (!currZone.zoneIsPacker))
      {
        seqArrayInfo += &layerSequenceData[currZone.seqIndex];
        bool isTSZone = SameString(referenceZoneID, currZone.intervalID);
        if (isTSZone)
        {
          char tabID[80];
          MakeString(tabID, "*", currZone.intervalID, "*", 80);
          tabIDs += tabID;
        }
        else
          tabIDs += currZone.intervalID;
        seqStTime += zoneSt;
        seqEndTime += &testEndTime;
        timeStepSequences += SameString(referenceZoneID, currZone.intervalID);
      }
    }

  }
  else
  {
    seqArrayInfo += &singleSequenceData;
    tabIDs += "Single";
    seqStTime += &testStartTime;
    seqEndTime += &testEndTime;
    timeStepSequences += true;
  }

  tabControl.SetEmpty();
  for (int i = 0; i < seqArrayInfo.Size(); i++)
  {
    NodeDescription& currNode = tabControl[i];
    currNode.m_szName = tabIDs[i];;
    currNode.m_ImageID = 0;
    currNode.m_rtDialog = RUNTIME_CLASS(DlgSequenceArray);
    currNode.m_pNextLevel = 0;
    currNode.m_pListFactory = &sequence;
  }
  tabControl.SetSize(seqArrayInfo.Size());
  tabControl += 0;
}

void DlgSeqMain::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_TAB1, m_ctrlTab);
  if (!pDX->m_bSaveAndValidate) {

    SequenceSetup();
    m_ctrlTab.DestroyNode();
    m_ctrlTab.DeleteAllItems();
    for (int i = 0; i < seqArrayInfo.Size(); i++) {
      if (seqArrayInfo[i]->IsNotEmpty()) {
        AddSequenceTab(i, seqArrayInfo[i], seqStTime.GetRef(i), seqEndTime.GetRef(i), timeStepSequences[i]);
      }
    }

    VERIFY(m_ctrlTab.CreateNode(this));
  }
}


BEGIN_MESSAGE_MAP(DlgSeqMain, CGenericDialog)
  //{{AFX_MSG_MAP(DlgSeqMain)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL DlgSeqMain::OnInitDialog()
{
  CGenericDialog::OnInitDialog();
  SetControlInfo(IDC_TAB1, resizeBoth);

  InitData(true);

  return TRUE;
}

void DlgSeqMain::AddSequenceTab(int nTab, SequenceArray* pSequenceArray, UnitReal& stTime, UnitReal& endTime, bool isTS)
{
  DlgNode* pDialog = new DlgNodeSequenceArray(&m_ctrlTab, nTab,
      &tabControl[nTab],*pSequenceArray, stTime, endTime, isTS);

  m_ctrlTab.AddNode(pDialog);
}


