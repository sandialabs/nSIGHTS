///////////////////////////////////////////////////////////////////////////////////
//
// PC_Platform3DPS.cpp
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
//      adds capabilities for 3D plotting only  view control
//  sliders and view animation dialog.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClass/U_Str.h>

#include <genPlotClass/C_PlotDef.h>
#include <genPlotClass/C_PlotObj.h>
#include <genPlotClass/C_AnnoObj.h>
#include <genPlotClass/PC_ObjLimitRect.h>
#include <genPlotClassPS/PC_Platform3DPS.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////
// PC_Platform3DPS

PC_Platform3DPS::PC_Platform3DPS(const char* plotType, PD_3D& assPlt, OGL3DBase& ogl3DObj) :
    base3DplotDef(assPlt),
    PC_PlatformPS(plotType, assPlt, ogl3DObj, 26, 118, 102),
    viewAnimationDialog(assPlt, *this),
    transparencyDialog(assPlt, *this),
    vrmlDialog("VRML")
{
}

PC_Platform3DPS::~PC_Platform3DPS ()
{
}

int PC_Platform3DPS::CalcStatusBarHeight()
{
    if (statusForm.IsSmall())
        return statusBarHeight2;
    else
        return statusBarHeight;
}

int PC_Platform3DPS::CalcButtonBarHeight()
{
    return buttonBarHeight;
}

void PC_Platform3DPS::UpdateViewSettings()
{
    PC_View currView = base3DplotDef.GetCurrentView();

    azimuthUI.SetValues(currView.azimuth);
    elevationUI.SetValues(currView.elevation);
    scaleUI.SetValues(currView.scale);
    xtranUI.SetValues(currView.translation.cX);
    ytranUI.SetValues(currView.translation.cY);
    ztranUI.SetValues(currView.translation.cZ);
}


//  callbacks
void PC_Platform3DPS::AzimuthValueCB()
{
    PC_View currView = base3DplotDef.GetCurrentView();
    if (!azimuthUI.ValHasChanged(currView.azimuth))
        return;
    base3DplotDef.ChangeView(currView);
}

void PC_Platform3DPS::AzimuthSliderCB(bool bTracking)
{
    azimuthUI.SliderValueChangedCB();
    if (!IsOptimizeRedraw() || !bTracking)
        AzimuthValueCB();
}


void PC_Platform3DPS::ElevationValueCB()
{
    PC_View currView = base3DplotDef.GetCurrentView();
    if (!elevationUI.ValHasChanged(currView.elevation))
        return;
    base3DplotDef.ChangeView(currView);
}
void PC_Platform3DPS::ElevationSliderCB(bool bTracking)
{
    elevationUI.SliderValueChangedCB();
    if (!IsOptimizeRedraw() || !bTracking)
        ElevationValueCB();
}


void PC_Platform3DPS::ScaleValueCB()
{
    PC_View currView = base3DplotDef.GetCurrentView();
    if (!scaleUI.ValHasChanged(currView.scale))
        return;
    base3DplotDef.ChangeView(currView);
}
void PC_Platform3DPS::ScaleSliderCB(bool bTracking)
{
    scaleUI.SliderValueChangedCB();
    if (!IsOptimizeRedraw() || !bTracking)
        ScaleValueCB();
}


void PC_Platform3DPS::XtranValueCB()
{
    PC_View currView = base3DplotDef.GetCurrentView();
    if (!xtranUI.ValHasChanged(currView.translation.cX))
        return;
    base3DplotDef.ChangeView(currView);
}
void PC_Platform3DPS::XtranSliderCB(bool bTracking)
{
    xtranUI.SliderValueChangedCB();
    if (!IsOptimizeRedraw() || !bTracking)
        XtranValueCB();
}

void PC_Platform3DPS::YtranValueCB()
{
    PC_View currView = base3DplotDef.GetCurrentView();
    if (!ytranUI.ValHasChanged(currView.translation.cY))
        return;
    base3DplotDef.ChangeView(currView);
}
void PC_Platform3DPS::YtranSliderCB(bool bTracking)
{
    ytranUI.SliderValueChangedCB();
    if (!IsOptimizeRedraw() || !bTracking)
        YtranValueCB();
}

void PC_Platform3DPS::ZtranValueCB()
{
    PC_View currView = base3DplotDef.GetCurrentView();
    if (!ztranUI.ValHasChanged(currView.translation.cZ))
        return;
    base3DplotDef.ChangeView(currView);
}
void PC_Platform3DPS::ZtranSliderCB(bool bTracking)
{
    ztranUI.SliderValueChangedCB();
    if (!IsOptimizeRedraw() || !bTracking)
        ZtranValueCB();
}


void PC_Platform3DPS::AttitudeResetCB()
{
    PC_View currView = base3DplotDef.GetCurrentView();
    currView.elevation = 90.0;
    currView.azimuth = 0.0;
    currView.scale = 1.0;
    base3DplotDef.ChangeView(currView);
}
void PC_Platform3DPS::TranslationResetCB()
{
    PC_View currView = base3DplotDef.GetCurrentView();
    currView.translation = Coord3D(0.0);
    base3DplotDef.ChangeView(currView);
}

static void  AzValCB (void* clientData)
{
    ((PC_Platform3DPS*) clientData)-> AzimuthValueCB();
}

static void  AzSlideCB (void* clientData, InteractiveWidget* inWidget)
{
    SliderWidget* slider = static_cast<SliderWidget*>(inWidget);
    ((PC_Platform3DPS*) clientData)-> AzimuthSliderCB(slider->IsTracking());
}

static void  ElevValCB (void* clientData)
{
    ((PC_Platform3DPS*) clientData)-> ElevationValueCB();
}


static void  ElevSlideCB (void* clientData, InteractiveWidget* inWidget)
{
    SliderWidget* slider = static_cast<SliderWidget*>(inWidget);
    ((PC_Platform3DPS*) clientData)-> ElevationSliderCB(slider->IsTracking());
}


static void  ScalValCB (void* clientData)
{
    ((PC_Platform3DPS*) clientData)-> ScaleValueCB();
}
static void  ScalSlideCB (void* clientData, InteractiveWidget* inWidget)
{
    SliderWidget* slider = static_cast<SliderWidget*>(inWidget);
    ((PC_Platform3DPS*) clientData)-> ScaleSliderCB(slider->IsTracking());
}

static void  XtranValCB (void* clientData)
{
    ((PC_Platform3DPS*) clientData)-> XtranValueCB();
}
static void  XtranSlideCB (void* clientData, InteractiveWidget* inWidget)
{
    SliderWidget* slider = static_cast<SliderWidget*>(inWidget);
    ((PC_Platform3DPS*) clientData)-> XtranSliderCB(slider->IsTracking());
}


static void  YtranValCB (void* clientData)
{
    ((PC_Platform3DPS*) clientData)-> YtranValueCB();
}
static void  YtranSlideCB (void* clientData, InteractiveWidget* inWidget)
{
    SliderWidget* slider = static_cast<SliderWidget*>(inWidget);
    ((PC_Platform3DPS*) clientData)-> YtranSliderCB(slider->IsTracking());
}

static void  ZtranValCB (void* clientData)
{
    ((PC_Platform3DPS*) clientData)-> ZtranValueCB();
}
static void  ZtranSlideCB (void* clientData, InteractiveWidget* inWidget)
{
    SliderWidget* slider = static_cast<SliderWidget*>(inWidget);
    ((PC_Platform3DPS*) clientData)-> ZtranSliderCB(slider->IsTracking());
}


static void  AttResetCB (void* clientData)
{
    ((PC_Platform3DPS*) clientData)-> AttitudeResetCB();
}

static void  TranResetCB (void* clientData)
{
    ((PC_Platform3DPS*) clientData)-> TranslationResetCB();
}


static void  AnimDialogCB (void* clientData)
{
    PC_Platform3DPS* specPtr = (PC_Platform3DPS*) clientData;
    specPtr->InitViewAnimation();
}

bool PC_Platform3DPS::IsOptimizeRedraw()
{
    base3DplotDef.optimizeRedraw = optimizeRedrawCheckbox.GetValue();
    return base3DplotDef.optimizeRedraw;
}

void PC_Platform3DPS::InitGraphicsWindow()
{
    if (initComplete)
        return;

    PC_PlatformPS::InitGraphicsWindow();

    ArgListC args;

    sliderForm.PreInitSetUp(0, 8, 58);

    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.RightOffset(5);
    args.SetPlaceHolder();
    args.BottomFormAttach();
    args.TopFormAttach();
    sliderForm.InitFormUI(statusForm.GetWidget(), args);

    //  frame for reset buttons
    args.Clear();
    args.AllFormAttach();
    //args.StdFormAttach();
    buttonFrame.SetOffsets(2, 2, 1, 1);
    buttonFrame.InitFramedUI("Reset", sliderForm.GetLeft(), args);

    //  frame for attitude control
    attitudeFrame.InitFramedUI("Attitude", sliderForm.GetMiddle(), args);

    //  frame for translation control
    transFrame.InitFramedUI("Translation", sliderForm.GetRight(), args);

    //  buttons
    args.StdFormAttach();
    args.AllOffsets(2, 2, 4, 4);
    args.BottomPosAttach(33);
    resetAttitudeButton.StdButtonInit("Att", buttonFrame.GetForm(), args);
    resetAttitudeButton.AddOtherObjectCallback(AttResetCB, this);

    args.StdTopObjectAttach(resetAttitudeButton.GetWidget());
    args.AllOffsets(2, 3, 4, 4);
    args.BottomPosAttach(66);
    resetTranslationButton.StdButtonInit("Tran", buttonFrame.GetForm(), args);
    resetTranslationButton.AddOtherObjectCallback(TranResetCB, this);

    args.StdTopObjectAttach(resetTranslationButton.GetWidget());
    args.AllOffsets(2, 3, 4, 4);
    optimizeRedrawCheckbox.StdToggleInit("Optim", buttonFrame.GetForm(), args);
    optimizeRedrawCheckbox.SetValue(base3DplotDef.optimizeRedraw);
    //optimizeRedrawCheckbox.AddOtherObjectCallback(OptimizeRedrawToggle, this);

    //  attitude - elevation
    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.RightOffset(5);
    args.SetPlaceHolder();
    args.TopFormAttach();

    elevationUI.SetSliderVals(-90.0, 90.0, 0, 360, 10);
    elevationUI.InitLabeledUI("El:", attitudeFrame.GetForm(), args);
    elevationUI.valSlider.AddCombinedObjectCallback(ElevSlideCB, this);
    elevationUI.valDisplay.AddOtherObjectCallback(ElevValCB, this);

    args.ResetPlace();
    args.TopUICAttach(elevationUI);
    //args.TopOffset(3);
    azimuthUI.SetSliderVals(-180.0, 180.0, 0, 720, 15);
    azimuthUI.InitLabeledUI("Az:", attitudeFrame.GetForm(), args);
    azimuthUI.valSlider.AddCombinedObjectCallback(AzSlideCB, this);
    azimuthUI.valDisplay.AddOtherObjectCallback(AzValCB, this);

    args.ResetPlace();
    args.TopUICAttach(azimuthUI);
    //args.TopOffset(3);
    scaleUI.logSlider = true;
    scaleUI.SetSliderVals(-3.0, 2.0, 0, 400, 25);
    scaleUI.InitLabeledUI("Sc:", attitudeFrame.GetForm(), args);
    scaleUI.valDisplay.SetFieldFormat(SC_RealFormat(ncf_Decimal, 3));
    scaleUI.valSlider.AddCombinedObjectCallback(ScalSlideCB, this);
    scaleUI.valDisplay.AddOtherObjectCallback(ScalValCB, this);

    args.ResetPlace();
    args.TopFormAttach();
    xtranUI.SetSliderVals(-2.0, 2.0, 0, 1000, 50);
    xtranUI.InitLabeledUI("X:", transFrame.GetForm(), args);
    xtranUI.valSlider.AddCombinedObjectCallback(XtranSlideCB, this);
    xtranUI.valDisplay.AddOtherObjectCallback(XtranValCB, this);

    args.ResetPlace();
    args.TopUICAttach(xtranUI);
    ytranUI.SetSliderVals(-2.0, 2.0, 0, 1000, 50);
    ytranUI.InitLabeledUI("Y:", transFrame.GetForm(), args);
    ytranUI.valSlider.AddCombinedObjectCallback(YtranSlideCB, this);
    ytranUI.valDisplay.AddOtherObjectCallback(YtranValCB, this);

    args.ResetPlace();
    args.TopUICAttach(ytranUI);
    ztranUI.SetSliderVals(-2.0, 2.0, 0, 1000, 50);
    ztranUI.InitLabeledUI("Z:", transFrame.GetForm(), args);
    ztranUI.valSlider.AddCombinedObjectCallback(ZtranSlideCB, this);
    ztranUI.valDisplay.AddOtherObjectCallback(ZtranValCB, this);

    layoutStatusBar.MenuOpen();
    initComplete = true;
    //RecalcLayout();
}

void PC_Platform3DPS::ResetGraphicsWindow()
{
    PC_PlatformPS::ResetGraphicsWindow();
    // nothing extra for now
}

void PC_Platform3DPS::RenameGraphicsWindow(const char* newName)
{
    if (viewAnimationDialog.IsVisible())
    {
        char controlName[80];
        MakeString(controlName, newName, "::View", 80);
        viewAnimationDialog.SetWindowText(controlName);
    }
    PC_PlatformPS::RenameGraphicsWindow(newName);
}



UINT PC_Platform3DPS::GetMenuResourceID()
{
    return IDR_VIEW_3D;
}

BEGIN_MESSAGE_MAP(PC_Platform3DPS, PC_PlatformPS)
    //{{AFX_MSG_MAP(PC_Platform3DPS)
    ON_COMMAND(ID_CONTROL_ANIMATION, OnAnimation)
    ON_COMMAND(ID_CONTROL_TRANSPARENCY, OnTransparency)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PC_Platform3DPS::OnAnimation()
{
    viewAnimationDialog.OpenViewAnimationDialog(this);
}

void PC_Platform3DPS::OnTransparency()
{
    transparencyDialog.OpenTransparencyDialog(this);
}

