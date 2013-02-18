///////////////////////////////////////////////////////////////////////////////////
//
// DlgDataCaptureMain.cpp
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
//      nPre dialog.  Why are there class definitions in this file??!
//
///////////////////////////////////////////////////////////////////////////////////

// DlgDataCaptureMain.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <genClass/U_Str.h>

#include <nsClass/nSightConst.h>
#include <Var/G_DataCapture.h>
#include <Var/G_Control.h>
#include <Dlg/UnconfinedObsWellDlg.h>
#include <Dlg/HorizontalAnisotropyObsWellDlg.h>

#include "PopupCmdDefines.h"

#include "DlgDataCaptureMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
// DlgDataCaptureMain dialog

IMPLEMENT_DYNCREATE(DlgDataCaptureMain, CGenericDialog)

DlgDataCaptureMain::DlgDataCaptureMain(CWnd* pParent /*=NULL*/) : CGenericDialog(DlgDataCaptureMain::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgDataCaptureMain)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

DlgDataCaptureMain::~DlgDataCaptureMain()
{
}

void DlgDataCaptureMain::DoDataExchange(CDataExchange* pDX)
{
    CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgDataCaptureMain)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgDataCaptureMain, CGenericDialog)
    //{{AFX_MSG_MAP(DlgDataCaptureMain)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


#include <genGrid/TextGridCell.h>
#include <genGrid/ComboGridCell.h>
#include <genUnits/UnitGridCell.h>

class DcapDesigGridCell : public CTextGridCell
{
    friend class DataCaptureGridCtrl;
public:
                    DcapDesigGridCell(int index);

    virtual void    UpdateText();

protected:
    const int       dCapIndex;
};

DcapDesigGridCell::DcapDesigGridCell(int index) :
    CTextGridCell(NULL, alignLeft),
    dCapIndex(index)
{
}


void DcapDesigGridCell::UpdateText()
{
    if (dataCaptureData.IsValid(dCapIndex))
        SetText(dataCaptureData.GetRef(dCapIndex).dataDesig);
    else
        SetText(nullStr);
}



class CaptureTypeGridCell : public CComboGridCell
{
    friend class DataCaptureGridCtrl;
public:
                    CaptureTypeGridCell(int index);

    virtual void    UpdateText();
    void            SetDataCaptureType(int nValue);

protected:
    virtual void    GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);

protected:
    const int       dCapIndex;
};


CaptureTypeGridCell::CaptureTypeGridCell(int index) :
    CComboGridCell(NULL, alignLeft),
    dCapIndex(index)
{
}

void CaptureTypeGridCell::UpdateText()
{
    int enumVal = -1;
    if (dataCaptureData.IsValid(dCapIndex))
        enumVal = int(dataCaptureData[dCapIndex]->captureType);
    SetEnumText(enumVal, DataCaptureGridCtrl::captureTypeStrings);
}


void CaptureTypeGridCell::SetDataCaptureType(int nValue)
{
    if (nValue < 0)
        nValue = 0;
    dataCaptureData[dCapIndex]->ChangeCaptureType(DataCaptureSpec::CaptureType(nValue));
    dataCaptureData[dCapIndex]->ClearWasActive();
}

void CaptureTypeGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
    SetEnumSelectionList(aText, aEnable, DataCaptureGridCtrl::captureTypeStrings);
}

class CaptureSubTypeGridCell : public CComboGridCell
{
    friend class DataCaptureGridCtrl;
public:
                    CaptureSubTypeGridCell(int index);

    virtual void    UpdateText();
    void            SetDataCaptureSubType(int nValue);

protected:
    virtual void    GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);

protected:
    const int       dCapIndex;
};


CaptureSubTypeGridCell::CaptureSubTypeGridCell(int index) :
    CComboGridCell(NULL, alignLeft),
    dCapIndex(index)
{
}

void CaptureSubTypeGridCell::UpdateText()
{
    int enumVal = -1;
    SC_StringArray* enumStrs = &DataCaptureGridCtrl::captureTypeStrings;
    if (dataCaptureData.IsValid(dCapIndex))
    {
        DataCaptureSpec& currRef = dataCaptureData.GetRef(dCapIndex);
        switch (currRef.captureType)
        {
            case DataCaptureSpec::ctPressure : {
                enumVal = int(currRef.pressureCapType);
                enumStrs = &DataCaptureGridCtrl::pressureCapTypeStrings;
                break;
            }
            case DataCaptureSpec::ctFlow : {
                enumVal = int(currRef.flowCapType);
                enumStrs = &DataCaptureGridCtrl::flowCapTypeStrings;
                break;
            }
            case DataCaptureSpec::ctProduction : {
                enumVal = int(currRef.productionCapType);
                enumStrs = &DataCaptureGridCtrl::productionCapTypeStrings;
                break;
            }
            case DataCaptureSpec::ctTestZone : {
                enumVal = int(currRef.testZoneCapType);
                enumStrs = &DataCaptureGridCtrl::testZoneCapTypeStrings;
                break;
            }
            case DataCaptureSpec::ctWaterTable : {
                enumVal = 0;
                enumStrs = &DataCaptureGridCtrl::waterTableCapTypeStrings;
                break;
            }
        }
    }

    SetEnumText(enumVal, *enumStrs);
}



void CaptureSubTypeGridCell::SetDataCaptureSubType(int nValue)
{
    if (nValue < 0)
        nValue = 0;

    DataCaptureSpec& currRef = dataCaptureData.GetRef(dCapIndex);
    switch (currRef.captureType)
    {
        case DataCaptureSpec::ctPressure : {
            currRef.pressureCapType = DataCaptureSpec::PressureCapType(nValue);
            if (currRef.pressureCapType != DataCaptureSpec::pctSuperposition)
                currRef.superData.SetConstant();
            break;
        }
        case DataCaptureSpec::ctFlow : {
            currRef.flowCapType = DataCaptureSpec::FlowCapType(nValue);
            break;
        }
        case DataCaptureSpec::ctProduction : {
            currRef.productionCapType = DataCaptureSpec::ProductionCapType(nValue);
            break;
        }
        case DataCaptureSpec::ctTestZone : {
            currRef.testZoneCapType = DataCaptureSpec::TestZoneCapType(nValue);
            break;
        }
        case DataCaptureSpec::ctWaterTable : {
            break;
        }
    }
    currRef.SetOutputUnits();
}

void CaptureSubTypeGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
    SC_StringArray* enumStrs = &DataCaptureGridCtrl::captureTypeStrings;
    if (dataCaptureData.IsValid(dCapIndex))
    {
        DataCaptureSpec& currRef = dataCaptureData.GetRef(dCapIndex);
        switch (currRef.captureType)
        {
            case DataCaptureSpec::ctPressure : {
                enumStrs = &DataCaptureGridCtrl::pressureCapTypeStrings;
                break;
            }
            case DataCaptureSpec::ctFlow : {
                enumStrs = &DataCaptureGridCtrl::flowCapTypeStrings;
                break;
            }
            case DataCaptureSpec::ctProduction : {
                enumStrs = &DataCaptureGridCtrl::productionCapTypeStrings;
                break;
            }
            case DataCaptureSpec::ctTestZone : {
                enumStrs = &DataCaptureGridCtrl::testZoneCapTypeStrings;
                break;
            }
            case DataCaptureSpec::ctWaterTable : {
                enumStrs = &DataCaptureGridCtrl::waterTableCapTypeStrings;
                break;
            }
        }
    }
    SetEnumSelectionList(aText, aEnable, *enumStrs);
}


// DataCaptures /////////////////////////////////////////////////////////////////////////////////

class DataCaptureRadiusGridCell : public CTextGridCell
{
    friend class DataCaptureGridCtrl;
public:
    DataCaptureRadiusGridCell(int index, const char* szText = NULL, Alignment alignment = alignRight);

    virtual void UpdateText();
    virtual bool CanEdit();
protected:
    virtual CWnd* CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar);

protected:
    const int dCapIndex;
};

DataCaptureRadiusGridCell::DataCaptureRadiusGridCell(int index, const char* szText, Alignment alignment) :
    CTextGridCell(szText, alignment),
    dCapIndex(index)
{
}


void DataCaptureRadiusGridCell::UpdateText()
{
    if (dataCaptureData.IsNotValid(dCapIndex))
    {
        SetText(nullStr);
        return;
    }

    DataCaptureSpec& currRef = dataCaptureData.GetRef(dCapIndex);

    CString sText;
    sText = "n/a";

    //  only valid for pressure
    if (currRef.captureType == DataCaptureSpec::ctPressure)
    {
        switch (currRef.pressureCapType) {
            case  DataCaptureSpec::pctTestZone : {
                break;
            }
            case  DataCaptureSpec::pctRadius :{
                if (control.Is2DRadial() || control.IsHorizontalAnisotropic())
                {
                    SC_SetupErr errData;
                    if (currRef.SetupOK(errData))
                    {
                        sText = "OK ";
                    }
                    else
                    {
                        sText = "BAD ";
                    }
                    sText += "/menu";
                }
                else
                    currRef.radiusData.GetUserString(sText);
                break;
            }
            case  DataCaptureSpec::pctSuperposition : {
                SC_SetupErr errData;
                // default values
                if (currRef.SetupOK(errData))
                {
                    sText = "OK ";
                }
                else
                {
                    sText = "BAD ";
                }
            }
        }
    }
    else if (currRef.captureType == DataCaptureSpec::ctWaterTable)
    {
        currRef.radiusData.GetUserString(sText);
    }

    SetText(sText);
}


bool DataCaptureRadiusGridCell::CanEdit()
{
    return  dataCaptureData.IsValid(dCapIndex) && dataCaptureData.GetRef(dCapIndex).UsesRadius();
}


CWnd* DataCaptureRadiusGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
    if (dataCaptureData.IsValid(dCapIndex))
    {
        DataCaptureSpec& currRef = dataCaptureData.GetRef(dCapIndex);

        if (currRef.UsesRadius())
        {
            if (currRef.IsPressure() && control.Is2DRadial())
            {
                CUnconfinedObsWellDlg dlg(currRef);
                dlg.DoModal();
            }
            else if (currRef.IsPressure() && control.IsHorizontalAnisotropic())
            {
                CHorizontalAnisotropyObsWellDlg dlg(currRef);
                dlg.DoModal();
            }
            else
            {
                CString loLim, upLim;
                currRef.radiusData.UnitIndex::GetUserString(0.001, loLim);
                currRef.radiusData.UnitIndex::GetUserString(1.0E+05, upLim);
                GenAppParameterRangeMsg(loLim, upLim);
                return CTextGridCell::CreateEditWnd(rect, pParent, nID, nRow, nCol, nFirstChar);
            }
        }
    }

    pParent->SetFocus();
    static_cast<CGridCtrlCell*>(pParent)->FinishEdit(NULL, -1, nFirstChar);
    return NULL;
}


class RadUnitsGridCell : public CUnitGridCell
{
    friend class DataCaptureGridCtrl;
public:
    RadUnitsGridCell(int index, Alignment alignment  = alignRight);
    virtual void UpdateText();

protected:
    const int dCapIndex;
};

RadUnitsGridCell::RadUnitsGridCell(int index, Alignment alignment) :
    CUnitGridCell(uDist, -1, alignment),
    dCapIndex(index)
{
}

void RadUnitsGridCell::UpdateText()
{
    if (dataCaptureData.IsNotValid(dCapIndex))
        SetText(nullStr);
    else
    {
        DataCaptureSpec& currRef = dataCaptureData.GetRef(dCapIndex);
        if (currRef.UsesRadiusUnits())
            SetText(currRef.radiusData.GetUnitString());
        else
            SetText("n/a");
    }
}

class OutputUnitsGridCell : public CUnitGridCell
{
    friend class DataCaptureGridCtrl;
public:
    OutputUnitsGridCell(int index, Alignment alignment  = alignRight);
    virtual void UpdateText();

protected:
    const int dCapIndex;
};

OutputUnitsGridCell::OutputUnitsGridCell(int index, Alignment alignment) :
    CUnitGridCell(uPress, -1, alignment),
    dCapIndex(index)
{
    if (dataCaptureData.IsValid(dCapIndex))
    {
        SetUnitType(dataCaptureData.GetRef(dCapIndex).outputUnits.unitType);
    }
}

void OutputUnitsGridCell::UpdateText()
{
    if (dataCaptureData.IsNotValid(dCapIndex))
    {
        SetText(nullStr);
    }
    else
    {
        SetText(dataCaptureData.GetRef(dCapIndex).outputUnits.GetUnitString());
    }
}



///////////////////////////////////////////////////////////////////////////////////

SC_StringArray  DataCaptureGridCtrl::captureTypeStrings;
SC_StringArray  DataCaptureGridCtrl::pressureCapTypeStrings;
SC_StringArray  DataCaptureGridCtrl::flowCapTypeStrings;
SC_StringArray  DataCaptureGridCtrl::productionCapTypeStrings;
SC_StringArray  DataCaptureGridCtrl::testZoneCapTypeStrings;
SC_StringArray  DataCaptureGridCtrl::waterTableCapTypeStrings;

DataCaptureGridCtrl::DataCaptureGridCtrl() :
    CGridCtrlCell()
{
}

void DataCaptureGridCtrl::ResetDataCaptureGrid()
{
    // start with a clean slate
    dataCapture.DataCaptureCleanup();

    // set type strings
    captureTypeStrings.Alloc(5);

    // always valid
    captureTypeStrings += "Pressure";
    captureTypeStrings += "Flow";
    captureTypeStrings += "Production";
    if (!control.IsGas())
        captureTypeStrings += "Test Zone";
    if (control.IsUnconfined())
        captureTypeStrings += "Water Table";

    pressureCapTypeStrings.Alloc(3);
    pressureCapTypeStrings += "Test Zone";
    pressureCapTypeStrings += "Observation Well";
    pressureCapTypeStrings += "Superposition";

    flowCapTypeStrings.Alloc(4);
    flowCapTypeStrings += "Well";
    flowCapTypeStrings += "Formation";
    flowCapTypeStrings += "Test Zone";
    flowCapTypeStrings += "Wellbore Storage";

    productionCapTypeStrings.Alloc(4);
    productionCapTypeStrings += "Well";
    productionCapTypeStrings += "Formation";
    productionCapTypeStrings += "Test Zone";
    productionCapTypeStrings += "Wellbore Storage";

    testZoneCapTypeStrings.Alloc(3);
    testZoneCapTypeStrings += "TZ Temp.";
    testZoneCapTypeStrings += "TZ Comp.";
    testZoneCapTypeStrings += "TZ Volume";

    waterTableCapTypeStrings.Alloc(1);
    waterTableCapTypeStrings += "Observation Well";

    // make sure there are 20 blanks
    dataCaptureData.ReAlloc(dataCaptureData.Size() + 20);
    SetRowCount(dataCaptureData.AllocSize() + 1);
    SetColCount(7);

    SetCenteredText(0, 1, "Well ID" );
    SetCenteredText(0, 2, "Output Type" );
    SetCenteredText(0, 3, "Sub-Type" );
    SetCenteredText(0, 4, "Radius" );
    SetCenteredText(0, 5, "RadiusUnits" );
    SetCenteredText(0, 6, "Output Units" );

    CString sRowHeader;
    for (int i = 0; i < dataCaptureData.AllocSize(); i++) {

        int row = i + 1;
        sRowHeader.Format("%d", row);
        SetCenteredText(row, 0, sRowHeader );

        SetCell(row, 1, new DcapDesigGridCell(i));
        SetCell(row, 2, new CaptureTypeGridCell(i));
        SetCell(row, 3, new CaptureSubTypeGridCell(i));
        SetCell(row, 4, new DataCaptureRadiusGridCell(i));
        SetCell(row, 5, new RadUnitsGridCell(i));
        SetCell(row, 6, new OutputUnitsGridCell(i));

        // reset all data
        for (int j = 1; j < 7; j++)
            ResetCell(i + 1, j);
    }

    SetEditMode();
    SetBorderThickness(4);
    AutoSize();
    for (int j = 1; j < 7; j++)
        SetColSize(j, GetColSize(j) + 15);

    InvalidateAll();
}


BEGIN_MESSAGE_MAP(DataCaptureGridCtrl,CGridCtrlCell)
    //{{AFX_MSG_MAP(DataCaptureGridCtrl)
    ON_WM_RBUTTONDOWN()
    ON_WM_KEYDOWN()
    //}}AFX_MSG_MAP
    ON_COMMAND_RANGE(WM_DCAPM_INS_BEFORE, WM_DCAPM_DELETE, OnPopupCommand)
END_MESSAGE_MAP()


void DataCaptureGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
    CGridCellIndices currCell = GetCellIndices(point);
    int currRow = currCell.m_nRow;
    if ((currRow <= 0) || (currRow > dataCaptureData.Size()))
        return;

    m_currDCap = currRow - 1;

    CMenu popMenu;
    popMenu.CreatePopupMenu();

    UINT insFlags = MF_STRING;
    if (dataCaptureData.IsNotValid(m_currDCap))
        insFlags = MF_STRING | MF_GRAYED;

    VERIFY(popMenu.AppendMenu(insFlags, WM_DCAPM_INS_BEFORE,    "Insert Before" ));
    VERIFY(popMenu.AppendMenu(insFlags, WM_DCAPM_INS_AFTER, "Insert After" ));
    VERIFY(popMenu.AppendMenu(insFlags, WM_DCAPM_DUPLICATE, "Duplicate" ));
    VERIFY(popMenu.AppendMenu(MF_STRING, WM_DCAPM_DELETE,   "Delete" ));

    ClientToScreen(&point);
    popMenu.TrackPopupMenu( TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this );

}


void DataCaptureGridCtrl::DeleteDataCapture(int index)
{
    dataCaptureUIChange = true;
    dataCaptureData.DeletePtr(index);
}

void DataCaptureGridCtrl::InsertDataCapture(int index)
{
    dataCaptureUIChange = true;
    if (!dataCaptureData.InsertPtr(index))
        return;
    dataCapture.CheckAndAllocDataCapture(index);
}


void DataCaptureGridCtrl::UpdateModifiedRows()
{
    for (int i = m_currDCap + 1; i < GetRowCount(); i++)
        for (int j = 1; j < GetColCount(); j++)
        {
            CGridCell* pCell = GetCell(i, j);
            pCell->UpdateText();
        }
    InvalidateAll();
}

void DataCaptureGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // is it a valid sequence row ?
    m_currDCap = m_cellActive.m_nRow - 1;
    if (m_currDCap >= 0)
    {
        bool opDone = false;
        switch (nChar) {
            case VK_DELETE: {
                // cannot delete top 2 rows
                if (m_currDCap > 1)
                {
                    DeleteDataCapture(m_currDCap);
                    opDone = true;
                }
                break;
            }
            case VK_INSERT: {
                // must be valid
                if (dataCaptureData.IsValid(m_currDCap))
                {
                    InsertDataCapture(m_currDCap);
                    // update can effect the row before
                    if (m_currDCap > 0)
                        m_currDCap--;
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


void DataCaptureGridCtrl::OnPopupCommand(UINT nMenuID)
{
    switch (nMenuID)
    {
        case WM_DCAPM_INS_BEFORE : {
            InsertDataCapture(m_currDCap);
            if (m_currDCap > 0)
                m_currDCap--;
            break;
        }
        case WM_DCAPM_INS_AFTER : {
            InsertDataCapture(m_currDCap + 1);
            break;
        }
        case WM_DCAPM_DUPLICATE : {
            if (dataCaptureData.InsertPtr(m_currDCap))
            {
                dataCaptureData.tListData[m_currDCap] = new DataCaptureSpecGlob(*dataCaptureData.tListData[m_currDCap + 1]);
            }
            break;
        }
        case WM_DCAPM_DELETE : {
            if (m_currDCap > 1)
                DeleteDataCapture(m_currDCap);
            break;
        }
        default: {ASSERT(0);}
    }

    UpdateModifiedRows();

}


void DataCaptureGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
    dataCaptureUIChange = true;

    int dCapIndex = m_cellActive.m_nRow - 1;

    bool updateAll = (dataCaptureData.IsNotValid(dCapIndex));
    bool updateDesig = false;

    bool updateUnits = false;
    bool updateRadius = false;

    dataCapture.CheckAndAllocDataCapture(dCapIndex);
    DataCaptureSpec& currRef = dataCaptureData.GetRef(dCapIndex);

    // Change values in Grid
    CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
    switch (m_cellActive.m_nCol) {
    case 1: // desig
        {
            if (!SameString(currRef.dataDesig, szText))
                currRef.makeDefaultDataDesig = false;

            CopyString(currRef.dataDesig, szText, DataCaptureSpec::dataDesigLen);

            if ((dCapIndex == 0) && (currRef.IsWell() && currRef.IsPressure()))
            {
                // changing main well name -- change for flow too
                if (dataCaptureData.IsValid(1))
                {
                    DataCaptureSpec& flowRef = dataCaptureData.GetRef(1);
                    if (flowRef.IsFlow() && flowRef.IsWell())
                    {
                        CopyString(flowRef.dataDesig, szText, DataCaptureSpec::dataDesigLen);
                        ResetCell(2, 1);
                    }
                }
            }
            // adding new -- assume obs well
            if (updateAll)
            {
                currRef.captureType = DataCaptureSpec::ctPressure;
                currRef.pressureCapType = DataCaptureSpec::pctRadius;
            }
            break;
        }
    case 2: // Type
        {
            static_cast<CaptureTypeGridCell*>(pCell)->SetDataCaptureType(nValue);
            updateAll = true;
            updateDesig = true;
            break;

        }
    case 3: // sub-type
        {
            static_cast<CaptureSubTypeGridCell*>(pCell)->SetDataCaptureSubType(nValue);
            updateUnits = (currRef.captureType == DataCaptureSpec::ctTestZone);
            updateRadius = (currRef.captureType == DataCaptureSpec::ctPressure) || 
                (currRef.captureType == DataCaptureSpec::ctWaterTable); 
            updateDesig = true;
            break;
        }
    case 4: // radii
        {
            if (currRef.UsesRadius())
            {
                if (currRef.IsPressure() && (control.Is2DRadial() || control.IsHorizontalAnisotropic()))
                    ResetCell(m_cellActive.m_nRow, 5);
                else
                    currRef.radiusData.IsWithinRange(szText, 0.001, 1.0E+05);
            }
            break;
        }
    case 5: // radii units
        {
            if (currRef.UsesRadiusUnits())
            {
                currRef.radiusData.SetConvIndex(nValue);
                if (currRef.UsesRadius())
                    ResetCell(m_cellActive.m_nRow, 4);
            }
            break;
        }
    case 6: // output units
        {
            currRef.outputUnits.SetConvIndex(nValue);
            break;
        }

    }


    if (updateAll)
    {
        for (int i = 1; i < GetColCount(); i++)
            ResetCell(m_cellActive.m_nRow, i);
        updateUnits = true;
    }
    else
    {
        ResetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
        if (updateRadius)
        {
            ResetCell(m_cellActive.m_nRow, 4);
            ResetCell(m_cellActive.m_nRow, 5);
        }
        if (updateUnits)
            ResetCell(m_cellActive.m_nRow, 6);
    }

    if (updateUnits)
    {
        OutputUnitsGridCell* currCell = static_cast<OutputUnitsGridCell*>(GetCell(m_cellActive.m_nRow, 6));
        currCell->SetUnitType(currRef.outputUnits.unitType);
    }

    if (updateDesig && dataCapture.SetDataCaptureDesigs())
    {
        for (int i = m_cellActive.m_nRow; i < GetRowCount(); i++)
            ResetCell(i, 1);
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

BOOL DlgDataCaptureMain::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_grid.SubclassDlgItem(IDC_GRID, this);

    SetControlInfo(IDC_GRID, resizeBoth);

    return TRUE;

}

void DlgDataCaptureMain::ResetAllControls()
{
    m_grid.ResetDataCaptureGrid();
}

