///////////////////////////////////////////////////////////////////////////////////
//
// DlgSeqTestZoneCurve.cpp
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

// DlgSeqTestZoneCurve.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <genClass/U_Str.h>
#include <objClass/DO_Curve.h>
#include <genApp/C_AppMenu.h>

#include <nsClass/nSightConst.h>
#include <Var/G_CurveFiles.h>
#include <Var/G_Sequence.h>
#include <Var/G_Control.h>

#include "PopupCmdDefines.h"

#include "SequenceTestZoneCurveDlg.h"
#include "DlgSeqTestZoneCurve.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSeqTestZoneCurve dialog

IMPLEMENT_DYNCREATE(DlgSeqTestZoneCurve, CGenericDialog)

DlgSeqTestZoneCurve::DlgSeqTestZoneCurve(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgSeqTestZoneCurve::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgSeqTestZoneCurve)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

DlgSeqTestZoneCurve::~DlgSeqTestZoneCurve()
{
}

void DlgSeqTestZoneCurve::DoDataExchange(CDataExchange* pDX)
{
    CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgSeqTestZoneCurve)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSeqTestZoneCurve, CGenericDialog)
    //{{AFX_MSG_MAP(DlgSeqTestZoneCurve)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// DlgSeqTestZoneCurve message handlers

#include <genGrid/TextGridCell.h>
#include <genGrid/ComboGridCell.h>
#include <genUnits/UnitGridCell.h>

class CurveTypeGridCell : public CComboGridCell
{
    friend class TestZoneCurveGridCtrl;

public:
                    CurveTypeGridCell(int index);

    virtual void    UpdateText();
    void            SetTestZoneCurveType(int nValue);

protected:
    virtual void    GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);

protected:
    const int       tzcIndex;
};


CurveTypeGridCell::CurveTypeGridCell(int index) :
    CComboGridCell(NULL, alignLeft),
    tzcIndex(index)
{
}

void CurveTypeGridCell::UpdateText()
{
    if (testZoneCurves.IsValid(tzcIndex))
        for (int i = 0; i < TestZoneCurveGridCtrl::curveTypeStrings.Size(); i++)
            if (int(testZoneCurves[tzcIndex]->tzType) == TestZoneCurveGridCtrl::curveTypeMap[i])
            {
                SetText(TestZoneCurveGridCtrl::curveTypeStrings[i]);
                return;
            }

    SetText(nullStr);
}


void CurveTypeGridCell::SetTestZoneCurveType(int nValue)
{
    if (nValue < 0)
        nValue = 0;
    testZoneCurves[tzcIndex]->ChangeCurveType(TestZoneCurve::TZCurveType(TestZoneCurveGridCtrl::curveTypeMap[nValue]));
}

void CurveTypeGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
    aText.RemoveAll();
    aEnable.RemoveAll();
    for (int i = 0; i < TestZoneCurveGridCtrl::curveTypeStrings.Size(); i++)
    {
        aText.Add(TestZoneCurveGridCtrl::curveTypeStrings[i]);
        aEnable.Add(true);
    }
}

class CurveFOGridCell : public CComboGridCell
{
    friend class TestZoneCurveGridCtrl;
public:
                    CurveFOGridCell(int index);

    virtual void    UpdateText();
    void            SetTestZoneCurveFO(int nValue);

protected:
    virtual void    GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);

protected:
    const int           tzcIndex;

};


CurveFOGridCell::CurveFOGridCell(int index) :
    CComboGridCell(NULL, alignLeft),
    tzcIndex(index)
{
}

void CurveFOGridCell::UpdateText()
{
    if (testZoneCurves.IsValid(tzcIndex))
    {
        if (!TestZoneCurveGridCtrl::curveFOOK)
        {
            SetText("no curve objects");
            return;
        }

        FuncObjRef& currFO = testZoneCurves[tzcIndex]->curveObjRef;
        if (currFO.IsNull())
            currFO = TestZoneCurveGridCtrl::curveFOData[0];

        for (int i = 0; i < TestZoneCurveGridCtrl::curveFOStrings.Size(); i++)
        {
            if (currFO == TestZoneCurveGridCtrl::curveFOData[i])
            {
                SetText(TestZoneCurveGridCtrl::curveFOStrings[i]);
                return;
            }
        }

    }
    SetText(nullStr);
}


void CurveFOGridCell::SetTestZoneCurveFO(int nValue)
{
    if (nValue < 0)
        nValue = 0;
    testZoneCurves[tzcIndex]->curveObjRef = TestZoneCurveGridCtrl::curveFOData[nValue];
}

void CurveFOGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{

    aText.RemoveAll();
    aEnable.RemoveAll();
    if (TestZoneCurveGridCtrl::curveFOOK)
    {
        for (int i = 0; i < TestZoneCurveGridCtrl::curveFOStrings.Size(); i++)
        {
            aText.Add(TestZoneCurveGridCtrl::curveFOStrings[i]);
            aEnable.Add(true);
        }
    }
    else
    {
        aText.Add("no crv obj");
        aEnable.Add(false);
    }
}


class CurveDesigGridCell : public CComboGridCell
{
    friend class    TestZoneCurveGridCtrl;
public:
                    CurveDesigGridCell(int index);

    virtual void    UpdateText();
    void            SetTestZoneCurveDesig(int nValue);

protected:
    virtual void    GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);

protected:
    const int       tzcIndex;
};


CurveDesigGridCell::CurveDesigGridCell(int index) :
    CComboGridCell(NULL, alignLeft),
    tzcIndex(index)
{
    if (TestZoneCurveGridCtrl::curveFileDesigStrings.IsEmpty())
    {
        SetText("Bad Curve File");
        SetReadOnly();
    }
}

void CurveDesigGridCell::UpdateText()
{
    if (TestZoneCurveGridCtrl::curveFileDesigStrings.IsEmpty())
        return;

    if (testZoneCurves.IsValid(tzcIndex))
    {
        char* currID = testZoneCurves.GetRef(tzcIndex).curveID;
        if (StringLength(currID) == 0)
        {
            SetText("???");
        }
        else
        {
            SetText(currID);
        }
    }
    else
    {
        SetText(nullStr);
    }
}

void CurveDesigGridCell::SetTestZoneCurveDesig(int nValue)
{
    if (TestZoneCurveGridCtrl::curveFileDesigStrings.IsEmpty())
        return;
    if (nValue < 0)
        nValue = 0;
    CopyString(testZoneCurves[tzcIndex]->curveID, TestZoneCurveGridCtrl::curveFileDesigStrings[nValue], DC_CurveWithID::curveIDlen);
}

void CurveDesigGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
    aText.RemoveAll();
    aEnable.RemoveAll();
    for (int i = 0; i < TestZoneCurveGridCtrl::curveFileDesigStrings.Size(); i++)
    {
        aText.Add(TestZoneCurveGridCtrl::curveFileDesigStrings[i]);
        aEnable.Add(true);
    }

    if (TestZoneCurveGridCtrl::curveFileDesigStrings.IsEmpty())
    {
        aText.Add("no file set");
        aEnable.Add(false);
    }
}


class SequenceDesigGridCell : public CComboGridCell
{
    friend class    TestZoneCurveGridCtrl;
public:
                    SequenceDesigGridCell(int index);

protected:
    virtual void    GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);
    void            DoUpdateText(const char* currID);
    void            SetSequence(char* seqID, int nValue);

protected:
    const int       tzcIndex;
};


SequenceDesigGridCell::SequenceDesigGridCell(int index) :
    CComboGridCell(NULL, alignLeft),
    tzcIndex(index)
{
    if (TestZoneCurveGridCtrl::sequenceDesigStrings.IsEmpty())
    {
        SetText("WTF ???");
        SetReadOnly();
    }
}


void SequenceDesigGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
    aText.RemoveAll();
    aEnable.RemoveAll();
    for (int i = 0; i < TestZoneCurveGridCtrl::sequenceDesigStrings.Size(); i++)
    {
        aText.Add(TestZoneCurveGridCtrl::sequenceDesigStrings[i]);
        aEnable.Add(true);
    }
}

void SequenceDesigGridCell::DoUpdateText(const char* currID)
{
    if (TestZoneCurveGridCtrl::sequenceDesigStrings.IsEmpty())
        return;

    if (testZoneCurves.IsValid(tzcIndex))
    {
        if (IsNull(currID))
        {
            SetText("???");
        }
        else
        {
            SetText(currID);
        }
        return;
    }

    SetText(nullStr);
}

void SequenceDesigGridCell::SetSequence(char* seqID, int nValue)
{
    if (TestZoneCurveGridCtrl::sequenceDesigStrings.IsEmpty())
        return;
    if (nValue < 0)
        nValue = 0;
    CopyString(seqID,  TestZoneCurveGridCtrl::sequenceDesigStrings[nValue], Sequence::seqIDLen);
}



class StartSequenceDesigGridCell : public SequenceDesigGridCell
{
    friend class    TestZoneCurveGridCtrl;
public:
                    StartSequenceDesigGridCell(int index)
                            : SequenceDesigGridCell(index) {};
    virtual void    UpdateText();
    void            SetStartSequence(int nValue);
};

void StartSequenceDesigGridCell::UpdateText()
{
    if (testZoneCurves.IsValid(tzcIndex))
    {
        DoUpdateText(testZoneCurves.GetRef(tzcIndex).startSeqID);
    }
    else
    {
        DoUpdateText(NULL);
    }
}


void StartSequenceDesigGridCell::SetStartSequence(int nValue)
{
    TestZoneCurve& currTZC = testZoneCurves.GetRef(tzcIndex);
    SetSequence(currTZC.startSeqID, nValue);
    if (StringLength(currTZC.endSeqID) == 0)
        SetSequence(currTZC.endSeqID, nValue);

}


class EndSequenceDesigGridCell : public SequenceDesigGridCell
{
    friend class    TestZoneCurveGridCtrl;
public:
                    EndSequenceDesigGridCell(int index)
                            : SequenceDesigGridCell(index) {};
    virtual void    UpdateText();
    void            SetEndSequence(int nValue);
};

void EndSequenceDesigGridCell::UpdateText()
{
    if (testZoneCurves.IsValid(tzcIndex))
    {
        DoUpdateText(testZoneCurves.GetRef(tzcIndex).endSeqID);
    }
    else
    {
        DoUpdateText(NULL);
    }
}


void EndSequenceDesigGridCell::SetEndSequence(int nValue)
{
    SetSequence(testZoneCurves[tzcIndex]->endSeqID, nValue);
}


// TestZoneCurves /////////////////////////////////////////////////////////////////////////////////

class TZDataGridCell : public CTextGridCell
{
    friend class TestZoneCurveGridCtrl;
public:
    TZDataGridCell(int index, const char* szText = NULL, Alignment alignment = alignRight);

    virtual void UpdateText();
protected:
    virtual bool CanEdit();
    virtual CWnd* CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar);

protected:
    const int tzcIndex;
};

TZDataGridCell::TZDataGridCell(int index, const char* szText, Alignment alignment) :
    CTextGridCell(szText, alignment),
    tzcIndex(index)
{
}


void TZDataGridCell::UpdateText()
{
    if (testZoneCurves.IsNotValid(tzcIndex))
    {
        SetText(nullStr);
        return;
    }

    TestZoneCurve& currTZC = testZoneCurves.GetRef(tzcIndex);

    CString sText;
    SC_SetupErr errData;

    // default values
    bool crvOK = currTZC.SetupOK(errData);
    if (crvOK)
    {
        sText = "OK ";
    }
    else
    {
        sText = "BAD ";
    }

    SetText(sText);
}

bool TZDataGridCell::CanEdit()
{
    return testZoneCurves[tzcIndex] != 0;
}

CWnd* TZDataGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
    if (testZoneCurves.IsNotValid(tzcIndex))
        return NULL;

    TestZoneCurve& currTZC = testZoneCurves.GetRef(tzcIndex);

    CSequenceTestZoneCurveDlg dlg(currTZC);
    dlg.DoModal();

    pParent->SetFocus();
    static_cast<CGridCtrlCell*>(pParent)->FinishEdit(NULL, -1, nFirstChar);
    return NULL;
}




///////////////////////////////////////////////////////////////////////////////////

SC_StringArray  TestZoneCurveGridCtrl::curveTypeStrings;
SC_IntArray     TestZoneCurveGridCtrl::curveTypeMap;

SC_StringArray  TestZoneCurveGridCtrl::curveFileDesigStrings;
SC_StringArray  TestZoneCurveGridCtrl::sequenceDesigStrings;

SC_StringArray  TestZoneCurveGridCtrl::curveFOStrings;
FuncObjRefArray TestZoneCurveGridCtrl::curveFOData;
bool            TestZoneCurveGridCtrl::curveFOOK;

TestZoneCurveGridCtrl::TestZoneCurveGridCtrl() :
    CGridCtrlCell()
{
}

void TestZoneCurveGridCtrl::ResetTestZoneCurveGrid()
{
    // start with a clean slate
    sequence.SequenceCleanup();

    // set type strings
    curveTypeStrings.Alloc(6);
    curveTypeMap.Alloc(6);

    // always valid
    curveTypeStrings += "Pressure";
    curveTypeMap += TestZoneCurve::tzPressure;

    curveTypeStrings += "Flow";
    curveTypeMap += TestZoneCurve::tzFlowRate;

    if (testZoneTemperatureVaries && control.IsLiquid())
    {
        curveTypeStrings += "Temperature";
        curveTypeMap += TestZoneCurve::tzTemperature;
    }

    if (testZoneCompressibilityVaries && control.IsLiquid())
    {
        curveTypeStrings += "Compressibility";
        curveTypeMap += TestZoneCurve::tzCompressibility;
    }

    if (testZoneVolumeVaries)
    {
        curveTypeStrings += "Volume Change";
        curveTypeMap += TestZoneCurve::tzDeltaVolume;
        curveTypeStrings += "Volume";
        curveTypeMap += TestZoneCurve::tzAbsVolume;
    }

    // curve file desigs
    if (control.UseCurveFiles())
    {
        nsCurveFileStatics::testZoneBCCurveFile.GetIDs(curveFileDesigStrings);
    }
    else
    {
        curveFOOK = AppMenuC::GetFuncObjList(typeid(DO_Curve), curveFOStrings, curveFOData, "   ");
    }

    // and sequences
    sequence.GetSequenceIDs(sequenceDesigStrings);


    // make sure there are 20 blanks
    testZoneCurves.ReAlloc(testZoneCurves.Size() + 20);
    SetRowCount(testZoneCurves.AllocSize() + 1);
    SetColCount(6);

    SetCenteredText(0, 1, "Type" );
    if (control.UseCurveFiles())
        SetCenteredText(0, 2, "Curve ID" );
    else
        SetCenteredText(0, 2, "Curve Object" );

    SetCenteredText(0, 3, "Start Sequence" );
    SetCenteredText(0, 4, "End Sequence" );
    SetCenteredText(0, 5, "Curve Data" );

    CString sRowHeader;
    for (int i = 0; i < testZoneCurves.AllocSize(); i++) {

        int row = i + 1;
        sRowHeader.Format("%d", row);
        SetCenteredText(row, 0, sRowHeader );

        SetCell(row, 1, new CurveTypeGridCell(i));
        if (control.UseCurveFiles())
            SetCell(row, 2, new CurveDesigGridCell(i));
        else
            SetCell(row, 2, new CurveFOGridCell(i));

        SetCell(row, 3, new StartSequenceDesigGridCell(i));
        SetCell(row, 4, new EndSequenceDesigGridCell(i));
        SetCell(row, 5, new TZDataGridCell(i));

        // reset all data
        for (int j = 1; j < 6; j++)
            ResetCell(i + 1, j);
    }

    SetEditMode();
    SetBorderThickness(4);
    AutoSize();
    for (int j = 1; j < 6; j++)
        SetColSize(j, GetColSize(j) + 15);

    InvalidateAll();
}


BEGIN_MESSAGE_MAP(TestZoneCurveGridCtrl,CGridCtrlCell)
    //{{AFX_MSG_MAP(TestZoneCurveGridCtrl)
    ON_WM_RBUTTONDOWN()
    ON_WM_KEYDOWN()
    //}}AFX_MSG_MAP
    ON_COMMAND_RANGE(WM_TZCM_INS_BEFORE, WM_TZCM_DELETE, OnPopupCommand)
END_MESSAGE_MAP()


void TestZoneCurveGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
    CGridCellIndices currCell = GetCellIndices(point);
    int currRow = currCell.m_nRow;
    if ((currRow <= 0) || (currRow > testZoneCurves.Size()))
        return;

    m_currTZC = currRow - 1;

    CMenu popMenu;
    popMenu.CreatePopupMenu();

    UINT insFlags = MF_STRING;
    if (testZoneCurves.IsNotValid(m_currTZC))
        insFlags = MF_STRING | MF_GRAYED;

    VERIFY(popMenu.AppendMenu(insFlags, WM_TZCM_INS_BEFORE, "Insert Before" ));
    VERIFY(popMenu.AppendMenu(insFlags, WM_TZCM_INS_AFTER,  "Insert After" ));
    VERIFY(popMenu.AppendMenu(insFlags, WM_TZCM_DUPLICATE,  "Duplicate" ));
    VERIFY(popMenu.AppendMenu(MF_STRING, WM_TZCM_DELETE,    "Delete" ));

    ClientToScreen(&point);
    popMenu.TrackPopupMenu( TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this );

}


void TestZoneCurveGridCtrl::DeleteTestZoneCurve(int index)
{
    testZoneCurves.DeletePtr(m_currTZC);
}

void TestZoneCurveGridCtrl::InsertTestZoneCurve(int index)
{
    if (!testZoneCurves.InsertPtr(index))
        return;
    sequence.CheckAndAllocTestZoneCurve(index);
}


void TestZoneCurveGridCtrl::UpdateModifiedRows()
{
    for (int i = m_currTZC + 1; i < GetRowCount(); i++)
        for (int j = 1; j < GetColCount(); j++)
        {
            CGridCell* pCell = GetCell(i, j);
            pCell->UpdateText();
        }
    InvalidateAll();
}

void TestZoneCurveGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // is it a valid sequence row ?
    m_currTZC = m_cellActive.m_nRow - 1;
    if (m_currTZC >= 0)
    {
        bool opDone = false;
        switch (nChar) {
            case VK_DELETE: {
                // cannot delete only row
                if ((testZoneCurves.Size() > 1) || (m_currTZC > 0))
                {
                    DeleteTestZoneCurve(m_currTZC);
                    opDone = true;
                }
                break;
            }
            case VK_INSERT: {
                // must be valid
                if (testZoneCurves.IsValid(m_currTZC))
                {
                    InsertTestZoneCurve(m_currTZC);
                    // update can effect the row before
                    if (m_currTZC > 0)
                        m_currTZC--;
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


void TestZoneCurveGridCtrl::OnPopupCommand(UINT nMenuID)
{
    switch (nMenuID)
    {
        case WM_TZCM_INS_BEFORE : {
            InsertTestZoneCurve(m_currTZC);
            if (m_currTZC > 0)
                m_currTZC--;
            break;
        }
        case WM_TZCM_INS_AFTER : {
            InsertTestZoneCurve(m_currTZC + 1);
            break;
        }
        case WM_TZCM_DUPLICATE : {
            if (testZoneCurves.InsertPtr(m_currTZC))
            {
                testZoneCurves.tListData[m_currTZC] = new TestZoneCurve(*testZoneCurves.tListData[m_currTZC + 1]);
            }
            break;
        }
        case WM_TZCM_DELETE : {
            DeleteTestZoneCurve(m_currTZC);
            break;
        }
        default: {ASSERT(0);}
    }

    UpdateModifiedRows();

}


void TestZoneCurveGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
    int tzcIndex = m_cellActive.m_nRow - 1;

    bool updateAll = (testZoneCurves.IsNotValid(tzcIndex));

    sequence.CheckAndAllocTestZoneCurve(tzcIndex);
    TestZoneCurve& currTZC = testZoneCurves.GetRef(tzcIndex);

    // Change values in Grid
    CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
    switch (m_cellActive.m_nCol) {
    case 1: // Type
        {
            static_cast<CurveTypeGridCell*>(pCell)->SetTestZoneCurveType(nValue);
            break;

        }
    case 2: // ID
        {
            if (control.UseCurveFiles())
                static_cast<CurveDesigGridCell*>(pCell)->SetTestZoneCurveDesig(nValue);
            else
                static_cast<CurveFOGridCell*>(pCell)->SetTestZoneCurveFO(nValue);
            break;
        }
    case 3: // start seq
        {
            static_cast<StartSequenceDesigGridCell*>(pCell)->SetStartSequence(nValue);
            ResetCell(m_cellActive.m_nRow, 4);
            break;
        }
    case 4: // end seq
        {
            static_cast<EndSequenceDesigGridCell*>(pCell)->SetEndSequence(nValue);
            break;
        }
    case 5: // seq data or duration status
        {
        }
        break;
    }


    if (updateAll)
    {
        for (int i = 1; i < GetColCount(); i++)
            ResetCell(m_cellActive.m_nRow, i);
    }
    else
    {
        ResetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
        ResetCell(m_cellActive.m_nRow, 5);
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

BOOL DlgSeqTestZoneCurve::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_grid.SubclassDlgItem(IDC_GRID, this);

    SetControlInfo(IDC_GRID, resizeBoth);

    return TRUE;

}

void DlgSeqTestZoneCurve::ResetAllControls()
{
    m_grid.ResetTestZoneCurveGrid();
}

