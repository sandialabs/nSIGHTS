///////////////////////////////////////////////////////////////////////////////////
//
// DPO_DerivativePS.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "nsLibAfx.h"

#include <nsClass/DO_TimeProcess.h>

#include <genClassPS/PS_UIBase.h>
#include <genApp/ExposedReal.h>

#include "DPO_Derivative.h"
#include "DPO_DerivativePS.h"

DPO_DerivativePS::DPO_DerivativePS() :
    inputTPObjRefUI(typeid(DO_XYData)),
    masterObjRefUI(typeid(DO_Derivative),false),
    timeProcessObjRefUI(typeid(DO_TimeProcess)),
    windowPctSpanMSUI(typeid(DO_Real), true),
    windowValSpanMSUI(typeid(DO_Real), true)
{
}


void DPO_DerivativePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_DerivativePS*>(inObj)->SetAllSensitive();
}

void DPO_DerivativePS::SetAllSensitive()
{
    bool isMaster     = masterObjRefUI.isMaster.GetValue();

    derivativeTypeUI.SetSensitive(isMaster);
    derivativeCalcTypeUI.SetSensitive(isMaster);
    //timeMultTypeUI.SetSensitive(isMaster);
    useSuperTimeUI.SetSensitive(isMaster);
    timeProcessUIC.SetSensitive(isMaster);
    optionFrame.SetSensitive(isMaster);

    int dctChoice = derivativeCalcTypeUI.GetChoice();
    windowCalcTypeUI.SetSensitive(isMaster && (dctChoice > 1));
    windowPtsSpanUI.SetSensitive(isMaster && ((dctChoice == 2) || (dctChoice == 1)));

    int dtChoice = derivativeTypeUI.GetChoice();

    logEpsUI.SetSensitive(isMaster && ((dctChoice < 2) || (dctChoice == 3) || (dctChoice == 5) ||  (dtChoice >0)));  
    offsetYUI.SetSensitive(isMaster && (dtChoice == 2));  

    bool isPct    =  isMaster && ((dctChoice  == 3) || (dctChoice  == 4));
    windowPctSpanMSUI.SetSensitive(isPct);
    if (isPct)
    {
        windowPctSpanMSUI.SetAllSensitive();
    }
    else
    {
        DPO_Derivative* currObj = static_cast<DPO_Derivative*>(MenuObjC::currMenuObj);
        currObj->exposedWindowPctSpanValue.CloseIfCreated();
        windowPctSpanMSUI.realValueMS.isExposed.SetValue(false);
    }
    bool isVal    =  isMaster && ((dctChoice  == 5) || (dctChoice  == 6));
    windowValSpanMSUI.SetSensitive(isVal);
    if (isVal)
    {
        windowValSpanMSUI.SetAllSensitive();
    }
    else
    {
        DPO_Derivative* currObj = static_cast<DPO_Derivative*>(MenuObjC::currMenuObj);
        currObj->exposedWindowValSpanValue.CloseIfCreated();
        windowValSpanMSUI.realValueMS.isExposed.SetValue(false);
    }


    timeProcessObjRefUI.SetSensitive(isMaster && useSuperTimeUI.GetValue());
}

void DPO_DerivativePS::ToggleExposedWindowPctSpanValue()
{
    SetAllSensitive();
    static_cast<DPO_Derivative*>(MenuObjC::currMenuObj)->
        exposedWindowPctSpanValue.ToggleExposed(windowPctSpanMSUI.TurnExposedOff());
}

void DPO_DerivativePS::ToggleExposedWindowPctSpanValueCB(void* inObj)
{
    static_cast<DPO_DerivativePS*>(inObj)->ToggleExposedWindowPctSpanValue();
}

void DPO_DerivativePS::ToggleExposedWindowValSpanValue()
{
    SetAllSensitive();
    static_cast<DPO_Derivative*>(MenuObjC::currMenuObj)->
        exposedWindowValSpanValue.ToggleExposed(windowValSpanMSUI.TurnExposedOff());
}

void DPO_DerivativePS::ToggleExposedWindowValSpanValueCB(void* inObj)
{
    static_cast<DPO_DerivativePS*>(inObj)->ToggleExposedWindowValSpanValue();
}

void DPO_DerivativePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputTPObjRefUI.InitFramedUI("Input P(t) Data", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    masterObjRefUI.horizontal = true;
    masterObjRefUI.InitFramedUI("Derivative Specification", PPform.GetWidget(), args);
    masterObjRefUI.isMaster.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(masterObjRefUI);
    selectForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    static const char* derivativeTypeLabels[]  = {"dY/dX", "dY/dlog10(X)", "dlog10(Y)/dlog10(X)", "dY/dln(X)"};
    derivativeTypeUI.PreInitSetUp(derivativeTypeLabels, 4);
    derivativeTypeUI.InitFramedUI("Derivative Type", selectForm.GetWidget(), args);
    derivativeTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(34, 66);
    static const char* derivativeCalcTypeLabels[]  = {"Simple", "2 Slope Average", "Window", 
                                                        "Log % Span", "Lin % Span", 
                                                        "Log Value Span", "Lin Value Span"};
    derivativeCalcTypeUI.PreInitSetUp(derivativeCalcTypeLabels, 7);
    derivativeCalcTypeUI.InitFramedUI("Derivative Calculation", selectForm.GetWidget(), args);
    derivativeCalcTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(33);
    static const char* windowCalcTypeLabels[]  = {"Linear", "Clark", "Simmons"};
    windowCalcTypeUI.PreInitSetUp(windowCalcTypeLabels, 3);
    windowCalcTypeUI.InitFramedUI("Windowed Calculation", selectForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(selectForm.GetWidget());
    valueForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    windowPtsSpanUI.SetFieldLen(4);
    windowPtsSpanUI.InitFramedUI("# points in Window", valueForm.GetWidget(), args);

    args.StdSplitMiddle(34, 66);
    logEpsUI.SetFieldLen(8);
    logEpsUI.SetFieldFormat(SC_SciFormat(3));
    logEpsUI.InitFramedUI("Log Epsilon", valueForm.GetWidget(), args);

    args.StdSplitFinish(33);
    offsetYUI.SetFieldLen(8);
    offsetYUI.SetFieldFormat(SC_SciFormat(3));
    offsetYUI.InitFramedUI("Y Offset", valueForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(valueForm.GetWidget());
    windowPctSpanMSUI.realValueUI.SetFieldLen(8);
    windowPctSpanMSUI.InitUI("Lin/Log % Span", args, PPform.GetWidget(), true, true, ToggleExposedWindowPctSpanValueCB, this);

    args.Clear();
    args.StdTopObjectAttach(windowPctSpanMSUI);
    windowValSpanMSUI.realValueUI.SetFieldLen(8);
    windowValSpanMSUI.InitUI("Lin/Log Value Span", args, PPform.GetWidget(), true, true, ToggleExposedWindowValSpanValueCB, this);

    args.Clear();
    args.StdTopObjectAttach(windowValSpanMSUI);
    //static const char* timeMultTypeLabels[]  = {"None", "Time", "delta T"};
    //timeMultTypeUI.PreInitSetUp(timeMultTypeLabels, 3);
    //timeMultTypeUI.InitFramedUI("Time Multiplier", PPform.GetWidget(), args);

    //args.Clear();
    //args.StdTopObjectAttach(timeMultTypeUI);
    timeProcessUIC.InitFramedUI("Time Processing", PPform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftPosAttach(5);
    useSuperTimeUI.StdToggleInit("Use superposition", timeProcessUIC.GetForm(), args);
    useSuperTimeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.ResetPlace();
    args.LeftPosAttach(60);
    args.RightFormAttach();
    timeProcessObjRefUI.InitFormUI(timeProcessUIC.GetForm(), args);


    args.Clear();
    args.StdTopObjectAttach(timeProcessUIC);
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.LeftPosAttach(5);
    absOutputYUI.StdToggleInit("ABS(derivative Y)", optionFrame.GetForm(), args);
}

void DPO_DerivativePS::MenuOpen(DPO_Derivative& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTPObjRefUI.StdMenuInit(inData.inputTPObjRef);
    masterObjRefUI.masterObjectList.StdMenuInit(inData.masterObjRef);
    timeProcessObjRefUI.StdMenuInit(inData.timeProcessObjRef);
    windowPctSpanMSUI.MenuInit(inData.windowPctSpanMS);
    windowValSpanMSUI.MenuInit(inData.windowValSpanMS);
}

void DPO_DerivativePS::SetMenuData(DPO_Derivative&  inData)
{
    inputTPObjRefUI.SetFuncObjRef(inData.inputTPObjRef);
    masterObjRefUI.SetMasterSlaveSettings(inData.isMaster, inData.masterObjRef);

    derivativeTypeUI.SetChoice(inData.derivativeDC.derivativeType);
    derivativeCalcTypeUI.SetChoice(inData.derivativeDC.derivativeCalcType);
    windowCalcTypeUI.SetChoice(inData.derivativeDC.windowCalcType);

    windowPtsSpanUI.SetFieldValue(inData.derivativeDC.windowPtsSpan);

    logEpsUI.SetFieldValue(inData.derivativeDC.logEps);
    offsetYUI.SetFieldValue(inData.derivativeDC.offsetY);

    //timeMultTypeUI.SetChoice(inData.derivativeDC.timeMultType);

    useSuperTimeUI.SetValue(inData.derivativeDC.useSuperTime);
    timeProcessObjRefUI.SetFuncObjRef(inData.timeProcessObjRef);

    windowPctSpanMSUI.SetValue(inData.windowPctSpanMS, inData.exposedWindowPctSpanValue);
    windowValSpanMSUI.SetValue(inData.windowValSpanMS, inData.exposedWindowValSpanValue);

    absOutputYUI.SetValue(inData.derivativeDC.absOutputY);

    MenuBaseC::SetMenuData(inData);

    SetAllSensitive();
}

bool DPO_DerivativePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    bool isMaster     = masterObjRefUI.isMaster.GetValue();

    inputTPObjRefUI.FuncObjRefOK(allOK, "P(t) input");
    masterObjRefUI.MasterSlaveOK(allOK, "input spec");

    int dctChoice = derivativeCalcTypeUI.GetChoice();
    if (isMaster)
    {
        if ((dctChoice == 2) || (dctChoice == 1))
            windowPtsSpanUI.FieldValueOK(allOK, "pts span", 1, 50);

        if ((dctChoice == 3) || (dctChoice == 4))
            windowPctSpanMSUI.UIdataOK(allOK);

        if ((dctChoice == 5) || (dctChoice == 6))
            windowValSpanMSUI.UIdataOK(allOK);

        int dtChoice = derivativeTypeUI.GetChoice();
        if ((dctChoice < 2) || (dctChoice == 3) || (dctChoice == 5) || (dtChoice >0))
            logEpsUI.FieldValueOK(allOK, "log Epsilon", 1.0E-35, 1.0E-05);

        if (dtChoice == 2)
            offsetYUI.FieldValueOK(allOK, "Y offset", 0.0, 1.0E-4);

        if (useSuperTimeUI.GetValue())
            timeProcessObjRefUI.FuncObjRefOK(allOK, "time process data source");
    }
    return allOK;
}

void DPO_DerivativePS::GetMenuData(DPO_Derivative&  outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.inputTPObjRef = inputTPObjRefUI.GetFuncObjRef();
    masterObjRefUI.GetMasterSlaveSettings(outData.isMaster, outData.masterObjRef);
    if (outData.isMaster)
    {
        outData.derivativeDC.derivativeType = DerivativeType(derivativeTypeUI.GetChoice());
        outData.derivativeDC.derivativeCalcType = DerivativeCalcType(derivativeCalcTypeUI.GetChoice());

        int dctChoice = derivativeCalcTypeUI.GetChoice();
        if (dctChoice > 1)
            outData.derivativeDC.windowCalcType = WindowCalcType(windowCalcTypeUI.GetChoice());

        if ((dctChoice == 2) || (dctChoice == 1))
            outData.derivativeDC.windowPtsSpan = windowPtsSpanUI.GetFieldValue();

        int dtChoice = derivativeTypeUI.GetChoice();
        if ((dctChoice < 2) || (dctChoice == 3) || (dctChoice == 5) || (dtChoice >0))
            outData.derivativeDC.logEps = logEpsUI.GetFieldValue();

        if (dtChoice == 2)
            outData.derivativeDC.offsetY = offsetYUI.GetFieldValue();

        //outData.derivativeDC.timeMultType = Derivative::TimeMultType(timeMultTypeUI.GetChoice());
        outData.derivativeDC.useSuperTime = useSuperTimeUI.GetValue();

        if (outData.derivativeDC.useSuperTime)
            outData.timeProcessObjRef = timeProcessObjRefUI.GetFuncObjRef();

        windowPctSpanMSUI.GetValue(outData.windowPctSpanMS);
        windowValSpanMSUI.GetValue(outData.windowValSpanMS);

        outData.derivativeDC.absOutputY = absOutputYUI.GetValue();
    }

}

