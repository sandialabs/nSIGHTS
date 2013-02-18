///////////////////////////////////////////////////////////////////////////////////
//
// PS_ViewAnimationDialog.cpp
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
//      implements 3D View Animation dialog. 
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <iostream>

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/PD_3D.h>

#include <genPlotClassPS/PC_Platform3DPS.h>
#include <genPlotClassPS/PS_ViewAnimationDialog.h>

void PS_ViewAnimationDialog::ViewAnimationElementUIC::InitLabeledUI(char        inLabel[],
                                                                    Widget          parent,
                                                                    const ArgListC&  inArgs)
{
    //  form
    BaseUIC::InitFormUI(parent, inArgs);

    //  label
    ArgListC args;
    args.Clear();
    args.BottomFormAttach();
    args.BottomOffset(5);
    args.SetPlaceHolder();
    args.LeftFormAttach();
    args.LeftOffset(5);
    label.InitWidget(GetForm(), args);
    label.SetLabelText(inLabel);

    //  start field
    args.ResetPlace();
    args.LeftOffset(5);
    args.SetPlaceHolder();

    args.LeftPosAttach(25);
    args.RightPosAttach(62);

    startValUI.SetFieldLen(10);
    startValUI.DontUseDefaultHeight();
    startValUI.InitWidget(GetForm(),args);

    //  end field
    args.ResetPlace();
    args.LeftBaseAttach(startValUI);
    args.RightFormAttach();

    endValUI.SetFieldLen(10);
    endValUI.DontUseDefaultHeight();
    endValUI.InitWidget(GetForm(),args);
}


void PS_ViewAnimationDialog::ViewAnimationElementUIC::SetFormats(const SC_RealFormat& rf)
{
    startValUI.SetFieldFormat(rf);
    endValUI.SetFieldFormat(rf);
}

void PS_ViewAnimationDialog::ViewAnimationElementUIC::SetValues(double&   sVal,
                                                                  double&   eVal)
{
    startVal = &sVal;
    endVal   = &eVal;
}

void PS_ViewAnimationDialog::ViewAnimationElementUIC::UpdateFromValues()
{
    startValUI.SetFieldValue(*startVal);
    endValUI.SetFieldValue(*endVal);
}

bool PS_ViewAnimationDialog::ViewAnimationElementUIC::UpdateToValues()
{
    bool wasOK = true;
    if (startValUI.FieldValueOK(wasOK, "Animation start value"))
        (*startVal) = startValUI.GetFieldValue();
    if (endValUI.FieldValueOK(wasOK, "Animation end value"))
        (*endVal) = endValUI.GetFieldValue();
    return wasOK;
}


void PS_ViewAnimationDialog::ViewAnimationUIC::Initialize(Widget    parent,
                                                          PC_View&  stView,
                                                          PC_View&  endView)
{
    static char* viewLabels[]  = {"Elev",
                                  "Azim",
                                  "Scale",
                                  "X tran",
                                  "Y tran",
                                  "Z tran",
                                  "X min",
                                  "X max",
                                  "Y min",
                                  "Y max",
                                  "Z min",
                                  "Z max"};

    ArgListC    args;
    args.Clear();
    args.StdFormAttach();
    for (int i = 0; i < nviewElements; i++)
    {
        viewAnimationData[i].InitLabeledUI(viewLabels[i], parent, args);
        args.Clear();
        args.TopUICAttach(viewAnimationData[i]);
        args.LeftFormAttach();
        args.RightFormAttach();
    }

    viewAnimationData[0].SetValues(stView.elevation, endView.elevation);
    viewAnimationData[1].SetValues(stView.azimuth, endView.azimuth);
    viewAnimationData[2].SetValues(stView.scale, endView.scale);

    viewAnimationData[3].SetValues(stView.translation.cX, endView.translation.cX);
    viewAnimationData[4].SetValues(stView.translation.cY, endView.translation.cY);
    viewAnimationData[5].SetValues(stView.translation.cZ, endView.translation.cZ);

    viewAnimationData[6].SetValues(stView.viewLimits.minLim.cX, endView.viewLimits.minLim.cX);
    viewAnimationData[7].SetValues(stView.viewLimits.maxLim.cX, endView.viewLimits.maxLim.cX);

    viewAnimationData[8].SetValues(stView.viewLimits.minLim.cY, endView.viewLimits.minLim.cY);
    viewAnimationData[9].SetValues(stView.viewLimits.maxLim.cY, endView.viewLimits.maxLim.cY);

    viewAnimationData[10].SetValues(stView.viewLimits.minLim.cZ, endView.viewLimits.minLim.cZ);
    viewAnimationData[11].SetValues(stView.viewLimits.maxLim.cZ, endView.viewLimits.maxLim.cZ);

}

void PS_ViewAnimationDialog::ViewAnimationUIC::SetFormats(PD_3D& plotDef)
{
    SC_RealFormat viewFormats[nviewElements];
    SC_DecFormat defFormat(3);
    int i;
    for (i = 0; i < nviewElements; i++)
        viewFormats[i] = defFormat;

    bool xLin, yLin, zLin;
    plotDef.GetAxesTypes(xLin, yLin, zLin);
    SC_SciFormat  logFormat(4);
    SC_RealFormat linFormat;

    if (xLin)
    {
        viewFormats[6] = linFormat;
        viewFormats[7] = linFormat;
    }
    else
    {
        viewFormats[6] = logFormat;
        viewFormats[7] = logFormat;
    }

    if (yLin)
    {
        viewFormats[8] = linFormat;
        viewFormats[9] = linFormat;
    }
    else
    {
        viewFormats[8] = logFormat;
        viewFormats[9] = logFormat;
    }

    if (zLin)
    {
        viewFormats[10] = linFormat;
        viewFormats[11] = linFormat;
    }
    else
    {
        viewFormats[10] = logFormat;
        viewFormats[11] = logFormat;
    }

    for (i = 0; i < nviewElements; i++)
    {
        viewAnimationData[i].SetFormats(viewFormats[i]);
    }

}


void PS_ViewAnimationDialog::ViewAnimationUIC::UpdateFromViews()
{
    for (int i = 0; i < nviewElements; i++)
        viewAnimationData[i].UpdateFromValues();
}

bool PS_ViewAnimationDialog::ViewAnimationUIC::UpdateToViews()
{
    bool allOK = true;
    for (int i = 0; i < nviewElements; i++)
        allOK = allOK && viewAnimationData[i].UpdateToValues();

    return allOK;
}


PS_ViewAnimationDialog::PS_ViewAnimationDialog(PD_3D& assPlot, PC_Platform3DPS&  assPlatform) :
    base3DplotDef(assPlot),
    base3DPlatform(assPlatform),
    viewAnimationDialogForm(this),
    DialogShell("::View"),
    m_bProcessing(false)
{
    animating = false;
}

PS_ViewAnimationDialog::~PS_ViewAnimationDialog ()
{
}


void  PS_ViewAnimationDialog::AnimateButtonCB (void* clientData)
{
    static_cast<PS_ViewAnimationDialog*>(clientData)->DoAnimate();
}
void  PS_ViewAnimationDialog::StepButtonCB (void* clientData)
{
    static_cast<PS_ViewAnimationDialog*>(clientData)->DoStep();
}

void  PS_ViewAnimationDialog::GotoStartButtonCB(void* clientData)
{
    static_cast<PS_ViewAnimationDialog*>(clientData)->DoGotoStart();
}
void  PS_ViewAnimationDialog::GotoEndButtonCB(void* clientData)
{
    static_cast<PS_ViewAnimationDialog*>(clientData)->DoGotoEnd();
}
void  PS_ViewAnimationDialog::SetStartButtonCB(void* clientData)
{
    static_cast<PS_ViewAnimationDialog*>(clientData)->DoSetStart();
}
void  PS_ViewAnimationDialog::SetEndButtonCB(void* clientData)
{
    static_cast<PS_ViewAnimationDialog*>(clientData)->DoSetEnd();
}

void  PS_ViewAnimationDialog::EndToStartButtonCB(void* clientData)
{
    static_cast<PS_ViewAnimationDialog*>(clientData)->DoEndToStart();
}
void  PS_ViewAnimationDialog::CloseButtonCB (void* clientData)
{
    static_cast<PS_ViewAnimationDialog*>(clientData)->DoClose();
}


void PS_ViewAnimationDialog::OpenViewAnimationDialog(CWnd* pParentWnd)
{
    if (!IsCreated()) {

        ArgListC args;
        CRect rect(0, 0, 260, 450);

        char plotName[80];
        base3DPlatform.GetMainWindowTitle(plotName, false, 80);
        ConcatString(plotName, "::View", 80);

        CreateShell(pParentWnd, true, plotName, rect);
        CenterWindow();
        //viewAnimationDialogForm.Create(this);

        args.Clear();
        args.StdFormAttach();
        elementForm.InitWidget(viewAnimationDialogForm.GetWidget(), args);
        viewAnimationData.Initialize(elementForm.GetWidget(), viewAnimationStartView, viewAnimationEndView);

        // log interp
        // TOFIX this form displays incorrectly
        args.Clear();
        args.StdTopObjectAttach(elementForm.GetWidget());
        toggleForm.InitWidget(viewAnimationDialogForm.GetWidget(), args);

        args.Clear();
        args.TopFormAttach();
        args.LeftFormAttach();
        args.LeftOffset(5);
        logScaleIncUI.StdToggleInit("log Interp", toggleForm.GetWidget(), args);

        // bottom row buttons
        args.Clear();
        args.LeftFormAttach();
        args.RightFormAttach();
        args.BottomFormAttach();
        bottomRowForm.InitWidget(viewAnimationDialogForm.GetWidget(), args);

        // animate
        args.StdSplitSetup();
        args.StdSplitMiddle(2, 24);
        animateButton.StdButtonInit("Anim.", bottomRowForm.GetWidget(), args);
        animateButton.AddOtherObjectCallback(AnimateButtonCB, this);

        args.StdSplitMiddle(26, 49);
        gotoEndButton.StdButtonInit("2End", bottomRowForm.GetWidget(), args);
        gotoEndButton.AddOtherObjectCallback(GotoEndButtonCB, this);

        args.StdSplitMiddle(51, 74);
        setEndButton.StdButtonInit("SetEnd", bottomRowForm.GetWidget(), args);
        setEndButton.AddOtherObjectCallback(SetEndButtonCB, this);

        //  close
        args.StdSplitMiddle(76, 99);
        closeButton.StdButtonInit("Close", bottomRowForm.GetWidget(), args);
        closeButton.AddOtherObjectCallback(CloseButtonCB, this);

        // next row buttons
        args.Clear();
        args.LeftFormAttach();
        args.RightFormAttach();
        args.BottomBaseAttach(bottomRowForm);
        nextRowForm.InitWidget(viewAnimationDialogForm.GetWidget(), args);

        args.StdSplitSetup();
        args.StdSplitMiddle(2, 24);
        stepButton.StdButtonInit("Step", nextRowForm.GetWidget(), args);
        stepButton.AddOtherObjectCallback(StepButtonCB, this);

        args.StdSplitMiddle(26, 49);
        gotoStartButton.StdButtonInit("2Start", nextRowForm.GetWidget(), args);
        gotoStartButton.AddOtherObjectCallback(GotoStartButtonCB, this);

        args.StdSplitMiddle(51, 74);
        setStartButton.StdButtonInit("SetStart", nextRowForm.GetWidget(), args);
        setStartButton.AddOtherObjectCallback(SetStartButtonCB, this);

        args.StdSplitMiddle(76, 99);
        endToStartButton.StdButtonInit("End2Start", nextRowForm.GetWidget(), args);
        endToStartButton.AddOtherObjectCallback(EndToStartButtonCB, this);

        // # of frames
        args.Clear();
        args.LeftFormAttach();
        args.RightFormAttach();
        args.BottomBaseAttach(nextRowForm);
        numberOfIncUI.SetFieldLen(5);
        numberOfIncUI.InitLabeledUI("# of frames", viewAnimationDialogForm.GetWidget(), args);
        numberOfIncUI.SetFieldValue(10);


        viewAnimationStartView = base3DplotDef.GetCurrentView();
        viewAnimationEndView   = viewAnimationStartView;

        SetForm(&viewAnimationDialogForm);
        ResetForm();
    }

    Manage();

    viewAnimationData.SetFormats(base3DplotDef);
    viewAnimationData.UpdateFromViews();
}

bool  PS_ViewAnimationDialog::SetDeltaView()
{
    if (!viewAnimationData.UpdateToViews()) {
        return false;
    }
    bool allOK = true;
    if (!numberOfIncUI.FieldValueOK( allOK, "# of frames", 1, 5000))
        return false;

    double nframes = double(numberOfIncUI.GetFieldValue());

    logScaleInc = (viewAnimationEndView.scale > 1E-5) &&
                  (viewAnimationStartView.scale > 1E-5) &&
                  logScaleIncUI.GetValue();

    if (logScaleInc)
        viewAnimationDeltaView.scale =
            (log10(viewAnimationEndView.scale) - log10(viewAnimationStartView.scale)) / nframes;
    else
        viewAnimationDeltaView.scale =
            (viewAnimationEndView.scale - viewAnimationStartView.scale) / nframes;

    viewAnimationDeltaView.elevation            = (viewAnimationEndView.elevation               - viewAnimationStartView.elevation)            / nframes;
    viewAnimationDeltaView.azimuth              = (viewAnimationEndView.azimuth                 - viewAnimationStartView.azimuth)              / nframes;

    viewAnimationDeltaView.translation.cX       = (viewAnimationEndView.translation.cX          - viewAnimationStartView.translation.cX)       / nframes;
    viewAnimationDeltaView.translation.cY       = (viewAnimationEndView.translation.cY          - viewAnimationStartView.translation.cY)       / nframes;
    viewAnimationDeltaView.translation.cZ       = (viewAnimationEndView.translation.cZ          - viewAnimationStartView.translation.cZ)       / nframes;
    viewAnimationDeltaView.viewLimits.minLim.cX = (viewAnimationEndView.viewLimits.minLim.cX    - viewAnimationStartView.viewLimits.minLim.cX) / nframes;
    viewAnimationDeltaView.viewLimits.minLim.cY = (viewAnimationEndView.viewLimits.minLim.cY    - viewAnimationStartView.viewLimits.minLim.cY) / nframes;
    viewAnimationDeltaView.viewLimits.minLim.cZ = (viewAnimationEndView.viewLimits.minLim.cZ    - viewAnimationStartView.viewLimits.minLim.cZ) / nframes;
    viewAnimationDeltaView.viewLimits.maxLim.cX = (viewAnimationEndView.viewLimits.maxLim.cX    - viewAnimationStartView.viewLimits.maxLim.cX) / nframes;
    viewAnimationDeltaView.viewLimits.maxLim.cY = (viewAnimationEndView.viewLimits.maxLim.cY    - viewAnimationStartView.viewLimits.maxLim.cY) / nframes;
    viewAnimationDeltaView.viewLimits.maxLim.cZ = (viewAnimationEndView.viewLimits.maxLim.cZ    - viewAnimationStartView.viewLimits.maxLim.cZ) / nframes;

    return true;
}



void PS_ViewAnimationDialog::NextAnimStep()
{
    PC_View currView = base3DplotDef.GetCurrentView();

    if (logScaleInc)
        currView.scale  =  pow(10.0, log10(currView.scale) + viewAnimationDeltaView.scale);
    else
        currView.scale  += viewAnimationDeltaView.scale;

    currView.elevation              +=      viewAnimationDeltaView.elevation           ;
    // fix possible step errors by limiting to +/- 90
    if (currView.elevation > 90.0)
        currView.elevation = 90.0;
    if (currView.elevation < -90.0)
        currView.elevation = -90.0;

    currView.azimuth                +=      viewAnimationDeltaView.azimuth             ;
    // fix possible step errors by going to opposite
    if (currView.azimuth < -180.0)
        currView.azimuth = 360.0 + currView.azimuth;
    if (currView.azimuth > 180.0)
        currView.azimuth = -360.0 + currView.azimuth;

    currView.translation.cX         +=      viewAnimationDeltaView.translation.cX      ;
    currView.translation.cY         +=      viewAnimationDeltaView.translation.cY      ;
    currView.translation.cZ         +=      viewAnimationDeltaView.translation.cZ      ;
    currView.viewLimits.minLim.cX   +=      viewAnimationDeltaView.viewLimits.minLim.cX;
    currView.viewLimits.minLim.cY   +=      viewAnimationDeltaView.viewLimits.minLim.cY;
    currView.viewLimits.minLim.cZ   +=      viewAnimationDeltaView.viewLimits.minLim.cZ;
    currView.viewLimits.maxLim.cX   +=      viewAnimationDeltaView.viewLimits.maxLim.cX;
    currView.viewLimits.maxLim.cY   +=      viewAnimationDeltaView.viewLimits.maxLim.cY;
    currView.viewLimits.maxLim.cZ   +=      viewAnimationDeltaView.viewLimits.maxLim.cZ;
    base3DplotDef.ChangeView(currView);

    if (!animating)
        return;

    animationIndex++;
    animating = (animationIndex < nanimStep);

    if (!animating)
        SetAnimateText();
}

void PS_ViewAnimationDialog::DoStep()
{
    if (SetDeltaView())
        NextAnimStep();
}

void PS_ViewAnimationDialog::DumpViewToCout(const PC_View& outView)
{
/*
    cout << "El: " << outView.elevation << " Az: " << outView.azimuth << " Sc: " << outView.scale << endl
         << "T " << outView.translation.cX << " " << outView.translation.cY << " " << outView.translation.cZ << endl
         << "X " << outView.viewLimits.minLim.cX << " " << outView.viewLimits.maxLim.cX << endl
         << "Y " << outView.viewLimits.minLim.cY << " " << outView.viewLimits.maxLim.cY << endl
         << "Z " << outView.viewLimits.minLim.cZ << " " << outView.viewLimits.maxLim.cZ << endl;
*/
}

void PS_ViewAnimationDialog::DoAnimate()
{
    if (animating)
    {
        animating = false;
    }
    else
    {
        if (!SetDeltaView())
            return;

        nanimStep = numberOfIncUI.GetFieldValue();

        // kluge - dumps start and end views to cout
        if (base3DplotDef.tgaOutput.dumpMethod == PC_DumpTGA::dtga_FullAuto)
        {
            using namespace std;
            cout << "------------ " << base3DplotDef.tgaOutput.nextIncrement << " "
                 << base3DplotDef.tgaOutput.nextIncrement + nanimStep - 1 << endl;
            DumpViewToCout(viewAnimationStartView);
            DumpViewToCout(viewAnimationEndView);
        }

        base3DplotDef.ChangeView(viewAnimationStartView);

        animationIndex = 0;
        animating = true;
        SetAnimateText();

        m_bProcessing = true;
        try {
            do {
                NextAnimStep();
                // let's read button clicks now
                PumpMessages();
            } while (animating);
        } catch(...) {
            m_bProcessing = false;
            throw;
        }
        m_bProcessing = false;
    }
    SetAnimateText();
}

void PS_ViewAnimationDialog::SetAnimateText()
{
    animateButton.SetSensitive(true);
    if (animating)
        animateButton.SetButtonText("Stop");
    else
        animateButton.SetButtonText("Animate");


    stepButton.SetSensitive(!animating);
    gotoStartButton.SetSensitive(!animating);
    gotoEndButton.SetSensitive(!animating);
    setStartButton.SetSensitive(!animating);
    setEndButton.SetSensitive(!animating);
    endToStartButton.SetSensitive(!animating);
    closeButton.SetSensitive(!animating);
}


void PS_ViewAnimationDialog::DoGotoStart()
{
    if (SetDeltaView())
        base3DplotDef.ChangeView(viewAnimationStartView);
}
void PS_ViewAnimationDialog::DoGotoEnd()
{
    if (SetDeltaView())
        base3DplotDef.ChangeView(viewAnimationEndView);
}

void PS_ViewAnimationDialog::DoSetStart()
{
    viewAnimationStartView = base3DplotDef.GetCurrentView();
    viewAnimationData.SetFormats(base3DplotDef);
    viewAnimationData.UpdateFromViews();
}

void PS_ViewAnimationDialog::DoSetEnd()
{
    viewAnimationEndView = base3DplotDef.GetCurrentView();
    viewAnimationData.SetFormats(base3DplotDef);
    viewAnimationData.UpdateFromViews();
}

void PS_ViewAnimationDialog::DoEndToStart()
{
    viewAnimationStartView   = viewAnimationEndView;
    viewAnimationData.UpdateFromViews();
}

void PS_ViewAnimationDialog::DoClose()
{
    Unmanage();
}

BEGIN_MESSAGE_MAP(PS_ViewAnimationDialog, DialogShell)
    //{{AFX_MSG_MAP(PS_ViewAnimationDialog)
    ON_WM_SIZE()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



void PS_ViewAnimationDialog::OnSize(UINT nType, int cx, int cy)
{
    //DialogShellModeless::OnSize(nType, cx, cy);
    DialogShell::OnSize(nType, cx, cy);

    if (cx > 0 && cy > 0) {
        static int xDrift = (::GetSystemMetrics(SM_CXHSCROLL)-2)/2;
        static int yDrift = (::GetSystemMetrics(SM_CYHSCROLL)-2)/2;
        FormContainer::Resize(xDrift, 0, cx - xDrift, cy - yDrift);
    }
}


void PS_ViewAnimationDialog::OnClose()
{
    DoClose();
}

