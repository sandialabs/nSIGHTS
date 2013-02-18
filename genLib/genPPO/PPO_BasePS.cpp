///////////////////////////////////////////////////////////////////////////////////
//
// PPO_BasePS.cpp
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

#include <genPPO/PPO_Base.h>
#include <genPPO/PPO_BasePS.h>

PPO_BasePS::~PPO_BasePS() {};

void  PPO_BasePS::InitMenu(const PPO_Base& inData)
{
    MenuBaseC::InitMenu();
    ArgListC   args;

//  frame for common settings
    args.AllFormAttach();
    objFrame.SetOffsets(0, 0, -1, -1);
    objFrame.InitFramedUI("Plot Settings", URform.GetWidget(), args);


    const PlotObjC& basePO = inData.basePO;
    usePolyOffset = basePO.is3DObject && basePO.usePolyOffset;
    useLayer =  !basePO.is3DObject;
    useReport = useLayer && basePO.reportOK;
    useActive = useLayer && basePO.activeOK;
    useOffsetCoord = basePO.is3DObject;
    useTran = basePO.is3DObject && basePO.SupportsTransparency();

    if (usePolyOffset)
    {
        args.Clear();
        args.BottomFormAttach();
        args.LeftFormAttach();
        args.RightPosAttach(45);
        polyOffsetUI.SetFieldLen(9);
        polyOffsetUI.InitLabeledUI("PolyOff", objFrame.GetForm(), args);
        polyOffsetUI.SetFieldFormat(SC_RealFormat(ncf_Scientific, 1));
    }

    if (useLayer)
    {
        static const char* layLabels[]  = {"Layer 0", "Layer 1", "Layer 2", "Layer 3",
                                     "Layer 4", "Layer 5", "Layer 6", "Layer 7"};
        layerChooser.PreInitSetUp(layLabels, 8);
        args.Clear();
        args.BottomFormAttach();
        args.RightPosAttach(30);
        layerChooser.InitWidget(objFrame.GetForm(), args);
    }
    else if (useTran)
    {
        static const char* tranLabels[]  =
                    {"Tran Grp 1", "Tran Grp 2", "Tran Grp 3",
                     "Tran Grp 4", "Tran Grp 5", "Tran Grp 6",
                     "Tran Grp 7", "Tran Grp 8"};
        tranChooser.PreInitSetUp(tranLabels, 8);
        args.Clear();
        args.BottomFormAttach();
        args.RightPosAttach(75);
        tranChooser.InitWidget(objFrame.GetForm(), args);
    }


    if (useReport || useActive)
    {
        args.StdToggleSetup(45);
        if (useReport)
            reportToggle.StdToggleInit("Report", objFrame.GetForm(), args);
        else
            activeToggle.StdToggleInit("Active", objFrame.GetForm(), args);
    }

    args.Clear();
    args.BottomFormAttach(5);
    args.RightFormAttach();
    args.RightOffset(50);
    doplotToggle.StdToggleInit("Plot", objFrame.GetForm(), args);
}




void  PPO_BasePS::SetMenuData(PPO_Base& inData)
{
    MenuBaseC::SetMenuData(inData);

    const PlotObjC& basePO = inData.basePO;
    if (usePolyOffset)
        polyOffsetUI.SetFieldValue(basePO.polyOffset);

    if (useLayer)
        layerChooser.SetChoice(basePO.plotLayer);

    if (useTran)
        tranChooser.SetChoice(basePO.transGroup);

    if (useReport)
        reportToggle.SetValue(basePO.cursorReport->showReport);

    if (useActive)
        activeToggle.SetValue(basePO.isActive);

    if (useOffsetCoord)
        offsetCoordUI.SetOffsetCoord(basePO.offsetCoord);

    doplotToggle.SetValue(basePO.doPlot);
}

bool PPO_BasePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    if (usePolyOffset)
        polyOffsetUI.FieldValueOK(allOK, "Polygon offset", -1.0E+04, 1.0E+04);
    if (useOffsetCoord)
        offsetCoordUI.UIdataOK(allOK);
    return allOK;
}

void PPO_BasePS::GetMenuData(PPO_Base&  outData)
{
    MenuBaseC::GetMenuData(outData);

    PlotObjC& basePO = outData.basePO;

    if (usePolyOffset)
        basePO.polyOffset  = polyOffsetUI.GetFieldValue();

    if (useLayer)
        basePO.plotLayer = layerChooser.GetChoice();

    if (useTran)
        basePO.transGroup = tranChooser.GetChoice();

    if (useReport)
        basePO.cursorReport->showReport = reportToggle.GetValue();

    if (useActive)
        basePO.isActive = activeToggle.GetValue();

    if (useOffsetCoord)
        offsetCoordUI.GetOffsetCoord(basePO.offsetCoord);

    basePO.doPlot    = doplotToggle.GetValue();
}


void PPO_BasePS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    if (useLayer)
        layerChooser.SetChoice(0);
    if (usePolyOffset)
        polyOffsetUI.SetFieldValue(0.0);
    if (useReport)
        reportToggle.SetValue(false);
    if (useActive)
        activeToggle.SetValue(true);
    doplotToggle.SetValue(true);
}

