///////////////////////////////////////////////////////////////////////////////////
//
// DlgLayerGeology.cpp
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


// DlgLayerGeology.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <genClass/U_Str.h>

#include <nsClass/nSightConst.h>
#include <Var/G_Layer.h>
#include <Var/G_Control.h>

#include "PopupCmdDefines.h"

#include "DlgLayerGeology.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgLayerGeology dialog

IMPLEMENT_DYNCREATE(DlgLayerGeology, CGenericDialog)

DlgLayerGeology::DlgLayerGeology(CWnd* pParent /*=NULL*/) :
  CGenericDialog(DlgLayerGeology::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgLayerGeology)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

DlgLayerGeology::~DlgLayerGeology()
{
}

void DlgLayerGeology::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgLayerGeology)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgLayerGeology, CGenericDialog)
  //{{AFX_MSG_MAP(DlgLayerGeology)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgLayerGeology message handlers

#include <genGrid/TextGridCell.h>
#include <genGrid/CheckGridCell.h>
#include <genUnits/UnitGridCell.h>


//    Designation

class GeoIDGridCell : public CTextGridCell
{
  friend class GeologyGridCtrl;
public:
  GeoIDGridCell(int index);

  virtual void  UpdateText();

protected:
  const int geoIndex;
};

GeoIDGridCell::GeoIDGridCell(int index) :
  CTextGridCell(NULL, alignLeft),
  geoIndex(index)
{
}


void GeoIDGridCell::UpdateText()
{
  if (geologyLayers[geoIndex].IsSet())
    SetText(geologyLayers[geoIndex].intervalID);
  else
    SetText(nullStr);
}


//    top depth elevation

class DepthElevValueGridCell : public CTextGridCell
{
  friend class GeologyGridCtrl;
public:
  DepthElevValueGridCell(int index);

  virtual void  FocusIn();
  virtual void  UpdateText();
  bool      SetDepthElev(const char* szText);

protected:
  const int geoIndex;
};


DepthElevValueGridCell::DepthElevValueGridCell(int index) :
  CTextGridCell(NULL, alignRight),
  geoIndex(index)
{
}

void DepthElevValueGridCell::FocusIn()
{
  UnitIndex temp(uDist);
  if (enterLayerDepths)
  {
    temp = geologyLayers[geoIndex].topDepth;
    temp.SetRangeMsg(0.0, 1.0E+5);
  }
  else
  {
    temp = geologyLayers[geoIndex].topElevation;
    temp.SetRangeMsg(-1.0E+04, 1.0E+04);
  }
}


bool DepthElevValueGridCell::SetDepthElev(const char* szText)
{
  GeologyLayer& currLayer = geologyLayers[geoIndex];
  bool isOK;
  if (enterLayerDepths)
    isOK =  currLayer.topDepth.IsWithinRange(szText, 0.0, 1.0E+5);
  else
    isOK =  currLayer.topElevation.IsWithinRange(szText, -1.0E+04, 1.0E+04);

  if (isOK && (!currLayer.IsSet()))
    currLayer.SetID("New");

  return isOK;
}


void DepthElevValueGridCell::UpdateText()
{
  GeologyLayer& currLayer = geologyLayers[geoIndex];
  if (currLayer.IsSet())
  {
    CString sText;
    if (enterLayerDepths)
    {
      currLayer.topDepth.GetUserString(sText);
    }
    else
    {
      currLayer.topElevation.GetUserString(sText);
    }
    SetText(sText);
  }
  else
    SetText(nullStr);
}



class GeolLayernNodeGridCell : public CTextGridCell
{
  friend class GeologyGridCtrl;
public:
  GeolLayernNodeGridCell(int index);

  virtual void  FocusIn();
  virtual void  UpdateText();
  bool      SetNNode(const char* szText);

protected:
  const int geoIndex;
};


GeolLayernNodeGridCell::GeolLayernNodeGridCell(int index) :
  CTextGridCell(NULL, alignRight),
  geoIndex(index)
{
}

void GeolLayernNodeGridCell::FocusIn()
{
  UnitReal temp;
  temp.SetRangeMsg(2.0, 500.0);
}


bool GeolLayernNodeGridCell::SetNNode(const char* szText)
{
  GeologyLayer& currLayer = geologyLayers[geoIndex];
  int nnode = -1;
  bool isOK = IsValidInt(szText, nnode) && (nnode > 1) && (nnode < 500);
  if (isOK)
  {
    if (!currLayer.IsSet())
      currLayer.SetID("New");
    currLayer.nintervalNodes = nnode;
  }
  return isOK;
}


void GeolLayernNodeGridCell::UpdateText()
{
  GeologyLayer& currLayer = geologyLayers[geoIndex];
  if (currLayer.IsSet())
  {
    char tempStr[80];
    IntToString(currLayer.nintervalNodes, tempStr, 80);
    SetText(tempStr);
  }
  else
    SetText(nullStr);
}

class ThicknessGridCell : public CTextGridCell
{
  friend class GeologyGridCtrl;
public:
  ThicknessGridCell(int index);

  virtual void  UpdateText();

protected:
  const int geoIndex;
};

ThicknessGridCell::ThicknessGridCell(int index) :
  CTextGridCell(NULL, alignRight),
  geoIndex(index)
{
}


void ThicknessGridCell::UpdateText()
{
  GeologyLayer& currLayer = geologyLayers[geoIndex];
  SetReadOnly();
  if (currLayer.IsSet())
  {
    CString sText;
    UnitReal temp;
    if (enterLayerDepths)
      temp = currLayer.topDepth;
    else
      temp = currLayer.topElevation;
    temp.SetMetricVal(currLayer.currThickness);
    temp.GetUserString(sText);
    SetText(sText);
  }
  else
    SetText(nullStr);
}

class HasSkinGridCell : public CCheckGridCell
{
  friend class GeologyGridCtrl;
public:
  HasSkinGridCell(int index);

  virtual void  UpdateText();
  virtual void  OnCellSelected(CWnd* pParent, int nRow, int nCol);

protected:
  const int geoIndex;
};

HasSkinGridCell::HasSkinGridCell(int index) :
  CCheckGridCell(false),
  geoIndex(index)
{
}

void HasSkinGridCell::UpdateText()
{
  SetValue(geologyLayers[geoIndex].layerHasSkin);
}

void HasSkinGridCell::OnCellSelected(CWnd* pParent, int nRow, int nCol)
{
  CCheckGridCell::OnCellSelected(pParent, nRow, nCol);
  geologyLayers[geoIndex].layerHasSkin = GetValue();
}

class IsIsotropicGridCell : public CCheckGridCell
{
  friend class GeologyGridCtrl;
public:
  IsIsotropicGridCell(int index);

  virtual void  UpdateText();
  virtual void  OnCellSelected(CWnd* pParent, int nRow, int nCol);

protected:
  const int geoIndex;
};

IsIsotropicGridCell::IsIsotropicGridCell(int index) :
  CCheckGridCell(false),
  geoIndex(index)
{
}


void IsIsotropicGridCell::UpdateText()
{
  SetValue(geologyLayers[geoIndex].layerIsIsotropic);
}

void IsIsotropicGridCell::OnCellSelected(CWnd* pParent, int nRow, int nCol)
{
  CCheckGridCell::OnCellSelected(pParent, nRow, nCol);
  geologyLayers[geoIndex].layerIsIsotropic = GetValue();
}


GeologyGridCtrl::GeologyGridCtrl() :
  CGridCtrlCell()
{
}

void GeologyGridCtrl::ResetGeologyGrid()
{
  // start with a clean slate
  layerSpec.GeoLayerCleanup();

  // set-up grid based on flags
  SetRowCount(geologyLayers.AllocSize() + 1);

  const int maxCol = 7;
  SetColCount(maxCol);

  SetCenteredText(0, 1, "ID" );
  if (enterLayerDepths)
    SetCenteredText(0, 2, "Top Depth" );
  else
    SetCenteredText(0, 2, "Top Elevation" );
  SetCenteredText(0, 3, "Has Skin?");
  SetCenteredText(0, 4, "Isotropic?");
  SetCenteredText(0, 5, "# nodes?");
  SetCenteredText(0, 6, "Thickness");

  CString sRowHeader;
  for (int i = 0; i < geologyLayers.AllocSize(); i++) {

    int row = i + 1;
    sRowHeader.Format("%d", row);
    SetCenteredText(row, 0, sRowHeader );

    SetCell(row, 1, new GeoIDGridCell(i));
    SetCell(row, 2, new DepthElevValueGridCell(i));
    SetCell(row, 3, new HasSkinGridCell(i));
    SetCell(row, 4, new IsIsotropicGridCell(i));
    SetCell(row, 5, new GeolLayernNodeGridCell(i));
    SetCell(row, 6, new ThicknessGridCell(i));

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


BEGIN_MESSAGE_MAP(GeologyGridCtrl,CGridCtrlCell)
  //{{AFX_MSG_MAP(GeologyGridCtrl)
  ON_WM_RBUTTONDOWN()
  ON_WM_KEYDOWN()
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(WM_LAYERGEOLOGY_INS_BEFORE, WM_LAYERGEOLOGY_DELETE, OnPopupCommand)
END_MESSAGE_MAP()


void GeologyGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
  CGridCellIndices currCell = GetCellIndices(point);
  int currRow = currCell.m_nRow;
  if ((currRow <= 0) || (currRow > geologyLayers.AllocSize()))
    return;

  m_currLayer = currRow - 1;

  CMenu popMenu;
  popMenu.CreatePopupMenu();

  UINT insFlags = MF_STRING;
  if (geologyLayers.IsFull())
    insFlags = MF_STRING | MF_GRAYED;

  UINT delFlags = MF_STRING;

  VERIFY(popMenu.AppendMenu(insFlags, WM_LAYERGEOLOGY_INS_BEFORE, "Insert Before" ));
  VERIFY(popMenu.AppendMenu(insFlags, WM_LAYERGEOLOGY_INS_AFTER,  "Insert After" ));
  VERIFY(popMenu.AppendMenu(delFlags, WM_LAYERGEOLOGY_DELETE,   "Delete" ));

  ClientToScreen(&point);
  popMenu.TrackPopupMenu( TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this );

}


void GeologyGridCtrl::DeleteGeology(int index)
{
  layerSpec.DeleteGeoLayer(index);
}

void GeologyGridCtrl::InsertGeology(int index)
{
  layerSpec.InsertGeoLayer(index);
}


void GeologyGridCtrl::UpdateModifiedRows()
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

void GeologyGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // is it a valid Geology row ?
  m_currLayer = m_cellActive.m_nRow - 1;
  if (m_currLayer >= 0)
  {
    bool opDone = false;
    switch (nChar) {
    case VK_DELETE: {
      DeleteGeology(m_currLayer);
      opDone = true;
      break;
    }
    case VK_INSERT: {
      // must be valid
      InsertGeology(m_currLayer);
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


void GeologyGridCtrl::OnPopupCommand(UINT nMenuID)
{
  switch (nMenuID)
  {
  case WM_LAYERGEOLOGY_INS_BEFORE : {
    InsertGeology(m_currLayer);
    break;
  }
  case WM_LAYERGEOLOGY_INS_AFTER : {
    InsertGeology(m_currLayer + 1);
    break;
  }
  case WM_LAYERGEOLOGY_DELETE : {
    DeleteGeology(m_currLayer);
    break;
  }
  default: {
    ASSERT(0);
  }
  }
  UpdateModifiedRows();
}


void GeologyGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
  int geoIndex = m_cellActive.m_nRow - 1;

  GeologyLayer& currLayer = geologyLayers[geoIndex];

  // Change values in Grid
  CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
  switch (m_cellActive.m_nCol) {
  case 1: // ID
  {
    currLayer.SetID(szText);
  }
  break;
  case 2: // Duration or start val
  {
    static_cast<DepthElevValueGridCell*>(pCell)->SetDepthElev(szText);
    if (!currLayer.IsSet())
      currLayer.SetID("New");
  }
  break;
  case 3: // skin
  {
  }
  break;
  case 4: // isotropic
  {
  }
  break;
  case 5: // nnode
  {
    static_cast<GeolLayernNodeGridCell*>(pCell)->SetNNode(szText);
    if (!currLayer.IsSet())
      currLayer.SetID("New");
  }
  break;
  case 6: // thickness
  {
  }
  break;
  }

  layerSpec.GeoLayerCleanup();
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

BOOL DlgLayerGeology::OnInitDialog()
{
  CGenericDialog::OnInitDialog();

  m_grid.SubclassDlgItem(IDC_GRID, this);

  SetControlInfo(IDC_GRID, resizeBoth);

  return TRUE;

}

void DlgLayerGeology::ResetAllControls()
{
  m_grid.ResetGeologyGrid();
}
