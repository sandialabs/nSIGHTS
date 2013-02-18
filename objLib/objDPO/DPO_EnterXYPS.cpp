///////////////////////////////////////////////////////////////////////////////////
//
// DPO_EnterXYPS.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClassPS/PS_UIBase.h>

#include "DPO_EnterXYPS.h"

#include <genClassPS/PS_Import.h>

///////////////////////////////////////////////////////////

// this could lead to problems if multiple menu items are ever open ....
static XYArray xyData;

int XYGridControl::m_nAllowance = 20;

XYArray::XYArray()
{
    SetResizable(32);
}

bool XYArray::XYOK(int i, double& xVal, double& yVal)
{
    ASSERT(i < Size());
    xVal = (*this)[i].x;
    yVal = (*this)[i].y;
    if (xVal == nullReal || yVal == nullReal) {
        return false;
    }
    return true;
}

void XYArray::SetAtGrow(int nIndex, XYItem& newVal)
{
    if (nIndex >= AllocSize())
        ReAlloc(AllocSize() * 2 + 1);
    (*this)[nIndex] = newVal;
    if (nIndex >= Size())
        SetSize(nIndex + 1);
}

class XYCell : public CTextGridCell
{
public:
                    XYCell(int nIndex, XYGridControl* pGrid);
protected:
    int m_nIndex;
    XYGridControl* m_pGrid;
};

class XCell : public XYCell
{
public:
    XCell(int nIndex, XYGridControl* pGrid);

    virtual void    UpdateText();
};

class YCell : public XYCell
{
public:
    YCell(int nIndex, XYGridControl* pGrid);

    virtual void    UpdateText();
};


XYCell::XYCell(int nIndex, XYGridControl* pGrid) :
    CTextGridCell(NULL, alignRight),
    m_nIndex(nIndex),
    m_pGrid(pGrid)
{
}


XCell::XCell(int nIndex, XYGridControl* pGrid) :
    XYCell(nIndex, pGrid)
{
}

void XCell::UpdateText()
{
    char tempStr[80];
    if (m_nIndex >= xyData.Size())
    {
        CopyString(tempStr, "---", 80);
    }
    else
    {
        double xVal = xyData[m_nIndex].x;
        if (RealIsNull(xVal))
        {
            CopyString(tempStr, "---", 80);
        }
        else
        {
            m_pGrid->xForm.RealToString(xVal, tempStr, 80);
        }
    }
    SetText(tempStr);
}

YCell::YCell(int nIndex, XYGridControl* pGrid) :
    XYCell(nIndex, pGrid)
{
}

void YCell::UpdateText()
{
    char tempStr[80];
    if (m_nIndex >= xyData.Size()) {
        CopyString(tempStr, "---", 80);
    } else {
        double yVal = xyData[m_nIndex].y;
        if (RealIsNull(yVal)) {
            CopyString(tempStr, "---", 80);
        } else {
            m_pGrid->yForm.RealToString(yVal, tempStr, 80);
        }
    }
    SetText(tempStr);
}

///////////////////////////////////////////////////////////
XYGridControl::XYGridControl()
{
}

#define WM_INSERT_BEFORE        WM_APP+1000
#define WM_INSERT_AFTER         WM_APP+1001
#define WM_DUPLICATE            WM_APP+1002
#define WM_DELETE               WM_APP+1003
#define WM_PASTEFROMCLIPBOARD   WM_APP+1004
#define WM_COPYTOCLIPBOARD      WM_APP+1005

BEGIN_MESSAGE_MAP(XYGridControl, CGridCtrlCell)
    //{{AFX_MSG_MAP(XYGridControl)
    ON_WM_RBUTTONDOWN()
    ON_WM_KEYDOWN()
    //}}AFX_MSG_MAP
    ON_COMMAND_RANGE(WM_INSERT_BEFORE, WM_COPYTOCLIPBOARD, OnPopupCommand)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
END_MESSAGE_MAP()


void XYGridControl::OnRButtonDown(UINT nFlags, CPoint point)
{
    CGridCellIndices currCell = GetCellIndices(point);
    int currRow = currCell.m_nRow;
    if (currRow <= 0) {
        return;
    }

    m_nCurrent = currRow - 1;

    CMenu popMenu;
    popMenu.CreatePopupMenu();

    UINT insFlags = MF_STRING;
    if (currRow > xyData.Size())
        insFlags |= MF_GRAYED;

    UINT dupFlags = MF_STRING;
    if (currRow > xyData.Size())
        dupFlags |= MF_GRAYED;

    UINT delFlags = MF_STRING;
    if (currRow > xyData.Size())
        delFlags |= MF_GRAYED;

    UINT pasteFlags = MF_STRING;
    if (!IsDataInClipboard()) {
        pasteFlags |= MF_GRAYED;
    }

    UINT copyFlags = MF_STRING;
    if (xyData.Size() < 1)
        copyFlags |= MF_GRAYED;

    VERIFY(popMenu.AppendMenu(insFlags, WM_INSERT_BEFORE,   "Insert Before" ));
    VERIFY(popMenu.AppendMenu(insFlags, WM_INSERT_AFTER,    "Insert After" ));
    VERIFY(popMenu.AppendMenu(delFlags, WM_DELETE,          "Delete" ));

    VERIFY(popMenu.AppendMenu(pasteFlags, WM_PASTEFROMCLIPBOARD, "Paste From Clipboard"));
    VERIFY(popMenu.AppendMenu(copyFlags, WM_COPYTOCLIPBOARD, "Copy To Clipboard"));

    ClientToScreen(&point);
    popMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this);

}

void XYGridControl::DeleteXY(int nIndex)
{
    if (nIndex < xyData.Size()) {
        xyData.DeleteItem(nIndex);
    }
}

void XYGridControl::InsertXY(int nIndex)
{
    xyData.InsertItem(XYItem(), nIndex);
}

void XYGridControl::UpdateModifiedRows()
{
    for (int i = m_nCurrent + 1; i < GetRowCount(); i++) {
        for (int j = 1; j < GetColCount(); j++) {
            CGridCell* pCell = GetCell(i, j);
            pCell->UpdateText();
        }
    }
    InvalidateAll();
}

void XYGridControl::UpdateAllRows()
{
    m_nCurrent = 0;
    UpdateModifiedRows();
}

void XYGridControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // is it a valid sequence row ?
    m_nCurrent = m_cellActive.m_nRow - 1;
    if (m_nCurrent >= 0) {
        bool bOpDone = false;
        switch (nChar) {
            case VK_DELETE:
                // cannot delete only row
                if (xyData.Size() > 1 || m_nCurrent > 0) {
                    DeleteXY(m_nCurrent);
                    bOpDone = true;
                }
                break;
            case VK_INSERT:
                InsertXY(m_nCurrent);
                // update can effect the row before
                if (m_nCurrent > 0) {
                    m_nCurrent--;
                }
                SetXYRows(GetRowCount() + 1);
                bOpDone = true;
                break;
        }

        if (bOpDone) {
            UpdateModifiedRows();
            return;
        }

    }
    CGridCtrlCell::OnKeyDown(nChar, nRepCnt, nFlags);
}

void XYGridControl::OnPopupCommand(UINT nMenuID)
{
    switch (nMenuID) {
        case WM_INSERT_BEFORE:
            InsertXY(m_nCurrent);
            if (m_nCurrent > 0)
                m_nCurrent--;
            break;
        case WM_INSERT_AFTER:
            InsertXY(m_nCurrent + 1);
            break;
        case WM_DELETE:
            DeleteXY(m_nCurrent);
            break;
        case WM_PASTEFROMCLIPBOARD: {
            PasteFromClipboardXY(m_nCurrent);
            break;
        }
        case WM_COPYTOCLIPBOARD: {
            CopyToClipboardXY();
            break;
        }
        default:
            ASSERT(0);
            break;
    }

    UpdateModifiedRows();
}

void XYGridControl::SetXYRows(int nRows)
{
    int nRowCount = GetRowCount();
    SetRowCount(nRows);
    if (nRowCount < nRows) {
        CString str;
        for (int i = nRowCount; i < nRows; i++) {
            str.Format("%d", i);
            SetCenteredText(i, 0, str);
            SetCell(i, 1, new XCell(i-1, this));
            SetCell(i, 2, new YCell(i-1, this));
        }
    }
}

void XYGridControl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
    int nIndex = m_cellActive.m_nRow - 1;

    // Change values in Grid
    CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
    double val;
    switch (m_cellActive.m_nCol) {
    case 1: // X
        if (!IsValidReal(szText, val))
        {
            val = nullReal;
        }
        if (nIndex < xyData.Size())
        {
            xyData[nIndex].x = val;
        }
        else
        {
            xyData.SetAtGrow(nIndex, XYItem(val, 0.0));
        }
        break;
    case 2: // Y
        if (!IsValidReal(szText, val))
        {
            val = nullReal;
        }
        if (nIndex < xyData.Size())
        {
            xyData[nIndex].y = val;
        }
        else
        {
            xyData.SetAtGrow(nIndex, XYItem(0.0, val));
        }
        break;
    }

    ResetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);

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

void XYGridControl::PasteFromClipboardXY(int nIndexFrom)
{
    int nIndex = nIndexFrom;
    PS_Import import;
    if (!import.OpenClipboardImport())
    {
        GenAppWarningMsg("XYGridControl", "Can't open clipboard");
        return;
    }

    SC_DoubleArray xyRowData;
    while (import.GetLineDoubles(xyRowData))
    {
        if (xyRowData.Size() > 1)
        {
            xyData.SetAtGrow(nIndex, XYItem(xyRowData[0], xyRowData[1]));
            nIndex++;
        }
    }

    import.CloseImport();
    int nCount = nIndex - nIndexFrom;
    CString strInfo;
    strInfo.Format("Imported %d rows from clipboard", nCount);
    GenAppInfoMsg("XYGridControl", strInfo);
}


void XYGridControl::CopyToClipboardXY()
{
    PS_TxtClipboard clipOut;

    if (!clipOut.OpenTxtClipboard(true))
    {
        GenAppWarningMsg("TableGridControl", "Can't open clipboard");
        return;
    }

    char tempStr[80];
    for (int i = 0; i < xyData.Size(); i++)
    {
        xForm.RealToString(xyData[i].x, tempStr, 80);
        ConcatString(tempStr, " \t", 80);
        clipOut.WriteSolidText(tempStr);

        yForm.RealToString(xyData[i].y, tempStr, 80);
        ConcatString(tempStr, " \t", 80);
        clipOut.WriteSolidText(tempStr);

        clipOut.WriteLine();
    }


    clipOut.Close();
}


void XYGridControl::OnEditPaste()
{
    if (!IsDataInClipboard()) {
        GenAppWarningMsg("XYGridControl", "No data in clipboard");
    }

    int currRow = GetActiveRow();
    if (currRow < m_nFixedRows || currRow > GetRowCount()) {
        return;
    }

    int nCurrent = currRow - 1;
    PasteFromClipboardXY(nCurrent);
    UpdateModifiedRows();
}

void XYGridControl::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsDataInClipboard());
}

////////////////////////////////////////////////////

DPO_EnterXYPS::DPO_EnterXYPS() :
    inputXYObjRefUI(typeid(DO_XYData))
{
}

void DPO_EnterXYPS::UpdateFromObjCB(void* inObj)
{
    static_cast<DPO_EnterXYPS*>(inObj)->UpdateFromObj();
}

void DPO_EnterXYPS::UpdateFromObj()
{
    if (inputXYObjRefUI.CheckDataObj())
    {
        ClearData();
        const DC_XYData& dataDC = *(static_cast<DO_XYData*>(inputXYObjRefUI.GetDataObj())->xyData);

        SC_RealFormat xForm, yForm;
        xFormatUI.GetRealFormatSettings(xForm);
        yFormatUI.GetRealFormatSettings(yForm);

        m_grid.SetXYRows(1 + dataDC.Size() + XYGridControl::m_nAllowance);
        for (int i = 0; i < dataDC.Size(); i++) {
            TRACE("%.13g\r\n", dataDC.xData[i]);
            xyData.SetAtGrow(i, XYItem(dataDC.xData[i], dataDC.yData[i]));
        }
        m_grid.UpdateAllRows();
        m_grid.AutoSize();
        m_grid.SetEditMode();
    }

}

void DPO_EnterXYPS::UpdateFormatCB(void* inObj)
{
    static_cast<DPO_EnterXYPS*>(inObj)->UpdateFormat();
}

void DPO_EnterXYPS::UpdateFormat()
{
    SC_RealFormat xForm, yForm;
    xFormatUI.GetRealFormatSettings(xForm);
    yFormatUI.GetRealFormatSettings(yForm);

    m_grid.xForm = xForm;
    m_grid.yForm = yForm;
    m_grid.UpdateAllRows();
}

void DPO_EnterXYPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_EnterXYPS*>(inObj)->SetAllSensitive();
}

void DPO_EnterXYPS::SetAllSensitive()
{
    updateFromInput.SetSensitive(inputXYObjRefUI.ListDataOK());
}

void DPO_EnterXYPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach();
    args.BottomFormAttach();
    args.RightPosAttach(80);
    inputXYObjRefUI.InitFramedUI("Input XY Data", URform.GetWidget(), args);
    inputXYObjRefUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
//    args.LeftPosAttach(83);
    args.RightFormAttach(5);
    args.BottomFormAttach(5);
    updateFromInput.StdButtonInit("Update", URform.GetWidget(), args);
    updateFromInput.AddOtherObjectCallback(UpdateFromObjCB, this);

    args.Clear();
    args.StdFormAttach();
    formatForm.InitWidget(PPform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach();
    args.RightPosAttach(50);
    xFormatUI.InitFramedUI("X Data Format", formatForm.GetWidget(), args);
    xFormatUI.formatTypeChooser.AddOtherObjectCallback(UpdateFormatCB, this);
    xFormatUI.formatPrecChooser.AddOtherObjectCallback(UpdateFormatCB, this);

    args.Clear();
    args.TopFormAttach();
    args.LeftPosAttach(50);
    args.RightFormAttach();
    yFormatUI.InitFramedUI("Y Data Format", formatForm.GetWidget(), args);
    yFormatUI.formatTypeChooser.AddOtherObjectCallback(UpdateFormatCB, this);
    yFormatUI.formatPrecChooser.AddOtherObjectCallback(UpdateFormatCB, this);

//  scrolled window in middle
    args.Clear();
    args.StdTopObjectAttach(formatForm.GetWidget());
    args.BottomFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);

    m_grid.InitWidget(PPform.GetWidget(), args);

    m_grid.SetColCount(3);
    m_grid.SetRowCount(1);

    CString str = "           ";
    m_grid.SetText(0, 0, str);
    m_grid.SetCenteredText(0, 1, "                    X                ");
    m_grid.SetCenteredText(0, 2, "                    Y                ");

// SetMainformScrolled(true, 440, 250);
}

void DPO_EnterXYPS::ClearData()
{
    for (int i = 0; i < xyData.Size(); i++)
        xyData.SetAtGrow(i, XYItem(nullReal, nullReal));
    xyData.SetSize(0);
}


void DPO_EnterXYPS::MenuOpen(DPO_EnterXY&   inData)
{
    MenuBaseC::MenuOpen(false, true);
    inputXYObjRefUI.StdMenuInit(inData.inputXYObjRef);
    ClearData();
}

void DPO_EnterXYPS::SetMenuData(DPO_EnterXY&    inData)
{
    inputXYObjRefUI.SetFuncObjRef(inData.inputXYObjRef);
    xFormatUI.SetRealFormatSettings(inData.xFormat);
    yFormatUI.SetRealFormatSettings(inData.yFormat);

    m_grid.xForm = inData.xFormat;
    m_grid.yForm = inData.yFormat;

    m_grid.SetXYRows(1 + inData.xyData.Size() + XYGridControl::m_nAllowance);
    ClearData();
    for (int i = 0; i < inData.xyData.Size(); i++) {
        xyData.SetAtGrow(i, XYItem(inData.xyData.xData[i], inData.xyData.yData[i]));
    }
    m_grid.UpdateAllRows();
    m_grid.AutoSize();
    m_grid.SetEditMode();

    SetAllSensitive();
    MenuBaseC::SetMenuData(inData);
}


bool DPO_EnterXYPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    return allOK;
}

void DPO_EnterXYPS::GetMenuData(DPO_EnterXY& outData)
{
    MenuBaseC::GetMenuData(outData);

    bool allOK = true;
    if (inputXYObjRefUI.FuncObjRefOK(allOK, "Data source")) {
        outData.inputXYObjRef = inputXYObjRefUI.GetFuncObjRef();
    }

    xFormatUI.GetRealFormatSettings(outData.xFormat);
    yFormatUI.GetRealFormatSettings(outData.yFormat);

    outData.xyData.Alloc(xyData.Size());
    int nOK = 0;
    for (int i = 0; i < xyData.Size(); i++)
    {
        if (xyData.XYOK(i, outData.xyData.xData[nOK], outData.xyData.yData[nOK]))
        {
            nOK++;
        }
    }
    outData.xyData.xData.SetSize(nOK);
    outData.xyData.yData.SetSize(nOK);
}

void DPO_EnterXYPS::MenuClear()
{
    ClearData();
    m_grid.UpdateAllRows();
}

