///////////////////////////////////////////////////////////////////////////////////
//
// VaryVarUnitsSuiteDlg.cpp
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
#include "VaryVarUnitsSuiteDlg.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVaryVarUnitsSuiteDlg dialog


CVaryVarUnitsSuiteDlg::CVaryVarUnitsSuiteDlg(VaryVarUnits& varyVar,
    const SC_RealFormat* overrideFormat,
    const double& minVal, const double& maxVal,
    CWnd* pParent /*=NULL*/) :
  CDialog(CVaryVarUnitsSuiteDlg::IDD, pParent),
  m_varyVar(varyVar), m_poverrideFormat(overrideFormat), m_minVal(minVal),  m_maxVal(maxVal)
{
  //{{AFX_DATA_INIT(CVaryVarUnitsSuiteDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CVaryVarUnitsSuiteDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CVaryVarUnitsSuiteDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVaryVarUnitsSuiteDlg, CDialog)
  //{{AFX_MSG_MAP(CVaryVarUnitsSuiteDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVaryVarUnitsSuiteDlg message handlers

#include <genGrid/TextGridCell.h>

class SuiteGridCell : public CTextGridCell
{
public:
  SuiteGridCell(  VaryVarUnits& varyVar,
                  const double& minVal, const double& maxVal,
                  const SC_RealFormat*  poverrideFormat,
                  double fValue, Alignment alignment = alignLeft);

  virtual void FocusIn();

  virtual void UpdateText();
  double GetValue();
  void SetValue(double fValue);
  void SetValue(const CString& inStr);

protected:
  VaryVarUnits& m_varyVar;
  const double& m_minVal;
  const double& m_maxVal;
  const SC_RealFormat* m_poverrideFormat;
  double m_fMetricValue;

private:
  void    GetUserString(const double&  inVal,
                        CString& inStr);

};


void SuiteGridCell::GetUserString(const double&  inVal,
                                  CString& inStr)
{
  m_varyVar.unitIndex.GetUserString(inVal, inStr, m_poverrideFormat);
}

double SuiteGridCell::GetValue()
{
  return m_fMetricValue;
}

void SuiteGridCell::SetValue(double fValue)
{
  m_fMetricValue = fValue;
}

void SuiteGridCell::SetValue(const CString& inStr)
{
  UnitReal temp(m_varyVar.unitIndex);
  if (temp.IsWithinRange(inStr, m_minVal, m_maxVal))
  {
    m_fMetricValue = temp.GetMetricVal();
    UpdateText();
  }
}

void SuiteGridCell::FocusIn()
{
  CString loLim, upLim;
  GetUserString(m_minVal, loLim);
  GetUserString(m_maxVal, upLim);
  GenAppParameterRangeMsg(loLim, upLim);
}


SuiteGridCell::SuiteGridCell(VaryVarUnits& varyVar,
                             const double& minVal, const double& maxVal,
                             const SC_RealFormat*  poverrideFormat,
                             double fValue, Alignment alignment) :
  CTextGridCell("", alignment),
  m_poverrideFormat(poverrideFormat),
  m_varyVar(varyVar),
  m_fMetricValue(fValue),
  m_minVal(minVal), m_maxVal(maxVal)
{
}

void SuiteGridCell::UpdateText()
{
  CString tempStr;
  GetUserString(m_fMetricValue, tempStr);
  SetText(tempStr);
}

BOOL CVaryVarUnitsSuiteDlg::OnInitDialog()
{
  m_grid.SubclassDlgItem(IDC_GRID, this);

  m_uiCtrl.Set(this, IDC_COMBO1, &m_varyVar.unitIndex);
  m_uiCtrl.AddCallback(this);

  CDialog::OnInitDialog();

  m_ngridCell = m_varyVar.suiteValues.AllocSize();

  m_grid.InitializeGrid(m_varyVar, m_poverrideFormat, m_minVal, m_maxVal);

  return TRUE;
}

void CVaryVarUnitsSuiteDlg::OnOK()
{
  // kluge to make sure that ctrl with current focus gets updated
  GotoDlgCtrl(GetDlgItem(IDOK));

  m_varyVar.suiteValues.SetEmpty();
  for (int i = 0; i < m_ngridCell; i++) {
    SuiteGridCell* pCell = static_cast<SuiteGridCell*>(m_grid.GetCell(i+1, 1));
    double currVal  = pCell->GetValue();
    if (!RealIsNull(currVal))
      m_varyVar.suiteValues += currVal;
  }

  CDialog::OnOK();
}


void CVaryVarUnitsSuiteDlg::DoCtrlUpdate()
{
  m_grid.UpdateGrid();
}


SuiteGridCtrl::SuiteGridCtrl() :
  CGridCtrlCell()
{
}

BEGIN_MESSAGE_MAP(SuiteGridCtrl,CGridCtrlCell)
  //{{AFX_MSG_MAP(SuiteGridCtrl)
  ON_WM_KEYDOWN()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void SuiteGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
  // Change values in Grid
  SuiteGridCell* pCell = static_cast<SuiteGridCell*>(GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol));
  pCell->SetValue(szText);
  InvalidateCellRect(CGridCellRange(m_cellActive, m_cellActive));

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
    StartEdit(nLastChar);
    break;
  }
}

void SuiteGridCtrl::UpdateGrid()
{
  for (int i = 1; i < GetRowCount(); i++) {
    CGridCell* pCell = GetCell(i, 1);
    pCell->UpdateText();
  }
  Invalidate();
}


void SuiteGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // check for one of entry cells
  if ((m_cellActive.m_nRow > 0) && (m_cellActive.m_nCol == 1))
  {
    int currRow = m_cellActive.m_nRow;
    SuiteGridCell  *nextCell, *currCell;

    switch (nChar) {
    case VK_DELETE: {
      // delete active cell
      currCell = static_cast<SuiteGridCell*>(GetCell(currRow, 1));
      for (int i = currRow; i < GetRowCount() - 1; i++) {
        nextCell = static_cast<SuiteGridCell*>(GetCell(i+1, 1));
        currCell->SetValue(nextCell->GetValue());
        currCell = nextCell;
      }
      currCell->SetValue(nullReal);
      UpdateGrid();
      return;
    }
    case VK_INSERT: {
      // last cell must be non-null
      currCell = static_cast<SuiteGridCell*>(GetCell(GetRowCount() - 1, 1));
      if (!RealIsNull(currCell->GetValue()))
        return;

      // insert rows
      for (int i = GetRowCount() - 1; i > currRow; i--) {
        nextCell = static_cast<SuiteGridCell*>(GetCell(i-1, 1));
        currCell->SetValue(nextCell->GetValue());
        currCell = nextCell;
      }
      currCell->SetValue(nullReal);
      UpdateGrid();
      return;
    }
    default :
      ;
    }

  }

  CGridCtrlCell::OnKeyDown(nChar, nRepCnt, nFlags);
}


void SuiteGridCtrl::InitializeGrid(VaryVarUnits&   gridVaryVar,
                                   const SC_RealFormat* overrideFormat,
                                   const double&   minVal,
                                   const double&   maxVal)
{
  int ngridCell = gridVaryVar.suiteValues.AllocSize();

  SetColCount(2);
  SetRowCount(ngridCell + 1);
  SetCenteredText(0, 0, "   " );
  SetCenteredText(0, 1, " Values " );
  CString sValue;
  for (int i = 0; i < ngridCell; i++) {
    double initVal;
    if (i < gridVaryVar.suiteValues.Size())
      initVal = gridVaryVar.suiteValues[i];
    else
      initVal = nullReal;
    SuiteGridCell* pCell = new SuiteGridCell(gridVaryVar, minVal, maxVal, overrideFormat, initVal, CTextGridCell::alignRight);
    sValue.Format( " %d ", i+1 );
    SetRightText(i+1, 0, sValue);
    SetCell(i+1, 1, pCell);
    pCell->UpdateText();
  }
  SetEditMode();
  AutoSize();
  SetMinColSize(0, 22);
  SetMinColSize(1, 250);
  ResetScrollBars();
}
