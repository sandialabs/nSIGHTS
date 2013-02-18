///////////////////////////////////////////////////////////////////////////////////
//
// PPO_AnnoBasePS.cpp
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

#include <genPlotClass/PD_3D.h>

#include <genPPO/PPO_AnnoBase.h>
#include <genPPO/PPO_AnnoBasePS.h>

PPO_AnnoBasePS::~PPO_AnnoBasePS() {};

void  PPO_AnnoBasePS::InitMenu(const PPO_AnnoBase& inData)
{
    const AnnoObjC& basePO = inData.basePO;

    useTran = dynamic_cast<PD_3D*>(basePO.assocPlot) && basePO.SupportsTransparency();

    MenuBaseC::InitMenu();
    ArgListC   args;

//  frame for common settings
    args.AllFormAttach();
    objFrame.SetOffsets(0, 0, -1, -1);
    objFrame.InitFramedUI("Plot Settings", URform.GetWidget(), args);

    args.Clear();
    args.BottomFormAttach(5);
    args.RightFormAttach();
    args.RightOffset(50);
    doplotToggle.StdToggleInit("Plot", objFrame.GetForm(), args);

    if (useTran)
    {
        static const char* tranLabels[]  =
                    {"Tran Grp 1", "Tran Grp 2", "Tran Grp 3",
                     "Tran Grp 4", "Tran Grp 5", "Tran Grp 6",
                     "Tran Grp 7", "Tran Grp 8"};
        tranChooser.PreInitSetUp(tranLabels, 8);
        args.Clear();
        args.BottomFormAttach();
        args.RightPosAttach(50);
        tranChooser.InitWidget(objFrame.GetForm(), args);
    }
}


void  PPO_AnnoBasePS::SetMenuData(PPO_AnnoBase& inData)
{
    MenuBaseC::SetMenuData(inData);

    doplotToggle.SetValue(inData.basePO.doPlot);

    if (useTran)
        tranChooser.SetChoice(inData.basePO.transGroup);

}

bool PPO_AnnoBasePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    return allOK;
}

void PPO_AnnoBasePS::GetMenuData(PPO_AnnoBase&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.basePO.doPlot    = doplotToggle.GetValue();

    if (useTran)
        outData.basePO.transGroup = tranChooser.GetChoice();
}


void PPO_AnnoBasePS::MenuDefault()
{
    MenuBaseC::MenuDefault();
    doplotToggle.SetValue(true);
}

