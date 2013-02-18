///////////////////////////////////////////////////////////////////////////////////
//
// DPO_RealToTablePS.cpp
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

#include "DPO_RealToTable.h"
#include "DPO_RealToTablePS.h"


DPO_RealToTablePS::RealDataUIC::RealDataUIC() : realObjRefUI(typeid(DO_Real))
{
}

void DPO_RealToTablePS::RealDataUIC::SetAllSensitiveCB(void* currObj)
{
 static_cast<DPO_RealToTablePS::RealDataUIC*>(currObj)->SetAllSensitive();
}

void DPO_RealToTablePS::RealDataUIC::ObjChange()
{
    if (realObjRefUI.CheckDataObj())
    {
        char tempStr[DFO_RealToTable::maxColumnID];
//RealIDUI.GetFieldString(tempStr, DC_RealWithID::RealIDlen);
//if (StringLength(tempStr) == 0)
        {
            CopyString(tempStr, realObjRefUI.GetFuncObjRef().objRef->GetID(), DFO_RealToTable::maxColumnID);
            columnIDUI.SetFieldString(tempStr);
        }
    }
}

void DPO_RealToTablePS::RealDataUIC::ObjChangeCB(void* inObj)
{
     static_cast<DPO_RealToTablePS::RealDataUIC*>(inObj)->ObjChange();
}


void DPO_RealToTablePS::RealDataUIC::InitRealData()
{
    static const int specFormwidth = 600;

    ArgListC args;

    args.TopFormAttach();
    args.LeftFormAttach();
    doRealUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doRealUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopFormAttach();
    args.RightPosAttach(50);
    realObjRefUI.InitFormUI(outsideForm.GetWidget(), args);
    realObjRefUI.AddOtherObjectCallback(ObjChangeCB, this);

    args.Clear();
    args.TopFormAttach();
    args.SpecifiedWidthAttach(55, 95, specFormwidth);
    columnIDUI.SetFieldLen(DFO_RealToTable::maxColumnID);
    columnIDUI.InitWidget(outsideForm.GetWidget(), args);

}

void DPO_RealToTablePS::RealDataUIC::SetRealData(const DFO_RealToTable::RealData& realData)
{
    doRealUI.SetValue(realData.doReal);
    realObjRefUI.SetFuncObjRef(realData.realObjRef);
    columnIDUI.SetFieldString(realData.columnID);
    SetAllSensitive();
}

void DPO_RealToTablePS::RealDataUIC::GetRealData(DFO_RealToTable::RealData& realData)
{
    realData.realObjRef = realObjRefUI.GetFuncObjRef();
    realData.doReal = doRealUI.GetValue();
    columnIDUI.GetFieldString(realData.columnID, DFO_RealToTable::maxColumnID);
}

bool DPO_RealToTablePS::RealDataUIC::RealDataOK(bool& prevOK)
{
    if (doRealUI.GetValue())
    {
        realObjRefUI.FuncObjRefOK(prevOK, "Real input");
    }
    return prevOK;
}


void DPO_RealToTablePS::RealDataUIC::SetAllSensitive()
{
    doRealUI.SetSensitive(true);
    bool doReal = doRealUI.GetValue();
    realObjRefUI.SetSensitive(doReal);
    columnIDUI.SetSensitive(doReal);
}

CSize DPO_RealToTablePS::RealScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 600);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// DPO_RealToTablePS

DPO_RealToTablePS::DPO_RealToTablePS()
{
}


void DPO_RealToTablePS::SetAllSensitive()
{
    for (int i = 0; i < DFO_RealToTable::maxTableCol; i++)
        realDataUI[i].SetAllSensitive();
    clearTableRows.SetSensitive(currObj->StatusOK());
    statusFrame.SetSensitive(currObj->StatusOK());
}

void DPO_RealToTablePS::ClearTableRows()
{
    currObj->realTable.ClearData();
    currObj->CalcDownStreamObjects();
    SetMenuData(*currObj);
}

void DPO_RealToTablePS::ClearTableRowsCB(void* inObj)
{
    static_cast<DPO_RealToTablePS*>(inObj)->ClearTableRows();
}

void DPO_RealToTablePS::ClearAllTableRows()
{
    FuncObjArray    tableFO;
    tableFO.SetResizable(8);
    for (int i = 0; i < FuncObjC::funcObjList.Size(); i++)
    {
        DFO_RealToTable* currFO = dynamic_cast<DFO_RealToTable*>(FuncObjC::funcObjList[i]);
        if (currFO != 0)
        {
            currFO->ClearTable();
            tableFO += currFO;
        }
    }

    FuncObjC::CalcSelectedObjects(foc_PortOnly, tableFO);
    SetMenuData(*currObj);
}

void DPO_RealToTablePS::ClearAllTableRowsCB(void* inObj)
{
    static_cast<DPO_RealToTablePS*>(inObj)->ClearAllTableRows();
}


void DPO_RealToTablePS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    urFrame.InitFramedUI("Table Options", URform.GetWidget(), args);

    args.Clear();
    args.LeftPosAttach(5);
    args.RightPosAttach(25);
    args.BottomFormAttach(5);
    clearTableRows.StdButtonInit(" Clear Rows ", urFrame.GetForm(), args);
    clearTableRows.AddOtherObjectCallback(ClearTableRowsCB, this);

    args.Clear();
    args.LeftPosAttach(30);
    args.RightPosAttach(55);
    args.BottomFormAttach(5);
    clearAllTableRows.StdButtonInit(" Clear All Tables ", urFrame.GetForm(), args);
    clearAllTableRows.AddOtherObjectCallback(ClearAllTableRowsCB, this);


    args.Clear();
    args.LeftPosAttach(75);
    args.BottomFormAttach(5);
    saveTableDataUI.StdToggleInit("Save table data", urFrame.GetForm(), args);

    // status area first at bottom
    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomFormAttach();
    statusFrame.InitFramedUI("Status", PPform.GetWidget(), args);

    args.StdSplitSetup();
    args.LeftOffset(10);
    args.RightOffset(20);
    nrowsStatus.InitLabeledUI("# of rows", statusFrame.GetForm(), args);

    args.StdSplitFinish();
    args.LeftOffset(20);
    // Radim: the offset doesn't have any effect
//  args.RightOffset(20);
    ncolsStatus.InitLabeledUI("# of columns", statusFrame.GetForm(), args);

    // now main frame
    args.Clear();
    args.StdFormAttach();
    args.BottomUICAttach(statusFrame);
    tableColFrame.InitFramedUI("Table Columns", PPform.GetWidget(), args);

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
    for (int i = 0; i < DFO_RealToTable::maxTableCol; i++)
    {
        realDataUI[i].outsideForm.InitWidget(scrollArea, args);
        realDataUI[i].InitRealData();

        args.Clear();
        args.StdTopObjectAttach(realDataUI[i].outsideForm.GetWidget());
        realDataUI[i].hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(realDataUI[i].hSep.GetWidget());
    }
}


void DPO_RealToTablePS::MenuOpen(DPO_RealToTable&   inData)
{
    MenuBaseC::MenuOpen(false, true);
    for (int i = 0; i < DFO_RealToTable::maxTableCol; i++)
    {
        realDataUI[i].realObjRefUI.StdMenuInit(inData.columnData[i].realObjRef);
    }
}


void DPO_RealToTablePS::SetMenuData(DPO_RealToTable&    inData)
{
    currObj = &inData;
    saveTableDataUI.SetValue(inData.saveTableData);

    for (int i = 0; i < DFO_RealToTable::maxTableCol; i++)
        realDataUI[i].SetRealData(inData.columnData[i]);
    MenuBaseC::SetMenuData(inData);

    if (inData.StatusOK())
    {
        nrowsStatus.SetFieldValue(inData.realTable.GetNRows());
        ncolsStatus.SetFieldValue(inData.realTable.GetNCols());
    }
    else
    {
        nrowsStatus.SetStatusNA();
        ncolsStatus.SetStatusNA();
    }

    SetAllSensitive();
}

bool DPO_RealToTablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    for (int i = 0; i < DFO_RealToTable::maxTableCol; i++)
        if (!realDataUI[i].RealDataOK(allOK))
            return false;
    return allOK;
}

void DPO_RealToTablePS::GetMenuData(DPO_RealToTable&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.saveTableData =  saveTableDataUI.GetValue();
    for (int i = 0; i < DFO_RealToTable::maxTableCol; i++)
        realDataUI[i].GetRealData(outData.columnData[i]);
}

void DPO_RealToTablePS::MenuClear()
{
    for (int i = 0; i < DFO_RealToTable::maxTableCol; i++)
    {
        realDataUI[i].doRealUI.SetValue(false);
        realDataUI[i].SetAllSensitive();
    }
}

