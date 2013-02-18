///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableToGridPS.cpp
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

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/DO_TableData.h>

#include "DPO_TableToGrid.h"
#include "DPO_TableToGridPS.h"


DPO_TableToGridPS::DPO_TableToGridPS()
{
}

void DPO_TableToGridPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableToGridPS*>(inObj)->SetAllSensitive();
}

void DPO_TableToGridPS::SetAllSensitive()
{
    bool isGrid = convertFormatUI.GetChoice() == 0;
    gridRowForm.SetSensitive(isGrid);
    useColumnIndexUI.SetSensitive(isGrid && columnValueIsHeaderUI.GetBoolChoice());

    bool isXYZList = convertFormatUI.GetChoice() == 1;
    xyzListRowForm.SetSensitive(isXYZList);
    tableColSelUI.selectedColUI.SetSensitive(isXYZList && tableColSelUI.tableDataOK);
    ycolumnIndexUI.SetSensitive(isXYZList && tableColSelUI.tableDataOK);
    dataColumnIndexUI.SetSensitive(isXYZList && tableColSelUI.tableDataOK);
}

void DPO_TableToGridPS::SetTableColumns()
{
    tableColSelUI.SetTableColumns(ycolumnIndexUI);
    tableColSelUI.SetTableColumns(dataColumnIndexUI);
    SetAllSensitive();
}

void DPO_TableToGridPS::SetTableColumnsCB(void* inObj)
{
    static_cast<DPO_TableToGridPS*>(inObj)->SetTableColumns();
}


void DPO_TableToGridPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    tableColSelUI.ObjRefInit(URform.GetWidget(), args);
    tableColSelUI.tableInputObjRefUI.AddOtherObjectCallback(SetTableColumnsCB, this);

    args.StdFormAttach();
    static const char* cfStrs[] = {"Grid", "XYZ List", "Other"};
    convertFormatUI.PreInitSetUp(cfStrs, 3);
    convertFormatUI.InitFramedUI("Conversion Format", PPform.GetWidget(), args);
    convertFormatUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(convertFormatUI);
    gridRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    columnValueIsHeaderUI.PreInitSetUp("Table Row 1", "Column Header");
    columnValueIsHeaderUI.InitFramedUI("Column Coordinate Data", gridRowForm.GetWidget(), args);
    columnValueIsHeaderUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(34, 66);
    useColumnIndexUI.PreInitSetUp("Value", "Index");
    useColumnIndexUI.InitFramedUI("Header Data", gridRowForm.GetWidget(), args);

    args.StdSplitFinish(33);
    gridXIsColumnsUI.PreInitSetUp("Table Rows", "Table Columns");
    gridXIsColumnsUI.InitFramedUI("Grid X Coordinate Data", gridRowForm.GetWidget(), args);

    args.StdTopObjectAttach(gridRowForm.GetWidget());
    xyzListRowForm.InitWidget(PPform.GetWidget(), args);
    args.StdSplitSetup(33);
    tableColSelUI.selectedColUI.InitFramedUI("Grid X Column", xyzListRowForm.GetWidget(), args);
    args.StdSplitMiddle(34, 66);
    ycolumnIndexUI.InitFramedUI("Grid Y Column", xyzListRowForm.GetWidget(), args);
    args.StdSplitFinish(33);
    dataColumnIndexUI.InitFramedUI("Grid Data Column", xyzListRowForm.GetWidget(), args);


    args.StdTopObjectAttach(xyzListRowForm.GetWidget());
    idRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    xdataIDUI.SetFieldLen(DC_DataCoord::dataIDLen);
    xdataIDUI.AttachObjectToFrame();
    xdataIDUI.InitFramedUI("Grid X data ID", idRowForm.GetWidget(), args);

    args.StdSplitFinish();
    ydataIDUI.SetFieldLen(DC_DataCoord::dataIDLen);
    ydataIDUI.AttachObjectToFrame();
    ydataIDUI.InitFramedUI("Grid Y data ID", idRowForm.GetWidget(), args);

    args.StdTopObjectAttach(idRowForm.GetWidget());
    optionForm.InitFramedUI("Conversion Options", PPform.GetWidget(), args);

    args.StdSplitSetup(25);
    tableRowModulusUI.AttachLabelToObject();
    tableRowModulusUI.SetFieldLen(4);
    tableRowModulusUI.InitLabeledUI("row modulus", optionForm.GetForm(), args);

    args.StdToggleSetup(35);
    includeLastRowUI.StdToggleInit("keep last row",optionForm.GetForm(), args);

    args.StdSplitMiddle(50, 75);
    tableColModulusUI.AttachLabelToObject();
    tableColModulusUI.SetFieldLen(4);
    tableColModulusUI.InitLabeledUI("col modulus", optionForm.GetForm(), args);

    args.StdToggleSetup(85);
    includeLastColUI.StdToggleInit("keep last col",optionForm.GetForm(), args);

    args.StdTopObjectAttach(optionForm);
    xcoordStatus.StdInit("Grid X Status", PPform.GetWidget(), args);

    args.StdTopObjectAttach(xcoordStatus.coordFrame);
    ycoordStatus.StdInit("Grid Y Status", PPform.GetWidget(), args);
}

void DPO_TableToGridPS::SetMenuData(DPO_TableToGrid&  inData)
{
    MenuBaseC::SetMenuData(inData);

    tableColSelUI.SetTableColSel(inData);
    convertFormatUI.SetChoice(int(inData.convertFormat));

    columnValueIsHeaderUI.SetBoolChoice(inData.columnValueIsHeader);
    gridXIsColumnsUI.SetBoolChoice(inData.gridXIsColumns);
    useColumnIndexUI.SetBoolChoice(inData.useColumnIndex);

    ycolumnIndexUI.SetSelection(inData.ycolumnIndex);
    dataColumnIndexUI.SetSelection(inData.dataColumnIndex);

    tableRowModulusUI.SetFieldValue(inData.tableRowModulus);
    tableColModulusUI.SetFieldValue(inData.tableColModulus);

    includeLastRowUI.SetValue(inData.includeLastRow);
    includeLastColUI.SetValue(inData.includeLastCol);

    xdataIDUI.SetFieldString(inData.xdataID);
    ydataIDUI.SetFieldString(inData.ydataID);

    if (inData.StatusOK())
    {
        xcoordStatus.SetOKStatus(inData.outputGrid.xData);
        ycoordStatus.SetOKStatus(inData.outputGrid.yData);
    }
    else
    {
        xcoordStatus.SetBadStatus();
        ycoordStatus.SetBadStatus();
    }

    SetTableColumns();
}


void DPO_TableToGridPS::MenuOpen(DPO_TableToGrid& inData)
{
    MenuBaseC::MenuOpen(false, false);
    tableColSelUI.MenuOpen(inData);
}


bool DPO_TableToGridPS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    tableColSelUI.UIdataOK(allOK);
    tableRowModulusUI.FieldValueOK(allOK, "row modulus", 1, 20);
    tableColModulusUI.FieldValueOK(allOK, "col modulus", 1, 20);
    return allOK;
}

void DPO_TableToGridPS::GetMenuData(DPO_TableToGrid&  outData)
{
    MenuBaseC::GetMenuData(outData);

    tableColSelUI.GetTableColSel(outData);

    outData.convertFormat = DFO_TableToGrid::ConvertFormat(convertFormatUI.GetChoice());

    if (outData.convertFormat == DFO_TableToGrid::cfGrid)
    {
        outData.columnValueIsHeader = columnValueIsHeaderUI.GetBoolChoice();
        if (outData.columnValueIsHeader)
            outData.useColumnIndex = useColumnIndexUI.GetBoolChoice();
        outData.gridXIsColumns = gridXIsColumnsUI.GetBoolChoice();
    }
    else if (outData.convertFormat == DFO_TableToGrid::cfXYZList)
    {
        outData.ycolumnIndex = ycolumnIndexUI.GetSelection();
        outData.dataColumnIndex = dataColumnIndexUI.GetSelection();
    }
    outData.includeLastRow = includeLastRowUI.GetValue();
    outData.includeLastCol = includeLastColUI.GetValue();
    outData.tableRowModulus = tableRowModulusUI.GetFieldValue();
    outData.tableColModulus = tableColModulusUI.GetFieldValue();

    xdataIDUI.GetFieldString(outData.xdataID, DC_GridData::gridIDLen);
    ydataIDUI.GetFieldString(outData.ydataID, DC_GridData::gridIDLen);
}

