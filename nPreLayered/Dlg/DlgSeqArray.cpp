///////////////////////////////////////////////////////////////////////////////////
//
// DlgSeqArray.cpp
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
#include <Var/G_Control.h>

#include "PopupCmdDefines.h"

#include "SequenceFlowHistDlg.h"
#include "SequenceSlugDlg.h"
#include "SequencePulseDlg.h"
#include "DlgSeqArray.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSeqMain dialog

IMPLEMENT_DYNCREATE(DlgSequenceArray, CGenericDialog)

DlgSequenceArray::DlgSequenceArray(CWnd* pParent /*=NULL*/) :
  CGenericDialog(DlgSequenceArray::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgSeqMain)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

DlgSequenceArray::~DlgSequenceArray()
{
}

void DlgSequenceArray::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgSeqMain)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSequenceArray, CGenericDialog)
  //{{AFX_MSG_MAP(DlgSeqMain)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgSeqMain message handlers

#include <genGrid/TextGridCell.h>
#include <genGrid/ComboGridCell.h>
#include <genGrid/CheckGridCell.h>
#include <genUnits/UnitGridCell.h>

class SeqTypeGridCell : public CComboGridCell
{
  friend class SequenceGridCtrl;
public:
  SeqTypeGridCell(int index, SequenceArray& seqIn);
  ~SeqTypeGridCell() {};

  virtual void  UpdateText();
  void      SetSequenceType(int nValue);

protected:
  virtual void  GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);

protected:
  const int   seqIndex;
  SequenceArray&  seqData;
};


SeqTypeGridCell::SeqTypeGridCell(int index, SequenceArray& seqIn) :
  CComboGridCell(NULL, alignLeft),
  seqIndex(index), seqData(seqIn)
{
}

void SeqTypeGridCell::UpdateText()
{
  if (seqData.IsValid(seqIndex))
    for (int i = 0; i < SequenceGridCtrl::seqTypeStrings.Size(); i++)
      if (int(seqData[seqIndex]->seqType) == SequenceGridCtrl::seqTypeMap[i])
      {
        SetText(SequenceGridCtrl::seqTypeStrings[i]);
        return;
      }
  SetText(nullStr);
}


void SeqTypeGridCell::SetSequenceType(int nValue)
{
  seqData.CheckAndAllocSequence(seqIndex);
  if (nValue < 0)
    nValue = 0;
  Sequence& currSeq = seqData.GetRef(seqIndex);
  currSeq.ChangeSeqType(Sequence::SequenceType(SequenceGridCtrl::seqTypeMap[nValue]));
  currSeq.ClearWasActive();
}

void SeqTypeGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
  aText.RemoveAll();
  aEnable.RemoveAll();
  for (int i = 0; i < SequenceGridCtrl::seqTypeStrings.Size(); i++)
  {
    aText.Add(SequenceGridCtrl::seqTypeStrings[i]);
    aEnable.Add(true);
  }
}

//    Designation

class DesigGridCell : public CTextGridCell
{
  friend class SequenceGridCtrl;
public:
  DesigGridCell(int index, SequenceArray& seqIn);

  virtual void  UpdateText();

protected:
  const int seqIndex;
  SequenceArray&  seqData;
};

DesigGridCell::DesigGridCell(int index, SequenceArray& seqIn) :
  CTextGridCell(NULL, alignLeft),
  seqIndex(index), seqData(seqIn)
{
}


void DesigGridCell::UpdateText()
{
  if (seqData.IsValid(seqIndex))
    SetText(seqData[seqIndex]->seqID);
  else
    SetText(nullStr);
}


//    duration

class TimeValueGridCell : public CTextGridCell
{
  friend class SequenceGridCtrl;
public:
  TimeValueGridCell(int index, SequenceArray& seqIn);

  virtual void  FocusIn();
  virtual void  UpdateText();

  bool      SetDuration(const char* szText);

protected:
  const int seqIndex;
  SequenceArray&  seqData;
};


TimeValueGridCell::TimeValueGridCell(int index, SequenceArray& seqIn) :
  CTextGridCell(NULL, alignRight),
  seqIndex(index), seqData(seqIn)
{
}

void TimeValueGridCell::FocusIn()
{
  UnitIndex temp(uTime);
  if (seqData.IsValid(seqIndex))
    temp = seqData.GetRef(seqIndex).seqLength;
  temp.SetRangeMsg(1.0E-3, 1.0E+12);
}

bool TimeValueGridCell::SetDuration(const char* szText)
{
  return seqData.GetRef(seqIndex).seqLength.IsWithinRange(szText, 1.0E-3, 1.0E+12);
}


void TimeValueGridCell::UpdateText()
{
  if (seqData.IsValid(seqIndex))
  {
    Sequence& currSeq = seqData.GetRef(seqIndex);
    CString sText;
    if (enterSequenceDurations)
    {
      currSeq.seqLength.GetUserString(sText);
    }
    else
    {
      UnitReal tempTime(testStartTime);
      tempTime.SetMetricVal(currSeq.seqLength.GetMetricVal());
      tempTime.GetUserString(sText);
    }
    SetText(sText);
  }
  else
    SetText(nullStr);
}


class StartTimeValueGridCell : public CTextGridCell
{
  friend class SequenceGridCtrl;
public:
  StartTimeValueGridCell(int index, SequenceArray& seqIn, UnitReal& stT, UnitReal& endT);
  virtual void  UpdateText();
  virtual void  FocusIn();
  bool      SetStartTime(const char* szText);

protected:
  const int seqIndex;
  SequenceArray&  seqData;
  UnitReal& stTime;
  UnitReal& endTime;
};


StartTimeValueGridCell::StartTimeValueGridCell(int index, SequenceArray& seqIn, UnitReal& stT, UnitReal& endT) :
  CTextGridCell(NULL, alignRight),
  seqIndex(index), seqData(seqIn), stTime(stT), endTime(endT)
{
}

void StartTimeValueGridCell::FocusIn()
{
  UnitIndex temp(stTime);
  temp.SetRangeMsg(-1.0E+09, endTime.GetMetricVal());
}

bool StartTimeValueGridCell::SetStartTime(const char* szText)
{
  double endLimit = endTime.GetMetricVal();
  if (seqIndex == 0)
  {
    if (!stTime.IsWithinRange(szText, -1.0E+09, endLimit))
      return false;
    seqData.SetSequenceTimes(stTime.GetMetricVal(), endTime.GetMetricVal());
    return true;
  }

  // limits are previous start time to end time
  if (seqData.IsNotValid(seqIndex - 1))
    return false;

  double stLimit = seqData.GetRef(seqIndex - 1).startTime;
  if (RealIsNull(stLimit))
    return false;

  UnitReal tempTime(stTime);

  if (tempTime.IsWithinRange(szText, stLimit, endLimit))
  {
    double currStart = tempTime.GetMetricVal();
    // re-set duration of prev seq
    seqData.GetRef(seqIndex - 1).seqLength.SetMetricVal(currStart - stLimit);

    // set current length
    // if current end time is null then arbitary at 3600 sec
    Sequence& currSeq = seqData.GetRef(seqIndex);

    // if last sequence entered set from end time
    if (seqIndex == (seqData.Size() - 1))
    {
      currSeq.seqLength.SetMetricVal(endLimit - currStart);
      return true;
    }


    if (RealIsNull(currSeq.endTime))
    {
      currSeq.seqLength.SetMetricVal(3600.0);
    }
    else
    {
      // in between sequences
      if ((seqData.AllocSize() > (seqIndex +1)) &&
          (seqData.IsValid(seqIndex + 1)))
      {
        currSeq.seqLength.SetMetricVal(currSeq.endTime - currStart);
      }
      else
      {
        // pathological
        // use end time as basis
        currSeq.seqLength.SetMetricVal(endLimit - currStart);
      }
    }

    return true;
  }
  return false;
}

void StartTimeValueGridCell::UpdateText()
{
  if (seqData.IsValid(seqIndex))
  {
    CString sText;
    UnitReal currStart = testStartTime;
    currStart.SetMetricVal(seqData[seqIndex]->startTime);
    currStart.GetUserString(sText);
    SetText(sText);
  }
  else
    SetText(nullStr);
}


///////////////////////////////////////////////////////////////////////////////////

class TimeUnitsGridCell : public CUnitGridCell
{
  friend class SequenceGridCtrl;
public:
  TimeUnitsGridCell(int index, SequenceArray& seqIn, Alignment alignment  = alignRight);
  virtual void  UpdateText();

protected:
  const int seqIndex;
  SequenceArray& seqData;
};

TimeUnitsGridCell::TimeUnitsGridCell(int index, SequenceArray& seqIn, Alignment alignment) :
  CUnitGridCell(uTime, nullUnitConvIndex, alignment),
  seqIndex(index), seqData(seqIn)
{
}

void TimeUnitsGridCell::UpdateText()
{
  if (seqData.IsValid(seqIndex))
    SetText(seqData[seqIndex]->seqLength.GetUnitString());
  else
    SetText(nullStr);
}



// Sequences /////////////////////////////////////////////////////////////////////////////////



class SeqDataGridCell : public CTextGridCell
{
  friend class SequenceGridCtrl;
public:
  SeqDataGridCell(int index, SequenceArray& seqIn, bool isTS, const char* szText = NULL, Alignment alignment = alignRight);

  virtual void  UpdateText();
protected:
  virtual bool  CanEdit();
  virtual CWnd* CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar);

protected:
  const int seqIndex;
  SequenceArray& seqData;
  bool isTSSeq;
};

SeqDataGridCell::SeqDataGridCell(int index, SequenceArray& seqIn, bool isTS, const char* szText, Alignment alignment) :
  CTextGridCell(szText, alignment),
  seqIndex(index), seqData(seqIn), isTSSeq(isTS)
{
}


void SeqDataGridCell::UpdateText()
{
  if (seqData.IsNotValid(seqIndex))
  {
    SetText(nullStr);
    return;
  }

  Sequence& currSeq = seqData.GetRef(seqIndex);

  CString sText;
  SC_SetupErr errData;

  // default values
  bool seqOK = currSeq.SetupOK(errData, isTSSeq);
  if (seqOK)
  {
    sText = "OK ";
  }
  else
  {
    sText = "BAD ";
  }

  SetText(sText);
}

bool SeqDataGridCell::CanEdit()
{
  return seqData.IsValid(seqIndex);
}

CWnd* SeqDataGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
  if (seqData.IsNotValid(seqIndex))
    return NULL;

  Sequence& currSeq = seqData.GetRef(seqIndex);

  switch (currSeq.seqType) {
  case Sequence::stFlow:
  {
    CSequenceFlowHistDlg dlg(currSeq, isTSSeq, currSeq.flowData, true);
    dlg.DoModal();
    break;
  }
  case Sequence::stHistory:
  {
    CSequenceFlowHistDlg dlg(currSeq, isTSSeq, currSeq.historyData, false);
    dlg.DoModal();
    break;
  }
  case Sequence::stPulse:
  {
    CSequencePulseDlg dlg(currSeq, isTSSeq);
    dlg.DoModal();
    break;
  }
  case Sequence::stSlug:
  {
    CSequenceSlugDlg dlg(currSeq, isTSSeq);
    dlg.DoModal();
    break;
  }
  case Sequence::stPacker:
  {
    break;
  }
  }

  pParent->SetFocus();
  static_cast<CGridCtrlCell*>(pParent)->FinishEdit(NULL, -1, nFirstChar);
  return NULL;
}


class UseAutoGridCell : public CCheckGridCell
{
  friend class SequenceGridCtrl;
public:
  UseAutoGridCell(int index, SequenceArray& seqIn);

  virtual void  UpdateText();
  virtual void  OnCellSelected(CWnd* pParent, int nRow, int nCol);

protected:
  const int seqIndex;
  SequenceArray& seqData;
};

UseAutoGridCell::UseAutoGridCell(int index, SequenceArray& seqIn) :
  CCheckGridCell(false),
  seqIndex(index), seqData(seqIn)
{
}


void UseAutoGridCell::UpdateText()
{
  bool isOK = seqData.IsValid(seqIndex);
  SetReadOnly(isOK);
  if (isOK)
    SetValue(seqData[seqIndex]->useInAutoSetup);
}

void UseAutoGridCell::OnCellSelected(CWnd* pParent, int nRow, int nCol)
{
  CCheckGridCell::OnCellSelected(pParent, nRow, nCol);
  if (seqData.IsValid(seqIndex))
    seqData[seqIndex]->useInAutoSetup = GetValue();
}


///////////////////////////////////////////////////////////////////////////////////

SC_StringArray  SequenceGridCtrl::seqTypeStrings;
SC_IntArray   SequenceGridCtrl::seqTypeMap;

SequenceGridCtrl::SequenceGridCtrl() :
  CGridCtrlCell()
{
}

void SequenceGridCtrl::InitGrid(SequenceArray& seqData, UnitReal& stTime, UnitReal& endTime, bool isTS)
{
  seqDataPtr = &seqData;

  // start with a clean slate
  sequence.SequenceCleanup();
  sequence.SetSequenceTimes(false);

  // make sure there are 20 blank sequences
  seqData.ReAlloc(seqData.Size() + 20);

  // set strings for type combos
  seqTypeStrings.Alloc(5);
  seqTypeMap.Alloc(5);

  // always valid
  seqTypeStrings += "Flow";
  seqTypeMap += Sequence::stFlow;
  seqTypeStrings += "History";
  seqTypeMap += Sequence::stHistory;
  seqTypeStrings += "Pulse";
  seqTypeMap += Sequence::stPulse;
  // liquid only
  if (control.IsLiquid())
  {
    seqTypeStrings += "Slug";
    seqTypeMap += Sequence::stSlug;
  }
  if (control.IsLayered())
  {
    seqTypeStrings += "Packer";
    seqTypeMap += Sequence::stPacker;
  }


  // set-up grid based on flags
  SetRowCount(seqData.AllocSize() + 1);

  int maxCol;
  if (enterSequenceDurations)
    maxCol = 8;
  else
    maxCol = 7;
  SetColCount(maxCol);

  SetCenteredText(0, 1, "Type" );
  SetCenteredText(0, 2, "Designation" );

  CString colHeader;
  CString unitStr = testStartTime.GetUnitString();
  colHeader = "StartTime [" + unitStr + "]";

  if (enterSequenceDurations)
  {
    SetCenteredText(0, 3, "Duration" );
    SetCenteredText(0, 4, "Units" );
    SetCenteredText(0, 5, "Sequence Data" );
    SetCenteredText(0, 6, colHeader);
    SetCenteredText(0, 7, "Auto?");
  }
  else
  {
    SetCenteredText(0, 3, colHeader);
    SetCenteredText(0, 4, "Sequence Data" );
    colHeader = "Duration [" + unitStr + "]";
    SetCenteredText(0, 5, colHeader);
    SetCenteredText(0, 6, "Auto?");
  }


  CString sRowHeader;
  for (int i = 0; i < seqData.AllocSize(); i++) {

    int row = i + 1;
    sRowHeader.Format("%d", row);
    SetCenteredText(row, 0, sRowHeader );

    SetCell(row, 1, new SeqTypeGridCell(i, seqData));
    SetCell(row, 2, new DesigGridCell(i, seqData));

    if (enterSequenceDurations)
    {
      SetCell(row, 3, new TimeValueGridCell(i, seqData));
      SetCell(row, 4, new TimeUnitsGridCell(i, seqData));
      SetCell(row, 5, new SeqDataGridCell(i, seqData, isTS));
      SetCell(row, 6, new StartTimeValueGridCell(i, seqData, stTime, endTime));
      SetCell(row, 7, new UseAutoGridCell(i, seqData));
    }
    else
    {
      SetCell(row, 3, new StartTimeValueGridCell(i, seqData, stTime, endTime));
      SetCell(row, 4, new SeqDataGridCell(i, seqData, isTS));
      SetCell(row, 5, new TimeValueGridCell(i, seqData));
      SetCell(row, 6, new UseAutoGridCell(i, seqData));
    }

    // last is readonly;
    GetCell(row, maxCol - 1)->SetReadOnly();

    // reset all data
    for (int j = 1; j < maxCol; j++)
      ResetCell(i + 1, j);
  }

  SetEditMode();
  SetBorderThickness(4);
  AutoSize();
  for (int j = 1; j < maxCol; j++)
    SetColSize(j, GetColSize(j) + 15);

  InvalidateAll();
}


BEGIN_MESSAGE_MAP(SequenceGridCtrl,CGridCtrlCell)
  //{{AFX_MSG_MAP(SequenceGridCtrl)
  ON_WM_RBUTTONDOWN()
  ON_WM_KEYDOWN()
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(WM_SEQM_INS_BEFORE, WM_SEQM_DELETE, OnPopupCommand)
END_MESSAGE_MAP()


void SequenceGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
  CGridCellIndices currCell = GetCellIndices(point);
  int currRow = currCell.m_nRow;
  if ((currRow <= 0) || (currRow > seqDataPtr->Size()))
    return;

  m_currSeq = currRow - 1;

  CMenu popMenu;
  popMenu.CreatePopupMenu();

  UINT insFlags = MF_STRING;
  if (seqDataPtr->IsNotValid(m_currSeq))
    insFlags = MF_STRING | MF_GRAYED;

  UINT dupFlags = insFlags;
  if (!enterSequenceDurations)
    dupFlags = MF_STRING | MF_GRAYED;

  UINT delFlags = MF_STRING;
  if ((seqDataPtr->Size() == 1) && (m_currSeq == 0))
    delFlags = MF_STRING | MF_GRAYED;

  VERIFY(popMenu.AppendMenu(insFlags, WM_SEQM_INS_BEFORE, "Insert Before" ));
  VERIFY(popMenu.AppendMenu(insFlags, WM_SEQM_INS_AFTER,  "Insert After" ));
  VERIFY(popMenu.AppendMenu(dupFlags, WM_SEQM_DUPLICATE,  "Duplicate" ));
  VERIFY(popMenu.AppendMenu(delFlags, WM_SEQM_DELETE,   "Delete" ));

  ClientToScreen(&point);
  popMenu.TrackPopupMenu( TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this );

}


void SequenceGridCtrl::DeleteSequence(int index)
{
  seqDataPtr->DeletePtr(index);
}

void SequenceGridCtrl::InsertSequence(int index)
{
  if (!seqDataPtr->InsertPtr(index))
    return;

  seqDataPtr->CheckAndAllocSequence(index);

  if (enterSequenceDurations)
    return;

  // split durations with adjacent sequence


  int halfRef;
  //  valid seq before ??
  if ((index > 0) && (seqDataPtr->IsValid(index - 1)))
  {
    // take before
    halfRef = index - 1;
  }
  // after -- should always be valid
  else if (seqDataPtr->IsValid(index + 1))
  {
    // adjust
    halfRef = index + 1;
    index++;
  }
  else
  {
    // give up
    return;
  }

  double halfDur = seqDataPtr->GetRef(halfRef).seqLength.GetMetricVal();
  if (!RealIsNull(halfDur))
  {
    halfDur /= 2.0;
    seqDataPtr->GetRef(index - 1).seqLength.SetMetricVal(halfDur);
    seqDataPtr->GetRef(index).seqLength.SetMetricVal(halfDur);
  }

}


void SequenceGridCtrl::UpdateModifiedRows()
{
  sequence.SetSequenceTimes(false);
  seqDataPtr->SetSequenceIDs();
  for (int i = m_currSeq + 1; i < GetRowCount(); i++)
  {
    for (int j = 1; j < GetColCount(); j++)
    {
      CGridCell* pCell = GetCell(i, j);
      pCell->UpdateText();
    }
  }
  InvalidateAll();
}

void SequenceGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // is it a valid sequence row ?
  m_currSeq = m_cellActive.m_nRow - 1;
  if (m_currSeq >= 0)
  {
    bool opDone = false;
    switch (nChar) {
    case VK_DELETE: {
      // cannot delete only row
      if ((seqDataPtr->Size() > 1) || (m_currSeq > 0))
      {
        DeleteSequence(m_currSeq);
        opDone = true;
      }
      break;
    }
    case VK_INSERT: {
      // must be valid
      if (seqDataPtr->IsValid(m_currSeq))
      {
        InsertSequence(m_currSeq);
        // update can effect the row before
        if (m_currSeq > 0)
          m_currSeq--;
        opDone = true;
      }
      break;
    }
    default :
      ;
    }

    if (opDone)
    {
      UpdateModifiedRows();
      return;
    }

  }

  CGridCtrlCell::OnKeyDown(nChar, nRepCnt, nFlags);
}


void SequenceGridCtrl::OnPopupCommand(UINT nMenuID)
{
  switch (nMenuID)
  {
  case WM_SEQM_INS_BEFORE : {
    InsertSequence(m_currSeq);
    if (m_currSeq > 0)
      m_currSeq--;
    break;
  }
  case WM_SEQM_INS_AFTER : {
    InsertSequence(m_currSeq + 1);
    break;
  }
  case WM_SEQM_DUPLICATE : {
    if (seqDataPtr->InsertPtr(m_currSeq))
    {
      seqDataPtr->tListData[m_currSeq] = new Sequence(*seqDataPtr->tListData[m_currSeq + 1]);
    }
    break;
  }
  case WM_SEQM_DELETE : {
    DeleteSequence(m_currSeq);
    break;
  }
  default: {
    ASSERT(0);
  }
  }

  UpdateModifiedRows();

}


void SequenceGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
  int seqIndex = m_cellActive.m_nRow - 1;

  bool updateAll = (seqDataPtr->IsNotValid(seqIndex));
  bool updateStatus = false;

  seqDataPtr->CheckAndAllocSequence(seqIndex);
  Sequence& currSeq = seqDataPtr->GetRef(seqIndex);

  // Change values in Grid
  CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
  switch (m_cellActive.m_nCol) {
  case 1: // Type
  {
    static_cast<SeqTypeGridCell*>(pCell)->SetSequenceType(nValue);
    updateStatus = true;
  }
  break;
  case 2: // ID
  {
    if (!SameString(currSeq.seqID, szText))
      currSeq.makeDefaultID = false;
    currSeq.SetID(szText);
  }
  break;
  case 3: // Duration or start val
  {
    if (enterSequenceDurations)
    {
      updateAll =  static_cast<TimeValueGridCell*>(pCell)->SetDuration(szText) || updateAll;
    }
    else
    {
      updateAll = static_cast<StartTimeValueGridCell*>(pCell)->SetStartTime(szText) || updateAll;
    }
  }
  break;
  case 4: // Units or Seq data
  {
    if (enterSequenceDurations)
    {
      currSeq.seqLength.SetConvIndex(nValue);
      CGridCell* pValueCell = GetCell(m_cellActive.m_nRow, 3);
      if (!updateAll)
      {
        pValueCell->UpdateText();

        // changes value text
        CGridCellIndices cell(m_cellActive.m_nRow, 3);
        InvalidateCellRect(CGridCellRange(cell, cell));
      }
    }
  }
  break;
  case 5: // seq data or duration status
  {
  }
  break;
  case 6: // start time or auto
  {
  }
  break;
  case 7: // auto
  {
  }
  break;
  }


  if (updateAll)
  {
    sequence.SetSequenceTimes(false);

    // the row
    for (int i = 1; i < GetColCount(); i++)
      ResetCell(m_cellActive.m_nRow, i);

    // the time status col
    int lastCol = GetColCount() - 1;
    for (int i = 1; i < GetRowCount(); i++)
      ResetCell(i, lastCol);

    // special case for previous row status
    if (m_cellActive.m_nRow > 1)
      ResetCell(m_cellActive.m_nRow - 1, lastCol - 1);

  }
  else
  {
    // current cell
    ResetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
    if (updateStatus)
    {
      if (seqDataPtr->SetSequenceIDs())
      {
        for (int i = m_cellActive.m_nRow; i < GetRowCount(); i++)
          ResetCell(i, 2);
      }

      if (enterSequenceDurations)
        ResetCell(m_cellActive.m_nRow, 5);
      else
        ResetCell(m_cellActive.m_nRow, 4);
    }

  }


  StopEdit(false);

  // if the right key continue editing on next cell
  switch (nLastChar) {
  case VK_TAB:
  case VK_DOWN:
  case VK_UP:
  case VK_RIGHT:
  case VK_LEFT:
  case VK_NEXT:
  case VK_PRIOR:
  case VK_HOME:
  case VK_END:
    OnKeyDown(nLastChar, 0, 0);
    UpdateWindow();
    if (m_cellActive != m_cellActiveOld)
      StartEdit(nLastChar);
    break;
  }
}

BOOL DlgSequenceArray::OnInitDialog()
{
  CGenericDialog::OnInitDialog();

  m_grid.SubclassDlgItem(IDC_GRID, this);

  SetControlInfo(IDC_GRID, resizeBoth);

  CGenericDialog::OnInitDialog();

  DlgNodeSequenceArray& currTab = *(static_cast<DlgNodeSequenceArray*>(GetInterfaceNode()));

  m_grid.InitGrid(currTab.m_seqData, currTab.m_startTime, currTab.m_endTime, currTab.m_TSZone);

  return TRUE;
}

