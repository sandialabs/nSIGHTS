///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ViewGridPS.cpp
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

#include "DPO_ViewGridPS.h"


class GridViewValueCell : public CTextGridCell
{
    protected:
        int                     m_colIndex;
        int                     m_rowIndex;
        GridViewGridControl*    m_pGrid;

    public:
                            GridViewValueCell(int colIndex, int rowIndex, GridViewGridControl* pGrid);

        virtual void        UpdateText();

        void                SetNullStr();
};


GridViewValueCell::GridViewValueCell(int colIndex, int rowIndex, GridViewGridControl* pGrid) :
    CTextGridCell(NULL, alignRight),
    m_colIndex(colIndex),
    m_rowIndex(rowIndex),
    m_pGrid(pGrid)
{
}

void GridViewValueCell::SetNullStr()
{
    SetText("---");
}

void GridViewValueCell::UpdateText()
{
    if (m_pGrid->gGrid == 0)
    {
        SetNullStr();
        return;
    }

    const DC_GridData& currTab = *(m_pGrid->gGrid);
    if ((m_colIndex >= currTab.yData.Size()) || (m_rowIndex >= currTab.xData.Size()))
    {
        SetNullStr();
        return;
    }

    double currVal = currTab.gData[m_rowIndex][m_colIndex];
    if (RealIsNull(currVal))
    {
        SetNullStr();
    }
    else
    {
        char tempStr[80];
        m_pGrid->gridFormat.RealToString(currVal, tempStr, 80);
        SetText(tempStr);
    }
}


///////////////////////////////////////////////////////////
GridViewGridControl::GridViewGridControl(SC_RealFormat& currForm)
    : gGrid(0), lastWasNull(true), gridFormat(currForm)
{
}

#define WM_COPYTOCLIPBOARD      WM_APP+1000

BEGIN_MESSAGE_MAP(GridViewGridControl, CGridCtrlCell)
    //{{AFX_MSG_MAP(GridViewGridControl)
    ON_WM_RBUTTONDOWN()
    //}}AFX_MSG_MAP
    ON_COMMAND_RANGE(WM_COPYTOCLIPBOARD, WM_COPYTOCLIPBOARD, OnPopupCommand)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
END_MESSAGE_MAP()


void GridViewGridControl::OnRButtonDown(UINT nFlags, CPoint point)
{
    CMenu popMenu;
    popMenu.CreatePopupMenu();

    UINT copyFlags = MF_STRING;
    if (gGrid == 0)
        copyFlags |= MF_GRAYED;

    VERIFY(popMenu.AppendMenu(copyFlags, WM_COPYTOCLIPBOARD, "Copy To Clipboard"));

    ClientToScreen(&point);
    popMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this);
}




void GridViewGridControl::OnPopupCommand(UINT nMenuID)
{
    CopyToClipboardGrid();
}

void GridViewGridControl::UpdateGrid(const DC_GridData*   inGrid)
{
    CWaitCursor w; // this can be time consuming

    gGrid = inGrid;
    if (gGrid == 0)
    {
        SetRowCount(2);
        SetColCount(2);
        SetCenteredText(0, 1, "Col1");
        SetCenteredText(1, 0, "Row1");
        SetCenteredText(1, 1, "no data");
        AutoSize();
        return;
    }

    int nRows = gGrid->xData.Size();
    int nCols = gGrid->yData.Size();
    SetRowCount(nRows + 1);
    SetColCount(nCols + 1);

    CString str;
    char tempStr[80];

    int i;
    for (i = 0; i < nCols; i++)
    {
        gridFormat.RealToString(gGrid->yData[i], tempStr, 80);
        str = tempStr;
        SetCenteredText(0, i + 1, str);
    }


    for (i = 0; i < nRows; i++)
    {
        gridFormat.RealToString(gGrid->xData[i], tempStr, 80);
        str = tempStr;
        SetCenteredText(i + 1, 0, str);
        for (int j = 0; j < nCols; j++)
            SetCell(i + 1, j + 1, new GridViewValueCell(j, i, this));

    }

    UpdateGridData();

    lastWasNull = false;
}

void GridViewGridControl::UpdateGridData()
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

void GridViewGridControl::CopyToClipboardGrid()
{
    if (gGrid == 0)
        return;

    PS_TxtClipboard clipOut(gGrid->yData.Size() * 30);

    if (!clipOut.OpenTxtClipboard(true))
    {
        GenAppWarningMsg("GridViewGridControl", "Can't open clipboard");
        return;
    }

    clipOut.WriteSolidText("GridX\t");

    int i;
    char tempStr[80];
    int maxCols = gGrid->yData.Size();
    for (i = 0; i < maxCols; i++)
    {
        gridFormat.RealToString(gGrid->yData[i], tempStr, 80);
        FillBlanks(tempStr);
        ConcatString(tempStr, " \t", 80);
        clipOut.WriteSolidText(tempStr);
    }
    clipOut.WriteLine();

    for (i = 0; i < gGrid->xData.Size(); i++)
    {
        gridFormat.RealToString(gGrid->xData[i], tempStr, 80);
        ConcatString(tempStr, " \t", 80);
        clipOut.WriteSolidText(tempStr);

        for (int j = 0; j < maxCols; j++)
        {
            gridFormat.RealToString(gGrid->gData[i][j], tempStr, 80);
            ConcatString(tempStr, " \t", 80);
            clipOut.WriteSolidText(tempStr);
        }
        clipOut.WriteLine();
    }

    clipOut.Close();
}


void GridViewGridControl::OnEditCopy()
{
    CopyToClipboardGrid();
}


////////////////////////////////////////////////////

DPO_ViewGridPS::DPO_ViewGridPS() :
    inputGridObjRefUI(typeid(DO_GridData)), gridUI(gridFormat)
{
}

void DPO_ViewGridPS::UpdateFromObjCB(void* inObj)
{
    static_cast<DPO_ViewGridPS*>(inObj)->UpdateFromObj();
}


void DPO_ViewGridPS::UpdateFromObj()
{
    if (inputGridObjRefUI.CheckDataObj())
    {
        const DC_GridData* dataDC = static_cast<DO_GridData*>(inputGridObjRefUI.GetDataObj())->gridData;
        gridUI.UpdateGrid(dataDC);
    }
    else
        gridUI.UpdateGrid(0);
}

void DPO_ViewGridPS::UpdateFormatCB(void* inObj)
{
    static_cast<DPO_ViewGridPS*>(inObj)->UpdateFormat();
}

void DPO_ViewGridPS::UpdateFormat()
{
    gridFormatUI.GetRealFormatSettings(gridFormat);
    gridUI.UpdateGridData();
}


void DPO_ViewGridPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.AllFormAttach();
    inputGridObjRefUI.InitFramedUI("Input Grid Data", URform.GetWidget(), args);
    inputGridObjRefUI.AddOtherObjectCallback(UpdateFromObjCB, this);

    args.Clear();
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    gridFormatUI.InitFramedUI("Data Format", topRowForm.GetWidget(), args);
    gridFormatUI.formatTypeChooser.AddOtherObjectCallback(UpdateFormatCB, this);
    gridFormatUI.formatPrecChooser.AddOtherObjectCallback(UpdateFormatCB, this);

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


void DPO_ViewGridPS::MenuOpen(DPO_ViewGrid&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputGridObjRefUI.StdMenuInit(inData.inputGridObjRef);
}

void DPO_ViewGridPS::SetMenuData(DPO_ViewGrid&    inData)
{
    MenuBaseC::SetMenuData(inData);

    inputGridObjRefUI.SetFuncObjRef(inData.inputGridObjRef);
    gridFormatUI.SetRealFormatSettings(inData.gridFormat);

    gridFormat = inData.gridFormat;

    UpdateFromObj();

}


bool DPO_ViewGridPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputGridObjRefUI.FuncObjRefOK(allOK, "Data source");
    return allOK;
}

void DPO_ViewGridPS::GetMenuData(DPO_ViewGrid& outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.inputGridObjRef = inputGridObjRefUI.GetFuncObjRef();
    gridFormatUI.GetRealFormatSettings(outData.gridFormat);
}

