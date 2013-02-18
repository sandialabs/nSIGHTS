///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DPS.cpp
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
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "PPD_3D.h"
#include "PPD_3DPS.h"

PPD_3DPS::PPD_3DPS() :
    fieldOfViewMSUI(typeid(DO_Real), false),
    eyeSeparationDivisorMSUI(typeid(DO_Real), false)
{}


void PPD_3DPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPD_3DPS*>(inObj)->SetAllSensitive();
}

void PPD_3DPS::SetAllSensitive()
{
    fieldOfViewMSUI.SetSensitive(plotProjectionUI.GetChoice() > 0);
    eyeSeparationDivisorMSUI.SetSensitive(plotProjectionUI.GetChoice() == 2);
}


void PPD_3DPS::InitMenu()
{
    PPD_BasePS::InitMenu();

    ArgListC args;

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.LeftFormAttach();
    args.RightUICAttach(marginFrame);
    viewFrame.InitFramedUI("Projection", lastRowForm.GetWidget(), args);

    args.StdFormAttach();
    args.RightOffset(10);

    const char* strs[] = {"Orthographic", "Perspective", "Stereo Perspective"};
    plotProjectionUI.PreInitSetUp(strs, 3, true);
    plotProjectionUI.InitFormUI(viewFrame.GetForm(), args);
    plotProjectionUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(plotProjectionUI);
    args.TopOffset(10);
    viewParamForm.InitWidget(viewFrame.GetForm(), args);

    args.StdSplitSetup();
    fieldOfViewMSUI.realValueUI.SetFieldFormat(SC_RealFormat(1));
    fieldOfViewMSUI.InitUI("Field of View",  args, viewParamForm.GetWidget(), false, false);

    args.StdSplitFinish();
    eyeSeparationDivisorMSUI.realValueUI.SetFieldFormat(SC_RealFormat(1));
    eyeSeparationDivisorMSUI.InitUI("Eye Separation Divisor",  args, viewParamForm.GetWidget(), false, false);

    SetMainformScrolled(true, 520, 310);
}


void PPD_3DPS::MenuOpen(PPD_3D& inData)
{
    PPD_BasePS::MenuOpen(inData);
    fieldOfViewMSUI.MenuInit(inData.base3DPD.fieldOfViewMS);
    eyeSeparationDivisorMSUI.MenuInit(inData.base3DPD.eyeSeparationDivisorMS);
}


void PPD_3DPS::SetMenuData(PPD_3D&  inData)
{
    PPD_BasePS::SetMenuData(inData);

    int projChoice = 0;
    if (inData.base3DPD.plotProjection == PD_3D::tdp_Perspective)
        if (inData.base3DPD.useStereo3DIfAvailable)
            projChoice = 2;
        else
            projChoice = 1;

    plotProjectionUI.SetChoice(projChoice);
    fieldOfViewMSUI.SetValue(inData.base3DPD.fieldOfViewMS);
    eyeSeparationDivisorMSUI.SetValue(inData.base3DPD.eyeSeparationDivisorMS);
    SetAllSensitive();
}

bool PPD_3DPS::UIdataOK()
{
    PPD_BasePS::UIdataOK();
    if (plotProjectionUI.GetChoice() > 0)
        fieldOfViewMSUI.UIdataOK(allOK, 1.0, 85.0);
    if (plotProjectionUI.GetChoice() == 2)
       eyeSeparationDivisorMSUI.UIdataOK(allOK, 5.0, 200.0);
    return allOK;
}

void PPD_3DPS::GetMenuData(PPD_3D& outData)
{
   PPD_BasePS::GetMenuData(outData);

   if (plotProjectionUI.GetChoice() == 0)
       outData.base3DPD.plotProjection = PD_3D::tdp_Orthographic;
   else
   {
        outData.base3DPD.plotProjection = PD_3D::tdp_Perspective;
        outData.base3DPD.useStereo3DIfAvailable = (plotProjectionUI.GetChoice() == 2);
        fieldOfViewMSUI.GetValue(outData.base3DPD.fieldOfViewMS);

        if (outData.base3DPD.useStereo3DIfAvailable)
            eyeSeparationDivisorMSUI.GetValue(outData.base3DPD.eyeSeparationDivisorMS);
   }
}

void  PPD_3DPS::MenuDefault()
{
    PPD_BasePS::MenuDefault();
    plotProjectionUI.SetChoice(0);
    fieldOfViewMSUI.realValueUI.SetFieldValue(40.0);
    eyeSeparationDivisorMSUI.realValueUI.SetFieldValue(40.0);
    SetAllSensitive();
}

