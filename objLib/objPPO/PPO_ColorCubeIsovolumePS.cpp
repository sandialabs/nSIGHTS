///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ColorCubeIsovolumePS.cpp
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


#include "PPO_ColorCubeIsovolumePS.h"

void PPO_ColorCubeIsovolumePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_ColorCubeIsovolumePS*>(inObj)->SetAllSensitive();
}

void PPO_ColorCubeIsovolumePS::SetAllSensitive()
{
    bool isPen = usePenUI.GetBoolChoice();
    isothermPenUI.SetSensitive(isPen);
    if (isPen)
    {
        limitAndMapForm.SetSensitive(false);
        outsideForm.SetSensitive(false);
    }
    else
    {
        PPO_ColorBasePS::SetAllSensitive();
        logRangeToggle.SetValue(logRangeUI.GetValue());
        clipToRangeBox.SetSensitive(false);
        logRangeToggle.SetSensitive(false);
    }
}


void PPO_ColorCubeIsovolumePS::ToggleexposedisoValue()
{
    isoValueMSUI.SetAllSensitive();
    static_cast<PPO_ColorCubeIsovolume*>(MenuObjC::currMenuObj)->exposedisoValue.ToggleExposed(isoValueMSUI.TurnExposedOff());
}

void PPO_ColorCubeIsovolumePS::ToggleexposedisoValueCB(void* inObj)
{
    static_cast<PPO_ColorCubeIsovolumePS*>(inObj)->ToggleexposedisoValue();
}

// ****************************************

PPO_ColorCubeIsovolumePS::PPO_ColorCubeIsovolumePS() :
    isoValueMSUI(typeid(DO_Real))
{
}

void PPO_ColorCubeIsovolumePS::InitMenu(const PPO_ColorCubeIsovolume&   inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC  args;

    args.StdFormAttach();
    cubeDataUI.StdInit(PPform.GetWidget(), inputDataObjRefUI, topRowForm, false);
    inputDataObjRefUI.AddOtherObjectCallback(ResetLabelsCB, this);

    args.StdTopObjectAttach(inputDataObjRefUI);
    isoValueMSUI.InitUI("Isovalue", args, PPform.GetWidget(), true, true, ToggleexposedisoValueCB, this);

    args.StdTopObjectAttach(isoValueMSUI);
    colorForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    usePenUI.PreInitSetUp("Color Map", "Selected Pen");
    usePenUI.InitFramedUI("Isovolume Color", colorForm.GetWidget(), args);
    usePenUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(34, 66);
    isothermPenUI.InitFramedUI("Single Color Pen", colorForm.GetWidget(), args);

    args.StdSplitFinish(33);
    transformFrame.InitFramedUI("Interpolate Transform", colorForm.GetWidget(), args);

    args.StdToggleSetup(5);
    logRangeUI.StdToggleInit("log 10", transformFrame.GetForm(), args);
    logRangeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(colorForm.GetWidget());
    static const char* limLabels[] = {"All data"};
    PPO_ColorBasePS::InitMenu(inData, args, limLabels, 1);

    // add new sensitivity
    limitSelectionMS.isMaster.AddOtherObjectCallback(SetAllSensitiveCB, this);
    autoLimitToggle.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(outsideForm.GetWidget());
    InitMapAndOffset(inData, args);
}

void PPO_ColorCubeIsovolumePS::MenuOpen(PPO_ColorCubeIsovolume& inData)
{
    PPO_CubeColorBasePS::MenuOpen(inData);
    isoValueMSUI.MenuInit(inData.isoValueMS);
    isothermPenUI.ResetPens(inData.objPenSet);
}

void PPO_ColorCubeIsovolumePS::SetMenuData(PPO_ColorCubeIsovolume& inData)
{
    PPO_CubeColorBasePS::SetMenuData(inData, inData);
    isoValueMSUI.SetValue(inData.isoValueMS, inData.exposedisoValue);
    usePenUI.SetBoolChoice(inData.usePen);
    isothermPenUI.SetPenSelection(inData.isothermPen);
    logRangeUI.SetValue(inData.rangeLimit.logRange);

    SetAllSensitive();
}

bool PPO_ColorCubeIsovolumePS::UIdataOK()
{
    PPO_CubeColorBasePS::UIdataOK();
    isoValueMSUI.UIdataOK(allOK);
    return allOK;
}

void PPO_ColorCubeIsovolumePS::GetMenuData(PPO_ColorCubeIsovolume& outData)
{
    PPO_CubeColorBasePS::GetMenuData(outData, outData);
    isoValueMSUI.GetValue(outData.isoValueMS);
    outData.usePen = usePenUI.GetBoolChoice();
    outData.isothermPen = isothermPenUI.GetPenSelection();
    outData.rangeLimit.logRange = logRangeUI.GetValue();
}

