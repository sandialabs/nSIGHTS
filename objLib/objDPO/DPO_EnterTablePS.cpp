///////////////////////////////////////////////////////////////////////////////////
//
// DPO_EnterTablePS.cpp
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
#include <genClassPS/PS_Import.h>

#include "DPO_EnterTablePS.h"


class ColumnCell : public CTextGridCell
{
    protected:
        int                 m_colIndex;
        TableGridControl*   m_pGrid;

    public:
                            ColumnCell(int colIndex, TableGridControl* pGrid);
        void                SetNullStr();
};

class ValueCell : public ColumnCell
{
    public:
                        ValueCell(int colIndex, int rowIndex, TableGridControl* pGrid);
        virtual void    UpdateText();

    protected:
        int             m_rowIndex;

};

class HeaderCell : public ColumnCell
{
    public:
                        HeaderCell(int colIndex, TableGridControl* pGrid);
        virtual void    UpdateText();
};


ColumnCell::ColumnCell(int colIndex, TableGridControl* pGrid) :
    CTextGridCell(NULL, alignRight),
    m_colIndex(colIndex),
    m_pGrid(pGrid)
{
}


void ColumnCell::SetNullStr()
{
    SetText("---");
}

ValueCell::ValueCell(int colIndex, int rowIndex, TableGridControl* pGrid) :
    ColumnCell(colIndex, pGrid),
    m_rowIndex(rowIndex)
{
}

void ValueCell::UpdateText()
{
    DC_TableData& currTab = m_pGrid->gTable;
    if ((m_colIndex >= currTab.GetNCols()) || (m_rowIndex >= currTab.GetNRows()))
    {
        SetNullStr();
        return;
    }

    double currVal = currTab.dataTable[m_colIndex][m_rowIndex];
    if (RealIsNull(currVal))
    {
        SetNullStr();
    }
    else
    {
        char tempStr[80];
        m_pGrid->tableFormat.RealToString(currVal, tempStr, 80);
        SetText(tempStr);
    }
}


HeaderCell::HeaderCell(int colIndex, TableGridControl* pGrid) :
    ColumnCell(colIndex, pGrid)
{
}

void HeaderCell::UpdateText()
{
    DC_TableData& currTab = m_pGrid->gTable;
    if (m_colIndex >= currTab.GetNCols())
        SetNullStr();
    else
        SetText(currTab.columnDesc[m_colIndex]);
}

///////////////////////////////////////////////////////////
TableGridControl::TableGridControl(DC_TableData& currTable, SC_RealFormat& currForm)
    : gTable(currTable), tableFormat(currForm)
{
}

#define WM_INSERT_ROW_BEFORE    WM_APP+1000
#define WM_INSERT_ROW_AFTER     WM_APP+1001
#define WM_DELETE_ROW           WM_APP+1002
#define WM_INSERT_COL_BEFORE    WM_APP+1003
#define WM_INSERT_COL_AFTER     WM_APP+1004
#define WM_DELETE_COL           WM_APP+1005
#define WM_PASTEFROMCLIPBOARD   WM_APP+1006
#define WM_COPYTOCLIPBOARD      WM_APP+1007

BEGIN_MESSAGE_MAP(TableGridControl, CGridCtrlCell)
    //{{AFX_MSG_MAP(TableGridControl)
    ON_WM_RBUTTONDOWN()
    ON_WM_KEYDOWN()
    //}}AFX_MSG_MAP
    ON_COMMAND_RANGE(WM_INSERT_ROW_BEFORE, WM_COPYTOCLIPBOARD, OnPopupCommand)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
END_MESSAGE_MAP()


void TableGridControl::OnRButtonDown(UINT nFlags, CPoint point)
{
    CGridCellIndices currCell = GetCellIndices(point);
    int currRow = currCell.m_nRow - 2;
    int currCol = currCell.m_nCol - 1;
    if ((currRow < 0) && (currCol < 0))
        return;

    m_nCurrRow = currRow;
    if (m_nCurrRow < 0)
        m_nCurrRow = 0;

    m_nCurrCol = currCol;
    if (m_nCurrCol < 0)
        m_nCurrCol = 0;

    CMenu popMenu;
    popMenu.CreatePopupMenu();

    int nRows = gTable.GetNRows();
    int nCols = gTable.GetNCols();

    UINT rowInsFlags = MF_STRING;
    if ((currRow < 0) || (currRow > nRows))
        rowInsFlags |= MF_GRAYED;

    UINT colInsFlags = MF_STRING;
    if ((currCol < 0) || (currCol > nCols))
        colInsFlags |= MF_GRAYED;

    UINT colDelFlags = MF_STRING;
    if ((currCol < 0) || (nCols == 1) || (currCol > nCols))
        colDelFlags |= MF_GRAYED;

    UINT pasteFlags = MF_STRING;
    if (!IsDataInClipboard()) {
        pasteFlags |= MF_GRAYED;
    }

    UINT copyFlags = MF_STRING;
    if ((nRows == 0) || (nCols == 0))
        copyFlags |= MF_GRAYED;


    VERIFY(popMenu.AppendMenu(rowInsFlags, WM_INSERT_ROW_BEFORE,  "Insert Row Before" ));
    VERIFY(popMenu.AppendMenu(rowInsFlags, WM_INSERT_ROW_AFTER,   "Insert Row After" ));
    VERIFY(popMenu.AppendMenu(rowInsFlags, WM_DELETE_ROW,         "Delete Row" ));

    VERIFY(popMenu.AppendMenu(colInsFlags, WM_INSERT_COL_BEFORE,  "Insert Column Before" ));
    VERIFY(popMenu.AppendMenu(colInsFlags, WM_INSERT_COL_AFTER,   "Insert Column After" ));
    VERIFY(popMenu.AppendMenu(colDelFlags, WM_DELETE_COL,         "Delete Column" ));

    VERIFY(popMenu.AppendMenu(pasteFlags, WM_PASTEFROMCLIPBOARD, "Paste From Clipboard"));
    VERIFY(popMenu.AppendMenu(copyFlags, WM_COPYTOCLIPBOARD, "Copy To Clipboard"));

    ClientToScreen(&point);
    popMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this);
}

void TableGridControl::DeleteRow(int rowIndex)
{
    gTable.DeleteRow(rowIndex);
}

void TableGridControl::InsertRow(int rowIndex)
{
    gTable.InsertRow(rowIndex);
}
void TableGridControl::DeleteCol(int colIndex)
{
    gTable.DeleteCol(colIndex);
}

void TableGridControl::InsertCol(int colIndex)
{
    gTable.InsertCol(colIndex);
}

void TableGridControl::UpdateModifiedRows()
{
    for (int i = m_nCurrRow + 1; i < GetRowCount(); i++)
    {
        for (int j = 1; j < GetColCount(); j++)
        {
            CGridCell* pCell = GetCell(i, j);
            pCell->UpdateText();
        }
    }
    InvalidateAll();
}

void TableGridControl::UpdateModifiedCols()
{
    for (int j = m_nCurrCol + 1; j < GetColCount(); j++)
    {
        for (int i = 1; i < GetRowCount(); i++)
        {
            CGridCell* pCell = GetCell(i, j);
            pCell->UpdateText();
        }
    }
    InvalidateAll();
}



void TableGridControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    m_nCurrRow = m_cellActive.m_nRow - 2;
    if (m_nCurrRow >= 0)
    {
        bool bOpDone = false;
        switch (nChar)
        {
            case VK_DELETE: {
                DeleteRow(m_nCurrRow);
                bOpDone = true;
                break;
            }
            case VK_INSERT: {
                InsertRow(m_nCurrRow);
                UpdateSize();
                bOpDone = true;
                break;
            }
            default: {
                break;
            }
        }

        if (bOpDone)
        {
            UpdateModifiedRows();
            return;
        }
    }

    CGridCtrlCell::OnKeyDown(nChar, nRepCnt, nFlags);
}

void TableGridControl::UpdateAll()
{
    m_nCurrRow = 0;
    UpdateModifiedRows();
}

void  TableGridControl::UpdateColSize()
{
    AutoSize();
    int maxSize = GetColSize(1);
    int i;
    for (i = 2; i < GetColCount(); i++)
    {
        int colSize = GetColSize(i);
        if (colSize > maxSize)
            maxSize= colSize;
    }

    maxSize += 5;
    for (i = 1; i < GetColCount(); i++)
        SetColSize(i, maxSize);
}

void TableGridControl::OnPopupCommand(UINT nMenuID)
{
    int nRowAlloc = gTable.dataTable[0].AllocSize();
    bool colOp = false;
    CWaitCursor w;

    switch (nMenuID) {
        case WM_INSERT_ROW_BEFORE: {
            InsertRow(m_nCurrRow);
            break;
        }
        case WM_INSERT_ROW_AFTER: {
            InsertRow(m_nCurrRow + 1);
            break;
        }
        case WM_DELETE_ROW: {
            DeleteRow(m_nCurrRow);
            break;
        }
        case WM_INSERT_COL_BEFORE: {
            InsertCol(m_nCurrCol);
            colOp = true;
            break;
        }
        case WM_INSERT_COL_AFTER: {
            InsertCol(m_nCurrCol + 1);
            colOp = true;
            break;
        }
        case WM_DELETE_COL: {
            DeleteCol(m_nCurrCol);
            colOp = true;
            break;
        }
        case WM_PASTEFROMCLIPBOARD: {
            PasteFromClipboardTable(m_nCurrRow);
            break;
        }
        case WM_COPYTOCLIPBOARD: {
            CopyToClipboardTable();
            break;
        }
        default:
            ASSERT(0);
            break;
    }

    if (colOp || (nRowAlloc != gTable.dataTable[0].AllocSize()))
    {
        UpdateSize();
        UpdateAll();
        UpdateColSize();
    }
    else
        UpdateModifiedRows();
}

void TableGridControl::UpdateSize()
{
    int nRowCount = GetRowCount() - 2;
    int nColCount = GetColCount() - 1;
    int nRows = gTable.dataTable[0].AllocSize();
    int nCols = gTable.dataTable.AllocSize();

    SetRowCount(nRows + 2);
    SetColCount(nCols + 1);

    if (nColCount < nCols)
    {
        CString str;
        for (int i = nColCount; i < nCols; i++)
        {
            str.Format("%d", i + 1);
            SetCenteredText(0, i + 1, str);
            SetCell(1, i + 1, new HeaderCell(i, this));
            for (int j = 0; j < nRows; j++)
            {
                SetCell(j + 2, i + 1, new ValueCell(i, j, this));
            }
        }
        nCols = nColCount;
    }


    if (nRowCount < nRows)
    {
        CString str;
        for (int i = nRowCount; i < nRows; i++)
        {
            if (i < 0)
            {
                SetCenteredText(1, 0, "Col ID");
                for (int j = 0; j < nCols; j++)
                    SetCell(1, j + 1, new HeaderCell(j, this));
            }
            else
            {
                if (i < gTable.rowDesc.Size())
                {
                    SetCenteredText(i + 2, 0, gTable.rowDesc[i]);
                }
                else
                {
                    str.Format("%d", i + 1);
                    SetCenteredText(i + 2, 0, str);
                }
                for (int j = 0; j < nCols; j++)
                    SetCell(i + 2, j + 1, new ValueCell(j, i, this));
            }
        }
    }
}

void TableGridControl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
    int rowIndex = m_cellActive.m_nRow - 2;
    int colIndex = m_cellActive.m_nCol - 1;

    if (rowIndex < 0)
    {
        // header
        gTable.columnDesc.SetString(szText, colIndex);
    }
    else
    {
        double val;
        if (!IsValidReal(szText, val))
        {
            val = nullReal;
        }
        gTable.dataTable[colIndex][rowIndex] = val;
    }

    // reset table dimensions
    if (colIndex >= gTable.dataTable.Size())
    {
        gTable.dataTable.SetSize(colIndex + 1);
        gTable.columnDesc.SetSize(colIndex + 1);
    }

    if (rowIndex >= gTable.GetNRows())
    {
        for (int i = 0; i < gTable.dataTable.Size(); i++)
            gTable.dataTable[i].SetSize(rowIndex + 1);
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


void TableGridControl::PasteFromClipboardTable(int startRow)
{
    PS_Import import;
    if (!import.OpenClipboardImport())
    {
        GenAppWarningMsg("TableGridControl", "Can't open clipboard");
        return;
    }

    // run through once to get size info
    SC_DoubleArray tableRowData;
    int maxCols = 0;
    int nRows = 0;
    bool firstIsEmpty = false;
    while (import.GetLineDoubles(tableRowData))
    {
        if ((nRows == 0) && tableRowData.IsEmpty())
            firstIsEmpty = true;

        if (tableRowData.Size() > maxCols)
            maxCols = tableRowData.Size();

        if (!tableRowData.IsEmpty())
            nRows++;
    }

    import.CloseImport();

    if (nRows == 0)
    {
        GenAppWarningMsg("TableGridControl", "No numeric data in clipboard");
        return;
    }


    // if start row is zero, we realloc table to required size rows and cols
    if ((startRow == 0) || (gTable.GetNRows() == 0))
    {
        gTable.ResizableAlloc(maxCols,  nRows + DPO_EnterTablePS::nextraRows, 80);
        CString colID;
        for (int i = 0; i < maxCols; i++)
        {
            colID.Format("Col%d", i + 1);
            gTable.SetTitle(i, colID);
        }
        gTable.dataTable.SetSize(maxCols);
    }
    else
    {
        // otherwise we limit import to existing # of cols
        maxCols = gTable.GetNCols();

        // and reset the size of each col to max start row
        for (int i = 0; i < maxCols; i++)
            if (gTable.dataTable[i].Size() > startRow)
                gTable.dataTable[i].SetSize(startRow);
    }


    if (!import.OpenClipboardImport())
    {
        GenAppWarningMsg("TableGridControl", "?? can't open clipboard again");
        return;
    }

    // kluge for columns
    bool readRowIDs = false;
    if (firstIsEmpty)
    {
        SC_StringArray colTitles;
        if (import.GetLineStrings(colTitles))
        {
            // kluge for copy/paste from view table
            if (SameUCString("RowID", colTitles[0]))
            {
                readRowIDs = (startRow == 0);
                colTitles.DeleteItem(0);
            }

            int nadd = maxCols;
            if (colTitles.Size() < nadd)
                nadd = colTitles.Size();
            for (int i = 0; i < nadd; i++)
                gTable.SetTitle(i, colTitles[i]);
        }
    }

    int nReject = 0;
    while (import.GetLineDoubles(tableRowData))
    {
        if (!tableRowData.IsEmpty())
        {
            for (int i = 0; i < maxCols; i++)
            {
                double dVal = nullReal;
                if (i < tableRowData.Size())
                    dVal = tableRowData[i];
                gTable.dataTable[i] += dVal;
            }
        }
        else
        {
            nReject++;
        }
    }

    import.CloseImport();

    // process row IDs
    if (!import.OpenClipboardImport())
    {
        GenAppWarningMsg("TableGridControl", "?? can't open clipboard again");
        return;
    }

    // if ViewTable, then skip first row
    SC_StringArray tableRowIDs;
    if (readRowIDs)
    {
        // skip first row
        import.GetLineStrings(tableRowIDs);
    }

    gTable.rowDesc.SetEmpty();
    while (import.GetLineStrings(tableRowIDs))
    {
        const char* possibleID = tableRowIDs[0];
        if (!readRowIDs)
        {
            // if numeric assume was data & break
            double dummy;
            if (IsValidReal(possibleID, dummy))
                break;
        }
        gTable.rowDesc += possibleID;
    }

    import.CloseImport();

    CString strInfo;
    strInfo.Format("Imported %d rows from clipboard(%d lines rejected)", nRows, nReject);
    GenAppInfoMsg("TableGridControl", strInfo);

    UpdateSize();
    UpdateAll();
    UpdateColSize();
}


void TableGridControl::CopyToClipboardTable()
{
    PS_TxtClipboard clipOut(gTable.GetNCols() * 30);

    if (!clipOut.OpenTxtClipboard(true))
    {
        GenAppWarningMsg("TableGridControl", "Can't open clipboard");
        return;
    }


    if (gTable.rowDesc.IsNotEmpty())
        clipOut.WriteSolidText("RowID\t");

    char tempStr[80];
    for (int i = 0; i < gTable.columnDesc.Size(); i++)
    {
        CopyString(tempStr, gTable.columnDesc[i], 80);
        FillBlanks(tempStr);
        ConcatString(tempStr, " \t", 80);
        clipOut.WriteSolidText(tempStr);
    }
    clipOut.WriteLine();

    int maxCols = gTable.GetNCols();
    for (int i = 0; i < gTable.GetNRows(); i++)
    {
        if (gTable.rowDesc.Size() > i)
        {
            CopyString(tempStr, gTable.rowDesc[i], 80);
            ConcatString(tempStr, " \t", 80);
            clipOut.WriteSolidText(tempStr);
        }
        for (int j = 0; j < maxCols; j++)
        {
            tableFormat.RealToString(gTable.dataTable[j][i], tempStr, 80);
            ConcatString(tempStr, " \t", 80);
            clipOut.WriteSolidText(tempStr);
        }
        clipOut.WriteLine();
    }

    clipOut.Close();
}


void TableGridControl::OnEditPaste()
{
    if (!IsDataInClipboard())
    {
        GenAppWarningMsg("TableGridControl", "No data in clipboard");
    }

    int currRow = GetActiveRow();
    if (currRow < 2 || currRow > GetRowCount())
    {
        return;
    }

    PasteFromClipboardTable(currRow - 2);
}

void TableGridControl::OnEditCopy()
{
    CopyToClipboardTable();
}



void TableGridControl::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsDataInClipboard());
}

////////////////////////////////////////////////////

DPO_EnterTablePS::DPO_EnterTablePS() :
    inputTableObjRefUI(typeid(DO_TableData)), gridUI(currTable, tableFormat)
{
}

void DPO_EnterTablePS::UpdateFromObjCB(void* inObj)
{
    static_cast<DPO_EnterTablePS*>(inObj)->UpdateFromObj();
}

void DPO_EnterTablePS::CopyFrom(const DC_TableData& inTable)
{
    int nallocCols = inTable.GetNCols();
    if (nallocCols < 2)
        nallocCols = 2;

    currTable.ResizableAlloc(nallocCols,  inTable.GetNRows() + nextraRows, 80);
    for (int i = 0; i < inTable.GetNCols(); i++)
    {
        currTable.SetTitle(i, inTable.GetTitle(i));
        currTable.dataTable[i] = inTable.dataTable[i];
        currTable.dataTable[i].SetResizable();
    }
    currTable.rowDesc = inTable.rowDesc;

    currTable.dataTable.SetSize(inTable.GetNCols());

    gridUI.SetColCount(1);
    gridUI.SetRowCount(1);
    gridUI.UpdateSize();
    gridUI.UpdateAll();
    gridUI.UpdateColSize();
    gridUI.SetEditMode();
}

void  DPO_EnterTablePS::CopyTo(DC_TableData& outTable)
{
    outTable.Alloc(currTable.GetNCols(),  currTable.GetNRows(), 80);
    for (int i = 0; i < currTable.GetNCols(); i++)
    {
        outTable.SetTitle(i, currTable.GetTitle(i));
        outTable.dataTable[i] = currTable.dataTable[i];
        outTable.dataTable[i].SetFixed();
    }
    if (currTable.rowDesc.IsNotEmpty())
        outTable.rowDesc = currTable.rowDesc;

    outTable.dataTable.SetSize(currTable.GetNCols());
}

void DPO_EnterTablePS::UpdateFromObj()
{
    if (inputTableObjRefUI.CheckDataObj())
    {
        ClearData();
        const DC_TableData& dataDC = *(static_cast<DO_TableData*>(inputTableObjRefUI.GetDataObj())->tableData);
        CopyFrom(dataDC);
    }
}

void DPO_EnterTablePS::UpdateFormatCB(void* inObj)
{
    static_cast<DPO_EnterTablePS*>(inObj)->UpdateFormat();
}

void DPO_EnterTablePS::UpdateFormat()
{
    tableFormatUI.GetRealFormatSettings(tableFormat);
    gridUI.UpdateAll();
    gridUI.UpdateColSize();
}

void DPO_EnterTablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_EnterTablePS*>(inObj)->SetAllSensitive();
}

void DPO_EnterTablePS::SetAllSensitive()
{
    updateFromInput.SetSensitive(inputTableObjRefUI.ListDataOK());
}

void DPO_EnterTablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach();
    args.BottomFormAttach();
    args.RightPosAttach(80);
    inputTableObjRefUI.InitFramedUI("Input Table Data", URform.GetWidget(), args);
    inputTableObjRefUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
//    args.LeftPosAttach(83);
    args.RightFormAttach(5);
    args.BottomFormAttach(5);
    updateFromInput.StdButtonInit("Update", URform.GetWidget(), args);
    updateFromInput.AddOtherObjectCallback(UpdateFromObjCB, this);

    args.Clear();
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    tableFormatUI.InitFramedUI("Data Format", topRowForm.GetWidget(), args);
    tableFormatUI.formatTypeChooser.AddOtherObjectCallback(UpdateFormatCB, this);
    tableFormatUI.formatPrecChooser.AddOtherObjectCallback(UpdateFormatCB, this);

//  scrolled window in middle
    args.StdTopObjectAttach(topRowForm.GetWidget());
    args.BottomFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);

    gridUI.InitWidget(PPform.GetWidget(), args);

    gridUI.SetColCount(1);
    gridUI.SetRowCount(1);
    gridUI.SetText(0, 0, "   ");

    SetMainformScrolled(true, 440, 250);
}

void DPO_EnterTablePS::ClearData()
{
    currTable.ClearData();
}


void DPO_EnterTablePS::MenuOpen(DPO_EnterTable&   inData)
{
    MenuBaseC::MenuOpen(false, true);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
    ClearData();
}

void DPO_EnterTablePS::SetMenuData(DPO_EnterTable&    inData)
{
    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);
    tableFormatUI.SetRealFormatSettings(inData.tableFormat);

    tableFormat = inData.tableFormat;
    CopyFrom(inData.tableDataDC);

    SetAllSensitive();
    MenuBaseC::SetMenuData(inData);
}


bool DPO_EnterTablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    return allOK;
}

void DPO_EnterTablePS::GetMenuData(DPO_EnterTable& outData)
{
    MenuBaseC::GetMenuData(outData);

    bool allOK = true;
    if (inputTableObjRefUI.FuncObjRefOK(allOK, "Data source"))
    {
        outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();
    }

    tableFormatUI.GetRealFormatSettings(outData.tableFormat);

    CopyTo(outData.tableDataDC);
}

void DPO_EnterTablePS::MenuClear()
{
    ClearData();
    gridUI.UpdateAll();
}

