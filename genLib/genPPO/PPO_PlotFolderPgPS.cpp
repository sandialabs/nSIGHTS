///////////////////////////////////////////////////////////////////////////////////
//
// PPO_PlotFolderPgPS.cpp
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genApp/U_Menu.h>
#include <genApp/C_MenuPage.h>
#include <genApp/C_PlotPage.h>
#include <genApp/C_PlotFolderPage.h>
#include <genPlotClass/C_PlotObjBase.h>

#include "PPO_PlotFolderPgPS.h"


void PPO_PlotFolderPgPS::SetObjectsOnPage(MenuPageC& currPage,
                                         bool       turnOn)
{
    if (!currPage.IsSameType(PlotFolderPageAlloc::PageTypeName()))
        GenAppInternalError("PPO_PlotFolderPgPS::SetObjectsOnPage");

    for (int i = 1; i < currPage.pageObjects.Size(); i++)
        static_cast<PlotObjBaseC*>(currPage.pageObjects[i]->objFunction)->doPlot = turnOn;


    for (int i = 0; i < currPage.childPages.Size(); i++)
        SetObjectsOnPage(*currPage.childPages[i], turnOn);
}

void PPO_PlotFolderPgPS::TurnObjects(bool turnOn)
{
    SetObjectsOnPage(*MenuPageC::currPageObj, turnOn);
    static_cast<const PlotPageC*>(MenuPageC::currPageObj->GetActualParent())->GetPlot()->GetMasterPlot().RedrawPlot();
}

void PPO_PlotFolderPgPS::TurnObjectsOnCB(void* inObj)
{
    static_cast<PPO_PlotFolderPgPS*>(inObj)->TurnObjects(true);
}

void PPO_PlotFolderPgPS::TurnObjectsOffCB(void* inObj)
{
    static_cast<PPO_PlotFolderPgPS*>(inObj)->TurnObjects(false);
}

void PPO_PlotFolderPgPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    buttonFrame.InitFramedUI("Plot Folder Objects Control", PPform.GetWidget(), args);

    args.StdSplitSetup();
    args.StdSplitMiddle(10, 40);
    turnObjectsOff.StdButtonInit("All Objects Off", buttonFrame.GetForm(), args);
    turnObjectsOff.AddOtherObjectCallback(TurnObjectsOffCB, this);

    args.StdSplitMiddle(60, 90);
    turnObjectsOn.StdButtonInit("All Objects On", buttonFrame.GetForm(), args);
    turnObjectsOn.AddOtherObjectCallback(TurnObjectsOnCB, this);
}


void PPO_PlotFolderPgPS::SetMenuData(PPO_PlotFolderPg&  inData)
{
    MenuBaseC::SetMenuData(inData);
}


void PPO_PlotFolderPgPS::GetMenuData(PPO_PlotFolderPg&  outData)
{
    MenuBaseC::GetMenuData(outData);
}



