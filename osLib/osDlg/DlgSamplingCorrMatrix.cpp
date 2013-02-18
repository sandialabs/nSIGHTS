///////////////////////////////////////////////////////////////////////////////////
//
// DlgSamplingCorrMatrix.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include "DlgSamplingCorrMatrix.h"

#include <genGrid/TextGridCell.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genUnits/C_GenUnits.h>
#include <osMain/G_Sample.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSamplingCorrMatrix dialog

IMPLEMENT_DYNCREATE(DlgSamplingCorrMatrix,CGenericDialog)

DlgSamplingCorrMatrix::DlgSamplingCorrMatrix(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgSamplingCorrMatrix::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgSamplingCorrMatrix)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void DlgSamplingCorrMatrix::DoDataExchange(CDataExchange* pDX)
{
    CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgSamplingCorrMatrix)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSamplingCorrMatrix, CGenericDialog)
    //{{AFX_MSG_MAP(DlgSamplingCorrMatrix)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgSamplingCorrMatrix message handlers

namespace correlationMatrixGrid {

   class ValueGridCell : public CTextGridCell
   {
       friend class CorrelationMatrixGridCtrl;
   public:
       ValueGridCell(double& dVal, const char* szText = NULL, Alignment alignment = alignRight);

       void        SetOther(ValueGridCell* other) {m_other = other;}

       virtual void UpdateText();
   protected:
       virtual bool CanEdit();
       virtual CSize GetExtent(CDC* pDC) const;

   protected:
       double& m_dval;
       ValueGridCell* m_other;
   };

   ValueGridCell::ValueGridCell(double& dVal, const char* szText, Alignment alignment) :
       CTextGridCell(szText, alignment),
       m_dval(dVal)
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
       CString sText;
       if (RealIsNull(m_dval))
       {
           sText = "---";
       }
       else
       {
           SC_DecFormat corrRF(3);
           corrRF.RealToString(m_dval, sText);
       }
       SetText(sText);
   }

   bool ValueGridCell::CanEdit()
   {
       return (m_other != this);
   }


}                                       // end namespace

using namespace correlationMatrixGrid;

///////////////////////////////////////////////////////////////////////////////////
CorrelationMatrixGridCtrl::CorrelationMatrixGridCtrl() :
    CGridCtrlCell()
{
}

BEGIN_MESSAGE_MAP(CorrelationMatrixGridCtrl,CGridCtrlCell)
    //{{AFX_MSG_MAP(CorrelationMatrixGridCtrl)
    ON_WM_KEYDOWN()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CorrelationMatrixGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
    // Change values in Grid
    ValueGridCell* pCell = static_cast<ValueGridCell*>(GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol));

    UnitReal dummy(uNone, pCell->m_dval);
    if (dummy.IsWithinRange(szText, -.999, 0.999))
    {
        pCell->m_dval = dummy.metricVal;
        pCell->m_other->m_dval = dummy.metricVal;
        SampVar::SetCorrelationMatrix(m_corrGroup, currMat);
    }

    pCell->UpdateText();
    pCell->m_other->UpdateText();

    InvalidateCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
    InvalidateCell(m_cellActive.m_nCol, m_cellActive.m_nRow);

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

void CorrelationMatrixGridCtrl::InitGrid(int corrGroup)
{
    SC_StringArray corrIDs;

    m_corrGroup = corrGroup;

    SampVar::GetCorrelationMatrix(corrGroup, currMat, corrIDs);

    SetColCount(currMat.Size() + 1);
    SetRowCount(currMat.Size() + 1);

    SetCenteredText(0, 0, "");

    int i, j;
    for (i = 0; i < currMat.Size(); i++) {
        SetCenteredText(0, i + 1, corrIDs[i]);
        SetCenteredText(i + 1, 0, corrIDs[i]);
        for (j = 0; j < currMat.Size(); j++) {
            ValueGridCell* pValueCell = new ValueGridCell(currMat[i][j]);
            if (i == j)
                pValueCell->SetReadOnly();
            SetCell(i + 1, j + 1, pValueCell);
            pValueCell->UpdateText();
        }
    }

    for (i = 0; i < currMat.Size(); i++)
    {
        for (j = 0; j < currMat.Size(); j++)
        {
            ValueGridCell* pCell = static_cast<ValueGridCell*>(GetCell(i + 1, j + 1));
            ValueGridCell* otherCell = static_cast<ValueGridCell*>(GetCell(j + 1, i + 1));
            pCell->SetOther(otherCell);
        }
    }

    SetEditMode();

    SetBorderThickness(4);
    AutoSize();
    for (i = 0; i < currMat.Size(); i++)
        SetColSize(i + 1, GetColSize(i + 1) + 10);
}

void CorrelationMatrixGridCtrl::InitDummy()
{
    SetColCount(1);
    SetRowCount(1);
    SetCenteredText(0, 0, "Less Than 2 Sample Values in Group");
    AutoSize();
    SetColSize(0, GetColSize(0) + 20);
}


void CorrelationMatrixGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // is it a valid sequence row ?
    int rowIndex = m_cellActive.m_nRow - 1;
    int colIndex = m_cellActive.m_nCol - 1;

    if ((nChar == VK_DELETE) && (rowIndex >= 0) && (colIndex >= 0) && (rowIndex != colIndex))
    {
        currMat[rowIndex][colIndex] = nullReal;
        currMat[colIndex][rowIndex] = nullReal;
        SampVar::SetCorrelationMatrix(m_corrGroup, currMat);

        GetCell(rowIndex + 1, colIndex + 1)->UpdateText();
        GetCell(colIndex + 1, rowIndex + 1)->UpdateText();

        InvalidateCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
        InvalidateCell(m_cellActive.m_nCol, m_cellActive.m_nRow);
        return;
    }

    CGridCtrlCell::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL DlgSamplingCorrMatrix::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_grid.SubclassDlgItem(IDC_GRID, this);
    SetControlInfo(IDC_GRID, resizeBoth);

    ResetAllControls();

    return TRUE;
}


void DlgSamplingCorrMatrix::ResetAllControls()
{
    int corrGroup = static_cast<DialogNodeSamplingCorrGroup*>(GetInterfaceNode())->m_corrGroup;
    if (SampVar::CorrGroupOK(corrGroup))
        m_grid.InitGrid(corrGroup);
    else
        m_grid.InitDummy();

}

