///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ExtendedProfileContourPS.cpp
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
//      File I/O and user interface for all nsPFOGL functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#include "NsLibAfx.h"

#include <nsClass/DO_ExtendedProfile.h>

#include "PPO_ExtendedProfileContour.h"
#include "PPO_ExtendedProfileContourPS.h"


PPO_ExtendedProfileContourPS::PPO_ExtendedProfileContourPS() :
    PPO_ContourSpecPS(typeid(DO_ExtendedProfile))
{
}

void PPO_ExtendedProfileContourPS::OverrideMasterPenCB(void* inObj)
{
    static_cast<PPO_ExtendedProfileContourPS*>(inObj)->OverrideMasterPen();
}

void PPO_ExtendedProfileContourPS::OverrideMasterPen()
{
    overrideMaster = overrideMasterUI.GetValue();
    SetAllSensitive();
}


void PPO_ExtendedProfileContourPS::SetAllSensitive()
{
    PPO_ContourSpecPS::SetAllSensitive();

    if (is3D)
    {
        bool plotTubes = plotAsTubesUI.GetBoolChoice();
        contourLineThkUI.SetSensitive(!plotTubes);
        contourLineTypeUI.SetSensitive(!plotTubes);
        mitreLinesUI.SetSensitive(!plotTubes);
        tubeDataUI.SetSensitive(plotTubes);
    }
}


void PPO_ExtendedProfileContourPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_ExtendedProfileContourPS*>(inObj)->SetAllSensitive();
}

void PPO_ExtendedProfileContourPS::InitMenu(const PPO_ExtendedProfileContour&   inData)
{
    ArgListC args;

    is3D = inData.is3DObject;

    if (is3D)
    {
        PPO_BasePS::InitMenu(inData);
        extendedProfileDataUI.StdInit(PPform.GetWidget(), dataObjRefUI, topRowForm);

        args.StdTopObjectAttach(extendedProfileDataUI.fixedZValueMSUI);

        PPO_ContourSpecPS::InitMenu(inData, args);

        args.StdTopObjectAttach(legendGCUI);
        extrusionRow.InitWidget(PPform.GetWidget(), args);

        args.StdSplitSetup(25);
        plotAsTubesUI.PreInitSetUp("Lines", "Tubes");
        plotAsTubesUI.InitFramedUI("Plot Type", extrusionRow.GetWidget(), args);
        plotAsTubesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

        args.StdSplitFinish(75);

        tubeDataUI.StdInit(extrusionRow.GetWidget(), args);

        args.StdTopObjectAttach(extrusionRow.GetWidget());
    }
    else
    {
        PPO_ContourSpecPS::InitMenu(inData, false);

        args.StdTopObjectAttach(legendGCUI);
    }

    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    overrideMasterUI.StdToggleInit("Override master line spec", optionFrame.GetForm(), args);
    overrideMasterUI.AddOtherObjectCallback(OverrideMasterPenCB, this);

    if (is3D)
    {
        args.StdTopObjectAttach(optionFrame);
        coordMapUI.StdInit(PPform.GetWidget(), args);

        args.StdTopObjectAttach(coordMapUI.coordMapFrame);
        offsetCoordUI.StdInit("Offset", PPform.GetWidget(), args);
    }

    SetMainformScrolled(true, 520, 330);
}

void PPO_ExtendedProfileContourPS::MenuOpen(PPO_ExtendedProfileContour& inData)
{
    PPO_ContourSpecPS::MenuOpen(inData);
    contourPenUI.ResetPens(inData.objPenSet);
    if (is3D)
        extendedProfileDataUI.MenuOpen(inData, inData);
}

void PPO_ExtendedProfileContourPS::SetMenuData(PPO_ExtendedProfileContour& inData)
{
    PPO_ContourSpecPS::SetMenuData(inData, inData);
    overrideMaster = inData.overrideMasterLineSpec;
    overrideMasterUI.SetValue(overrideMaster);

    if (inData.is3DObject)
    {
        extendedProfileDataUI.Set3DExtendedProfileData(inData);
        plotAsTubesUI.SetBoolChoice(inData.plotAsTubes);
        tubeDataUI.SetTubeExtrusionSpec(inData.tubeData);
        coordMapUI.Set3DCoordMap(inData.coordMap);
    }
    SetAllSensitive();
}

bool PPO_ExtendedProfileContourPS::UIdataOK()
{
    PPO_ContourSpecPS::UIdataOK();
    if (useOffsetCoord)                 // proxy for is 3D
    {
        extendedProfileDataUI.UIdataOK(allOK);
    }
    return allOK;
}

void PPO_ExtendedProfileContourPS::GetMenuData(PPO_ExtendedProfileContour& outData)
{
    overrideMaster = overrideMasterUI.GetValue();
    PPO_ContourSpecPS::GetMenuData(outData, outData);
    if (outData.is3DObject)
    {
        extendedProfileDataUI.Get3DExtendedProfileData(outData);
        outData.plotAsTubes = plotAsTubesUI.GetBoolChoice();
        if (outData.plotAsTubes)
            tubeDataUI.GetTubeExtrusionSpec(outData.tubeData);
        coordMapUI.Get3DCoordMap(outData.coordMap);
    }
}

