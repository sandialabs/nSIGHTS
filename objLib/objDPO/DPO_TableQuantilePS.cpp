///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableQuantilePS.cpp
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

#include "DPO_TableQuantile.h"
#include "DPO_TableQuantilePS.h"


DPO_TableQuantilePS::DPO_TableQuantilePS()
{
}


void DPO_TableQuantilePS::SetAreaColumnTableColumns()
{
    tableColSelUI.SetTableColumns(areaColumnUI);
    areaColumnUI.SetSensitive(tableColSelUI.tableDataOK && areaWeightedUI.GetBoolChoice());
}

void DPO_TableQuantilePS::SetAreaColumnTableColumnsCB(void* inObj)
{
    static_cast<DPO_TableQuantilePS*>(inObj)->SetAreaColumnTableColumns();
}


void DPO_TableQuantilePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableQuantilePS*>(inObj)->SetAllSensitive();
}

void DPO_TableQuantilePS::SetAllSensitive()
{
    areaColumnUI.SetSensitive(tableColSelUI.tableDataOK && areaWeightedUI.GetBoolChoice());
    quantileTypeUI.SetSensitive(!areaWeightedUI.GetBoolChoice());
    quantileMSUI.SetSensitive((!areaWeightedUI.GetBoolChoice()) && (quantileTypeUI.GetChoice() == 3));
}

void DPO_TableQuantilePS::InitMenu()
{
    DPO_QuantilePS::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    tableColSelUI.ObjRefInit(URform.GetWidget(), args);
    tableColSelUI.tableInputObjRefUI.AddOtherObjectCallback(SetAreaColumnTableColumnsCB, this);

    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(30);
    areaWeightedUI.PreInitSetUp("normal", "area weighted");
    areaWeightedUI.InitFramedUI("CDF Type", topRow.GetWidget(), args);
    areaWeightedUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(30, 65);
    tableColSelUI.selectedColUI.InitFramedUI("Data Column", topRow.GetWidget(), args);

    args.StdSplitFinish(35);
    areaColumnUI.InitFramedUI("Area Column", topRow.GetWidget(), args);

    args.StdTopObjectAttach(topRow.GetWidget());
    InitQuantileCommon(args);
}

void DPO_TableQuantilePS::SetMenuData(DPO_TableQuantile&  inData)
{
    tableColSelUI.SetTableColSel(inData);
    DPO_QuantilePS::SetMenuData(inData);
    areaColumnUI.SetSelection(inData.areaColumn);
    areaWeightedUI.SetBoolChoice(inData.areaWeighted);
    SetAreaColumnTableColumns();
    SetAllSensitive();
}

void DPO_TableQuantilePS::MenuOpen(DPO_TableQuantile& inData)
{
    DPO_QuantilePS::MenuOpen(inData);
    tableColSelUI.MenuOpen(inData);
}


bool DPO_TableQuantilePS::UIdataOK()
{
    DPO_QuantilePS::UIdataOK();
    tableColSelUI.UIdataOK(allOK);
    return allOK;
}

void DPO_TableQuantilePS::GetMenuData(DPO_TableQuantile&  outData)
{
    DPO_QuantilePS::GetMenuData(outData);
    tableColSelUI.GetTableColSel(outData);
    outData.areaWeighted = areaWeightedUI.GetBoolChoice();
    if (outData.areaWeighted)
        outData.areaColumn = areaColumnUI.GetSelection();
}


