///////////////////////////////////////////////////////////////////////////////////
//
// DPO_InterpTableColumnPS.cpp
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

#include "DPO_InterpTableColumn.h"
#include "DPO_InterpTableColumnPS.h"


DPO_InterpTableColumnPS::DPO_InterpTableColumnPS()
    : interpXValMSUI(typeid(DO_Real), true)
{
}

void DPO_InterpTableColumnPS::SetSelectedYColTableColumns()
{
    tableColSelUI.SetTableColumns(selectedYColUI);
    selectedYColUI.SetSensitive(tableColSelUI.tableDataOK);
}

void DPO_InterpTableColumnPS::SetSelectedYColTableColumnsCB(void* inObj)
{
    static_cast<DPO_InterpTableColumnPS*>(inObj)->SetSelectedYColTableColumns();
}

void DPO_InterpTableColumnPS::ToggleExposedInterpXValCB(void* inObj)
{
    static_cast<DPO_InterpTableColumnPS*>(inObj)->ToggleExposedInterpXVal();
}

void DPO_InterpTableColumnPS::ToggleExposedInterpXVal()
{
    interpXValMSUI.SetAllSensitive();
    // recast current to local
    static_cast<DPO_InterpTableColumn*>(MenuObjC::currMenuObj)->exposedInterpXVal.ToggleExposed(interpXValMSUI.TurnExposedOff());
}


void DPO_InterpTableColumnPS::InitMenu()
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
    optionsFrame.InitFramedUI("Interpolate Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    logInterpXUI.StdToggleInit("Interp on log X data", optionsFrame.GetForm(), args);

    args.StdToggleSetup(50);
    logInterpYUI.StdToggleInit("Interp on log Y data", optionsFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(optionsFrame);
    interpXValMSUI.InitUI("Interpolant X", args, PPform.GetWidget(), true, true, ToggleExposedInterpXValCB, this);

    args.Clear();
    args.StdTopObjectAttach(interpXValMSUI);
    interpValStatus.InitFramedUI("Interpolation Results", PPform.GetWidget(), args);

}

void DPO_InterpTableColumnPS::SetMenuData(DPO_InterpTableColumn&  inData)
{
    tableColSelUI.SetTableColSel(inData);
    selectedYColUI.SetSelection(inData.selectedYCol);
    logInterpXUI.SetValue(inData.logInterpX);
    logInterpYUI.SetValue(inData.logInterpY);
    interpXValMSUI.SetValue(inData.interpXValMS, inData.exposedInterpXVal);

    UpdateStatus(inData);
    MenuBaseC::SetMenuData(inData);
    SetSelectedYColTableColumns();
}

void DPO_InterpTableColumnPS::UpdateStatus(DPO_InterpTableColumn&   inData)
{
    if (inData.StatusOK())
    {
        interpValStatus.SetFieldValue(inData.outputDO.realVal);
    }
    else
    {
        interpValStatus.SetStatusNA();
    }
}

void DPO_InterpTableColumnPS::MenuOpen(DPO_InterpTableColumn& inData)
{
    MenuBaseC::MenuOpen(false, false);
    tableColSelUI.MenuOpen(inData);
    interpXValMSUI.MenuInit(inData.interpXValMS);
}


bool DPO_InterpTableColumnPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    tableColSelUI.UIdataOK(allOK);
    interpXValMSUI.UIdataOK(allOK);
    return allOK;
}

void DPO_InterpTableColumnPS::GetMenuData(DPO_InterpTableColumn&  outData)
{
    MenuBaseC::GetMenuData(outData);
    tableColSelUI.GetTableColSel(outData);
    outData.selectedYCol = selectedYColUI.GetSelection();
    interpXValMSUI.GetValue(outData.interpXValMS);
    outData.logInterpX = logInterpXUI.GetValue();
    outData.logInterpY = logInterpYUI.GetValue();
}

