///////////////////////////////////////////////////////////////////////////////////
//
// PS_TransparencyDialog.cpp
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
//      Platform specific implementation of genPlotClass codes
//  adds Windows and OpenGL specific code.  Includes
//  operating system independent implementation of
//  2D and 3D OpenGL functionality (platform specific
//  in this case refers to the OpenGL renderer).
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
#include <genPlotClassPS/PS_TransparencyDialog.h>


void PS_TransparencyDialog::TransparencyGroupUIC::SetAllSensitive()
{
    bool isConstant = tranTypeUI.GetChoice() == 1;
    bool isVariable= tranTypeUI.GetChoice() == 2;
    constantTransFactorUI.SetSensitive(isConstant);
    startTransFactorUI.SetSensitive(isVariable);
    endTransFactorUI.SetSensitive(isVariable);
}

void PS_TransparencyDialog::TransparencyGroupUIC::SetAllSensitiveCB(void* clientData)
{
    static_cast<PS_TransparencyDialog::TransparencyGroupUIC*>(clientData)->SetAllSensitive();
}

void PS_TransparencyDialog::TransparencyGroupUIC::InitFramedUI(char         inLabel[],
                                                               Widget           parent,
                                                               const ArgListC&  inArgs)
{
    //  form
    BaseUIC::InitFramedUI(inLabel, parent, inArgs);

    //  label
    ArgListC args;

    args.StdSplitSetup(20);
    const char* tranTypeStrs[] = {"Opaque", "Constant", "Variable"};
    tranTypeUI.PreInitSetUp(tranTypeStrs, 3);
    tranTypeUI.InitWidget(GetForm(), args);
    tranTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(20, 45);
    constantTransFactorUI.AttachLabelToObject();
    constantTransFactorUI.SetFieldFormat(SC_DecFormat(3));
    constantTransFactorUI.SetFieldLen(5);
    constantTransFactorUI.InitLabeledUI("Constant", GetForm(), args);

    args.StdSplitMiddle(50, 75);
    startTransFactorUI.AttachLabelToObject();
    startTransFactorUI.SetFieldFormat(SC_DecFormat(3));
    startTransFactorUI.SetFieldLen(5);
    startTransFactorUI.InitLabeledUI("Start", GetForm(), args);

    args.StdSplitFinish(25);
    endTransFactorUI.AttachLabelToObject();
    endTransFactorUI.SetFieldFormat(SC_DecFormat(3));
    endTransFactorUI.SetFieldLen(5);
    endTransFactorUI.InitLabeledUI("End", GetForm(), args);
}


void PS_TransparencyDialog::TransparencyGroupUIC::SetGroup(const PC_TransparencySpec::TransparencyGroup& inGroup)
{
    int tranType = 0;
    if (inGroup.groupIsTransparent)
        if (inGroup.transparencyIsConstant)
            tranType = 1;
        else
            tranType = 2;

    tranTypeUI.SetChoice(tranType);
    constantTransFactorUI.SetFieldValue(inGroup.constantTransFactor);
    startTransFactorUI.SetFieldValue(inGroup.startTransFactor);
    endTransFactorUI.SetFieldValue(inGroup.endTransFactor);
    SetAllSensitive();
}

void PS_TransparencyDialog::TransparencyGroupUIC::GetGroup(PC_TransparencySpec::TransparencyGroup& outGroup)
{
    outGroup.groupIsTransparent = tranTypeUI.GetChoice() > 0;
    outGroup.transparencyIsConstant = tranTypeUI.GetChoice() == 1;

    if (outGroup.groupIsTransparent)
    {
        bool wasOK = true;

        if (outGroup.transparencyIsConstant )
        {
            if (constantTransFactorUI.FieldValueOK(wasOK, "Constant", 0.0, 1.0))
                outGroup.constantTransFactor = constantTransFactorUI.GetFieldValue();
        }
        else
        {
            if (startTransFactorUI.FieldValueOK(wasOK, "Start", 0.0, 1.0))
                outGroup.startTransFactor = startTransFactorUI.GetFieldValue();

            if (endTransFactorUI.FieldValueOK(wasOK, "Endrt", 0.0, 1.0))
                outGroup.endTransFactor = endTransFactorUI.GetFieldValue();
        }
    }
}


PS_TransparencyDialog::PS_TransparencyDialog(PD_3D& assPlot, PC_Platform3DPS&  assPlatform) :
    base3DplotDef(assPlot),
    base3DPlatform(assPlatform),
    transparencyDialogForm(this),
    DialogShell("::Transparency"),
    m_bProcessing(false)
{
    animating = false;
}

PS_TransparencyDialog::~PS_TransparencyDialog ()
{
}


void  PS_TransparencyDialog::AnimateButtonCB (void* clientData)
{
    static_cast<PS_TransparencyDialog*>(clientData)->DoAnimate();
}
void  PS_TransparencyDialog::StepButtonCB (void* clientData)
{
    static_cast<PS_TransparencyDialog*>(clientData)->DoStep();
}

void  PS_TransparencyDialog::StartButtonCB(void* clientData)
{
    static_cast<PS_TransparencyDialog*>(clientData)->DoStart();
}
void  PS_TransparencyDialog::EndButtonCB(void* clientData)
{
    static_cast<PS_TransparencyDialog*>(clientData)->DoEnd();
}
void  PS_TransparencyDialog::CloseButtonCB (void* clientData)
{
    static_cast<PS_TransparencyDialog*>(clientData)->DoClose();
}
void  PS_TransparencyDialog::ApplyButtonCB (void* clientData)
{
    static_cast<PS_TransparencyDialog*>(clientData)->DoApply();
}

void PS_TransparencyDialog::SetAllSensitive()
{
    bool doTransparency = useTransparencyUI.GetValue();
    for (int i = 0; i < PC_TransparencySpec::maxTransparencyGroup; i++)
    {
        transparencyGroupsUI[i].SetSensitive(doTransparency);
        if (doTransparency)
            transparencyGroupsUI[i].SetAllSensitive();
    }
    ntransStepsUI.SetSensitive(doTransparency);
    stepButton.SetSensitive(doTransparency);
    startButton.SetSensitive(doTransparency);
    endButton.SetSensitive(doTransparency);
    animateButton.SetSensitive(doTransparency);
}

void PS_TransparencyDialog::SetAllSensitiveCB(void* clientData)
{
    static_cast<PS_TransparencyDialog*>(clientData)->SetAllSensitive();
}



void PS_TransparencyDialog::DoCreateDialog(CWnd* pParentWnd)
{
    ArgListC args;
    CRect rect(0, 0, 350, 550);

    char plotName[80];
    base3DPlatform.GetMainWindowTitle(plotName, false, 80);
    ConcatString(plotName, "::Transparency", 80);

    CreateShell(pParentWnd, true, plotName, rect);
    CenterWindow();

    args.StdFormAttach();
    topRowForm.InitWidget(transparencyDialogForm.GetWidget(), args);

    args.StdToggleSetup(5);
    useTransparencyUI.StdToggleInit("Enable transparency", topRowForm.GetWidget(), args);
    useTransparencyUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    groupForm.InitWidget(transparencyDialogForm.GetWidget(), args);

    args.StdFormAttach();
    for (int i = 0; i < PC_TransparencySpec::maxTransparencyGroup; i++)
    {
        char tempStr[80];
        ConcatInt(i + 1, "Group ", tempStr, 1, 80);
        transparencyGroupsUI[i].InitFramedUI(tempStr, groupForm.GetWidget(), args);
        args.StdTopObjectAttach(transparencyGroupsUI[i]);
    }

    // bottom row buttons
    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomFormAttach();
    bottomRowForm.InitWidget(transparencyDialogForm.GetWidget(), args);

    // animate
    args.StdSplitMiddle(2, 30);
    animateButton.StdButtonInit("Anim.", bottomRowForm.GetWidget(), args);
    animateButton.AddOtherObjectCallback(AnimateButtonCB, this);

    args.StdSplitMiddle(32, 64);
    applyButton.StdButtonInit("Apply", bottomRowForm.GetWidget(), args);
    applyButton.AddOtherObjectCallback(ApplyButtonCB, this);

    //  close
    args.StdSplitMiddle(66, 99);
    closeButton.StdButtonInit("Close", bottomRowForm.GetWidget(), args);
    closeButton.AddOtherObjectCallback(CloseButtonCB, this);

    // next row buttons
    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomBaseAttach(bottomRowForm);
    nextRowForm.InitWidget(transparencyDialogForm.GetWidget(), args);

    args.StdSplitMiddle(2, 30);
    stepButton.StdButtonInit("Step", nextRowForm.GetWidget(), args);
    stepButton.AddOtherObjectCallback(StepButtonCB, this);

    args.StdSplitMiddle(32, 64);
    startButton.StdButtonInit("Start", nextRowForm.GetWidget(), args);
    startButton.AddOtherObjectCallback(StartButtonCB, this);

    args.StdSplitMiddle(66, 98);
    endButton.StdButtonInit("End", nextRowForm.GetWidget(), args);
    endButton.AddOtherObjectCallback(EndButtonCB, this);

    // # of frames
    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomBaseAttach(nextRowForm);
    ntransStepsUI.SetFieldLen(5);
    ntransStepsUI.InitLabeledUI("# of frames", transparencyDialogForm.GetWidget(), args);

    SetForm(&transparencyDialogForm);
    ResetForm();
}



void PS_TransparencyDialog::OpenTransparencyDialog(CWnd* pParentWnd)
{
    if (!IsCreated())
        DoCreateDialog(pParentWnd);

    Manage();

    UpdateUIFromDef();
}

void PS_TransparencyDialog::UpdateUIFromDef()
{
    PC_TransparencySpec& defSpec = base3DplotDef.transSpec;
    useTransparencyUI.SetValue(defSpec.useTransparency);
    for (int i = 0; i < PC_TransparencySpec::maxTransparencyGroup; i++)
        transparencyGroupsUI[i].SetGroup(defSpec.transparencyGroups[i]);
    ntransStepsUI.SetFieldValue(defSpec.ntransSteps);

    SetAllSensitive();
}

void PS_TransparencyDialog::UpdateDefFromUI()
{
    PC_TransparencySpec& defSpec = base3DplotDef.transSpec;
    defSpec.useTransparency = useTransparencyUI.GetValue();
    if (defSpec.useTransparency)
    {
        for (int i = 0; i < PC_TransparencySpec::maxTransparencyGroup; i++)
            transparencyGroupsUI[i].GetGroup(defSpec.transparencyGroups[i]);
        bool wasOK = true;
        if (ntransStepsUI.FieldValueOK(wasOK, "# of steps", 2, 1000))
            defSpec.ntransSteps = ntransStepsUI.GetFieldValue();
    }
    UpdateUIFromDef();
}


void PS_TransparencyDialog::NextAnimStep()
{
    if (!animating)
        return;

    base3DplotDef.transSpec.currTransStep++;
    base3DplotDef.GetMasterPlot().DrawPlot();
    animating = (base3DplotDef.transSpec.currTransStep < base3DplotDef.transSpec.ntransSteps);

    if (!animating)
        SetAnimateText();
}

void PS_TransparencyDialog::DoStep()
{
    UpdateDefFromUI();
    base3DplotDef.transSpec.currTransStep++;
    if (base3DplotDef.transSpec.currTransStep > base3DplotDef.transSpec.ntransSteps)
        base3DplotDef.transSpec.currTransStep = base3DplotDef.transSpec.ntransSteps;
    base3DplotDef.GetMasterPlot().DrawPlot();
}


void PS_TransparencyDialog::DoAnimate()
{
    if (animating)
    {
        animating = false;
    }
    else
    {
        UpdateDefFromUI();
        base3DplotDef.transSpec.currTransStep = 0;
        base3DplotDef.GetMasterPlot().DrawPlot();

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

void PS_TransparencyDialog::SetAnimateText()
{
    animateButton.SetSensitive(true);
    if (animating)
        animateButton.SetButtonText("Stop");
    else
        animateButton.SetButtonText("Animate");

    stepButton.SetSensitive(!animating);
    startButton.SetSensitive(!animating);
    endButton.SetSensitive(!animating);
    applyButton.SetSensitive(!animating);
    closeButton.SetSensitive(!animating);
}


void PS_TransparencyDialog::DoStart()
{
    UpdateDefFromUI();
    base3DplotDef.transSpec.currTransStep = 0;
    base3DplotDef.GetMasterPlot().DrawPlot();
}
void PS_TransparencyDialog::DoEnd()
{
    UpdateDefFromUI();
    base3DplotDef.transSpec.currTransStep = base3DplotDef.transSpec.ntransSteps;
    base3DplotDef.GetMasterPlot().DrawPlot();
}

void PS_TransparencyDialog::DoApply()
{
    UpdateDefFromUI();
    base3DplotDef.GetMasterPlot().DrawPlot();
}


void PS_TransparencyDialog::DoClose()
{
    UpdateDefFromUI();
    Unmanage();
}

BEGIN_MESSAGE_MAP(PS_TransparencyDialog, DialogShell)
    //{{AFX_MSG_MAP(PS_TransparencyDialog)
    ON_WM_SIZE()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



void PS_TransparencyDialog::OnSize(UINT nType, int cx, int cy)
{
    //DialogShellModeless::OnSize(nType, cx, cy);
    DialogShell::OnSize(nType, cx, cy);

    if (cx > 0 && cy > 0) {
        static int xDrift = (::GetSystemMetrics(SM_CXHSCROLL)-2)/2;
        static int yDrift = (::GetSystemMetrics(SM_CYHSCROLL)-2)/2;
        FormContainer::Resize(xDrift, 0, cx - xDrift, cy - yDrift);
    }
}


void PS_TransparencyDialog::OnClose()
{
    DoClose();
}


