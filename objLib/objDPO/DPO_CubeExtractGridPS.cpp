///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CubeExtractGridPS.cpp
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

#include "DPO_CubeExtractGrid.h"
#include "DPO_CubeExtractGridPS.h"

DPO_CubeExtractGridPS::DPO_CubeExtractGridPS()
    : extractValMSUI(typeid(DO_Real), true)
{
}


void DPO_CubeExtractGridPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_CubeExtractGridPS*>(inObj)->SetAllSensitive();
}

void DPO_CubeExtractGridPS::SetAllSensitive()
{
    extractValMSUI.SetAllSensitive();
}

void DPO_CubeExtractGridPS::ResetLabelsCB(void* inObj)
{
    static_cast<DPO_CubeExtractGridPS*>(inObj)->ResetLabels();
}

void DPO_CubeExtractGridPS::ResetLabels()
{
    gridXUI.SetLabels(cubeInputUI.cubeLabels);
    gridYUI.SetLabels(cubeInputUI.cubeLabels);
}


void DPO_CubeExtractGridPS::ToggleExposedExtractValCB(void* inObj)
{
    static_cast<DPO_CubeExtractGridPS*>(inObj)->ToggleExposedExtractVal();
}

void DPO_CubeExtractGridPS::ToggleExposedExtractVal()
{
    SetAllSensitive();
    // recast current to local
    static_cast<DPO_CubeExtractGrid*>(MenuObjC::currMenuObj)->
        exposedExtractVal.ToggleExposed(extractValMSUI.TurnExposedOff());
}



void DPO_CubeExtractGridPS::InitMenu()
{

    MenuBaseC::InitMenu();

    cubeInputUI.StdInit(URform.GetWidget());
    cubeInputUI.cubeInputObjRefUI.AddOtherObjectCallback(ResetLabelsCB, this);

    ArgListC args;
    args.Clear();
    args.StdFormAttach();
    extractFrame.InitFramedUI("Extraction Options", PPform.GetWidget(), args);

    static const char* gridStrings[] = {" Cube X ", " Cube Y " , " Cube Z "};

    args.StdSplitSetup(33);
    gridXUI.PreInitSetUp(gridStrings, 3);
    gridXUI.InitLabeledUI("Grid X :", extractFrame.GetForm(), args);

    args.StdSplitMiddle(35, 70);
    gridYUI.PreInitSetUp(gridStrings, 3);
    gridYUI.InitLabeledUI("Grid Y :", extractFrame.GetForm(), args);

    args.ResetPlace();
    args.LeftPosAttach(75);
    logInterpUI.StdToggleInit("Interp on log data", extractFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(extractFrame);
    extractValMSUI.InitUI("Extraction Constant", args, PPform.GetWidget(), true, true, ToggleExposedExtractValCB, this);

}

void DPO_CubeExtractGridPS::SetMenuData(DPO_CubeExtractGrid&  inData)
{
    MenuBaseC::SetMenuData(inData);
    cubeInputUI.SetCubeInput(inData);
    ResetLabels();

    gridXUI.SetChoice(inData.gridX);
    gridYUI.SetChoice(inData.gridY);
    logInterpUI.SetValue(inData.logInterp);
    extractValMSUI.SetValue(inData.extractValMS, inData.exposedExtractVal);
    SetAllSensitive();
}

void DPO_CubeExtractGridPS::MenuOpen(DPO_CubeExtractGrid& inData)
{
    MenuBaseC::MenuOpen(false, false);
    cubeInputUI.MenuOpen(inData);
    extractValMSUI.MenuInit(inData.extractValMS);
}


bool DPO_CubeExtractGridPS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    cubeInputUI.UIdataOK(allOK);
    extractValMSUI.UIdataOK(allOK);
    return allOK;
}

void DPO_CubeExtractGridPS::GetMenuData(DPO_CubeExtractGrid&  outData)
{
    MenuBaseC::GetMenuData(outData);
    cubeInputUI.GetCubeInput(outData);
    extractValMSUI.GetValue(outData.extractValMS);
    outData.gridX  = DC_CubeData::ExtractAxes(gridXUI.GetChoice());
    outData.gridY  = DC_CubeData::ExtractAxes(gridYUI.GetChoice());
    outData.logInterp = logInterpUI.GetValue();
}

