///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CombineTablesPS.cpp
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

#include <string.h>

#include <genClass/U_Str.h>

#include "DPO_CombineTables.h"
#include "DPO_CombineTablesPS.h"


DPO_CombineTablesPS::TableDataUIC::TableDataUIC()
    : tableObjRefUI(typeid(DO_TableData))
{
    tableDataOK = false;
}


void DPO_CombineTablesPS::TableDataUIC::SetSelectedColColumns()
{
    tableDataOK = tableObjRefUI.CheckDataObj();
    if (tableDataOK)
    {
        const SC_StringArray& colDesc = static_cast<DO_TableData*>(tableObjRefUI.GetDataObj())->tableData->columnDesc;;
        selectedColUI.SetListText(colDesc);
    }
    else
    {
        selectedColUI.SetListNA();
    }
    SetAllSensitive();
}

void DPO_CombineTablesPS::TableDataUIC::SetSelectedColColumnsCB(void* inObj)
{
    static_cast<TableDataUIC*>(inObj)->SetSelectedColColumns();
}


void DPO_CombineTablesPS::TableDataUIC::ObjChange()
{
    if (tableDataOK)
    {
        char tempStr[DFO_CombineTables::maxColumnID];
        const SC_StringArray& colDesc = static_cast<DO_TableData*>(tableObjRefUI.GetDataObj())->tableData->columnDesc;
        int colNum = selectedColUI.GetSelection();
        if (colNum < colDesc.Size())
        {
            CopyString(tempStr, colDesc[colNum], DFO_CombineTables::maxColumnID);
            columnIDUI.SetFieldString(tempStr);
        }
    }
}

void DPO_CombineTablesPS::TableDataUIC::ObjChangeCB(void* inObj)
{
     static_cast<DPO_CombineTablesPS::TableDataUIC*>(inObj)->ObjChange();
}


void DPO_CombineTablesPS::TableDataUIC::InitTableData()
{
    static const int specFormwidth = 700;

    ArgListC args;

    args.TopFormAttach();
    args.LeftFormAttach();
    doTableColUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doTableColUI.AddOtherObjectCallback(SetSelectedColColumnsCB, this);

    args.Clear();
    args.TopFormAttach();
    args.SpecifiedWidthAttach(5, 40, specFormwidth);
    tableObjRefUI.InitFormUI(outsideForm.GetWidget(), args);
    tableObjRefUI.AddOtherObjectCallback(SetSelectedColColumnsCB, this);
    tableObjRefUI.AddOtherObjectCallback(ObjChangeCB, this);

    args.Clear();
    args.TopFormAttach();
    args.SpecifiedWidthAttach(41, 72, specFormwidth);
    selectedColUI.InitWidget(outsideForm.GetWidget(), args);
    selectedColUI.AddOtherObjectCallback(ObjChangeCB, this);

    args.Clear();
    args.TopFormAttach();
    args.SpecifiedWidthAttach(73, 99, specFormwidth);
    columnIDUI.SetFieldLen(DFO_CombineTables::maxColumnID);
    columnIDUI.InitWidget(outsideForm.GetWidget(), args);

}

void DPO_CombineTablesPS::TableDataUIC::SetTableData(const DFO_CombineTables::TableData& tableData)
{
    doTableColUI.SetValue(tableData.doTableCol);
    tableObjRefUI.SetFuncObjRef(tableData.tableObjRef);
    selectedColUI.SetSelection(tableData.tableColumnIndex);
    columnIDUI.SetFieldString(tableData.columnID);
    SetSelectedColColumns();
}

void DPO_CombineTablesPS::TableDataUIC::GetTableData(DFO_CombineTables::TableData& tableData)
{
    tableData.doTableCol = doTableColUI.GetValue();
    tableData.tableObjRef = tableObjRefUI.GetFuncObjRef();
    tableData.tableColumnIndex = selectedColUI.GetSelection();
    columnIDUI.GetFieldString(tableData.columnID, DFO_CombineTables::maxColumnID);
}

bool DPO_CombineTablesPS::TableDataUIC::TableDataOK(bool& prevOK)
{
    if (doTableColUI.GetValue())
    {
        tableObjRefUI.FuncObjRefOK(prevOK, "Table input");
    }
    return prevOK;
}


void DPO_CombineTablesPS::TableDataUIC::SetAllSensitive()
{
    doTableColUI.SetSensitive(true);
    bool doTableCol = doTableColUI.GetValue();
    tableObjRefUI.SetSensitive(doTableCol);
    selectedColUI.SetSensitive(doTableCol && tableDataOK);
    columnIDUI.SetSensitive(doTableCol);
}

CSize DPO_CombineTablesPS::RealScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 700);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// DPO_CombineTablesPS

DPO_CombineTablesPS::DPO_CombineTablesPS()
{
}


void DPO_CombineTablesPS::SetAllSensitive()
{
    for (int i = 0; i < DFO_CombineTables::maxTableCol; i++)
        tableDataUI[i].SetSelectedColColumns();
}


void DPO_CombineTablesPS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.AllFormAttach();
    tableColFrame.InitFramedUI("Table Columns to Combine", PPform.GetWidget(), args);

    args.Clear();
    args.AllFormAttach();
    args.LeftOffset(5);
    args.RightOffset(5);

    //  Radim: if this is not put in the space where the horizontal scroll bar would
    //  go wipes out the top of the attached statusFrame
    args.BottomOffset(25);

    scrollArea.InitWidget(tableColFrame.GetForm(), args);

    args.Clear();
    args.StdFormAttach();
    for (int i = 0; i < DFO_CombineTables::maxTableCol; i++)
    {
        tableDataUI[i].outsideForm.InitWidget(scrollArea, args);
        tableDataUI[i].InitTableData();

        args.Clear();
        args.StdTopObjectAttach(tableDataUI[i].outsideForm.GetWidget());
        tableDataUI[i].hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(tableDataUI[i].hSep.GetWidget());
    }
}


void DPO_CombineTablesPS::MenuOpen(DPO_CombineTables&   inData)
{
    MenuBaseC::MenuOpen(false, true);
    for (int i = 0; i < DFO_CombineTables::maxTableCol; i++)
    {
        tableDataUI[i].tableObjRefUI.StdMenuInit(inData.columnData[i].tableObjRef);
    }
}


void DPO_CombineTablesPS::SetMenuData(DPO_CombineTables&    inData)
{
    for (int i = 0; i < DFO_CombineTables::maxTableCol; i++)
        tableDataUI[i].SetTableData(inData.columnData[i]);
    MenuBaseC::SetMenuData(inData);

    SetAllSensitive();
}

bool DPO_CombineTablesPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    for (int i = 0; i < DFO_CombineTables::maxTableCol; i++)
        if (!tableDataUI[i].TableDataOK(allOK))
            return false;
    return allOK;
}

void DPO_CombineTablesPS::GetMenuData(DPO_CombineTables&    outData)
{
    MenuBaseC::GetMenuData(outData);
    for (int i = 0; i < DFO_CombineTables::maxTableCol; i++)
        tableDataUI[i].GetTableData(outData.columnData[i]);
}

void DPO_CombineTablesPS::MenuClear()
{
    for (int i = 0; i < DFO_CombineTables::maxTableCol; i++)
        tableDataUI[i].doTableColUI.SetValue(false);
    SetAllSensitive();
}

