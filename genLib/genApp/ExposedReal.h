///////////////////////////////////////////////////////////////////////////////////
//
// ExposedReal.h
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

#ifndef C_EXPOSEDREAL_H
#define C_EXPOSEDREAL_H

#include <genDataClass/DSC_RealMSSpec.h>

#ifndef ADCONSOLEAPP
#include <genClassPS/PS_UIClass.h>
#endif

#include <genApp/C_ExposedObj.h>

class ExposedReal : public ExposedObjC {

#ifndef ADCONSOLEAPP
private:
    RealEntryUIC                rValUI;

    BoolRadioBoxUIC             logStepChooserUI;

    BoolRadioBoxChooserWidget   rangeSelectChooser;
    RealEntryUIC                incValUI;
    IntEntryUIC                 numberOfIncUI;

    //      forms & frames
    BaseUIC                     optionFrame;
    FormWidget                  bottomRowForm;
    FormWidget                  nextRowForm;

    PushButton                  stepButton;
    PushButton                  resetStartButton;
    PushButton                  setStartToEndButton;
#endif

    double&                     exposedReal;

    double                      startVal;
    double                      incVal;
    int                         nStep;
    bool                        isLog;

#ifndef ADCONSOLEAPP

protected:
    RealEntryUIC                startValUI;
    RealEntryUIC                endValUI;
    BaseUIC                     sliderFrame;
    SliderWidget                valueSliderUI;
    BaseUIC                     sliderOptionFrame;
    ToggleBoxWidget             trackSlider;
#endif

public:
    ExposedReal(FuncObjC& FOref,
                const char* mainT,
                double&   realRef);
    virtual                     ~ExposedReal();


#ifndef ADCONSOLEAPP
    //  C_ExposedObj virtual
    virtual void                Update();
#endif

    //  C_CfgFileOBJ virtual
    virtual void                WriteToFile();
    virtual bool                ReadFromFile();


#ifndef ADCONSOLEAPP
protected:
    //  animation virtual
    virtual void                NextAnimationStep();

private:
    //  for call back support
    static void                 RValCB(void* inObj);
    static void                 StartEndCB(void* inObj);
    static void                 SliderCB(void* inObj);
    static void                 RangeSelectCB(void* inObj);
    static void                 AnimateButtonCB(void* inObj);
    static void                 StepButtonCB(void* inObj);
    static void                 ResetStartButtonCB(void* inObj);
    static void                 SetStartToEndButtonCB(void* inObj);

    void                        DoRValCB();
    void                        DoStartEndCB();
    void                        DoSliderCB();
    void                        DoRangeSelectCB();
    void                        DoAnimateButtonCB();
    void                        DoStepButtonCB();
    void                        DoResetStartButtonCB();
    void                        DoSetStartToEndButtonCB();

    virtual void                SetAllSensitive();
    bool                        SetIncVal(bool forAnim);
    void                        SetButtonStatus();
    bool                                            SliderOK();

    void                        WriteUIReal(RealEntryUIC& valUIC);
#endif

};


class ExposedRealWrapper : public ExposedWrapper {
private:
    // used by create virtual
    FuncObjC&       FOref;
    const char* mainTitle;
    double&         realRef;

public:
    ExposedRealWrapper( FuncObjC&   inFORef,
                        const char* inTitle,
                        double&         inRealRef) :
        FOref(inFORef), mainTitle(inTitle),
        realRef(inRealRef) {}

    ExposedRealWrapper( FuncObjC&   inFORef,
                        DSC_RealMSSpec& iSpec) :
        FOref(inFORef), mainTitle(iSpec.valueID),
        realRef(iSpec.enteredRealValue) {}

    virtual                 ~ExposedRealWrapper() {};

    virtual void    CreateObject();
};

#endif // C_EXPOSEDREAL_H

