///////////////////////////////////////////////////////////////////////////////////
//
// PPO_LabelAxesPS.cpp
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

#include <genClass/DO_TableData.h>

#include "PPO_LabelAxesPS.h"

// ****************************************

PPO_LabelAxesPS::PPO_LabelAxesPS()
: labelArrayObjRefUI(typeid(DO_LabelArray)),
tableObjRefUI(typeid(DO_TableData))
{
}

void PPO_LabelAxesPS::SetAllSensitive()
{
    maxTxtLenUI.SetSensitive(limTxtLenUI.GetValue());
    labelArrayObjRefUI.SetSensitive(labelSourceUI.GetChoice() == 0);
    tableObjRefUI.SetSensitive(labelSourceUI.GetChoice() > 0);
    skipCol0UI.SetSensitive(labelSourceUI.GetChoice() == 1);
}

void PPO_LabelAxesPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_LabelAxesPS*>(inObj)->SetAllSensitive();
}

void PPO_LabelAxesPS::InitMenu(const PPO_LabelAxes& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();

    const char* lsStrs[] = {" Label Array ", " Table Column IDs ", " Table Row IDs "};
    labelSourceUI.PreInitSetUp(lsStrs, 3);
    labelSourceUI.InitFramedUI("Label source", PPform.GetWidget(), args);
    labelSourceUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(labelSourceUI);
    objRefRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    labelArrayObjRefUI.InitFramedUI("Label Array", objRefRow.GetWidget(), args);

    args.StdSplitFinish();
    tableObjRefUI.InitFramedUI("Table", objRefRow.GetWidget(), args);

    args.StdTopObjectAttach(objRefRow.GetWidget());
    secondRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(25);
    isXAxisUI.PreInitSetUp("Y Axis", "X Axis");
    isXAxisUI.InitFramedUI("Axis To Label", secondRow.GetWidget(), args);

    args.StdSplitMiddle(26, 74);
    lengthFrame.InitFramedUI("Increment Label Length", secondRow.GetWidget(), args);

    args.StdToggleSetup(5);
    limTxtLenUI.StdToggleInit("Limit?", lengthFrame.GetForm(), args);
    limTxtLenUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(75);
    maxTxtLenUI.SetFieldLen(4);
    maxTxtLenUI.AttachLabelToObject();
    maxTxtLenUI.InitLabeledUI("Max text length", lengthFrame.GetForm(), args);

    args.StdSplitFinish(25);
    optionFrame.InitFramedUI("Options", secondRow.GetWidget(), args);

    args.StdToggleSetup(5);
    skipCol0UI.StdToggleInit("Skip column 1", optionFrame.GetForm(), args);

    args.StdTopObjectAttach(secondRow.GetWidget());
    axesLabelUI.SetFieldLen(0);
    axesLabelUI.InitFramedUI("Axes Label", PPform.GetWidget(), args);
}


void PPO_LabelAxesPS::MenuOpen(PPO_LabelAxes&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    labelArrayObjRefUI.StdMenuInit(inData.labelArrayObjRef);
    tableObjRefUI.StdMenuInit(inData.tableObjRef);
}

void PPO_LabelAxesPS::SetMenuData(PPO_LabelAxes&    inData)
{
    PPO_BasePS::SetMenuData(inData);

    labelSourceUI.SetChoice(inData.labelSource);

    labelArrayObjRefUI.SetFuncObjRef(inData.labelArrayObjRef);
    tableObjRefUI.SetFuncObjRef(inData.tableObjRef);

    isXAxisUI.SetBoolChoice(inData.isXAxis);

    limTxtLenUI.SetValue(inData.limTxtLen);
    maxTxtLenUI.SetFieldValue(inData.maxTxtLen);

    skipCol0UI.SetValue(inData.skipCol0);

    axesLabelUI.SetFieldString(inData.axesLabel);

    SetAllSensitive();
}

bool PPO_LabelAxesPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();

    if (labelSourceUI.GetChoice() == 0)
        labelArrayObjRefUI.FuncObjRefOK(allOK, "Label array source");
    else
        tableObjRefUI.FuncObjRefOK(allOK, "Table source");

    if (limTxtLenUI.GetValue())
        maxTxtLenUI.FieldValueOK(allOK, "Max incr. length", 0, 60);

    return allOK;
}

void PPO_LabelAxesPS::GetMenuData(PPO_LabelAxes&  outData)
{
    PPO_BasePS::GetMenuData(outData);
    outData.labelSource = PPO_LabelAxes::LabelSource(labelSourceUI.GetChoice());

    if (outData.labelSource == PPO_LabelAxes::lsLabelArray)
        outData.labelArrayObjRef = labelArrayObjRefUI.GetFuncObjRef();
    else
        outData.tableObjRef = tableObjRefUI.GetFuncObjRef();

    outData.isXAxis = isXAxisUI.GetBoolChoice();

    outData.limTxtLen = limTxtLenUI.GetValue();
    if (outData.limTxtLen)
        outData.maxTxtLen = maxTxtLenUI.GetFieldValue();
    if (outData.labelSource == PPO_LabelAxes::lsTableCol)
        outData.skipCol0 = skipCol0UI.GetValue();

    axesLabelUI.GetFieldString(outData.axesLabel, PFO_LabelAxes::maxLabelLen);
}

