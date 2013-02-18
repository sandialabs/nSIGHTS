///////////////////////////////////////////////////////////////////////////////////
//
// ExposedReal.cpp
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              Implements exposed object support for real-valued data.
//              Derived from ExposedObjC
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#ifndef ADCONSOLEAPP
#include <genApp/U_Menu.h>
#endif

#include <genClass/C_FuncObj.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genApp/AppConfigFile.h>


#include "ExposedReal.h"

ExposedReal :: ExposedReal(FuncObjC& FOref,
                           const char* mainT,
                           double&   realRef) : ExposedObjC(FOref, mainT), exposedReal(realRef)
{
#ifndef ADCONSOLEAPP

    ArgListC args;

    //  value
    args.StdFormAttach();
    startValUI.SetFieldLen(12);
    rValUI.InitFramedUI("Value", exposedObjectDialogForm.GetWidget(), args);
    rValUI.AddOtherObjectCallback(RValCB, this);
    rValUI.SetFieldValue(exposedReal);

    // log/lin widget
    args.StdTopObjectAttach(rValUI);
    logStepChooserUI.PreInitSetUp("Lin.", "Log.", true);
    logStepChooserUI.InitFramedUI("Step Type", exposedObjectDialogForm.GetWidget(), args);

    // frame for options
    args.StdTopObjectAttach(logStepChooserUI);
    optionFrame.InitFramedUI("Step Options", exposedObjectDialogForm.GetWidget(), args);

    // range widget
    args.StdFormAttach();
    args.LeftOffset(10);
    rangeSelectChooser.PreInitSetUp("Range", "Incr.", true);
    rangeSelectChooser.InitWidget(optionFrame.GetForm(), args);
    rangeSelectChooser.AddOtherObjectCallback(RangeSelectCB, this);

    // start field
    args.StdTopObjectAttach(rangeSelectChooser.GetWidget());
    startValUI.SetFieldLen(12);
    startValUI.InitLabeledUI("Start", optionFrame.GetForm(), args);
    startValUI.AddOtherObjectCallback(StartEndCB, this);

    // end field
    args.StdTopObjectAttach(startValUI);
    endValUI.SetFieldLen(12);
    endValUI.InitLabeledUI("End", optionFrame.GetForm(), args);
    endValUI.AddOtherObjectCallback(StartEndCB, this);

    // inc field
    args.StdTopObjectAttach(endValUI);
    incValUI.SetFieldLen(12);
    incValUI.InitLabeledUI("Inc", optionFrame.GetForm(), args);

    args.StdTopObjectAttach(optionFrame);
    sliderFrame.InitFramedUI("Value Adjust", exposedObjectDialogForm.GetWidget(), args);

    args.AllFormAttach();
    valueSliderUI.InitWidget(sliderFrame.GetForm(), args);
    valueSliderUI.AddOtherObjectCallback(SliderCB, this);

    args.StdTopObjectAttach(sliderFrame);
    sliderOptionFrame.InitFramedUI("Slider Options", exposedObjectDialogForm.GetWidget(), args);

    args.StdToggleSetup(5);
    trackSlider.StdToggleInit("track", sliderOptionFrame.GetForm(), args);

    // buttons
    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomFormAttach();
    bottomRowForm.InitWidget(exposedObjectDialogForm.GetWidget(), args);

    args.Clear();
    args.AddArg(Arg::XmNwidth, 80);
    args.BottomFormAttach(2);
    args.SetPlaceHolder();

    // animate
    args.LeftPosAttach(2);
    //TODO: Recompute size in X
    //animateButton.FixedSizeButtonArgs(args);
    animateButton.StdButtonInit(" Animate ", bottomRowForm.GetWidget(), args);
    animateButton.AddOtherObjectCallback(AnimateButtonCB, this);

    //  end to start
    args.ResetPlace();
    args.LeftPosAttach(50);
    setStartToEndButton.StdButtonInit(" End2St ", bottomRowForm.GetWidget(), args);
    setStartToEndButton.AddOtherObjectCallback(SetStartToEndButtonCB, this);

    // next row buttons
    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomBaseAttach(bottomRowForm);
    nextRowForm.InitWidget(exposedObjectDialogForm.GetWidget(), args);

    args.Clear();
    args.AddArg(Arg::XmNwidth, 80);
    args.BottomFormAttach(2);
    args.TopOffset(4);
    args.SetPlaceHolder();

    // step button
    args.LeftPosAttach(2);
    stepButton.StdButtonInit(" Step ", nextRowForm.GetWidget(), args);
    stepButton.AddOtherObjectCallback(StepButtonCB, this);

    //  to start
    args.ResetPlace();
    args.LeftPosAttach(50);
    resetStartButton.StdButtonInit(" ResetSt ", nextRowForm.GetWidget(), args);
    resetStartButton.AddOtherObjectCallback(ResetStartButtonCB, this);

    // and n
    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomBaseAttach(nextRowForm);
    numberOfIncUI.SetFieldLen(6);
    numberOfIncUI.InitFramedUI("# of steps", exposedObjectDialogForm.GetWidget(), args);

    //exposedObjectDialogForm.Manage();
    SetForm(&exposedObjectDialogForm);
    ResetForm();

    // set title
    Update();

    // set data
    startValUI.SetFieldValue(exposedReal);
    endValUI.SetFieldValue(exposedReal);
    incValUI.SetFieldValue(1.0);
    numberOfIncUI.SetFieldValue(10);
    trackSlider.SetValue(false);
    SetAllSensitive();
#endif
}

ExposedReal::~ExposedReal()
{
}

#ifndef ADCONSOLEAPP

void  ExposedReal::SetAllSensitive()
{
    rValUI.SetSensitive(!animating);
    logStepChooserUI.SetSensitive(!animating);

    optionFrame.SetSensitive(!animating);
    bool useIncr = rangeSelectChooser.GetBoolChoice();
    if (!animating)
        {
            endValUI.SetSensitive(!useIncr);
            incValUI.SetSensitive(useIncr);
        }

    numberOfIncUI.SetSensitive(!animating);

    setStartToEndButton.SetSensitive((!useIncr) && (!animating));
    resetStartButton.SetSensitive(!animating);
    stepButton.SetSensitive(!animating);
    bool sOK = SliderOK();
    sliderFrame.SetSensitive(sOK);
    sliderOptionFrame.SetSensitive(sOK);
}


bool ExposedReal::SliderOK()
{
    if (animating || rangeSelectChooser.GetBoolChoice())
        return false;

    bool ok = true;
    isLog = logStepChooserUI.GetBoolChoice();
    if (!startValUI.FieldValueOK(ok, "Start", false))
        return false;

    double sliderStart = startValUI.GetFieldValue();
    startValUI.SetFieldValue(sliderStart);

    if (isLog && (sliderStart < stdEps))
        return false;

    if (!endValUI.FieldValueOK(ok, "End", false))
        return false;

    double sliderEnd = endValUI.GetFieldValue();
    if (isLog && (sliderEnd < stdEps))
        return false;
    endValUI.SetFieldValue(sliderEnd);

    if (isLog)
        {
            sliderStart = log10(sliderStart);
            sliderEnd = log10(sliderEnd);
        }

    valueSliderUI.realMin = sliderStart;
    valueSliderUI.realMax = sliderEnd;
    return true;
}

void ExposedReal::DoStartEndCB()
{
    if (!SliderOK())
        return;

    double sVal = exposedReal;
    if (isLog && (sVal < stdEps))
        sVal = log10(sVal);

    if (Limit::WithinOneLimit(valueSliderUI.realMin, valueSliderUI.realMax, sVal))
        valueSliderUI.SetRealValue(sVal);

    SetAllSensitive();
}

void ExposedReal::StartEndCB(void* inObj)
{
    (static_cast<ExposedReal*>(inObj))->DoStartEndCB();
}

void ExposedReal::DoSliderCB()
{
    if (!SliderOK())
        return;
    double sVal = valueSliderUI.GetRealValue();
    if (isLog)
        sVal = InvLgt(sVal);

    rValUI.SetFieldValue(sVal);

    if (valueSliderUI.IsTracking() && (!trackSlider.GetValue()))
        return;

    if (fabs(sVal - exposedReal) <= fabs(exposedReal / 1.0E+08))
        return;

    exposedReal = sVal;
    Execute();
}

void ExposedReal::SliderCB(void* inObj)
{
    (static_cast<ExposedReal*>(inObj))->DoSliderCB();
}


void  ExposedReal::SetButtonStatus()
{
    SetAllSensitive();
    SetAnimateText();
}

void ExposedReal::DoRValCB()
{
    bool ok = true;
    if (!rValUI.FieldValueOK(ok, mainTitle))
        return;

    double tmp = rValUI.GetFieldValue();

    if (fabs(tmp - exposedReal) <= fabs(exposedReal / 1.0E+08))
        return;

    exposedReal = tmp;
    Execute();
    SetAllSensitive();

}

void ExposedReal::RValCB(void* inObj)
{
    (static_cast<ExposedReal*>(inObj))->DoRValCB();
}


void ExposedReal::DoRangeSelectCB()
{
    SetAllSensitive();
}
void ExposedReal::RangeSelectCB(void* inObj)
{
    (static_cast<ExposedReal*>(inObj))->DoRangeSelectCB();
}


bool ExposedReal::SetIncVal(bool forAnim)
{
    isLog = logStepChooserUI.GetBoolChoice();
    bool useIncr = rangeSelectChooser.GetBoolChoice();

    bool ok = true;
    if (!numberOfIncUI.FieldValueOK(ok, "# of steps", 1, 5000))
        return false;
    nStep = numberOfIncUI.GetFieldValue();

    if (forAnim || (!useIncr))
        {
            if (!startValUI.FieldValueOK(ok, "Start"))
                return false;
            startVal = startValUI.GetFieldValue();
            if (isLog && (startVal < stdEps))
                {
                    PropErrorMsg("Start must be > 0.0");
                    return false;
                }
        }

    if (useIncr)
        {
            if (!incValUI.FieldValueOK(ok, "Inc"))
                return false;
            incVal = incValUI.GetFieldValue();

            if (fabs(incVal) < stdEps)
                {
                    PropErrorMsg("abs(Inc) must be > 0.0");
                    return false;
                }

            if (isLog && (fabs(incVal) > 2.0))
                {
                    PropErrorMsg("abs(log Inc) must be < 2.0");
                    return false;
                }
        }
    else
        {
            if (!endValUI.FieldValueOK(ok, "End"))
                return false;
            double eVal = endValUI.GetFieldValue();

            if (isLog && (eVal < stdEps))
                {
                    PropErrorMsg("End must be > 0.0");
                    return false;
                }

            if (isLog)
                incVal = (log10(eVal) - log10(startVal));
            else
                incVal = (eVal - startVal);
            incVal /= double(nStep);

            if (fabs(incVal) < stdEps)
                {
                    PropErrorMsg("abs(end - start) must be > 0.0");
                    return false;
                }

        }
    return true;
}


void ExposedReal::DoAnimateButtonCB()
{
    // class specific
    if (!animating)
        {
            if (!SetIncVal(true))
                return;
            exposedReal = startVal;
        }

    StdAnimateCallback();
    SetButtonStatus();
}


void ExposedReal::NextAnimationStep()
{
    if (!animating)
        return;

    Execute();
    if (exposedFO.GetStatus() == os_OK)
        {
            if (isLog )
                exposedReal = pow(10.0, log10(exposedReal) + incVal);
            else
                exposedReal += incVal;
            animationIndex++;
            animating = (animationIndex <= nStep);
        }
    else
        animating = false;

    if (!animating)
        SetButtonStatus();
}

void ExposedReal::AnimateButtonCB(void* inObj)
{
    static_cast<ExposedReal*>(inObj)->DoAnimateButtonCB();
}

void ExposedReal::DoStepButtonCB()
{
    if (SetIncVal(false))
        {
            if (isLog )
                {
                    if (exposedReal < stdEps)
                        {
                            PropErrorMsg("Value must be > 0.0");
                            return;
                        }
                    exposedReal = pow(10.0, log10(exposedReal) + incVal);
                }
            else
                exposedReal += incVal;

            Execute();
        }
}

void ExposedReal::StepButtonCB(void* inObj)
{
    (static_cast<ExposedReal*>(inObj))->DoStepButtonCB();
}

void ExposedReal::DoResetStartButtonCB()
{
    bool ok = true;
    if (!startValUI.FieldValueOK(ok, "Start"))
        return;

    double tmp = startValUI.GetFieldValue();
    rValUI.SetFieldValue(tmp);
    DoRValCB();
}

void ExposedReal::ResetStartButtonCB(void* inObj)
{
    (static_cast<ExposedReal*>(inObj))->DoResetStartButtonCB();
}
void ExposedReal::DoSetStartToEndButtonCB()
{
    bool ok = true;
    if (!endValUI.FieldValueOK(ok, "End"))
        return;

    double tmp = endValUI.GetFieldValue();
    startValUI.SetFieldValue(tmp);
}

void ExposedReal::SetStartToEndButtonCB(void* inObj)
{
    (static_cast<ExposedReal*>(inObj))->DoSetStartToEndButtonCB();
}


void ExposedReal::Update()
{
    if (!IsExposed())
        return;

    ExposedObjC::Update();
    rValUI.SetFieldValue(exposedReal);

}

void ExposedReal::WriteUIReal(RealEntryUIC& valUIC)
{
    bool ok = true;
    double uiVal = 0.1;
    if (valUIC.FieldValueOK(ok, "UIV"))
        uiVal = valUIC.GetFieldValue();
    WriteDouble(uiVal);
}
#endif



void ExposedReal::WriteToFile()
{
    const int objMajVer = 1;  // added track slider
    //    const int objMajVer = 0;
    WriteObjectHeader(objMajVer);

#ifndef ADCONSOLEAPP
    WriteBool(logStepChooserUI.GetBoolChoice());
    WriteBool(rangeSelectChooser.GetBoolChoice());

    bool ok = true;
    int uinStep = 10;
    if (numberOfIncUI.FieldValueOK(ok, "UIn", 1, 5000))
        uinStep = numberOfIncUI.GetFieldValue();
    WriteInt(uinStep);

    WriteUIReal(startValUI);
    WriteUIReal(endValUI);
    WriteUIReal(incValUI);
    WriteBool(trackSlider.GetValue()); // added v1

#else
    // dummy writes

    WriteBool(false);
    WriteBool(false);
    WriteInt(0);
    WriteDouble(0.0);
    WriteDouble(0.0);
    WriteDouble(0.0);
    WriteBool(false);
#endif

    WriteLine();
}


bool ExposedReal::ReadFromFile()
{
    ReadObjectHeader();

#ifndef ADCONSOLEAPP

    logStepChooserUI.SetBoolChoice(ReadBool());
    rangeSelectChooser.SetBoolChoice(ReadBool());
    numberOfIncUI.SetFieldValue(ReadInt());
    startValUI.SetFieldValue(ReadDouble());
    endValUI.SetFieldValue(ReadDouble());
    incValUI.SetFieldValue(ReadDouble());
    if (exposedReadVersion > 0)
        trackSlider.SetValue(ReadBool());
#else
    // dummy reads
    ReadBool();
    ReadBool();
    ReadInt();
    ReadDouble();
    ReadDouble();
    ReadDouble();
    if (exposedReadVersion > 0)
        ReadBool();
#endif

    CheckForInputFailure("Reading ExposedReal");

#ifndef ADCONSOLEAPP
    DoStartEndCB();
    SetAllSensitive();
#endif

    return true;
}


void ExposedRealWrapper::CreateObject()
{
    if (exposedObj == 0)
        exposedObj = new ExposedReal(FOref, mainTitle, realRef);
}

