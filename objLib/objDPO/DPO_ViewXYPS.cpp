///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ViewXYPS.cpp
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

#include "DPO_ViewXYPS.h"

#include <genClassPS/PS_Import.h>


class XYViewValueCell : public CTextGridCell
{
    protected:
        bool                isX;
        int                 m_rowIndex;
        XYViewGridControl*  m_pGrid;

    public:
                            XYViewValueCell(bool inX, int rowIndex, XYViewGridControl* pGrid);

        virtual void        UpdateText();

        void                SetNullStr();
};


XYViewValueCell::XYViewValueCell(bool inX, int rowIndex, XYViewGridControl* pGrid) :
    CTextGridCell(NULL, alignRight),
    isX(inX),
    m_rowIndex(rowIndex),
    m_pGrid(pGrid)
{
}

void XYViewValueCell::SetNullStr()
{
    SetText("---");
}

void XYViewValueCell::UpdateText()
{
    if (m_pGrid->gXY == 0)
    {
        SetNullStr();
        return;
    }

    const DC_XYData& currXY = *(m_pGrid->gXY);
    if (m_rowIndex >= currXY.Size())
    {
        SetNullStr();
        return;
    }

    char tempStr[80];
    CopyString(tempStr, "null", 80);
    if (isX)
    {
        double currVal = currXY.xData[m_rowIndex];
        if (!RealIsNull(currVal))
            m_pGrid->xFormat.RealToString(currVal, tempStr, 80);
    }
    else
    {
        double currVal = currXY.yData[m_rowIndex];
        if (!RealIsNull(currVal))
            m_pGrid->yFormat.RealToString(currVal, tempStr, 80);
    }
    SetText(tempStr);
}




XYViewGridControl::XYViewGridControl(SC_RealFormat& currX, SC_RealFormat& currY)
    : gXY(0), lastWasNull(true), xFormat(currX), yFormat(currY)
{
}

#define WM_COPYTOCLIPBOARD      WM_APP+1000

BEGIN_MESSAGE_MAP(XYViewGridControl, CGridCtrlCell)
    //{{AFX_MSG_MAP(XYViewGridControl)
    ON_WM_RBUTTONDOWN()
    //}}AFX_MSG_MAP
    ON_COMMAND_RANGE(WM_COPYTOCLIPBOARD, WM_COPYTOCLIPBOARD, OnPopupCommand)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
END_MESSAGE_MAP()


void XYViewGridControl::OnRButtonDown(UINT nFlags, CPoint point)
{
    CMenu popMenu;
    popMenu.CreatePopupMenu();

    UINT copyFlags = MF_STRING;
    if (gXY == 0)
        copyFlags |= MF_GRAYED;

    VERIFY(popMenu.AppendMenu(copyFlags, WM_COPYTOCLIPBOARD, "Copy To Clipboard"));

    ClientToScreen(&point);
    popMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this);
}




void XYViewGridControl::OnPopupCommand(UINT nMenuID)
{
    CopyToClipboardXY();
}

void XYViewGridControl::UpdateXY(const DC_XYData*   inXY)
{
    SetColCount(3);
    SetCenteredText(0, 1, "    X Data    ");
    SetCenteredText(0, 2, "    Y Data    ");


    gXY = inXY;
    if (gXY == 0)
    {
        SetRowCount(2);
        SetCenteredText(1, 0, "1");
        SetCenteredText(1, 1, "no data");
        SetCenteredText(1, 2, "no data");
        AutoSize();
        return;
    }


    int nRows = gXY->Size();
    SetRowCount(nRows + 1);

    CString str;

    for (int i = 0; i < nRows; i++)
    {
        str.Format("%d", i + 1);
        SetCenteredText(i + 1, 0, str);
        SetCell(i + 1, 1, new XYViewValueCell(true, i, this));
        SetCell(i + 1, 2, new XYViewValueCell(false, i, this));
    }

    UpdateXYData();

    lastWasNull = false;
}

void XYViewGridControl::UpdateXYData()
{
    for (int i = 1; i < GetRowCount(); i++)
    {
        GetCell(i, 1)->UpdateText();
        GetCell(i, 2)->UpdateText();
    }

    AutoSize();

    InvalidateAll();
}

void XYViewGridControl::CopyToClipboardXY()
{
    if (gXY == 0)
        return;

    PS_TxtClipboard clipOut(80);

    if (!clipOut.OpenTxtClipboard(true))
    {
        GenAppWarningMsg("XYViewGridControl", "Can't open clipboard");
        return;
    }

    clipOut.WriteSolidText("XData \t yData \t");
    clipOut.WriteLine();

    char tempStr[80];
    for (int i = 0; i < gXY->Size(); i++)
    {
        xFormat.RealToString(gXY->xData[i], tempStr, 80);
        ConcatString(tempStr, " \t", 80);
        clipOut.WriteSolidText(tempStr);

        yFormat.RealToString(gXY->yData[i], tempStr, 80);
        ConcatString(tempStr, " \t", 80);
        clipOut.WriteSolidText(tempStr);

        clipOut.WriteLine();
    }

    clipOut.Close();
}


void XYViewGridControl::OnEditCopy()
{
    CopyToClipboardXY();
}



////////////////////////////////////////////////////

DPO_ViewXYPS::DPO_ViewXYPS() :
    inputXYObjRefUI(typeid(DO_XYData)), m_grid(xFormat, yFormat)

{
}

void DPO_ViewXYPS::UpdateFromObjCB(void* inObj)
{
    static_cast<DPO_ViewXYPS*>(inObj)->UpdateFromObj();
}

void DPO_ViewXYPS::UpdateFromObj()
{
    if (inputXYObjRefUI.CheckDataObj())
    {
        const DC_XYData* dataDC = static_cast<DO_XYData*>(inputXYObjRefUI.GetDataObj())->xyData;
        m_grid.UpdateXY(dataDC);
    }
    else
        m_grid.UpdateXY(0);
}

void DPO_ViewXYPS::UpdateFormatCB(void* inObj)
{
    static_cast<DPO_ViewXYPS*>(inObj)->UpdateFormat();
}

void DPO_ViewXYPS::UpdateFormat()
{
    xFormatUI.GetRealFormatSettings(xFormat);
    yFormatUI.GetRealFormatSettings(yFormat);

    m_grid.UpdateXYData();
}

void DPO_ViewXYPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ViewXYPS*>(inObj)->SetAllSensitive();
}

void DPO_ViewXYPS::SetAllSensitive()
{
}

void DPO_ViewXYPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.AllFormAttach();
    inputXYObjRefUI.InitFramedUI("Input XY Data", URform.GetWidget(), args);
    inputXYObjRefUI.AddOtherObjectCallback(UpdateFromObjCB, this);

    args.Clear();
    args.StdFormAttach();
    formatForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    xFormatUI.InitFramedUI("X Data Format", formatForm.GetWidget(), args);
    xFormatUI.formatTypeChooser.AddOtherObjectCallback(UpdateFormatCB, this);
    xFormatUI.formatPrecChooser.AddOtherObjectCallback(UpdateFormatCB, this);

    args.StdSplitFinish();
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


void DPO_ViewXYPS::MenuOpen(DPO_ViewXY& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputXYObjRefUI.StdMenuInit(inData.inputXYObjRef);
}

void DPO_ViewXYPS::SetMenuData(DPO_ViewXY&  inData)
{
    MenuBaseC::SetMenuData(inData);

    inputXYObjRefUI.SetFuncObjRef(inData.inputXYObjRef);
    xFormatUI.SetRealFormatSettings(inData.xFormat);
    yFormatUI.SetRealFormatSettings(inData.yFormat);

    xFormat = inData.xFormat;
    yFormat = inData.yFormat;

    UpdateFromObj();

}


bool DPO_ViewXYPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    return allOK;
}

void DPO_ViewXYPS::GetMenuData(DPO_ViewXY& outData)
{
    MenuBaseC::GetMenuData(outData);

    bool allOK = true;
    if (inputXYObjRefUI.FuncObjRefOK(allOK, "Data source")) {
        outData.inputXYObjRef = inputXYObjRefUI.GetFuncObjRef();
    }

    xFormatUI.GetRealFormatSettings(outData.xFormat);
    yFormatUI.GetRealFormatSettings(outData.yFormat);
}


