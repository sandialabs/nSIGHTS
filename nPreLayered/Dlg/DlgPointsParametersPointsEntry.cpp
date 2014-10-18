///////////////////////////////////////////////////////////////////////////////////
//
// DlgPointsParametersPointsEntry.cpp
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


// DlgPointsParametersPointsEntry.cpp : implementation file
//
#include "stdafx.h"

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genGrid/TextGridCell.h>
#include <genGrid/ComboGridCell.h>

#include <osMain/G_Vary.h>

#include <UI/NPreInterfaceApp.h>
#include <Var/G_Control.h>

#include "Resource.h"
#include "PopupCmdDefines.h"
#include "VaryVarUnitsDlg.h"
#include "DlgPointsParametersPointsEntry.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// namespace required to avoid linking in wrong ValueGridCell .....
namespace fprpointsParameter {

class PointsParameterGridCell  {

public:
  PointsParameterGridCell(int index, PointsParameterGridCtrl& grid);
  ~PointsParameterGridCell() {};

  void            SetPointsCurveXYValid();
  PointsCurveXY&  GetPointsCurveXY();
  bool            PointsCurveXYIsValid();

  bool            SetNullText(CGridCell* pCell);

protected:
  virtual bool    SetPtr() = 0;

protected:
  const int                   m_index;
  PointsParameterGridCtrl&    m_grid;


};

PointsParameterGridCell::PointsParameterGridCell(int index, PointsParameterGridCtrl& grid) :
  m_index(index), m_grid(grid)
{
}


void PointsParameterGridCell::SetPointsCurveXYValid()
{
  m_grid.SetPointsCurveXYValid(m_index);
}

PointsCurveXY& PointsParameterGridCell::GetPointsCurveXY()
{
  return m_grid.GetPointsCurveXY(m_index);
}


bool PointsParameterGridCell::PointsCurveXYIsValid()
{
  return m_grid.PointsCurveXYIsValid(m_index);
}


bool PointsParameterGridCell::SetNullText(CGridCell* pCell)
{
  if (!PointsCurveXYIsValid())
  {
    pCell->SetText("---");
    return true;
  }
  return false;
}


class TypeGridCell : public CComboGridCell, public PointsParameterGridCell
{
public:
  TypeGridCell(int index, bool isX, PointsParameterGridCtrl& grid);
  TypeGridCell();

  virtual void  UpdateText();
  void      SetPointType(int nValue);

protected:
  virtual void  GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);
  virtual bool    SetPtr();

private:
  void      SetPointTypeStrings();

protected:
  const bool      getX;
  PointsCurveValue* m_pointsCurveValue;
  SC_IntArray     m_pointTypeMap;
  SC_StringArray    m_pointTypeStrings;
};

TypeGridCell::TypeGridCell(int index, bool isX, PointsParameterGridCtrl& grid) :
  CComboGridCell(NULL, alignRight),
  PointsParameterGridCell(index, grid),
  getX(isX), m_pointsCurveValue(0)
{
  m_pointTypeStrings.Alloc(3);
  m_pointTypeMap.Alloc(3);
  SetPointTypeStrings();
}

void TypeGridCell::UpdateText()
{
  if (SetNullText(this))
    return;

  SetPtr();

  for (int i = 0; i < m_pointTypeStrings.Size(); i++)
    if (int(m_pointsCurveValue->pType) == m_pointTypeMap[i])
    {
      SetText(m_pointTypeStrings[i]);
      return;
    }
  SetText("???");

}

void TypeGridCell::SetPointTypeStrings()
{
  m_pointTypeStrings.SetEmpty();
  m_pointTypeMap.SetEmpty();

  // always valid
  m_pointTypeStrings += "Fixed";
  m_pointTypeMap += PointsCurveValue::pcvFixed;

  //  range or vary
  if (!control.IsSampleSim())
  {
    if (vary.IsRange())
      m_pointTypeStrings += "Range";
    else
      m_pointTypeStrings += "Suite";
    m_pointTypeMap += PointsCurveValue::pcvVary;
  }

  if (control.IsOptimization())
  {
    m_pointTypeStrings += "Optimize";
    m_pointTypeMap += PointsCurveValue::pcvOpt;
  }
}

void TypeGridCell::SetPointType(int nValue)
{
  if (!SetPtr())
    return;

  PointsCurveValue::PointsCurveValueType  newType = PointsCurveValue::PointsCurveValueType(m_pointTypeMap[nValue]);

  m_pointsCurveValue->pType = newType;
  if (m_pointsCurveValue->IsOpt())
    m_pointsCurveValue->InitOpt();
  else if (m_pointsCurveValue->IsVary())
    m_pointsCurveValue->InitVary();

  m_pointsCurveValue->SetActive(true);
  m_pointsCurveValue->ClearWasActive();

  m_grid.ResetRow(m_index + 1);  // overkill but easy
}

void TypeGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
  // update for possible vary changes
  SetPointTypeStrings();

  aText.RemoveAll();
  aEnable.RemoveAll();
  for (int i = 0; i < m_pointTypeStrings.Size(); i++)
  {
    aText.Add(m_pointTypeStrings[i]);
    aEnable.Add(true);
  }
}

bool TypeGridCell::SetPtr()
{
  PointsCurveXY& currXY = GetPointsCurveXY();
  if (getX)
  {
    m_pointsCurveValue = &currXY.xVal;
  }
  else
  {
    m_pointsCurveValue = &currXY.yVal;
  }
  return true;
}


class ValueGridCell : public CTextGridCell, public PointsParameterGridCell {


public:
  ValueGridCell(int index, PointsParameterGridCtrl& grid, double minL, double maxL, UnitIndex&  units);

  virtual void    UpdateText();
  virtual void    SetValue(CString inStr);

protected:
  virtual CWnd*   CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar);

protected:
  double*         m_fValue;
  UnitIndex&      m_units;
  double          minLimit;
  double          maxLimit;

};


ValueGridCell::ValueGridCell(int index, PointsParameterGridCtrl& grid, double minL, double maxL, UnitIndex&  units) :
  CTextGridCell("", alignRight),
  PointsParameterGridCell(index, grid),
  minLimit(minL), maxLimit(maxL), m_units(units), m_fValue(0)
{
}

CWnd* ValueGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
  if (!SetPtr())
    return NULL;

  CString loLim, upLim;
  m_units.GetUserString(minLimit, loLim);
  m_units.GetUserString(maxLimit, upLim);
  GenAppParameterRangeMsg(loLim, upLim);
  return CTextGridCell::CreateEditWnd(rect, pParent, nID, nRow, nCol, nFirstChar);
}


void ValueGridCell::SetValue(CString inStr)
{
  if (!PointsCurveXYIsValid())
    return;

  SetPtr();
  if (m_fValue == NULL)
    return;

  UnitReal temp(m_units, *m_fValue);
  if (temp.IsWithinRange(inStr, minLimit, maxLimit))
    *m_fValue = temp.GetMetricVal();

  UpdateText();
}



void ValueGridCell::UpdateText()
{
  if (SetNullText(this))
    return;

  CString tempStr;

  if (!SetPtr())
  {
    tempStr = "n/a";
  }
  else if (RealIsNull(*m_fValue))
  {
    tempStr = "---";
  }
  else
  {
    m_units.GetUserString(*m_fValue, tempStr);
  }
  SetText(tempStr);
}


class XYValueGridCell : public ValueGridCell {

public:
  XYValueGridCell(int index, bool isX, PointsParameterGridCtrl& grid, double minL, double maxL, UnitIndex&  units);

  virtual void    UpdateText();
  virtual void    SetValue(CString inStr);

protected:
  virtual CWnd*   CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar);
  virtual bool    SetPtr();

  const bool      getX;
  PointsCurveValue* m_pointsCurveValue;
};


XYValueGridCell::XYValueGridCell(int index, bool isX, PointsParameterGridCtrl& grid, double minL, double maxL, UnitIndex&  units) :
  ValueGridCell(index, grid, minL, maxL, units), getX(isX)
{
}


bool  XYValueGridCell::SetPtr()
{
  PointsCurveXY& currXY = GetPointsCurveXY();
  if (getX)
  {
    m_pointsCurveValue = &currXY.xVal;
  }
  else
  {
    m_pointsCurveValue = &currXY.yVal;
  }
  m_fValue = &(m_pointsCurveValue->pVal);
  return true;
}


CWnd* XYValueGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
  if (!SetPtr())
    return NULL;

  if (m_pointsCurveValue->IsVary())
  {
    VaryVarUnitsDlg dlgdlg(*m_pointsCurveValue->pVaryData, 0, minLimit, maxLimit);
    pParent->SetFocus();
    static_cast<CGridCtrlCell*>(pParent)->FinishEdit(NULL, -1, nFirstChar);
    return NULL;
  }

  return ValueGridCell::CreateEditWnd(rect, pParent, nID, nRow, nCol, nFirstChar);
}


void XYValueGridCell::SetValue(CString inStr)
{
  if (!PointsCurveXYIsValid())
    return;

  SetPtr();
  if (m_pointsCurveValue->IsVary())
    return;

  ValueGridCell::SetValue(inStr);
}



void XYValueGridCell::UpdateText()
{
  if (SetNullText(this))
    return;

  SetPtr();

  if (m_pointsCurveValue->IsVary())
  {
    char tempStr[40];
    m_pointsCurveValue->pVaryData->GetMenuStr(tempStr, 40);
    SetText(tempStr);
    return;
  }

  ValueGridCell::UpdateText();
}




class XOptMinValueGridCell : public ValueGridCell {
public:
  XOptMinValueGridCell(int index, PointsParameterGridCtrl& grid, double minL, double maxL, UnitIndex& units) :
    ValueGridCell(index, grid, minL, maxL, units) {}

protected:
  virtual bool    SetPtr();


};

bool  XOptMinValueGridCell::SetPtr()
{
  PointsCurveXY& currXY = GetPointsCurveXY();
  m_fValue = 0;
  if (currXY.xVal.IsOpt())
    m_fValue = &currXY.xVal.pOptData->optMinLim;

  return currXY.xVal.IsOpt();
}

class XOptMaxValueGridCell : public ValueGridCell {
public:
  XOptMaxValueGridCell(int index, PointsParameterGridCtrl& grid, double minL, double maxL, UnitIndex& units) :
    ValueGridCell(index, grid, minL, maxL, units) {}

protected:
  virtual bool    SetPtr();


};

bool  XOptMaxValueGridCell::SetPtr()
{
  PointsCurveXY& currXY = GetPointsCurveXY();
  m_fValue = 0;
  if (currXY.xVal.IsOpt())
    m_fValue = &currXY.xVal.pOptData->optMaxLim;

  return currXY.xVal.IsOpt();
}



}  // end namespace

using namespace fprpointsParameter;



PointsParameterGridCtrl::PointsParameterGridCtrl() :
  m_pVals(0)
{

} // namespace




void  PointsParameterGridCtrl::InitGrid(Parameter* par)
{
  static const int minPoints = 16;

  PointsCurve* pCrv = par->GetPointsCurve();

  m_pVals = &pCrv->pVals;

  m_pVals->Cleanup();
  int nreAlloc = 16;
  if (m_pVals->Size() > nreAlloc)
    nreAlloc = m_pVals->Size() + 8;
  m_pVals->ReAlloc(nreAlloc);

  SetRowCount(m_pVals->AllocSize() + 1);

  int nCol = 5;
  if (control.IsOptimization())
  {
    SetColCount(7);
    SetCenteredText(0, 3, "OptMin");
    SetCenteredText(0, 4, "OptMax");
    nCol = 7;
  }
  else
  {
    SetColCount(5);
  }
  SetCenteredText(0, 1, "XType");
  SetCenteredText(0, nCol - 2, "YType");

  // kluge to tie into basic par limits
  double minX, maxX;
  CString sRowHeader;
  switch (par->GetCurveType()) {
  case Parameter::cptRadial: {
    sRowHeader = "Radius ";
    minX = 0.0;
    maxX = 1.0E+06;
    break;
  }
  case Parameter::cptPressure: {
    sRowHeader = "Pressure ";
    minX = 0.0;
    maxX = 1.0E+06;
    break;
  }
  case Parameter::cptTime: {
    sRowHeader = "Time ";
    minX = -1.0E+09;
    maxX =  1.0E+12;
    break;
  }
  }
  char tempStr[80];
  pCrv->xUnitIndex.MakeUnitString(tempStr, 80, true);
  sRowHeader += tempStr;
  SetCenteredText(0, 2, sRowHeader);

  sRowHeader = par->shortName;
  sRowHeader += " ";;

  pCrv->yUnitIndex.MakeUnitString(tempStr, 80, true);
  sRowHeader += tempStr;
  SetCenteredText(0, nCol - 1, sRowHeader);

  for (int i = 0; i < m_pVals->AllocSize(); i++) {

    int row = i + 1;
    sRowHeader.Format("%d", row);
    SetCenteredText(row, 0, sRowHeader );

    SetCell(row, 1, new TypeGridCell(i, true, *this));
    SetCell(row, 2, new XYValueGridCell(i, true, *this, minX, maxX, pCrv->xUnitIndex));

    int nextCol = 3;
    if (control.IsOptimization())
    {
      SetCell(row, nextCol++, new XOptMinValueGridCell(i, *this, minX, maxX, pCrv->xUnitIndex));
      SetCell(row, nextCol++, new XOptMaxValueGridCell(i, *this, minX, maxX, pCrv->xUnitIndex));
    }

    SetCell(row, nextCol++, new TypeGridCell(i, false, *this));
    SetCell(row, nextCol++, new XYValueGridCell(i, false, *this, par->minLimit, par->maxLimit, pCrv->yUnitIndex));

    ResetRow(row);
  }

  SetEditMode();
  SetBorderThickness(4);
  AutoSize();

  for (int i = 0; i < nCol; i++)
    SetColSize(i, GetColSize(i) + 20);

  InvalidateAll();
}



void PointsParameterGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
  int indx = m_cellActive.m_nRow - 1;
  PointsCurveXY&  currXY = GetPointsCurveXY(indx);

  // Change values in Grid
  CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);

  int mappedCol = m_cellActive.m_nCol;
  if ((!control.IsOptimization()) && (mappedCol > 2))
    mappedCol += 2;

  switch (mappedCol) {
  case 0: {
    break;
  }
  case 1 : {    // xtype
    static_cast<TypeGridCell*>(pCell)->SetPointType(nValue);
    pCell->SetText(szText);
    break;
  }
  case 2: {
    XYValueGridCell* vCell = static_cast<XYValueGridCell*>(pCell);
    vCell->SetValue(szText);
    break;
  }
  case 3: {        // x min
    ValueGridCell* vCell = static_cast<ValueGridCell*>(pCell);
    vCell->SetValue(szText);
    break;
  }
  case 4: {        // x max
    ValueGridCell* vCell = static_cast<ValueGridCell*>(pCell);
    vCell->SetValue(szText);
    break;
  }
  case 5 : {    // ytype
    static_cast<TypeGridCell*>(pCell)->SetPointType(nValue);
    pCell->SetText(szText);
    break;
  }
  case 6: {       // yvalue
    XYValueGridCell* vCell = static_cast<XYValueGridCell*>(pCell);
    vCell->SetValue(szText);
    break;
  }
  }

  ResetRow(m_cellActive.m_nRow);

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

void PointsParameterGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
  CGridCellIndices currCell = GetCellIndices(point);
  int currRow = currCell.m_nRow;
  if ((currRow <= 0) || (currRow > m_pVals->Size()))
    return;

  m_currIndex = currRow - 1;

  CMenu popMenu;
  popMenu.CreatePopupMenu();

  UINT insFlags = MF_STRING;
  if (m_pVals->IsNotValid(m_currIndex) || m_pVals->IsFull())
    insFlags = MF_STRING | MF_GRAYED;

  VERIFY(popMenu.AppendMenu(insFlags,     WM_POINTPAR_INS_BEFORE,  "Insert Before" ));
  VERIFY(popMenu.AppendMenu(insFlags,     WM_POINTPAR_INS_AFTER,   "Insert After" ));
  VERIFY(popMenu.AppendMenu(MF_STRING,    WM_POINTPAR_DELETE,      "Delete" ));

  ClientToScreen(&point);
  popMenu.TrackPopupMenu( TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this );
}


void PointsParameterGridCtrl::DeleteRow(int index)
{
  m_pVals->DeletePtr(index);
}

void PointsParameterGridCtrl::InsertRow(int index)
{
  if (!m_pVals->InsertPtr(index))
    return;
  SetPointsCurveXYValid(index);
}


void PointsParameterGridCtrl::UpdateModifiedRows()
{
  for (int i = m_currIndex + 1; i < GetRowCount(); i++)
    ResetRow(i);
}

void PointsParameterGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // is it a valid sequence row ?
  m_currIndex = m_cellActive.m_nRow - 1;
  if (m_currIndex >= 0)
  {
    bool opDone = false;
    switch (nChar) {
    case VK_DELETE: {
      if (m_pVals->IsValid(m_currIndex))
      {
        DeleteRow(m_currIndex);
        opDone = true;
      }
      break;
    }
    case VK_INSERT: {
      // must be valid
      if (m_pVals->IsValid(m_currIndex) && (!m_pVals->IsFull()))
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


void PointsParameterGridCtrl::OnPopupCommand(UINT nMenuID)
{
  switch (nMenuID)
  {
  case WM_POINTPAR_INS_BEFORE : {
    InsertRow(m_currIndex);
    if (m_currIndex > 0)
      m_currIndex--;
    break;
  }
  case WM_POINTPAR_INS_AFTER : {
    InsertRow(m_currIndex + 1);
    break;
  }
  case WM_POINTPAR_DELETE : {
    DeleteRow(m_currIndex);
    break;
  }
  default: {
    ASSERT(0);
  }
  }

  UpdateModifiedRows();

}

void PointsParameterGridCtrl::SetPointsCurveXYValid(int index)
{
  if (!m_pVals->IsValid(index))
  {
    m_pVals->MakeNew(index);
    if (index >= m_pVals->Size())
      m_pVals->SetSize(index + 1);
  }

}

PointsCurveXY&  PointsParameterGridCtrl::GetPointsCurveXY(int index)
{
  SetPointsCurveXYValid(index);
  return m_pVals->GetRef(index);

}
bool PointsParameterGridCtrl::PointsCurveXYIsValid(int index)
{
  return m_pVals->IsValid(index);

}



BEGIN_MESSAGE_MAP(PointsParameterGridCtrl,CGridCtrlCell)
  //{{AFX_MSG_MAP(ParameterGridCtrl)
  ON_WM_RBUTTONDOWN()
  ON_WM_KEYDOWN()
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(WM_POINTPAR_INS_BEFORE, WM_POINTPAR_DELETE, OnPopupCommand)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgPointsParametersPointsEntry dialog

IMPLEMENT_DYNCREATE(DlgPointsParametersPointsEntry,CGenericDialog)

DlgPointsParametersPointsEntry::DlgPointsParametersPointsEntry(CWnd* pParent /*=NULL*/)
  : CGenericDialog(DlgPointsParametersPointsEntry::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgPointsParametersPointsEntry)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void DlgPointsParametersPointsEntry::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DlgPointsParametersPointsEntry)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgPointsParametersPointsEntry, CGenericDialog)
  //{{AFX_MSG_MAP(DlgPointsParametersPointsEntry)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgPointsParametersPointsEntry message handlers

BOOL DlgPointsParametersPointsEntry::OnInitDialog()
{
  CGenericDialog::OnInitDialog();

  m_grid.SubclassDlgItem(IDC_GRID, this);

  ResetAllControls();

  SetControlInfo(IDC_GRID, resizeBoth);

  return TRUE;
}


void DlgPointsParametersPointsEntry::ResetAllControls()
{
  TabbedFolderNodePointsParameterInside* pFolder =
    static_cast<TabbedFolderNodePointsParameterInside*>(GetParent()->GetParent());

  m_pParameter = pFolder->m_pParameter;
  m_grid.InitGrid(m_pParameter);
}


