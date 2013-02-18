///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PenSetPS.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              User interface for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include <genClassPS/PS_Color.h>
#include <genApp/U_MenuDPO.h>

#include "DPO_PenSetPS.h"


// **************************************** slider stuff

void DPO_PenSetPS::SliderCB(void* inObj)
{
    DPO_PenSetPS* penSetPS = static_cast<DPO_PenSetPS*>(inObj);

    SC_ColorSpec& tmpSpec = penSetPS->penColors[penSetPS->currPenButton];
    tmpSpec.RH = penSetPS->redHueSlider.valSlider.GetRealValue();
    tmpSpec.GS = penSetPS->greenSatSlider.valSlider.GetRealValue();
    tmpSpec.BV = penSetPS->blueValSlider.valSlider.GetRealValue();
    if (penSetPS->useHSV.GetBoolChoice())
        tmpSpec.ToRGB();

    penSetPS->SetButtonColor(penSetPS->currPenButton);
}

void PenSetSliderUIC::InitLabeledUI(char             label[],
                                    Widget           parent,
                                    const ArgListC&  inArgs,
                                    DPO_PenSetPS*    penSetPS)
{
    attachPos = 15;
    rightOffset = 20;
    SetSliderVals(0.0, 1.0, 0, 1000, 100);
    EntrySliderUIC::InitLabeledUI(label, parent, inArgs);
    valSlider.AddOtherObjectCallback(DPO_PenSetPS::SliderCB, penSetPS);
    valDisplay.AddOtherObjectCallback(DPO_PenSetPS::SliderCB, penSetPS);
    valDisplay.SetFieldFormat(SC_DecFormat(3));
}


//  draw widget using colour
void DPO_PenSetPS::SetButtonColor(int buttonIndx)
{
    PenSetButton& penButton = penButtons[buttonIndx];
    penButton.SetWidgetColor(penColors[buttonIndx]);
    if (penButton.IsCreated()) {
        penButton.Invalidate();
        penButton.UpdateWindow();
    }
}

void DPO_PenSetPS::SetSliderVals()
{
    SC_ColorSpec tmpSpec = penColors[currPenButton];
    if (useHSV.GetBoolChoice())
        tmpSpec.ToHSV();
    redHueSlider.SetValues(tmpSpec.RH);
    greenSatSlider.SetValues(tmpSpec.GS);
    blueValSlider.SetValues(tmpSpec.BV);
}


void DPO_PenSetPS::SetRowLabels()
{
    // updates row labels based on HSV/RGB toggle
    BaseUIC* sliderLabels[3];
    sliderLabels[0] = &redHueSlider;
    sliderLabels[1] = &greenSatSlider;
    sliderLabels[2] = &blueValSlider;
    PropSetColorLabels(sliderLabels, useHSV.GetBoolChoice());
}


void PenSetButton::Select()
{
    if (buttonIndx == m_pMenu->currPenButton)
        return;

    m_pMenu->penButtons[m_pMenu->currPenButton].SelectButton(false);

    m_pMenu->currPenButton = buttonIndx;

    SelectButton(true);
    m_pMenu->SetSliderVals();
}

void  PenSetButton::SelectCB (void* inObj)
{
    static_cast<PenSetButton*>(inObj)->Select();
}

void PenSetButton::DoInit(int             indx,
                          LayoutManager&  parent,
                          ArgListC&       args)
{
    buttonIndx = indx;
    args.LeftOffset(5);
    InitWidget(parent, args);
    AddOtherObjectCallback(SelectCB, this);
    SelectButton(buttonIndx == m_pMenu->currPenButton);
}

CSize PenSetScrolledForm::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = LayoutManager::SetWindowPosX(0, 440);
    szForm.cy = LayoutManager::SetWindowPosY(0, 1000);
    return szForm;
}


DPO_PenSetPS::DPO_PenSetPS() :
    currPenButton(0)
{
}

void DPO_PenSetPS::UseHSVCB(void* inObj)
{
    static_cast<DPO_PenSetPS*>(inObj)->SetSliderVals();
    static_cast<DPO_PenSetPS*>(inObj)->SetRowLabels();
}


void DPO_PenSetPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    //  lower RGB data
    args.Clear();
    args.BottomFormAttach();
    args.LeftFormAttach();
    args.RightFormAttach();
    rgbForm.InitFormUI(PPform.GetWidget(), args);

    //  rgb chooser
    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftFormAttach();

    useHSV.SetTopAttach();
    useHSV.PreInitSetUp("Red/Green/Blue", "Hue/Sat/Val", false);
    useHSV.InitFramedUI("Pen Color Method", rgbForm.GetForm(), args);
    useHSV.AddOtherObjectCallback(UseHSVCB, this);

    //  lower RGB data
    args.ResetPlace();
    args.LeftUICAttach(useHSV);
    args.RightFormAttach();
    sliderForm.InitFramedUI("Pen Color Settings", rgbForm.GetForm(), args);

    args.Clear();
    args.RightFormAttach();
    args.LeftFormAttach();
    args.SetPlaceHolder();

    args.BottomFormAttach();
    blueValSlider.InitLabeledUI("Blue ", sliderForm.GetForm(), args, this);

    args.ResetPlace();
    args.BottomUICAttach(blueValSlider);
    greenSatSlider.InitLabeledUI("Green     ", sliderForm.GetForm(), args, this);

    args.ResetPlace();
    args.BottomUICAttach(greenSatSlider);
    redHueSlider.InitLabeledUI("Red", sliderForm.GetForm(), args, this);


    //  scrolled window in middle
    args.Clear();
    args.StdFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);
    args.BottomUICAttach(rgbForm);

    scrollArea.InitWidget(PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();

    for ( int i = 0; i < DC_PenSet::maxPenSet; i++)
        {
            penForms[i].InitWidget(scrollArea, args);

            //  set field widget
            args.Clear();
            args.TopFormAttach();
            args.BottomFormAttach();
            args.SetPlaceHolder();
            args.LeftFormAttach();
            args.RightPosAttach(30);
            penDescs[i].SetFieldLen(15);
            penDescs[i].InitWidget(penForms[i], args);

            args.ResetPlace();
            args.LeftWidgetAttach(penDescs[i].GetWidget());
            args.RightFormAttach();
            penButtons[i].m_pMenu = this;
            penButtons[i].DoInit(i, penForms[i], args);

            args.Clear();
            args.StdTopObjectAttach(penForms[i].GetWidget());
        }
    SetMainformScrolled(true, 480, 250);
}


void DPO_PenSetPS::SetMenuData(DPO_PenSet&  inData)
{
    for ( int i = 0; i < DC_PenSet::maxPenSet; i++)
        {
            penColors[i] = inData.penSet.penColor[i];
            SetButtonColor(i);
            penDescs[i].SetFieldString(inData.penSet.penLabel[i]);
        }

    useHSV.SetBoolChoice(inData.useHSVspec);
    SetRowLabels();
    SetSliderVals();

    MenuBaseC::SetMenuData(inData);
}

bool DPO_PenSetPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    return allOK;
}

void DPO_PenSetPS::GetMenuData(DPO_PenSet&  outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.useHSVspec  = useHSV.GetBoolChoice();
    for ( int i = 0; i < DC_PenSet::maxPenSet; i++)
        {
            outData.penSet.penColor[i]     = penColors[i];
            penDescs[i].GetFieldString(outData.penSet.penLabel[i], stdInputStrLen);
        }
}

void DPO_PenSetPS::MenuDefault()
{
    MenuBaseC::MenuDefault();
    useHSV.SetBoolChoice(true);
    DC_PenSet temp;
    for ( int i = 0; i < DC_PenSet::maxPenSet; i++)
        {
            penColors[i] = temp.GetColor(i);
            SetButtonColor(i);
            penDescs[i].SetFieldString(temp.GetLabel(i));
        }
    SetRowLabels();
    SetSliderVals();
}

