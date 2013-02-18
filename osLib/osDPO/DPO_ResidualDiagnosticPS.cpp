///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ResidualDiagnosticPS.cpp
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
//      I/O and user interface for all osDFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include "DPO_ResidualDiagnosticPS.h"

DPO_ResidualDiagnosticPS::DPO_ResidualDiagnosticPS() :
    residInputObjRefUI(typeid(DO_ResidData))
{
}

void DPO_ResidualDiagnosticPS::SetAllSensitive()
{
    nnormalPointsUI.SetSensitive(diagnosticTypeUI.GetChoice() != 0);
}


void DPO_ResidualDiagnosticPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ResidualDiagnosticPS*>(inObj)->SetAllSensitive();
}


void DPO_ResidualDiagnosticPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    residInputObjRefUI.InitFramedUI("Residual Data", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(50);

    static const char* diagStrings[] = {"Quantile-Normal", "Standard "};
    diagnosticTypeUI.PreInitSetUp(diagStrings, 2);
    diagnosticTypeUI.InitFramedUI("Diagnostic Type", topRow.GetWidget(), args);
    diagnosticTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(50);
    nnormalPointsUI.InitFramedUI("Points in Normal Distribution", topRow.GetWidget(), args);
}

void DPO_ResidualDiagnosticPS::MenuOpen(DPO_ResidualDiagnostic& inData)
{
    MenuBaseC::MenuOpen(false, false);
    residInputObjRefUI.StdMenuInit(inData.residInputObjRef);
}


void DPO_ResidualDiagnosticPS::SetMenuData(DPO_ResidualDiagnostic&  inData)
{
    residInputObjRefUI.SetFuncObjRef(inData.residInputObjRef);

    diagnosticTypeUI.SetChoice(int(inData.diagnosticType));
    nnormalPointsUI.SetFieldValue(inData.nnormalPoints);

    MenuBaseC::SetMenuData(inData);
}

bool  DPO_ResidualDiagnosticPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    residInputObjRefUI.FuncObjRefOK(allOK, "Residual data source");
    if (diagnosticTypeUI.GetChoice() != 0)
        nnormalPointsUI.FieldValueOK(allOK, "# of normal points", 10, 1000);

    return allOK;
}

void DPO_ResidualDiagnosticPS::GetMenuData(DPO_ResidualDiagnostic&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.residInputObjRef = residInputObjRefUI.GetFuncObjRef();

    outData.diagnosticType = DPO_ResidualDiagnostic::DiagnosticType(diagnosticTypeUI.GetChoice());

    if (diagnosticTypeUI.GetChoice() != 0)
        outData.nnormalPoints = nnormalPointsUI.GetFieldValue();
}

