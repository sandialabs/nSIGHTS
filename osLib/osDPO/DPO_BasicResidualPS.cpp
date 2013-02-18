///////////////////////////////////////////////////////////////////////////////////
//
// DPO_BasicResidualPS.cpp
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

#include "DPO_BasicResidualPS.h"

DPO_BasicResidualPS::DPO_BasicResidualPS() :
    residInputObjRefUI(typeid(DO_ResidData))
{
}

void DPO_BasicResidualPS::SetAllSensitive()
{
    xoutputIsIndexUI.SetSensitive(sortingSpecUI.GetChoice() == 0);
}


void DPO_BasicResidualPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_BasicResidualPS*>(inObj)->SetAllSensitive();
}


void DPO_BasicResidualPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    residInputObjRefUI.InitFramedUI("Residual Data", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(50);

    static const char* sortStrings[] = {" None ", " Up " , " Down "};
    sortingSpecUI.PreInitSetUp(sortStrings, 3);
    sortingSpecUI.InitFramedUI("Sorting", topRow.GetWidget(), args);
    sortingSpecUI.AddOtherObjectCallback(SetAllSensitiveCB, this);


    args.StdSplitFinish(50);
    xoutputIsIndexUI.PreInitSetUp("X Value", "Index");
    xoutputIsIndexUI.InitFramedUI("X Output", topRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRow.GetWidget());
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    standardizeUI.StdToggleInit("Standardize", optionsFrame.GetForm(), args);
}

void DPO_BasicResidualPS::MenuOpen(DPO_BasicResidual& inData)
{
    MenuBaseC::MenuOpen(false, false);
    residInputObjRefUI.StdMenuInit(inData.residInputObjRef);
}


void DPO_BasicResidualPS::SetMenuData(DPO_BasicResidual&  inData)
{
    residInputObjRefUI.SetFuncObjRef(inData.residInputObjRef);

    sortingSpecUI.SetChoice(int(inData.sortingSpec));
    xoutputIsIndexUI.SetBoolChoice(inData.xoutputIsIndex);
    standardizeUI.SetValue(inData.standardize);

    MenuBaseC::SetMenuData(inData);
}

bool  DPO_BasicResidualPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    residInputObjRefUI.FuncObjRefOK(allOK, "Residual data source");
    return allOK;
}

void DPO_BasicResidualPS::GetMenuData(DPO_BasicResidual&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.residInputObjRef = residInputObjRefUI.GetFuncObjRef();

    outData.sortingSpec = DPO_BasicResidual::SortingSpec(sortingSpecUI.GetChoice());
    outData.xoutputIsIndex = xoutputIsIndexUI.GetBoolChoice();
    outData.standardize = standardizeUI.GetValue();

}

