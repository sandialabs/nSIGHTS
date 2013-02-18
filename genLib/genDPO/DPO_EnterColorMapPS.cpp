///////////////////////////////////////////////////////////////////////////////////
//
// DPO_EnterColorMapPS.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              User interface for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/DO_ColorMap.h>

#include <genClassPS/PS_Color.h>
#include <genClassPS/PS_UIBase.h>
#include <genClassPS/PS_Import.h>

#include <genApp/U_Menu.h>

#include "DPO_EnterColorMapPS.h"


namespace nsDPO_EnterColorMapPS {   // required to avoid collisions with other grid

    class ValueCell : public CTextGridCell
    {
    protected:
        int                     m_rowIndex;
        int                     m_colIndex;
        ColorMapGridControl*    m_pGrid;

    public:
        ValueCell(int rowIndex, int colIndex, ColorMapGridControl* pGrid);
        virtual void    UpdateText();
        void            SetNullStr();
    };


    class ColorCell : public CTextGridCell
    {
    protected:
        int                  m_rowIndex;
        ColorMapGridControl* m_pGrid;

    private:
        COLORREF            m_clrBackground;

    public:
        ColorCell(int rowIndex, ColorMapGridControl* pGrid);

        virtual void    UpdateText();

    protected:
        virtual void    DrawUnconstrained(CDC* pDC, int x, int y) const;
        virtual void    DrawConstrained(CDC* pDC, int x, int y, int cx, int cy) const;

    };


    ColorCell::ColorCell(int rowIndex, ColorMapGridControl* pGrid) :
        CTextGridCell(NULL, alignRight),
        m_rowIndex(rowIndex),
        m_pGrid(pGrid)
    {
    }

    void ColorCell::UpdateText()
    {
        DC_ColorMap& currCM = m_pGrid->gColorMap;
        if (m_rowIndex >= currCM.ncolorMap)
            m_clrBackground = ::GetSysColor(COLOR_MENU);
        else
            {
                SC_ColorSpec currColor = currCM.colorMapColors[m_rowIndex];

                if (RealIsNull(currColor.RH) || (currColor.RH < 0.0) || (currColor.RH > 1.0))
                    currColor.RH = 0.0;

                if(RealIsNull(currColor.GS) || (currColor.GS < 0.0) || (currColor.GS > 1.0))
                    currColor.GS = 0.0;

                if (RealIsNull(currColor.BV) || (currColor.BV < 0.0) || (currColor.RH > 1.0))
                    currColor.BV = 0.0;

                if (m_pGrid->useHSV)
                    currColor.ToRGB();

                m_clrBackground = ConvertSC_ColorSpec(currColor);
            }
    }

    void ColorCell::DrawUnconstrained(CDC* pDC, int x, int y) const
    {

        //    pDC->TextOut(x, y, m_sText, StringLength(m_sText));
    }

    void ColorCell::DrawConstrained(CDC* pDC, int x, int y, int cx, int cy) const
    {
        CRect rect(x, y, x + cx, y + cy);
        pDC->FillSolidRect(rect, m_clrBackground);
    }




    ValueCell::ValueCell(int rowIndex, int colIndex, ColorMapGridControl* pGrid) :
        CTextGridCell(NULL, alignRight),
        m_rowIndex(rowIndex),
        m_colIndex(colIndex),
        m_pGrid(pGrid)
    {
    }

    void ValueCell::SetNullStr()
    {
        SetText("---");
    }

    void ValueCell::UpdateText()
    {
        DC_ColorMap& currCM = m_pGrid->gColorMap;
        if (m_rowIndex >= currCM.ncolorMap)
            {
                SetNullStr();
                return;
            }

        SC_ColorSpec& currSpec = currCM.colorMapColors[m_rowIndex];
        double currVal;
        switch (m_colIndex )
            {
            case 0 : {currVal = currSpec.RH; break;}
            case 1 : {currVal = currSpec.GS; break;}
            case 2 : {currVal = currSpec.BV; break;}
            }
        if (RealIsNull(currVal) || (currVal < 0.0) || (currVal > 1.0))
            {
                SetNullStr();
            }
        else
            {
                char tempStr[80];
                SC_DecFormat rForm(3);
                rForm.RealToString(currVal, tempStr, 80);
                SetText(tempStr);
            }
    }




    ///////////////////////////////////////////////////////////
    ColorMapGridControl::ColorMapGridControl(DC_ColorMap& currCM)
        : gColorMap(currCM), useHSV(true)
    {
    }

#define WM_INSERT_ROW_BEFORE    WM_APP+1000
#define WM_INSERT_ROW_AFTER     WM_APP+1001
#define WM_DELETE_ROW           WM_APP+1002
#define WM_COPYFROMCLIPBOARD    WM_APP+1003

    BEGIN_MESSAGE_MAP(ColorMapGridControl, CGridCtrlCell)
    //{{AFX_MSG_MAP(ColorMapGridControl)
    ON_WM_RBUTTONDOWN()
    ON_WM_KEYDOWN()
    //}}AFX_MSG_MAP
    ON_COMMAND_RANGE(WM_INSERT_ROW_BEFORE, WM_COPYFROMCLIPBOARD, OnPopupCommand)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
    END_MESSAGE_MAP()


    void ColorMapGridControl::OnRButtonDown(UINT nFlags, CPoint point)
    {
        CGridCellIndices currCell = GetCellIndices(point);
        int currRow = currCell.m_nRow - 1;
        int currCol = currCell.m_nCol - 1;
        if ((currRow < 0) && (currCol < 0))
            return;

        m_nCurrRow = currRow;
        if (m_nCurrRow < 0)
            m_nCurrRow = 0;

        CMenu popMenu;
        popMenu.CreatePopupMenu();

        int nRows = gColorMap.ncolorMap;

        UINT rowInsFlags = MF_STRING;
        if ((currRow < 0) || (currRow > nRows) || (nRows == DC_ColorMap::maxCMcolors))
            rowInsFlags |= MF_GRAYED;

        UINT clipFlags = MF_STRING;
        if (!IsDataInClipboard()) {
            clipFlags |= MF_GRAYED;
        }

        VERIFY(popMenu.AppendMenu(rowInsFlags, WM_INSERT_ROW_BEFORE,  "Insert Row Before" ));
        VERIFY(popMenu.AppendMenu(rowInsFlags, WM_INSERT_ROW_AFTER,   "Insert Row After" ));
        VERIFY(popMenu.AppendMenu(rowInsFlags, WM_DELETE_ROW,         "Delete Row" ));
        VERIFY(popMenu.AppendMenu(clipFlags, WM_COPYFROMCLIPBOARD, "Copy From Clipboard"));

        ClientToScreen(&point);
        popMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this);
    }

    void ColorMapGridControl::DeleteRow(int rowIndex)
    {
        if (rowIndex >= gColorMap.ncolorMap)
            return;

        for (int i = rowIndex; i < gColorMap.ncolorMap; i++)
            {
                gColorMap.colorMapColors[i] = gColorMap.colorMapColors[i + 1];
            }

        gColorMap.ncolorMap--;
        gColorMap.colorMapColors[gColorMap.ncolorMap] = SC_ColorSpec(-1.0, -1.0, -1.0);
    }

    void ColorMapGridControl::InsertRow(int rowIndex)
    {
        if (rowIndex >= gColorMap.ncolorMap)
            return;

        for (int i = gColorMap.ncolorMap; i > rowIndex; i--)
            {
                gColorMap.colorMapColors[i] = gColorMap.colorMapColors[i - 1];
            }
        gColorMap.ncolorMap++;
    }


    void ColorMapGridControl::UpdateModifiedRows()
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


    void ColorMapGridControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        m_nCurrRow = m_cellActive.m_nRow - 1;
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

    void ColorMapGridControl::UpdateAll()
    {
        m_nCurrRow = 0;
        UpdateModifiedRows();
    }

    void  ColorMapGridControl::UpdateColSize()
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

    void ColorMapGridControl::OnPopupCommand(UINT nMenuID)
    {
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
        case WM_COPYFROMCLIPBOARD: {
            CopyFromClipboardTable(m_nCurrRow);
            break;
        }
        default:
            ASSERT(0);
            break;
        }
        UpdateModifiedRows();
    }


    void ColorMapGridControl::FinishEdit(const char* szText, int nValue, int nLastChar)
    {
        int rowIndex = m_cellActive.m_nRow - 1;
        int colIndex = m_cellActive.m_nCol - 1;

        double val;
        if ((!IsValidReal(szText, val)) || (val < 0.0) || (val > 1.0))
            val = 0.0;

        SC_ColorSpec& currSpec = gColorMap.colorMapColors[rowIndex];

        switch (colIndex )
            {
            case 0 : {currSpec.RH = val; break;}
            case 1 : {currSpec.GS = val; break;}
            case 2 : {currSpec.BV = val; break;}
            case 3 : {break;}
            }

        if (rowIndex >= gColorMap.ncolorMap)
            {
                for (int i = gColorMap.ncolorMap; i < rowIndex; i++)
                    gColorMap.colorMapColors[i] = SC_ColorSpec(0.0, 0.0, 0.0);
                m_nCurrRow = gColorMap.ncolorMap - 1;
                gColorMap.ncolorMap = rowIndex + 1;
                UpdateModifiedRows();
            }
        else
            {
                for (int j = 1; j < GetColCount(); j++)
                    {
                        CGridCell* pCell = GetCell(rowIndex + 1, j);
                        pCell->UpdateText();
                        InvalidateCell(rowIndex + 1, j);
                    }
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


    void ColorMapGridControl::CopyFromClipboardTable(int startRow)
    {
        PS_Import import;
        if (!import.OpenClipboardImport())
            {
                GenAppWarningMsg("ColorMapGridControl", "Can't open clipboard");
                return;
            }

        // run through once to get size info
        SC_DoubleArray tableRowData;
        int maxCols = 0;
        int nRows = 0;
        while (import.GetLineDoubles(tableRowData))
            {
                if (tableRowData.IsEmpty())
                    continue;

                if (tableRowData.Size() != 3)
                    {
                        GenAppWarningMsg("ColorMapGridControl", "Expecting RGB/HSV triplets");
                        import.CloseImport();
                        return;
                    }

                nRows++;
            }

        import.CloseImport();

        if (nRows == 0)
            {
                GenAppWarningMsg("ColorMapGridControl", "No numeric data in clipboard");
                return;
            }

        if (nRows + startRow > DC_ColorMap::maxCMcolors)
            {
                GenAppWarningMsg("ColorMapGridControl", "Maximum 256 colors");
                return;
            }


        if (!import.OpenClipboardImport())
            {
                GenAppWarningMsg("ColorMapGridControl", "?? can't open clipboard again");
                return;
            }


        int nReject = 0;
        while (import.GetLineDoubles(tableRowData))
            {
                if (tableRowData.Size() == 3)
                    {
                        for (int i = 0; i < 3; i++)
                            if ((tableRowData[i] < 0.0) ||  (tableRowData[i] > 1.0))
                                tableRowData[i] = 0.1;

                        gColorMap.colorMapColors[startRow].RH = tableRowData[0];
                        gColorMap.colorMapColors[startRow].GS = tableRowData[1];
                        gColorMap.colorMapColors[startRow].BV = tableRowData[2];
                        startRow++;
                    }
                else
                    {
                        nReject++;
                    }
            }
        gColorMap.ncolorMap = startRow;

        import.CloseImport();

        CString strInfo;
        strInfo.Format("Imported %d rows from clipboard(%d lines rejected)", nRows, nReject);
        GenAppInfoMsg("ColorMapGridControl", strInfo);

        UpdateAll();
        UpdateColSize();
    }

    void ColorMapGridControl::OnEditPaste()
    {
        if (!IsDataInClipboard())
            {
                GenAppWarningMsg("ColorMapGridControl", "No data in clipboard");
            }

        int currRow = GetActiveRow();
        if (currRow < 1 || currRow > GetRowCount())
            {
                return;
            }

        CopyFromClipboardTable(currRow - 1);
    }

    void ColorMapGridControl::OnUpdateEditPaste(CCmdUI* pCmdUI)
    {
        pCmdUI->Enable(IsDataInClipboard());
    }




    void ColorMapGridControl::Initialize()
    {
        SetColCount(5);
        SetRowCount(DC_ColorMap::maxCMcolors + 1);

        CString str;
        for (int i = 1; i <= DC_ColorMap::maxCMcolors; i++)
            {
                str.Format("%d", i);
                SetCenteredText(i, 0, str);
                SetCell(i , 1, new ValueCell(i - 1, 0, this));
                SetCell(i , 2, new ValueCell(i - 1, 1, this));
                SetCell(i , 3, new ValueCell(i - 1, 2, this));
                SetCell(i , 4, new ColorCell(i - 1, this));
            }
        SetColLabels();
    }


    void  ColorMapGridControl::SetColLabels()
    {
        if (useHSV)
            {
                SetCenteredText(0, 1, " Hue ");
                SetCenteredText(0, 2, " Saturation ");
                SetCenteredText(0, 3, " Value ");
            }
        else
            {
                SetCenteredText(0, 1, " Red ");
                SetCenteredText(0, 2, " Green ");
                SetCenteredText(0, 3, " Blue ");
            }
        SetCenteredText(0, 4, " Example Color ");
    }

};  // end NS

using namespace nsDPO_EnterColorMapPS;

DPO_EnterColorMapPS::DPO_EnterColorMapPS() :
    gridUI(currColorMap),
    inputColorMapObjRefUI(typeid(DO_ColorMap))
{
}

void  DPO_EnterColorMapPS::ToggleHSV()
{
    bool useHSV = useHSVspecUI.GetBoolChoice();

    if ((useHSV && gridUI.useHSV) || ((!useHSV) && (!gridUI.useHSV)))
        return;

    for ( int i = 0; i < currColorMap.ncolorMap; i++)
        if (useHSV)
            currColorMap.colorMapColors[i].ToHSV();
        else
            currColorMap.colorMapColors[i].ToRGB();

    gridUI.useHSV = useHSV;
    gridUI.SetColLabels();
    gridUI.UpdateColSize();
    gridUI.UpdateAll();
}

void DPO_EnterColorMapPS::UseHSVspecCB(void* inObj)
{
    static_cast<DPO_EnterColorMapPS*>(inObj)->ToggleHSV();
}


void DPO_EnterColorMapPS::UpdateFromObjCB(void* inObj)
{
    static_cast<DPO_EnterColorMapPS*>(inObj)->UpdateFromObj();
}

void DPO_EnterColorMapPS::UpdateFromObj()
{
    if (inputColorMapObjRefUI.CheckDataObj())
        {
            currColorMap = *(static_cast<DO_ColorMap*>(inputColorMapObjRefUI.GetDataObj())->colorMapData);

            gridUI.useHSV = false;
            useHSVspecUI.SetBoolChoice(false);
            gridUI.SetColLabels();
            gridUI.UpdateColSize();
            gridUI.UpdateAll();
        }
}

void DPO_EnterColorMapPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_EnterColorMapPS*>(inObj)->SetAllSensitive();
}

void DPO_EnterColorMapPS::SetAllSensitive()
{
    updateFromInput.SetSensitive(inputColorMapObjRefUI.ListDataOK());
}

void DPO_EnterColorMapPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach();
    args.BottomFormAttach();
    args.RightPosAttach(80);
    inputColorMapObjRefUI.InitFramedUI("Input Color Map", URform.GetWidget(), args);
    inputColorMapObjRefUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.RightFormAttach(5);
    args.BottomFormAttach(5);
    updateFromInput.StdButtonInit("Update", URform.GetWidget(), args);
    updateFromInput.AddOtherObjectCallback(UpdateFromObjCB, this);


    args.Clear();
    args.StdFormAttach();
    useHSVspecUI.rightOffset = 45;
    useHSVspecUI.PreInitSetUp("Red/Green/Blue", "Hue/Sat/Val", true);
    useHSVspecUI.InitFramedUI("Color Specification Method", PPform.GetWidget(), args);
    useHSVspecUI.AddOtherObjectCallback(UseHSVspecCB, this);

    args.Clear();
    args.StdTopObjectAttach(useHSVspecUI);
    args.BottomFormAttach();

    gridUI.InitWidget(PPform.GetWidget(), args);
    gridUI.Initialize();

    SetMainformScrolled(true, 520, 250);
}


void DPO_EnterColorMapPS::MenuOpen(DPO_EnterColorMap& inData)
{
    MenuBaseC::MenuOpen(true, true);
    inputColorMapObjRefUI.StdMenuInit(FuncObjRef());
}

void DPO_EnterColorMapPS::SetMenuData(DPO_EnterColorMap&    inData)
{
    useHSVspecUI.SetBoolChoice(inData.useHSVspec);
    gridUI.useHSV = inData.useHSVspec;
    currColorMap = inData.enterColorMap;

    gridUI.SetColLabels();
    gridUI.UpdateColSize();
    gridUI.UpdateAll();

    MenuBaseC::SetMenuData(inData);
}

bool DPO_EnterColorMapPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    return allOK;
}

void DPO_EnterColorMapPS::GetMenuData(DPO_EnterColorMap&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.useHSVspec  = useHSVspecUI.GetBoolChoice();
    outData.enterColorMap = currColorMap;
}

void DPO_EnterColorMapPS::MenuDefault()
{
    MenuBaseC::MenuDefault();
    useHSVspecUI.SetBoolChoice(true);

    gridUI.useHSV = true;

    currColorMap.ncolorMap = 5;
    currColorMap.colorMapColors[0] = SC_ColorSpec(0.0,   1.0, 1.0);
    currColorMap.colorMapColors[1] = SC_ColorSpec(0.15,  1.0, 1.0);
    currColorMap.colorMapColors[2] = SC_ColorSpec(0.30,  1.0, 1.0);
    currColorMap.colorMapColors[3] = SC_ColorSpec(0.45,  1.0, 1.0);
    currColorMap.colorMapColors[4] = SC_ColorSpec(0.60,  1.0, 1.0);
    for (int i = currColorMap.ncolorMap; i < DC_ColorMap::maxCMcolors; i++)
        currColorMap.colorMapColors[i] = SC_ColorSpec(-1.0, -1.0, -1.0);

    gridUI.SetColLabels();
    gridUI.UpdateAll();
}

