///////////////////////////////////////////////////////////////////////////////////
//
// PPO_EnterXYOnPlotPS.cpp
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

#include <genClass/DO_XYData.h>
#include <genPlotClass/C_PlotDef.h>

#include "PPO_EnterXYOnPlotPS.h"

void PPO_EnterXYOnPlotPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_EnterXYOnPlotPS*>(inObj)->SetAllSensitive();
}


void PPO_EnterXYOnPlotPS::SetAllSensitive()
{
    currentPtSymbolUI.SetSensitive(true);
    enteredPtSymbolUI.SetSensitive(true);
    deletedPtSymbolUI.SetSensitive(true);
}


void PPO_EnterXYOnPlotPS::UpdateFromObjCB(void* inObj)
{
    static_cast<PPO_EnterXYOnPlotPS*>(inObj)->UpdateFromObj();
}

void PPO_EnterXYOnPlotPS::UpdateFromObj()
{
    if (inputXYObjRefUI.CheckDataObj())
    {
        const DC_XYData& newDC = *(static_cast<DO_XYData*>(inputXYObjRefUI.GetDataObj())->xyData);
        currObj->UpdateFrom(newDC);
        currObj->inputXYObjRef = inputXYObjRefUI.GetFuncObjRef();
    }
}


PPO_EnterXYOnPlotPS::PPO_EnterXYOnPlotPS() :
    inputXYObjRefUI(typeid(DO_XYData))
{
}

void PPO_EnterXYOnPlotPS::InitMenu(const PPO_EnterXYOnPlot&   inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC  args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(80);
    inputXYObjRefUI.InitFramedUI("Input XY Data", topRowForm.GetWidget(), args);
    inputXYObjRefUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.RightFormAttach(5);
    args.BottomFormAttach(5);
    updateFromInput.StdButtonInit("Update", topRowForm.GetWidget(), args);
    updateFromInput.AddOtherObjectCallback(UpdateFromObjCB, this);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    currentPtSymbolUI.InitFramedUI("Points to Keep", PPform.GetWidget(), args);

    args.StdTopObjectAttach(currentPtSymbolUI);
    enteredPtSymbolUI.InitFramedUI("Entered Points", PPform.GetWidget(), args);

    args.StdTopObjectAttach(enteredPtSymbolUI);
    deletedPtSymbolUI.InitFramedUI("Points to Delete", PPform.GetWidget(), args);

    args.StdTopObjectAttach(deletedPtSymbolUI);
    currLinePenUI.InitFramedUI("Pen for Connecting Line", PPform.GetWidget(), args);
}

void PPO_EnterXYOnPlotPS::MenuOpen(PPO_EnterXYOnPlot& inData)
{
    MenuBaseC::MenuOpen(true, false);

    currentPtSymbolUI.ResetPens(inData.objPenSet);
    enteredPtSymbolUI.ResetPens(inData.objPenSet);
    deletedPtSymbolUI.ResetPens(inData.objPenSet);

    currLinePenUI.ResetPens(inData.objPenSet);
    inputXYObjRefUI.StdMenuInit(inData.inputXYObjRef);

    currObj = &inData;
}

void PPO_EnterXYOnPlotPS::SetMenuData(PPO_EnterXYOnPlot& inData)
{
    PPO_BasePS::SetMenuData(inData);

    inputXYObjRefUI.SetFuncObjRef(inData.inputXYObjRef);
    currentPtSymbolUI.SetSymbolSpec(inData.currentPtSymbol);
    enteredPtSymbolUI.SetSymbolSpec(inData.enteredPtSymbol);
    deletedPtSymbolUI.SetSymbolSpec(inData.deletedPtSymbol);

    currLinePenUI.SetPenSelection(inData.currLinePen);

    SetAllSensitive();
}

bool PPO_EnterXYOnPlotPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    return allOK;
}

void PPO_EnterXYOnPlotPS::GetMenuData(PPO_EnterXYOnPlot& outData)
{
    PPO_BasePS::GetMenuData(outData);

    currentPtSymbolUI.GetSymbolSpec(outData.currentPtSymbol);
    enteredPtSymbolUI.GetSymbolSpec(outData.enteredPtSymbol);
    deletedPtSymbolUI.GetSymbolSpec(outData.deletedPtSymbol);

    outData.currLinePen = currLinePenUI.GetPenSelection();
}

