///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectOptResultsPS.cpp
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

#include <genClassPS/PS_UIBase.h>

#include "DPO_SelectOptResultsPS.h"

DPO_SelectOptResultsPS::DPO_SelectOptResultsPS()
{
}

void DPO_SelectOptResultsPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    resultsFrame.InitFramedUI("Basic Table Columns", topRow.GetWidget(), args);

    args.StdToggleSetup(5);
    includeOptResultsUI.StdToggleInit("Optimized values", resultsFrame.GetForm(), args);

    args.StdToggleSetup(40);
    includeCaseParametersUI.StdToggleInit("Case par", resultsFrame.GetForm(), args);

    args.StdToggleSetup(75);
    includeOptStatusUI.StdToggleInit("Opt status", resultsFrame.GetForm(), args);

    args.StdSplitFinish(35);

    const char* startEndStrings[] = {"None",
        "Regular - Segment Table", "Normalized - Segment Table",
        "Regular - Points Table", "Normalized - PointsTable"};
    startEndResultsUI.PreInitSetUp(startEndStrings, 5);
    startEndResultsUI.InitFramedUI("Start/End Table Output", topRow.GetWidget(), args);

    args.StdTopObjectAttach(topRow.GetWidget());
    DPO_SelectOptPS::InitMenu(args, false, true);
}

void DPO_SelectOptResultsPS::MenuOpen(DPO_SelectOptResults& inData)
{
    DPO_SelectOptPS::MenuOpen(inData);
}


void DPO_SelectOptResultsPS::SetMenuData(DPO_SelectOptResults&  inData)
{
    DPO_SelectOptPS::SetMenuData(inData);

    includeOptResultsUI.SetValue(inData.includeOptResults);
    includeCaseParametersUI.SetValue(inData.includeCaseParameters);
    includeOptStatusUI.SetValue(inData.includeOptStatus);
    startEndResultsUI.SetChoice(inData.startEndResults);
}

bool DPO_SelectOptResultsPS::UIdataOK()
{
    DPO_SelectOptPS::UIdataOK();
    if ( allOK && ! (includeOptResultsUI.GetValue() || includeCaseParametersUI.GetValue() || includeOptStatusUI.GetValue()))
    {
        PropErrorMsg("at least one table columns must be set");
        allOK = false;
    }
    return allOK;
}

void DPO_SelectOptResultsPS::GetMenuData(DPO_SelectOptResults&  outData)
{
    DPO_SelectOptPS::GetMenuData(outData);
    outData.includeOptResults = includeOptResultsUI.GetValue();
    outData.includeCaseParameters = includeCaseParametersUI.GetValue();
    outData.includeOptStatus = includeOptStatusUI.GetValue();
    outData.startEndResults = DFO_SelectOptResults::StartEndResults(startEndResultsUI.GetChoice());
}

