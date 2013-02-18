///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableToRealPS.cpp
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

#include "DPO_TableToReal.h"
#include "DPO_TableToRealPS.h"


DPO_TableToRealPS::DPO_TableToRealPS()
{
}

void DPO_TableToRealPS::SetSensitive()
{
    selectedIndexUI.SetSensitive(extractOpUI.GetChoice() == 4);
}

void DPO_TableToRealPS::SetSensitiveCB(void* inObj)
{
    static_cast<DPO_TableToRealPS*>(inObj)->SetSensitive();
}

void DPO_TableToRealPS::InitMenu()
{
    MenuBaseC::InitMenu();

    tableColSelUI.StdInit(URform.GetWidget(), PPform.GetWidget());

    ArgListC args;
    args.StdTopObjectAttach(tableColSelUI.selectedColUI);
    topRowForm.InitWidget(PPform.GetWidget(), args);


    static const char* opStrs[] = {"Minimum", "Maximum", "# of rows", "Last row value", "Specified row value"};

    args.StdSplitSetup();
    extractOpUI.PreInitSetUp(opStrs, 5);
    extractOpUI.InitFramedUI("Operation", topRowForm.GetWidget(), args);
    extractOpUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdSplitFinish();
    selectedIndexUI.InitFramedUI("Index of row to select (0 based)", topRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    resultStatus.InitFramedUI("Current value", PPform.GetWidget(), args);
}

void DPO_TableToRealPS::SetMenuData(DPO_TableToReal&  inData)
{
    tableColSelUI.SetTableColSel(inData);

    extractOpUI.SetChoice(int(inData.extractOp));
    selectedIndexUI.SetFieldValue(inData.selectedIndex);

    resultStatus.SetSensitive(inData.StatusOK());
    if (inData.StatusOK())
        resultStatus.SetFieldValue(inData.extractOutputDO.realVal);
    else
        resultStatus.SetStatusNA();

    MenuBaseC::SetMenuData(inData);

    SetSensitive();
}


void DPO_TableToRealPS::MenuOpen(DPO_TableToReal& inData)
{
    MenuBaseC::MenuOpen(true, false);
    tableColSelUI.MenuOpen(inData);
}


bool DPO_TableToRealPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    tableColSelUI.UIdataOK(allOK);
    if (extractOpUI.GetChoice() == 4)
        selectedIndexUI.FieldValueOK(allOK, "Selected row", 0, 1000000);
    return allOK;
}

void DPO_TableToRealPS::GetMenuData(DPO_TableToReal&  outData)
{
    MenuBaseC::GetMenuData(outData);
    tableColSelUI.GetTableColSel(outData);
    outData.extractOp =  DFO_TableToReal::ExtractOp(extractOpUI.GetChoice());
    if (extractOpUI.GetChoice() == 4)
         outData.selectedIndex = selectedIndexUI.GetFieldValue();
}


void DPO_TableToRealPS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    extractOpUI.SetChoice(0);
    selectedIndexUI.SetFieldValue(0);
}

