///////////////////////////////////////////////////////////////////////////////////
//
// PPO_GridFishnetPS.cpp
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

#include <objClass/DO_GridData.h>

#include "PPO_GridFishnetPS.h"

// ****************************************

PPO_GridFishnetPS::PPO_GridFishnetPS() :
    gridDataObjRefUI(typeid(DO_GridData))
{
}

void PPO_GridFishnetPS::SetAllSensitive()
{
    bool plotTubes = plotAsTubesUI.GetBoolChoice();

    gridLineThkUI.SetSensitive(!plotTubes);
    gridLineTypeUI.SetSensitive(!plotTubes);

    tubeDataUI.SetSensitive(plotTubes);

    bool isOK = true;
    if (xplotModulusUI.FieldValueOK(isOK, "X modulus", 1, 100))
        plotLastXUI.SetSensitive(xplotModulusUI.GetFieldValue() > 1);

    isOK = true;
    if (yplotModulusUI.FieldValueOK(isOK, "Y modulus", 1, 100))
        plotLastYUI.SetSensitive(yplotModulusUI.GetFieldValue() > 1);

}

void PPO_GridFishnetPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_GridFishnetPS*>(inObj)->SetAllSensitive();
}



void PPO_GridFishnetPS::InitMenu(const PPO_GridFishnet& inData)
{
    PPO_BasePS::InitMenu(inData);

    is3D = inData.is3DObject;

    ArgListC    args;
    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(66);
    gridDataObjRefUI.topOffset = 0;
    gridDataObjRefUI.InitFramedUI("Grid Data", topRow.GetWidget(), args);

    args.StdSplitFinish(34);
    gridPenUI.InitFramedUI("Pen", topRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRow.GetWidget());
    lineRow.InitWidget(PPform.GetWidget(), args);

    int endSplit = 50;
    if (is3D)
    {
        args.StdSplitSetup(33);
        plotAsTubesUI.PreInitSetUp("Lines", "Tubes");
        plotAsTubesUI.InitFramedUI("Plot Type", lineRow.GetWidget(), args);
        plotAsTubesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

        args.StdSplitMiddle(33, 66);
        endSplit = 33;
    }
    else
    {
        args.StdSplitSetup();
    }

    gridLineThkUI.InitFramedUI("Line Thickness", lineRow.GetWidget(), args);

    args.StdSplitFinish(endSplit);
    gridLineTypeUI.InitFramedUI("Line Type", lineRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(lineRow.GetWidget());

    if (is3D)
    {
        tubeDataUI.StdInit(PPform.GetWidget(), args);
        args.StdTopObjectAttach(tubeDataUI.specFrame);
    }


    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdSplitSetup(25);
    xplotModulusUI.SetFieldLen(4);
    xplotModulusUI.InitLabeledUI("X modulus", optionsFrame.GetForm(), args);
    xplotModulusUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(30, 45);
    plotLastXUI.StdToggleInit("Plot Last X", optionsFrame.GetForm(), args);

    args.StdSplitMiddle(50, 75);
    yplotModulusUI.SetFieldLen(4);
    yplotModulusUI.InitLabeledUI("Y modulus", optionsFrame.GetForm(), args);
    yplotModulusUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(20);
    plotLastYUI.StdToggleInit("Plot Last Y", optionsFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(optionsFrame);
    legendOverrideUI.InitFramedUI("Legend Description", PPform.GetWidget(), args);

    if (is3D)
    {
        args.Clear();
        args.StdTopObjectAttach(legendOverrideUI);
        coordMapUI.StdInit(PPform.GetWidget(), args);

        args.Clear();
        args.StdTopObjectAttach(coordMapUI.coordMapFrame);
        offsetCoordUI.StdInit("Offset", PPform.GetWidget(), args);
    }
}


void PPO_GridFishnetPS::MenuOpen(PPO_GridFishnet&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    gridDataObjRefUI.StdMenuInit(inData.gridDataObjRef);
    gridPenUI.ResetPens(inData.objPenSet);
}

void PPO_GridFishnetPS::SetMenuData(PPO_GridFishnet&    inData)
{
    gridDataObjRefUI.SetFuncObjRef(inData.gridDataObjRef);

    gridPenUI.SetPenSelection(inData.gridPen);
    gridLineThkUI.SetLineThick(inData.gridLineThk);
    gridLineTypeUI.SetLineType(inData.gridLineType);

    xplotModulusUI.SetFieldValue(inData.xplotModulus);
    plotLastXUI.SetValue(inData.plotLastX);

    yplotModulusUI.SetFieldValue(inData.yplotModulus);
    plotLastYUI.SetValue(inData.plotLastY);

    legendOverrideUI.SetLegendOverride(inData.legendOverride);
    if (is3D)
    {
        plotAsTubesUI.SetBoolChoice(inData.plotAsTubes);
        tubeDataUI.SetTubeExtrusionSpec(inData.tubeData);
        coordMapUI.Set3DCoordMap(inData.coordMap);
        SetAllSensitive();
    }

    PPO_BasePS::SetMenuData(inData);
}

bool PPO_GridFishnetPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    gridDataObjRefUI.FuncObjRefOK(allOK, "grid input");
    xplotModulusUI.FieldValueOK(allOK, "X modulus", 1, 100);
    yplotModulusUI.FieldValueOK(allOK, "Y modulus", 1, 100);

    return allOK;
}

void PPO_GridFishnetPS::GetMenuData(PPO_GridFishnet&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.gridDataObjRef = gridDataObjRefUI.GetFuncObjRef();
    outData.gridPen    = gridPenUI.GetPenSelection();
    legendOverrideUI.GetLegendOverride(outData.legendOverride);

    outData.xplotModulus = xplotModulusUI.GetFieldValue();
    if(outData.xplotModulus > 1)
        outData.plotLastX = plotLastXUI.GetValue();

    outData.yplotModulus = yplotModulusUI.GetFieldValue();
    if(outData.yplotModulus > 1)
        outData.plotLastY = plotLastYUI.GetValue();

    if (is3D)
    {
        outData.plotAsTubes = plotAsTubesUI.GetBoolChoice();
        if (outData.plotAsTubes)
            tubeDataUI.GetTubeExtrusionSpec(outData.tubeData);
        coordMapUI.Get3DCoordMap(outData.coordMap);
    }


    if (!outData.plotAsTubes)
    {
        outData.gridLineThk = gridLineThkUI.GetLineThick();
        outData.gridLineType = gridLineTypeUI.GetLineType();
    }


}

