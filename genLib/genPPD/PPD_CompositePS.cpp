///////////////////////////////////////////////////////////////////////////////////
//
// PPD_CompositePS.cpp
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
//      adds components for composite plots.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "PPD_CompositePS.h"



void PPD_CompositePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPD_CompositePS*>(inObj)->SetAllSensitive();
}

void PPD_CompositePS::SetAllSensitive()
{
    layoutPenUI.SetSensitive(showLayoutUI.GetValue());
}

void PPD_CompositePS::ResetPen()
{
    PPD_BasePS::ResetPen();
    if (penSource.CheckDataObj())
    {
        DO_PenSet* penDO = static_cast<DO_PenSet*>(penSource.GetDataObj());
        layoutPenUI.ResetPens(penDO->penSetData);
    }
}


void PPD_CompositePS::InitMenu()
{
    PPD_BasePS::InitMenu();

    ArgListC args;

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.LeftFormAttach();
    args.RightUICAttach(marginFrame);
    autoLayoutUI.PreInitSetUp("Free-Form", "Rows/Column", false);
    autoLayoutUI.InitFramedUI("Layout Format", lastRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(lastRowForm.GetWidget());
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    showLayoutUI.StdToggleInit("Show layout", optionFrame.GetForm(), args);
    showLayoutUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitSetup();
    args.StdSplitMiddle(30, 70);
    layoutPenUI.InitLabeledUI("Layout pen", optionFrame.GetForm(), args);

    SetMainformScrolled(true, 520, 220);
}

void PPD_CompositePS::MenuOpen(PPD_Composite&   inData)
{
    PPD_BasePS::MenuOpen(inData);
}


void PPD_CompositePS::SetMenuData(PPD_Composite&    inData)
{
    PPD_BasePS::SetMenuData(inData);
    autoLayoutUI.SetBoolChoice(inData.autoLayout);

    showLayoutUI.SetValue(inData.showLayout);
    layoutPenUI.SetPenSelection(inData.layoutPen);
    SetAllSensitive();
}

bool PPD_CompositePS::UIdataOK()
{
    PPD_BasePS::UIdataOK();
    return allOK;
}

void PPD_CompositePS::GetMenuData(PPD_Composite& outData)
{
    PPD_BasePS::GetMenuData(outData);
    outData.autoLayout = autoLayoutUI.GetBoolChoice();

    outData.showLayout = showLayoutUI.GetValue();
    outData.layoutPen = layoutPenUI.GetPenSelection();
}

void  PPD_CompositePS::MenuDefault()
{
    PPD_BasePS::MenuDefault();
    autoLayoutUI.SetBoolChoice(true);
}

