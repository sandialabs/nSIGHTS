///////////////////////////////////////////////////////////////////////////////////
//
// PPO_2DAnalyticPS.cpp
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

#include "PPO_2DAnalyticPS.h"

void PPO_2DAnalyticPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_2DAnalyticPS*>(inObj)->SetAllSensitive();
}


void PPO_2DAnalyticPS::SetAllSensitive()
{
}



PPO_2DAnalyticPS::PPO_2DAnalyticPS()
{
}

void PPO_2DAnalyticPS::InitMenu(const PPO_2DAnalytic&   inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC  args;
    args.StdFormAttach();
    centerPtSymbolUI.InitFramedUI("Center Point", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(centerPtSymbolUI);
    rotationPtSymbolUI.InitFramedUI("Rotation Handle", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(rotationPtSymbolUI);
    endPtSymbolUI.InitFramedUI("Line End Handle", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(endPtSymbolUI);
    lineSpecUI.InitFramedUI("Line", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(lineSpecUI);
    statusFrame.InitFramedUI("Results", PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    outputCxUI.InitLabeledUI("Center X", statusFrame.GetLeft(), args);

    args.Clear();
    args.StdTopObjectAttach(outputCxUI);
    outputCyUI.InitLabeledUI("Center Y", statusFrame.GetLeft(), args);

    args.Clear();
    args.StdTopObjectAttach(outputCyUI);
    outputDxUI.InitLabeledUI("X Length", statusFrame.GetLeft(), args);

    args.Clear();
    args.StdTopObjectAttach(outputDxUI);
    outputDyUI.InitLabeledUI("Y Length", statusFrame.GetLeft(), args);

    args.Clear();
    args.StdFormAttach();
    outputDUI.InitLabeledUI("Length", statusFrame.GetRight(), args);

    args.Clear();
    args.StdTopObjectAttach(outputDUI);
    outputMUI.InitLabeledUI("Slope", statusFrame.GetRight(), args);

    args.Clear();
    args.StdTopObjectAttach(outputMUI);
    outputBUI.InitLabeledUI("Y intercept", statusFrame.GetRight(), args);

    SC_SciFormat defForm(8);

    outputCxUI.SetFieldFormat(defForm);
    outputCyUI.SetFieldFormat(defForm);
    outputDxUI.SetFieldFormat(defForm);
    outputDyUI.SetFieldFormat(defForm);
    outputDUI.SetFieldFormat(defForm);
    outputMUI.SetFieldFormat(defForm);
    outputBUI.SetFieldFormat(defForm);

}

void PPO_2DAnalyticPS::MenuOpen(PPO_2DAnalytic& inData)
{
    MenuBaseC::MenuOpen(true, false);

    centerPtSymbolUI.ResetPens(inData.objPenSet);
    rotationPtSymbolUI.ResetPens(inData.objPenSet);
    endPtSymbolUI.ResetPens(inData.objPenSet);
    lineSpecUI.ResetPens(inData.objPenSet);
}

void PPO_2DAnalyticPS::SetMenuData(PPO_2DAnalytic& inData)
{
    PPO_BasePS::SetMenuData(inData);

    centerPtSymbolUI.SetSymbolSpec(inData.centerPtSymbol);
    rotationPtSymbolUI.SetSymbolSpec(inData.rotationPtSymbol);
    endPtSymbolUI.SetSymbolSpec(inData.endPtSymbol);
    lineSpecUI.SetLineSpec(inData.lineSpec);
    SetAllSensitive();
    UpdateStatus(inData);
}

bool PPO_2DAnalyticPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    return allOK;
}

void PPO_2DAnalyticPS::GetMenuData(PPO_2DAnalytic& outData)
{
    PPO_BasePS::GetMenuData(outData);

    centerPtSymbolUI.GetSymbolSpec(outData.centerPtSymbol);
    rotationPtSymbolUI.GetSymbolSpec(outData.rotationPtSymbol);
    endPtSymbolUI.GetSymbolSpec(outData.endPtSymbol);
    lineSpecUI.GetLineSpec(outData.lineSpec);
}

void PPO_2DAnalyticPS::UpdateStatus(const PPO_2DAnalytic& inData)
{
    outputCxUI.SetFieldValue(inData.outputCxDO.realVal);
    outputCyUI.SetFieldValue(inData.outputCyDO.realVal);
    outputDxUI.SetFieldValue(inData.outputDxDO.realVal);
    outputDyUI.SetFieldValue(inData.outputDyDO.realVal);
    outputDUI.SetFieldValue(inData.outputDDO.realVal);
    outputMUI.SetFieldValue(inData.outputMDO.realVal);
    outputBUI.SetFieldValue(inData.outputBDO.realVal);
}

