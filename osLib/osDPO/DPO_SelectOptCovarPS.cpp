///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectOptCovarPS.cpp
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

#include "DPO_SelectOptCovarPS.h"

DPO_SelectOptCovarPS::DPO_SelectOptCovarPS()
{
    showOKonly = true;
}

void DPO_SelectOptCovarPS::ToggleSubFits()
{
    showFitComponents = includeSubFitsUI.GetValue();
    indexMSUI.indexListUI.ClearSelections();
    DPO_SelectOptPS::SetIndexList();
}

void DPO_SelectOptCovarPS::ToggleSubFitsCB(void* inObj)
{
    static_cast<DPO_SelectOptCovarPS*>(inObj)->ToggleSubFits();
}

void DPO_SelectOptCovarPS::SetAllSensitive()
{
    selectActualCovarUI.SetSensitive(!includeSubFitsUI.GetValue());
    DPO_SelectOptPS::SetAllSensitive();
}

void DPO_SelectOptCovarPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    includeSubFitsUI.StdToggleInit("Fit components", optionsFrame.GetForm(), args);
    includeSubFitsUI.AddOtherObjectCallback(ToggleSubFitsCB, this);

    args.StdSplitMiddle(40, 80);
    selectActualCovarUI.PreInitSetUp("Estimated", "Actual", true);
    selectActualCovarUI.InitWidget(optionsFrame.GetForm(), args);

    args.Clear();
    args.TopUICAttach(optionsFrame);
    DPO_SelectOptPS::InitMenu(args, true, true);
}

void DPO_SelectOptCovarPS::MenuOpen(DPO_SelectOptCovar& inData)
{
    DPO_SelectOptPS::MenuOpen(inData);
}


void DPO_SelectOptCovarPS::SetMenuData(DPO_SelectOptCovar&  inData)
{
    selectActualCovarUI.SetBoolChoice(inData.selectActualCovar);
    includeSubFitsUI.SetValue(inData.includeSubFits);

    DPO_SelectOptPS::SetMenuData(inData);
}

bool DPO_SelectOptCovarPS::UIdataOK()
{
    DPO_SelectOptPS::UIdataOK();
    return allOK;
}

void DPO_SelectOptCovarPS::GetMenuData(DPO_SelectOptCovar&  outData)
{
    DPO_SelectOptPS::GetMenuData(outData);

    outData.selectActualCovar = selectActualCovarUI.GetBoolChoice();
    outData.includeSubFits = showFitComponents;
}

