///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableFullCorrelationPS.cpp
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

#include "DPO_TableFullCorrelation.h"
#include "DPO_TableFullCorrelationPS.h"


DPO_TableFullCorrelationPS::DPO_TableFullCorrelationPS()
    : inputTableObjRefUI(typeid(DO_TableData))
{
}


void DPO_TableFullCorrelationPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Input Table Data", URform.GetWidget(), args);


    args.Clear();
    args.StdFormAttach();
    static const char* corrLabels[] = {"Pearson Linear Correlation", "Spearman Rank-Order", "Other"};
    correlationTypeUI.PreInitSetUp(corrLabels, 3);
    correlationTypeUI.InitFramedUI("Correlation Type", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(correlationTypeUI);
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    logDataUI.StdToggleInit("log Data", optionsFrame.GetForm(), args);
}

void DPO_TableFullCorrelationPS::SetMenuData(DPO_TableFullCorrelation&  inData)
{
    MenuBaseC::SetMenuData(inData);
    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);
    correlationTypeUI.SetChoice(int(inData.correlationType));
    logDataUI.SetValue(inData.logData);
}


void DPO_TableFullCorrelationPS::MenuOpen(DPO_TableFullCorrelation& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
}


bool DPO_TableFullCorrelationPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableObjRefUI.FuncObjRefOK(allOK, "Input table");
    return allOK;
}

void DPO_TableFullCorrelationPS::GetMenuData(DPO_TableFullCorrelation&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();
    outData.correlationType = DFO_TableFullCorrelation::CorrelationType(correlationTypeUI.GetChoice());
    outData.logData = logDataUI.GetValue();
}

