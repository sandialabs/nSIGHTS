///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableToXYPS.cpp
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

#include "DPO_TableToXY.h"
#include "DPO_TableToXYPS.h"


DPO_TableToXYPS::DPO_TableToXYPS()
{
}

void DPO_TableToXYPS::SetSelectedYColTableColumns()
{
    tableColSelUI.SetTableColumns(selectedYColUI);
    selectedYColUI.SetSensitive(tableColSelUI.tableDataOK);
}

void DPO_TableToXYPS::SetSelectedYColTableColumnsCB(void* inObj)
{
    static_cast<DPO_TableToXYPS*>(inObj)->SetSelectedYColTableColumns();
}


void DPO_TableToXYPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    tableColSelUI.ObjRefInit(URform.GetWidget(), args);
    tableColSelUI.tableInputObjRefUI.AddOtherObjectCallback(SetSelectedYColTableColumnsCB, this);

    args.Clear();
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    tableColSelUI.selectedColUI.InitFramedUI("X Data Column", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    selectedYColUI.InitFramedUI("Y Data Column", topRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    outputDescUI.SetFieldLen(40);
    outputDescUI.InitFramedUI("Output Description", PPform.GetWidget(), args);
}

void DPO_TableToXYPS::SetMenuData(DPO_TableToXY&  inData)
{
    tableColSelUI.SetTableColSel(inData);
    selectedYColUI.SetSelection(inData.selectedYCol);
    outputDescUI.SetFieldString(inData.outputDesc);

    MenuBaseC::SetMenuData(inData);
    SetSelectedYColTableColumns();
}


void DPO_TableToXYPS::MenuOpen(DPO_TableToXY& inData)
{
    MenuBaseC::MenuOpen(false, false);
    tableColSelUI.MenuOpen(inData);
}


bool DPO_TableToXYPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    tableColSelUI.UIdataOK(allOK);
    return allOK;
}

void DPO_TableToXYPS::GetMenuData(DPO_TableToXY&  outData)
{
    MenuBaseC::GetMenuData(outData);
    tableColSelUI.GetTableColSel(outData);
    outData.selectedYCol = selectedYColUI.GetSelection();
    outputDescUI.GetFieldString(outData.outputDesc, DPO_TableToXY::outputDescLen);
}

