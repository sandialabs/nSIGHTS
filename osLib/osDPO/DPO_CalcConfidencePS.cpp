///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CalcConfidencePS.cpp
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

#include "DPO_CalcConfidence.h"
#include "DPO_CalcConfidencePS.h"

DPO_CalcConfidencePS::DPO_CalcConfidencePS() :
    optSimResultsObjRefUI(typeid(DO_OptSimResults))
{
}

void DPO_CalcConfidencePS::SetIndexList()
{
    SC_StringArray runIDs, fitIDs, caseIDs;
    bool autoBF = autoBestCaseUI.GetValue();
    if (optSimResultsObjRefUI.CheckDataObj())
    {
        DC_OptSimResults*  dataDC = static_cast<DO_OptSimResults*>(optSimResultsObjRefUI.GetDataObj())->optResults;

        dataDC->GetRunIDs(runIDs);

        int currRunIndx = runIndexUI.GetSingleSelection();

        if (currRunIndx > dataDC->UpperBound())
        {
            runIndexUI.SetSingleSelection(0);
            currRunIndx = 0;
        }

        OptSimRunResults& currRun = (*dataDC)[currRunIndx];
        currRun.GetFitIDs(fitIDs);

        static const int caseStrLen = CaseVarDescription::caseVarDescLen;
        OptCaseResultsArray& currCases = (*dataDC)[currRunIndx].runResults;
        int numCases = currCases.Size();
        caseIDs.AllocAndSetSize(numCases);
        for (int j = 0; j < numCases; j++)
        CopyString(caseIDs[j], currCases[j].caseDesc, caseStrLen);

    }

    listDataOK = !runIDs.IsEmpty() && !fitIDs.IsEmpty() && !caseIDs.IsEmpty();
    if (listDataOK)
    {
        runIndexUI.SetListText(runIDs);
        fitIndexUI.SetListText(fitIDs);
        bestCaseIndexUI.SetListText(caseIDs);
    }
    else
    {
        runIndexUI.SetListNA();
        fitIndexUI.SetListNA();
        bestCaseIndexUI.SetListNA();
    }

    SetAllSensitive();
}

void DPO_CalcConfidencePS::SetIndexListCB(void* inObj)
{
    static_cast<DPO_CalcConfidencePS*>(inObj)->SetIndexList();
}

void DPO_CalcConfidencePS::SetAllSensitive()
{
    bool autoBF = autoBestCaseUI.GetValue();
    bestCaseFrame.SetSensitive(!autoBF);
    bestCaseIndexUI.SetSensitive(!autoBF);
}


void DPO_CalcConfidencePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_CalcConfidencePS*>(inObj)->SetAllSensitive();
}

void DPO_CalcConfidencePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    optSimResultsObjRefUI.InitFramedUI("nSIGHTS Opt Results To Select From", URform.GetWidget(), args);
    optSimResultsObjRefUI.AddOtherObjectCallback(SetIndexListCB, this);

    args.StdFormAttach();
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdSplitSetup();
    args.StdToggleSetup(5);
    autoBestCaseUI.StdToggleInit("automatically determine best fit case", optionFrame.GetForm(), args);
    autoBestCaseUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    static const char* distrStrings[] = {"F-test Method", "Log-Likelihood Method"};
    distributionUI.PreInitSetUp(distrStrings, 2);
    distributionUI.InitWidget(optionFrame.GetForm(), args);

    args.StdTopObjectAttach(optionFrame);
    runFrame.InitFramedUI("Select Run", PPform.GetWidget(), args);

    args.AllFormAttach();
    runIndexUI.InitWidget(runFrame.GetForm(), args);
    runIndexUI.AddOtherObjectCallback(SetIndexListCB, this);

    args.StdTopObjectAttach(runFrame);
    fitFrame.InitFramedUI("Select Fit", PPform.GetWidget(), args);

    args.AllFormAttach();
    fitIndexUI.InitWidget(fitFrame.GetForm(), args);

    args.StdTopObjectAttach(fitFrame);
    bestCaseFrame.InitFramedUI("Select Best Fit Case", PPform.GetWidget(), args);

    args.AllFormAttach();
    args.TopOffset(2);
    bestCaseIndexUI.InitWidget(bestCaseFrame.GetForm(), args);

}

void DPO_CalcConfidencePS::MenuOpen(DPO_CalcConfidence& inData)
{
    MenuBaseC::MenuOpen(false, false);

    optSimResultsObjRefUI.StdMenuInit(inData.optSimResultsObjRef);
}


void DPO_CalcConfidencePS::SetMenuData(DPO_CalcConfidence&  inData)
{
    MenuBaseC::SetMenuData(inData);


    optSimResultsObjRefUI.SetFuncObjRef(inData.optSimResultsObjRef);
    runIndexUI.SetSingleSelection(inData.runIndex);
    fitIndexUI.SetSingleSelection(inData.fitIndex);
    bestCaseIndexUI.SetSingleSelection(inData.bestCaseIndex);
    autoBestCaseUI.SetValue(inData.autoBestCase);
    distributionUI.SetChoice(int(inData.distribution));

    SetIndexList();
}

bool DPO_CalcConfidencePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    optSimResultsObjRefUI.FuncObjRefOK(allOK, "Opt input");
    return allOK;
}

void DPO_CalcConfidencePS::GetMenuData(DPO_CalcConfidence&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.optSimResultsObjRef = optSimResultsObjRefUI.GetFuncObjRef();

    outData.autoBestCase = autoBestCaseUI.GetValue();
    outData.distribution = DFO_CalcConfidence::Distribution(distributionUI.GetChoice());

    if (listDataOK)
    {
        outData.runIndex = runIndexUI.GetSingleSelection();
        outData.fitIndex = fitIndexUI.GetSingleSelection();
        if (!outData.autoBestCase)
            outData.bestCaseIndex = bestCaseIndexUI.GetSingleSelection();
    }
}

