///////////////////////////////////////////////////////////////////////////////////
//
// LPO_SATablePS.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "SaLibAfx.h"

#include <genClass/U_Str.h>

#include <saClass/DO_SATable.h>

#include "LPO_SATable.h"
#include "LPO_SATablePS.h"

LPO_SATablePS::LPO_SATablePS() :
    tableObjRefUI(typeid(DO_SATable))
{
}


void LPO_SATablePS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    tableObjRefUI.InitFramedUI("SA Table Data To List", URform.GetWidget(), args);

    args.StdFormAttach();
    const char* listFormatStr[3] = {"WIPP Input", "Step Table", "Variable Table"};
    listFormatUI.PreInitSetUp(listFormatStr, 3);
    listFormatUI.InitFramedUI("Listing Format", PPform.GetWidget(), args);
}


void LPO_SATablePS::MenuOpen(LPO_SATable&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    tableObjRefUI.StdMenuInit(inData.tableObjRef);
}


void LPO_SATablePS::SetMenuData(LPO_SATable&    inData)
{
    MenuBaseC::SetMenuData(inData);
    tableObjRefUI.SetFuncObjRef(inData.tableObjRef);
    listFormatUI.SetChoice(int(inData.listFormat));
}

bool LPO_SATablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    tableObjRefUI.FuncObjRefOK(allOK, "Transfer table data source");
    return allOK;
}

void LPO_SATablePS::GetMenuData(LPO_SATable&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.tableObjRef = tableObjRefUI.GetFuncObjRef();
    outData.listFormat = LFO_SATable::ListFormat(listFormatUI.GetChoice());
}

