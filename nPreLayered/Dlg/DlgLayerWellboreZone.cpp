///////////////////////////////////////////////////////////////////////////////////
//
// DlgLayerWellboreZone.cpp
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


// DlgLayerWellboreZone.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <genClass/U_Str.h>

#include <nsClass/nSightConst.h>
#include <Var/G_Layer.h>
#include <Var/G_Control.h>

#include "PopupCmdDefines.h"

#include "DlgLayerWellboreZone.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgLayerWellboreZone dialog

IMPLEMENT_DYNCREATE(DlgLayerWellboreZone, CGenericDialog)

DlgLayerWellboreZone::DlgLayerWellboreZone(CWnd* pParent /*=NULL*/) :
  CGenericDialog(DlgLayerWellboreZone::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgLayerWellboreZone)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

DlgLayerWellboreZone::~DlgLayerWellboreZone()
{
}

void DlgLayerWellboreZone::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgLayerWellboreZone)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgLayerWellboreZone, CGenericDialog)
  //{{AFX_MSG_MAP(DlgLayerWellboreZone)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgLayerWellboreZone message handlers

#include <genGrid/TextGridCell.h>
#include <genGrid/CheckGridCell.h>
#include <genUnits/UnitGridCell.h>


//    Designation

class WellboreZoneIDGridCell : public CTextGridCell
{
  friend class WellboreZoneGridCtrl;
public:
  WellboreZoneIDGridCell(int index);

  virtual void  UpdateText();

protected:
  const int wbzIndex;
};

WellboreZoneIDGridCell::WellboreZoneIDGridCell(int index) :
  CTextGridCell(NULL, alignLeft),
  wbzIndex(index)
{
}


void WellboreZoneIDGridCell::UpdateText()
{
  if (!wellBoreZones[wbzIndex].IsSet())
    SetText(nullStr);
  else
    SetText(wellBoreZones[wbzIndex].intervalID);
}


//    top depth elevation

class WBZDepthElevValueGridCell : public CTextGridCell
{
  friend class WellboreZoneGridCtrl;
public:
  WBZDepthElevValueGridCell(int index);

  virtual void  FocusIn();
  virtual void  UpdateText();
  bool      SetDepthElev(const char* szText);

protected:
  const int wbzIndex;
};


WBZDepthElevValueGridCell::WBZDepthElevValueGridCell(int index) :
  CTextGridCell(NULL, alignRight),
  wbzIndex(index)
{
}

void WBZDepthElevValueGridCell::FocusIn()
{
  UnitIndex temp(uDist);
  if (enterLayerDepths)
  {
    temp = wellBoreZones[wbzIndex].topDepth;
    temp.SetRangeMsg(0.0, 1.0E+5);
  }
  else
  {
    temp = wellBoreZones[wbzIndex].topElevation;
    temp.SetRangeMsg(-1.0E+04, 1.0E+04);
  }
}


bool WBZDepthElevValueGridCell::SetDepthElev(const char* szText)
{
  WellBoreZone& currZone = wellBoreZones[wbzIndex];
  bool isOK;
  if (enterLayerDepths)
    isOK =  currZone.topDepth.IsWithinRange(szText, 0.0, 1.0E+5);
  else
    isOK =  currZone.topElevation.IsWithinRange(szText, -1.0E+04, 1.0E+04);

  if (isOK && (!currZone.IsSet()))
    currZone.SetID("New");

  return isOK;
}


void WBZDepthElevValueGridCell::UpdateText()
{
  WellBoreZone& currZone = wellBoreZones[wbzIndex];
  if (currZone.IsSet())
  {
    char tempStr[80];
    currZone.GetUserDepthString(tempStr, 80);
    SetText(tempStr);
  }
  else
    SetText(nullStr);
}

class WBZnNodeGridCell : public CTextGridCell
{
  friend class WellboreZoneGridCtrl;
public:
  WBZnNodeGridCell(int index);

  virtual void  FocusIn();
  virtual void  UpdateText();
  bool      SetNNode(const char* szText);

protected:
  const int wbzIndex;
};


WBZnNodeGridCell::WBZnNodeGridCell(int index) :
  CTextGridCell(NULL, alignRight),
  wbzIndex(index)
{
}

void WBZnNodeGridCell::FocusIn()
{
  UnitReal temp;
  temp.SetRangeMsg(2.0, 500.0);
}


bool WBZnNodeGridCell::SetNNode(const char* szText)
{
  WellBoreZone& currZone = wellBoreZones[wbzIndex];

  int nnode = -1;
  bool isOK = IsValidInt(szText, nnode) && (nnode > 1) && (nnode < 500);
  if (isOK)
  {
    if (!currZone.IsSet())
      currZone.SetID("New");
    currZone.nintervalNodes = nnode;
  }
  return isOK;
}


void WBZnNodeGridCell::UpdateText()
{
  WellBoreZone& currZone = wellBoreZones[wbzIndex];
  if (currZone.IsSet())
  {
    char tempStr[80];
    IntToString(currZone.nintervalNodes, tempStr, 80);
    SetText(tempStr);
  }
  else
    SetText(nullStr);
}


class WBZDeltaVolumeGridCell : public CTextGridCell
{
  friend class WellboreZoneGridCtrl;
public:
  WBZDeltaVolumeGridCell(int index);

  virtual void  FocusIn();
  virtual void  UpdateText();
  bool      SetDeltaVolume(const char* szText);

protected:
  const int wbzIndex;
};


WBZDeltaVolumeGridCell::WBZDeltaVolumeGridCell(int index) :
  CTextGridCell(NULL, alignRight),
  wbzIndex(index)
{
}

void WBZDeltaVolumeGridCell::FocusIn()
{
  UnitReal temp(wellBoreZones[wbzIndex].deltaVolume);
  temp.SetConvIndex(wellboreZoneDeltaVolumeUnits.GetConvIndex());
  temp.SetRangeMsg(-1000.0, 1000.0);
}


bool WBZDeltaVolumeGridCell::SetDeltaVolume(const char* szText)
{
  WellBoreZone& currZone = wellBoreZones[wbzIndex];
  if (currZone.zoneIsPacker)
    return false;
  bool isOK = currZone.deltaVolume.IsWithinRange(szText, -1.0E+03, 1.0E+03);
  if (isOK && (!currZone.IsSet()))
    currZone.SetID("New");
  return isOK;
}


void WBZDeltaVolumeGridCell::UpdateText()
{
  WellBoreZone& currZone = wellBoreZones[wbzIndex];
  if (currZone.IsSet())
  {
    if (currZone.zoneIsPacker)
    {
      SetText("n/a");
    }
    else
    {
      char tempStr[80];
      currZone.GetUserDeltaVolumeString(tempStr, 80);
      SetText(tempStr);
    }
    SetReadOnly(currZone.zoneIsPacker);
  }
  else
    SetText(nullStr);
}

class WBZTZCompGridCell : public CTextGridCell
{
  friend class WellboreZoneGridCtrl;
public:
  WBZTZCompGridCell(int index);

  virtual void  FocusIn();
  virtual void  UpdateText();
  bool      SetTZComp(const char* szText);

protected:
  const int wbzIndex;
};


WBZTZCompGridCell::WBZTZCompGridCell(int index) :
  CTextGridCell(NULL, alignRight),
  wbzIndex(index)
{
}

void WBZTZCompGridCell::FocusIn()
{
  UnitReal temp(wellBoreZones[wbzIndex].tzComp);
  temp.SetConvIndex(wellboreZoneTZCompUnits.GetConvIndex());
  temp.SetRangeMsg(1E-13, 1E-6);
}


bool WBZTZCompGridCell::SetTZComp(const char* szText)
{
  WellBoreZone& currZone = wellBoreZones[wbzIndex];
  if (currZone.zoneIsPacker)
    return false;
  bool isOK = currZone.tzComp.IsWithinRange(szText, 1E-13, 1E-6);
  if (isOK && (!currZone.IsSet()))
    currZone.SetID("New");
  return isOK;
}


void WBZTZCompGridCell::UpdateText()
{
  WellBoreZone& currZone = wellBoreZones[wbzIndex];
  if (currZone.IsSet())
  {
    if (currZone.zoneIsPacker)
    {
      SetText("n/a");
    }
    else
    {
      char tempStr[80];
      currZone.GetUserTZCompString(tempStr, 80);
      SetText(tempStr);
    }
    SetReadOnly(currZone.zoneIsPacker);
  }
  else
    SetText(nullStr);
}



class WBZThicknessGridCell : public CTextGridCell
{
  friend class WellboreZoneGridCtrl;
public:
  WBZThicknessGridCell(int index);

  virtual void  UpdateText();

protected:
  const int wbzIndex;
};



WBZThicknessGridCell::WBZThicknessGridCell(int index) :
  CTextGridCell(NULL, alignRight),
  wbzIndex(index)
{
}


void WBZThicknessGridCell::UpdateText()
{
  WellBoreZone& currZone = wellBoreZones[wbzIndex];
  SetReadOnly();
  if (currZone.IsSet())
  {
    char tempStr[80];
    currZone.GetUserThickString(tempStr, 80);
    SetText(tempStr);
  }
  else
    SetText(nullStr);
}

class IsPackerGridCell : public CCheckGridCell
{
  friend class WellboreZoneGridCtrl;
public:
  IsPackerGridCell(int index, WellboreZoneGridCtrl* assocGrid);

  virtual void  UpdateText();
  virtual void  OnCellSelected(CWnd* pParent, int nRow, int nCol);

protected:
  const int wbzIndex;
  WellboreZoneGridCtrl* grid;
};

IsPackerGridCell::IsPackerGridCell(int index, WellboreZoneGridCtrl* assocGrid) :
  CCheckGridCell(false),
  wbzIndex(index),
  grid(assocGrid)
{
}

void IsPackerGridCell::UpdateText()
{
  SetValue(wellBoreZones[wbzIndex].zoneIsPacker);
}

void IsPackerGridCell::OnCellSelected(CWnd* pParent, int nRow, int nCol)
{
  CCheckGridCell::OnCellSelected(pParent, nRow, nCol);
  wellBoreZones[wbzIndex].zoneIsPacker = GetValue();
  grid->GetCell(nRow, nCol + 2)->UpdateText();
  grid->GetCell(nRow, nCol + 3)->UpdateText();
  grid->InvalidateCell(nRow, nCol + 2);
  grid->InvalidateCell(nRow, nCol + 3);
}


WellboreZoneGridCtrl::WellboreZoneGridCtrl() :
  CGridCtrlCell()
{
}

void WellboreZoneGridCtrl::ResetWellboreZoneGrid()
{
  // start with a clean slate
  LayerStaticSupport::WellBoreZoneCleanup();

  // set-up grid based on flags
  SetRowCount(wellBoreZones.AllocSize() + 1);

  const int maxCol = 8;
  SetColCount(maxCol);

  CString colHeader;
  CString unitStr;

  SetCenteredText(0, 1, "ID" );
  if (enterLayerDepths)
  {
    unitStr = bottomLayerDepth.GetUnitString();
    colHeader = "Top Depth [";
  }
  else
  {
    unitStr = bottomLayerElevation.GetUnitString();
    colHeader = "Top Elevation [";
  }
  colHeader += unitStr + "]";
  SetCenteredText(0, 2, colHeader);
  SetCenteredText(0, 3, "Packer?");
  SetCenteredText(0, 4, "# nodes");

  colHeader = "Thickness [" + unitStr + "]";
  SetCenteredText(0, 7, colHeader);

  unitStr = wellboreZoneDeltaVolumeUnits.GetUnitString();
  colHeader = "Delta Volume ["+ unitStr +"]";
  SetCenteredText(0, 5, colHeader);

  unitStr = wellboreZoneTZCompUnits.GetUnitString();
  colHeader = "TZ Compressibility ["+ unitStr +"]";
  SetCenteredText(0, 6, colHeader);


  CString sRowHeader;
  for (int i = 0; i < wellBoreZones.AllocSize(); i++) {

    int row = i + 1;
    sRowHeader.Format("%d", row);
    SetCenteredText(row, 0, sRowHeader );

    SetCell(row, 1, new WellboreZoneIDGridCell(i));
    SetCell(row, 2, new WBZDepthElevValueGridCell(i));
    SetCell(row, 3, new IsPackerGridCell(i, this));
    SetCell(row, 4, new WBZnNodeGridCell(i));
    SetCell(row, 5, new WBZDeltaVolumeGridCell(i));
    SetCell(row, 6, new WBZTZCompGridCell(i));
    SetCell(row, 7, new WBZThicknessGridCell(i));

    for (int j = 1; j < maxCol; j++)
      ResetCell(row, j);
  }

  SetEditMode();
  SetBorderThickness(4);
  AutoSize();
  for (int j = 1; j < maxCol; j++)
    SetColSize(j, GetColSize(j) + 15);

  InvalidateAll();
}


BEGIN_MESSAGE_MAP(WellboreZoneGridCtrl,CGridCtrlCell)
  //{{AFX_MSG_MAP(WellboreZoneGridCtrl)
  ON_WM_RBUTTONDOWN()
  ON_WM_KEYDOWN()
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(WM_LAYERWELLBOREZONE_INS_BEFORE, WM_LAYERWELLBOREZONE_DELETE, OnPopupCommand)
END_MESSAGE_MAP()


void WellboreZoneGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
  CGridCellIndices currCell = GetCellIndices(point);
  int currRow = currCell.m_nRow;
  if ((currRow <= 0) || (currRow > wellBoreZones.AllocSize()))
    return;

  m_currZone = currRow - 1;

  CMenu popMenu;
  popMenu.CreatePopupMenu();

  UINT insFlags = MF_STRING;
  if (wellBoreZones.IsFull())
    insFlags = MF_STRING | MF_GRAYED;

  UINT delFlags = MF_STRING;

  VERIFY(popMenu.AppendMenu(insFlags, WM_LAYERWELLBOREZONE_INS_BEFORE,  "Insert Before" ));
  VERIFY(popMenu.AppendMenu(insFlags, WM_LAYERWELLBOREZONE_INS_AFTER, "Insert After" ));
  VERIFY(popMenu.AppendMenu(delFlags, WM_LAYERWELLBOREZONE_DELETE,    "Delete" ));

  ClientToScreen(&point);
  popMenu.TrackPopupMenu( TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this );

}


void WellboreZoneGridCtrl::DeleteWellboreZone(int index)
{
  layerSpec.DeleteWellBoreZone(index);
}

void WellboreZoneGridCtrl::InsertWellboreZone(int index)
{
  layerSpec.InsertWellBoreZone(index);
}


void WellboreZoneGridCtrl::UpdateModifiedRows()
{
  for (int i = 1; i < GetRowCount(); i++)
  {
    for (int j = 1; j < GetColCount(); j++)
    {
      CGridCell* pCell = GetCell(i, j);
      pCell->UpdateText();
    }
  }
  InvalidateAll();
}

void WellboreZoneGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // is it a valid WellboreZone row ?
  m_currZone = m_cellActive.m_nRow - 1;
  if (m_currZone >= 0)
  {
    bool opDone = false;
    switch (nChar) {
    case VK_DELETE: {
      DeleteWellboreZone(m_currZone);
      opDone = true;
      break;
    }
    case VK_INSERT: {
      // must be valid
      InsertWellboreZone(m_currZone);
      opDone = true;
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


void WellboreZoneGridCtrl::OnPopupCommand(UINT nMenuID)
{
  switch (nMenuID)
  {
  case WM_LAYERWELLBOREZONE_INS_BEFORE : {
    InsertWellboreZone(m_currZone);
    break;
  }
  case WM_LAYERWELLBOREZONE_INS_AFTER : {
    InsertWellboreZone(m_currZone + 1);
    break;
  }
  case WM_LAYERWELLBOREZONE_DELETE : {
    DeleteWellboreZone(m_currZone);
    break;
  }
  default: {
    ASSERT(0);
  }
  }
  UpdateModifiedRows();
}


void WellboreZoneGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
  int wbzIndex = m_cellActive.m_nRow - 1;

  WellBoreZone& currZone = wellBoreZones[wbzIndex];

  // Change values in Grid
  CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
  bool setIfNew = false;
  switch (m_cellActive.m_nCol) {
  case 1: // ID
  {
    currZone.SetID(szText);
  }
  break;
  case 2: // Duration or start val
  {
    static_cast<WBZDepthElevValueGridCell*>(pCell)->SetDepthElev(szText);
    setIfNew = true;
  }
  break;
  case 3: // packer
  {
    // update dVol text
    ResetCell(m_cellActive.m_nRow, 4);
  }
  break;
  case 4: // nnode
  {
    static_cast<WBZnNodeGridCell*>(pCell)->SetNNode(szText);
    setIfNew = true;
  }
  break;
  case 5: // delta volume
  {
    static_cast<WBZDeltaVolumeGridCell*>(pCell)->SetDeltaVolume(szText);
    setIfNew = true;
  }
  case 6: // TZ comp
  {
    static_cast<WBZTZCompGridCell*>(pCell)->SetTZComp(szText);
    setIfNew = true;
  }
  break;
  }

  if (setIfNew && (!currZone.IsSet()))
    currZone.SetID("New");


  LayerStaticSupport::WellBoreZoneCleanup();
  UpdateModifiedRows();

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

BOOL DlgLayerWellboreZone::OnInitDialog()
{
  CGenericDialog::OnInitDialog();

  m_grid.SubclassDlgItem(IDC_GRID, this);

  SetControlInfo(IDC_GRID, resizeBoth);

  return TRUE;

}

void DlgLayerWellboreZone::ResetAllControls()
{
  m_grid.ResetWellboreZoneGrid();
}
