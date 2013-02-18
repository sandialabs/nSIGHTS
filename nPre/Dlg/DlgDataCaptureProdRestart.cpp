///////////////////////////////////////////////////////////////////////////////////
//
// DlgDataCaptureProdRestart.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains certain rights in this software.
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
//
// DESCRIPTION:
//
//      nPre dialog.
//
///////////////////////////////////////////////////////////////////////////////////

// DlgDataCaptureProdRestart.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <genClass/U_Str.h>

#include <nsClass/nSightConst.h>
#include <Var/G_DataCapture.h>
#include <Var/G_Control.h>

#include "DlgDataCaptureProdRestart.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgDataCaptureProdRestart dialog

IMPLEMENT_DYNCREATE(DlgDataCaptureProdRestart, CGenericDialog)

DlgDataCaptureProdRestart::DlgDataCaptureProdRestart(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgDataCaptureProdRestart::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgDataCaptureProdRestart)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

DlgDataCaptureProdRestart::~DlgDataCaptureProdRestart()
{
}

void DlgDataCaptureProdRestart::DoDataExchange(CDataExchange* pDX)
{
    CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgDataCaptureProdRestart)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgDataCaptureProdRestart, CGenericDialog)
    //{{AFX_MSG_MAP(DlgDataCaptureProdRestart)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


#include <genGrid/TextGridCell.h>
#include <genGrid/ComboGridCell.h>
#include <genUnits/UnitGridCell.h>

class ProdRestartTimeGridCell : public CTextGridCell
{
public:
    ProdRestartTimeGridCell(int index, const char* szText = NULL, Alignment alignment = alignRight);

    virtual void UpdateText();
protected:
    virtual CWnd* CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar);

protected:
    const int m_Index;
};

ProdRestartTimeGridCell::ProdRestartTimeGridCell(int index, const char* szText, Alignment alignment) :
    CTextGridCell(szText, alignment),
    m_Index(index)
{
}


void ProdRestartTimeGridCell::UpdateText()
{
    CString sText;
    productionRestartUnits.GetUserString(productionRestartTimes[m_Index], sText);
    SetText(sText);
}


CWnd* ProdRestartTimeGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
    CString loLim, upLim;
    productionRestartUnits.GetUserString(-1.0E+09, loLim);
    productionRestartUnits.GetUserString(+1.0E+09, upLim);

    GenAppParameterRangeMsg(loLim, upLim);
    return CTextGridCell::CreateEditWnd(rect, pParent, nID, nRow, nCol, nFirstChar);
}

///////////////////////////////////////////////////////////////////////////////////


ProdRestartGridCtrl::ProdRestartGridCtrl() :
    CGridCtrlCell()
{
}

void ProdRestartGridCtrl::ResetGrid()
{
    static const int maxRestart = 16;
    productionRestartTimes.ReAlloc(maxRestart);

    SetRowCount(maxRestart  + 1);
    SetColCount(2);

    SetCenteredText(0, 1, "Restart Time" );

    CString sRowHeader;
    for (int i = 0; i < maxRestart; i++)
    {
        int row = i + 1;
        sRowHeader.Format("%d", row);
        SetCenteredText(row, 0, sRowHeader );

        if (i > productionRestartTimes.UpperBound())
            productionRestartTimes[i] = nullReal;

        SetCell(row, 1, new ProdRestartTimeGridCell(i));
    }

    SetEditMode();
    SetBorderThickness(4);
    AutoSize();
    SetColSize(1, GetColSize(1) + 15);

    UpdateAllRows();
}


BEGIN_MESSAGE_MAP(ProdRestartGridCtrl,CGridCtrlCell)
    //{{AFX_MSG_MAP(ProdRestartGridCtrl)
    ON_WM_KEYDOWN()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



void ProdRestartGridCtrl::UpdateAllRows()
{
    productionRestartTimes.Cleanup();
    productionRestartTimes.Sort(true);
    for (int i = 1; i < GetRowCount(); i++)
    {
        CGridCell* pCell = GetCell(i, 1);
        pCell->UpdateText();
    }
    InvalidateAll();
}

void ProdRestartGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // is it a valid sequence row ?
    m_currRow = m_cellActive.m_nRow - 1;
    if ((m_currRow >= 0) && (m_currRow < productionRestartTimes.Size()))
    {
        switch (nChar) {
            case VK_DELETE: {
                productionRestartTimes.DeleteItem(m_currRow);
                productionRestartTimes[productionRestartTimes.Size()] = nullReal;
                UpdateAllRows();
                break;
            }
            default : ;
        }
    }

    CGridCtrlCell::OnKeyDown(nChar, nRepCnt, nFlags);
}

void ProdRestartGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
    int index = m_cellActive.m_nRow - 1;

    UnitReal temp(productionRestartUnits, productionRestartTimes[index]);
    if (temp.IsWithinRange(szText, -1.0E+08, 1.0E+08))
    {
        productionRestartTimes[index] = temp.GetMetricVal();
        if (index > productionRestartTimes.UpperBound())
            productionRestartTimes.SetSize(index + 1);
    }
    StopEdit(false);

    UpdateAllRows();


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

BOOL DlgDataCaptureProdRestart::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_grid.SubclassDlgItem(IDC_GRID, this);

    m_uiCtrl.Set(this, IDC_COMBO1, &productionRestartUnits);
    m_uiCtrl.AddCallback(this);

    SetControlInfo(IDC_GRID, resizeBoth);

    ResetAllControls();

    return TRUE;

}

void DlgDataCaptureProdRestart::DoCtrlUpdate()
{
    m_grid.UpdateAllRows();
}


void DlgDataCaptureProdRestart::ResetAllControls()
{
    m_grid.ResetGrid();
    m_uiCtrl.DoCtrlReset();
}

