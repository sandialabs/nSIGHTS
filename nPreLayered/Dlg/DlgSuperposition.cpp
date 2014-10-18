///////////////////////////////////////////////////////////////////////////////////
//
// DlgSuperposition.cpp
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


// DlgSuperposition.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include "DlgSuperposition.h"
#include "PopupCmdDefines.h"


#include <genGrid/TextGridCell.h>
#include <genGrid/ComboGridCell.h>
#include <genUnits/UnitGridCell.h>
#include "SampVarUnitsDlg.h"
#include "OptVarUnitsDlg.h"
#include "VaryVarUnitsDlg.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <Var/G_Control.h>
#include <Var/G_DataCapture.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSuperposition dialog

IMPLEMENT_DYNCREATE(DlgSuperposition,CGenericDialog)

DlgSuperposition::DlgSuperposition(CWnd* pParent /*=NULL*/) :
  CGenericDialog(DlgSuperposition::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgSuperposition)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void DlgSuperposition::DoDataExchange(CDataExchange* pDX)
{
  CGenericDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgSuperposition)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSuperposition, CGenericDialog)
  //{{AFX_MSG_MAP(DlgSuperposition)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgSuperposition message handlers



///////////////////////////////////////////////////////////////////////////////////

// namespace required to avoid linking in wrong ValueGridCell .....
namespace superpositionGrid {


class SuperpositionGridCell  {

public:
  SuperpositionGridCell(int index, SuperpositionGridCtrl& grid);
  ~SuperpositionGridCell() {};

  void            SetComponentValid();
  SuperComponent& GetComponent();
  bool            ComponentIsValid();

  bool            SetNullText(CGridCell* pCell);


protected:
  const int               m_index;
  SuperpositionGridCtrl&  m_grid;
};

SuperpositionGridCell::SuperpositionGridCell(int index, SuperpositionGridCtrl& grid) :
  m_index(index), m_grid(grid)
{
}


void SuperpositionGridCell::SetComponentValid()
{
  m_grid.SetComponentValid(m_index);
}

SuperComponent& SuperpositionGridCell::GetComponent()
{
  return m_grid.GetComponent(m_index);
}


bool SuperpositionGridCell::ComponentIsValid()
{
  return m_grid.ComponentIsValid(m_index);
}

bool SuperpositionGridCell::SetNullText(CGridCell* pCell)
{
  if (!ComponentIsValid())
  {
    pCell->SetText("---");
    return true;
  }
  return false;
}


class TypeGridCell : public CComboGridCell, public SuperpositionGridCell {

public:
  TypeGridCell(int index, SuperpositionGridCtrl& grid, const char* szText = NULL, Alignment alignment = alignLeft);
  TypeGridCell();

  virtual void  UpdateText();
  void            SetSuperType(int nValue);

protected:
  virtual void  GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);

private:
  void            SetSuperTypeStrings();

protected:
  SC_IntArray      m_superTypeMap;
  SC_StringArray   m_superTypeStrings;
};

TypeGridCell::TypeGridCell(int index, SuperpositionGridCtrl& grid, const char* szText, Alignment alignment) :
  CComboGridCell(szText, alignment), SuperpositionGridCell(index, grid)
{
  m_superTypeStrings.Alloc(3);
  m_superTypeMap.Alloc(3);

  m_superTypeStrings += "Constant";
  m_superTypeMap += SuperComponent::stConstant;

  if (control.IsOptimization())
  {
    m_superTypeStrings += "Optimize";
    m_superTypeMap += SuperComponent::stOptimized;
  }

  if (control.IsSampleSim())
  {
    m_superTypeStrings += "Sampled";
    m_superTypeMap += SuperComponent::stSampled;
  }
  else
  {
    m_superTypeStrings += "Suite/Range";
    m_superTypeMap += SuperComponent::stVary;
  }

}

void TypeGridCell::UpdateText()
{
  if (!SetNullText(this))
    SetEnumText(int(GetComponent().superType), m_superTypeStrings, m_superTypeMap);
}


void TypeGridCell::SetSuperType(int nValue)
{
  if (ComponentIsValid() && (nValue >=0))
  {
    GetComponent().ChangeSuperType(SuperComponent::SuperType(m_superTypeMap[nValue]));
    GetComponent().ClearWasActive();
  }
}

void TypeGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
  SetEnumSelectionList(aText, aEnable, m_superTypeStrings);
}

///////////////////////////////////////////////////////////////////////////////////
class ValueGridCell : public CTextGridCell, public SuperpositionGridCell {
public:
  ValueGridCell(int index, SuperpositionGridCtrl& grid, const char* szText = NULL, Alignment alignment = alignRight);

  virtual void    UpdateText();

protected:
  virtual CWnd* CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar);
  virtual CSize GetExtent(CDC* pDC) const;

};

ValueGridCell::ValueGridCell(int index, SuperpositionGridCtrl& grid, const char* szText, Alignment alignment) :
  CTextGridCell(szText, alignment), SuperpositionGridCell(index, grid)
{
}

CSize ValueGridCell::GetExtent(CDC* pDC) const
{
  CSize sz = GetNewlineTextExtent(pDC, m_sText);
  sz.cx += 30;
  return sz;
}

void ValueGridCell::UpdateText()
{
  if (SetNullText(this))
    return;

  SuperComponent& currComp = GetComponent();


  SC_SetupErr errData;
  // default values
  if (!currComp.SetupOK(errData))
  {
    SetText("BAD");
    return;
  }

  char tempStr[40];
  switch (currComp.superType) {
  case SuperComponent::stConstant:
  {
    currComp.superRadUnits.GetUserString(currComp.constantRad, tempStr, 40);
    break;
  }
  case SuperComponent::stOptimized: {
    currComp.optRadVal.GetMenuStr(tempStr, 40);
    break;
  }
  case SuperComponent::stSampled: {
    currComp.sampRadVal.GetMenuStr(tempStr, 40);
    break;
  }
  case SuperComponent::stVary: {
    currComp.varyRadVal.GetMenuStr(tempStr, 40);
    break;
  }
  }

  SetText(tempStr);
}



CWnd* ValueGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
  SuperComponent& currComp = GetComponent();

  static const double minRad = 1.0E-4;
  static const double maxRad = 1.0E+6;

  switch (currComp.superType) {
  case SuperComponent::stConstant:  {
    CString loLim, upLim;
    currComp.superRadUnits.GetUserString(minRad, loLim);
    currComp.superRadUnits.GetUserString(maxRad, upLim);
    GenAppParameterRangeMsg(loLim, upLim);
    return CTextGridCell::CreateEditWnd(rect, pParent, nID, nRow, nCol, nFirstChar);
  }
  case SuperComponent::stOptimized: {
    COptVarUnitsDlg dlg(currComp.optRadVal, 0, minRad, maxRad);
    dlg.DoModal();
    break;
  }
  case SuperComponent::stSampled: {
    CSampVarUnitsDlg dlg(currComp.sampRadVal, 0, minRad, maxRad);
    dlg.DoModal();
    break;
  }
  case SuperComponent::stVary: {
    VaryVarUnitsDlg dlg(currComp.varyRadVal, 0, minRad, maxRad);
    break;
  }
  }

  pParent->SetFocus();
  static_cast<CGridCtrlCell*>(pParent)->FinishEdit(NULL, -1, nFirstChar);
  return NULL;
}


class OpGridCell : public CComboGridCell, public SuperpositionGridCell {
public:
  OpGridCell(int index, SuperpositionGridCtrl& grid, const char* szText = NULL, Alignment alignment = alignRight);
  OpGridCell();

  virtual void  UpdateText();
  void            SetSuperOp(int nValue);

protected:
  virtual void  GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);

protected:
  SC_IntArray      m_superOpMap;
  SC_StringArray   m_superOpStrings;
};

OpGridCell::OpGridCell(int index, SuperpositionGridCtrl& grid, const char* szText, Alignment alignment) :
  CComboGridCell(szText, alignment), SuperpositionGridCell(index, grid)
{
  m_superOpStrings.Alloc(4);
  m_superOpMap.Alloc(4);

  m_superOpStrings += "+ Pressure";
  m_superOpMap += SuperComponent::soAddP;

  m_superOpStrings += "- Pressure";
  m_superOpMap += SuperComponent::soSubP;

  m_superOpStrings += "+ dPressure";
  m_superOpMap += SuperComponent::soAddDP;

  m_superOpStrings += "- dPressure";
  m_superOpMap += SuperComponent::soSubDP;

}

void OpGridCell::UpdateText()
{
  if (!SetNullText(this))
    SetEnumText(int(GetComponent().superOp), m_superOpStrings, m_superOpMap);
}

void OpGridCell::SetSuperOp(int nValue)
{
  if (ComponentIsValid() && (nValue >= 0))
    GetComponent().superOp = SuperComponent::SuperOp(m_superOpMap[nValue]);
}

void OpGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
  SetEnumSelectionList(aText, aEnable, m_superOpStrings);
}


} // namespace

using namespace superpositionGrid;


///////////////////////////////////////////////////////////////////////////////////
SuperpositionGridCtrl::SuperpositionGridCtrl() :
  CGridCtrlCell()
{
}

BEGIN_MESSAGE_MAP(SuperpositionGridCtrl,CGridCtrlCell)
  //{{AFX_MSG_MAP(SuperpositionGridCtrl)
  ON_WM_RBUTTONDOWN()
  ON_WM_KEYDOWN()
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(WM_DCAPSUP_INS_BEFORE, WM_DCAPSUP_DELETE, OnPopupCommand)
END_MESSAGE_MAP()


void SuperpositionGridCtrl::SetComponentValid(int index)
{
  if (!m_psuperData->IsValid(index))
  {
    m_psuperData->MakeNew(index);
    if (index >= m_psuperData->Size())
      m_psuperData->SetSize(index + 1);
  }
}

SuperComponent& SuperpositionGridCtrl::GetComponent(int index)
{
  SetComponentValid(index);
  return m_psuperData->GetRef(index);
}

bool SuperpositionGridCtrl::ComponentIsValid(int index)
{
  return m_psuperData->IsValid(index);
}


void SuperpositionGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
  // Change values in Grid

  int indx = m_cellActive.m_nRow - 1;

  SuperComponent&  currComp = GetComponent(indx);


  CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
  switch (m_cellActive.m_nCol) {
  case 1: {
    static_cast<TypeGridCell*>(pCell)->SetSuperType(nValue);
    break;
  }
  case 2: {
    if (currComp.superType == SuperComponent::stConstant)
    {
      UnitReal temp(currComp.superRadUnits, currComp.constantRad);
      if (temp.IsWithinRange(szText, 1.0E-4, 1.0E+06))
        currComp.constantRad = temp.GetMetricVal();
    }
    break;
  }
  case 3: {
    static_cast<OpGridCell*>(pCell)->SetSuperOp(nValue);
    break;
  }
  }

  for (int j = 1; j < GetColCount(); j++)
  {
    pCell = GetCell(m_cellActive.m_nRow, j);
    pCell->UpdateText();
    InvalidateCell(m_cellActive.m_nRow, j);
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

void SuperpositionGridCtrl::InitGrid(SuperComponentArray* superData)
{
  static const int maxSuper = 16;
  m_psuperData = superData;

  m_psuperData->Cleanup();

  m_psuperData->ReAlloc(maxSuper);

  SetRowCount(maxSuper + 1);
  SetColCount(4);
  SetCenteredText(0, 1, "Type     ");
  SetCenteredText(0, 2, "Radius   ");
  SetCenteredText(0, 3, "Operation");

  CString sRowHeader;
  for (int i = 0; i < maxSuper; i++) {

    int row = i + 1;
    sRowHeader.Format("%d", row);
    SetCenteredText(row, 0, sRowHeader );

    SetCell(row, 1, new TypeGridCell(i, *this));
    SetCell(row, 2, new ValueGridCell(i, *this));
    SetCell(row, 3, new OpGridCell(i, *this));

    // reset all data
    for (int j = 0; j < 4; j++)
      ResetCell(row, j);

  }
  SetEditMode();

  SetBorderThickness(4);
  AutoSize();
  SetColSize(1, GetColSize(1) + 10);
  SetColSize(2, GetColSize(2) + 20);

  InvalidateAll();
}


void SuperpositionGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
  CGridCellIndices currCell = GetCellIndices(point);
  int currRow = currCell.m_nRow;
  if ((currRow <= 0) || (currRow > m_psuperData->Size()))
    return;

  m_currIndex = currRow - 1;

  CMenu popMenu;
  popMenu.CreatePopupMenu();

  UINT insFlags = MF_STRING;
  if (m_psuperData->IsNotValid(m_currIndex))
    insFlags = MF_STRING | MF_GRAYED;

  VERIFY(popMenu.AppendMenu(insFlags,     WM_DCAPSUP_INS_BEFORE,  "Insert Before" ));
  VERIFY(popMenu.AppendMenu(insFlags,     WM_DCAPSUP_INS_AFTER,   "Insert After" ));
  VERIFY(popMenu.AppendMenu(insFlags,     WM_DCAPSUP_DUPLICATE,   "Duplicate" ));
  VERIFY(popMenu.AppendMenu(MF_STRING,    WM_DCAPSUP_DELETE,      "Delete" ));

  ClientToScreen(&point);
  popMenu.TrackPopupMenu( TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this );
}


void SuperpositionGridCtrl::DeleteRow(int index)
{
  m_psuperData->DeletePtr(index);
}

void SuperpositionGridCtrl::InsertRow(int index)
{
  if (!m_psuperData->InsertPtr(index))
    return;
  SetComponentValid(index);
}


void SuperpositionGridCtrl::UpdateModifiedRows()
{
  for (int i = m_currIndex + 1; i < GetRowCount(); i++)
  {
    for (int j = 1; j < GetColCount(); j++)
    {
      CGridCell* pCell = GetCell(i, j);
      pCell->UpdateText();
    }
  }
  InvalidateAll();
}

void SuperpositionGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // is it a valid sequence row ?
  m_currIndex = m_cellActive.m_nRow - 1;
  if (m_currIndex >= 0)
  {
    bool opDone = false;
    switch (nChar) {
    case VK_DELETE: {
      DeleteRow(m_currIndex);
      break;
    }
    case VK_INSERT: {
      // must be valid
      if (m_psuperData->IsValid(m_currIndex))
      {
        InsertRow(m_currIndex);
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


void SuperpositionGridCtrl::OnPopupCommand(UINT nMenuID)
{
  switch (nMenuID)
  {
  case WM_DCAPSUP_INS_BEFORE : {
    InsertRow(m_currIndex);
    if (m_currIndex > 0)
      m_currIndex--;
    break;
  }
  case WM_DCAPSUP_INS_AFTER : {
    InsertRow(m_currIndex + 1);
    break;
  }
  case WM_DCAPSUP_DUPLICATE : {
    InsertRow(m_currIndex + 1);
    break;
  }
  case WM_DCAPSUP_DELETE : {
    DeleteRow(m_currIndex);
    break;
  }
  default: {
    ASSERT(0);
  }
  }

  UpdateModifiedRows();

}


BOOL DlgSuperposition::OnInitDialog()
{
  CGenericDialog::OnInitDialog();

  m_grid.SubclassDlgItem(IDC_GRID, this);

  SetControlInfo(IDC_GRID, resizeBoth);

  ResetAllControls();

  return TRUE;
}

void DlgSuperposition::ResetAllControls()
{
  DataCaptureSpec& superSpec = static_cast<DlgNodeSuperposition*>(GetInterfaceNode())->m_superData;

  m_grid.InitGrid(&superSpec.superData);
}

