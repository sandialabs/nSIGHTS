///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableConcatPS.cpp
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

#include "DPO_TableConcat.h"
#include "DPO_TableConcatPS.h"


DPO_TableConcatPS::TableInputDataUIC::TableInputDataUIC()
    :tableObjRefUI(typeid(DO_TableData))
{
}


void DPO_TableConcatPS::TableInputDataUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<DPO_TableConcatPS::TableInputDataUIC*>(currObj)->SetAllSensitive();
}



void DPO_TableConcatPS::TableInputDataUIC::InitTableInputData()
{
    ArgListC args;

    args.StdToggleSetup(2);
    doTableUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doTableUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(80);
    tableObjRefUI.InitWidget(outsideForm.GetWidget(), args);
}

void DPO_TableConcatPS::TableInputDataUIC::SetTableInputData(const DFO_TableConcat::TableInputData& tableInputData)
{
    doTableUI.SetValue(tableInputData.doTable);
    tableObjRefUI.SetFuncObjRef(tableInputData.tableObjRef);
    SetAllSensitive();
}

void DPO_TableConcatPS::TableInputDataUIC::GetTableInputData(DFO_TableConcat::TableInputData& tableInputData)
{
    tableInputData.tableObjRef = tableObjRefUI.GetFuncObjRef();
    tableInputData.doTable = doTableUI.GetValue();
}

bool DPO_TableConcatPS::TableInputDataUIC::TableInputDataOK(bool& prevOK)
{
    if (doTableUI.GetValue())
    {
        tableObjRefUI.FuncObjRefOK(prevOK, "Table input");
    }
    return prevOK;
}


void DPO_TableConcatPS::TableInputDataUIC::SetAllSensitive()
{
    doTableUI.SetSensitive(true);
    bool doTable = doTableUI.GetValue();
    tableObjRefUI.SetSensitive(doTable);
}

CSize DPO_TableConcatPS::TableScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 600);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// DPO_TableConcatPS

DPO_TableConcatPS::DPO_TableConcatPS()
    : inputTableObjRefUI(typeid(DO_TableData))
{
    int ninput = DFO_TableConcat::maxInputTable - 1;
    tableInputDataUI.Alloc(ninput);
    for (int i = 0; i < ninput; i++)
        tableInputDataUI += new TableInputDataUIC();
}

DPO_TableConcatPS::~DPO_TableConcatPS()
{
}

void DPO_TableConcatPS::SetAllSensitive()
{
    skipFirstColumnUI.SetSensitive(!concatRowsUI.GetBoolChoice());
    for (int i = 1; i < DFO_TableConcat::maxInputTable; i++)
        tableInputDataUI.GetRef(i - 1).SetAllSensitive();
}


void DPO_TableConcatPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableConcatPS*>(inObj)->SetAllSensitive();
}


void DPO_TableConcatPS::InitMenu()
{
    MenuBaseC::InitMenu();
    ArgListC args;

    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Base Table", URform.GetWidget(), args);

    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    concatRowsUI.PreInitSetUp("Columns", "Rows");
    concatRowsUI.InitFramedUI("Concatenate Operation", topRowForm.GetWidget(), args);
    concatRowsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    optionFrame.InitFramedUI("Options", topRowForm.GetWidget(), args);

    args.StdToggleSetup(5);
    skipFirstColumnUI.StdToggleInit("skip first column when adding", optionFrame.GetForm(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    args.BottomFormAttach();
    tableColFrame.InitFramedUI("Tables to Concatenate", PPform.GetWidget(), args);

    args.Clear();
    args.AllFormAttach();
    args.LeftOffset(5);
    args.RightOffset(5);
    args.BottomOffset(25);
    scrollArea.InitWidget(tableColFrame.GetForm(), args);

    args.StdFormAttach();
    for (int i = 0; i < DFO_TableConcat::maxInputTable - 1; i++)
    {
        tableInputDataUI.GetRef(i).outsideForm.InitWidget(scrollArea, args);
        tableInputDataUI.GetRef(i).InitTableInputData();

        args.StdTopObjectAttach(tableInputDataUI.GetRef(i).outsideForm.GetWidget());
        tableInputDataUI.GetRef(i).hSep.InitWidget(scrollArea, args);

        args.StdTopObjectAttach(tableInputDataUI.GetRef(i).hSep.GetWidget());
    }
}


void DPO_TableConcatPS::MenuOpen(DPO_TableConcat&   inData)
{
    MenuBaseC::MenuOpen(false, true);

    inputTableObjRefUI.StdMenuInit(inData.tablesToConcat[0].tableObjRef);
    for (int i = 1; i < DFO_TableConcat::maxInputTable; i++)
    {
        tableInputDataUI.GetRef(i - 1).tableObjRefUI.StdMenuInit(inData.tablesToConcat[i].tableObjRef);
    }
}


void DPO_TableConcatPS::SetMenuData(DPO_TableConcat&    inData)
{
    MenuBaseC::SetMenuData(inData);

    inputTableObjRefUI.SetFuncObjRef(inData.tablesToConcat[0].tableObjRef);
    concatRowsUI.SetBoolChoice(inData.concatRows);
    skipFirstColumnUI.SetValue(inData.skipFirstColumn);
    for (int i = 1; i < DFO_TableConcat::maxInputTable; i++)
        tableInputDataUI.GetRef(i - 1).SetTableInputData(inData.tablesToConcat[i]);

    SetAllSensitive();
}

bool DPO_TableConcatPS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    inputTableObjRefUI.FuncObjRefOK(allOK, "Base table");
    for (int i = 1; i < DFO_TableConcat::maxInputTable; i++)
        if (!tableInputDataUI.GetRef(i - 1).TableInputDataOK(allOK))
            return false;

    return allOK;
}

void DPO_TableConcatPS::GetMenuData(DPO_TableConcat&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.concatRows = concatRowsUI.GetBoolChoice();
    if (!outData.concatRows)
        outData.skipFirstColumn = skipFirstColumnUI.GetValue();

    outData.tablesToConcat[0].tableObjRef = inputTableObjRefUI.GetFuncObjRef();
    for (int i = 1; i < DFO_TableConcat::maxInputTable; i++)
        tableInputDataUI.GetRef(i - 1).GetTableInputData(outData.tablesToConcat[i]);
}

