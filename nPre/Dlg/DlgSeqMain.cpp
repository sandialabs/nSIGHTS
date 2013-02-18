///////////////////////////////////////////////////////////////////////////////////
//
// DlgSeqMain.cpp
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
//      nPre dialog.  Why are there class definitions in this file?
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
#include "DlgSeqMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSeqMain dialog

IMPLEMENT_DYNCREATE(DlgSeqMain, CGenericDialog)

DlgSeqMain::DlgSeqMain(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgSeqMain::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgSeqMain)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

DlgSeqMain::~DlgSeqMain()
{
}

void DlgSeqMain::DoDataExchange(CDataExchange* pDX)
{
    CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgSeqMain)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSeqMain, CGenericDialog)
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
                    SeqTypeGridCell(int index);
                    ~SeqTypeGridCell() {};

    virtual void    UpdateText();
    void            SetSequenceType(int nValue);

protected:
    virtual void    GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);

protected:
    const int       seqIndex;
};


SeqTypeGridCell::SeqTypeGridCell(int index) :
    CComboGridCell(NULL, alignLeft),
    seqIndex(index)
{
}

void SeqTypeGridCell::UpdateText()
{
    if (sequenceData[seqIndex] != 0)
        for (int i = 0; i < SequenceGridCtrl::seqTypeStrings.Size(); i++)
            if (int(sequenceData[seqIndex]->seqType) == SequenceGridCtrl::seqTypeMap[i])
            {
                SetText(SequenceGridCtrl::seqTypeStrings[i]);
                return;
            }
    SetText(nullStr);
}


void SeqTypeGridCell::SetSequenceType(int nValue)
{
    sequence.CheckAndAllocSequence(seqIndex);
    if (nValue < 0)
        nValue = 0;
    sequenceData[seqIndex]->ChangeSeqType(Sequence::SequenceType(SequenceGridCtrl::seqTypeMap[nValue]));
    sequenceData[seqIndex]->ClearWasActive();
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
                    DesigGridCell(int index);

    virtual void    UpdateText();

protected:
    const int seqIndex;
};

DesigGridCell::DesigGridCell(int index) :
    CTextGridCell(NULL, alignLeft),
    seqIndex(index)
{
}


void DesigGridCell::UpdateText()
{
    if (sequenceData[seqIndex] != 0)
        SetText(sequenceData[seqIndex]->seqID);
    else
        SetText(nullStr);
}


//    duration

class TimeValueGridCell : public CTextGridCell
{
    friend class SequenceGridCtrl;
public:
                    TimeValueGridCell(int index);

    virtual void    FocusIn();
    virtual void    UpdateText();

    bool            SetDuration(const char* szText);

protected:
    const int seqIndex;
};


TimeValueGridCell::TimeValueGridCell(int index) :
    CTextGridCell(NULL, alignRight),
    seqIndex(index)
{
}

void TimeValueGridCell::FocusIn()
{
    UnitIndex temp(uTime);
    if (sequenceData.IsValid(seqIndex))
        temp = sequenceData.GetRef(seqIndex).seqLength;
    temp.SetRangeMsg(1.0E-3, 1.0E+12);
}

bool TimeValueGridCell::SetDuration(const char* szText)
{
    return sequenceData.GetRef(seqIndex).seqLength.IsWithinRange(szText, 1.0E-3, 1.0E+12);
}


void TimeValueGridCell::UpdateText()
{
    if (sequenceData.IsValid(seqIndex))
    {
        Sequence& currSeq = sequenceData.GetRef(seqIndex);
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
                    StartTimeValueGridCell(int index);
    virtual void    UpdateText();
    virtual void    FocusIn();
    bool            SetStartTime(const char* szText);

protected:
    const int seqIndex;
};


StartTimeValueGridCell::StartTimeValueGridCell(int index) :
    CTextGridCell(NULL, alignRight),
    seqIndex(index)
{
}

void StartTimeValueGridCell::FocusIn()
{
    UnitIndex temp(testStartTime);
    temp.SetRangeMsg(-1.0E+09, testEndTime.GetMetricVal());
}

bool StartTimeValueGridCell::SetStartTime(const char* szText)
{
    double endLimit = testEndTime.GetMetricVal();
    if (seqIndex == 0)
    {
        if (!testStartTime.IsWithinRange(szText, -1.0E+09, endLimit))
            return false;
        sequence.SetSequenceTimes();
        return true;
    }

    // limits are previous start time to end time
    if (sequenceData.IsNotValid(seqIndex - 1))
        return false;

    double stLimit = sequenceData.GetRef(seqIndex - 1).startTime;
    if (RealIsNull(stLimit))
        return false;

    UnitReal tempTime(testStartTime);

    if (tempTime.IsWithinRange(szText, stLimit, endLimit))
    {
        double currStart = tempTime.GetMetricVal();
        // re-set duration of prev seq
        sequenceData.GetRef(seqIndex - 1).seqLength.SetMetricVal(currStart - stLimit);

        // set current length
        // if current end time is null then arbitary at 3600 sec
        Sequence& currSeq = sequenceData.GetRef(seqIndex);

        // if last sequence entered set from end time
        if (seqIndex == (sequenceData.Size() - 1))
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
            if ((sequenceData.AllocSize() > (seqIndex +1)) &&
                (sequenceData.IsValid(seqIndex + 1)))
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
    if (sequenceData.IsValid(seqIndex))
    {
        CString sText;
        UnitReal currStart = testStartTime;
        currStart.SetMetricVal(sequenceData[seqIndex]->startTime);
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
                    TimeUnitsGridCell(int index, Alignment alignment  = alignRight);
    virtual void    UpdateText();

protected:
    const int seqIndex;
};

TimeUnitsGridCell::TimeUnitsGridCell(int index, Alignment alignment) :
    CUnitGridCell(uTime, nullUnitConvIndex, alignment),
    seqIndex(index)
{
}

void TimeUnitsGridCell::UpdateText()
{
    if (sequenceData[seqIndex] != 0)
        SetText(sequenceData[seqIndex]->seqLength.GetUnitString());
    else
        SetText(nullStr);
}



// Sequences /////////////////////////////////////////////////////////////////////////////////



class SeqDataGridCell : public CTextGridCell
{
    friend class SequenceGridCtrl;
public:
                    SeqDataGridCell(int index, const char* szText = NULL, Alignment alignment = alignRight);

    virtual void    UpdateText();
protected:
    virtual bool    CanEdit();
    virtual CWnd*   CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar);

protected:
    const int seqIndex;
};

SeqDataGridCell::SeqDataGridCell(int index, const char* szText, Alignment alignment) :
    CTextGridCell(szText, alignment),
    seqIndex(index)
{
}


void SeqDataGridCell::UpdateText()
{
    if (sequenceData.IsNotValid(seqIndex))
    {
        SetText(nullStr);
        return;
    }

    Sequence& currSeq = sequenceData.GetRef(seqIndex);

    CString sText;
    SC_SetupErr errData;

    // default values
    bool seqOK = currSeq.SetupOK(errData);
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
    return sequenceData[seqIndex] != 0;
}

CWnd* SeqDataGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
    if (sequenceData.IsNotValid(seqIndex))
        return NULL;

    Sequence& currSeq = sequenceData.GetRef(seqIndex);

    switch (currSeq.seqType) {
    case Sequence::stFlow:
        {
            CSequenceFlowHistDlg dlg(currSeq, currSeq.flowData, true);
            dlg.DoModal();
            break;
        }
    case Sequence::stHistory:
        {
            CSequenceFlowHistDlg dlg(currSeq, currSeq.historyData, false);
            dlg.DoModal();
            break;
        }
    case Sequence::stPulse:
        {
            CSequencePulseDlg dlg(currSeq);
            dlg.DoModal();
            break;
        }
    case Sequence::stSlug:
        {
            CSequenceSlugDlg dlg(currSeq);
            dlg.DoModal();
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
                    UseAutoGridCell(int index);

    virtual void    UpdateText();
    virtual void    OnCellSelected(CWnd* pParent, int nRow, int nCol);

protected:
    const int seqIndex;
};

UseAutoGridCell::UseAutoGridCell(int index) :
    CCheckGridCell(false),
    seqIndex(index)
{
}


void UseAutoGridCell::UpdateText()
{
    bool isOK = sequenceData[seqIndex] != 0;
    SetReadOnly(isOK);
    if (isOK)
        SetValue(sequenceData[seqIndex]->useInAutoSetup);
}

void UseAutoGridCell::OnCellSelected(CWnd* pParent, int nRow, int nCol)
{
    CCheckGridCell::OnCellSelected(pParent, nRow, nCol);
    if (sequenceData[seqIndex] != 0)
        sequenceData[seqIndex]->useInAutoSetup = GetValue();
}


///////////////////////////////////////////////////////////////////////////////////

SC_StringArray  SequenceGridCtrl::seqTypeStrings;
SC_IntArray     SequenceGridCtrl::seqTypeMap;

SequenceGridCtrl::SequenceGridCtrl() :
    CGridCtrlCell()
{
}

void SequenceGridCtrl::ResetSequenceGrid()
{
    // start with a clean slate
    sequence.SequenceCleanup();
    sequence.SetSequenceTimes();

    // make sure there are 20 blank sequences
    sequenceData.ReAlloc(sequenceData.Size() + 20);

    // set strings for type combos
    seqTypeStrings.Alloc(4);
    seqTypeMap.Alloc(4);

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


    // set-up grid based on flags
    SetRowCount(sequenceData.AllocSize() + 1);

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
    for (int i = 0; i < sequenceData.AllocSize(); i++) {

        int row = i + 1;
        sRowHeader.Format("%d", row);
        SetCenteredText(row, 0, sRowHeader );

        SetCell(row, 1, new SeqTypeGridCell(i));
        SetCell(row, 2, new DesigGridCell(i));

        if (enterSequenceDurations)
        {
            SetCell(row, 3, new TimeValueGridCell(i));
            SetCell(row, 4, new TimeUnitsGridCell(i));
            SetCell(row, 5, new SeqDataGridCell(i));
            SetCell(row, 6, new StartTimeValueGridCell(i));
            SetCell(row, 7, new UseAutoGridCell(i));
        }
        else
        {
            SetCell(row, 3, new StartTimeValueGridCell(i));
            SetCell(row, 4, new SeqDataGridCell(i));
            SetCell(row, 5, new TimeValueGridCell(i));
            SetCell(row, 6, new UseAutoGridCell(i));
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
    if ((currRow <= 0) || (currRow > sequenceData.Size()))
        return;

    m_currSeq = currRow - 1;

    CMenu popMenu;
    popMenu.CreatePopupMenu();

    UINT insFlags = MF_STRING;
    if (sequenceData.IsNotValid(m_currSeq))
        insFlags = MF_STRING | MF_GRAYED;

    UINT dupFlags = insFlags;
    if (!enterSequenceDurations)
        dupFlags = MF_STRING | MF_GRAYED;

    UINT delFlags = MF_STRING;
    if ((sequenceData.Size() == 1) && (m_currSeq == 0))
        delFlags = MF_STRING | MF_GRAYED;

    VERIFY(popMenu.AppendMenu(insFlags, WM_SEQM_INS_BEFORE, "Insert Before" ));
    VERIFY(popMenu.AppendMenu(insFlags, WM_SEQM_INS_AFTER,  "Insert After" ));
    VERIFY(popMenu.AppendMenu(dupFlags, WM_SEQM_DUPLICATE,  "Duplicate" ));
    VERIFY(popMenu.AppendMenu(delFlags, WM_SEQM_DELETE,     "Delete" ));

    ClientToScreen(&point);
    popMenu.TrackPopupMenu( TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this );

}


void SequenceGridCtrl::DeleteSequence(int index)
{
    sequenceData.DeletePtr(index);
}

void SequenceGridCtrl::InsertSequence(int index)
{
    if (!sequenceData.InsertPtr(index))
        return;

    sequence.CheckAndAllocSequence(index);

    if (enterSequenceDurations)
        return;

    // split durations with adjacent sequence


    int halfRef;
    //  valid seq before ??
    if ((index > 0) && (sequenceData.IsValid(index - 1)))
    {
        // take before
        halfRef = index - 1;
    }
    // after -- should always be valid
    else if (sequenceData.IsValid(index + 1))
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

    double halfDur = sequenceData.GetRef(halfRef).seqLength.GetMetricVal();
    if (!RealIsNull(halfDur))
    {
        halfDur /= 2.0;
        sequenceData.GetRef(index - 1).seqLength.SetMetricVal(halfDur);
        sequenceData.GetRef(index).seqLength.SetMetricVal(halfDur);
    }

}


void SequenceGridCtrl::UpdateModifiedRows()
{
    sequence.SetSequenceTimes();
    sequence.SetSequenceIDs();
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
                if ((sequenceData.Size() > 1) || (m_currSeq > 0))
                {
                    DeleteSequence(m_currSeq);
                    opDone = true;
                }
                break;
            }
            case VK_INSERT: {
                // must be valid
                if (sequenceData.IsValid(m_currSeq))
                {
                    InsertSequence(m_currSeq);
                    // update can effect the row before
                    if (m_currSeq > 0)
                        m_currSeq--;
                    opDone = true;
                }
                break;
            }
            default : ;
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
            if (sequenceData.InsertPtr(m_currSeq))
            {
                sequenceData.tListData[m_currSeq] = new Sequence(*sequenceData.tListData[m_currSeq + 1]);
            }
            break;
        }
        case WM_SEQM_DELETE : {
            DeleteSequence(m_currSeq);
            break;
        }
        default: {ASSERT(0);}
    }

    UpdateModifiedRows();

}


void SequenceGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
    int seqIndex = m_cellActive.m_nRow - 1;

    bool updateAll = (sequenceData.IsNotValid(seqIndex));
    bool updateStatus = false;

    sequence.CheckAndAllocSequence(seqIndex);
    Sequence& currSeq = sequenceData.GetRef(seqIndex);

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
            if (strcmp(currSeq.seqID, szText) != 0)
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
        sequence.SetSequenceTimes();

        // the row
        int i;
        for (i = 1; i < GetColCount(); i++)
            ResetCell(m_cellActive.m_nRow, i);

        // the time status col
        int lastCol = GetColCount() - 1;
        for (i = 1; i < GetRowCount(); i++)
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
            if (sequence.SetSequenceIDs())
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

BOOL DlgSeqMain::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_grid.SubclassDlgItem(IDC_GRID, this);

    SetControlInfo(IDC_GRID, resizeBoth);

    return TRUE;

}

void DlgSeqMain::ResetAllControls()
{
    m_grid.ResetSequenceGrid();
}

