///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableAddBasePS.cpp
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

#include "DPO_TableAddBase.h"
#include "DPO_TableAddBasePS.h"


DPO_TableAddBasePS::TableInputDataUIC::TableInputDataUIC(const type_info& tblType)
    :tableObjRefUI(tblType)
{
}

DPO_TableAddBasePS::TableInputDataUIC::TableInputDataUIC()
    :tableObjRefUI(typeid(DO_TableData))
{
}

void DPO_TableAddBasePS::TableInputDataUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<DPO_TableAddBasePS::TableInputDataUIC*>(currObj)->SetAllSensitive();
}



void DPO_TableAddBasePS::TableInputDataUIC::InitTableInputData()
{
    static const int specFormwidth = 400;

    ArgListC args;

    args.TopFormAttach();
    args.LeftFormAttach();
    doTableUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doTableUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopFormAttach();
    args.SpecifiedWidthAttach(25, 95, specFormwidth);
    tableObjRefUI.InitFormUI(outsideForm.GetWidget(), args);
}

void DPO_TableAddBasePS::TableInputDataUIC::SetTableInputData(const DFO_TableAddBase::TableInputData& tableInputData)
{
    doTableUI.SetValue(tableInputData.doTable);
    tableObjRefUI.SetFuncObjRef(tableInputData.tableObjRef);
    SetAllSensitive();
}

void DPO_TableAddBasePS::TableInputDataUIC::GetTableInputData(DFO_TableAddBase::TableInputData& tableInputData)
{
    tableInputData.tableObjRef = tableObjRefUI.GetFuncObjRef();
    tableInputData.doTable = doTableUI.GetValue();
}

bool DPO_TableAddBasePS::TableInputDataUIC::TableInputDataOK(bool& prevOK)
{
    if (doTableUI.GetValue())
    {
        tableObjRefUI.FuncObjRefOK(prevOK, "Table input");
    }
    return prevOK;
}


void DPO_TableAddBasePS::TableInputDataUIC::SetAllSensitive()
{
    doTableUI.SetSensitive(true);
    bool doTable = doTableUI.GetValue();
    tableObjRefUI.SetSensitive(doTable);
}

CSize DPO_TableAddBasePS::TableScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 400);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// DPO_TableAddBasePS

DPO_TableAddBasePS::DPO_TableAddBasePS(const type_info& tblType)
    : inputTableObjRefUI(tblType)
{
    int ninput = DFO_TableAddBase::maxInputTable - 1;
    tableInputDataUI.Alloc(ninput);
    for (int i = 0; i < ninput; i++)
        tableInputDataUI += new TableInputDataUIC(tblType);
}

DPO_TableAddBasePS::~DPO_TableAddBasePS()
{
}

void DPO_TableAddBasePS::SetAllSensitive()
{
    if (doXFrame)
        xcolumnIndexUI.SetSensitive(listDataOK && skipXColumnUI.GetValue());
    for (int i = 1; i < DFO_TableAddBase::maxInputTable; i++)
        tableInputDataUI.GetRef(i - 1).SetAllSensitive();
}


void DPO_TableAddBasePS::ResetColumnList()
{
    listDataOK = inputTableObjRefUI.CheckDataObj();

    if (listDataOK)
    {
        const SC_StringArray& colDesc = static_cast<DO_TableData*>(inputTableObjRefUI.GetDataObj())->tableData->columnDesc;;
        xcolumnIndexUI.SetListText(colDesc);
    }
    else
    {
        xcolumnIndexUI.SetListNA();
    }

    SetAllSensitive();
}

void DPO_TableAddBasePS::ResetColumnListCB(void* inObj)
{
    static_cast<DPO_TableAddBasePS*>(inObj)->ResetColumnList();
}


void DPO_TableAddBasePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableAddBasePS*>(inObj)->SetAllSensitive();
}


void DPO_TableAddBasePS::InitMenu(bool initXFrame)
{
    MenuBaseC::InitMenu();
    ArgListC args;

    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Base Table", URform.GetWidget(), args);

    doXFrame = initXFrame;
    args.Clear();
    args.StdFormAttach();
    if (doXFrame)
    {
        inputTableObjRefUI.AddOtherObjectCallback(ResetColumnListCB, this);

        //  form for row with row/col selections
        skipXFrame.InitFramedUI("X Column Processing", PPform.GetWidget(), args);

        args.StdToggleSetup(5);
        skipXColumnUI.StdToggleInit("skip X column", skipXFrame.GetForm(), args);
        skipXColumnUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

        args.StdSplitSetup();
        args.StdSplitFinish(60);
        xcolumnIndexUI.InitWidget(skipXFrame.GetForm(), args);

        args.Clear();
        args.StdTopObjectAttach(skipXFrame);
    }

    // now main frame
    args.BottomFormAttach();
    tableColFrame.InitFramedUI("Tables to Add", PPform.GetWidget(), args);

    args.Clear();
    args.AllFormAttach();
    args.LeftOffset(5);
    args.RightOffset(5);

    args.BottomOffset(25);

    scrollArea.InitWidget(tableColFrame.GetForm(), args);

    args.Clear();
    args.StdFormAttach();
    for (int i = 0; i < DFO_TableAddBase::maxInputTable - 1; i++)
    {
        tableInputDataUI.GetRef(i).outsideForm.InitWidget(scrollArea, args);
        tableInputDataUI.GetRef(i).InitTableInputData();

        args.Clear();
        args.StdTopObjectAttach(tableInputDataUI.GetRef(i).outsideForm.GetWidget());
        tableInputDataUI.GetRef(i).hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(tableInputDataUI.GetRef(i).hSep.GetWidget());
    }
}


void DPO_TableAddBasePS::MenuOpen(DPO_TableAddBase&   inData)
{
    MenuBaseC::MenuOpen(false, true);

    inputTableObjRefUI.StdMenuInit(inData.bpRef.tablesToAdd[0].tableObjRef);
    for (int i = 1; i < DFO_TableAddBase::maxInputTable; i++)
    {
        tableInputDataUI.GetRef(i - 1).tableObjRefUI.StdMenuInit(inData.bpRef.tablesToAdd[i].tableObjRef);
    }
}


void DPO_TableAddBasePS::SetMenuData(DPO_TableAddBase&    inData)
{
    MenuBaseC::SetMenuData(inData);

    inputTableObjRefUI.SetFuncObjRef(inData.bpRef.tablesToAdd[0].tableObjRef);
    for (int i = 1; i < DFO_TableAddBase::maxInputTable; i++)
        tableInputDataUI.GetRef(i - 1).SetTableInputData(inData.bpRef.tablesToAdd[i]);

    if (doXFrame)
    {
        skipXColumnUI.SetValue(inData.bpRef.skipXColumn);
        xcolumnIndexUI.SetSelection(inData.bpRef.xcolumnIndex);
        ResetColumnList();
    }

    SetAllSensitive();
}

bool DPO_TableAddBasePS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    inputTableObjRefUI.FuncObjRefOK(allOK, "Base table");
    for (int i = 1; i < DFO_TableAddBase::maxInputTable; i++)
        if (!tableInputDataUI.GetRef(i - 1).TableInputDataOK(allOK))
            return false;

    return allOK;
}

void DPO_TableAddBasePS::GetMenuData(DPO_TableAddBase&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.bpRef.tablesToAdd[0].tableObjRef = inputTableObjRefUI.GetFuncObjRef();

    if (doXFrame)
    {
        outData.bpRef.skipXColumn = skipXColumnUI.GetValue();
        outData.bpRef.xcolumnIndex = xcolumnIndexUI.GetSelection();
    }
    for (int i = 1; i < DFO_TableAddBase::maxInputTable; i++)
        tableInputDataUI.GetRef(i - 1).GetTableInputData(outData.bpRef.tablesToAdd[i]);
}

