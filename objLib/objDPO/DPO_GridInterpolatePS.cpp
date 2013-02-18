///////////////////////////////////////////////////////////////////////////////////
//
// DPO_GridInterpolatePS.cpp
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

#include <genClassPS/PS_UIBase.h>

#include "DPO_GridInterpolatePS.h"

DPO_GridInterpolatePS::DPO_GridInterpolatePS()
{
}

void DPO_GridInterpolatePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    inputAUI.gridInputObjRefUI.InitFramedUI("Data to Interpolate From", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    inputBUI.gridInputObjRefUI.InitFramedUI("Interpolant Grid", topRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    logInterpUI.StdToggleInit("log Data", optionsFrame.GetForm(), args);

}

void DPO_GridInterpolatePS::MenuOpen(DPO_GridInterpolate& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputAUI.MenuOpen(inData.inputA);
    inputBUI.MenuOpen(inData.inputB);
}


void DPO_GridInterpolatePS::SetMenuData(DPO_GridInterpolate&  inData)
{
    inputAUI.SetGridInput(inData.inputA);
    inputBUI.SetGridInput(inData.inputB);

    logInterpUI.SetValue(inData.logInterp);

    MenuBaseC::SetMenuData(inData);
}

bool DPO_GridInterpolatePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputAUI.UIdataOK(allOK);
    inputBUI.UIdataOK(allOK);
    return allOK;
}

void DPO_GridInterpolatePS::GetMenuData(DPO_GridInterpolate&  outData)
{
    MenuBaseC::GetMenuData(outData);

    inputAUI.GetGridInput(outData.inputA);
    inputBUI.GetGridInput(outData.inputB);

    outData.logInterp = logInterpUI.GetValue();
}

