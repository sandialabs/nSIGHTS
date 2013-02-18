///////////////////////////////////////////////////////////////////////////////////
//
// PPO_VariableArrayGridLinesPS.cpp
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <objClass/DO_VariableArray.h>

#include "PPO_VariableArrayGridLinesPS.h"

// ****************************************

PPO_VariableArrayGridLinesPS::PPO_VariableArrayGridLinesPS() :
    variableArrayObjRefUI(typeid(DO_VariableArray)),
    labelFontUI(true)
{
}

void PPO_VariableArrayGridLinesPS::SetAllSensitive()
{
    labelFormatUI.SetSensitive(true);
    int labType = labelFormatUI.labelTypeUI.GetChoice();
    labelFontUI.SetSensitive(labType  > 0);
    valueFormatUI.SetSensitive(labType == 2);
}

void PPO_VariableArrayGridLinesPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_VariableArrayGridLinesPS*>(inObj)->SetAllSensitive();
}


void PPO_VariableArrayGridLinesPS::InitMenu(const PPO_VariableArrayGridLines& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    variableArrayObjRefUI.InitFramedUI("Variable Array Input", PPform.GetWidget(), args);

    args.StdTopObjectAttach(variableArrayObjRefUI);
    gridLineSpecUI.InitFramedUI("Grid Line Format", PPform.GetWidget(), args);

    args.StdTopObjectAttach(gridLineSpecUI);
    labelFormatUI.InitFramedUI("Label Format", PPform.GetWidget(), args);
    labelFormatUI.labelTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(labelFormatUI);
    labelFontUI.InitFramedUI("Label Font", PPform.GetWidget(), args);

    args.StdTopObjectAttach(labelFontUI);
    bottomRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    arrayIsXUI.PreInitSetUp("Y Grid", "X Grid");
    arrayIsXUI.InitFramedUI("Grid Line Orientation", bottomRow.GetWidget(), args);

    args.StdSplitFinish();
    valueFormatUI.InitFramedUI("Value Format",  bottomRow.GetWidget(), args);
}


void PPO_VariableArrayGridLinesPS::MenuOpen(PPO_VariableArrayGridLines&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    variableArrayObjRefUI.StdMenuInit(inData.variableArrayObjRef);
    gridLineSpecUI.ResetPens(inData.objPenSet);
}

void PPO_VariableArrayGridLinesPS::SetMenuData(PPO_VariableArrayGridLines&    inData)
{
    PPO_BasePS::SetMenuData(inData);

    variableArrayObjRefUI.SetFuncObjRef(inData.variableArrayObjRef);
    gridLineSpecUI.SetLineSpec(inData.gridLineSpec);

    labelFormatUI.SetGridLineLabelFormat(inData.labelFormat);

    labelFontUI.SetFontSettings(inData.labelFont);

    arrayIsXUI.SetBoolChoice(inData.arrayIsX);
    valueFormatUI.SetRealFormatSettings(inData.valueFormat);

    SetAllSensitive();
}

bool PPO_VariableArrayGridLinesPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    variableArrayObjRefUI.FuncObjRefOK(allOK, "Variable array source");
    labelFormatUI.UIdataOK(allOK);
    return allOK;
}

void PPO_VariableArrayGridLinesPS::GetMenuData(PPO_VariableArrayGridLines&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.variableArrayObjRef = variableArrayObjRefUI.GetFuncObjRef();
    gridLineSpecUI.GetLineSpec(outData.gridLineSpec);
    labelFormatUI.GetGridLineLabelFormat(outData.labelFormat);

    outData.arrayIsX = arrayIsXUI.GetBoolChoice();

    if (outData.labelFormat.labelType != PSC_GridLineLabelFormat::ltNone)
    {
        labelFontUI.GetFontSettings(outData.labelFont);
    }

    if (outData.labelFormat.labelType == PSC_GridLineLabelFormat::ltValue)
        valueFormatUI.GetRealFormatSettings(outData.valueFormat);

}


