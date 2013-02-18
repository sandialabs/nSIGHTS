///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ViewTablePS.cpp
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

#include "DPO_ViewTablePS.h"


class ViewValueCell : public CTextGridCell
{
    protected:
        int                     m_colIndex;
        int                     m_rowIndex;
        TableViewGridControl*   m_pGrid;

    public:
                            ViewValueCell(int colIndex, int rowIndex, TableViewGridControl* pGrid);

        virtual void        UpdateText();

        void                SetNullStr();
};


ViewValueCell::ViewValueCell(int colIndex, int rowIndex, TableViewGridControl* pGrid) :
    CTextGridCell(NULL, alignRight),
    m_colIndex(colIndex),
    m_rowIndex(rowIndex),
    m_pGrid(pGrid)
{
}

void ViewValueCell::SetNullStr()
{
    SetText("---");
}

void ViewValueCell::UpdateText()
{
    if (m_pGrid->gTable == 0)
    {
        SetNullStr();
        return;
    }

    const DC_TableData& currTab = *(m_pGrid->gTable);
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



///////////////////////////////////////////////////////////
TableViewGridControl::TableViewGridControl(SC_RealFormat& currForm)
    : gTable(0), lastWasNull(true), tableFormat(currForm)
{
}

#define WM_COPYTOCLIPBOARD      WM_APP+1000

BEGIN_MESSAGE_MAP(TableViewGridControl, CGridCtrlCell)
    //{{AFX_MSG_MAP(TableViewGridControl)
    ON_WM_RBUTTONDOWN()
    //}}AFX_MSG_MAP
    ON_COMMAND_RANGE(WM_COPYTOCLIPBOARD, WM_COPYTOCLIPBOARD, OnPopupCommand)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
END_MESSAGE_MAP()


void TableViewGridControl::OnRButtonDown(UINT nFlags, CPoint point)
{
    CMenu popMenu;
    popMenu.CreatePopupMenu();

    UINT copyFlags = MF_STRING;
    if (gTable == 0)
        copyFlags |= MF_GRAYED;

    VERIFY(popMenu.AppendMenu(copyFlags, WM_COPYTOCLIPBOARD, "Copy To Clipboard"));

    ClientToScreen(&point);
    popMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this);
}




void TableViewGridControl::OnPopupCommand(UINT nMenuID)
{
    CopyToClipboardTable();
}

void TableViewGridControl::UpdateTable(const DC_TableData*   inTable)
{
    CWaitCursor w; // this can be time consuming

    gTable = inTable;
    if (gTable == 0)
    {
        SetRowCount(2);
        SetColCount(2);
        SetCenteredText(0, 1, "Col1");
        SetCenteredText(1, 0, "Row1");
        SetCenteredText(1, 1, "no data");
        AutoSize();
        return;
    }


    int nRows = gTable->dataTable[0].AllocSize();
    int nCols = gTable->dataTable.AllocSize();
    SetRowCount(nRows + 1);
    SetColCount(nCols + 1);

    CString str;

    for (int i = 0; i < nCols; i++)
    {
        if (gTable->columnDesc.Size() > i)
            str = gTable->columnDesc[i];
        else
            str.Format("%d", i + 1);
        SetCenteredText(0, i + 1, str);
    }


    for (int i = 0; i < nRows; i++)
    {
        if (gTable->rowDesc.Size() > i)
            str = gTable->rowDesc[i];
        else
            str.Format("%d", i + 1);
        SetCenteredText(i + 1, 0, str);
        for (int j = 0; j < nCols; j++)
            SetCell(i + 1, j + 1, new ViewValueCell(j, i, this));

    }

    UpdateTableData();

    lastWasNull = false;
}

void TableViewGridControl::UpdateTableData()
{
    for (int i = 1; i < GetRowCount(); i++)
    {
        for (int j = 1; j < GetColCount(); j++)
        {
            CGridCell* pCell = GetCell(i, j);
            pCell->UpdateText();
        }
    }


    AutoSize();
    int maxSize = GetColSize(1);
    for (int i = 2; i < GetColCount(); i++)
    {
        int colSize = GetColSize(i);
        if (colSize > maxSize)
            maxSize= colSize;
    }

    maxSize += 5;
    for (int i = 1; i < GetColCount(); i++)
        SetColSize(i, maxSize);

    InvalidateAll();
}

void TableViewGridControl::CopyToClipboardTable()
{
    if (gTable == 0)
        return;

    PS_TxtClipboard clipOut(gTable->GetNCols() * 30);

    if (!clipOut.OpenTxtClipboard(true))
    {
        GenAppWarningMsg("TableViewGridControl", "Can't open clipboard");
        return;
    }


    if (gTable->rowDesc.IsNotEmpty())
        clipOut.WriteSolidText("RowID\t");

    int i;
    char tempStr[80];
    for (i = 0; i < gTable->columnDesc.Size(); i++)
    {
        CopyString(tempStr, gTable->columnDesc[i], 80);
        FillBlanks(tempStr);
        ConcatString(tempStr, " \t", 80);
        clipOut.WriteSolidText(tempStr);
    }
    clipOut.WriteLine();

    int maxCols = gTable->GetNCols();
    for (i = 0; i < gTable->GetNRows(); i++)
    {
        if (gTable->rowDesc.Size() > i)
        {
            CopyString(tempStr, gTable->rowDesc[i], 80);
            ConcatString(tempStr, " \t", 80);
            clipOut.WriteSolidText(tempStr);
        }
        for (int j = 0; j < maxCols; j++)
        {
            tableFormat.RealToString(gTable->dataTable[j][i], tempStr, 80);
            ConcatString(tempStr, " \t", 80);
            clipOut.WriteSolidText(tempStr);
        }
        clipOut.WriteLine();
    }

    clipOut.Close();
}


void TableViewGridControl::OnEditCopy()
{
    CopyToClipboardTable();
}


////////////////////////////////////////////////////

DPO_ViewTablePS::DPO_ViewTablePS() :
    inputTableObjRefUI(typeid(DO_TableData)), gridUI(tableFormat)
{
}

void DPO_ViewTablePS::UpdateFromObjCB(void* inObj)
{
    static_cast<DPO_ViewTablePS*>(inObj)->UpdateFromObj();
}


void DPO_ViewTablePS::UpdateFromObj()
{
    if (inputTableObjRefUI.CheckDataObj())
    {
        const DC_TableData* dataDC = static_cast<DO_TableData*>(inputTableObjRefUI.GetDataObj())->tableData;
        gridUI.UpdateTable(dataDC);
    }
    else
        gridUI.UpdateTable(0);
}

void DPO_ViewTablePS::UpdateFormatCB(void* inObj)
{
    static_cast<DPO_ViewTablePS*>(inObj)->UpdateFormat();
}

void DPO_ViewTablePS::UpdateFormat()
{
    tableFormatUI.GetRealFormatSettings(tableFormat);
    gridUI.UpdateTableData();
}


void DPO_ViewTablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Input Table Data", URform.GetWidget(), args);
    inputTableObjRefUI.AddOtherObjectCallback(UpdateFromObjCB, this);

    args.Clear();
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    tableFormatUI.InitFramedUI("Data Format", topRowForm.GetWidget(), args);
    tableFormatUI.formatTypeChooser.AddOtherObjectCallback(UpdateFormatCB, this);
    tableFormatUI.formatPrecChooser.AddOtherObjectCallback(UpdateFormatCB, this);

//  scrolled window in middle
    args.Clear();
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


void DPO_ViewTablePS::MenuOpen(DPO_ViewTable&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
}

void DPO_ViewTablePS::SetMenuData(DPO_ViewTable&    inData)
{
    MenuBaseC::SetMenuData(inData);

    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);
    tableFormatUI.SetRealFormatSettings(inData.tableFormat);

    tableFormat = inData.tableFormat;

    UpdateFromObj();

}


bool DPO_ViewTablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableObjRefUI.FuncObjRefOK(allOK, "Data source");
    return allOK;
}

void DPO_ViewTablePS::GetMenuData(DPO_ViewTable& outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();
    tableFormatUI.GetRealFormatSettings(outData.tableFormat);
}

