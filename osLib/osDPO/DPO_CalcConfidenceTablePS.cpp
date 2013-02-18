///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CalcConfidenceTablePS.cpp
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

#include <genClass/U_Str.h>
#include <genClassPS/PS_UIBase.h>

#include "DPO_CalcConfidenceTable.h"
#include "DPO_CalcConfidenceTablePS.h"

DPO_CalcConfidenceTablePS::DPO_CalcConfidenceTablePS()
{
}

void DPO_CalcConfidenceTablePS::SetAllSensitive()
{
    bestCaseIndexUI.SetSensitive(!autoBestCaseUI.GetValue());
}


void DPO_CalcConfidenceTablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_CalcConfidenceTablePS*>(inObj)->SetAllSensitive();
}

void DPO_CalcConfidenceTablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    tableColSelUI.ObjRefInit(URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    tableColSelUI.selectedColUI.InitFramedUI("SSE Data Column", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    bestCaseIndexUI.InitFramedUI("Best Case Row Index", topRowForm.GetWidget(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    npForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    nDataPtsUI.InitFramedUI("# of Data Points", npForm.GetWidget(), args);

    args.StdSplitFinish();
    nParamUI.InitFramedUI("# of Parameters", npForm.GetWidget(), args);

    args.StdTopObjectAttach(npForm.GetWidget());
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdSplitSetup();
    args.StdToggleSetup(5);
    autoBestCaseUI.StdToggleInit("automatically determine best fit case", optionFrame.GetForm(), args);
    autoBestCaseUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    static const char* distrStrings[] = {"F-test Method", "Log-Likelihood Method"};
    distributionUI.PreInitSetUp(distrStrings, 2);
    distributionUI.InitWidget(optionFrame.GetForm(), args);

}

void DPO_CalcConfidenceTablePS::MenuOpen(DPO_CalcConfidenceTable& inData)
{
    MenuBaseC::MenuOpen(false, false);

    tableColSelUI.MenuOpen(inData);
}


void DPO_CalcConfidenceTablePS::SetMenuData(DPO_CalcConfidenceTable&  inData)
{
    MenuBaseC::SetMenuData(inData);

    tableColSelUI.SetTableColSel(inData);
    bestCaseIndexUI.SetFieldValue(inData.bestCaseIndex);
    nDataPtsUI.SetFieldValue(inData.nDataPts);
    nParamUI.SetFieldValue(inData.nParam);
    autoBestCaseUI.SetValue(inData.autoBestCase);
    distributionUI.SetChoice(int(inData.distribution));

    SetAllSensitive();
}

bool DPO_CalcConfidenceTablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    tableColSelUI.UIdataOK(allOK);
    bestCaseIndexUI.FieldValueOK(allOK, "best case row index", 0, 1000000);
    nDataPtsUI.FieldValueOK(allOK, "# data pts", 1, 1000000);
    nParamUI.FieldValueOK(allOK, "# parameters", 1, 1000000);
    return allOK;
}

void DPO_CalcConfidenceTablePS::GetMenuData(DPO_CalcConfidenceTable&  outData)
{
    MenuBaseC::GetMenuData(outData);
    tableColSelUI.GetTableColSel(outData);

    outData.autoBestCase = autoBestCaseUI.GetValue();
    outData.distribution = DFO_CalcConfidenceTable::Distribution(distributionUI.GetChoice());

    outData.nDataPts = nDataPtsUI.GetFieldValue();
    outData.nParam = nParamUI.GetFieldValue();
    outData.bestCaseIndex = bestCaseIndexUI.GetFieldValue();

}

